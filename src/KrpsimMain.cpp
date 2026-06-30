#include <iostream>
#include <stdexcept>
#include <string>

#include "krpsim/MockConfigs.hpp"
#include "krpsim/Simulator.hpp"
#include "krpsim/Solver.hpp"

static void printStocks(const krpsim::StockMap& stocks)
{
  for (const auto& stock : stocks) {
    std::cout << "- " << stock.first << ": " << stock.second << "\n";
  }
}

static void printStartableProcesses(
  const std::string& configName,
  const krpsim::Config& config
)
{
  krpsim::SimulationState state = krpsim::makeInitialState(config);
  std::vector<const krpsim::Process*> startableProcesses =
    krpsim::getStartableProcesses(config, state);

  std::cout << "Mock config: " << configName << "\n";
  std::cout << "Startable processes at cycle " << state.cycle << ":\n";
  for (const krpsim::Process* process : startableProcesses) {
    std::cout << "- " << process->name << "\n";
  }
  std::cout << "\n";
}

static void printStartProcessDemo()
{
  krpsim::Config config = krpsim::mock::ikeaDemo();
  krpsim::SimulationState state = krpsim::makeInitialState(config);
  std::vector<const krpsim::Process*> startableProcesses =
    krpsim::getStartableProcesses(config, state);

  if (startableProcesses.empty()) {
    return;
  }

  const krpsim::Process* process = startableProcesses.front();

  std::cout << "Start process demo on ikeaDemo\n";
  std::cout << "Before starting " << process->name << ":\n";
  printStocks(state.stocks);

  krpsim::startProcess(state, *process);

  std::cout << "After starting " << process->name << ":\n";
  printStocks(state.stocks);
  std::cout << "Running events:\n";
  for (const krpsim::Event& event : state.runningEvents) {
    std::cout << "- " << event.processName << " ends at cycle "
              << event.endCycle << "\n";
  }

  state.cycle = 15;
  krpsim::completeEventsAtCycle(state, state.cycle);

  std::cout << "After completing events at cycle " << state.cycle << ":\n";
  printStocks(state.stocks);
}

static void printTrace(const krpsim::Trace& trace)
{
  for (const krpsim::TraceAction& action : trace) {
    std::cout << action.cycle << ":" << action.processName << "\n";
  }
}

static void printSolveNaiveDemo()
{
  krpsim::SimulationResult result =
    krpsim::solveNaive(krpsim::mock::simpleDemo(), 100);

  std::cout << "\nNaive solver trace on simpleDemo:\n";
  printTrace(result.trace);
  std::cout << "Final stocks:\n";
  printStocks(result.finalState.stocks);
  std::cout << "Final cycle: " << result.finalState.cycle << "\n";
}

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
  return false;
}

static void printSimulationResult(const krpsim::SimulationResult& result)
{
  std::cout << "Trace:\n";
  printTrace(result.trace);
  std::cout << "Final stocks:\n";
  printStocks(result.finalState.stocks);
  std::cout << "Final cycle: " << result.finalState.cycle << "\n";
}

int main(int argc, char** argv)
{
  if (argc == 1) {
    std::cout << "Dev mode with mock configs\n\n";
    printStartableProcesses("simpleDemo", krpsim::mock::simpleDemo());
    printStartableProcesses("ikeaDemo", krpsim::mock::ikeaDemo());
    printStartProcessDemo();
    printSolveNaiveDemo();
    return 0;
  }

  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <mock_name> <delay>\n";
    std::cerr << "Available mocks: simple, ikea\n";
    return 1;
  }

  krpsim::Config config;
  if (!loadMockConfig(argv[1], config)) {
    std::cerr << "Unknown mock config: " << argv[1] << "\n";
    std::cerr << "Available mocks: simple, ikea\n";
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

  krpsim::SimulationResult result = krpsim::solveNaive(config, maxCycle);
  printSimulationResult(result);
  return 0;
}
