#include "krpsim/Parser.hpp"

#include <fstream>
#include <stdexcept>

namespace krpsim {
namespace {

std::string trim(const std::string& value)
{
    const std::string whitespace = " \t\r\n";
    const std::size_t begin = value.find_first_not_of(whitespace);

    if (begin == std::string::npos) {
        return "";
    }

    const std::size_t end = value.find_last_not_of(whitespace);
    return value.substr(begin, end - begin + 1);
}

long long parseNonNegativeNumber(const std::string& value, const std::string& context)
{
    std::size_t consumed = 0;
    long long number = 0;

    try {
        number = std::stoll(trim(value), &consumed);
    } catch (const std::exception&) {
        throw std::runtime_error("Invalid number in " + context + ": " + value);
    }

    if (consumed != trim(value).size() || number < 0) {
        throw std::runtime_error("Invalid number in " + context + ": " + value);
    }
    return number;
}

std::string parseParenthesizedGroup(const std::string& line, std::size_t& index)
{
    if (index >= line.size() || line[index] != '(') {
        throw std::runtime_error("Expected '(' in line: " + line);
    }

    const std::size_t groupBegin = index + 1;
    const std::size_t groupEnd = line.find(')', groupBegin);

    if (groupEnd == std::string::npos) {
        throw std::runtime_error("Expected ')' in line: " + line);
    }

    index = groupEnd + 1;
    return line.substr(groupBegin, groupEnd - groupBegin);
}

StockMap parseStockMap(const std::string& group, const std::string& context)
{
    StockMap stocks;
    std::size_t begin = 0;

    if (trim(group).empty()) {
        return stocks;
    }

    while (begin <= group.size()) {
        const std::size_t separator = group.find(';', begin);
        const std::string item = trim(group.substr(
            begin,
            separator == std::string::npos ? std::string::npos : separator - begin
        ));
        const std::size_t colon = item.find(':');

        if (colon == std::string::npos || colon == 0 || colon + 1 >= item.size()) {
            throw std::runtime_error("Invalid stock entry in " + context + ": " + item);
        }

        const std::string name = trim(item.substr(0, colon));
        const Quantity quantity = parseNonNegativeNumber(item.substr(colon + 1), context);

        stocks[name] += quantity;

        if (separator == std::string::npos) {
            break;
        }
        begin = separator + 1;
    }

    return stocks;
}

void parseOptimizeLine(Config& config, const std::string& line)
{
    std::size_t index = std::string("optimize:").size();
    const std::string group = parseParenthesizedGroup(line, index);
    std::size_t begin = 0;

    if (trim(line.substr(index)).empty() == false) {
        throw std::runtime_error("Unexpected characters after optimize line: " + line);
    }

    while (begin <= group.size()) {
        const std::size_t separator = group.find(';', begin);
        const std::string target = trim(group.substr(
            begin,
            separator == std::string::npos ? std::string::npos : separator - begin
        ));

        if (target.empty()) {
            throw std::runtime_error("Empty optimize target in line: " + line);
        }
        if (target == "time") {
            config.optimizeTime = true;
        } else {
            config.optimizeResources.push_back(target);
        }

        if (separator == std::string::npos) {
            break;
        }
        begin = separator + 1;
    }
}

Process parseProcessLine(const std::string& line)
{
    Process process;
    const std::size_t nameEnd = line.find(":(");

    if (nameEnd == std::string::npos || nameEnd == 0) {
        throw std::runtime_error("Invalid process line: " + line);
    }

    process.name = trim(line.substr(0, nameEnd));
    std::size_t index = nameEnd + 1;
    process.needs = parseStockMap(parseParenthesizedGroup(line, index), process.name + " needs");

    if (index >= line.size() || line[index] != ':') {
        throw std::runtime_error("Expected ':' after needs in line: " + line);
    }
    ++index;

    if (index < line.size() && line[index] == '(') {
        process.results = parseStockMap(
            parseParenthesizedGroup(line, index),
            process.name + " results"
        );
        if (index >= line.size() || line[index] != ':') {
            throw std::runtime_error("Expected ':' after results in line: " + line);
        }
        ++index;
    } else if (index < line.size() && line[index] == ':') {
        process.results = StockMap{};
        ++index;
    } else {
        throw std::runtime_error("Expected results or ':' in line: " + line);
    }

    process.duration = parseNonNegativeNumber(line.substr(index), process.name + " duration");
    return process;
}

void parseConfigLine(Config& config, const std::string& line)
{
    if (line.rfind("optimize:", 0) == 0) {
        parseOptimizeLine(config, line);
        return;
    }

    if (line.find(":(") != std::string::npos) {
        config.processes.push_back(parseProcessLine(line));
        return;
    }

    const std::size_t colon = line.find(':');
    if (colon == std::string::npos || colon == 0 || colon + 1 >= line.size()) {
        throw std::runtime_error("Invalid stock line: " + line);
    }

    const std::string name = trim(line.substr(0, colon));
    config.initialStocks[name] = parseNonNegativeNumber(line.substr(colon + 1), name);
}

} // namespace

Config parseConfigFile(const std::string& path)
{
    std::ifstream file(path);
    Config config;
    std::string line;

    if (!file) {
        throw std::runtime_error("Cannot open config file: " + path);
    }

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        parseConfigLine(config, line);
    }

    return config;
}

Trace parseTraceFile(const std::string& path)
{
    std::ifstream file(path);
    Trace trace;
    std::string line;

    if (!file) {
        throw std::runtime_error("Cannot open trace file: " + path);
    }

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        const std::size_t colon = line.find(':');
        if (colon == std::string::npos || colon == 0 || colon + 1 >= line.size()) {
            throw std::runtime_error("Invalid trace line: " + line);
        }

        trace.push_back(TraceAction{
            parseNonNegativeNumber(line.substr(0, colon), "trace cycle"),
            trim(line.substr(colon + 1))
        });
    }

    return trace;
}

} // namespace krpsim
