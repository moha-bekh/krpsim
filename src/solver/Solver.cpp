#include "krpsim/Solver.hpp"

#include "krpsim/Simulator.hpp"

namespace krpsim {

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

} // namespace krpsim
