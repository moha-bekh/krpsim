# krpsim

Temporary project README. The project is still in development, but the core
engine is already usable with mocked configurations.

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

## Current Status

Implemented:

- core data structures (`Config`, `Process`, `SimulationState`, `Event`,
  `Trace`);
- simulator primitives;
- trace verifier;
- debug printers;
- mocked configs based on the resource files;
- temporary naive demo runner.

Not implemented yet:

- real parser from config files;
- real solver strategies;
- final CLI behavior;
- benchmark runner.

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
- the parser will later only build a `Config`.

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

- `build/krpsim`
- `build/krpsim_verif`

Useful commands:

```sh
make run
make verif
make clean
make re
```

## Development Demo

Running without arguments starts the temporary development mode:

```sh
make run
```

This mode uses mocked configs from `src/mocks/MockConfigs.cpp`, runs a naive
solver demo, prints the generated trace, and verifies it.

The current demo list is in `src/KrpsimMain.cpp`:

```cpp
std::vector<DemoCase> demos = {
  {"simple", krpsim::mock::simpleDemo()},
  {"ikea", krpsim::mock::ikeaDemo()},
  {"steak", krpsim::mock::steakDemo()},
  // {"recre", krpsim::mock::recreDemo()},
  // {"pomme", krpsim::mock::pommeDemo()},
  // {"inception", krpsim::mock::inceptionDemo()},
};
```

Comment or uncomment scenarios to test them.

`recre`, `pomme`, and `inception` can contain cycles or long-running behavior,
so the debug runner uses a `maxCycle` limit.

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

## Next Steps

Short-term:

- sync engine work with the solver branch;
- move the temporary naive demo toward a real `solveNaive`;
- test simulator/verifier against `ikea`, `steak`, and `recre`;
- add parser once the engine is stable.

Longer-term:

- implement several solver strategies;
- compare generated traces;
- add benchmarks;
- wire the final CLI to parser + solver + verifier.
