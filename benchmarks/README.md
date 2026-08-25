# krpsim benchmarks

CodSpeed benchmark suite for the krpsim solvers, built with `google_benchmark`
via CodSpeed's fork. This is a separate CMake project living alongside the
Makefile build; it does not affect `make`, `make run`, or `make verif`.

## Layout

- `../CMakeLists.txt`: builds `krpsim_core`, a static library containing
  everything under `src/` except the two `main` entry points
  (`src/core`, `src/mocks`, `src/solver`, `src/parser`, `src/debug`).
- `CMakeLists.txt`: fetches `google_benchmark` (CodSpeed fork) and links it
  against `krpsim_core` to produce the `krpsim_bench` executable.
- `bench_parser.cpp`: parses every file in `resources/` with
  `krpsim::parseConfigFile`, isolating parsing/I/O cost from solving cost.
- `bench_solvers.cpp`: runs each solver against the in-code mock configs from
  `src/mocks/MockConfigs.cpp` (no file I/O) across a spread of `maxCycle`
  budgets.

## What's benchmarked and why

Two concerns are measured separately: parsing (`bench_parser.cpp`) and
solving (`bench_solvers.cpp`). Solving uses the mock configs directly instead
of `resources/` files so solver benchmarks aren't polluted by parser cost.

Solver coverage, one section per `solve*` function in `Solver.hpp`:

- **`solveNaive`, `solveTargetPlan`, `solveGreedyByScore`**: roughly linear in
  `maxCycle`, so each runs against all 6 mock configs (`simple`, `ikea`,
  `steak`, `recre`, `pomme`, `inception`) at two cycle budgets (`100`, `500`)
  to also catch regressions that only show up at scale.
- **`solveBeamSearch`**: the most expensive strategy — beam width is fixed at
  20, but the trace-length cap grows with `maxCycle * process count`, and cost
  compounds with config complexity. Cycle budgets are kept smaller (`50` and
  `100`/`200` depending on the config) after measuring that `pomme`/`inception`
  at `maxCycle=200` took 55-186ms natively, which would balloon under
  instrumentation. `pomme` and `inception` (the two heaviest configs) get the
  `100` tier instead of `200`.
- **`solveBest`**: runs all four other strategies internally, so it's already
  the sum of the above. One `maxCycle=100` benchmark per config is enough to
  track it without duplicating coverage.

Config selection favors diversity over exhaustiveness:

- `simple` / `ikea`: short linear/branching chains, cheap baseline.
- `steak`: a reusable blocking resource (`poele`) shared across processes.
- `recre` / `pomme`: larger production/economy loops.
- `inception`: a recursive, self-referential dependency graph (`clock` →
  `dream` loops) — the heaviest and most adversarial case, especially for
  `solveBeamSearch`.

## Local build

```sh
cmake -B build_bench -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build_bench -j
./build_bench/benchmarks/krpsim_bench
```

Filter to a subset with google_benchmark's standard flags, e.g.:

```sh
./build_bench/benchmarks/krpsim_bench --benchmark_filter=BeamSearch
```

### Running through CodSpeed locally

```sh
cmake -B build_bench -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCODSPEED_MODE=simulation
cmake --build build_bench -j
codspeed run -m simulation -- ./build_bench/benchmarks/krpsim_bench
```

Note: `codspeed run` requires the runner to detect the host OS version and
currently fails with `Error: Failed to get OS version` on rolling-release
distros without `VERSION_ID` in `/etc/os-release` (e.g. Arch Linux). The
instrumented binary still builds and runs fine directly
(`./build_bench/benchmarks/krpsim_bench`); this only affects the CodSpeed
wrapper locally. CI (`ubuntu-latest`) is unaffected.

## CI

`.github/workflows/codspeed.yml` builds with `-DCODSPEED_MODE=simulation` and
runs `krpsim_bench` through `CodSpeedHQ/action@v5` on every push to `main` and
on pull requests.

## Adding a benchmark

- New solver: add a small wrapper function in `bench_solvers.cpp` following
  the existing `BM_Solve*` pattern (loop body must stay inside
  `for (auto _ : state)`, wrap the result in `benchmark::DoNotOptimize`), then
  register it with `BENCHMARK_CAPTURE` for the configs/cycle budgets that
  matter.
- New resource file: add a matching `BENCHMARK_CAPTURE(BM_ParseConfig, ...)`
  line in `bench_parser.cpp`.
- New mock config: add `BENCHMARK_CAPTURE` entries per solver in
  `bench_solvers.cpp`; keep an eye on native run time before committing to a
  cycle budget — anything creeping past ~50ms natively is worth trimming down,
  since instrumentation multiplies the cost.
