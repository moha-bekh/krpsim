#include "krpsim/Simulator.hpp"

#include <climits>
#include <stdexcept>

namespace krpsim {

SimulationState makeInitialState(const Config &config) {

  return SimulationState{0, config.initialStocks, {}};
}

bool canStartProcess(const SimulationState &state, const Process &process) {

  for (StockMap::const_iterator needsIt = process.needs.begin();
        needsIt != process.needs.end(); ++needsIt) {

    const std::string &resource = needsIt->first;
    Quantity procQuantity = needsIt->second;

    StockMap::const_iterator stockIt = state.stocks.find(resource);
    if (stockIt == state.stocks.end()) {
      return false;
    }

    Quantity stockQuantity = stockIt->second;
    if (stockQuantity < procQuantity) {
      return false;
    }
  }
  return true;
}

void startProcess(SimulationState& state, const Process& process) {

  if (canStartProcess(state, process) == false) {
    throw std::runtime_error("Cannot start process: " + process.name);
  }

  for (StockMap::const_iterator needsIt = process.needs.begin();
        needsIt != process.needs.end(); ++needsIt) {

    const std::string &resource = needsIt->first;
    const Quantity procQuantity = needsIt->second;

    StockMap::iterator stockIt = state.stocks.find(resource);
    const Quantity stockQuantity = stockIt->second;

    stockIt->second = stockQuantity - procQuantity;
  }

  Event newEvent = {
    state.cycle + process.duration,
    process.name,
    process.results
  };

  state.runningEvents.push_back(newEvent);
}

void completeEventsAtCycle(SimulationState& state, Cycle cycle) {

  state.cycle = cycle;

  for (std::vector<Event>::iterator eventIt = state.runningEvents.begin();
        eventIt != state.runningEvents.end();) {

    if (eventIt->endCycle == cycle) {

      for (StockMap::const_iterator resultIt = eventIt->results.begin();
           resultIt != eventIt->results.end(); ++resultIt) {

        const std::string resource = resultIt->first;
        const Quantity resultQuantity = resultIt->second;

        state.stocks[resource] += resultQuantity;
      }

      eventIt = state.runningEvents.erase(eventIt);

    } else {
      ++eventIt;
    }
  }

}

bool hasPendingEvents(const SimulationState& state) {
  return !state.runningEvents.empty();
}

Cycle nextEventCycle(const SimulationState& state) {

  Cycle nextCycle = LLONG_MAX;

  for (std::vector<Event>::const_iterator eventIt = state.runningEvents.begin();
       eventIt != state.runningEvents.end(); ++eventIt) {

    if (eventIt->endCycle < nextCycle) {
      nextCycle = eventIt->endCycle;
    }
  }

  return nextCycle;
}

} // namespace krpsim
