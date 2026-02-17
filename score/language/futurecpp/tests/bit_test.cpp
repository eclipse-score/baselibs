/********************************************************************************
 * Copyright (c) 2019 Contributors to the Eclipse Foundation
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

///
/// \file
/// \copyright Copyright (c) 2019 Contributors to the Eclipse Foundation
///
/// Tests are derived from the examples at
///  * https://en.cppreference.com/w/cpp/numeric/bit_cast
///  * https://chromium.googlesource.com/chromium/src/base/+/master/bit_cast.h
///

#include <score/bit.hpp>
#include <score/bit.hpp> // include guard test

#include <score/assert_support.hpp>
#include <score/size.hpp>

#include <cstdint>

#include <gtest/gtest.h>

namespace
{
using score::cpp::literals::operator""_UZ;

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(BitCastTest, DoubleToUnsignedInt64)
{
    const auto result = score::cpp::bit_cast<std::uint64_t>(19880124.0);
    ASSERT_EQ(result, 0x4172f58bc0000000);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(BitCastTest, UnsignedInt64ToDouble)
{
    const auto result = score::cpp::bit_cast<double>(0x4172f58bc0000000);
    ASSERT_DOUBLE_EQ(result, 19880124.0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(BitCastTest, FloatToInt32)
{
    const auto result = score::cpp::bit_cast<std::int32_t>(3.14159265358979F);
    ASSERT_EQ(result, 0x40490fdb);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(BitCastTest, Int32ToFloat)
{
    const auto result = score::cpp::bit_cast<float>(0x40490fdb);
    ASSERT_EQ(result, 3.14159265358979F);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(BitCastTest, DoesNotCopy)
{
    const auto origin = float{0.0F};
    const auto* from = &origin;

    const auto* result = score::cpp::bit_cast<std::int32_t*>(from);

    ASSERT_EQ(static_cast<const void*>(result), static_cast<const void*>(from));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(RotlTest, PositiveLeftRotation)
{
    auto rotl8 = score::cpp::rotl<std::uint8_t>;
    constexpr int rotl8_bits = std::numeric_limits<std::uint8_t>::digits;

    EXPECT_EQ(rotl8(0U, 0), 0U);
    EXPECT_EQ(rotl8(0U, 4), 0U);
    EXPECT_EQ(rotl8(12U, 0), 12U);
    EXPECT_EQ(rotl8(0xFFU, 0), 0xFFU);

    EXPECT_EQ(rotl8(0b01001101, 1), 0b10011010);
    EXPECT_EQ(rotl8(0b01001101, 3), 0b01101010);
    EXPECT_EQ(rotl8(0b01001101, rotl8_bits), 0b01001101);
    EXPECT_EQ(rotl8(0b01001101, rotl8_bits + 2), 0b00110101);
    EXPECT_EQ(rotl8(0b01001101, rotl8_bits * 4 + 1), 0b10011010);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(RotlTest, NegativeLeftRotation)
{
    auto rotl8 = score::cpp::rotl<std::uint8_t>;
    constexpr int rotl8_bits = std::numeric_limits<std::uint8_t>::digits;

    EXPECT_EQ(rotl8(0b01001101, -1), 0b10100110);
    EXPECT_EQ(rotl8(0b01001101, -3), 0b10101001);
    EXPECT_EQ(rotl8(0b01001101, -rotl8_bits), 0b01001101);
    EXPECT_EQ(rotl8(0b01001101, -rotl8_bits - 2), 0b01010011);
    EXPECT_EQ(rotl8(0b01001101, -rotl8_bits * 4 - 1), 0b10100110);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(RotlTest, 64BitLeftRotation)
{
    auto rotl64 = score::cpp::rotl<std::uint64_t>;
    constexpr int rotl64_bits = std::numeric_limits<std::uint64_t>::digits;

    EXPECT_EQ(rotl64(0x262766C04705670DULL, 5), 0xC4ECD808E0ACE1A4ULL);
    EXPECT_EQ(rotl64(0x262766C04705670DULL, -5), 0x69313B3602382B38ULL);
    EXPECT_EQ(rotl64(0x262766C04705670DULL, rotl64_bits), 0x262766C04705670DULL);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(RotrTest, IdenticalToNegativeLeftRotation)
{
    EXPECT_EQ(score::cpp::rotr(123456789U, 0), score::cpp::rotl(123456789U, 0));
    EXPECT_EQ(score::cpp::rotr(123456789U, 3), score::cpp::rotl(123456789U, -3));
    EXPECT_EQ(score::cpp::rotr(123456789U, 29), score::cpp::rotl(123456789U, -29));
    EXPECT_EQ(score::cpp::rotr(123456789U, 73), score::cpp::rotl(123456789U, -73));
    EXPECT_EQ(score::cpp::rotr(123456789U, -4), score::cpp::rotl(123456789U, 4));
    EXPECT_EQ(score::cpp::rotr(123456789U, -47), score::cpp::rotl(123456789U, 47));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(HasSingleBitTest, Spec)
{
    EXPECT_TRUE(score::cpp::has_single_bit(std::uint8_t{0b00000001}));
    EXPECT_TRUE(score::cpp::has_single_bit(std::uint8_t{0b00000010}));
    EXPECT_TRUE(score::cpp::has_single_bit(std::uint8_t{0b00000100}));
    EXPECT_TRUE(score::cpp::has_single_bit(std::uint8_t{0b00001000}));
    EXPECT_TRUE(score::cpp::has_single_bit(std::uint8_t{0b00010000}));
    EXPECT_TRUE(score::cpp::has_single_bit(std::uint8_t{0b00100000}));
    EXPECT_TRUE(score::cpp::has_single_bit(std::uint8_t{0b01000000}));
    EXPECT_TRUE(score::cpp::has_single_bit(std::uint8_t{0b10000000}));

    EXPECT_FALSE(score::cpp::has_single_bit(std::uint8_t{0b00000000}));
    EXPECT_FALSE(score::cpp::has_single_bit(std::uint8_t{0b00000011}));
    EXPECT_FALSE(score::cpp::has_single_bit(std::uint8_t{0b00000101}));
    EXPECT_FALSE(score::cpp::has_single_bit(std::uint8_t{0b00000110}));
    EXPECT_FALSE(score::cpp::has_single_bit(std::uint8_t{0b00000111}));
    EXPECT_FALSE(score::cpp::has_single_bit(std::uint8_t{0b00001001}));

    EXPECT_TRUE(score::cpp::has_single_bit(std::uint16_t{1U << 15}));
    EXPECT_TRUE(score::cpp::has_single_bit(1U << 31));
    EXPECT_TRUE(score::cpp::has_single_bit(1_UZ << 63));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(HasSingleBitTest, Constexpr)
{
    static_assert(score::cpp::has_single_bit(std::uint8_t{0b00000001}), "");
    static_assert(score::cpp::has_single_bit(std::uint8_t{0b00000010}), "");
    static_assert(score::cpp::has_single_bit(std::uint8_t{0b00000100}), "");
    static_assert(score::cpp::has_single_bit(std::uint8_t{0b00001000}), "");
    static_assert(score::cpp::has_single_bit(std::uint8_t{0b00010000}), "");
    static_assert(score::cpp::has_single_bit(std::uint8_t{0b00100000}), "");
    static_assert(score::cpp::has_single_bit(std::uint8_t{0b01000000}), "");
    static_assert(score::cpp::has_single_bit(std::uint8_t{0b10000000}), "");

    static_assert(!score::cpp::has_single_bit(std::uint8_t{0b00000000}), "");
    static_assert(!score::cpp::has_single_bit(std::uint8_t{0b00000011}), "");
    static_assert(!score::cpp::has_single_bit(std::uint8_t{0b00000101}), "");
    static_assert(!score::cpp::has_single_bit(std::uint8_t{0b00000110}), "");
    static_assert(!score::cpp::has_single_bit(std::uint8_t{0b00000111}), "");
    static_assert(!score::cpp::has_single_bit(std::uint8_t{0b00001001}), "");

    static_assert(score::cpp::has_single_bit(std::uint16_t{1U << 15}), "");
    static_assert(score::cpp::has_single_bit(1U << 31), "");
    static_assert(score::cpp::has_single_bit(1_UZ << 63), "");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(BitWidthTest, Spec)
{
    EXPECT_EQ(score::cpp::bit_width(std::uint8_t{0b00000000}), 0U);
    EXPECT_EQ(score::cpp::bit_width(std::uint8_t{0b00000001}), 1U);
    EXPECT_EQ(score::cpp::bit_width(std::uint8_t{0b00000011}), 2U);
    EXPECT_EQ(score::cpp::bit_width(std::uint8_t{0b00000111}), 3U);
    EXPECT_EQ(score::cpp::bit_width(std::uint8_t{0b00001111}), 4U);
    EXPECT_EQ(score::cpp::bit_width(std::uint8_t{0b00011111}), 5U);
    EXPECT_EQ(score::cpp::bit_width(std::uint8_t{0b00111111}), 6U);
    EXPECT_EQ(score::cpp::bit_width(std::uint8_t{0b01111111}), 7U);
    EXPECT_EQ(score::cpp::bit_width(std::uint8_t{0b11111111}), 8U);

    EXPECT_EQ(score::cpp::bit_width(std::uint8_t{0b00000000}), 0U);
    EXPECT_EQ(score::cpp::bit_width(std::uint8_t{0b00000001}), 1U);
    EXPECT_EQ(score::cpp::bit_width(std::uint8_t{0b00000010}), 2U);
    EXPECT_EQ(score::cpp::bit_width(std::uint8_t{0b00000100}), 3U);
    EXPECT_EQ(score::cpp::bit_width(std::uint8_t{0b00001000}), 4U);
    EXPECT_EQ(score::cpp::bit_width(std::uint8_t{0b00010000}), 5U);
    EXPECT_EQ(score::cpp::bit_width(std::uint8_t{0b00100000}), 6U);
    EXPECT_EQ(score::cpp::bit_width(std::uint8_t{0b01000000}), 7U);
    EXPECT_EQ(score::cpp::bit_width(std::uint8_t{0b10000000}), 8U);

    EXPECT_EQ(score::cpp::bit_width(std::numeric_limits<std::uint16_t>::max()), 16);
    EXPECT_EQ(score::cpp::bit_width(std::numeric_limits<std::uint32_t>::max()), 32);
    EXPECT_EQ(score::cpp::bit_width(std::numeric_limits<std::uint64_t>::max()), 64);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(BitWidthTest, Constexpr)
{
    static_assert(score::cpp::bit_width(std::uint8_t{0b00000000}) == 0U, "");
    static_assert(score::cpp::bit_width(std::uint8_t{0b00000001}) == 1U, "");
    static_assert(score::cpp::bit_width(std::uint8_t{0b00000011}) == 2U, "");
    static_assert(score::cpp::bit_width(std::uint8_t{0b00000111}) == 3U, "");
    static_assert(score::cpp::bit_width(std::uint8_t{0b00001111}) == 4U, "");
    static_assert(score::cpp::bit_width(std::uint8_t{0b00011111}) == 5U, "");
    static_assert(score::cpp::bit_width(std::uint8_t{0b00111111}) == 6U, "");
    static_assert(score::cpp::bit_width(std::uint8_t{0b01111111}) == 7U, "");
    static_assert(score::cpp::bit_width(std::uint8_t{0b11111111}) == 8U, "");

    static_assert(score::cpp::bit_width(std::uint8_t{0b00000000}) == 0U, "");
    static_assert(score::cpp::bit_width(std::uint8_t{0b00000001}) == 1U, "");
    static_assert(score::cpp::bit_width(std::uint8_t{0b00000010}) == 2U, "");
    static_assert(score::cpp::bit_width(std::uint8_t{0b00000100}) == 3U, "");
    static_assert(score::cpp::bit_width(std::uint8_t{0b00001000}) == 4U, "");
    static_assert(score::cpp::bit_width(std::uint8_t{0b00010000}) == 5U, "");
    static_assert(score::cpp::bit_width(std::uint8_t{0b00100000}) == 6U, "");
    static_assert(score::cpp::bit_width(std::uint8_t{0b01000000}) == 7U, "");
    static_assert(score::cpp::bit_width(std::uint8_t{0b10000000}) == 8U, "");

    static_assert(score::cpp::bit_width(std::numeric_limits<std::uint16_t>::max()) == 16, "");
    static_assert(score::cpp::bit_width(std::numeric_limits<std::uint32_t>::max()) == 32, "");
    static_assert(score::cpp::bit_width(std::numeric_limits<std::uint64_t>::max()) == 64, "");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(BitCeilTest, Spec)
{
    EXPECT_EQ(score::cpp::bit_ceil(std::uint8_t{0b00000000}), 0b00000001);
    EXPECT_EQ(score::cpp::bit_ceil(std::uint8_t{0b00000001}), 0b00000001);
    EXPECT_EQ(score::cpp::bit_ceil(std::uint8_t{0b00000010}), 0b00000010);
    EXPECT_EQ(score::cpp::bit_ceil(std::uint8_t{0b00000011}), 0b00000100);
    EXPECT_EQ(score::cpp::bit_ceil(std::uint8_t{0b00000100}), 0b00000100);
    EXPECT_EQ(score::cpp::bit_ceil(std::uint8_t{0b00000101}), 0b00001000);
    EXPECT_EQ(score::cpp::bit_ceil(std::uint8_t{0b00000111}), 0b00001000);
    EXPECT_EQ(score::cpp::bit_ceil(std::uint8_t{0b00001000}), 0b00001000);
    EXPECT_EQ(score::cpp::bit_ceil(std::uint8_t{0b00001001}), 0b00010000);
    EXPECT_EQ(score::cpp::bit_ceil(std::uint8_t{0b00001111}), 0b00010000);
    EXPECT_EQ(score::cpp::bit_ceil(std::uint8_t{0b00010000}), 0b00010000);
    EXPECT_EQ(score::cpp::bit_ceil(std::uint8_t{0b00010001}), 0b00100000);
    EXPECT_EQ(score::cpp::bit_ceil(std::uint8_t{0b00011111}), 0b00100000);
    EXPECT_EQ(score::cpp::bit_ceil(std::uint8_t{0b00100000}), 0b00100000);
    EXPECT_EQ(score::cpp::bit_ceil(std::uint8_t{0b00100001}), 0b01000000);
    EXPECT_EQ(score::cpp::bit_ceil(std::uint8_t{0b00111111}), 0b01000000);
    EXPECT_EQ(score::cpp::bit_ceil(std::uint8_t{0b01000000}), 0b01000000);
    EXPECT_EQ(score::cpp::bit_ceil(std::uint8_t{0b01000001}), 0b10000000);
    EXPECT_EQ(score::cpp::bit_ceil(std::uint8_t{0b01111111}), 0b10000000);
    EXPECT_EQ(score::cpp::bit_ceil(std::uint8_t{0b10000000}), 0b10000000);

    EXPECT_EQ(score::cpp::bit_ceil(static_cast<std::uint16_t>((1U << 14) + 1U)), (1U << 15));
    EXPECT_EQ(score::cpp::bit_ceil((1U << 30) + 1U), (1U << 31));
    EXPECT_EQ(score::cpp::bit_ceil((1_UZ << 62) + 1U), (1_UZ << 63));

    EXPECT_EQ(score::cpp::bit_ceil(static_cast<std::uint16_t>((1U << 15) - 1U)), (1U << 15));
    EXPECT_EQ(score::cpp::bit_ceil((1U << 31) - 1U), (1U << 31));
    EXPECT_EQ(score::cpp::bit_ceil((1_UZ << 63) - 1U), (1_UZ << 63));

    EXPECT_EQ(score::cpp::bit_ceil(static_cast<std::uint16_t>(1U << 15)), (1U << 15));
    EXPECT_EQ(score::cpp::bit_ceil(1U << 31), (1U << 31));
    EXPECT_EQ(score::cpp::bit_ceil(1_UZ << 63), (1_UZ << 63));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(BitCeilTest, Constexpr)
{
    static_assert(score::cpp::bit_ceil(std::uint8_t{0b00000000}) == 0b00000001, "");
    static_assert(score::cpp::bit_ceil(std::uint8_t{0b00000001}) == 0b00000001, "");
    static_assert(score::cpp::bit_ceil(std::uint8_t{0b00000010}) == 0b00000010, "");
    static_assert(score::cpp::bit_ceil(std::uint8_t{0b00000011}) == 0b00000100, "");
    static_assert(score::cpp::bit_ceil(std::uint8_t{0b00000100}) == 0b00000100, "");
    static_assert(score::cpp::bit_ceil(std::uint8_t{0b00000101}) == 0b00001000, "");
    static_assert(score::cpp::bit_ceil(std::uint8_t{0b00000111}) == 0b00001000, "");
    static_assert(score::cpp::bit_ceil(std::uint8_t{0b00001000}) == 0b00001000, "");
    static_assert(score::cpp::bit_ceil(std::uint8_t{0b00001001}) == 0b00010000, "");
    static_assert(score::cpp::bit_ceil(std::uint8_t{0b00001111}) == 0b00010000, "");
    static_assert(score::cpp::bit_ceil(std::uint8_t{0b00010000}) == 0b00010000, "");
    static_assert(score::cpp::bit_ceil(std::uint8_t{0b00010001}) == 0b00100000, "");
    static_assert(score::cpp::bit_ceil(std::uint8_t{0b00011111}) == 0b00100000, "");
    static_assert(score::cpp::bit_ceil(std::uint8_t{0b00100000}) == 0b00100000, "");
    static_assert(score::cpp::bit_ceil(std::uint8_t{0b00100001}) == 0b01000000, "");
    static_assert(score::cpp::bit_ceil(std::uint8_t{0b00111111}) == 0b01000000, "");
    static_assert(score::cpp::bit_ceil(std::uint8_t{0b01000000}) == 0b01000000, "");
    static_assert(score::cpp::bit_ceil(std::uint8_t{0b01000001}) == 0b10000000, "");
    static_assert(score::cpp::bit_ceil(std::uint8_t{0b01111111}) == 0b10000000, "");
    static_assert(score::cpp::bit_ceil(std::uint8_t{0b10000000}) == 0b10000000, "");

    static_assert(score::cpp::bit_ceil(static_cast<std::uint16_t>((1U << 14) + 1U)) == (1U << 15), "");
    static_assert(score::cpp::bit_ceil((1U << 30) + 1U) == (1U << 31), "");
    static_assert(score::cpp::bit_ceil((1_UZ << 62) + 1U) == (1_UZ << 63), "");

    static_assert(score::cpp::bit_ceil(static_cast<std::uint16_t>((1U << 15) - 1U)) == (1U << 15), "");
    static_assert(score::cpp::bit_ceil((1U << 31) - 1U) == (1U << 31), "");
    static_assert(score::cpp::bit_ceil((1_UZ << 63) - 1U) == (1_UZ << 63), "");

    static_assert(score::cpp::bit_ceil(static_cast<std::uint16_t>(1U << 15)) == (1U << 15), "");
    static_assert(score::cpp::bit_ceil(1U << 31) == (1U << 31), "");
    static_assert(score::cpp::bit_ceil(1_UZ << 63) == (1_UZ << 63), "");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(BitCeilTest, BitCeilIsUndefinedIfResultIsNotRepresentable)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::bit_ceil(std::uint8_t{(1U << 7) + 1U}));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::bit_ceil(std::uint16_t{(1U << 15) + 1U}));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::bit_ceil((1U << 31) + 1U));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::bit_ceil((1_UZ << 63) + 1U));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(BitFloorTest, Spec)
{
    EXPECT_EQ(score::cpp::bit_floor(std::uint8_t{0b00000000}), 0b00000000);
    EXPECT_EQ(score::cpp::bit_floor(std::uint8_t{0b00000001}), 0b00000001);
    EXPECT_EQ(score::cpp::bit_floor(std::uint8_t{0b00000010}), 0b00000010);
    EXPECT_EQ(score::cpp::bit_floor(std::uint8_t{0b00000011}), 0b00000010);
    EXPECT_EQ(score::cpp::bit_floor(std::uint8_t{0b00000100}), 0b00000100);
    EXPECT_EQ(score::cpp::bit_floor(std::uint8_t{0b00000101}), 0b00000100);
    EXPECT_EQ(score::cpp::bit_floor(std::uint8_t{0b00000111}), 0b00000100);
    EXPECT_EQ(score::cpp::bit_floor(std::uint8_t{0b00001000}), 0b00001000);
    EXPECT_EQ(score::cpp::bit_floor(std::uint8_t{0b00001001}), 0b00001000);
    EXPECT_EQ(score::cpp::bit_floor(std::uint8_t{0b00001111}), 0b00001000);
    EXPECT_EQ(score::cpp::bit_floor(std::uint8_t{0b00010000}), 0b00010000);
    EXPECT_EQ(score::cpp::bit_floor(std::uint8_t{0b00010001}), 0b00010000);
    EXPECT_EQ(score::cpp::bit_floor(std::uint8_t{0b00011111}), 0b00010000);
    EXPECT_EQ(score::cpp::bit_floor(std::uint8_t{0b00100000}), 0b00100000);
    EXPECT_EQ(score::cpp::bit_floor(std::uint8_t{0b00100001}), 0b00100000);
    EXPECT_EQ(score::cpp::bit_floor(std::uint8_t{0b00111111}), 0b00100000);
    EXPECT_EQ(score::cpp::bit_floor(std::uint8_t{0b01000000}), 0b01000000);
    EXPECT_EQ(score::cpp::bit_floor(std::uint8_t{0b01000001}), 0b01000000);
    EXPECT_EQ(score::cpp::bit_floor(std::uint8_t{0b01111111}), 0b01000000);
    EXPECT_EQ(score::cpp::bit_floor(std::uint8_t{0b10000000}), 0b10000000);
    EXPECT_EQ(score::cpp::bit_floor(std::uint8_t{0b10000001}), 0b10000000);
    EXPECT_EQ(score::cpp::bit_floor(std::uint8_t{0b11111111}), 0b10000000);

    EXPECT_EQ(score::cpp::bit_floor(std::uint16_t{1U << 15}), (1U << 15));
    EXPECT_EQ(score::cpp::bit_floor(std::uint16_t{(1U << 15) + 1U}), (1U << 15));
    EXPECT_EQ(score::cpp::bit_floor(std::numeric_limits<std::uint16_t>::max()), (1U << 15));

    EXPECT_EQ(score::cpp::bit_floor(1U << 31), (1U << 31));
    EXPECT_EQ(score::cpp::bit_floor((1U << 31) + 1U), (1U << 31));
    EXPECT_EQ(score::cpp::bit_floor(std::numeric_limits<std::uint32_t>::max()), (1U << 31));

    EXPECT_EQ(score::cpp::bit_floor(1_UZ << 63), (1_UZ << 63));
    EXPECT_EQ(score::cpp::bit_floor((1_UZ << 63) + 1U), (1_UZ << 63));
    EXPECT_EQ(score::cpp::bit_floor(std::numeric_limits<std::uint64_t>::max()), (1_UZ << 63));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(BitFloorTest, Constexpr)
{
    static_assert(score::cpp::bit_floor(std::uint8_t{0b00000000}) == 0b00000000, "");
    static_assert(score::cpp::bit_floor(std::uint8_t{0b00000001}) == 0b00000001, "");
    static_assert(score::cpp::bit_floor(std::uint8_t{0b00000010}) == 0b00000010, "");
    static_assert(score::cpp::bit_floor(std::uint8_t{0b00000011}) == 0b00000010, "");
    static_assert(score::cpp::bit_floor(std::uint8_t{0b00000100}) == 0b00000100, "");
    static_assert(score::cpp::bit_floor(std::uint8_t{0b00000101}) == 0b00000100, "");
    static_assert(score::cpp::bit_floor(std::uint8_t{0b00000111}) == 0b00000100, "");
    static_assert(score::cpp::bit_floor(std::uint8_t{0b00001000}) == 0b00001000, "");
    static_assert(score::cpp::bit_floor(std::uint8_t{0b00001001}) == 0b00001000, "");
    static_assert(score::cpp::bit_floor(std::uint8_t{0b00001111}) == 0b00001000, "");
    static_assert(score::cpp::bit_floor(std::uint8_t{0b00010000}) == 0b00010000, "");
    static_assert(score::cpp::bit_floor(std::uint8_t{0b00010001}) == 0b00010000, "");
    static_assert(score::cpp::bit_floor(std::uint8_t{0b00011111}) == 0b00010000, "");
    static_assert(score::cpp::bit_floor(std::uint8_t{0b00100000}) == 0b00100000, "");
    static_assert(score::cpp::bit_floor(std::uint8_t{0b00100001}) == 0b00100000, "");
    static_assert(score::cpp::bit_floor(std::uint8_t{0b00111111}) == 0b00100000, "");
    static_assert(score::cpp::bit_floor(std::uint8_t{0b01000000}) == 0b01000000, "");
    static_assert(score::cpp::bit_floor(std::uint8_t{0b01000001}) == 0b01000000, "");
    static_assert(score::cpp::bit_floor(std::uint8_t{0b01111111}) == 0b01000000, "");
    static_assert(score::cpp::bit_floor(std::uint8_t{0b10000000}) == 0b10000000, "");
    static_assert(score::cpp::bit_floor(std::uint8_t{0b10000001}) == 0b10000000, "");
    static_assert(score::cpp::bit_floor(std::uint8_t{0b11111111}) == 0b10000000, "");

    static_assert(score::cpp::bit_floor(std::uint16_t{1U << 15}) == (1U << 15), "");
    static_assert(score::cpp::bit_floor(std::uint16_t{(1U << 15) + 1U}) == (1U << 15), "");
    static_assert(score::cpp::bit_floor(std::numeric_limits<std::uint16_t>::max()) == (1U << 15), "");

    static_assert(score::cpp::bit_floor(1U << 31) == (1U << 31), "");
    static_assert(score::cpp::bit_floor((1U << 31) + 1U) == (1U << 31), "");
    static_assert(score::cpp::bit_floor(std::numeric_limits<std::uint32_t>::max()) == (1U << 31), "");

    static_assert(score::cpp::bit_floor(1_UZ << 63) == (1_UZ << 63), "");
    static_assert(score::cpp::bit_floor((1_UZ << 63) + 1U) == (1_UZ << 63), "");
    static_assert(score::cpp::bit_floor(std::numeric_limits<std::uint64_t>::max()) == (1_UZ << 63), "");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(CountLZeroTest, Spec)
{
    EXPECT_EQ(score::cpp::countl_zero(std::uint8_t{0b00000000}), 8);
    EXPECT_EQ(score::cpp::countl_zero(std::uint8_t{0b00000001}), 7);
    EXPECT_EQ(score::cpp::countl_zero(std::uint8_t{0b00000010}), 6);
    EXPECT_EQ(score::cpp::countl_zero(std::uint8_t{0b00000011}), 6);
    EXPECT_EQ(score::cpp::countl_zero(std::uint8_t{0b00000100}), 5);
    EXPECT_EQ(score::cpp::countl_zero(std::uint8_t{0b00000111}), 5);
    EXPECT_EQ(score::cpp::countl_zero(std::uint8_t{0b00001000}), 4);
    EXPECT_EQ(score::cpp::countl_zero(std::uint8_t{0b00001111}), 4);
    EXPECT_EQ(score::cpp::countl_zero(std::uint8_t{0b00010000}), 3);
    EXPECT_EQ(score::cpp::countl_zero(std::uint8_t{0b00011111}), 3);
    EXPECT_EQ(score::cpp::countl_zero(std::uint8_t{0b00100000}), 2);
    EXPECT_EQ(score::cpp::countl_zero(std::uint8_t{0b00111111}), 2);
    EXPECT_EQ(score::cpp::countl_zero(std::uint8_t{0b01000000}), 1);
    EXPECT_EQ(score::cpp::countl_zero(std::uint8_t{0b01111111}), 1);
    EXPECT_EQ(score::cpp::countl_zero(std::uint8_t{0b10000000}), 0);
    EXPECT_EQ(score::cpp::countl_zero(std::uint8_t{0b11111111}), 0);

    EXPECT_EQ(score::cpp::countl_zero(std::uint16_t{0}), 16);
    EXPECT_EQ(score::cpp::countl_zero(std::numeric_limits<std::uint16_t>::max()), 0);

    EXPECT_EQ(score::cpp::countl_zero(0U), 32);
    EXPECT_EQ(score::cpp::countl_zero(std::numeric_limits<std::uint32_t>::max()), 0);

    EXPECT_EQ(score::cpp::countl_zero(0_UZ), 64);
    EXPECT_EQ(score::cpp::countl_zero(std::numeric_limits<std::uint64_t>::max()), 0);

    EXPECT_EQ(score::cpp::countl_zero(0ULL), std::numeric_limits<unsigned long long>::digits);
    EXPECT_EQ(score::cpp::countl_zero(std::numeric_limits<unsigned long long>::max()), 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(CountLZeroTest, Constexpr)
{
    static_assert(score::cpp::countl_zero(std::uint8_t{0b00000000}) == 8, "");
    static_assert(score::cpp::countl_zero(std::uint8_t{0b00000001}) == 7, "");
    static_assert(score::cpp::countl_zero(std::uint8_t{0b00000010}) == 6, "");
    static_assert(score::cpp::countl_zero(std::uint8_t{0b00000011}) == 6, "");
    static_assert(score::cpp::countl_zero(std::uint8_t{0b00000100}) == 5, "");
    static_assert(score::cpp::countl_zero(std::uint8_t{0b00000111}) == 5, "");
    static_assert(score::cpp::countl_zero(std::uint8_t{0b00001000}) == 4, "");
    static_assert(score::cpp::countl_zero(std::uint8_t{0b00001111}) == 4, "");
    static_assert(score::cpp::countl_zero(std::uint8_t{0b00010000}) == 3, "");
    static_assert(score::cpp::countl_zero(std::uint8_t{0b00011111}) == 3, "");
    static_assert(score::cpp::countl_zero(std::uint8_t{0b00100000}) == 2, "");
    static_assert(score::cpp::countl_zero(std::uint8_t{0b00111111}) == 2, "");
    static_assert(score::cpp::countl_zero(std::uint8_t{0b01000000}) == 1, "");
    static_assert(score::cpp::countl_zero(std::uint8_t{0b01111111}) == 1, "");
    static_assert(score::cpp::countl_zero(std::uint8_t{0b10000000}) == 0, "");
    static_assert(score::cpp::countl_zero(std::uint8_t{0b11111111}) == 0, "");

    static_assert(score::cpp::countl_zero(std::uint16_t{0}) == 16, "");
    static_assert(score::cpp::countl_zero(std::numeric_limits<std::uint16_t>::max()) == 0, "");

    static_assert(score::cpp::countl_zero(0U) == 32, "");
    static_assert(score::cpp::countl_zero(std::numeric_limits<std::uint32_t>::max()) == 0, "");

    static_assert(score::cpp::countl_zero(0_UZ) == 64, "");
    static_assert(score::cpp::countl_zero(std::numeric_limits<std::uint64_t>::max()) == 0, "");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(CountLOneTest, Spec)
{
    EXPECT_EQ(score::cpp::countl_one(std::uint8_t{0b11111111}), 8);
    EXPECT_EQ(score::cpp::countl_one(std::uint8_t{0b11111110}), 7);
    EXPECT_EQ(score::cpp::countl_one(std::uint8_t{0b11111101}), 6);
    EXPECT_EQ(score::cpp::countl_one(std::uint8_t{0b11111100}), 6);
    EXPECT_EQ(score::cpp::countl_one(std::uint8_t{0b11111011}), 5);
    EXPECT_EQ(score::cpp::countl_one(std::uint8_t{0b11111000}), 5);
    EXPECT_EQ(score::cpp::countl_one(std::uint8_t{0b11110111}), 4);
    EXPECT_EQ(score::cpp::countl_one(std::uint8_t{0b11110000}), 4);
    EXPECT_EQ(score::cpp::countl_one(std::uint8_t{0b11101111}), 3);
    EXPECT_EQ(score::cpp::countl_one(std::uint8_t{0b11100000}), 3);
    EXPECT_EQ(score::cpp::countl_one(std::uint8_t{0b11011111}), 2);
    EXPECT_EQ(score::cpp::countl_one(std::uint8_t{0b11000000}), 2);
    EXPECT_EQ(score::cpp::countl_one(std::uint8_t{0b10111111}), 1);
    EXPECT_EQ(score::cpp::countl_one(std::uint8_t{0b10000000}), 1);
    EXPECT_EQ(score::cpp::countl_one(std::uint8_t{0b01111111}), 0);
    EXPECT_EQ(score::cpp::countl_one(std::uint8_t{0b00000000}), 0);

    EXPECT_EQ(score::cpp::countl_one(std::numeric_limits<std::uint16_t>::max()), 16);
    EXPECT_EQ(score::cpp::countl_one(std::uint16_t{0}), 0);

    EXPECT_EQ(score::cpp::countl_one(std::numeric_limits<std::uint32_t>::max()), 32);
    EXPECT_EQ(score::cpp::countl_one(0U), 0);

    EXPECT_EQ(score::cpp::countl_one(std::numeric_limits<std::uint64_t>::max()), 64);
    EXPECT_EQ(score::cpp::countl_one(0_UZ), 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(CountLOneTest, Constexpr)
{
    static_assert(score::cpp::countl_one(std::uint8_t{0b11111111}) == 8, "");
    static_assert(score::cpp::countl_one(std::uint8_t{0b11111110}) == 7, "");
    static_assert(score::cpp::countl_one(std::uint8_t{0b11111101}) == 6, "");
    static_assert(score::cpp::countl_one(std::uint8_t{0b11111100}) == 6, "");
    static_assert(score::cpp::countl_one(std::uint8_t{0b11111011}) == 5, "");
    static_assert(score::cpp::countl_one(std::uint8_t{0b11111000}) == 5, "");
    static_assert(score::cpp::countl_one(std::uint8_t{0b11110111}) == 4, "");
    static_assert(score::cpp::countl_one(std::uint8_t{0b11110000}) == 4, "");
    static_assert(score::cpp::countl_one(std::uint8_t{0b11101111}) == 3, "");
    static_assert(score::cpp::countl_one(std::uint8_t{0b11100000}) == 3, "");
    static_assert(score::cpp::countl_one(std::uint8_t{0b11011111}) == 2, "");
    static_assert(score::cpp::countl_one(std::uint8_t{0b11000000}) == 2, "");
    static_assert(score::cpp::countl_one(std::uint8_t{0b10111111}) == 1, "");
    static_assert(score::cpp::countl_one(std::uint8_t{0b10000000}) == 1, "");
    static_assert(score::cpp::countl_one(std::uint8_t{0b01111111}) == 0, "");
    static_assert(score::cpp::countl_one(std::uint8_t{0b00000000}) == 0, "");

    static_assert(score::cpp::countl_one(std::numeric_limits<std::uint16_t>::max()) == 16, "");
    static_assert(score::cpp::countl_one(std::uint16_t{0}) == 0, "");

    static_assert(score::cpp::countl_one(std::numeric_limits<std::uint32_t>::max()) == 32, "");
    static_assert(score::cpp::countl_one(0U) == 0, "");

    static_assert(score::cpp::countl_one(std::numeric_limits<std::uint64_t>::max()) == 64, "");
    static_assert(score::cpp::countl_one(0_UZ) == 0, "");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(CountRZeroTest, Spec)
{
    EXPECT_EQ(score::cpp::countr_zero(std::uint8_t{0b00000001}), 0);
    EXPECT_EQ(score::cpp::countr_zero(std::uint8_t{0b11111111}), 0);
    EXPECT_EQ(score::cpp::countr_zero(std::uint8_t{0b00000010}), 1);
    EXPECT_EQ(score::cpp::countr_zero(std::uint8_t{0b11111110}), 1);
    EXPECT_EQ(score::cpp::countr_zero(std::uint8_t{0b00000100}), 2);
    EXPECT_EQ(score::cpp::countr_zero(std::uint8_t{0b11111100}), 2);
    EXPECT_EQ(score::cpp::countr_zero(std::uint8_t{0b00001000}), 3);
    EXPECT_EQ(score::cpp::countr_zero(std::uint8_t{0b11111000}), 3);
    EXPECT_EQ(score::cpp::countr_zero(std::uint8_t{0b00010000}), 4);
    EXPECT_EQ(score::cpp::countr_zero(std::uint8_t{0b11110000}), 4);
    EXPECT_EQ(score::cpp::countr_zero(std::uint8_t{0b00100000}), 5);
    EXPECT_EQ(score::cpp::countr_zero(std::uint8_t{0b11100000}), 5);
    EXPECT_EQ(score::cpp::countr_zero(std::uint8_t{0b01000000}), 6);
    EXPECT_EQ(score::cpp::countr_zero(std::uint8_t{0b11000000}), 6);
    EXPECT_EQ(score::cpp::countr_zero(std::uint8_t{0b10000000}), 7);
    EXPECT_EQ(score::cpp::countr_zero(std::uint8_t{0b00000000}), 8);

    EXPECT_EQ(score::cpp::countr_zero(std::numeric_limits<std::uint16_t>::max()), 0);
    EXPECT_EQ(score::cpp::countr_zero(std::uint16_t{0}), 16);

    EXPECT_EQ(score::cpp::countr_zero(std::numeric_limits<std::uint32_t>::max()), 0);
    EXPECT_EQ(score::cpp::countr_zero(0U), 32);

    EXPECT_EQ(score::cpp::countr_zero(std::numeric_limits<std::uint64_t>::max()), 0);
    EXPECT_EQ(score::cpp::countr_zero(0_UZ), 64);

    EXPECT_EQ(score::cpp::countr_zero(std::numeric_limits<unsigned long long>::max()), 0);
    EXPECT_EQ(score::cpp::countr_zero(0ULL), std::numeric_limits<unsigned long long>::digits);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(CountRZeroTest, Constexpr)
{
    static_assert(score::cpp::countr_zero(std::uint8_t{0b00000001}) == 0, "");
    static_assert(score::cpp::countr_zero(std::uint8_t{0b11111111}) == 0, "");
    static_assert(score::cpp::countr_zero(std::uint8_t{0b00000010}) == 1, "");
    static_assert(score::cpp::countr_zero(std::uint8_t{0b11111110}) == 1, "");
    static_assert(score::cpp::countr_zero(std::uint8_t{0b00000100}) == 2, "");
    static_assert(score::cpp::countr_zero(std::uint8_t{0b11111100}) == 2, "");
    static_assert(score::cpp::countr_zero(std::uint8_t{0b00001000}) == 3, "");
    static_assert(score::cpp::countr_zero(std::uint8_t{0b11111000}) == 3, "");
    static_assert(score::cpp::countr_zero(std::uint8_t{0b00010000}) == 4, "");
    static_assert(score::cpp::countr_zero(std::uint8_t{0b11110000}) == 4, "");
    static_assert(score::cpp::countr_zero(std::uint8_t{0b00100000}) == 5, "");
    static_assert(score::cpp::countr_zero(std::uint8_t{0b11100000}) == 5, "");
    static_assert(score::cpp::countr_zero(std::uint8_t{0b01000000}) == 6, "");
    static_assert(score::cpp::countr_zero(std::uint8_t{0b11000000}) == 6, "");
    static_assert(score::cpp::countr_zero(std::uint8_t{0b10000000}) == 7, "");
    static_assert(score::cpp::countr_zero(std::uint8_t{0b00000000}) == 8, "");

    static_assert(score::cpp::countr_zero(std::numeric_limits<std::uint16_t>::max()) == 0, "");
    static_assert(score::cpp::countr_zero(std::uint16_t{0}) == 16, "");

    static_assert(score::cpp::countr_zero(std::numeric_limits<std::uint32_t>::max()) == 0, "");
    static_assert(score::cpp::countr_zero(0U) == 32, "");

    static_assert(score::cpp::countr_zero(std::numeric_limits<std::uint64_t>::max()) == 0, "");
    static_assert(score::cpp::countr_zero(0_UZ) == 64, "");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(CountROneTest, Spec)
{
    EXPECT_EQ(score::cpp::countr_one(std::uint8_t{0b00000000}), 0);
    EXPECT_EQ(score::cpp::countr_one(std::uint8_t{0b11111110}), 0);
    EXPECT_EQ(score::cpp::countr_one(std::uint8_t{0b00000001}), 1);
    EXPECT_EQ(score::cpp::countr_one(std::uint8_t{0b11111101}), 1);
    EXPECT_EQ(score::cpp::countr_one(std::uint8_t{0b00000011}), 2);
    EXPECT_EQ(score::cpp::countr_one(std::uint8_t{0b11111011}), 2);
    EXPECT_EQ(score::cpp::countr_one(std::uint8_t{0b00000111}), 3);
    EXPECT_EQ(score::cpp::countr_one(std::uint8_t{0b11110111}), 3);
    EXPECT_EQ(score::cpp::countr_one(std::uint8_t{0b00001111}), 4);
    EXPECT_EQ(score::cpp::countr_one(std::uint8_t{0b11101111}), 4);
    EXPECT_EQ(score::cpp::countr_one(std::uint8_t{0b00011111}), 5);
    EXPECT_EQ(score::cpp::countr_one(std::uint8_t{0b11011111}), 5);
    EXPECT_EQ(score::cpp::countr_one(std::uint8_t{0b00111111}), 6);
    EXPECT_EQ(score::cpp::countr_one(std::uint8_t{0b10111111}), 6);
    EXPECT_EQ(score::cpp::countr_one(std::uint8_t{0b01111111}), 7);
    EXPECT_EQ(score::cpp::countr_one(std::uint8_t{0b11111111}), 8);

    EXPECT_EQ(score::cpp::countr_one(std::uint16_t{0}), 0);
    EXPECT_EQ(score::cpp::countr_one(std::numeric_limits<std::uint16_t>::max()), 16);

    EXPECT_EQ(score::cpp::countr_one(0U), 0);
    EXPECT_EQ(score::cpp::countr_one(std::numeric_limits<std::uint32_t>::max()), 32);

    EXPECT_EQ(score::cpp::countr_one(0_UZ), 0);
    EXPECT_EQ(score::cpp::countr_one(std::numeric_limits<std::uint64_t>::max()), 64);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(CountROneTest, Constexpr)
{
    static_assert(score::cpp::countr_one(std::uint8_t{0b00000000}) == 0, "");
    static_assert(score::cpp::countr_one(std::uint8_t{0b11111110}) == 0, "");
    static_assert(score::cpp::countr_one(std::uint8_t{0b00000001}) == 1, "");
    static_assert(score::cpp::countr_one(std::uint8_t{0b11111101}) == 1, "");
    static_assert(score::cpp::countr_one(std::uint8_t{0b00000011}) == 2, "");
    static_assert(score::cpp::countr_one(std::uint8_t{0b11111011}) == 2, "");
    static_assert(score::cpp::countr_one(std::uint8_t{0b00000111}) == 3, "");
    static_assert(score::cpp::countr_one(std::uint8_t{0b11110111}) == 3, "");
    static_assert(score::cpp::countr_one(std::uint8_t{0b00001111}) == 4, "");
    static_assert(score::cpp::countr_one(std::uint8_t{0b11101111}) == 4, "");
    static_assert(score::cpp::countr_one(std::uint8_t{0b00011111}) == 5, "");
    static_assert(score::cpp::countr_one(std::uint8_t{0b11011111}) == 5, "");
    static_assert(score::cpp::countr_one(std::uint8_t{0b00111111}) == 6, "");
    static_assert(score::cpp::countr_one(std::uint8_t{0b10111111}) == 6, "");
    static_assert(score::cpp::countr_one(std::uint8_t{0b01111111}) == 7, "");
    static_assert(score::cpp::countr_one(std::uint8_t{0b11111111}) == 8, "");

    static_assert(score::cpp::countr_one(std::uint16_t{0}) == 0, "");
    static_assert(score::cpp::countr_one(std::numeric_limits<std::uint16_t>::max()) == 16, "");

    static_assert(score::cpp::countr_one(0U) == 0, "");
    static_assert(score::cpp::countr_one(std::numeric_limits<std::uint32_t>::max()) == 32, "");

    static_assert(score::cpp::countr_one(0_UZ) == 0, "");
    static_assert(score::cpp::countr_one(std::numeric_limits<std::uint64_t>::max()) == 64, "");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(PopcountTest, Spec)
{
    EXPECT_EQ(score::cpp::popcount(std::uint8_t{0b00000000}), 0);
    EXPECT_EQ(score::cpp::popcount(std::uint8_t{0b00010000}), 1);
    EXPECT_EQ(score::cpp::popcount(std::uint8_t{0b00010100}), 2);
    EXPECT_EQ(score::cpp::popcount(std::uint8_t{0b00101010}), 3);
    EXPECT_EQ(score::cpp::popcount(std::uint8_t{0b00111100}), 4);
    EXPECT_EQ(score::cpp::popcount(std::uint8_t{0b00111011}), 5);
    EXPECT_EQ(score::cpp::popcount(std::uint8_t{0b11110101}), 6);
    EXPECT_EQ(score::cpp::popcount(std::uint8_t{0b11110111}), 7);
    EXPECT_EQ(score::cpp::popcount(std::uint8_t{0b11111111}), 8);

    EXPECT_EQ(score::cpp::popcount(std::uint16_t{0}), 0);
    EXPECT_EQ(score::cpp::popcount(static_cast<std::uint16_t>(1U << 15)), 1);
    EXPECT_EQ(score::cpp::popcount(std::numeric_limits<std::uint16_t>::max()), 16);

    EXPECT_EQ(score::cpp::popcount(0U), 0);
    EXPECT_EQ(score::cpp::popcount(1U << 31), 1);
    EXPECT_EQ(score::cpp::popcount(std::numeric_limits<std::uint32_t>::max()), 32);

    EXPECT_EQ(score::cpp::popcount(0_UZ), 0);
    EXPECT_EQ(score::cpp::popcount(1_UZ << 63), 1);
    EXPECT_EQ(score::cpp::popcount(std::numeric_limits<std::uint64_t>::max()), 64);

    const int ull_digits{std::numeric_limits<unsigned long long>::digits};
    EXPECT_EQ(score::cpp::popcount(0ULL), 0);
    EXPECT_EQ(score::cpp::popcount(1ULL << (ull_digits - 1)), 1);
    EXPECT_EQ(score::cpp::popcount(std::numeric_limits<unsigned long long>::max()), ull_digits);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039075
TEST(PopcountTest, Constexpr)
{
    static_assert(score::cpp::popcount(std::uint8_t{0b00000000}) == 0, "");
    static_assert(score::cpp::popcount(std::uint8_t{0b00010000}) == 1, "");
    static_assert(score::cpp::popcount(std::uint8_t{0b00010100}) == 2, "");
    static_assert(score::cpp::popcount(std::uint8_t{0b00101010}) == 3, "");
    static_assert(score::cpp::popcount(std::uint8_t{0b00111100}) == 4, "");
    static_assert(score::cpp::popcount(std::uint8_t{0b00111011}) == 5, "");
    static_assert(score::cpp::popcount(std::uint8_t{0b11110101}) == 6, "");
    static_assert(score::cpp::popcount(std::uint8_t{0b11110111}) == 7, "");
    static_assert(score::cpp::popcount(std::uint8_t{0b11111111}) == 8, "");

    static_assert(score::cpp::popcount(std::uint16_t{0}) == 0, "");
    static_assert(score::cpp::popcount(static_cast<std::uint16_t>(1U << 15)) == 1, "");
    static_assert(score::cpp::popcount(std::numeric_limits<std::uint16_t>::max()) == 16, "");

    static_assert(score::cpp::popcount(0U) == 0, "");
    static_assert(score::cpp::popcount(1U << 31) == 1, "");
    static_assert(score::cpp::popcount(std::numeric_limits<std::uint32_t>::max()) == 32, "");

    static_assert(score::cpp::popcount(0_UZ) == 0, "");
    static_assert(score::cpp::popcount(1_UZ << 63) == 1, "");
    static_assert(score::cpp::popcount(std::numeric_limits<std::uint64_t>::max()) == 64, "");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039104
TEST(AlignUpTest, WhenSecondArgumentIsNotAPowerOfTwoThenAlignUpIsNotDefined)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::align_up(0_UZ, 3_UZ));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::align_up(0U, 3U));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::align_up<std::uint16_t>(1U, 3U));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::align_up<std::uint8_t>(2U, 3U));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039104
TEST(AlignUpTest, WhenResultIsNotRepresentableThenAlignUpIsNotDefined)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::align_up((1_UZ << 63U) + 1U, 1_UZ << 63U));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::align_up((1U << 31U) + 1U, 1U << 31U));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::align_up<std::uint16_t>((1U << 15U) + 1U, 1U << 15U));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::align_up<std::uint8_t>((1U << 7U) + 1U, 1U << 7U));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::align_up<std::uint8_t>(253U, 4U));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::align_up<std::uint8_t>(255U, 2U));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039104
TEST(AlignUpTest, Spec)
{
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(0U, 1U), 0U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(1U, 1U), 1U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(2U, 1U), 2U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(3U, 1U), 3U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(253U, 1U), 253U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(254U, 1U), 254U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(255U, 1U), 255U);

    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(0U, 2U), 0U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(1U, 2U), 2U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(2U, 2U), 2U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(3U, 2U), 4U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(252U, 2U), 252U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(253U, 2U), 254U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(254U, 2U), 254U);

    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(0U, 4U), 0U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(1U, 4U), 4U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(2U, 4U), 4U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(3U, 4U), 4U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(4U, 4U), 4U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(5U, 4U), 8U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(248U, 4U), 248U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(249U, 4U), 252U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(250U, 4U), 252U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(251U, 4U), 252U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(252U, 4U), 252U);

    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(0U, 128U), 0U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(1U, 128U), 128U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(2U, 128U), 128U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(127U, 128U), 128U);
    EXPECT_EQ(score::cpp::align_up<std::uint8_t>(128U, 128U), 128U);

    EXPECT_EQ(score::cpp::align_up<std::uint16_t>((1U << 7U) + 1U, 1U << 7U), 1U << 8U);
    EXPECT_EQ(score::cpp::align_up<std::uint16_t>(0U, 1U << 15U), 0U);
    EXPECT_EQ(score::cpp::align_up<std::uint16_t>(1U, 1U << 15U), 1U << 15U);
    EXPECT_EQ(score::cpp::align_up<std::uint16_t>(2U, 1U << 15U), 1U << 15U);
    EXPECT_EQ(score::cpp::align_up<std::uint16_t>((1U << 15U) - 1U, 1U << 15U), 1U << 15U);
    EXPECT_EQ(score::cpp::align_up<std::uint16_t>(1U << 15U, 1U << 15U), 1U << 15U);

    EXPECT_EQ(score::cpp::align_up((1U << 15U) + 1U, 1U << 15U), 1U << 16U);
    EXPECT_EQ(score::cpp::align_up(0U, 1U << 31U), 0U);
    EXPECT_EQ(score::cpp::align_up(1U, 1U << 31U), (1U << 31U));
    EXPECT_EQ(score::cpp::align_up(2U, 1U << 31U), (1U << 31U));
    EXPECT_EQ(score::cpp::align_up((1U << 31U) - 1U, 1U << 31U), (1U << 31U));
    EXPECT_EQ(score::cpp::align_up((1U << 31U), 1U << 31U), (1U << 31U));

    EXPECT_EQ(score::cpp::align_up((1_UZ << 31U) + 1U, 1_UZ << 31U), 1_UZ << 32U);
    EXPECT_EQ(score::cpp::align_up(0_UZ, 1_UZ << 63U), 0_UZ);
    EXPECT_EQ(score::cpp::align_up(1_UZ, 1_UZ << 63U), (1_UZ << 63U));
    EXPECT_EQ(score::cpp::align_up(2_UZ, 1_UZ << 63U), (1_UZ << 63U));
    EXPECT_EQ(score::cpp::align_up((1_UZ << 63U) - 1U, 1_UZ << 63U), (1_UZ << 63U));
    EXPECT_EQ(score::cpp::align_up((1_UZ << 63U), 1_UZ << 63U), (1_UZ << 63U));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039104
TEST(AlignUpTest, Constexpr)
{
    static_assert(score::cpp::align_up<std::uint8_t>(0U, 1U) == 0U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(1U, 1U) == 1U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(2U, 1U) == 2U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(3U, 1U) == 3U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(253U, 1U) == 253U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(254U, 1U) == 254U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(255U, 1U) == 255U, "");

    static_assert(score::cpp::align_up<std::uint8_t>(0U, 2U) == 0U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(1U, 2U) == 2U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(2U, 2U) == 2U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(3U, 2U) == 4U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(252U, 2U) == 252U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(253U, 2U) == 254U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(254U, 2U) == 254U, "");

    static_assert(score::cpp::align_up<std::uint8_t>(0U, 4U) == 0U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(1U, 4U) == 4U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(2U, 4U) == 4U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(3U, 4U) == 4U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(4U, 4U) == 4U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(5U, 4U) == 8U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(248U, 4U) == 248U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(249U, 4U) == 252U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(250U, 4U) == 252U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(251U, 4U) == 252U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(252U, 4U) == 252U, "");

    static_assert(score::cpp::align_up<std::uint8_t>(0U, 128U) == 0U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(1U, 128U) == 128U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(2U, 128U) == 128U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(127U, 128U) == 128U, "");
    static_assert(score::cpp::align_up<std::uint8_t>(128U, 128U) == 128U, "");

    static_assert(score::cpp::align_up<std::uint16_t>((1U << 7U) + 1U, 1U << 7U) == (1U << 8U), "");
    static_assert(score::cpp::align_up<std::uint16_t>(0U, 1U << 15U) == 0U, "");
    static_assert(score::cpp::align_up<std::uint16_t>(1U, 1U << 15U) == 1U << 15U, "");
    static_assert(score::cpp::align_up<std::uint16_t>(2U, 1U << 15U) == 1U << 15U, "");
    static_assert(score::cpp::align_up<std::uint16_t>((1U << 15U) - 1U, 1U << 15U) == 1U << 15U, "");
    static_assert(score::cpp::align_up<std::uint16_t>(1U << 15U, 1U << 15U) == 1U << 15U, "");

    static_assert(score::cpp::align_up((1U << 15U) + 1U, 1U << 15U) == (1U << 16U), "");
    static_assert(score::cpp::align_up(0U, 1U << 31U) == 0U, "");
    static_assert(score::cpp::align_up(1U, 1U << 31U) == (1U << 31U), "");
    static_assert(score::cpp::align_up(2U, 1U << 31U) == (1U << 31U), "");
    static_assert(score::cpp::align_up((1U << 31U) - 1U, 1U << 31U) == (1U << 31U), "");
    static_assert(score::cpp::align_up((1U << 31U), 1U << 31U) == (1U << 31U), "");

    static_assert(score::cpp::align_up((1_UZ << 31U) + 1U, 1_UZ << 31U) == 1_UZ << 32U, "");
    static_assert(score::cpp::align_up(0_UZ, 1_UZ << 63U) == 0_UZ, "");
    static_assert(score::cpp::align_up(1_UZ, 1_UZ << 63U) == (1_UZ << 63U), "");
    static_assert(score::cpp::align_up(2_UZ, 1_UZ << 63U) == (1_UZ << 63U), "");
    static_assert(score::cpp::align_up((1_UZ << 63U) - 1U, 1_UZ << 63U) == (1_UZ << 63U), "");
    static_assert(score::cpp::align_up((1_UZ << 63U), 1_UZ << 63U) == (1_UZ << 63U), "");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039104
TEST(AlignDownTest, WhenSecondArgumentIsNotAPowerOfTwoThenAlignDownIsNotDefined)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::align_down(0_UZ, 3_UZ));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::align_down(0U, 3U));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::align_down<std::uint16_t>(1U, 3U));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::align_down<std::uint8_t>(2U, 3U));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039104
TEST(AlignDownTest, Spec)
{
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(0U, 1U), 0U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(1U, 1U), 1U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(2U, 1U), 2U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(3U, 1U), 3U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(253U, 1U), 253U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(254U, 1U), 254U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(255U, 1U), 255U);

    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(0U, 2U), 0U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(1U, 2U), 0U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(2U, 2U), 2U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(3U, 2U), 2U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(252U, 2U), 252U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(253U, 2U), 252U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(254U, 2U), 254U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(255U, 2U), 254U);

    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(0U, 4U), 0U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(1U, 4U), 0U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(2U, 4U), 0U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(3U, 4U), 0U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(4U, 4U), 4U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(5U, 4U), 4U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(251U, 4U), 248U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(252U, 4U), 252U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(253U, 4U), 252U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(254U, 4U), 252U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(255U, 4U), 252U);

    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(0U, 128U), 0U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(1U, 128U), 0U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(2U, 128U), 0U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(127U, 128U), 0U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(128U, 128U), 128U);
    EXPECT_EQ(score::cpp::align_down<std::uint8_t>(255U, 128U), 128U);

    EXPECT_EQ(score::cpp::align_down<std::uint16_t>(1U << 8U, 1U << 8U), (1U << 8U));
    EXPECT_EQ(score::cpp::align_down<std::uint16_t>(0U, 1U << 15U), 0U);
    EXPECT_EQ(score::cpp::align_down<std::uint16_t>(1U, 1U << 15U), 0U);
    EXPECT_EQ(score::cpp::align_down<std::uint16_t>(2U, 1U << 15U), 0U);
    EXPECT_EQ(score::cpp::align_down<std::uint16_t>((1U << 15U) - 1U, 1U << 15U), 0U);
    EXPECT_EQ(score::cpp::align_down<std::uint16_t>((1U << 15U), 1U << 15U), (1U << 15U));
    EXPECT_EQ(score::cpp::align_down<std::uint16_t>(std::numeric_limits<std::uint16_t>::max(), 1U << 15U), (1U << 15U));

    EXPECT_EQ(score::cpp::align_down(1U << 16U, 1U << 16U), (1U << 16U));
    EXPECT_EQ(score::cpp::align_down(0U, 1U << 31U), 0U);
    EXPECT_EQ(score::cpp::align_down(1U, 1U << 31U), 0U);
    EXPECT_EQ(score::cpp::align_down(2U, 1U << 31U), 0U);
    EXPECT_EQ(score::cpp::align_down((1U << 31U) - 1U, 1U << 31U), 0U);
    EXPECT_EQ(score::cpp::align_down((1U << 31U), 1U << 31U), (1U << 31U));
    EXPECT_EQ(score::cpp::align_down(std::numeric_limits<std::uint32_t>::max(), 1U << 31U), (1U << 31U));

    EXPECT_EQ(score::cpp::align_down(1_UZ << 32U, 1_UZ << 32U), (1_UZ << 32U));
    EXPECT_EQ(score::cpp::align_down(0_UZ, 1_UZ << 63U), 0_UZ);
    EXPECT_EQ(score::cpp::align_down(1_UZ, 1_UZ << 63U), 0_UZ);
    EXPECT_EQ(score::cpp::align_down(2_UZ, 1_UZ << 63U), 0_UZ);
    EXPECT_EQ(score::cpp::align_down((1_UZ << 63U) - 1U, 1_UZ << 63U), 0_UZ);
    EXPECT_EQ(score::cpp::align_down((1_UZ << 63U), 1_UZ << 63U), (1_UZ << 63U));
    EXPECT_EQ(score::cpp::align_down(std::numeric_limits<std::uint64_t>::max(), 1_UZ << 63U), (1_UZ << 63U));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#10039104
TEST(AlignDownTest, Constexpr)
{
    static_assert(score::cpp::align_down<std::uint8_t>(0U, 1U) == 0U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(1U, 1U) == 1U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(2U, 1U) == 2U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(3U, 1U) == 3U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(253U, 1U) == 253U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(254U, 1U) == 254U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(255U, 1U) == 255U, "");

    static_assert(score::cpp::align_down<std::uint8_t>(0U, 2U) == 0U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(1U, 2U) == 0U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(2U, 2U) == 2U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(3U, 2U) == 2U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(252U, 2U) == 252U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(253U, 2U) == 252U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(254U, 2U) == 254U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(255U, 2U) == 254U, "");

    static_assert(score::cpp::align_down<std::uint8_t>(0U, 4U) == 0U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(1U, 4U) == 0U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(2U, 4U) == 0U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(3U, 4U) == 0U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(4U, 4U) == 4U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(5U, 4U) == 4U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(251U, 4U) == 248U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(252U, 4U) == 252U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(253U, 4U) == 252U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(254U, 4U) == 252U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(255U, 4U) == 252U, "");

    static_assert(score::cpp::align_down<std::uint8_t>(0U, 128U) == 0U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(1U, 128U) == 0U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(2U, 128U) == 0U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(127U, 128U) == 0U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(128U, 128U) == 128U, "");
    static_assert(score::cpp::align_down<std::uint8_t>(255U, 128U) == 128U, "");

    static_assert(score::cpp::align_down<std::uint16_t>(1U << 8U, 1U << 8U) == (1U << 8U), "");
    static_assert(score::cpp::align_down<std::uint16_t>(0U, 1U << 15U) == 0U, "");
    static_assert(score::cpp::align_down<std::uint16_t>(1U, 1U << 15U) == 0U, "");
    static_assert(score::cpp::align_down<std::uint16_t>(2U, 1U << 15U) == 0U, "");
    static_assert(score::cpp::align_down<std::uint16_t>((1U << 15U) - 1U, 1U << 15U) == 0U, "");
    static_assert(score::cpp::align_down<std::uint16_t>((1U << 15U), 1U << 15U) == (1U << 15U), "");
    static_assert(score::cpp::align_down<std::uint16_t>(std::numeric_limits<std::uint16_t>::max(), 1U << 15U) == (1U << 15U),
                  "");

    static_assert(score::cpp::align_down(1U << 16U, 1U << 16U) == (1U << 16U), "");
    static_assert(score::cpp::align_down(0U, 1U << 31U) == 0U, "");
    static_assert(score::cpp::align_down(1U, 1U << 31U) == 0U, "");
    static_assert(score::cpp::align_down(2U, 1U << 31U) == 0U, "");
    static_assert(score::cpp::align_down((1U << 31U) - 1U, 1U << 31U) == 0U, "");
    static_assert(score::cpp::align_down((1U << 31U), 1U << 31U) == (1U << 31U), "");
    static_assert(score::cpp::align_down(std::numeric_limits<std::uint32_t>::max(), 1U << 31U) == (1U << 31U), "");

    static_assert(score::cpp::align_down(1_UZ << 32U, 1_UZ << 32U) == (1_UZ << 32U), "");
    static_assert(score::cpp::align_down(0_UZ, 1_UZ << 63U) == 0_UZ, "");
    static_assert(score::cpp::align_down(1_UZ, 1_UZ << 63U) == 0_UZ, "");
    static_assert(score::cpp::align_down(2_UZ, 1_UZ << 63U) == 0_UZ, "");
    static_assert(score::cpp::align_down((1_UZ << 63U) - 1U, 1_UZ << 63U) == 0_UZ, "");
    static_assert(score::cpp::align_down((1_UZ << 63U), 1_UZ << 63U) == (1_UZ << 63U), "");
    static_assert(score::cpp::align_down(std::numeric_limits<std::uint64_t>::max(), 1_UZ << 63U) == (1_UZ << 63U), "");
}

} // namespace
