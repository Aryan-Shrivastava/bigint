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
