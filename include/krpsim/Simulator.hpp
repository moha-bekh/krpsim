#pragma once

#include "krpsim/Types.hpp"

namespace krpsim {

SimulationState makeInitialState(const Config& config);

bool canStartProcess(const SimulationState& state, const Process& process);

void startProcess(SimulationState& state, const Process& process);

void completeEventsAtCycle(SimulationState& state, Cycle cycle);

bool hasPendingEvents(const SimulationState& state);

Cycle nextEventCycle(const SimulationState& state);

} // namespace krpsim
