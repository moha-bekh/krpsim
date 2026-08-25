#include "krpsim/Parser.hpp"

#include <benchmark/benchmark.h>

#include <string>

namespace {

void BM_ParseConfig(benchmark::State& state, std::string filename)
{
    const std::string path = std::string(KRPSIM_RESOURCES_DIR) + "/" + filename;

    for (auto _ : state) {
        krpsim::Config config = krpsim::parseConfigFile(path);
        benchmark::DoNotOptimize(config);
    }
}

} // namespace

BENCHMARK_CAPTURE(BM_ParseConfig, simple, "simple");
BENCHMARK_CAPTURE(BM_ParseConfig, ikea, "ikea");
BENCHMARK_CAPTURE(BM_ParseConfig, steak, "steak");
BENCHMARK_CAPTURE(BM_ParseConfig, recre, "recre");
BENCHMARK_CAPTURE(BM_ParseConfig, pomme, "pomme");
BENCHMARK_CAPTURE(BM_ParseConfig, inception, "inception");
BENCHMARK_CAPTURE(BM_ParseConfig, custom_drone_trap, "custom_drone_trap");
BENCHMARK_CAPTURE(BM_ParseConfig, custom_greenhouse_loop, "custom_greenhouse_loop");
BENCHMARK_CAPTURE(BM_ParseConfig, custom_parallel_tools, "custom_parallel_tools");
BENCHMARK_CAPTURE(BM_ParseConfig, custom_robot_factory, "custom_robot_factory");
BENCHMARK_CAPTURE(BM_ParseConfig, custom_signal_ladder, "custom_signal_ladder");
