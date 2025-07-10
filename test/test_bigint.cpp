#include "../include/BigInt.h"
#include <cassert>
#include <iostream>

void testConstructors() {
    BigInt a(123456789);
    assert(a.toString() == "123456789");

    BigInt b("98765432109876543210");
    assert(b.toString() == "98765432109876543210");

    BigInt c;
    assert(c.toString() == "0");

    std::cout << "✅ testConstructors passed.\n";
}

void testAddition() {
    BigInt a("99999999999999999999");
    BigInt b("1");
    BigInt c = a + b;
    assert(c.toString() == "100000000000000000000");

    BigInt d("123456789");
    BigInt e("987654321");
    BigInt f = d + e;
    assert(f.toString() == "1111111110");

    std::cout << "✅ testAddition passed.\n";
}

void testMultiplication() {
    BigInt a("12345");
    BigInt b("6789");
    BigInt c = a * b;
    assert(c.toString() == "83810205");

    BigInt d("10000000000000000000");
    BigInt e("10");
    BigInt f = d * e;
    assert(f.toString() == "100000000000000000000");

    std::cout << "✅ testMultiplication passed.\n";
}

void testEquality() {
    BigInt a("123456789");
    BigInt b("123456789");
    BigInt c("987654321");

    assert(a == b);
    assert(!(a == c));

    std::cout << "✅ testEquality passed.\n";
}

void testSerialization() {
    BigInt a("123456789012345678901234567890");
    std::string bin = a.serializeBinary();
    BigInt b = BigInt::deserializeBinary(bin);
    assert(a == b);

    std::cout << "✅ testSerialization passed.\n";
}

int main() {
    testConstructors();
    testAddition();
    testMultiplication();
    testEquality();
    testSerialization();

    std::cout << "\n🎉 All tests passed successfully!\n";
    return 0;
}
