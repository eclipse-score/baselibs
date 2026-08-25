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
#include "score/bitmanipulation/bit_manipulation.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <cstdint>

namespace score
{
namespace platform
{
namespace
{

using ::testing::Eq;

TEST(HalfByte, CanBeConstructedFromUInt8)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__byte_operations");
    RecordProperty("Description",
                   "Check that a HalfByte can be constructed from a uint8_t value within its 4-bit range.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    constexpr HalfByte value{std::uint8_t{4u}};

    EXPECT_THAT(value, Eq(4u));
}

TEST(HalfByte, CanBeConstructedFromUInt16IfInRange)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__byte_operations");
    RecordProperty("Description",
                   "Check that a HalfByte can be constructed from a uint16_t value that fits within its 4-bit range.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    constexpr HalfByte value{std::uint16_t{4u}};

    EXPECT_THAT(value, Eq(4u));
}

TEST(HalfByte, CanBeConstructedFromBigUInt8ButUpperHalfIsDropped)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__byte_operations");
    RecordProperty("Description",
                   "Check that constructing a HalfByte from a uint8_t value larger than 4 bits drops the upper bits "
                   "and keeps only the lower nibble.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    constexpr HalfByte value{std::uint8_t{77u}};  // 01001101

    EXPECT_THAT(value, Eq(13u));
}

TEST(Byte, CanBeConstructedFromUInt8)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__byte_operations");
    RecordProperty("Description", "Check that a Byte can be constructed directly from a uint8_t value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    constexpr Byte value{std::uint8_t{4u}};

    EXPECT_THAT(value, Eq(4u));
}

TEST(Byte, CanBeConstructedFromUInt16IfInRange)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__byte_operations");
    RecordProperty("Description",
                   "Check that a Byte can be constructed from a uint16_t value that fits within a single byte.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    constexpr Byte value{std::uint16_t{4u}};

    EXPECT_THAT(value, Eq(4u));
}

TEST(Byte, CanBeConstructedFromTwoHalfBytes)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__byte_operations");
    RecordProperty("Description",
                   "Check that a Byte constructed from an upper and lower HalfByte concatenates them into the expected "
                   "combined value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    constexpr HalfByte upperHalfByte{std::uint8_t{13u}};  // 00001101
    constexpr HalfByte lowerHalfByte{std::uint8_t{4u}};   // 00000100

    constexpr Byte combinedByte{upperHalfByte, lowerHalfByte};

    EXPECT_THAT(combinedByte, Eq(212u));  // 11010100
}

TEST(Byte, CanBeConstructedFromTwoBigHalfBytes)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__byte_operations");
    RecordProperty("Description",
                   "Check that constructing a Byte from HalfBytes built from oversized values still yields the correct "
                   "combined byte, since only the lower nibble of each half is kept.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    constexpr HalfByte upperHalfByte{77u};  // 01001101
    constexpr HalfByte lowerHalfByte{36u};  // 00100100

    constexpr Byte combinedByte{upperHalfByte, lowerHalfByte};

    EXPECT_THAT(combinedByte, Eq(212u));  // 11010100
}

TEST(Byte, CanBeConstructedFromTwoZeroHalfBytes)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__byte_operations");
    RecordProperty("Description", "Check that a Byte constructed from two zero HalfBytes results in the value zero.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    constexpr HalfByte zeroUpperHalfByte{0};  // 00000000
    constexpr HalfByte zeroLowerHalfByte{0};  // 00000000

    constexpr Byte combinedByte{zeroUpperHalfByte, zeroLowerHalfByte};

    EXPECT_THAT(combinedByte, Eq(0));  // 00000000
}

TEST(Byte, CanBeConstructedFromTwoHalfBytesWithMaxValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__byte_operations");
    RecordProperty("Description",
                   "Check that a Byte constructed from two max-value HalfBytes results in the value 255.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    constexpr HalfByte halfByteWithMaxValue{255};  // 11111111

    constexpr Byte combinedByte{halfByteWithMaxValue, halfByteWithMaxValue};

    EXPECT_THAT(combinedByte, Eq(255));  // 11111111
}

TEST(Extract, UpperHalfByteFromAByte)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__byte_operations");
    RecordProperty("Description", "Check that UpperHalfByte extracts the upper nibble of a Byte.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    constexpr Byte input{212u};  // 11010100

    constexpr auto upperHalfByte = input.UpperHalfByte();

    EXPECT_THAT(upperHalfByte, Eq(13));  // 00001101
}

TEST(Extract, LowerHalfByteFromAByte)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__byte_operations");
    RecordProperty("Description", "Check that LowerHalfByte extracts the lower nibble of a Byte.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    constexpr Byte input{212};  // 11010100

    constexpr auto lowerHalfByte = input.LowerHalfByte();

    EXPECT_THAT(lowerHalfByte, Eq(4));  // 00000100
}

TEST(Extract, LowerHalfByteFromAByteCanBeConvertedToUInt8)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__byte_operations");
    RecordProperty("Description",
                   "Check that a HalfByte extracted from a Byte converts implicitly to its underlying uint8_t value.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    constexpr Byte input{212};  // 11010100

    const std::uint8_t lowerHalfByteAsUInt8{input.LowerHalfByte()};

    EXPECT_THAT(lowerHalfByteAsUInt8, Eq(4));  // 00000100
}

TEST(SetBit, WithUInt8)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bit_operations");
    RecordProperty("Description", "Check that SetBit sets the requested bit of a uint8_t value and returns true.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    std::uint8_t input{0};                     // 00000000
    constexpr std::uint8_t expectedResult{4};  // 00000100

    EXPECT_TRUE(::score::platform::SetBit(input, 2U));

    EXPECT_EQ(input, expectedResult);
}

TEST(SetBit, WithUInt64)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bit_operations");
    RecordProperty(
        "Description",
        "Check that SetBit sets the requested bit of a uint64_t value, supporting integral types up to 64 bits.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    // 00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000000
    std::uint64_t input{0};
    // 00000000'00000000'00000100'00000000'00000000'00000000'00000000'00000000
    constexpr std::uint64_t expectedResult{4398046511104ULL};

    EXPECT_TRUE(::score::platform::SetBit(input, 42U));

    EXPECT_EQ(input, expectedResult);
}

TEST(SetBit, OverflowWithUInt8)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bounds_safety");
    RecordProperty("Description",
                   "Check that SetBit returns false and leaves the value unmodified when the bit position exceeds "
                   "the width of a uint8_t value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    constexpr std::uint8_t expectedResult{0b1011'0101U};  // must stay unchanged
    std::uint8_t input{expectedResult};

    EXPECT_FALSE(::score::platform::SetBit(input, 8U));
    EXPECT_EQ(input, expectedResult);

    EXPECT_FALSE(::score::platform::SetBit(input, 10U));
    EXPECT_EQ(input, expectedResult);
}

TEST(ClearBit, WithUInt8)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bit_operations");
    RecordProperty("Description", "Check that ClearBit clears the requested bit of a uint8_t value and returns true.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    std::uint8_t input{24};                    // 00011000
    constexpr std::uint8_t expectedResult{8};  // 00001000

    EXPECT_TRUE(::score::platform::ClearBit(input, 4U));

    EXPECT_EQ(input, expectedResult);
}

TEST(ClearBit, WithUInt64)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bit_operations");
    RecordProperty(
        "Description",
        "Check that ClearBit clears the requested bit of a uint64_t value, supporting integral types up to 64 bits.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    // 00000000'00000000'00011000'00000000'00000000'00000000'00000000'00000000
    std::uint64_t input{26388279066624ULL};
    // 00000000'00000000'00001000'00000000'00000000'00000000'00000000'00000000
    constexpr std::uint64_t expectedResult{8796093022208ULL};

    EXPECT_TRUE(::score::platform::ClearBit(input, 44U));

    EXPECT_EQ(input, expectedResult);
}

TEST(ClearBit, OverflowWithUInt8)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bounds_safety");
    RecordProperty("Description",
                   "Check that ClearBit returns false and leaves the value unmodified when the bit position exceeds "
                   "the width of a uint8_t value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    constexpr std::uint8_t expectedResult{0b1011'0101U};  // must stay unchanged
    std::uint8_t input{expectedResult};

    EXPECT_FALSE(::score::platform::ClearBit(input, 8U));
    EXPECT_EQ(input, expectedResult);

    EXPECT_FALSE(::score::platform::ClearBit(input, 10U));
    EXPECT_EQ(input, expectedResult);
}

TEST(ToggleBit, WithUInt8)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bit_operations");
    RecordProperty("Description", "Check that ToggleBit flips the requested bits of a uint8_t value and returns true.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    std::uint8_t input{24};                     // 00011000
    constexpr std::uint8_t expectedResult{20};  // 00010100

    EXPECT_TRUE(::score::platform::ToggleBit(input, 2U));
    ::score::platform::ToggleBit(input, 3U);

    EXPECT_EQ(input, expectedResult);
}

TEST(ToggleBit, WithUInt64)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bit_operations");
    RecordProperty(
        "Description",
        "Check that ToggleBit flips the requested bits of a uint64_t value, supporting integral types up to 64 bits.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    // 00000000'00000000'00011000'00000000'00000000'00000000'00000000'00000000
    std::uint64_t input{26388279066624ULL};
    // 00000000'00000000'00010100'00000000'00000000'00000000'00000000'00000000
    constexpr std::uint64_t expectedResult{21990232555520ULL};

    EXPECT_TRUE(::score::platform::ToggleBit(input, 42U));
    ::score::platform::ToggleBit(input, 43U);

    EXPECT_EQ(input, expectedResult);
}

TEST(ToggleBit, OverflowWithUInt8)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bounds_safety");
    RecordProperty("Description",
                   "Check that ToggleBit returns false and leaves the value unmodified when the bit position exceeds "
                   "the width of a uint8_t value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    constexpr std::uint8_t expectedResult{0b1011'0101U};  // must stay unchanged
    std::uint8_t input{expectedResult};

    EXPECT_FALSE(::score::platform::ToggleBit(input, 8U));
    EXPECT_EQ(input, expectedResult);

    EXPECT_FALSE(::score::platform::ToggleBit(input, 10U));
    EXPECT_EQ(input, expectedResult);
}

TEST(CheckBit, WithUInt8)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bit_operations");
    RecordProperty("Description",
                   "Check that CheckBit reports the correct set/unset state of bits in a uint8_t value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    std::uint8_t input{24};  // 00011000

    ASSERT_TRUE(::score::platform::CheckBit(input, 3U));
    ASSERT_FALSE(::score::platform::CheckBit(input, 2U));
}

TEST(CheckBit, WithUInt64)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bit_operations");
    RecordProperty("Description",
                   "Check that CheckBit reports the correct set/unset state of bits in a uint64_t value, supporting "
                   "integral types up to 64 bits.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    // 00000000'00000000'00011000'00000000'00000000'00000000'00000000'00000000
    std::uint64_t input{26388279066624ULL};

    ASSERT_TRUE(::score::platform::CheckBit(input, 43U));
    ASSERT_FALSE(::score::platform::CheckBit(input, 42U));
}

TEST(CheckBit, OverflowWithUInt8)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bounds_safety");
    RecordProperty("Description",
                   "Check that CheckBit returns false when the bit position exceeds the width of a uint8_t value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    std::uint8_t input{0};  // 00000000

    EXPECT_FALSE(::score::platform::CheckBit(input, 8U));
    EXPECT_FALSE(::score::platform::CheckBit(input, 10U));
}

TEST(GetByte, FromUint8)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__byte_operations");
    RecordProperty("Description", "Check that GetByte extracts the single byte of a uint8_t value unchanged.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    std::uint8_t input{42};
    const auto extracted_byte = ::score::platform::GetByte<0>(input);
    EXPECT_EQ(extracted_byte, input);
}

TEST(GetByte, FromInt8)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__byte_operations");
    RecordProperty("Description", "Check that GetByte extracts the single byte of a signed int8_t value unchanged.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::int8_t input{42};
    const auto extracted_byte = ::score::platform::GetByte<0>(input);
    EXPECT_EQ(extracted_byte, input);
}

TEST(GetByte, FromUint16)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__byte_operations");
    RecordProperty("Description", "Check that GetByte extracts the correct byte at each position of a uint16_t value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::uint16_t input{0x0A0B};

    EXPECT_EQ(::score::platform::GetByte<1>(input), 0x0A);
    EXPECT_EQ(::score::platform::GetByte<0>(input), 0x0B);
}

TEST(GetByte, FromInt16)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__byte_operations");
    RecordProperty("Description",
                   "Check that GetByte extracts the correct byte at each position of a signed int16_t value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::int16_t input{0x0A0B};

    EXPECT_EQ(::score::platform::GetByte<1>(input), 0x0A);
    EXPECT_EQ(::score::platform::GetByte<0>(input), 0x0B);
}

TEST(GetByte, FromUint32)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__byte_operations");
    RecordProperty("Description", "Check that GetByte extracts the correct byte at each position of a uint32_t value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::uint32_t input{0x0A0B0C0D};

    EXPECT_EQ(::score::platform::GetByte<3>(input), 0x0A);
    EXPECT_EQ(::score::platform::GetByte<2>(input), 0x0B);
    EXPECT_EQ(::score::platform::GetByte<1>(input), 0x0C);
    EXPECT_EQ(::score::platform::GetByte<0>(input), 0x0D);
}

TEST(GetByte, FromInt32)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__byte_operations");
    RecordProperty("Description",
                   "Check that GetByte extracts the correct byte at each position of a signed int32_t value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::int32_t input{0x0A0B0C0D};

    EXPECT_EQ(::score::platform::GetByte<3>(input), 0x0A);
    EXPECT_EQ(::score::platform::GetByte<2>(input), 0x0B);
    EXPECT_EQ(::score::platform::GetByte<1>(input), 0x0C);
    EXPECT_EQ(::score::platform::GetByte<0>(input), 0x0D);
}

TEST(GetByte, FromUint64)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__byte_operations");
    RecordProperty("Description",
                   "Check that GetByte extracts the correct byte at each position of a uint64_t value, the widest "
                   "supported integral type.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    std::uint64_t input{0x0A0B0C0D04030201};

    EXPECT_EQ(::score::platform::GetByte<7>(input), 0x0A);
    EXPECT_EQ(::score::platform::GetByte<6>(input), 0x0B);
    EXPECT_EQ(::score::platform::GetByte<5>(input), 0x0C);
    EXPECT_EQ(::score::platform::GetByte<4>(input), 0x0D);
    EXPECT_EQ(::score::platform::GetByte<3>(input), 0x04);
    EXPECT_EQ(::score::platform::GetByte<2>(input), 0x03);
    EXPECT_EQ(::score::platform::GetByte<1>(input), 0x02);
    EXPECT_EQ(::score::platform::GetByte<0>(input), 0x01);
}

TEST(GetByte, FromInt64)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__byte_operations");
    RecordProperty("Description",
                   "Check that GetByte extracts the correct byte at each position of a signed int64_t value, the "
                   "widest supported integral type.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    std::int64_t input{0x0A0B0C0D04030201};

    EXPECT_EQ(::score::platform::GetByte<7>(input), 0x0A);
    EXPECT_EQ(::score::platform::GetByte<6>(input), 0x0B);
    EXPECT_EQ(::score::platform::GetByte<5>(input), 0x0C);
    EXPECT_EQ(::score::platform::GetByte<4>(input), 0x0D);
    EXPECT_EQ(::score::platform::GetByte<3>(input), 0x04);
    EXPECT_EQ(::score::platform::GetByte<2>(input), 0x03);
    EXPECT_EQ(::score::platform::GetByte<1>(input), 0x02);
    EXPECT_EQ(::score::platform::GetByte<0>(input), 0x01);
}

}  // namespace
}  // namespace platform
}  // namespace score
