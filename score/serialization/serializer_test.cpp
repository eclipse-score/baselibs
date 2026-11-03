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

#include "score/serialization/serializer.h"

#include "score/serialization/details/serialization_visitor.h"
#include "score/serialization/test_doubles/fake_serialization_strategy.h"

#include <gtest/gtest.h>

#include <score/span.hpp>

#include <cstring>

namespace score
{
namespace serialization
{

namespace
{

struct Payload
{
    std::uint8_t a;
    std::uint8_t b;
    std::vector<std::uint8_t> c;
    std::uint8_t d;
    std::vector<std::uint8_t> e;
};

struct OtherPayload
{
    std::uint8_t a;
    std::uint8_t b;
    std::uint8_t c;
};

SCORE_STRUCT_VISITABLE(Payload, a, b, c, d, e)
SCORE_STRUCT_VISITABLE(OtherPayload, a, b, c)

std::array<std::uint8_t, 512> global_buffer{};

static std::tuple<std::vector<std::uint8_t>,
                  std::array<std::uint8_t, 512>,
                  score::cpp::static_vector<std::uint8_t, 512>,
                  std::list<std::uint8_t>,
                  std::deque<std::uint8_t>,
                  score::cpp::span<std::uint8_t>>
    allParams{{}, {}, {}, {}, {}, {global_buffer}};

template <typename T>
struct SerializerFixture : public ::testing::Test
{
    SerializerFixture() : buffer{std::get<T>(allParams)} {}
    T buffer;
};

class SerializationTest : public ::testing::Test
{
  public:
    void SetUp() override {}
    void TearDown() override {}
};

TYPED_TEST_SUITE_P(SerializerFixture);

TYPED_TEST_P(SerializerFixture, CanSerializeComplexDataStructure)
{
    // Given a complex data structure
    Payload payload{};
    payload.a = 0x42;
    payload.b = 0x43;
    payload.c = {0x44, 0x45, 0x46};
    payload.d = 0x47;
    payload.e = {0x48, 0x49, 0x50};

    // When serializing it into a byte stream
    serialization::Serializer<test::FakeSerializationStrategy> unit{test::FakeSerializationStrategy{false}};
    EXPECT_TRUE(unit.Serialize(payload, this->buffer));

    // Then the layout is as expected
    auto data = this->buffer.begin();
    EXPECT_EQ(*data++, 0x42);
    EXPECT_EQ(*data++, 0x43);
    EXPECT_EQ(*data++, 0x03);  // the 8 bytes indicate the number of elements stored in the vector as a std::uint64_t
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x44);
    EXPECT_EQ(*data++, 0x45);
    EXPECT_EQ(*data++, 0x46);
    EXPECT_EQ(*data++, 0x47);
    EXPECT_EQ(*data++, 0x03);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x48);
    EXPECT_EQ(*data++, 0x49);
    EXPECT_EQ(*data++, 0x50);
}

TYPED_TEST_P(SerializerFixture, SerializationStrategyCanBeCustomizable)
{
    // Given a complex data structure
    Payload payload{};
    payload.a = 0x42;
    payload.b = 0x43;
    payload.c = {0x44, 0x45, 0x46};
    payload.d = 0x47;
    payload.e = {0x48, 0x49, 0x50};

    // When serializing it into a byte stream using custom options that a strategy can provide
    serialization::Serializer<test::FakeSerializationStrategy> unit{test::FakeSerializationStrategy{true}};
    EXPECT_TRUE(unit.Serialize(payload, this->buffer));

    // Then the layout is as expected
    auto data = this->buffer.begin();
    EXPECT_EQ(*data++, 0x42);
    EXPECT_EQ(*data++, 0x43);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x03);  // here a swap happened, the 8 bytes indicate the number of elements stored in the vector
                               // as a std::uint64_t
    EXPECT_EQ(*data++, 0x44);
    EXPECT_EQ(*data++, 0x45);
    EXPECT_EQ(*data++, 0x46);
    EXPECT_EQ(*data++, 0x47);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x00);
    EXPECT_EQ(*data++, 0x03);  // here a swap happened
    EXPECT_EQ(*data++, 0x48);
    EXPECT_EQ(*data++, 0x49);
    EXPECT_EQ(*data++, 0x50);
}

TYPED_TEST_P(SerializerFixture, SerializationStrategyCanUseMetaInformation)
{
    // Given a simple data structure
    OtherPayload payload{};
    payload.a = 0x42;
    payload.b = 0x43;
    payload.c = 0x44;

    // When serializing it into a byte stream, with a strategy that only cares about the positions in the structure
    serialization::Serializer<test::OtherFakeSerializationStrategy> unit{test::OtherFakeSerializationStrategy{}};
    EXPECT_TRUE(unit.Serialize(payload, this->buffer));

    // Then the layout is as expected
    auto data = this->buffer.begin();
    EXPECT_EQ(*data++, 0x01);
    EXPECT_EQ(*data++, 0x02);
    EXPECT_EQ(*data++, 0x03);
}

TYPED_TEST_P(SerializerFixture, CanDeserializeSimpleType)
{
    // Given a simple type that is serialized
    Serializer<test::FakeSerializationStrategy> unit{test::FakeSerializationStrategy{false}};
    const std::uint32_t expected_value{0x04030201};
    std::uint32_t payload{};
    ASSERT_TRUE(unit.Serialize(expected_value, this->buffer));

    // When deserializing it
    const auto result = unit.DeSerialize(this->buffer, payload);

    // Then the payload contains the expected value
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 4);
    EXPECT_EQ(payload, expected_value);
}

TYPED_TEST_P(SerializerFixture, ToSmallBufferForDeserialization)
{
    // Given a too small buffer for the payload
    std::uint32_t payload{};
    std::vector<std::uint8_t> local_buffer{0x01, 0x02, 0x03};

    // When deserializing it
    Serializer<test::FakeSerializationStrategy> unit{test::FakeSerializationStrategy{false}};
    const auto result = unit.DeSerialize(local_buffer, payload);

    // Then an error is reported
    EXPECT_FALSE(result.has_value());
}

TYPED_TEST_P(SerializerFixture, DeserialzingCorrectlyAssingsFieldOrder)
{
    // Given a simple data structure
    OtherPayload payload{};

    // When deserializing it
    serialization::Serializer<test::OtherFakeSerializationStrategy> unit{test::OtherFakeSerializationStrategy{}};
    EXPECT_TRUE(unit.DeSerialize(this->buffer, payload));

    // Then the layout is as expected (since the strategy only cares about the position, not the buffer content
    OtherPayload expected_payload{};
    expected_payload.a = 0x01;
    expected_payload.b = 0x02;
    expected_payload.c = 0x03;

    EXPECT_EQ(expected_payload.a, payload.a);
    EXPECT_EQ(expected_payload.b, payload.b);
    EXPECT_EQ(expected_payload.c, payload.c);
}

TYPED_TEST_P(SerializerFixture, StrategyCanReportErrors)
{
    // Given a simple data structure
    std::vector<std::uint8_t> payload{};

    // When serializing a error is reported
    serialization::Serializer<test::OtherFakeSerializationStrategy> unit{test::OtherFakeSerializationStrategy{}};
    const auto result = unit.DeSerialize(this->buffer, payload);

    // Then the serialization fails
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), test::OtherFakeSerializationStrategy::expected_error);
}

TYPED_TEST_P(SerializerFixture, CompleteDeserializationCanReportErrors)
{
    // Given a simple data structure
    std::vector<std::uint8_t> payload{};

    // When serializing a error is reported
    serialization::Serializer<test::OtherFakeSerializationStrategy> unit{test::OtherFakeSerializationStrategy{}};
    const auto result = unit.DeSerializeCompleteData(this->buffer, payload);

    // Then the serialization fails
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), test::OtherFakeSerializationStrategy::expected_error);
}

TEST_F(SerializationTest, CompleteDeserializationFail)
{
    // Given a small buffer for the payload
    std::uint16_t payload{};
    std::vector<std::uint8_t> local_buffer{0x01, 0x02, 0x03, 0x04};

    // When deserializing it
    Serializer<test::FakeSerializationStrategy> unit{test::FakeSerializationStrategy{false}};
    const auto result = unit.DeSerializeCompleteData(local_buffer, payload);

    // Then an error is reported
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), SerializationError::kNotAllDataDeserialized);
}

TEST_F(SerializationTest, CompleteDeserializationPass)
{
    std::uint32_t payload{};
    const std::uint32_t expected_value{0x04030201};
    std::vector<std::uint8_t> local_buffer{0x01, 0x02, 0x03, 0x04};

    // When deserializing it
    Serializer<test::FakeSerializationStrategy> unit{test::FakeSerializationStrategy{false}};
    const auto result = unit.DeSerializeCompleteData(local_buffer, payload);

    // Then the payload contains the expected value
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 4);
    EXPECT_EQ(payload, expected_value);
}

REGISTER_TYPED_TEST_SUITE_P(SerializerFixture,
                            CanSerializeComplexDataStructure,
                            SerializationStrategyCanBeCustomizable,
                            SerializationStrategyCanUseMetaInformation,
                            CanDeserializeSimpleType,
                            ToSmallBufferForDeserialization,
                            DeserialzingCorrectlyAssingsFieldOrder,
                            StrategyCanReportErrors,
                            CompleteDeserializationCanReportErrors);

using Types = testing::Types<std::vector<std::uint8_t>,
                             std::array<std::uint8_t, 512>,
                             score::cpp::static_vector<std::uint8_t, 512>,
                             std::list<std::uint8_t>,
                             std::deque<std::uint8_t>,
                             score::cpp::span<std::uint8_t>>;
INSTANTIATE_TYPED_TEST_SUITE_P(TestPrefix, SerializerFixture, Types, /*unused*/);

}  // namespace
}  // namespace serialization
}  // namespace score
