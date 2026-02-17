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
#include "score/mw/log/detail/text_recorder/text_format.h"
#include "score/mw/log/detail/verbose_payload.h"

#include "gtest/gtest.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
namespace test
{
namespace
{

class TextFormatFixture : public ::testing::Test
{
  public:
    ByteVector buffer{};
    VerbosePayload payload{100, buffer};
    ByteVector zero_sized_buffer{};
    VerbosePayload depleted_payload{0, zero_sized_buffer};
    ByteVector size_two_buffer{};
    VerbosePayload capacity_two_payload{2, size_two_buffer};
};

template <typename T>
class UnsupportedTypesCoverage : public testing::Test
{
  public:
    ByteVector buffer{};
    VerbosePayload payload{100, buffer};
    T value = 123;
};

using UnsupportedTypes = ::testing::Types<int8_t, int16_t, int32_t, int64_t>;
TYPED_TEST_SUITE(UnsupportedTypesCoverage, UnsupportedTypes, /*unused*/);

TYPED_TEST(UnsupportedTypesCoverage, VerifyUnsupportedTypesActionsHex)
{
    ::testing::Test::RecordProperty("ParentRequirement", "SCR-1633236");
    ::testing::Test::RecordProperty("ASIL", "B");
    ::testing::Test::RecordProperty(
        "Description", "Verifies Type-Information for integer values with hex representation can not be logged.");
    ::testing::Test::RecordProperty("TestingTechnique", "Requirements-based test");
    ::testing::Test::RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(this->payload, this->value, IntegerRepresentation::kHex);
    EXPECT_EQ(this->buffer.size(), 0);
}

TYPED_TEST(UnsupportedTypesCoverage, VerifyUnsupportedTypesActionsOctal)
{
    ::testing::Test::RecordProperty("ParentRequirement", "SCR-1633236");
    ::testing::Test::RecordProperty("ASIL", "B");
    ::testing::Test::RecordProperty(
        "Description", "Verifies Type-Information for integer values with octal representation can not be logged.");
    ::testing::Test::RecordProperty("TestingTechnique", "Requirements-based test");
    ::testing::Test::RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(this->payload, this->value, IntegerRepresentation::kOctal);
    EXPECT_EQ(this->buffer.size(), 0);
}

TYPED_TEST(UnsupportedTypesCoverage, VerifyUnsupportedTypesActionsBin)
{
    ::testing::Test::RecordProperty("ParentRequirement", "SCR-1633236");
    ::testing::Test::RecordProperty("ASIL", "B");
    ::testing::Test::RecordProperty(
        "Description", "Verifies Type-Information for integer values with binary representation can not be logged.");
    ::testing::Test::RecordProperty("TestingTechnique", "Requirements-based test");
    ::testing::Test::RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(this->payload, this->value, IntegerRepresentation::kBinary);
    EXPECT_EQ(this->buffer.size(), 0);
}

TEST_F(TextFormatFixture, DepletedBufferPassed)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies no log can be set for a zero buffer size.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(depleted_payload, std::int32_t{123U});

    EXPECT_EQ(zero_sized_buffer.size(), 0);
}

TEST_F(TextFormatFixture, PositiveValueForBool)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for a positive value with bool in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, bool{true});

    EXPECT_EQ(buffer.at(0), 'T');
    EXPECT_EQ(buffer.at(1), 'r');
    EXPECT_EQ(buffer.at(2), 'u');
    EXPECT_EQ(buffer.at(3), 'e');
    EXPECT_EQ(buffer.at(4), ' ');
    EXPECT_EQ(buffer.size(), 5);
}

TEST_F(TextFormatFixture, NegativeValueForBool)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for a negative value with bool in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, bool{false});

    EXPECT_EQ(buffer.at(0), 'F');
    EXPECT_EQ(buffer.at(1), 'a');
    EXPECT_EQ(buffer.at(2), 'l');
    EXPECT_EQ(buffer.at(3), 's');
    EXPECT_EQ(buffer.at(4), 'e');
    EXPECT_EQ(buffer.at(5), ' ');
    EXPECT_EQ(buffer.size(), 6);
}

TEST_F(TextFormatFixture, PositiveValueOnBufferFull)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for a positive value with int64 size in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(capacity_two_payload, std::string_view{"xxx"});
    TextFormat::Log(capacity_two_payload, std::int8_t{123U});

    //  Buffer content not changed by second insertion:
    EXPECT_EQ(size_two_buffer.at(0), 'x');
    EXPECT_EQ(size_two_buffer.at(1), ' ');
    EXPECT_EQ(size_two_buffer.size(), 2);
}

TEST_F(TextFormatFixture, PositiveValueForInt8)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for a positive value with int8 size in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, std::int8_t{123U});

    EXPECT_EQ(buffer.at(0), '1');
    EXPECT_EQ(buffer.at(1), '2');
    EXPECT_EQ(buffer.at(2), '3');
    EXPECT_EQ(buffer.at(3), ' ');
    EXPECT_EQ(buffer.size(), 4);
}

TEST_F(TextFormatFixture, NegativeValueInt8)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for a negative value with int8 size in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, std::int8_t{-123});

    EXPECT_EQ(buffer.at(0), '-');
    EXPECT_EQ(buffer.at(1), '1');
    EXPECT_EQ(buffer.at(2), '2');
    EXPECT_EQ(buffer.at(3), '3');
    EXPECT_EQ(buffer.at(4), ' ');
    EXPECT_EQ(buffer.size(), 5);
}

TEST_F(TextFormatFixture, PositiveValueForInt16)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for a positive value with int16 size in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, std::int16_t{123U});

    EXPECT_EQ(buffer.at(0), '1');
    EXPECT_EQ(buffer.at(1), '2');
    EXPECT_EQ(buffer.at(2), '3');
    EXPECT_EQ(buffer.at(3), ' ');
    EXPECT_EQ(buffer.size(), 4);
}

TEST_F(TextFormatFixture, NegativeValueInt16)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for a negative value with int16 size in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, std::numeric_limits<int16_t>::min());
    EXPECT_EQ(buffer.size(), 7);
    EXPECT_EQ(0, std::memcmp(buffer.data(), "-32768 ", buffer.size()));
}

TEST_F(TextFormatFixture, PositiveValueInt32)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for a positive value with int32 size in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, std::numeric_limits<int32_t>::max());
    EXPECT_EQ(buffer.size(), 11);
    EXPECT_EQ(0, std::memcmp(buffer.data(), "2147483647 ", buffer.size()));
}

TEST_F(TextFormatFixture, NegativeValueInt32)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for a negative value with int32 size in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, std::numeric_limits<int32_t>::min());
    EXPECT_EQ(buffer.at(0), '-');
    EXPECT_EQ(buffer.at(1), '2');
    EXPECT_EQ(buffer.at(2), '1');
    EXPECT_EQ(buffer.at(3), '4');
    EXPECT_EQ(buffer.at(4), '7');
    EXPECT_EQ(buffer.at(5), '4');
    EXPECT_EQ(buffer.at(6), '8');
    EXPECT_EQ(buffer.at(7), '3');
    EXPECT_EQ(buffer.at(8), '6');
    EXPECT_EQ(buffer.at(9), '4');
    EXPECT_EQ(buffer.at(10), '8');
    EXPECT_EQ(buffer.at(11), ' ');
    EXPECT_EQ(buffer.size(), 12);
    EXPECT_EQ(0, std::memcmp(buffer.data(), "-2147483648 ", buffer.size()));
}

TEST_F(TextFormatFixture, PositiveValueInt64)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for a positive value with int64 size in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, int64_t{std::numeric_limits<int64_t>::max()});

    EXPECT_EQ(buffer.size(), 20);
    EXPECT_EQ(0, std::memcmp(buffer.data(), "9223372036854775807 ", buffer.size()));
}

TEST_F(TextFormatFixture, NegativeValueInt64)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for a negative value with int64 size in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, std::numeric_limits<int64_t>::min());

    EXPECT_EQ(buffer.size(), 21);
    EXPECT_EQ(0, std::memcmp(buffer.data(), "-9223372036854775808 ", buffer.size()));
}

TEST_F(TextFormatFixture, PositiveValueForUint8)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for positive value with uint8 representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, std::uint8_t{234U});

    EXPECT_EQ(buffer.at(0), '2');
    EXPECT_EQ(buffer.at(1), '3');
    EXPECT_EQ(buffer.at(2), '4');
    EXPECT_EQ(buffer.at(3), ' ');
    EXPECT_EQ(buffer.size(), 4);
}

TEST_F(TextFormatFixture, HexFormatUint8)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for uint8 value with hex representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, LogHex8{234U});

    EXPECT_EQ(buffer.at(0), 'e');
    EXPECT_EQ(buffer.at(1), 'a');
    EXPECT_EQ(buffer.at(2), ' ');
    EXPECT_EQ(buffer.size(), 3);
}

TEST_F(TextFormatFixture, BinaryFormatUint8)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for uint8 value with binary representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, LogBin8{234U});

    EXPECT_EQ(buffer.at(0), '1');
    EXPECT_EQ(buffer.at(1), '1');
    EXPECT_EQ(buffer.at(2), '1');
    EXPECT_EQ(buffer.at(3), '0');
    EXPECT_EQ(buffer.at(4), '1');
    EXPECT_EQ(buffer.at(5), '0');
    EXPECT_EQ(buffer.at(6), '1');
    EXPECT_EQ(buffer.at(7), '0');
    EXPECT_EQ(buffer.at(8), ' ');
    EXPECT_EQ(buffer.size(), 9);
}

TEST_F(TextFormatFixture, OctalFormatUint8)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for uint8 value with octal representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, std::uint8_t{234U}, IntegerRepresentation::kOctal);

    EXPECT_EQ(buffer.at(0), '3');
    EXPECT_EQ(buffer.at(1), '5');
    EXPECT_EQ(buffer.at(2), '2');
    EXPECT_EQ(buffer.at(3), ' ');
    EXPECT_EQ(buffer.size(), 4);
}

TEST_F(TextFormatFixture, PositiveValueForUint16)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for a positive value with uint16 representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, std::uint16_t{43456U});

    EXPECT_EQ(buffer.at(0), '4');
    EXPECT_EQ(buffer.at(1), '3');
    EXPECT_EQ(buffer.at(2), '4');
    EXPECT_EQ(buffer.at(3), '5');
    EXPECT_EQ(buffer.at(4), '6');
    EXPECT_EQ(buffer.at(5), ' ');
    EXPECT_EQ(buffer.size(), 6);
}

TEST_F(TextFormatFixture, HexFormatUint16)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for uint16 value with hex representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, LogHex16{123U});

    EXPECT_EQ(buffer.at(0), '7');
    EXPECT_EQ(buffer.at(1), 'b');
    EXPECT_EQ(buffer.at(2), ' ');
    EXPECT_EQ(buffer.size(), 3);
}

TEST_F(TextFormatFixture, BinaryFormatUint16)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for uint16 value with binary representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, LogBin16{43456U});

    EXPECT_EQ(buffer.size(), 17);
    EXPECT_EQ(0, std::memcmp(buffer.data(), "1010100111000000 ", buffer.size()));
}

TEST_F(TextFormatFixture, OctalFormatUint16)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for uint16 value with octal representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, std::uint16_t{43456U}, IntegerRepresentation::kOctal);

    EXPECT_EQ(buffer.at(0), '1');
    EXPECT_EQ(buffer.at(1), '2');
    EXPECT_EQ(buffer.at(2), '4');
    EXPECT_EQ(buffer.at(3), '7');
    EXPECT_EQ(buffer.at(4), '0');
    EXPECT_EQ(buffer.at(5), '0');
    EXPECT_EQ(buffer.at(6), ' ');
    EXPECT_EQ(buffer.size(), 7);
}

TEST_F(TextFormatFixture, PositiveValueForUint32)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for positive value with uint32_t size in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, std::uint32_t{std::numeric_limits<int32_t>::max()} + 1);

    EXPECT_EQ(buffer.size(), 11);
    EXPECT_EQ(0, std::memcmp(buffer.data(), "2147483648 ", buffer.size()));
}

TEST_F(TextFormatFixture, HexFormatUint32)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for uint32 with hex representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, LogHex32{52345U});

    EXPECT_EQ(buffer.at(0), 'c');
    EXPECT_EQ(buffer.at(1), 'c');
    EXPECT_EQ(buffer.at(2), '7');
    EXPECT_EQ(buffer.at(3), '9');
    EXPECT_EQ(buffer.at(4), ' ');
    EXPECT_EQ(buffer.size(), 5);
}

TEST_F(TextFormatFixture, BinFormatUint32)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for uint32 with binary representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, LogBin32{52345U});

    EXPECT_EQ(buffer.size(), 33);
    EXPECT_EQ(0, std::memcmp(buffer.data(), "00000000000000001100110001111001 ", buffer.size()));
}

TEST_F(TextFormatFixture, OctalFormatUint32)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for uint32 with octal representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, std::uint32_t{52349U}, IntegerRepresentation::kOctal);

    EXPECT_EQ(buffer.size(), 7);
    EXPECT_EQ(0, std::memcmp(buffer.data(), "146175 ", buffer.size()));
}

TEST_F(TextFormatFixture, PositiveValueForUint64)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for positive value with uint64_t size  in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, uint64_t{std::numeric_limits<int64_t>::max()} + 1);

    EXPECT_EQ(buffer.size(), 20);
    EXPECT_EQ(0, std::memcmp(buffer.data(), "9223372036854775808 ", buffer.size()));
}

TEST_F(TextFormatFixture, BinaryFormat_InsufficientBuffer)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for binary representation shall be cropped in case of insufficent buffer "
                   "for its bytes.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(capacity_two_payload, std::uint8_t{234U}, IntegerRepresentation::kBinary);

    ASSERT_EQ(size_two_buffer.at(0), '1');
    ASSERT_EQ(size_two_buffer.at(1), ' ');

    ASSERT_EQ(size_two_buffer.size(), 2);
}

TEST_F(TextFormatFixture, BinaryFormatWhenBufferFull)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for binary representation shall be cropped in case of insufficent buffer "
                   "for its bytes.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    //  Make the buffer full
    TextFormat::Log(capacity_two_payload, std::string_view{"xxx"});
    TextFormat::Log(capacity_two_payload, std::uint8_t{234U}, IntegerRepresentation::kBinary);

    //  When buffer is full no new data should appear
    ASSERT_EQ(size_two_buffer.at(0), 'x');
    ASSERT_EQ(size_two_buffer.at(1), ' ');

    ASSERT_EQ(size_two_buffer.size(), 2);
}

TEST_F(TextFormatFixture, HexFormat_InsufficientBuffer)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for hex representation shall only store bytes of data equal to the "
                   "allocated capacity.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(capacity_two_payload, std::uint32_t{52345U}, IntegerRepresentation::kHex);

    ASSERT_EQ(size_two_buffer.at(0), 'c');
    ASSERT_EQ(size_two_buffer.at(1), ' ');

    ASSERT_EQ(size_two_buffer.size(), 2);
}

TEST_F(TextFormatFixture, HexFormatUint64)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for uint64 with hex representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, LogHex64{12379813812177893520U});

    EXPECT_EQ(buffer.size(), 17);
    EXPECT_EQ(0, std::memcmp(buffer.data(), "abcdef1234567890 ", buffer.size()));
}

TEST_F(TextFormatFixture, BinaryFormatUint64)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for uint64 with binary representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, LogBin64{12379813812177893520U});

    EXPECT_EQ(buffer.size(), 65);
    EXPECT_EQ(
        0,
        std::memcmp(buffer.data(), "1010101111001101111011110001001000110100010101100111100010010000 ", buffer.size()));
}

TEST_F(TextFormatFixture, OctalFormatUint64)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for uint64 with octal representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, std::uint64_t{12379813812177893520U}, IntegerRepresentation::kOctal);

    EXPECT_EQ(buffer.size(), 23);
    EXPECT_EQ(0, std::memcmp(buffer.data(), "1257157361106425474220 ", buffer.size()));
}

TEST_F(TextFormatFixture, LogFloat)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for float in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, 1.23F);

    ASSERT_EQ(buffer.at(0), '1');
    ASSERT_EQ(buffer.at(1), '.');
    ASSERT_EQ(buffer.at(2), '2');
    ASSERT_EQ(buffer.at(3), '3');
    ASSERT_EQ(buffer.at(4), '0');
    ASSERT_EQ(buffer.at(5), '0');
    ASSERT_EQ(buffer.at(6), '0');
    ASSERT_EQ(buffer.at(7), '0');
    ASSERT_EQ(buffer.at(8), ' ');

    EXPECT_EQ(buffer.size(), 9);
}

TEST_F(TextFormatFixture, LogDouble)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for double in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, 1.23);

    ASSERT_EQ(buffer.at(0), '1');
    ASSERT_EQ(buffer.at(1), '.');
    ASSERT_EQ(buffer.at(2), '2');
    ASSERT_EQ(buffer.at(3), '3');
    ASSERT_EQ(buffer.at(4), '0');
    ASSERT_EQ(buffer.at(5), '0');
    ASSERT_EQ(buffer.at(6), '0');
    ASSERT_EQ(buffer.at(7), '0');
    ASSERT_EQ(buffer.at(8), ' ');

    EXPECT_EQ(buffer.size(), 9);
}

TEST_F(TextFormatFixture, StringValueCorrectlyTransformed)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for string in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, std::string_view{"Hello World"});

    // String
    ASSERT_EQ(buffer.at(0), 'H');
    ASSERT_EQ(buffer.at(1), 'e');
    ASSERT_EQ(buffer.at(2), 'l');
    ASSERT_EQ(buffer.at(3), 'l');
    ASSERT_EQ(buffer.at(4), 'o');
    ASSERT_EQ(buffer.at(5), ' ');
    ASSERT_EQ(buffer.at(6), 'W');
    ASSERT_EQ(buffer.at(7), 'o');
    ASSERT_EQ(buffer.at(8), 'r');
    ASSERT_EQ(buffer.at(9), 'l');
    ASSERT_EQ(buffer.at(10), 'd');
    ASSERT_EQ(buffer.at(11), ' ');

    ASSERT_EQ(buffer.size(), 12);
}

TEST_F(TextFormatFixture, TerminateShallPutNewLine)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies that TerminateLog shall put new line in data buffer.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::TerminateLog(payload);
    ASSERT_EQ(buffer.at(0), '\n');
    ASSERT_EQ(buffer.size(), 1);
}

TEST_F(TextFormatFixture, StringValueWhenBufferFull)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for string shall be intact in case of using full buffer.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    //  Make the buffer full
    TextFormat::Log(capacity_two_payload, std::string_view{"xxx"});
    //  Try to put more data into buffer
    TextFormat::Log(capacity_two_payload, std::string_view{"Hello World"});

    // String is not changed
    ASSERT_EQ(size_two_buffer.at(0), 'x');
    ASSERT_EQ(size_two_buffer.at(1), ' ');

    ASSERT_EQ(size_two_buffer.size(), 2);
}

TEST_F(TextFormatFixture, EmptyString)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for empty string will not allocate memory.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    TextFormat::Log(payload, std::string_view{""});

    ASSERT_EQ(buffer.size(), 0);
}

TEST_F(TextFormatFixture, RawValueSimpleConversionToHex)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "Verifies Type-Information for raw value will be converted to hex values nibble by nibble before storing.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::vector<char> data{{1, 2, 0x1F}};
    TextFormat::Log(payload, LogRawBuffer{data.data(), 3});

    // Data
    ASSERT_EQ(buffer.at(0), '0');
    ASSERT_EQ(buffer.at(1), '1');
    ASSERT_EQ(buffer.at(2), '0');
    ASSERT_EQ(buffer.at(3), '2');
    ASSERT_EQ(buffer.at(4), '1');
    ASSERT_EQ(buffer.at(5), 'f');
    ASSERT_EQ(buffer.at(6), ' ');
    // Size
    ASSERT_EQ(buffer.size(), 7);
}

TEST_F(TextFormatFixture, RawValueSimpleConversionToHexInsufficientBuffer)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for raw value will be converted to hex values nibble by nibble and will "
                   "be cropped in case of using insufficient buffer.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::vector<char> data{{1, 2, 0x1F}};
    TextFormat::Log(capacity_two_payload, LogRawBuffer{data.data(), 3});

    // Data
    ASSERT_EQ(size_two_buffer.at(0), '0');
    ASSERT_EQ(size_two_buffer.at(1), '1');
    // Size
    ASSERT_EQ(size_two_buffer.size(), 2);
}

TEST_F(TextFormatFixture, RawValueZeroLengthBuffer)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for raw value with zero size will not allocate any memory for logging.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::vector<char> data{};
    TextFormat::Log(payload, LogRawBuffer{data.data(), 0});

    // Size
    ASSERT_EQ(buffer.size(), 0);
}

TEST_F(TextFormatFixture, RawValueZeroMaxSizeBuffer)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies raw value with zero max size buffer will not allocate any memory for logging.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    ByteVector empty_buffer{};
    LogRawBuffer data{"test data"};
    VerbosePayload zero_payload{0U, empty_buffer};

    TextFormat::Log(zero_payload, data);

    ASSERT_TRUE(zero_payload.GetSpan().empty());
}

TEST(FormattingFunction, ShallConvertNegativeValuesToZero)
{
    ASSERT_EQ(FormattingFunctionReturnCast(std::int32_t{-1}), std::size_t{0UL});
}

TEST(FormattingFunction, ShallReturnEmptyIfPayloadMaxSizeEqualToZero)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies getting empty payload in case of the max size for allocated memory is equal to zero.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    ByteVector buffer{};
    VerbosePayload payload{0U, buffer};
    TextFormat::PutFormattedTime(payload);

    ASSERT_TRUE(payload.GetSpan().empty());
}

}  // namespace
}  // namespace test
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
