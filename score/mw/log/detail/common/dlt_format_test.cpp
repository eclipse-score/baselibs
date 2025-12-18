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
#include "score/mw/log/detail/common/dlt_format.h"

#include "gtest/gtest.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
namespace
{

class DLTFormatFixture : public ::testing::Test
{
  public:
    ByteVector buffer_{};
    VerbosePayload payload_{100, buffer_};
    ByteVector size_two_buffer_{};
    VerbosePayload size_two_payload_{2, size_two_buffer_};
};

TEST_F(DLTFormatFixture, TypeInfomrationForBoolean)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00139, PRS_Dlt_00625");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for boolean value is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, true);

    ASSERT_EQ(buffer_.at(0), '\x11');
    ASSERT_EQ(buffer_.at(1), '\x00');
    ASSERT_EQ(buffer_.at(2), '\x00');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, BooleanValueTrueCorrectlyTransformed)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00422, PRS_Dlt_00423, PRS_Dlt_00369");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies boolean value TRUE is correctly transformed.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, true);

    ASSERT_EQ(buffer_.at(4), '\x01');
}

TEST_F(DLTFormatFixture, BooleanValueFalseCorrectlyTransformed)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236, PRS_Dlt_00423, PRS_Dlt_00369");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00423, PRS_Dlt_00369");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies boolean value FALSE is correctly transformed.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, false);

    ASSERT_EQ(buffer_.at(4), '\x00');
}

TEST_F(DLTFormatFixture, TypeInfomrationForUint8)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00386, PRS_Dlt_00356, PRS_Dlt_00358, PRS_Dlt_00370");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for uint8 value is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::uint8_t{42U});

    ASSERT_EQ(buffer_.at(0), '\x41');
    ASSERT_EQ(buffer_.at(1), '\x00');
    ASSERT_EQ(buffer_.at(2), '\x00');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, Uint8ValueTrueCorrectlyTransformed)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00370");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies uint8 value is correctly transformed.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::uint8_t{0x42});

    ASSERT_EQ(buffer_.at(4), '\x42');
}

TEST_F(DLTFormatFixture, TypeIsNotStoredIfNotWholePayloadFitsIntoBuffer)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00386, PRS_Dlt_00356, PRS_Dlt_00358, PRS_Dlt_00370");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for uint8 value is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Given a buffer that is to small to fit TypeInformation + Payload
    ByteVector buffer{};
    VerbosePayload payload{3, buffer};

    // When Logging the type
    DLTFormat::Log(payload, std::uint8_t{42U});

    // Then nothing is stored in the buffer (since it would have overflowed)
    ASSERT_EQ(buffer.size(), 0U);
}

TEST_F(DLTFormatFixture, TypeInfomrationForUint16)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00386, PRS_Dlt_00356, PRS_Dlt_00358, PRS_Dlt_00370");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for uint16 value is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::uint16_t{0xABCD});

    ASSERT_EQ(buffer_.at(0), '\x42');
    ASSERT_EQ(buffer_.at(1), '\x00');
    ASSERT_EQ(buffer_.at(2), '\x00');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, Uint16ValueTrueCorrectlyTransformed)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00370");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies uint16 value is correctly transformed.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::uint16_t{0x42AB});

    ASSERT_EQ(buffer_.at(4), '\xAB');
    ASSERT_EQ(buffer_.at(5), '\x42');
}

TEST_F(DLTFormatFixture, TypeInfomrationForUint32)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00386, PRS_Dlt_00356, PRS_Dlt_00358, PRS_Dlt_00370");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for uint32 value is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::uint32_t{0xABCDEF00});

    ASSERT_EQ(buffer_.at(0), '\x43');
    ASSERT_EQ(buffer_.at(1), '\x00');
    ASSERT_EQ(buffer_.at(2), '\x00');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, Uint32ValueTrueCorrectlyTransformed)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00370");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies uint32 value is correctly transformed.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::uint32_t{0x42ABCDEF});

    ASSERT_EQ(buffer_.at(4), '\xEF');
    ASSERT_EQ(buffer_.at(5), '\xCD');
    ASSERT_EQ(buffer_.at(6), '\xAB');
    ASSERT_EQ(buffer_.at(7), '\x42');
}

TEST_F(DLTFormatFixture, TypeInfomrationForUint64)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00386, PRS_Dlt_00356, PRS_Dlt_00358, PRS_Dlt_00370");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for uint64 value is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::uint64_t{0xABCDEF00ABCDEF00});

    ASSERT_EQ(buffer_.at(0), '\x44');
    ASSERT_EQ(buffer_.at(1), '\x00');
    ASSERT_EQ(buffer_.at(2), '\x00');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, Uint64ValueTrueCorrectlyTransformed)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00370");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies uint64 value is correctly transformed.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::uint64_t{0x42ABCDEF01020304});

    ASSERT_EQ(buffer_.at(4), '\x04');
    ASSERT_EQ(buffer_.at(5), '\x03');
    ASSERT_EQ(buffer_.at(6), '\x02');
    ASSERT_EQ(buffer_.at(7), '\x01');
    ASSERT_EQ(buffer_.at(8), '\xEF');
    ASSERT_EQ(buffer_.at(9), '\xCD');
    ASSERT_EQ(buffer_.at(10), '\xAB');
    ASSERT_EQ(buffer_.at(11), '\x42');
}

TEST_F(DLTFormatFixture, TypeInformationForInt8)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00386, PRS_Dlt_00356, PRS_Dlt_00358, PRS_Dlt_00370");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for int8 value is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::int8_t{-42});

    ASSERT_EQ(buffer_.at(0), '\x21');
    ASSERT_EQ(buffer_.at(1), '\x00');
    ASSERT_EQ(buffer_.at(2), '\x00');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, Int8ValueTrueCorrectlyTransformed)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00370");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies int8 value is correctly transformed.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::int8_t{-42});

    ASSERT_EQ(buffer_.at(4), '\xd6');
}

TEST_F(DLTFormatFixture, TypeInformationForInt16)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00386, PRS_Dlt_00356, PRS_Dlt_00358, PRS_Dlt_00370");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for int16 value is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::int16_t{-32768});

    ASSERT_EQ(buffer_.at(0), '\x22');
    ASSERT_EQ(buffer_.at(1), '\x00');
    ASSERT_EQ(buffer_.at(2), '\x00');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, Int16ValueTrueCorrectlyTransformed)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00370");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies int16 value is correctly transformed.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::int16_t{-32768});

    ASSERT_EQ(buffer_.at(4), '\x00');
    ASSERT_EQ(buffer_.at(5), '\x80');
}

TEST_F(DLTFormatFixture, TypeInformationForInt32)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00386, PRS_Dlt_00356, PRS_Dlt_00358, PRS_Dlt_00370");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for int32 value is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::int32_t{-2147483648});

    ASSERT_EQ(buffer_.at(0), '\x23');
    ASSERT_EQ(buffer_.at(1), '\x00');
    ASSERT_EQ(buffer_.at(2), '\x00');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, Int32ValueTrueCorrectlyTransformed)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00370");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies int32 value is correctly transformed.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::int32_t{-2147483648});

    ASSERT_EQ(buffer_.at(4), '\x00');
    ASSERT_EQ(buffer_.at(5), '\x00');
    ASSERT_EQ(buffer_.at(6), '\x00');
    ASSERT_EQ(buffer_.at(7), '\x80');
}

TEST_F(DLTFormatFixture, TypeInformationForInt64)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00386, PRS_Dlt_00356, PRS_Dlt_00358, PRS_Dlt_00370");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for int64 value is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::numeric_limits<std::int64_t>::min());

    ASSERT_EQ(buffer_.at(0), '\x24');
    ASSERT_EQ(buffer_.at(1), '\x00');
    ASSERT_EQ(buffer_.at(2), '\x00');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, Int64ValueTrueCorrectlyTransformed)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00370");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies int64 value is correctly transformed.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::numeric_limits<std::int64_t>::min());

    ASSERT_EQ(buffer_.at(4), '\x00');
    ASSERT_EQ(buffer_.at(5), '\x00');
    ASSERT_EQ(buffer_.at(6), '\x00');
    ASSERT_EQ(buffer_.at(7), '\x00');
    ASSERT_EQ(buffer_.at(8), '\x00');
    ASSERT_EQ(buffer_.at(9), '\x00');
    ASSERT_EQ(buffer_.at(10), '\x00');
    ASSERT_EQ(buffer_.at(11), '\x80');
}

TEST_F(DLTFormatFixture, TypeInformationForFloat)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00390, PRS_Dlt_00145, PRS_Dlt_00371");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for float value is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, float{1.0f});

    ASSERT_EQ(buffer_.at(0), '\x83');
    ASSERT_EQ(buffer_.at(1), '\x00');
    ASSERT_EQ(buffer_.at(2), '\x00');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, FloatValueCorrectlyTransformed)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00371");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies float value is correctly transformed.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, float{1.0f});

    ASSERT_EQ(buffer_.at(4), '\x00');
    ASSERT_EQ(buffer_.at(5), '\x00');
    ASSERT_EQ(buffer_.at(6), '\x80');
    ASSERT_EQ(buffer_.at(7), '\x3F');
}

TEST_F(DLTFormatFixture, TypeInformationForDouble)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00390, PRS_Dlt_00145, PRS_Dlt_00371");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for double value is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, double{1.0});

    ASSERT_EQ(buffer_.at(0), '\x84');
    ASSERT_EQ(buffer_.at(1), '\x00');
    ASSERT_EQ(buffer_.at(2), '\x00');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, DoubleValueCorrectlyTransformed)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00371");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies double value is correctly transformed.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, double{1.0});

    ASSERT_EQ(buffer_.at(4), '\x00');
    ASSERT_EQ(buffer_.at(5), '\x00');
    ASSERT_EQ(buffer_.at(6), '\x00');
    ASSERT_EQ(buffer_.at(7), '\x00');
    ASSERT_EQ(buffer_.at(8), '\x00');
    ASSERT_EQ(buffer_.at(9), '\x00');
    ASSERT_EQ(buffer_.at(10), '\xF0');
    ASSERT_EQ(buffer_.at(11), '\x3F');
}

TEST_F(DLTFormatFixture, TypeInformationForString)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00420, PRS_Dlt_00155");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for string value is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::string_view{"Hello World"});

    ASSERT_EQ(buffer_.at(0), '\x00');
    ASSERT_EQ(buffer_.at(1), '\x82');
    ASSERT_EQ(buffer_.at(2), '\x00');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, StringValueCorrectlyTransformed)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00156, PRS_Dlt_00373");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies string value is correctly transformed.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::string_view{"Hello World"});

    // Length
    ASSERT_EQ(buffer_.at(4), '\x0C');
    ASSERT_EQ(buffer_.at(5), '\x00');

    // String
    ASSERT_EQ(buffer_.at(6), 'H');
    ASSERT_EQ(buffer_.at(7), 'e');
    ASSERT_EQ(buffer_.at(8), 'l');
    ASSERT_EQ(buffer_.at(9), 'l');
    ASSERT_EQ(buffer_.at(10), 'o');
    ASSERT_EQ(buffer_.at(11), ' ');
    ASSERT_EQ(buffer_.at(12), 'W');
    ASSERT_EQ(buffer_.at(13), 'o');
    ASSERT_EQ(buffer_.at(14), 'r');
    ASSERT_EQ(buffer_.at(15), 'l');
    ASSERT_EQ(buffer_.at(16), 'd');
    ASSERT_EQ(buffer_.at(17), '\x00');
}

TEST_F(DLTFormatFixture, StringValueDoesNotFitNullTermination)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00156, PRS_Dlt_00373");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies string value does not fit in buffer shall be cropped.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    buffer_.clear();
    buffer_.shrink_to_fit();
    buffer_.reserve(17);
    DLTFormat::Log(payload_, std::string_view{"Hello World"});

    // String
    ASSERT_EQ(buffer_.at(6), 'H');
    ASSERT_EQ(buffer_.at(7), 'e');
    ASSERT_EQ(buffer_.at(8), 'l');
    ASSERT_EQ(buffer_.at(9), 'l');
    ASSERT_EQ(buffer_.at(10), 'o');
    ASSERT_EQ(buffer_.at(11), ' ');
    ASSERT_EQ(buffer_.at(12), 'W');
    ASSERT_EQ(buffer_.at(13), 'o');
    ASSERT_EQ(buffer_.at(14), 'r');
    ASSERT_EQ(buffer_.at(15), 'l');
    ASSERT_EQ(buffer_.at(16), '\x00');
}

TEST_F(DLTFormatFixture, TypeInformationForUint8InHex)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00386, PRS_Dlt_00356, PRS_Dlt_00358, PRS_Dlt_00370, PRS_Dlt_00782");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for uint8 value with hex representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::uint8_t{42U}, score::mw::log::detail::IntegerRepresentation::kHex);

    ASSERT_EQ(buffer_.at(0), '\x41');
    ASSERT_EQ(buffer_.at(1), '\x00');
    ASSERT_EQ(buffer_.at(2), '\x01');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, TypeInformationForHex8InHex)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for 8 bits value in hex representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, LogHex8{0xFF}, score::mw::log::detail::IntegerRepresentation::kHex);

    ASSERT_EQ(buffer_.at(0), '\x41');
    ASSERT_EQ(buffer_.at(1), '\x00');
    ASSERT_EQ(buffer_.at(2), '\x01');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, TypeInformationForHex16InHex)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for 16 bits value in hex representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, LogHex16{0xFFFF}, score::mw::log::detail::IntegerRepresentation::kHex);

    ASSERT_EQ(buffer_.at(0), '\x42');
    ASSERT_EQ(buffer_.at(1), '\x00');
    ASSERT_EQ(buffer_.at(2), '\x01');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, TypeInformationForHex32InHex)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for 32 bits value in hex representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, LogHex32{0xFFFFFF}, score::mw::log::detail::IntegerRepresentation::kHex);

    ASSERT_EQ(buffer_.at(0), '\x43');
    ASSERT_EQ(buffer_.at(1), '\x00');
    ASSERT_EQ(buffer_.at(2), '\x01');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, TypeInformationForHex64InHex)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for 64 bits value in hex representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, LogHex64{0xFFFFFFFF}, score::mw::log::detail::IntegerRepresentation::kHex);

    ASSERT_EQ(buffer_.at(0), '\x44');
    ASSERT_EQ(buffer_.at(1), '\x00');
    ASSERT_EQ(buffer_.at(2), '\x01');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, TypeInformationForBin8InBin)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for 8 bits value in binary representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, LogBin8{0xFF}, score::mw::log::detail::IntegerRepresentation::kBinary);

    ASSERT_EQ(buffer_.at(0), '\x41');
    ASSERT_EQ(buffer_.at(1), '\x80');
    ASSERT_EQ(buffer_.at(2), '\x01');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, TypeInformationForBin16InBin)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for 16 bits value in binary representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, LogBin16{0xFFFF}, score::mw::log::detail::IntegerRepresentation::kBinary);

    ASSERT_EQ(buffer_.at(0), '\x42');
    ASSERT_EQ(buffer_.at(1), '\x80');
    ASSERT_EQ(buffer_.at(2), '\x01');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, TypeInformationForBin32InBin)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for 32 bits value in binary representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, LogBin32{0xFFFFFF}, score::mw::log::detail::IntegerRepresentation::kBinary);

    ASSERT_EQ(buffer_.at(0), '\x43');
    ASSERT_EQ(buffer_.at(1), '\x80');
    ASSERT_EQ(buffer_.at(2), '\x01');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, TypeInformationForBin64InBin)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for 64 bits value in binary representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, LogBin64{0xFFFFFFFF}, score::mw::log::detail::IntegerRepresentation::kBinary);

    ASSERT_EQ(buffer_.at(0), '\x44');
    ASSERT_EQ(buffer_.at(1), '\x80');
    ASSERT_EQ(buffer_.at(2), '\x01');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, TypeInformationForUint16InHex)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00386, PRS_Dlt_00356, PRS_Dlt_00358, PRS_Dlt_00370, PRS_Dlt_00782");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for uint16 value in hex representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::uint16_t{0xABCD}, score::mw::log::detail::IntegerRepresentation::kHex);

    ASSERT_EQ(buffer_.at(0), '\x42');
    ASSERT_EQ(buffer_.at(1), '\x00');
    ASSERT_EQ(buffer_.at(2), '\x01');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, TypeInformationForUint32InHex)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00386, PRS_Dlt_00356, PRS_Dlt_00358, PRS_Dlt_00370, PRS_Dlt_00782");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for uint32 value in hex representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::uint32_t{0xABCDEF00}, score::mw::log::detail::IntegerRepresentation::kHex);

    ASSERT_EQ(buffer_.at(0), '\x43');
    ASSERT_EQ(buffer_.at(1), '\x00');
    ASSERT_EQ(buffer_.at(2), '\x01');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, TypeInformationForUint64InHex)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00386, PRS_Dlt_00356, PRS_Dlt_00358, PRS_Dlt_00370, PRS_Dlt_00782");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for uint64 value in hex representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::uint64_t{0xABCDEF00ABCDEF00}, score::mw::log::detail::IntegerRepresentation::kHex);

    ASSERT_EQ(buffer_.at(0), '\x44');
    ASSERT_EQ(buffer_.at(1), '\x00');
    ASSERT_EQ(buffer_.at(2), '\x01');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, TypeInformationUInt8InBinary)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00386, PRS_Dlt_00356, PRS_Dlt_00358, PRS_Dlt_00370, PRS_Dlt_00782");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for uint8 value with bin representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::uint8_t{42U}, score::mw::log::detail::IntegerRepresentation::kBinary);

    ASSERT_EQ(buffer_.at(0), '\x41');
    ASSERT_EQ(buffer_.at(1), '\x80');
    ASSERT_EQ(buffer_.at(2), '\x01');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, TypeInformationUint16InBinary)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00386, PRS_Dlt_00356, PRS_Dlt_00358, PRS_Dlt_00370, PRS_Dlt_00782");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for uint16 value in bin representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::uint16_t{0xABCD}, score::mw::log::detail::IntegerRepresentation::kBinary);

    ASSERT_EQ(buffer_.at(0), '\x42');
    ASSERT_EQ(buffer_.at(1), '\x80');
    ASSERT_EQ(buffer_.at(2), '\x01');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, TypeInformationUint32InBinary)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00386, PRS_Dlt_00356, PRS_Dlt_00358, PRS_Dlt_00370, PRS_Dlt_00782");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for uint32 value in bin representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::uint32_t{0xABCDEF00}, score::mw::log::detail::IntegerRepresentation::kBinary);

    ASSERT_EQ(buffer_.at(0), '\x43');
    ASSERT_EQ(buffer_.at(1), '\x80');
    ASSERT_EQ(buffer_.at(2), '\x01');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, TypeInformationUint64InBinary)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00386, PRS_Dlt_00356, PRS_Dlt_00358, PRS_Dlt_00370, PRS_Dlt_00782");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies Type-Information for uint64 value in bin representation is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    DLTFormat::Log(payload_, std::uint64_t{0xABCDEF00ABCDEF00}, score::mw::log::detail::IntegerRepresentation::kBinary);

    ASSERT_EQ(buffer_.at(0), '\x44');
    ASSERT_EQ(buffer_.at(1), '\x80');
    ASSERT_EQ(buffer_.at(2), '\x01');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, TypeInformationForRaw)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00625");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Type-Information for raw value is in correct format.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::vector<char> data{{1, 2, 3}};
    DLTFormat::Log(payload_, LogRawBuffer{data.data(), 3});

    ASSERT_EQ(buffer_.at(0), '\x00');
    ASSERT_EQ(buffer_.at(1), '\x04');
    ASSERT_EQ(buffer_.at(2), '\x00');
    ASSERT_EQ(buffer_.at(3), '\x00');
}

TEST_F(DLTFormatFixture, RawValueCorrectlyTransformed)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00160, PRS_Dlt_00374");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies raw value is correctly transformed.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::vector<char> data{{1, 2, 3}};
    DLTFormat::Log(payload_, LogRawBuffer{data.data(), 3});

    // Length
    ASSERT_EQ(buffer_.at(4), '\x03');
    ASSERT_EQ(buffer_.at(5), '\x00');

    // Data
    ASSERT_EQ(buffer_.at(6), 1);
    ASSERT_EQ(buffer_.at(7), 2);
    ASSERT_EQ(buffer_.at(8), 3);
}

TEST_F(DLTFormatFixture, RawValueDoesNotFitWhole)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00160, PRS_Dlt_00374");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies raw buffer does not fit in buffer shall be cropped.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    buffer_.clear();
    buffer_.shrink_to_fit();
    buffer_.reserve(4 + 2 + 2);
    std::vector<char> data{{1, 2, 3}};
    DLTFormat::Log(payload_, LogRawBuffer{data.data(), 3});

    // Length
    ASSERT_EQ(buffer_.at(4), '\x02');
    ASSERT_EQ(buffer_.at(5), '\x00');

    // String
    ASSERT_EQ(buffer_.at(6), 1);
    ASSERT_EQ(buffer_.at(7), 2);
}

TEST_F(DLTFormatFixture, RawValueDoesNotFitAny)
{
    RecordProperty("ParentRequirement", "SCR-1633144, SCR-1633236");
    RecordProperty("AutosarRequirement", "PRS_Dlt_00160, PRS_Dlt_00374");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies empty buffer does not fit any byte, shall be skipped.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    buffer_.clear();
    buffer_.shrink_to_fit();
    buffer_.reserve(4 + 2);
    std::vector<char> data{{1, 2, 3}};
    DLTFormat::Log(payload_, LogRawBuffer{data.data(), 3});

    // Length is zero
    ASSERT_EQ(buffer_.size(), 0);
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
