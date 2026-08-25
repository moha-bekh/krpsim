include(FetchContent)
FetchContent_Declare(
  google_benchmark
  GIT_REPOSITORY https://github.com/CodSpeedHQ/codspeed-cpp
  SOURCE_SUBDIR google_benchmark
  GIT_TAG main
)
set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON)
set(BENCHMARK_ENABLE_TESTING OFF)
FetchContent_MakeAvailable(google_benchmark)

add_executable(krpsim_bench
    bench_parser.cpp
    bench_solvers.cpp
)
target_link_libraries(krpsim_bench PRIVATE krpsim_core benchmark::benchmark)
target_compile_definitions(krpsim_bench PRIVATE
    KRPSIM_RESOURCES_DIR="${CMAKE_SOURCE_DIR}/resources"
)
