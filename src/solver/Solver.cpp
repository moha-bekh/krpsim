#include "krpsim/Solver.hpp"

#include "krpsim/Simulator.hpp"

#include <map>
#include <set>
#include <stdexcept>

namespace krpsim {
namespace {

const Process* findProducerForResource(const Config& config, const std::string& resource)
{
    for (const Process& process : config.processes) {
        if (process.results.find(resource) != process.results.end()) {
            return &process;
        }
    }
    return nullptr;
}

Quantity getStockQuantity(const StockMap& stocks, const std::string& resource)
{
    StockMap::const_iterator stock = stocks.find(resource);

    if (stock == stocks.end()) {
        return 0;
    }
    return stock->second;
}

Quantity getProducedQuantity(const Process& process, const std::string& resource)
{
    StockMap::const_iterator result = process.results.find(resource);

    if (result == process.results.end()) {
        return 0;
    }
    return result->second;
}

bool ensureResource(
    const Config& config,
    StockMap& plannedStocks,
    std::map<std::string, Quantity>& processBudget,
    std::set<std::string>& visitingResources,
    const std::string& resource,
    Quantity quantity
);

bool planProcessLaunch(
    const Config& config,
    StockMap& plannedStocks,
    std::map<std::string, Quantity>& processBudget,
    std::set<std::string>& visitingResources,
    const Process& process,
    Quantity count
)
{
    for (Quantity i = 0; i < count; ++i) {
        for (const auto& need : process.needs) {
            if (!ensureResource(
                config,
                plannedStocks,
                processBudget,
                visitingResources,
                need.first,
                need.second
            )) {
                return false;
            }
        }

        ++processBudget[process.name];
        for (const auto& result : process.results) {
            plannedStocks[result.first] += result.second;
        }
    }
    return true;
}

bool ensureResource(
    const Config& config,
    StockMap& plannedStocks,
    std::map<std::string, Quantity>& processBudget,
    std::set<std::string>& visitingResources,
    const std::string& resource,
    Quantity quantity
)
{
    if (getStockQuantity(plannedStocks, resource) < quantity) {
        if (visitingResources.find(resource) != visitingResources.end()) {
            return false;
        }

        const Process* producer = findProducerForResource(config, resource);
        if (producer == nullptr) {
            return false;
        }

        const Quantity producedPerRun = getProducedQuantity(*producer, resource);
        if (producedPerRun <= 0) {
            return false;
        }

        const Quantity missing = quantity - getStockQuantity(plannedStocks, resource);
        const Quantity runCount = (missing + producedPerRun - 1) / producedPerRun;

        visitingResources.insert(resource);
        const bool planned = planProcessLaunch(
            config,
            plannedStocks,
            processBudget,
            visitingResources,
            *producer,
            runCount
        );
        visitingResources.erase(resource);

        if (!planned || getStockQuantity(plannedStocks, resource) < quantity) {
            return false;
        }
    }

    plannedStocks[resource] -= quantity;
    return true;
}

std::map<std::string, Quantity> buildTargetPlan(const Config& config)
{
    std::map<std::string, Quantity> processBudget;

    if (config.optimizeResources.empty()) {
        return processBudget;
    }

    const std::string& target = config.optimizeResources.front();
    const Process* producer = findProducerForResource(config, target);

    if (producer == nullptr) {
        return processBudget;
    }

    StockMap plannedStocks = config.initialStocks;
    std::set<std::string> visitingResources;

    if (!planProcessLaunch(config, plannedStocks, processBudget, visitingResources, *producer, 1)) {
        processBudget.clear();
    }
    return processBudget;
}

bool hasRemainingBudget(const std::map<std::string, Quantity>& processBudget)
{
    for (const auto& item : processBudget) {
        if (item.second > 0) {
            return true;
        }
    }
    return false;
}

const Process* choosePlannedStartableProcess(
    const Config& config,
    const SimulationState& state,
    const std::map<std::string, Quantity>& processBudget
)
{
    for (const Process& process : config.processes) {
        std::map<std::string, Quantity>::const_iterator budget = processBudget.find(process.name);
        if (budget != processBudget.end()
            && budget->second > 0
            && canStartProcess(state, process)) {
            return &process;
        }
    }
    return nullptr;
}

SimulationResult solveWithProcessBudget(
    const Config& config,
    Cycle maxCycle,
    std::map<std::string, Quantity> processBudget
)
{
    SimulationResult result;

    result.finalState = makeInitialState(config);
    while (result.finalState.cycle <= maxCycle) {
        completeEventsAtCycle(result.finalState, result.finalState.cycle);

        const Process* process = choosePlannedStartableProcess(
            config,
            result.finalState,
            processBudget
        );

        if (process != nullptr) {
            result.trace.push_back(TraceAction{
                result.finalState.cycle,
                process->name
            });
            startProcess(result.finalState, *process);
            --processBudget[process->name];
            continue;
        }

        if (!hasPendingEvents(result.finalState)) {
            break;
        }

        Cycle nextCycle = nextEventCycle(result.finalState);
        if (nextCycle > maxCycle) {
            break;
        }
        result.finalState.cycle = nextCycle;
    }

    if (hasRemainingBudget(processBudget)) {
        return solveNaive(config, maxCycle);
    }
    return result;
}

} // namespace

std::vector<const Process*> getStartableProcesses(
    const Config& config,
    const SimulationState& state
)
{
    std::vector<const Process*> startableProcesses;

    for (const Process& process : config.processes) {
        if (canStartProcess(state, process)) {
            startableProcesses.push_back(&process);
        }
    }

    return startableProcesses;
}

SimulationResult solveNaive(const Config& config, Cycle maxCycle)
{
    SimulationResult result;

    result.solverName = "solveNaive";
    result.finalState = makeInitialState(config);
    while (result.finalState.cycle <= maxCycle) {
        completeEventsAtCycle(result.finalState, result.finalState.cycle);

        std::vector<const Process*> startableProcesses =
            getStartableProcesses(config, result.finalState);

        if (!startableProcesses.empty()) {
            const Process* process = startableProcesses.front();

            result.trace.push_back(TraceAction{
                result.finalState.cycle,
                process->name
            });
            startProcess(result.finalState, *process);
            continue;
        }

        if (!hasPendingEvents(result.finalState)) {
            break;
        }

        Cycle nextCycle = nextEventCycle(result.finalState);
        if (nextCycle > maxCycle) {
            break;
        }
        result.finalState.cycle = nextCycle;
    }
    return result;
}

SimulationResult solveTargetPlan(const Config& config, Cycle maxCycle)
{
    std::map<std::string, Quantity> processBudget = buildTargetPlan(config);

    if (processBudget.empty()) {
        return solveNaive(config, maxCycle);
    }
    SimulationResult result = solveWithProcessBudget(config, maxCycle, processBudget);
    result.solverName = "solveTargetPlan";
    return result;
}

bool isBetterResult(
    const Config& config,
    const SimulationResult& candidate,
    const SimulationResult& currentBest
)
{
    for (const std::string& resource : config.optimizeResources) {
        const Quantity candidateQuantity = getStockQuantity(candidate.finalState.stocks, resource);
        const Quantity bestQuantity = getStockQuantity(currentBest.finalState.stocks, resource);

        if (candidateQuantity != bestQuantity) {
            return candidateQuantity > bestQuantity;
        }
    }

    if (config.optimizeTime && candidate.finalState.cycle != currentBest.finalState.cycle) {
        return candidate.finalState.cycle < currentBest.finalState.cycle;
    }
    return candidate.trace.size() < currentBest.trace.size();
}

SimulationResult solveBest(const Config& config, Cycle maxCycle)
{
    SimulationResult best = solveNaive(config, maxCycle);
    SimulationResult targetPlan = solveTargetPlan(config, maxCycle);

    if (isBetterResult(config, targetPlan, best)) {
        best = targetPlan;
    }
    best.solverName = "solveBest -> " + best.solverName;
    return best;
}

} // namespace krpsim
