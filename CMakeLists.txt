# This CMake project exists only to build the CodSpeed benchmark suite.
# The Makefile remains the source of truth for building krpsim/krpsim_verif.
cmake_minimum_required(VERSION 3.14)
project(krpsim_bench_project VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

add_library(krpsim_core STATIC
    src/core/Simulator.cpp
    src/core/Verifier.cpp
    src/mocks/MockConfigs.cpp
    src/solver/Solver.cpp
    src/parser/Parser.cpp
    src/debug/Debug.cpp
)
target_include_directories(krpsim_core PUBLIC include)

option(KRPSIM_BUILD_BENCHMARKS "Build the CodSpeed benchmark suite" ON)
if(KRPSIM_BUILD_BENCHMARKS)
  add_subdirectory(benchmarks)
endif()
