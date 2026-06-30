#include "krpsim/Verifier.hpp"
#include "krpsim/Simulator.hpp"

namespace krpsim {

VerificationResult verifyTrace(const Config &config, const Trace &trace) {

  SimulationState state = makeInitialState(config);
  VerificationResult result = {false, 0, {}, ""};

  for (std::vector<TraceAction>::const_iterator traceIt = trace.begin();
       traceIt != trace.end(); ++traceIt) {

    if (traceIt->cycle < state.cycle) {
      result.errorMessage = "Trace cycle goes backward before process: " + traceIt->processName;
      result.lastCycle = state.cycle;
      result.finalStocks = state.stocks;
      return result;
    }

    while (hasPendingEvents(state) && nextEventCycle(state) <= traceIt->cycle) {
      completeEventsAtCycle(state, nextEventCycle(state));
    }

    state.cycle = traceIt->cycle;

    std::vector<Process>::const_iterator procIt = config.processes.begin();
    while (procIt != config.processes.end() && procIt->name != traceIt->processName) {
      ++procIt;
    }

    if (procIt == config.processes.end()) {
      result.errorMessage = "Process not found: " + traceIt->processName;
      result.lastCycle = state.cycle;
      result.finalStocks = state.stocks;
      return result;
    }

    if (canStartProcess(state, *procIt) == false) {
      result.errorMessage = "Cannot start process: " + traceIt->processName;
      result.lastCycle = state.cycle;
      result.finalStocks = state.stocks;
      return result;
    }

    startProcess(state, *procIt);
  }

  while (hasPendingEvents(state)) {
    completeEventsAtCycle(state, nextEventCycle(state));
  }

  result.ok = true;
  result.lastCycle = state.cycle;
  result.finalStocks = state.stocks;

  return result;
}

} // namespace krpsim
