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
#ifndef SCORE_LIB_JSON_INTERNAL_PARSER_NUMBER_PARSER_TEST_SUITE_H
#define SCORE_LIB_JSON_INTERNAL_PARSER_NUMBER_PARSER_TEST_SUITE_H

#include "gtest/gtest.h"

#include <cmath>
#include <cstdint>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>

#include "score/json/internal/model/any.h"
#include "score/json/internal/model/object.h"
#include "score/result/result.h"

namespace score
{
namespace json
{
namespace
{

std::string buffer_json{R"(
{
    "bool_min":false,
    "bool_max":true,
    "uint8_min":0,
    "uint8_max":255,
    "uint16_min":0,
    "uint16_max":65535,
    "uint32_min":0,
    "uint32_max":4294967295,
    "uint64_min":0,
    "uint64_max":18446744073709551615,
    "int8_min":-128,
    "int8_max":127,
    "int16_min":-32768,
    "int16_max":32767,
    "int32_min":-2147483648,
    "int32_max":2147483647,
    "int64_min":-9223372036854775808,
    "int64_max":9223372036854775807,
    "float_min": 1.175494350822287507969e-38,
    "float_max": 3.402823466385288598117e+38,
    "double_min":2.225073858507201383090e-308,
    "double_max":1.797693134862315708145e+308
    }
)"};

template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
score::Result<T> GetValueOfObject(const score::json::Any& any, const std::string& key)
{
    return any.As<score::json::Object>().value().get().at(key).As<T>();
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
void CheckReturnedValues(score::Result<T> min, score::Result<T> max)
{
    EXPECT_TRUE(min.has_value());
    EXPECT_EQ(min.value(), std::numeric_limits<T>::min());
    EXPECT_TRUE(max.has_value());
    EXPECT_EQ(max, std::numeric_limits<T>::max());
}

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
void CheckReturnedValues(score::Result<T> min, score::Result<T> max)
{
    EXPECT_TRUE(min.has_value());
    auto diff = std::abs(min.value() - std::numeric_limits<T>::min());
    EXPECT_TRUE(diff < std::numeric_limits<T>::epsilon());

    EXPECT_TRUE(max.has_value());
    diff = std::abs(max.value() - std::numeric_limits<T>::max());
    EXPECT_TRUE(diff < std::numeric_limits<T>::epsilon());
}

template <typename T>
class NumberTest : public testing::Test
{
};
TYPED_TEST_SUITE_P(NumberTest);

TYPED_TEST_P(NumberTest, WhenParsingABoolStoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description",
                         "Test the limits, over-limit and under-limit of bool data-type, cf. RFC-8259 section 9");
    this->RecordProperty("TestType", "Requirements-based test");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Priority", "3");

    // Given a simple JSON buffer and parsing it
    auto root = TypeParam::FromBuffer(buffer_json);

    // Then parsing is successful
    EXPECT_TRUE(root.has_value());

    // When reading key values of an object that is interpreted as bool
    auto min = GetValueOfObject<bool>(root.value(), "bool_min");
    auto max = GetValueOfObject<bool>(root.value(), "bool_max");

    // Then the correct values are returned
    CheckReturnedValues<bool>(min, max);
}

TYPED_TEST_P(NumberTest, WhenParsingAUint8StoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description",
                         "Test the limits, over-limit and under-limit of uint8 data-type, cf. RFC-8259 section 9");
    this->RecordProperty("TestType", "Requirements-based test");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Priority", "3");

    // Given a simple JSON buffer and parsing it
    auto root = TypeParam::FromBuffer(buffer_json);

    // Then parsing is successful
    EXPECT_TRUE(root.has_value());

    // When reading key values of an object that is interpreted as std::uint8_t
    auto min = GetValueOfObject<std::uint8_t>(root.value(), "uint8_min");
    auto max = GetValueOfObject<std::uint8_t>(root.value(), "uint8_max");

    // Then the correct values are returned
    CheckReturnedValues<std::uint8_t>(min, max);
}

TYPED_TEST_P(NumberTest, WhenParsingAUint16StoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description",
                         "Test the limits, over-limit and under-limit of uint16 data-type, cf. RFC-8259 section 9");
    this->RecordProperty("TestType", "Requirements-based test");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Priority", "3");

    // Given a simple JSON buffer and parsing it
    auto root = TypeParam::FromBuffer(buffer_json);

    // Then parsing is successful
    EXPECT_TRUE(root.has_value());

    // When reading key values of an object that is interpreted as bool
    auto min = GetValueOfObject<std::uint16_t>(root.value(), "uint16_min");
    auto max = GetValueOfObject<std::uint16_t>(root.value(), "uint16_max");

    // Then the correct values are returned
    CheckReturnedValues<std::uint16_t>(min, max);
}

TYPED_TEST_P(NumberTest, WhenParsingAUint32StoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description",
                         "Test the limits, over-limit and under-limit of uint32 data-type, cf. RFC-8259 section 9");
    this->RecordProperty("TestType", "Requirements-based test");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Priority", "3");

    // Given a simple JSON buffer and parsing it
    auto root = TypeParam::FromBuffer(buffer_json);

    // Then parsing is successful
    EXPECT_TRUE(root.has_value());

    // When reading key values of an object that is interpreted as bool
    auto min = GetValueOfObject<std::uint32_t>(root.value(), "uint32_min");
    auto max = GetValueOfObject<std::uint32_t>(root.value(), "uint32_max");

    // Then the correct values are returned
    CheckReturnedValues<std::uint32_t>(min, max);
}

TYPED_TEST_P(NumberTest, WhenParsingAUint64StoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description",
                         "Test the limits, over-limit and under-limit of uint64 data-type, cf. RFC-8259 section 9");
    this->RecordProperty("TestType", "Requirements-based test");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Priority", "3");

    // Given a simple JSON buffer and parsing it
    auto root = TypeParam::FromBuffer(buffer_json);

    // Then parsing is successful
    EXPECT_TRUE(root.has_value());

    // When reading key values of an object that is interpreted as bool
    auto min = GetValueOfObject<std::uint64_t>(root.value(), "uint64_min");
    auto max = GetValueOfObject<std::uint64_t>(root.value(), "uint64_max");

    // Then the correct values are returned
    CheckReturnedValues<std::uint64_t>(min, max);
}

TYPED_TEST_P(NumberTest, WhenParsingAInt8StoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description",
                         "Test the limits, over-limit and under-limit of int8 data-type, cf. RFC-8259 section 9");
    this->RecordProperty("TestType", "Requirements-based test");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Priority", "3");

    // Given a simple JSON buffer and parsing it
    auto root = TypeParam::FromBuffer(buffer_json);

    // Then parsing is successful
    EXPECT_TRUE(root.has_value());

    // When reading key values of an object that is interpreted as bool
    auto min = GetValueOfObject<std::int8_t>(root.value(), "int8_min");
    auto max = GetValueOfObject<std::int8_t>(root.value(), "int8_max");

    // Then the correct values are returned
    CheckReturnedValues<std::int8_t>(min, max);
}

TYPED_TEST_P(NumberTest, WhenParsingAInt16StoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description",
                         "Test the limits, over-limit and under-limit of int16 data-type, cf. RFC-8259 section 9");
    this->RecordProperty("TestType", "Requirements-based test");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Priority", "3");

    // Given a simple JSON buffer and parsing it
    auto root = TypeParam::FromBuffer(buffer_json);

    // Then parsing is successful
    EXPECT_TRUE(root.has_value());

    // When reading key values of an object that is interpreted as bool
    auto min = GetValueOfObject<std::int16_t>(root.value(), "int16_min");
    auto max = GetValueOfObject<std::int16_t>(root.value(), "int16_max");

    // Then the correct values are returned
    CheckReturnedValues<std::int16_t>(min, max);
}

TYPED_TEST_P(NumberTest, WhenParsingAInt32StoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description",
                         "Test the limits, over-limit and under-limit of int32 data-type, cf. RFC-8259 section 9");
    this->RecordProperty("TestType", "Requirements-based test");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Priority", "3");

    // Given a simple JSON buffer and parsing it
    auto root = TypeParam::FromBuffer(buffer_json);

    // Then parsing is successful
    EXPECT_TRUE(root.has_value());

    // When reading key values of an object that is interpreted as bool
    auto min = GetValueOfObject<std::int32_t>(root.value(), "int32_min");
    auto max = GetValueOfObject<std::int32_t>(root.value(), "int32_max");

    // Then the correct values are returned
    CheckReturnedValues<std::int32_t>(min, max);
}

TYPED_TEST_P(NumberTest, WhenParsingAInt64StoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description",
                         "Test the limits, over-limit and under-limit of int64 data-type, cf. RFC-8259 section 9");
    this->RecordProperty("TestType", "Requirements-based test");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Priority", "3");

    // Given a simple JSON buffer and parsing it
    auto root = TypeParam::FromBuffer(buffer_json);

    // Then parsing is successful
    EXPECT_TRUE(root.has_value());

    // When reading key values of an object that is interpreted as bool
    auto min = GetValueOfObject<std::int64_t>(root.value(), "int64_min");
    auto max = GetValueOfObject<std::int64_t>(root.value(), "int64_max");

    // Then the correct values are returned
    CheckReturnedValues<std::int64_t>(min, max);
}
TYPED_TEST_P(NumberTest, WhenParsingAFloatStoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description",
                         "Test the limits, over-limit and under-limit of float data-type, cf. RFC-8259 section 9");
    this->RecordProperty("TestType", "Requirements-based test");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Priority", "3");

    // Given a simple JSON buffer and parsing it
    auto root = TypeParam::FromBuffer(buffer_json);

    // Then parsing is successful
    EXPECT_TRUE(root.has_value());

    // When reading key values of an object that is interpreted as double
    auto min = GetValueOfObject<double>(root.value(), "float_min");
    auto max = GetValueOfObject<double>(root.value(), "float_max");

    // Then the correct values are returned
    CheckReturnedValues<float>(min, max);
}

TYPED_TEST_P(NumberTest, WhenParsingADoubleStoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description",
                         "Test the limits, over-limit and under-limit of double data-type, cf. RFC-8259 section 9");
    this->RecordProperty("TestType", "Requirements-based test");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Priority", "3");

    // Given a simple JSON buffer and parsing it
    auto root = TypeParam::FromBuffer(buffer_json);

    // Then parsing is successful
    EXPECT_TRUE(root.has_value());

    // When reading key values of an object that is interpreted as double
    auto min = GetValueOfObject<double>(root.value(), "double_min");
    auto max = GetValueOfObject<double>(root.value(), "double_max");

    // Then the correct values are returned
    CheckReturnedValues<double>(min, max);
}

REGISTER_TYPED_TEST_SUITE_P(NumberTest,
                            WhenParsingABoolStoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned,
                            WhenParsingAUint8StoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned,
                            WhenParsingAUint16StoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned,
                            WhenParsingAUint32StoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned,
                            WhenParsingAUint64StoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned,
                            WhenParsingAInt8StoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned,
                            WhenParsingAInt16StoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned,
                            WhenParsingAInt32StoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned,
                            WhenParsingAInt64StoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned,
                            WhenParsingAFloatStoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned,
                            WhenParsingADoubleStoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned);

}  // namespace
}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_PARSER_NUMBER_PARSER_TEST_SUITE_H
