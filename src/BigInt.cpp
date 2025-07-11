// src/BigInt.cpp
#include "BigInt.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <immintrin.h>  // For AVX2 intrinsics

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
        uint64_t rem = 0;
        for (int i = static_cast<int>(temp.digits.size()) - 1; i >= 0; --i) {
            uint64_t cur = (rem << 32) + temp.digits[i];
            temp.digits[i] = static_cast<uint32_t>(cur / 10);
            rem = cur % 10;
        }
        result.push_back(static_cast<char>('0' + rem));
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

// Addition operator with AVX2 optimization
BigInt BigInt::operator+(const BigInt& other) const {
    size_t n = std::max(digits.size(), other.digits.size());
    std::vector<uint32_t> a = digits;
    std::vector<uint32_t> b = other.digits;
    a.resize(n);
    b.resize(n);

    BigInt result;
    result.digits.assign(n, 0);
    uint64_t carry = 0;
    size_t i = 0;

#ifdef __AVX2__
    const size_t block = 8;
    for (; i + block <= n; i += block) {
        __m256i va = _mm256_loadu_si256((__m256i const*)&a[i]);
        __m256i vb = _mm256_loadu_si256((__m256i const*)&b[i]);
        __m256i vsum = _mm256_add_epi32(va, vb);
        alignas(32) uint32_t temp[8];
        _mm256_store_si256((__m256i*)temp, vsum);
        for (int k = 0; k < 8; ++k) {
            uint64_t s = static_cast<uint64_t>(temp[k]) + carry;
            result.digits[i + k] = static_cast<uint32_t>(s & 0xFFFFFFFF);
            carry = s >> 32;
        }
    }
#endif

    for (; i < n; ++i) {
        uint64_t s = carry + a[i] + b[i];
        result.digits[i] = static_cast<uint32_t>(s & 0xFFFFFFFF);
        carry = s >> 32;
    }
    if (carry) result.digits.push_back(static_cast<uint32_t>(carry));
    return result;
}

// Subtraction operator (assumes *this >= other)
BigInt BigInt::operator-(const BigInt& other) const {
    BigInt result;
    result.digits.clear();

    int64_t carry = 0;
    for (size_t i = 0; i < digits.size(); ++i) {
        int64_t diff = static_cast<int64_t>(digits[i]) - (i < other.digits.size() ? other.digits[i] : 0) + carry;
        if (diff < 0) {
            diff += BASE;
            carry = -1;
        } else {
            carry = 0;
        }
        result.digits.push_back(static_cast<uint32_t>(diff));
    }

    while (result.digits.size() > 1 && result.digits.back() == 0)
        result.digits.pop_back();

    return result;
}

// Multiplication operator using Karatsuba
BigInt BigInt::operator*(const BigInt& other) const {
    return karatsuba(*this, other);
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

// Helper to shift BigInt left by k base-2^32 digits
BigInt shiftLeft(const BigInt& num, size_t k) {
    if (num == BigInt(0)) return num;
    BigInt result;
    result.digits = std::vector<uint32_t>(k, 0);
    result.digits.insert(result.digits.end(), num.digits.begin(), num.digits.end());
    return result;
}

// Karatsuba multiplication
BigInt karatsuba(const BigInt& x, const BigInt& y) {
    size_t n = std::max(x.digits.size(), y.digits.size());
    if (n <= 32) {
        BigInt result;
        result.digits.assign(x.digits.size() + y.digits.size(), 0);
        for (size_t i = 0; i < x.digits.size(); ++i) {
            uint64_t carry = 0;
            for (size_t j = 0; j < y.digits.size() || carry; ++j) {
                uint64_t prod = result.digits[i + j] + static_cast<uint64_t>(x.digits[i]) * (j < y.digits.size() ? y.digits[j] : 0) + carry;
                result.digits[i + j] = static_cast<uint32_t>(prod & 0xFFFFFFFF);
                carry = prod >> 32;
            }
        }
        while (result.digits.size() > 1 && result.digits.back() == 0)
            result.digits.pop_back();
        return result;
    }

    size_t m = n / 2;
    BigInt x0, x1;
    x0.digits.assign(x.digits.begin(), x.digits.begin() + std::min(m, x.digits.size()));
    x1.digits.assign(x.digits.begin() + std::min(m, x.digits.size()), x.digits.end());
    BigInt y0, y1;
    y0.digits.assign(y.digits.begin(), y.digits.begin() + std::min(m, y.digits.size()));
    y1.digits.assign(y.digits.begin() + std::min(m, y.digits.size()), y.digits.end());

    BigInt z0 = karatsuba(x0, y0);
    BigInt z2 = karatsuba(x1, y1);
    BigInt z1 = karatsuba(x0 + x1, y0 + y1) - z0 - z2;

    return shiftLeft(z2, 2 * m) + shiftLeft(z1, m) + z0;
}

// ML integration hooks
std::vector<uint32_t> BigInt::getDigits() const {
    return digits;
}

std::vector<uint8_t> BigInt::toByteVector() const {
    std::vector<uint8_t> bytes;
    for (uint32_t d : digits) {
        for (int i = 0; i < 4; ++i) {
            bytes.push_back(static_cast<uint8_t>((d >> (i * 8)) & 0xFF));
        }
    }
    return bytes;
}

BigInt BigInt::fromByteVector(const std::vector<uint8_t>& bytes) {
    BigInt result;
    result.digits.clear();
    for (size_t i = 0; i < bytes.size(); i += 4) {
        uint32_t val = 0;
        for (int j = 0; j < 4 && (i + j) < bytes.size(); ++j) {
            val |= (static_cast<uint32_t>(bytes[i + j]) << (j * 8));
        }
        result.digits.push_back(val);
    }
    return result;
}

std::vector<float> BigInt::toNormalizedFeatures() const {
    std::vector<float> feats;
    feats.reserve(digits.size());
    for (uint32_t d : digits) {
        feats.push_back(static_cast<float>(d) / static_cast<float>(BASE));
    }
    return feats;
}

void testMLHooks() {
    BigInt num("1234");
    auto limbs = num.getDigits();
    assert(!limbs.empty());
    auto bytes = num.toByteVector();
    assert(bytes.size() == limbs.size()*4);
    BigInt num2 = BigInt::fromByteVector(bytes);
    assert(num == num2);
    auto feats = num.toNormalizedFeatures();
    assert(feats.size() == limbs.size());
    std::cout << "✅ testMLHooks passed.\n";
}