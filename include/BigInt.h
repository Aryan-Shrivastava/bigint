#ifndef BIGINT_H
#define BIGINT_H

#include <vector>
#include <string>
#include <cstdint>
#include <iostream>

class BigInt {
public:
    BigInt();
    BigInt(uint64_t num);
    BigInt(const std::string& numStr);

    std::string toString() const;

    // Arithmetic operators
    BigInt operator+(const BigInt& other) const;
    BigInt operator*(const BigInt& other) const;

    // Comparison
    bool operator==(const BigInt& other) const;

    // Serialization
    std::string serializeBinary() const;
    static BigInt deserializeBinary(const std::string& data);

private:
    std::vector<uint32_t> digits;  // Base 2^32 representation
    static const uint64_t BASE = 0x100000000ULL;  // 2^32
};

#endif // BIGINT_H
