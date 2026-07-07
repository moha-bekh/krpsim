#pragma once

#include "krpsim/Types.hpp"

#include <vector>

namespace krpsim {

std::vector<const Process*> getStartableProcesses(
    const Config& config,
    const SimulationState& state
);

SimulationResult solveNaive(const Config& config, Cycle maxCycle);

SimulationResult solveTargetPlan(const Config& config, Cycle maxCycle);

SimulationResult solveGreedyByScore(const Config& config, Cycle maxCycle);

bool isBetterResult(
    const Config& config,
    const SimulationResult& candidate,
    const SimulationResult& currentBest
);

SimulationResult solveBest(const Config& config, Cycle maxCycle);

SimulationResult solveBeamSearch(const Config& config, Cycle maxCycle);

} // namespace krpsim
