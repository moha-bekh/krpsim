#include <iostream>

#include "krpsim/Debug.hpp"
#include "krpsim/MockConfigs.hpp"
#include "krpsim/Parser.hpp"
#include "krpsim/Verifier.hpp"

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
        std::cerr << "Usage: " << argv[0] << " <file> <result_to_test>\n";
        return 1;
    }

    try {
        const krpsim::Config config = loadConfig(argv[1]);
        const krpsim::Trace trace = krpsim::parseTraceFile(argv[2]);
        const krpsim::VerificationResult result = krpsim::verifyTrace(config, trace);

        krpsim::debug::printVerificationResult(result);
        return result.ok ? 0 : 1;
    } catch (const std::exception& error) {
        std::cerr << error.what() << "\n";
        return 1;
    }
}
