#include "krpsim/Simulator.hpp"

namespace krpsim {

SimulationState makeInitialState(const Config& config)
{
    SimulationState state;

    state.cycle = 0;
    state.stocks = config.initialStocks;
    return state;
}

bool canStartProcess(const SimulationState& state, const Process& process)
{
    for (const auto& need : process.needs) {
        const std::string& stockName = need.first;
        Quantity requiredQuantity = need.second;
        Quantity availableQuantity = 0;

        auto stock = state.stocks.find(stockName);
        if (stock != state.stocks.end()) {
            availableQuantity = stock->second;
        }
        if (availableQuantity < requiredQuantity) {
            return false;
        }
    }
    return true;
}

void startProcess(SimulationState& state, const Process& process)
{
    for (const auto& need : process.needs) {
        state.stocks[need.first] -= need.second;
    }

    state.runningEvents.push_back(Event{
        state.cycle + process.duration,
        process.name,
        process.results
    });
}

void completeEventsAtCycle(SimulationState& state, Cycle cycle)
{
    std::vector<Event> stillRunningEvents;

    for (const Event& event : state.runningEvents) {
        if (event.endCycle == cycle) {
            for (const auto& result : event.results) {
                state.stocks[result.first] += result.second;
            }
        } else {
            stillRunningEvents.push_back(event);
        }
    }

    state.runningEvents = stillRunningEvents;
}

bool hasPendingEvents(const SimulationState& state)
{
    return !state.runningEvents.empty();
}

Cycle nextEventCycle(const SimulationState& state)
{
    Cycle nextCycle = state.runningEvents.front().endCycle;

    for (const Event& event : state.runningEvents) {
        if (event.endCycle < nextCycle) {
            nextCycle = event.endCycle;
        }
    }
    return nextCycle;
}

} // namespace krpsim
