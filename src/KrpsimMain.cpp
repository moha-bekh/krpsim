#include <iostream>

#include "krpsim/Debug.hpp"
#include "krpsim/MockConfigs.hpp"
#include "krpsim/Simulator.hpp"
#include "krpsim/Types.hpp"

int main(int argc, char **argv) {

  if (argc == 1) {

    // WARNING: DEV mode witch mock configs

    krpsim::Config config = krpsim::mock::simpleDemo();
    krpsim::debug::printConfig(config);

    krpsim::SimulationState state = krpsim::makeInitialState(config);
    krpsim::debug::printSimulationState(state);

    krpsim::Trace trace;

    while (true) {

      bool startedSomething = false;

      for (std::vector<krpsim::Process>::iterator processIt = config.processes.begin();
           processIt != config.processes.end(); ++processIt) {

        std::cout << "Can start: " << krpsim::canStartProcess(state, *processIt) << "\n";
        if (krpsim::canStartProcess(state, *processIt)) {

          krpsim::TraceAction action = {
            state.cycle,
            processIt->name
          };

          trace.push_back(action);

          krpsim::startProcess(state, *processIt);
          krpsim::debug::printSimulationState(state);

          startedSomething = true;
        }
      }

      if (startedSomething) {
        continue;
      }

      if (krpsim::hasPendingEvents(state) == false) {
        break ;
      }

      krpsim::Cycle nextCycle = krpsim::nextEventCycle(state);
      krpsim::completeEventsAtCycle(state, nextCycle);
      krpsim::debug::printSimulationState(state);
    }

    return 0;
  }

  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <file> <delay>\n";
    return 1;
  }

  std::cout << "krpsim skeleton: solver not implemented yet\n";
  return 0;
}
