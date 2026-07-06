#include "krpsim/Debug.hpp"

#include "krpsim/MockConfigs.hpp"
#include "krpsim/Simulator.hpp"
#include "krpsim/Solver.hpp"
#include "krpsim/Verifier.hpp"

#include <iostream>

namespace krpsim::debug {

static const char* const SEPARATOR = "--------------------------------------------------\n";

void printStockMap(const StockMap& stocks) {
  if (stocks.empty()) {
    std::cout << "none";
    return;
  }

  for (StockMap::const_iterator it = stocks.begin(); it != stocks.end(); ++it) {
    if (it != stocks.begin()) {
      std::cout << "; ";
    }
    std::cout << it->first << ":" << it->second;
  }
}

void printProcess(const Process& process) {
  std::cout << "  " << process.name << "\n";

  std::cout << "    needs:   ";
  printStockMap(process.needs);
  std::cout << "\n";

  std::cout << "    results: ";
  printStockMap(process.results);
  std::cout << "\n";

  std::cout << "    delay:   " << process.duration << "\n";
}

void printConfig(const Config& config) {
  std::cout << "\nConfig\n";
  std::cout << SEPARATOR;

  std::cout << "Initial stocks:\n";
  std::cout << "  ";
  printStockMap(config.initialStocks);
  std::cout << "\n\n";

  std::cout << "Processes:\n";
  for (std::vector<Process>::const_iterator it = config.processes.begin();
       it != config.processes.end();
       ++it) {
    printProcess(*it);
    std::cout << "\n";
  }

  std::cout << "Optimize:\n";
  std::cout << "  time: " << (config.optimizeTime ? "yes" : "no") << "\n";

  std::cout << "  resources: ";
  if (config.optimizeResources.empty()) {
    std::cout << "none";
  } else {
    for (std::vector<std::string>::const_iterator it = config.optimizeResources.begin();
         it != config.optimizeResources.end();
         ++it) {
      if (it != config.optimizeResources.begin()) {
        std::cout << "; ";
      }
      std::cout << *it;
    }
  }
  std::cout << "\n";
}

void printEvent(const Event& event) {
  std::cout << "  " << event.processName << "\n";
  std::cout << "    end cycle: " << event.endCycle << "\n";
  std::cout << "    results:   ";
  printStockMap(event.results);
  std::cout << "\n";
}

void printTraceAction(const TraceAction& action) {
  std::cout << "  " << action.cycle << ":" << action.processName << "\n";
}

void printTrace(const Trace& trace) {
  std::cout << "\nTrace\n";
  std::cout << SEPARATOR;

  if (trace.empty()) {
    std::cout << "  none\n";
    return;
  }

  for (Trace::const_iterator it = trace.begin(); it != trace.end(); ++it) {
    printTraceAction(*it);
  }
}

void printSimulationState(const SimulationState& state) {
  std::cout << "\nSimulation state\n";
  std::cout << SEPARATOR;

  std::cout << "Cycle:\n";
  std::cout << "  " << state.cycle << "\n\n";

  std::cout << "Stocks:\n";
  std::cout << "  ";
  printStockMap(state.stocks);
  std::cout << "\n\n";

  std::cout << "Running events:\n";
  if (state.runningEvents.empty()) {
    std::cout << "  none\n";
    return;
  }

  for (std::vector<Event>::const_iterator it = state.runningEvents.begin();
       it != state.runningEvents.end();
       ++it) {
    printEvent(*it);
    if (it + 1 != state.runningEvents.end()) {
      std::cout << "\n";
    }
  }
}

void printSimulationResult(const SimulationResult& result) {
  std::cout << "\nSimulation result\n";
  std::cout << SEPARATOR;

  std::cout << "Solver:\n";
  if (result.solverName.empty()) {
    std::cout << "  unknown\n\n";
  } else {
    std::cout << "  " << result.solverName << "\n\n";
  }

  printTrace(result.trace);
  std::cout << "\n";
  printSimulationState(result.finalState);
}

void printVerificationResult(const VerificationResult& result) {
  std::cout << "\nVerification result\n";
  std::cout << SEPARATOR;

  std::cout << "Status:\n";
  std::cout << "  " << (result.ok ? "ok" : "error") << "\n\n";

  std::cout << "Last cycle:\n";
  std::cout << "  " << result.lastCycle << "\n\n";

  std::cout << "Final stocks:\n";
  std::cout << "  ";
  printStockMap(result.finalStocks);
  std::cout << "\n";

  if (!result.errorMessage.empty()) {
    std::cout << "\nError:\n";
    std::cout << "  " << result.errorMessage << "\n";
  }
}

SimulationResult runBestSolverDemo(const Config& config, Cycle maxCycle) {
  return solveBest(config, maxCycle);
}

void runBestSolverDemoCase(const std::string& name, const Config& config, Cycle maxCycle) {
  std::cout << "\n===== " << name << " =====\n";

  SimulationResult simulation = runBestSolverDemo(config, maxCycle);
  printSimulationResult(simulation);

  VerificationResult verification = verifyTrace(config, simulation.trace);
  printVerificationResult(verification);
}

} // namespace krpsim::debug
