#include "../include/BigInt.h"
#include <chrono>
#include <iostream>

int main() {
    BigInt a("1234567890123456789012345678901234567890");
    BigInt b("9876543210987654321098765432109876543210");

    // Benchmark addition
    auto start_add = std::chrono::high_resolution_clock::now();
    BigInt c = a + b;
    auto end_add = std::chrono::high_resolution_clock::now();
    auto duration_add = std::chrono::duration_cast<std::chrono::nanoseconds>(end_add - start_add).count();
    std::cout << "Addition took: " << duration_add << " ns\n";

    // Benchmark multiplication
    auto start_mul = std::chrono::high_resolution_clock::now();
    BigInt d = a * b;
    auto end_mul = std::chrono::high_resolution_clock::now();
    auto duration_mul = std::chrono::duration_cast<std::chrono::nanoseconds>(end_mul - start_mul).count();
    std::cout << "Multiplication took: " << duration_mul << " ns\n";

    return 0;
}
