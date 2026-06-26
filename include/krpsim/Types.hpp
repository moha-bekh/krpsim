#pragma once

#include <map>
#include <string>
#include <vector>

namespace krpsim {

using Cycle = long long;
using Quantity = long long;
using StockMap = std::map<std::string, Quantity>;

struct Process {
    std::string name;
    StockMap needs;
    StockMap results;
    Cycle duration = 0;
};

struct Config {
    StockMap initialStocks;
    std::vector<Process> processes;
    bool optimizeTime = false;
    std::vector<std::string> optimizeResources;
};

struct Event {
    Cycle endCycle = 0;
    std::string processName;
    StockMap results;
};

struct TraceAction {
    Cycle cycle = 0;
    std::string processName;
};

using Trace = std::vector<TraceAction>;

struct SimulationState {
    Cycle cycle = 0;
    StockMap stocks;
    std::vector<Event> runningEvents;
};

struct SimulationResult {
    Trace trace;
    SimulationState finalState;
};

struct VerificationResult {
    bool ok = false;
    Cycle lastCycle = 0;
    StockMap finalStocks;
    std::string errorMessage;
};

} // namespace krpsim
