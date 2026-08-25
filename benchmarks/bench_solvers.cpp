#include "krpsim/MockConfigs.hpp"
#include "krpsim/Solver.hpp"
#include "krpsim/Types.hpp"

#include <benchmark/benchmark.h>

using krpsim::Config;
using krpsim::Cycle;
using krpsim::SimulationResult;

namespace {

// solveNaive / solveTargetPlan / solveGreedyByScore are near-linear in
// maxCycle, so they can afford a wider spread of cycle budgets.
void BM_SolveNaive(benchmark::State& state, Config config, Cycle maxCycle)
{
    for (auto _ : state) {
        SimulationResult result = krpsim::solveNaive(config, maxCycle);
        benchmark::DoNotOptimize(result);
    }
}

void BM_SolveTargetPlan(benchmark::State& state, Config config, Cycle maxCycle)
{
    for (auto _ : state) {
        SimulationResult result = krpsim::solveTargetPlan(config, maxCycle);
        benchmark::DoNotOptimize(result);
    }
}

void BM_SolveGreedyByScore(benchmark::State& state, Config config, Cycle maxCycle)
{
    for (auto _ : state) {
        SimulationResult result = krpsim::solveGreedyByScore(config, maxCycle);
        benchmark::DoNotOptimize(result);
    }
}

// solveBeamSearch expands up to `beamWidth` nodes per level and its trace
// cap grows with maxCycle, so cycle budgets are kept smaller here to bound
// CI runtime.
void BM_SolveBeamSearch(benchmark::State& state, Config config, Cycle maxCycle)
{
    for (auto _ : state) {
        SimulationResult result = krpsim::solveBeamSearch(config, maxCycle);
        benchmark::DoNotOptimize(result);
    }
}

// solveBest runs all four other strategies internally, so it is the most
// expensive entry point: one cycle budget per config is enough to track it.
void BM_SolveBest(benchmark::State& state, Config config, Cycle maxCycle)
{
    for (auto _ : state) {
        SimulationResult result = krpsim::solveBest(config, maxCycle);
        benchmark::DoNotOptimize(result);
    }
}

} // namespace

// --- solveNaive ---------------------------------------------------------
BENCHMARK_CAPTURE(BM_SolveNaive, simple_c100, krpsim::mock::simpleDemo(), 100);
BENCHMARK_CAPTURE(BM_SolveNaive, simple_c500, krpsim::mock::simpleDemo(), 500);
BENCHMARK_CAPTURE(BM_SolveNaive, ikea_c100, krpsim::mock::ikeaDemo(), 100);
BENCHMARK_CAPTURE(BM_SolveNaive, ikea_c500, krpsim::mock::ikeaDemo(), 500);
BENCHMARK_CAPTURE(BM_SolveNaive, steak_c100, krpsim::mock::steakDemo(), 100);
BENCHMARK_CAPTURE(BM_SolveNaive, steak_c500, krpsim::mock::steakDemo(), 500);
BENCHMARK_CAPTURE(BM_SolveNaive, recre_c100, krpsim::mock::recreDemo(), 100);
BENCHMARK_CAPTURE(BM_SolveNaive, recre_c500, krpsim::mock::recreDemo(), 500);
BENCHMARK_CAPTURE(BM_SolveNaive, pomme_c100, krpsim::mock::pommeDemo(), 100);
BENCHMARK_CAPTURE(BM_SolveNaive, pomme_c500, krpsim::mock::pommeDemo(), 500);
BENCHMARK_CAPTURE(BM_SolveNaive, inception_c100, krpsim::mock::inceptionDemo(), 100);
BENCHMARK_CAPTURE(BM_SolveNaive, inception_c500, krpsim::mock::inceptionDemo(), 500);

// --- solveTargetPlan -----------------------------------------------------
BENCHMARK_CAPTURE(BM_SolveTargetPlan, simple_c100, krpsim::mock::simpleDemo(), 100);
BENCHMARK_CAPTURE(BM_SolveTargetPlan, simple_c500, krpsim::mock::simpleDemo(), 500);
BENCHMARK_CAPTURE(BM_SolveTargetPlan, ikea_c100, krpsim::mock::ikeaDemo(), 100);
BENCHMARK_CAPTURE(BM_SolveTargetPlan, ikea_c500, krpsim::mock::ikeaDemo(), 500);
BENCHMARK_CAPTURE(BM_SolveTargetPlan, steak_c100, krpsim::mock::steakDemo(), 100);
BENCHMARK_CAPTURE(BM_SolveTargetPlan, steak_c500, krpsim::mock::steakDemo(), 500);
BENCHMARK_CAPTURE(BM_SolveTargetPlan, recre_c100, krpsim::mock::recreDemo(), 100);
BENCHMARK_CAPTURE(BM_SolveTargetPlan, recre_c500, krpsim::mock::recreDemo(), 500);
BENCHMARK_CAPTURE(BM_SolveTargetPlan, pomme_c100, krpsim::mock::pommeDemo(), 100);
BENCHMARK_CAPTURE(BM_SolveTargetPlan, pomme_c500, krpsim::mock::pommeDemo(), 500);
BENCHMARK_CAPTURE(BM_SolveTargetPlan, inception_c100, krpsim::mock::inceptionDemo(), 100);
BENCHMARK_CAPTURE(BM_SolveTargetPlan, inception_c500, krpsim::mock::inceptionDemo(), 500);

// --- solveGreedyByScore ---------------------------------------------------
BENCHMARK_CAPTURE(BM_SolveGreedyByScore, simple_c100, krpsim::mock::simpleDemo(), 100);
BENCHMARK_CAPTURE(BM_SolveGreedyByScore, simple_c500, krpsim::mock::simpleDemo(), 500);
BENCHMARK_CAPTURE(BM_SolveGreedyByScore, ikea_c100, krpsim::mock::ikeaDemo(), 100);
BENCHMARK_CAPTURE(BM_SolveGreedyByScore, ikea_c500, krpsim::mock::ikeaDemo(), 500);
BENCHMARK_CAPTURE(BM_SolveGreedyByScore, steak_c100, krpsim::mock::steakDemo(), 100);
BENCHMARK_CAPTURE(BM_SolveGreedyByScore, steak_c500, krpsim::mock::steakDemo(), 500);
BENCHMARK_CAPTURE(BM_SolveGreedyByScore, recre_c100, krpsim::mock::recreDemo(), 100);
BENCHMARK_CAPTURE(BM_SolveGreedyByScore, recre_c500, krpsim::mock::recreDemo(), 500);
BENCHMARK_CAPTURE(BM_SolveGreedyByScore, pomme_c100, krpsim::mock::pommeDemo(), 100);
BENCHMARK_CAPTURE(BM_SolveGreedyByScore, pomme_c500, krpsim::mock::pommeDemo(), 500);
BENCHMARK_CAPTURE(BM_SolveGreedyByScore, inception_c100, krpsim::mock::inceptionDemo(), 100);
BENCHMARK_CAPTURE(BM_SolveGreedyByScore, inception_c500, krpsim::mock::inceptionDemo(), 500);

// --- solveBeamSearch (smaller cycle budgets: combinatorial search) -------
BENCHMARK_CAPTURE(BM_SolveBeamSearch, simple_c50, krpsim::mock::simpleDemo(), 50);
BENCHMARK_CAPTURE(BM_SolveBeamSearch, simple_c200, krpsim::mock::simpleDemo(), 200);
BENCHMARK_CAPTURE(BM_SolveBeamSearch, ikea_c50, krpsim::mock::ikeaDemo(), 50);
BENCHMARK_CAPTURE(BM_SolveBeamSearch, ikea_c200, krpsim::mock::ikeaDemo(), 200);
BENCHMARK_CAPTURE(BM_SolveBeamSearch, steak_c50, krpsim::mock::steakDemo(), 50);
BENCHMARK_CAPTURE(BM_SolveBeamSearch, steak_c200, krpsim::mock::steakDemo(), 200);
BENCHMARK_CAPTURE(BM_SolveBeamSearch, recre_c50, krpsim::mock::recreDemo(), 50);
BENCHMARK_CAPTURE(BM_SolveBeamSearch, recre_c200, krpsim::mock::recreDemo(), 200);
BENCHMARK_CAPTURE(BM_SolveBeamSearch, pomme_c50, krpsim::mock::pommeDemo(), 50);
BENCHMARK_CAPTURE(BM_SolveBeamSearch, pomme_c100, krpsim::mock::pommeDemo(), 100);
BENCHMARK_CAPTURE(BM_SolveBeamSearch, inception_c50, krpsim::mock::inceptionDemo(), 50);
BENCHMARK_CAPTURE(BM_SolveBeamSearch, inception_c100, krpsim::mock::inceptionDemo(), 100);

// --- solveBest (runs all 4 strategies internally, one budget per config) -
BENCHMARK_CAPTURE(BM_SolveBest, simple_c100, krpsim::mock::simpleDemo(), 100);
BENCHMARK_CAPTURE(BM_SolveBest, ikea_c100, krpsim::mock::ikeaDemo(), 100);
BENCHMARK_CAPTURE(BM_SolveBest, steak_c100, krpsim::mock::steakDemo(), 100);
BENCHMARK_CAPTURE(BM_SolveBest, recre_c100, krpsim::mock::recreDemo(), 100);
BENCHMARK_CAPTURE(BM_SolveBest, pomme_c100, krpsim::mock::pommeDemo(), 100);
BENCHMARK_CAPTURE(BM_SolveBest, inception_c100, krpsim::mock::inceptionDemo(), 100);

BENCHMARK_MAIN();
