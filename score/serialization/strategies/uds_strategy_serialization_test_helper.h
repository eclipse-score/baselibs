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

#ifndef SCORE_SERIALIZATION_STRATEGIES_UDS_STRATEGY_SERIALIZATION_TEST_HELPER_H
#define SCORE_SERIALIZATION_STRATEGIES_UDS_STRATEGY_SERIALIZATION_TEST_HELPER_H

#include "score/serialization/serializer.h"
#include "score/serialization/strategies/hex_string_to_bytes.h"
#include "score/serialization/strategies/uds_strategy.h"

#include <score/vector.hpp>

#include <gtest/gtest.h>

namespace score
{
namespace serialization
{
template <typename T>
class SerializationTestFixture : public ::testing::TestWithParam<std::tuple<T, std::string>>
{
  public:
    SerializationTestFixture() : serializer_{{UDSStrategy{}}} {};
    void SetUp() override
    {
        expected_value_ = std::get<0>(gParameter);
        expected_value_bytes_ = HexStringToBytes(std::get<1>(gParameter));
    };

  protected:
    static std::tuple<T, std::string> gParameter;
    Serializer<UDSStrategy> serializer_;
    T expected_value_;
    score::cpp::pmr::vector<std::uint8_t> expected_value_bytes_;
};

TYPED_TEST_SUITE_P(SerializationTestFixture);

TYPED_TEST_P(SerializationTestFixture, CanDeSerialize)
{
    // Given a vector of bytes
    TypeParam value{};
    score::cpp::pmr::vector<std::uint8_t> serialized_payload{this->expected_value_bytes_};

    // Then it can be serialized into its respective type
    EXPECT_TRUE(this->serializer_.DeSerialize(serialized_payload, value).has_value());
    // And it matches the expected value (initialised object)
    EXPECT_EQ(value, this->expected_value_);
}

TYPED_TEST_P(SerializationTestFixture, DeSerializeEmptyPayload)
{
    // Given an empty serialized payload
    score::cpp::pmr::vector<std::uint8_t> serialized_payload{};

    TypeParam value{};
    TypeParam empty_value{};

    // When calling deserialize, we have two possibilities (depending on TypeParam):

    // - either an empty value is returned (for variable-size output)
    auto result = this->serializer_.DeSerialize(serialized_payload, value);
    if (result.has_value())
    {
        EXPECT_EQ(value, empty_value);
    }
    // - or the deserializer hits some fixed-size element in the output, and fails on the iterator
    else
    {
        EXPECT_EQ(result.error(), SerializationError::kOutOfBoundsWrite);
    }
}

TYPED_TEST_P(SerializationTestFixture, CanSerialize)
{
    // Given a object to serialize
    TypeParam value{this->expected_value_};
    score::cpp::pmr::vector<std::uint8_t> serialized_payload{};

    // Then it can be serialized into a vector of bytes
    EXPECT_TRUE(this->serializer_.Serialize(value, serialized_payload).has_value());
    // And it matches the expected vector of bytes
    EXPECT_EQ(serialized_payload, this->expected_value_bytes_);
}

REGISTER_TYPED_TEST_SUITE_P(SerializationTestFixture, CanSerialize, CanDeSerialize, DeSerializeEmptyPayload);

}  // namespace serialization
}  // namespace score

#endif  // SCORE_SERIALIZATION_STRATEGIES_UDS_STRATEGY_SERIALIZATION_TEST_HELPER_H
