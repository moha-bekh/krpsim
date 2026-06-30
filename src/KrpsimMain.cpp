#include <iostream>

#include "krpsim/Types.hpp"
#include "krpsim/Simulator.hpp"
#include "krpsim/Verifier.hpp"
#include "krpsim/Debug.hpp"
#include "krpsim/MockConfigs.hpp"

int main(int argc, char **argv) {

  if (argc == 1) {

    // WARNING: DEV mode witch mock configs

    krpsim::Config config = krpsim::mock::simpleDemo();
    // krpsim::debug::printConfig(config);

    krpsim::SimulationState state = krpsim::makeInitialState(config);
    // krpsim::debug::printSimulationState(state);

    krpsim::SimulationResult simulation = krpsim::debug::runNaiveSolverDemo(config);
    // krpsim::debug::printTrace(simulation.trace);

    krpsim::VerificationResult verification = verifyTrace(config, simulation.trace);
    krpsim::debug::printVerificationResult(verification);

    return 0;
  }

  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <file> <delay>\n";
    return 1;
  }

  std::cout << "krpsim skeleton: solver not implemented yet\n";
  return 0;
}
