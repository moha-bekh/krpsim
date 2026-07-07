# krpsim verifier traces

These files are small fixtures for `krpsim_verif`.

Valid:

- `simple_valid.trace`: valid trace for `resources/simple`.

Invalid:

- `simple_unknown_process.trace`: references a process that does not exist.
- `simple_missing_resource.trace`: starts a process without required stock.
- `simple_cycle_backward.trace`: cycles go backward.
- `simple_too_early.trace`: starts a process before its dependency is produced.
- `simple_malformed.trace`: invalid trace line format.

Suggested checks:

```sh
./build/krpsim_verif resources/simple traces/simple_valid.trace
./build/krpsim_verif resources/simple traces/simple_unknown_process.trace
./build/krpsim_verif resources/simple traces/simple_missing_resource.trace
./build/krpsim_verif resources/simple traces/simple_cycle_backward.trace
./build/krpsim_verif resources/simple traces/simple_too_early.trace
./build/krpsim_verif resources/simple traces/simple_malformed.trace
```

