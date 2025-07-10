// src/BigInt.cpp
#include "BigInt.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

// Default constructor
BigInt::BigInt() {
    digits.push_back(0);
}

// Constructor from uint64_t
BigInt::BigInt(uint64_t num) {
    while (num > 0) {
        digits.push_back(static_cast<uint32_t>(num & 0xFFFFFFFF));
        num >>= 32;
    }
    if (digits.empty()) digits.push_back(0);
}

// Constructor from decimal string
BigInt::BigInt(const std::string& numStr) {
    *this = BigInt(0);
    for (char c : numStr) {
        if (c < '0' || c > '9') continue;
        *this = (*this * BigInt(10)) + BigInt(c - '0');
    }
}

// Convert to decimal string
std::string BigInt::toString() const {
    if (*this == BigInt(0)) return "0";

    BigInt temp = *this;
    std::string result;
    while (!(temp == BigInt(0))) {
        BigInt quotient, remainder;
        uint64_t rem = 0;
        for (int i = temp.digits.size() - 1; i >= 0; --i) {
            uint64_t cur = (rem << 32) + temp.digits[i];
            temp.digits[i] = static_cast<uint32_t>(cur / 10);
            rem = cur % 10;
        }
        result.push_back(static_cast<char>('0' + rem));
        // Trim leading zeros
        while (temp.digits.size() > 1 && temp.digits.back() == 0)
            temp.digits.pop_back();
    }
    std::reverse(result.begin(), result.end());
    return result;
}

// Equality operator
bool BigInt::operator==(const BigInt& other) const {
    return digits == other.digits;
}

// Addition operator
BigInt BigInt::operator+(const BigInt& other) const {
    BigInt result;
    result.digits.clear();

    const size_t n = std::max(digits.size(), other.digits.size());
    uint64_t carry = 0;

    for (size_t i = 0; i < n || carry; ++i) {
        uint64_t sum = carry;
        if (i < digits.size()) sum += digits[i];
        if (i < other.digits.size()) sum += other.digits[i];

        result.digits.push_back(static_cast<uint32_t>(sum & 0xFFFFFFFF));
        carry = sum >> 32;
    }
    return result;
}

// Multiplication operator (naive, to be optimized later)
BigInt BigInt::operator*(const BigInt& other) const {
    BigInt result;
    result.digits.assign(digits.size() + other.digits.size(), 0);

    for (size_t i = 0; i < digits.size(); ++i) {
        uint64_t carry = 0;
        for (size_t j = 0; j < other.digits.size() || carry; ++j) {
            uint64_t prod = result.digits[i + j] +
                static_cast<uint64_t>(digits[i]) * (j < other.digits.size() ? other.digits[j] : 0) + carry;
            result.digits[i + j] = static_cast<uint32_t>(prod & 0xFFFFFFFF);
            carry = prod >> 32;
        }
    }
    // Trim leading zeros
    while (result.digits.size() > 1 && result.digits.back() == 0)
        result.digits.pop_back();
    return result;
}

// Serialization to binary string
std::string BigInt::serializeBinary() const {
    std::string s;
    for (uint32_t d : digits) {
        for (int i = 0; i < 4; ++i) {
            s.push_back(static_cast<char>((d >> (i * 8)) & 0xFF));
        }
    }
    return s;
}

// Deserialization from binary string
BigInt BigInt::deserializeBinary(const std::string& data) {
    BigInt result;
    result.digits.clear();

    for (size_t i = 0; i < data.size(); i += 4) {
        uint32_t val = 0;
        for (int j = 0; j < 4 && (i + j) < data.size(); ++j) {
            val |= (static_cast<uint32_t>(static_cast<unsigned char>(data[i + j])) << (j * 8));
        }
        result.digits.push_back(val);
    }
    return result;
}
