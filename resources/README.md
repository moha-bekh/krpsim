# krpsim resource set

Provided subject-like resources:

- `simple`: linear chain.
- `ikea`: exact resource allocation.
- `steak`: reusable blocking tool.
- `recre`: empty result and long-running production.
- `pomme`: larger economy workflow.
- `inception`: deep dependency graph.

Custom resources:

- `custom_robot_factory`: finite exact assembly, good target-plan demo, shuffled process order.
- `custom_drone_trap`: finite naive-trap, target-plan should beat naive.
- `custom_greenhouse_loop`: renewable long-running production loop.
- `custom_signal_ladder`: deep dependency chain for greedy scoring, reversed process order.
- `custom_parallel_tools`: reusable tools and parallel process scheduling, mixed process order.

Suggested checks:

```sh
./build/krpsim resources/custom_robot_factory 100
./build/krpsim resources/custom_drone_trap 100
./build/krpsim resources/custom_greenhouse_loop 200
./build/krpsim resources/custom_signal_ladder 100
./build/krpsim resources/custom_parallel_tools 100
```
