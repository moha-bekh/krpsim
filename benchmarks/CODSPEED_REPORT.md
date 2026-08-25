# CodSpeed on krpsim: what happened

This document tells the story of setting up CodSpeed benchmarking on krpsim,
analyzing the first run, the resulting optimization, and the current state.
Technical setup details (layout, how to add a benchmark, sharding):
see [`README.md`](./README.md).

## 1. Starting point

krpsim has 5 solving strategies (`solveNaive`, `solveTargetPlan`,
`solveGreedyByScore`, `solveBeamSearch`, `solveBest`) but no way to
compare their cost against each other, nor to know whether a future change
makes them slower. Goal: a `google_benchmark` suite (CodSpeed fork) covering
parsing and the 5 solvers on the 6 mock configs, run in CI in
`simulation` mode (deterministic instruction counting, near-zero noise).

## 2. What the first run showed

First CodSpeed run on `main` (commit `f55c174`, 65 benchmarks). The
raw numbers were already telling:

- **Parsing**: 77 to 260 µs depending on the file — negligible everywhere.
- **`solveNaive` / `solveTargetPlan` / `solveGreedyByScore`**: near-
  linear in `maxCycle` (25 µs to a few ms), nothing abnormal.
- **`solveBest`** matches exactly the sum of the other 4 strategies
  (e.g. on `inception_c100`: 163.2 ms measured vs 163.4 ms when adding
  up Naive + TargetPlan + Greedy + Beam) — no hidden structural overhead.
- **`solveBeamSearch`** was by far the most expensive, with very
  uneven scaling depending on the config:

  | Config | Low `maxCycle` | High `maxCycle` | Ratio |
  |---|---|---|---|
  | `ikea`, `pomme` | ~9.8ms / ~313µs | identical | ×1 (solver converges and stops before the limit) |
  | `simple`, `steak` | — | — | ×1.0–1.3 |
  | `recre` | 16 ms | 105.8 ms | **×6.6** |
  | `inception` | 7.1 ms | 160.9 ms | **×22.6** |

  `recre` (potentially infinite production) and especially `inception`
  (`dream` recursive loops) kept exploring well beyond the other
  configs — the signal that it was worth digging into.

## 3. The flamegraph found the real problem

Rooting the flamegraph of `BM_SolveBeamSearch[inception_c100]` (the most
expensive run), one item dominated by far:

```
isBetterResult → buildResourceWeights → markUsefulResource   = 47% of total time
```

`buildResourceWeights(config)` only depends on the config, not on the
simulation state — it's a pure function. Yet `isBetterResult` was
recomputing it entirely (a recursive traversal of the resource dependency
graph) at **every candidate comparison** in the beam search's inner loop,
even though `solveBeamSearch` already had these weights computed once
locally for `scoreBeamState` — they were simply never passed on to
`isBetterResult`.

## 4. The fix

The change (commit `2deecb9`):

- Extracted the comparison logic into a new internal helper
  `isBetterResultUsingWeights(config, candidate, currentBest, weights)`,
  which takes the weights as a parameter instead of recomputing them.
- `solveBeamSearch` calls this helper with the `weights` it already has
  locally (computed once at the top of the function).
- `isBetterResult` — the public API declared in `Solver.hpp`, used
  elsewhere (`solveBest`) — keeps exactly the same signature and the same
  behavior; it simply delegates to the helper after computing the
  weights (it's only called 3 times per run, so its cost is
  negligible).

**Functional non-regression check**: the traces produced by
`krpsim` on the 11 files in `resources/` remained **bit-for-bit
identical** before/after the fix — only the redundant computation
disappeared, no behavior changed.

## 5. What CodSpeed measured on this change

`compare_runs` between the baseline run (`f55c174`) and the post-fix
run (`9dd5193`):

| Status | Count |
|---|---:|
| ⚡ Improvements | 11 |
| ✅ Unchanged | 54 |
| Regressions | 0 |

Breakdown of the most significant improvements:

| Benchmark | Before | After | Gain |
|---|---:|---:|---:|
| `BM_SolveBeamSearch[inception_c100]` | 160.9 ms | 90.7 ms | **+77.5%** |
| `BM_SolveBest[inception_c100]` | 163.2 ms | 92.9 ms | **+75.7%** |
| `BM_SolveBeamSearch[pomme_c100/c50]` | 313.4 µs | 179.6 µs | +74.5% |
| `BM_SolveBeamSearch[simple_c50]` | 126.3 µs | 95.1 µs | +32.8% |
| `BM_SolveBest[ikea_c100]` | 10 ms | 8.6 ms | +16.9% |
| `BM_SolveBeamSearch[ikea_c50/c200]` | 9.8 ms | 8.5 ms | +15% |

The remaining 54 benchmarks (parsing, `solveNaive`, `solveTargetPlan`,
`solveGreedyByScore`) are unchanged, as expected: the fix only touches
the beam search's internal path.

**Notable point automatically detected by CodSpeed**: the report
flagged an environment difference between the two runs (CPU `AMD EPYC
9V74` on the first run vs `AMD EPYC 7763` on the second — two different
CI runners assigned by GitHub). CodSpeed precisely listed the
benchmarks affected by this hardware change (parsing and
`solveBest`) and explained how this can influence the estimate even
with identical source code, **without mixing it up** with the real
improvements measured on the beam search. This is exactly the kind of
false signal that plain local timing can't distinguish from a real
regression/improvement.

## 6. Setting up sharding + partial runs

To test these two CodSpeed features, the CI was restructured
(commits `238fad1`, `9dd5193`):

- A `changes` job detects (via `dorny/paths-filter`) whether the change
  touches parsing code, solving code, or both, and dynamically generates
  the list of shards to run.
- 4 shards run in parallel in the same workflow (`parser`,
  `cheap-solvers`, `beam-search`, `best`), each filtered via
  `--benchmark_filter` on the same `krpsim_bench` binary.
- On a pull request that only touches parsing (or only solving),
  only the relevant shards run — the others are automatically
  filled in by CodSpeed with results from the reference (baseline) run.
- On a push to `main` or a `workflow_dispatch`, all shards
  run systematically, to keep a complete baseline.

Run `9dd5193` confirms that the 4 shards aggregate correctly into a single
65-benchmark report on the CodSpeed side, despite running as 4 separate
jobs.

## 7. Current state

- 65 active benchmarks, spread across 4 parallel CI shards.
- No known regressions; beam search on the recursive configs
  (`inception`, `recre`) remains the most expensive strategy but has been
  reduced by ~35 to ~77% depending on the config.
- The Makefile (`make`, `make run`, `make verif`) was never touched;
  the CMake/benchmarks build is entirely separate.

## 8. What this demonstrated about the tool

- **Simulation mode is stable enough to draw conclusions from a
  single run**: the non-linear scaling of `solveBeamSearch` on
  `inception`/`recre` was visible from the very first run, with no
  comparison history.
- **The flamegraph pointed directly at the culprit function** — no need
  to profile manually, the `query_flamegraph` query was enough to
  identify `isBetterResult`/`buildResourceWeights` as 47% of total
  time, which guided the fix in a matter of minutes.
- **`compare_runs` gives an immediately actionable diff** — 11
  improvements, 0 regressions, quantified benchmark by benchmark, even
  before opening a PR.
- **Environment difference detection avoids false positives** — a
  CPU change between two CI runs could have looked like noise
  or masked a real signal; CodSpeed isolated and attributed it precisely.
- **Sharding + partial runs work as documented**: a run
  pushed as 4 parallel jobs aggregates into a single report, and a PR that
  only touches part of the code can skip the unrelated shards without
  losing the history of those benchmarks.
