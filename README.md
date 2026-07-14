# krpsim

Resource/process scheduling simulator for the 42 `krpsim` project.

## Goal

`krpsim` is a resource/process scheduling simulator.

A configuration describes:

- initial stocks;
- processes with required resources (`needs`);
- delayed results (`results`);
- process duration;
- optimization targets.

The engine simulates process execution over cycles. Resources are consumed when
a process starts, and results are produced only when the process finishes.

## Status

Implemented:

- core data structures (`Config`, `Process`, `SimulationState`, `Event`,
  `Trace`);
- simulator primitives;
- trace verifier;
- config parser for resource files;
- trace parser for verifier input;
- naive solver;
- target-plan solver for simple dependency chains;
- greedy score-based solver;
- beam-search solver;
- solver result comparison with `solveBest`;
- custom resource files and verifier traces for demonstrations.

## Architecture

```text
Config
  |
  v
Simulator <--- Solver
  |            |
  |            v
  |          Trace
  |            |
  v            v
State      Verifier
```

The simulator is the source of truth:

- a solver generates a `Trace`;
- the verifier replays a `Trace`;
- the parser builds a `Config` from a resource file.

## Core Rules

When a process starts:

1. all resources from `needs` are removed immediately from stocks;
2. no result is produced immediately;
3. an event is created at `current_cycle + duration`;
4. the solver records a trace action: `<cycle>:<process_name>`.

When an event completes:

1. all resources from `results` are added to stocks;
2. the event is removed from the running events.

Resources such as tools or workers are not special cases. If a process needs
`four:1` and returns `four:1`, the resource is simply unavailable during the
process duration.

## Build

```sh
make
```

This builds:

- `krpsim`
- `krpsim_verif`

Object and dependency files are kept in `build/`.

Useful commands:

```sh
make run
make verif
make clean
make re
```

## CLI

`krpsim` accepts a resource file and a maximum cycle:

```sh
./krpsim resources/simple 100
./krpsim resources/ikea 100
```

The generated trace is printed on stdout in the subject format:

```text
0:achat_materiel
10:realisation_produit
40:livraison
```

The solver name, final cycle and final stocks are printed on stderr so stdout
can be redirected directly into the verifier.

`krpsim_verif` accepts a resource file and a trace file:

```sh
./krpsim_verif resources/simple trace.txt
```

Verifier fixtures are available in `traces/`.

## Quick Run

Build and run a simple example:

```sh
make run
```

This runs:

```sh
./krpsim resources/simple 100
```

Running without arguments now prints usage and exits with an error, matching the
subject CLI expectation.

Mock configs still exist for development in `src/mocks/MockConfigs.cpp`:

```cpp
krpsim::mock::simpleDemo()
krpsim::mock::ikeaDemo()
krpsim::mock::steakDemo()
```

## Mock Configs

Available mock configs:

- `simpleDemo()`: linear chain, best first test;
- `ikeaDemo()`: branching production and assembly;
- `steakDemo()`: reusable blocking tool (`poele`);
- `recreDemo()`: empty results and potentially infinite production;
- `pommeDemo()`: larger economy/production loop;
- `inceptionDemo()`: complex recursive time-production system.

The recommended test order is:

```text
simple -> ikea -> steak -> recre -> pomme -> inception
```

## Parser

The config parser currently supports:

- comments starting with `#`;
- initial stocks with `name:quantity`;
- processes with `name:(needs):(results):duration`;
- empty result groups, for example `manger:(bonbon:1)::10`;
- `optimize:(time;resource)` targets.

The trace parser supports lines in the form:

```text
cycle:process_name
```

## Solvers

Current solver functions:

- `solveNaive`: launches the first startable process.
- `solveTargetPlan`: starts from the first optimized resource, builds a simple
  dependency plan, then executes the required processes.
- `solveGreedyByScore`: scores every startable process at each cycle using the
  optimized resources and their dependencies.
- `solveBeamSearch`: explores several promising traces and keeps the best beam
  states.
- `solveBest`: runs multiple solver strategies and keeps the best result using
  the configured `optimize` targets.

`SimulationResult` includes the selected solver name, printed on stderr by
`krpsim`.

Example: for `resources/ikea`, `solveTargetPlan` can build the exact component
plan needed for one `armoire`:

```text
0:do_montant
0:do_montant
0:do_fond
0:do_etagere
0:do_etagere
0:do_etagere
20:do_armoire_ikea
```

## Trace Verification

The verifier checks whether a trace is legal for a given config.

Example valid trace for `simple`:

```text
0:achat_materiel
10:realisation_produit
40:livraison
```

The verifier:

1. starts from the initial state;
2. advances events up to each trace action cycle;
3. checks that the requested process exists;
4. checks that the process can start;
5. starts the process;
6. completes remaining events at the end.

It returns a `VerificationResult` with:

- `ok`;
- `lastCycle`;
- `finalStocks`;
- `errorMessage`.

## Useful Final Checks

```sh
make fclean
make
./krpsim resources/simple 100 > /tmp/simple.trace
./krpsim_verif resources/simple /tmp/simple.trace
./krpsim resources/ikea 100 > /tmp/ikea.trace
./krpsim_verif resources/ikea /tmp/ikea.trace
```

Additional finite and infinite demonstration configs are in `resources/`.
Invalid verifier traces are in `traces/`.
