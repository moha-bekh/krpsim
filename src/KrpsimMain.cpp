#include <iostream>

#include "krpsim/Types.hpp"
#include "krpsim/Simulator.hpp"
#include "krpsim/Verifier.hpp"
#include "krpsim/Debug.hpp"
#include "krpsim/MockConfigs.hpp"

int main(int argc, char **argv) {

  if (argc == 1) {

    // WARNING: DEV mode witch mock configs

    const krpsim::Cycle maxCycle = 1000;

    // Single scenario mode:
    // krpsim::debug::runNaiveSolverDemoCase("simple", krpsim::mock::simpleDemo(), maxCycle);

    // Suite mode: comment or uncomment scenarios while testing.
    struct DemoCase {
      const char* name;
      krpsim::Config config;
    };

    std::vector<DemoCase> demos = {
      {"simple", krpsim::mock::simpleDemo()},
      {"ikea", krpsim::mock::ikeaDemo()},
      {"steak", krpsim::mock::steakDemo()},
      // {"recre", krpsim::mock::recreDemo()},
      // {"pomme", krpsim::mock::pommeDemo()},
      // {"inception", krpsim::mock::inceptionDemo()},
    };

    for (std::vector<DemoCase>::const_iterator it = demos.begin();
         it != demos.end();
         ++it) {
      krpsim::debug::runNaiveSolverDemoCase(it->name, it->config, maxCycle);
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
