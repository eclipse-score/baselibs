/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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

#include "score/serialization/strategies/uds_strategy.h"

#include "score/serialization/serializer.h"
#include "score/serialization/strategies/unsigned_integral_only_strategy.h"
#include "score/serialization/types/length_prefixed_vector.h"

#include <gtest/gtest.h>

#include <score/static_vector.hpp>
#include <score/string.hpp>
#include <score/vector.hpp>
#include <vector>

namespace score
{
namespace serialization
{
namespace
{

class UDSStrategyFixture : public ::testing::Test
{
  public:
    UDSStrategy strategy_{};
    Serializer<UDSStrategy> unit_{strategy_};
    std::vector<std::uint8_t> buffer_{};

    template <typename TypeToTest>
    void TestSerializationAndDeserializationProcess(const TypeToTest& value)
    {
        // Given we serialize a value
        auto result = unit_.Serialize(value, buffer_);
        ASSERT_TRUE(result.has_value());

        // When deserializing it
        TypeToTest new_value{};
        result = unit_.DeSerialize(buffer_, new_value);
        ASSERT_TRUE(result.has_value());

        // Then the values match
        EXPECT_EQ(value, new_value);
    }
};

TEST_F(UDSStrategyFixture, CanSerializeUint8)
{
    const std::uint8_t value{0x42};

    TestSerializationAndDeserializationProcess(value);

    EXPECT_EQ(buffer_.at(0), value);
}

TEST_F(UDSStrategyFixture, CanSerializeUint16)
{
    const std::uint16_t value{0x0A0B};

    TestSerializationAndDeserializationProcess(value);

    auto iterator = buffer_.cbegin();
    EXPECT_EQ(*iterator++, 0x0A);
    EXPECT_EQ(*iterator++, 0x0B);
}

TEST_F(UDSStrategyFixture, CanSerializeUint32)
{
    const std::uint32_t value{0x0A0B0C0D};

    TestSerializationAndDeserializationProcess(value);

    auto iterator = buffer_.cbegin();
    EXPECT_EQ(*iterator++, 0x0A);
    EXPECT_EQ(*iterator++, 0x0B);
    EXPECT_EQ(*iterator++, 0x0C);
    EXPECT_EQ(*iterator++, 0x0D);
}

TEST_F(UDSStrategyFixture, CanSerializeUint64)
{
    const std::uint64_t value{0x0A0B0C0D0E0F0102};

    TestSerializationAndDeserializationProcess(value);

    auto iterator = buffer_.cbegin();
    EXPECT_EQ(*iterator++, 0x0A);
    EXPECT_EQ(*iterator++, 0x0B);
    EXPECT_EQ(*iterator++, 0x0C);
    EXPECT_EQ(*iterator++, 0x0D);
    EXPECT_EQ(*iterator++, 0x0E);
    EXPECT_EQ(*iterator++, 0x0F);
    EXPECT_EQ(*iterator++, 0x01);
    EXPECT_EQ(*iterator++, 0x02);
}

struct SomeStruct
{
    std::uint16_t first{0x0A0B};
    std::uint8_t second{0x0C};
};
SCORE_STRUCT_VISITABLE(SomeStruct, first, second)

bool operator==(const SomeStruct& left, const SomeStruct& right)
{
    return (left.first == right.first) && (left.second == right.second);
}

TEST_F(UDSStrategyFixture, CanSerializeStruct)
{
    const SomeStruct value{};

    TestSerializationAndDeserializationProcess(value);

    auto iterator = buffer_.cbegin();
    EXPECT_EQ(*iterator++, 0x0A);
    EXPECT_EQ(*iterator++, 0x0B);
    EXPECT_EQ(*iterator++, 0x0C);
}

struct MotherStruct
{
    SomeStruct other_struct{};
    std::uint16_t third{0x0D0E};
};
SCORE_STRUCT_VISITABLE(MotherStruct, other_struct, third)

bool operator==(const MotherStruct& left, const MotherStruct& right)
{
    return (left.other_struct == right.other_struct) && (left.third == right.third);
}

TEST_F(UDSStrategyFixture, CanSerializeStructInStruct)
{
    const MotherStruct value{};

    TestSerializationAndDeserializationProcess(value);

    auto iterator = buffer_.cbegin();
    EXPECT_EQ(*iterator++, 0x0A);
    EXPECT_EQ(*iterator++, 0x0B);
    EXPECT_EQ(*iterator++, 0x0C);
    EXPECT_EQ(*iterator++, 0x0D);
    EXPECT_EQ(*iterator++, 0x0E);
}

TEST_F(UDSStrategyFixture, CanSerializeEnum)
{
    enum class SomeEnum : std::uint8_t
    {
        kFirst = 0x42,
        kNext = 0x43
    };

    const SomeEnum value{SomeEnum::kNext};

    TestSerializationAndDeserializationProcess(value);

    auto iterator = buffer_.cbegin();
    EXPECT_EQ(*iterator++, 0x043);
}

TEST_F(UDSStrategyFixture, SerializationFailsWhenStringSizeExceedsMaximumLimit)
{
    const score::cpp::pmr::string value(helper::fixed_string_size + 1, 'A');

    auto result = unit_.Serialize(value, buffer_);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(buffer_.size(), 0);
}

TEST_F(UDSStrategyFixture, CanSerializePmrString)
{
    const score::cpp::pmr::string value{"Hallo Welt"};

    TestSerializationAndDeserializationProcess(value);

    auto iterator = buffer_.cbegin();
    EXPECT_EQ(*iterator++, 10);
    EXPECT_EQ(*iterator++, 'H');
    EXPECT_EQ(*iterator++, 'a');
    EXPECT_EQ(*iterator++, 'l');
    EXPECT_EQ(*iterator++, 'l');
    EXPECT_EQ(*iterator++, 'o');
    EXPECT_EQ(*iterator++, ' ');
    EXPECT_EQ(*iterator++, 'W');
    EXPECT_EQ(*iterator++, 'e');
    EXPECT_EQ(*iterator++, 'l');
    EXPECT_EQ(*iterator++, 't');
    for (auto counter{value.size()}; counter < 255; counter++)
    {
        EXPECT_EQ(*iterator++, '\0');
    }
    EXPECT_EQ(buffer_.size(), 1 + 255);
}

TEST_F(UDSStrategyFixture, CanSerializeStaticVector)
{
    const score::cpp::static_vector<std::uint16_t, 25> value{0x0A0B, 0x0C0D, 0x0E0F};

    TestSerializationAndDeserializationProcess(value);

    auto iterator = buffer_.cbegin();
    EXPECT_EQ(*iterator++, 3);
    EXPECT_EQ(*iterator++, 0x0A);
    EXPECT_EQ(*iterator++, 0x0B);
    EXPECT_EQ(*iterator++, 0x0C);
    EXPECT_EQ(*iterator++, 0x0D);
    EXPECT_EQ(*iterator++, 0x0E);
    EXPECT_EQ(*iterator++, 0x0F);
}

TEST_F(UDSStrategyFixture, CanSerializeStructInStaticVector)
{
    const score::cpp::static_vector<SomeStruct, 3> value{SomeStruct{}, SomeStruct{}, SomeStruct{}};

    TestSerializationAndDeserializationProcess(value);

    auto iterator = buffer_.cbegin();
    EXPECT_EQ(*iterator++, 0x03);
    EXPECT_EQ(*iterator++, 0x0A);  // first struct
    EXPECT_EQ(*iterator++, 0x0B);
    EXPECT_EQ(*iterator++, 0x0C);
    EXPECT_EQ(*iterator++, 0x0A);  // second struct
    EXPECT_EQ(*iterator++, 0x0B);
    EXPECT_EQ(*iterator++, 0x0C);
    EXPECT_EQ(*iterator++, 0x0A);  // third struct
    EXPECT_EQ(*iterator++, 0x0B);
    EXPECT_EQ(*iterator++, 0x0C);
}

TEST_F(UDSStrategyFixture, CanSerializeStaticVectorOutOfBounds)
{
    // Given that we expect a static_vector of size two, but the size in buffer says 3
    score::cpp::static_vector<std::uint16_t, 2> value{};
    buffer_.push_back(3);

    // When deserializing
    const auto result = unit_.DeSerialize(buffer_, value);

    // Then an error is returned
    EXPECT_FALSE(result.has_value());
}

TEST_F(UDSStrategyFixture, CanSerializeStringEmptyBuffer)
{
    // Given that we expect a string, but the buffer is empty
    score::cpp::pmr::string value{};
    buffer_.clear();

    // When deserializing
    const auto result = unit_.DeSerialize(buffer_, value);

    // The output is empty
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(value.empty());
}

TEST_F(UDSStrategyFixture, CanSerializeStaticVectorEmptyBuffer)
{
    // Given that we expect a static_vector of size two, but the buffer is empty
    score::cpp::static_vector<std::uint16_t, 2> value{};
    buffer_.clear();

    // When deserializing
    const auto result = unit_.DeSerialize(buffer_, value);

    // The output is empty
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(value.empty());
}

TEST_F(UDSStrategyFixture, CanSerializeStaticVectorShortBuffer)
{
    // Given that we expect a static_vector of size two, but size in buffer says 1
    score::cpp::static_vector<std::uint8_t, 2> value{};

    buffer_.clear();
    buffer_.push_back(1);
    buffer_.push_back(0xFF);

    // When deserializing
    const auto result = unit_.DeSerialize(buffer_, value);

    // The output is partially filled
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(value.size(), 1);
    EXPECT_EQ(value[0], 0xFF);
}

TEST_F(UDSStrategyFixture, CanSerializePmrVector)
{
    const score::cpp::pmr::vector<std::uint16_t> value{0x0A0B, 0x0C0D, 0x0E0F};

    TestSerializationAndDeserializationProcess(value);

    auto iterator = buffer_.cbegin();
    EXPECT_EQ(*iterator++, 0x0A);
    EXPECT_EQ(*iterator++, 0x0B);
    EXPECT_EQ(*iterator++, 0x0C);
    EXPECT_EQ(*iterator++, 0x0D);
    EXPECT_EQ(*iterator++, 0x0E);
    EXPECT_EQ(*iterator++, 0x0F);
}

TEST_F(UDSStrategyFixture, CanSerializeStructInPmrVector)
{
    const score::cpp::pmr::vector<SomeStruct> value{SomeStruct{}, SomeStruct{}, SomeStruct{}};

    TestSerializationAndDeserializationProcess(value);

    auto iterator = buffer_.cbegin();
    EXPECT_EQ(*iterator++, 0x0A);  // first struct
    EXPECT_EQ(*iterator++, 0x0B);
    EXPECT_EQ(*iterator++, 0x0C);
    EXPECT_EQ(*iterator++, 0x0A);  // second struct
    EXPECT_EQ(*iterator++, 0x0B);
    EXPECT_EQ(*iterator++, 0x0C);
    EXPECT_EQ(*iterator++, 0x0A);  // third struct
    EXPECT_EQ(*iterator++, 0x0B);
    EXPECT_EQ(*iterator++, 0x0C);
}

TEST_F(UDSStrategyFixture, CanSerializeArray)
{
    const std::array<std::uint16_t, 3> value{0x0A0B, 0x0C0D, 0x0E0F};

    TestSerializationAndDeserializationProcess(value);

    auto iterator = buffer_.cbegin();
    EXPECT_EQ(*iterator++, 0x0A);
    EXPECT_EQ(*iterator++, 0x0B);
    EXPECT_EQ(*iterator++, 0x0C);
    EXPECT_EQ(*iterator++, 0x0D);
    EXPECT_EQ(*iterator++, 0x0E);
    EXPECT_EQ(*iterator++, 0x0F);
}

TEST_F(UDSStrategyFixture, CanSerializeStructInArray)
{
    const std::array<SomeStruct, 3> value{SomeStruct{}, SomeStruct{}, SomeStruct{}};

    TestSerializationAndDeserializationProcess(value);

    auto iterator = buffer_.cbegin();
    EXPECT_EQ(*iterator++, 0x0A);  // first struct
    EXPECT_EQ(*iterator++, 0x0B);
    EXPECT_EQ(*iterator++, 0x0C);
    EXPECT_EQ(*iterator++, 0x0A);  // second struct
    EXPECT_EQ(*iterator++, 0x0B);
    EXPECT_EQ(*iterator++, 0x0C);
    EXPECT_EQ(*iterator++, 0x0A);  // third struct
    EXPECT_EQ(*iterator++, 0x0B);
    EXPECT_EQ(*iterator++, 0x0C);
}

TEST_F(UDSStrategyFixture, CanSerializeLengthPrefixedVector)
{
    const score::serialization::LengthPrefixedVector<std::uint16_t, std::uint16_t> value{0x0A0B, 0x0C0D, 0x0E0F};

    TestSerializationAndDeserializationProcess(value);

    auto iterator = buffer_.cbegin();
    EXPECT_EQ(*iterator++, 0x00);
    EXPECT_EQ(*iterator++, 0x03);
    EXPECT_EQ(*iterator++, 0x0A);
    EXPECT_EQ(*iterator++, 0x0B);
    EXPECT_EQ(*iterator++, 0x0C);
    EXPECT_EQ(*iterator++, 0x0D);
    EXPECT_EQ(*iterator++, 0x0E);
    EXPECT_EQ(*iterator++, 0x0F);
}

TEST_F(UDSStrategyFixture, CanSerializeStructInLengthPrefixedVector)
{
    const score::serialization::LengthPrefixedVector<SomeStruct, std::uint16_t> value{
        SomeStruct{}, SomeStruct{}, SomeStruct{}};

    TestSerializationAndDeserializationProcess(value);

    auto iterator = buffer_.cbegin();
    EXPECT_EQ(*iterator++, 0x00);  // length
    EXPECT_EQ(*iterator++, 0x03);
    EXPECT_EQ(*iterator++, 0x0A);  // first struct
    EXPECT_EQ(*iterator++, 0x0B);
    EXPECT_EQ(*iterator++, 0x0C);
    EXPECT_EQ(*iterator++, 0x0A);  // second struct
    EXPECT_EQ(*iterator++, 0x0B);
    EXPECT_EQ(*iterator++, 0x0C);
    EXPECT_EQ(*iterator++, 0x0A);  // third struct
    EXPECT_EQ(*iterator++, 0x0B);
    EXPECT_EQ(*iterator++, 0x0C);
}

TEST_F(UDSStrategyFixture, CanDeserializeEmptyLengthPrefixedVector)
{
    // Given an empty LengthPrefixedVector is serialized (counter == 0 at deserialization)
    const score::serialization::LengthPrefixedVector<std::uint16_t, std::uint16_t> value{};

    TestSerializationAndDeserializationProcess(value);

    // Then the buffer contains only the zero-length prefix and the result is empty
    ASSERT_EQ(buffer_.size(), 2U);
    EXPECT_EQ(buffer_.at(0), 0x00);
    EXPECT_EQ(buffer_.at(1), 0x00);
}

TEST_F(UDSStrategyFixture, DeserializeLengthPrefixedVectorFailsWhenPrefixExceedsBuffer)
{
    // Given a length prefix claiming more elements than the buffer actually contains
    score::serialization::LengthPrefixedVector<std::uint16_t, std::uint16_t> value{};
    buffer_.clear();
    buffer_.push_back(0xFF);  // length prefix high byte
    buffer_.push_back(0xFF);  // length prefix low byte -> claims 0xFFFF elements, none present

    // When deserializing
    const auto result = unit_.DeSerialize(buffer_, value);

    // Then an out-of-bounds error is returned instead of iterating over absent data
    EXPECT_FALSE(result.has_value());
}

TEST_F(UDSStrategyFixture, DeserializeClearsReusedDestinations_LengthPrefixedVector)
{
    // Given
    const score::serialization::LengthPrefixedVector<std::uint16_t, std::uint16_t> length_prefixed{0x0A0B, 0x0C0D};
    score::serialization::LengthPrefixedVector<std::uint16_t, std::uint16_t> out{0xDEAD, 0xBEEF, 0xFEED};

    // When
    buffer_.clear();
    auto serialize_result = unit_.Serialize(length_prefixed, buffer_);
    auto deserialize_result = unit_.DeSerialize(buffer_, out);

    // Then
    ASSERT_TRUE(serialize_result.has_value());
    ASSERT_TRUE(deserialize_result.has_value());
    EXPECT_EQ(out, length_prefixed);
}

TEST_F(UDSStrategyFixture, DeserializeClearsReusedDestinations_String)
{
    // Given
    const score::cpp::pmr::string string_value{"Hi"};
    score::cpp::pmr::string out{"stale"};

    // When
    buffer_.clear();
    auto serialize_result = unit_.Serialize(string_value, buffer_);
    auto deserialize_result = unit_.DeSerialize(buffer_, out);

    // Then
    ASSERT_TRUE(serialize_result.has_value());
    ASSERT_TRUE(deserialize_result.has_value());
    EXPECT_EQ(out, string_value);
}

TEST_F(UDSStrategyFixture, DeserializeClearsReusedDestinations_PmrVector)
{
    // Given
    const score::cpp::pmr::vector<std::uint16_t> pmr_vector{0x0A0B};
    score::cpp::pmr::vector<std::uint16_t> out{0xDEAD, 0xBEEF, 0xFEED};

    // When
    buffer_.clear();
    auto serialize_result = unit_.Serialize(pmr_vector, buffer_);
    auto deserialize_result = unit_.DeSerialize(buffer_, out);

    // Then
    ASSERT_TRUE(serialize_result.has_value());
    ASSERT_TRUE(deserialize_result.has_value());
    EXPECT_EQ(out, pmr_vector);
}

TEST_F(UDSStrategyFixture, DeserializeClearsReusedDestinations_StaticVector)
{
    // Given
    const score::cpp::static_vector<std::uint16_t, 4> static_vector_value{0x0A0B, 0x0C0D};
    score::cpp::static_vector<std::uint16_t, 4> out{0xDEAD, 0xBEEF, 0xFEED};

    // When
    buffer_.clear();
    auto serialize_result = unit_.Serialize(static_vector_value, buffer_);
    auto deserialize_result = unit_.DeSerialize(buffer_, out);

    // Then
    ASSERT_TRUE(serialize_result.has_value());
    ASSERT_TRUE(deserialize_result.has_value());
    EXPECT_EQ(out, static_vector_value);
}

TEST_F(UDSStrategyFixture, DeserializeEmptyInputClearsReusedStaticVector)
{
    // Given a static_vector destination with stale data and an empty input buffer
    score::cpp::static_vector<std::uint16_t, 4> value{0xDEAD, 0xBEEF};
    buffer_.clear();

    // When deserializing
    const auto result = unit_.DeSerialize(buffer_, value);

    // Then the accepted empty input yields an empty destination rather than retaining stale data
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(value.empty());
}

TEST(UnsignedIntegralOnlyStrategyTest, SerializeNormalByteOrder)
{
    score::serialization::UnsignedIntegralOnlyStrategy strategy(/*reverse_byte_order=*/false);
    std::vector<uint8_t> buffer;
    score::serialization::SerializeIterator<std::vector<uint8_t>> it(buffer);
    score::serialization::SerializationVisitor<std::vector<uint8_t>, score::serialization::UnsignedIntegralOnlyStrategy>
        visitor(buffer, strategy);
    const uint32_t value = 0x12345678;

    strategy.Serialize(visitor, it, value);

    ASSERT_EQ(buffer.size(), 4u);
    EXPECT_EQ(buffer[0], 0x78);
    EXPECT_EQ(buffer[1], 0x56);
    EXPECT_EQ(buffer[2], 0x34);
    EXPECT_EQ(buffer[3], 0x12);
}

TEST(UnsignedIntegralOnlyStrategyTest, DeSerializeReverseByteOrder)
{
    score::serialization::UnsignedIntegralOnlyStrategy strategy(/*reverse_byte_order=*/false);
    // Buffer in little-endian order for 0x12345678
    std::vector<uint8_t> buffer = {0x78, 0x56, 0x34, 0x12};
    score::serialization::DeSerializeIterator<std::vector<uint8_t>> it(buffer);
    score::serialization::SerializationVisitor<std::vector<uint8_t>, score::serialization::UnsignedIntegralOnlyStrategy>
        visitor(buffer, strategy);
    uint32_t value = 0;

    EXPECT_TRUE(strategy.DeSerialize(visitor, it, value));

    EXPECT_EQ(value, 0x12345678u);
}

}  // namespace
}  // namespace serialization
}  // namespace score
