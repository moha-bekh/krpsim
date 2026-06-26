#pragma once

#include "krpsim/Types.hpp"

#include <vector>

namespace krpsim {

std::vector<const Process*> getStartableProcesses(
    const Config& config,
    const SimulationState& state
);

SimulationResult solveNaive(const Config& config, Cycle maxCycle);

} // namespace krpsim
