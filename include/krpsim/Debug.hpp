#pragma once

#include "krpsim/Types.hpp"

namespace krpsim::debug {

void printStockMap(const StockMap& stocks);
void printProcess(const Process& process);
void printConfig(const Config& config);
void printEvent(const Event& event);
void printTraceAction(const TraceAction& action);
void printTrace(const Trace& trace);
void printSimulationState(const SimulationState& state);
void printSimulationResult(const SimulationResult& result);
void printVerificationResult(const VerificationResult& result);

} // namespace krpsim::debug
