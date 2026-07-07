#include <iostream>
#include <stdexcept>
#include <string>

#include "krpsim/Debug.hpp"
#include "krpsim/MockConfigs.hpp"
#include "krpsim/Parser.hpp"
#include "krpsim/Solver.hpp"

static bool loadMockConfig(const std::string& name, krpsim::Config& config)
{
    if (name == "simple" || name == "simpleDemo") {
        config = krpsim::mock::simpleDemo();
        return true;
    }
    if (name == "ikea" || name == "ikeaDemo") {
        config = krpsim::mock::ikeaDemo();
        return true;
    }
    if (name == "steak" || name == "steakDemo") {
        config = krpsim::mock::steakDemo();
        return true;
    }
    if (name == "recre" || name == "recreDemo") {
        config = krpsim::mock::recreDemo();
        return true;
    }
    if (name == "pomme" || name == "pommeDemo") {
        config = krpsim::mock::pommeDemo();
        return true;
    }
    if (name == "inception" || name == "inceptionDemo") {
        config = krpsim::mock::inceptionDemo();
        return true;
    }
    return false;
}

static void printAvailableMocks()
{
    std::cerr << "Available mocks: simple, ikea, steak, recre, pomme, inception\n";
}

static krpsim::Config loadConfig(const std::string& input)
{
    krpsim::Config config;

    if (loadMockConfig(input, config)) {
        return config;
    }
    return krpsim::parseConfigFile(input);
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <file|mock_name> <delay>\n";
        printAvailableMocks();
        return 1;
    }

    krpsim::Config config;
    try {
        config = loadConfig(argv[1]);
    } catch (const std::exception& error) {
        std::cerr << error.what() << "\n";
        return 1;
    }

    krpsim::Cycle maxCycle = 0;
    try {
        maxCycle = std::stoll(argv[2]);
    } catch (const std::exception&) {
        std::cerr << "Invalid delay: " << argv[2] << "\n";
        return 1;
    }
    if (maxCycle < 0) {
        std::cerr << "Delay must be positive or zero\n";
        return 1;
    }

    krpsim::SimulationResult result = krpsim::solveBest(config, maxCycle);
    krpsim::debug::printSimulationResult(result);
    return 0;
}
