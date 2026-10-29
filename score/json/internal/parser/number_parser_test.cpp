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
#include "gtest/gtest.h"

#include "score/json/internal/model/any.h"
#include "score/json/internal/model/object.h"
#include "score/json/internal/parser/vajson/util/number.h"
#include "score/json/json_parser.h"
#include "score/result/result.h"

#include <cmath>
#include <cstdint>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <type_traits>

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
    this->RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    this->RecordProperty("Description",
                         "Check that parsing the JSON boolean values false and true returns the correct value.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "boundary-values");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    this->RecordProperty(
        "Description",
        "Check that parsing a JSON number at the uint8_t limits, cf. RFC-8259 section 9, returns the correct value.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "boundary-values");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    this->RecordProperty(
        "Description",
        "Check that parsing a JSON number at the uint16_t limits, cf. RFC-8259 section 9, returns the correct value.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "boundary-values");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    this->RecordProperty(
        "Description",
        "Check that parsing a JSON number at the uint32_t limits, cf. RFC-8259 section 9, returns the correct value.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "boundary-values");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    this->RecordProperty(
        "Description",
        "Check that parsing a JSON number at the uint64_t limits, cf. RFC-8259 section 9, returns the correct value.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "boundary-values");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    this->RecordProperty(
        "Description",
        "Check that parsing a JSON number at the int8_t limits, cf. RFC-8259 section 9, returns the correct value.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "boundary-values");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    this->RecordProperty(
        "Description",
        "Check that parsing a JSON number at the int16_t limits, cf. RFC-8259 section 9, returns the correct value.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "boundary-values");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    this->RecordProperty(
        "Description",
        "Check that parsing a JSON number at the int32_t limits, cf. RFC-8259 section 9, returns the correct value.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "boundary-values");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    this->RecordProperty(
        "Description",
        "Check that parsing a JSON number at the int64_t limits, cf. RFC-8259 section 9, returns the correct value.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "boundary-values");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    this->RecordProperty("Description",
                         "Check that parsing a JSON number at the float limits, cf. RFC-8259 section 9, returns the "
                         "correct value within its precision.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "boundary-values");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    this->RecordProperty("Description",
                         "Check that parsing a JSON number at the double limits, cf. RFC-8259 section 9, returns the "
                         "correct value within its precision.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "boundary-values");

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

/// @brief Gives the typed test suite a static entry point to JsonParser.
struct JsonParserEntryPoint
{
    static auto FromBuffer(const std::string_view buffer) -> score::Result<Any>
    {
        return JsonParser{}.FromBuffer(buffer);
    }
};

// These tests are written in way that they can only be used with vajson parser.
template <typename T>
std::optional<T> ParseNumberAs(const std::string& json_number)
{
    const auto result = score::json::vajson::JsonNumber::New(json_number).value().As<T>();
    if (result.has_value())
    {
        return result.value();
    }
    return {};
}

/**
 * @brief Smoke testing the basic ranges plus underflow and overflow.
 * Note: Exhausitve parser testing shall be done by the supplier of the underlying JSON parser.
 */
template <typename T>
void range_test(const ::std::string& underflow, const std::string& overflow)
{
    EXPECT_FALSE(ParseNumberAs<T>(underflow).has_value());
    EXPECT_FALSE(ParseNumberAs<T>(overflow).has_value());
}

TEST(Number, Bool)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of bool data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");          // requirements test
    RecordProperty("DerivationTechnique", "boundary-values");  // requirements

    range_test<bool>("-1", "2");
}

TEST(Number, Uint8)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of uint8 data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");          // requirements test
    RecordProperty("DerivationTechnique", "boundary-values");  // requirements

    range_test<std::uint8_t>("-1", "256");
}

TEST(Number, Uint16)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of uint16 data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");          // requirements test
    RecordProperty("DerivationTechnique", "boundary-values");  // requirements

    range_test<std::uint16_t>("-1", "65536");
}

TEST(Number, Uint32)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of uint32 data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");          // requirements test
    RecordProperty("DerivationTechnique", "boundary-values");  // requirements

    range_test<std::uint32_t>("-1", "4294967296");
}

TEST(Number, Uint64)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of uint64 data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");          // requirements test
    RecordProperty("DerivationTechnique", "boundary-values");  // requirements

    range_test<std::uint64_t>("-1", "18446744073709551616");
}

TEST(Number, Int8)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of int8 data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");          // requirements test
    RecordProperty("DerivationTechnique", "boundary-values");  // requirements

    range_test<std::int8_t>("-129", "128");
}

TEST(Number, Int16)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of int16 data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");          // requirements test
    RecordProperty("DerivationTechnique", "boundary-values");  // requirements

    range_test<std::int16_t>("-32769", "32768");
}

TEST(Number, Int32)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of int32 data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");          // requirements test
    RecordProperty("DerivationTechnique", "boundary-values");  // requirements

    range_test<std::int32_t>("-2147483649", "2147483648");
}

TEST(Number, Int64)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of int64 data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");          // requirements test
    RecordProperty("DerivationTechnique", "boundary-values");  // requirements

    range_test<std::int64_t>("-9223372036854775809", "9223372036854775808");
}

TEST(Number, Float)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of float data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");          // requirements test
    RecordProperty("DerivationTechnique", "boundary-values");  // requirements

    range_test<float>("-3.402823476385288598117e+38", "3.402823476385288598117e+38");
}

TEST(Number, Double)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of double data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");          // requirements test
    RecordProperty("DerivationTechnique", "boundary-values");  // requirements

    range_test<double>("-1.797693134862415708145e+308", "1.797693134862415708145e+308");
}

TEST(Number, WithDecimalPointWithoutFractionalPartCannotBeParsedAsInteger)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test interpreting number with decimal point as floating point only, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");                // requirements test
    RecordProperty("DerivationTechnique", "requirements-analysis");  // requirements

    // Tolerated behavior of vajson that interprets any number with decimal point as floating point only.
    EXPECT_EQ(ParseNumberAs<int64_t>("-1.0").has_value(), false);
}

TEST(Number, WithExponentialNotationWithoutFractionalPartCannotBeParsedAsInteger)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Test interpreting number with exponent notation as floating point only, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");                // requirements test
    RecordProperty("DerivationTechnique", "requirements-analysis");  // requirements

    // Tolerated behavior of vajson that interprets any number with exponent notation as floating point only.
    EXPECT_EQ(ParseNumberAs<int64_t>("-1e2").has_value(), false);
    EXPECT_EQ(ParseNumberAs<int64_t>("-10e-1").has_value(), false);
}

TEST(Number, FloatingPointWithoutDecimalPoint)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Number without decimal point can be treated as floating point too, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");                // requirements test
    RecordProperty("DerivationTechnique", "requirements-analysis");  // requirements

    EXPECT_EQ(ParseNumberAs<float>("-18446744073709551615").value_or(-999), -18446744073709551615.0f);
    EXPECT_EQ(ParseNumberAs<double>("18446744073709551615").value_or(-999), 18446744073709551615.0);
}

TEST(Number, NearDoubleLimitsIsParsable)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description", "Near-limit finite doubles are accepted across architectures.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    EXPECT_TRUE(ParseNumberAs<double>("2.225073858507201383090e-308").has_value());
    EXPECT_TRUE(ParseNumberAs<double>("1.797693134862315708145e+308").has_value());
}

TEST(Number, FloatingPointOverflowIsRejected)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description", "Overflowing floating-point values are rejected.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    EXPECT_FALSE(ParseNumberAs<double>("1e309").has_value());
    EXPECT_FALSE(ParseNumberAs<double>("-1e309").has_value());
}

INSTANTIATE_TYPED_TEST_SUITE_P(Test, NumberTest, JsonParserEntryPoint, /*unused*/);
}  // namespace
}  // namespace json
}  // namespace score
