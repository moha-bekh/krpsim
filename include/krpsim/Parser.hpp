#pragma once

#include "krpsim/Types.hpp"

#include <string>

namespace krpsim {

Config parseConfigFile(const std::string& path);

Trace parseTraceFile(const std::string& path);

} // namespace krpsim
