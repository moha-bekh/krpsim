# krpsim --- Research-Oriented Roadmap

## Philosophy

The goal is **not** to start with parsing.

Instead, build the project as if the configuration already existed in
memory (mocked `Config` objects). This keeps the whole team focused on
the real value of the project:

-   Event-driven simulation
-   Resource management
-   Scheduling
-   Optimization
-   Algorithms
-   Verification

The parser becomes a thin input layer added once the engine is stable.

------------------------------------------------------------------------

# Overall Architecture

``` text
        Config (mock)
              │
              ▼
      +----------------+
      |   Simulator    |
      +----------------+
         ▲         │
         │         ▼
      Solver ---> Trace
         │         │
         └-------> Verifier
```

The **Simulator** is the single source of truth.

-   Solver generates a trace.
-   Verifier replays a trace.
-   Parser only builds a Config.

------------------------------------------------------------------------

# Development Strategy

## Phase 0 --- Study the configuration format

Before writing any code, analyse every provided configuration.

### Use case progression

1.  **simple**

-   linear dependency chain
-   first valid simulation

2.  **ikea**

-   branching production
-   resource allocation
-   assembling final product

3.  **steak**

-   reusable tools
-   alternative production paths
-   scheduling decisions

4.  **recre**

-   empty outputs
-   profitable vs unprofitable actions
-   infinite production possibility

5.  **pomme**

-   industrial workflow
-   production + economy
-   cyclic systems

6.  **inception**

-   recursive production
-   acceleration
-   complex dependency graph

------------------------------------------------------------------------

# Important patterns

-   empty result list
-   reusable resources (tool returned later)
-   multiple processes at same cycle
-   delayed production
-   finite systems
-   infinite systems
-   multiple valid solutions
-   processes that are legal but strategically bad

------------------------------------------------------------------------

# Edge cases

Simulator must correctly handle:

-   empty outputs
-   zero remaining stock
-   several events finishing simultaneously
-   several launches at same cycle
-   resource returned at completion
-   impossible process
-   deadlock
-   infinite loop
-   duplicated resource names
-   optimize(time)
-   optimize(resource)
-   optimize(time + resource)

------------------------------------------------------------------------

# Phase 1 --- Core Engine (Pair Programming)

Both developers work together.

Deliverables:

-   Process
-   Config
-   State
-   Event
-   Trace
-   Simulator
-   Verifier

Goal:

A deterministic simulation engine that can replay any valid trace.

------------------------------------------------------------------------

# Phase 2 --- Research Mode

Instead of splitting the project by files, split it by algorithms.

Both developers already understand the engine.

Now each one explores different solving strategies.

## Developer A

Research:

-   Greedy
-   Priority Scheduling
-   Shortest Duration First
-   Weighted Score Scheduler

Questions:

-   Which process gives the highest long-term value?
-   Can a simple heuristic outperform more complex searches?

------------------------------------------------------------------------

## Developer B

Research:

-   Resource-aware scheduling
-   Randomized Greedy
-   Beam Search (optional)
-   DFS / Backtracking prototype
-   Alternative scoring models

Questions:

-   Is exploration better than exploitation?
-   Can randomness improve the final solution?

------------------------------------------------------------------------

# Phase 3 --- Benchmark

Every solver implements the same interface.

``` cpp
Trace solve(const Config&, int delay);
```

Create a benchmark runner:

``` text
Run Greedy
Run Priority
Run Random
Run DFS
Compare results
Keep best trace
```

Now the project becomes a small research project.

------------------------------------------------------------------------

# Phase 4 --- Swap

Each developer reviews and improves the other's strategies.

Objective:

-   understand every algorithm
-   compare implementations
-   merge best ideas

------------------------------------------------------------------------

# Phase 5 --- Parsing

Only after the engine is stable.

Developer A

-   Config parser

Developer B

-   Trace parser
-   CLI

Because parsing is now only:

``` text
file -> Config
```

instead of being coupled with the algorithm.

------------------------------------------------------------------------

# Suggested Solver Interface

``` cpp
class ISolver {
public:
    virtual Trace solve(const Config&, int maxDelay) = 0;
};
```

Possible implementations:

-   NaiveSolver
-   GreedySolver
-   PrioritySolver
-   ShortestJobSolver
-   RandomSolver
-   BeamSolver
-   BranchBoundSolver

------------------------------------------------------------------------

# Success Criteria

1.  Simulator is always correct.
2.  Verifier accepts every generated trace.
3.  Multiple independent solving strategies exist.
4.  Strategies can be benchmarked.
5.  Parsing is only an adapter layer.

------------------------------------------------------------------------

# Learning Outcomes

Simulation: - event queues - state machines - resource management

Algorithms: - greedy - priority queues - graph search - heuristics -
branch & bound - benchmarking

Software Design: - interfaces - dependency inversion - modular
architecture - testability

The project should feel closer to a research prototype than a single
monolithic implementation.
