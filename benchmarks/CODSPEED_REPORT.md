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

## 7. Adding memory and walltime modes

Simulation mode answers "did the instruction count change" but says nothing
about actual memory behavior or real elapsed time. To get those two other
angles, the CI was extended (commit `42f8b5e`) with two more CodSpeed
modes alongside `simulation`:

- **`memory`** — deterministic allocation counting (peak bytes, total
  allocated/freed, alloc/free call counts). Runs on the same 4-shard,
  partial-run rules as `simulation`, since it's just as noise-free.
- **`walltime`** — real wall-clock time. CodSpeed recommends its own
  Hosted Macro Runners for stable walltime numbers, which this repo
  doesn't have (plain `ubuntu-latest`), so this mode is scoped down: only
  the two solver-heavy shards (`beam-search`, `best`), and only on push to
  `main` / manual `workflow_dispatch` — never gating a pull request.

## 8. What memory mode revealed

First memory-mode run on `main` (commit `42f8b5e`) showed something
that wasn't visible in simulation mode: **peak memory stays small and
flat, but total allocation churn explodes with `maxCycle`** — the exact
opposite of what "memory problem" usually means.

| Benchmark | Peak memory | Total allocated | Alloc calls | Free calls |
|---|---:|---:|---:|---:|
| `BM_SolveBeamSearch[inception_c50]` | 16.1 KB | 918 KB | 3,380 | 13,520 |
| `BM_SolveBeamSearch[inception_c100]` | 218.9 KB | **44.1 MB** | 70,867 | 283,468 |
| `BM_SolveBeamSearch[recre_c200]` | 199.4 KB | 25.3 MB | 264,156 | 1,056,333 |

Peak memory only grew ×13.6 between `inception_c50` and `inception_c100`,
but total bytes allocated grew ×48 — a single `inception_c100` run
allocates and frees 44 MB of small (~622 B average) objects to end up with
a 219 KB working set. That ratio is the signature of **allocation churn**,
not a growing footprint: the same small containers being repeatedly
cloned and thrown away rather than the beam's actual state growing.

`get_benchmark_result` with `runner_mode: Memory` was needed to see this —
`compare_runs`'s headline metric is peak bytes, which stays too small and
flat to show churn; the `allocCalls`/`totalAllocatedBytes` breakdown is
where it's visible.

Walltime confirmed the same cost hierarchy as simulation (`inception`,
`recre` far ahead of the rest), with low noise on the runs inspected —
e.g. `BM_SolveBeamSearch[inception_c100]` had 0 IQR/stdev outlier rounds
out of 5, stdev of 21 µs on a 9.6 ms median.

## 9. The second fix: cutting allocation churn

`SimulationState` (a `std::map<std::string, Quantity> stocks` plus a
`vector<Event>` where each `Event` owns its own `StockMap`) is exactly the
kind of type where a naive copy is expensive — every copy re-allocates a
full tree of map nodes. `solveBeamSearch`'s per-step loop
(`src/solver/Solver.cpp`) had three copies of it that weren't needed
(commit `614b1b3`):

- `for (BeamNode node : beam)` copied every beam entry just to run
  `completeEventsAtCycle` on it. Since `beam` is fully replaced right
  after the loop anyway (`beam = candidates`), mutating in place through
  a reference (`BeamNode& node`) is safe and copy-free.
- A full throwaway `SimulationResult` (state + trace copy) was built for
  every node just to feed `isBetterResultUsingWeights`, even though most
  candidates lose the comparison and get discarded immediately.
  `isBetterResultUsingWeights` was split into a `(state, trace)`-taking
  version used directly by the hot loop, with the original
  `SimulationResult`-taking signature kept as a thin wrapper — so
  `isBetterResult` (the public API in `Solver.hpp`) and its other caller
  are unaffected.
- `beam = candidates` at the end of each step copy-assigned the whole
  candidate vector instead of moving it (`beam = std::move(candidates)`).

**Functional non-regression check**: `krpsim` (which always runs
`solveBest`, exercising `solveBeamSearch` and the shared `isBetterResult`
path) produced **bit-for-bit identical** stdout/stderr on all 10 files in
`resources/` and all 6 mock configs, across `maxCycle` 50/100/200/500,
before vs after — 160 outputs compared, zero diffs.

## 10. What CodSpeed measured on this second change

`compare_runs` between the pre-fix run (`42f8b5e`) and the post-fix run
(`614b1b3`), across all three modes:

| Status | Count |
|---|---:|
| ⚡ Improvements | 34 |
| ❌ Regressions | 2 |
| ✅ Unchanged | 112 |

Breakdown of the most significant improvements:

| Benchmark | Mode | Before | After | Gain |
|---|---|---:|---:|---:|
| `BM_SolveBeamSearch[inception_c100]` | Simulation | 90.7 ms | 58.3 ms | **+55.4%** |
| `BM_SolveBest[inception_c100]` | WallTime | 12.1 ms | 7.6 ms | **+58.8%** |
| `BM_SolveBest[inception_c100]` | Simulation | 92.9 ms | 60.9 ms | +52.5% |
| `BM_SolveBeamSearch[steak_c50/c200]` | Simulation | ~2.5 ms | ~1.6 ms | +48.7% |
| `BM_SolveBeamSearch[recre_c200]` | Simulation | 105.4 ms | 78.7 ms | +34.0% |
| `BM_SolveBest[recre_c100]` | Simulation | 29.4 ms | 22.5 ms | +30.6% |
| `BM_SolveBeamSearch[inception_c100]` | WallTime | 9.6 ms | 7.5 ms | +28.2% |
| `BM_SolveBeamSearch[inception_c50]` | Memory | 16.1 KB | 11.7 KB | +37.0% |
| `BM_SolveBeamSearch[steak_c50/c200]` | Memory | 46.9 KB | 37.7 KB | +24.4% |

The two regressions — `BM_SolveBeamSearch[pomme_c50]` and `[pomme_c100]`
on WallTime, -28.9% and -27.6% — sit at 15 µs → ~21 µs, i.e. inside the
noise floor for wall-clock timing. `pomme`'s beam search converges and
stops before either cycle budget (see §2), so nothing in this fix touches
its actual code path. `compare_runs`'s own environment-differences report
lists both benchmarks under a CPU change between the two runs (`Intel
Xeon Platinum 8573C` → `AMD EPYC 9V74`) that affected essentially every
WallTime benchmark in this comparison — the same false-signal pattern
flagged in §5, this time affecting two microsecond-scale benchmarks
enough to flip their sign. Every other WallTime benchmark measured across
that same hardware change still shows a real improvement in the same
direction as simulation and memory, which is why these two are read as
noise rather than a regression from the fix.

On memory specifically: `inception_c100`'s peak-bytes number barely moved
(218.9 KB → 213.5 KB, under `compare_runs`'s significance threshold, hence
"unchanged"), but the churn numbers behind it did: total allocated bytes
44.1 MB → 29.4 MB (-33%), alloc calls 70,867 → 47,633 (-33%).
`recre_c200` similarly: 25.3 MB → 19.0 MB allocated (-25%), 264,156 →
196,650 alloc calls (-26%). Peak-bytes is the metric `compare_runs`
diffs, so this improvement was only visible by pulling the full
`allocCalls`/`totalAllocatedBytes` breakdown per-benchmark, same as in §8.

## 11. Current state

- 148 active benchmarks across 3 modes (simulation, memory, walltime),
  the last scoped to 2 of the 4 CI shards.
- No known regressions from either fix; the two WallTime deltas on
  `pomme` are attributed to a CI hardware change, not the code.
- `solveBeamSearch` on the recursive configs (`inception`, `recre`)
  remains the most expensive strategy, but combined across both rounds of
  fixes it's now roughly 2-3x faster than the very first measured run
  (`inception_c100`: 160.9 ms → 58.3 ms in simulation).
- The Makefile (`make`, `make run`, `make verif`) was never touched;
  the CMake/benchmarks build is entirely separate.

## 12. What this demonstrated about the tool

- **Simulation mode is stable enough to draw conclusions from a
  single run**: the non-linear scaling of `solveBeamSearch` on
  `inception`/`recre` was visible from the very first run, with no
  comparison history.
- **The flamegraph pointed directly at the culprit function** — no need
  to profile manually, the `query_flamegraph` query was enough to
  identify `isBetterResult`/`buildResourceWeights` as 47% of total
  time, which guided the fix in a matter of minutes.
- **`compare_runs` gives an immediately actionable diff** — improvements
  and regressions quantified benchmark by benchmark, even before opening
  a PR.
- **Environment difference detection avoids false positives** — a
  CPU change between two CI runs could have looked like noise
  or masked a real signal; CodSpeed isolated and attributed it precisely,
  twice (§5 and §10).
- **Sharding + partial runs work as documented**: a run
  pushed as 4 parallel jobs aggregates into a single report, and a PR that
  only touches part of the code can skip the unrelated shards without
  losing the history of those benchmarks.
- **Memory mode's headline metric (peak bytes) and its detailed breakdown
  (allocation churn) can tell different stories** — peak memory here
  stayed small and barely moved, while total allocated bytes and alloc
  counts dropped by a third. A fix can be real and significant without
  showing up in the metric `compare_runs` diffs by default.
