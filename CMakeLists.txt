cmake_minimum_required(VERSION 3.10)
project(BigIntLibrary)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include headers
include_directories(include)

# Create BigInt library target
add_library(BigInt
    src/BigInt.cpp
)

# Test executable
add_executable(BigIntTest
    test/test_bigint.cpp
)

# Link library to test
target_link_libraries(BigIntTest BigInt)

# Benchmark executable
add_executable(BigIntBench
    benchmarks/bench.cpp
)
target_link_libraries(BigIntBench BigInt)

# After add_library(BigInt ...)
target_compile_options(BigInt PRIVATE -mavx2 -mbmi2)

# After existing targets...

# ML integration example executable
add_executable(ml_example
    examples/ml_integration_example.cpp
)

# Link against the BigInt library
target_link_libraries(ml_example PRIVATE BigInt)

# Optional: Enable C++17 and any needed flags
set_target_properties(ml_example PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)
