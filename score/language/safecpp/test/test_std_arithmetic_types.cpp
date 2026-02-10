/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
/// MISRA C++ 3-9-2 demands specific length typedefs. For integer data types the C++ standard provides <cstdint>.
/// For floating point data types there exists <cmath>. This header uses a more restrict definition of float_t and
/// double_t by enforcing a 32bit IEC559 floating point for float_t and a 64bit IEC559 floating point for double_t
/// instead of the "most efficient floating-point type at least as wide as ... ".

#include <gtest/gtest.h>
#include <climits>
#include <cstdint>
#include <limits>
#include <type_traits>

TEST(StdFloatTypes, IECConformance)
{
    // Test compliance with IEC559/IEEE754.
    static_assert(std::numeric_limits<float>::is_iec559 == true, "float doesn't conform to IEC559/IEEE754");

    // Test the bit length of the mantissa (significand) including the hidden bit.
    static_assert(std::numeric_limits<float>::digits == 24, "float doesn't conform to IEC559/IEEE754");

    // Test compliance with IEC559/IEEE754.
    static_assert(std::numeric_limits<double>::is_iec559 == true, "double doesn't conform to IEC559/IEEE754");

    // Test the bit length of the mantissa (significand) including the hidden bit.
    static_assert(std::numeric_limits<double>::digits == 53, "double doesn't conform to IEC559/IEEE754");

    // Test compliance with IEC559/IEEE754.
    static_assert(std::numeric_limits<long double>::is_iec559 == true, "long double doesn't conform to IEC559/IEEE754");

    // Test the bit length of the mantissa (significand) including the hidden bit.
    // IEC559/IEEE754 specifies the binary64-extended format and binary 128 format. Both of them have at least 63
    // mantissa bits and one additional hidden bit.
    static_assert(std::numeric_limits<long double>::digits >= 64, "long double doesn't conform to IEC559/IEEE754");
}

TEST(StdIntegralTypes, signess_and_bytes_number)
{
// Test identifies implementation defined behavior.
// As the standard does not specify if plain char is signed or unsigned.
// char on QNX is considered as unsigned char and on GCC and Clang is considered as signed.
#if defined(__QNX__)
    // As the signess of char depends on using qemu or the real hardware, this line got commented until we decide
    // How to move forward in this ticket Ticket-141110
    // static_assert(std::numeric_limits<char>::is_signed == false,
    //               "it is expected a 'char' to be unsigned in a QNX environment");
#else
    // Note: char signedness is implementation-defined and varies by architecture
    // x86_64 Linux: signed char, aarch64 Linux: unsigned char
#if defined(__x86_64__)
    static_assert(std::numeric_limits<char>::is_signed == true,
                  "it is expected a 'char' to be signed in a x86_64 linux environment");
#elif defined(__aarch64__)
    static_assert(std::numeric_limits<char>::is_signed == false,
                  "it is expected a 'char' to be unsigned in a aarch64 linux environment");
#endif
#endif
    // Test identifies number of bytes
    static_assert(sizeof(char) == 1, "it is expected a char bytes' number to be 1");

    static_assert(sizeof(signed char) == 1, "it is expected a signed char bytes' number to be 1");

    static_assert(sizeof(unsigned char) == 1, "it is expected a unsigned char bytes' number to be 1");

    static_assert(sizeof(short int) == 2, "it is expected a short int bytes' number to be 2");

    static_assert(sizeof(unsigned short int) == 2, "it is expected a unsigned short int bytes' number to be 2");

    static_assert(sizeof(int) == 4, "it is expected a int bytes' number to be 4");

    static_assert(sizeof(unsigned int) == 4, "it is expected a unsigned int bytes' number to be 4");

    static_assert(sizeof(long int) == 8, "it is expected a long int bytes' number to be 8");

    static_assert(sizeof(unsigned long int) == 8, "it is expected a unsigned long int bytes' number to be 8");

    static_assert(sizeof(long long int) == 8, "it is expected a long long int bytes' number to be 8");

    static_assert(sizeof(unsigned long long int) == 8, "it is expected a unsigned long long int bytes' number to be 8");

    // Required since our code relies on `std::uint8_t` to be usable for interpreting an object's memory repr. byte-wise
    // Technically, using `std::uint8_t` for such purpose is undefined behavior since it violates the C++ type aliasing
    // rules. These state that an object's byte representation is only permitted to be accessed via the types `char`,
    // `unsigned char` or `std::byte` (cf. section 6.9 [basic.types] point 2 of the C++17 standard). However, since
    // plenty of platform code already (incorrectly) uses `std::uint8_t` for such purpose, we mitigate by asserting
    // that `std::uint8_t` is just a type alias for `unsigned char` and we hence immediately notice once that changes.
    static_assert(std::is_same_v<std::uint8_t, unsigned char>,
                  "std::uint8_t is expected to be an alias for unsigned char");
}
