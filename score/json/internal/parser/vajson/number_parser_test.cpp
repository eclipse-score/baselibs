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
#ifdef VAJSON

#include "amsr/json/util/number.h"
#include "score/json/internal/parser/number_parser_test_suite.h"
#include "score/json/internal/parser/vajson/vajson_parser.h"

#include <limits>
#include <optional>

namespace score
{
namespace json
{
namespace
{

// These tests are written in way that they can only be used with vajson parser.
template <typename T>
std::optional<T> ParseNumberAs(const std::string& json_number)
{
    const auto result = amsr::json::JsonNumber::New(json_number).Value().As<T>();
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
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of bool data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    range_test<bool>("-1", "2");
}

TEST(Number, Uint8)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of uint8 data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    range_test<std::uint8_t>("-1", "256");
}

TEST(Number, Uint16)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of uint16 data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    range_test<std::uint16_t>("-1", "65536");
}

TEST(Number, Uint32)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of uint32 data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    range_test<std::uint32_t>("-1", "4294967296");
}

TEST(Number, Uint64)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of uint64 data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    range_test<std::uint64_t>("-1", "18446744073709551616");
}

TEST(Number, Int8)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of int8 data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    range_test<std::int8_t>("-129", "128");
}

TEST(Number, Int16)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of int16 data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    range_test<std::int16_t>("-32769", "32768");
}

TEST(Number, Int32)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of int32 data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    range_test<std::int32_t>("-2147483649", "2147483648");
}

TEST(Number, Int64)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of int64 data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    range_test<std::int64_t>("-9223372036854775809", "9223372036854775808");
}

TEST(Number, Float)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of float data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    range_test<float>("-3.402823476385288598117e+38", "3.402823476385288598117e+38");
}

TEST(Number, Double)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Test the limits, over-limit and under-limit of double data-type, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    range_test<double>("-1.797693134862415708145e+308", "1.797693134862415708145e+308");
}

TEST(Number, WithDecimalPointWithoutFractionalPartCannotBeParsedAsInteger)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Test interpreting number with decimal point as floating point only, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Tolerated behavior of vajson that interprets any number with decimal point as floating point only.
    EXPECT_EQ(ParseNumberAs<int64_t>("-1.0").has_value(), false);
}

TEST(Number, WithExponentialNotationWithoutFractionalPartCannotBeParsedAsInteger)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Test interpreting number with exponent notation as floating point only, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Tolerated behavior of vajson that interprets any number with exponent notation as floating point only.
    EXPECT_EQ(ParseNumberAs<int64_t>("-1e2").has_value(), false);
    EXPECT_EQ(ParseNumberAs<int64_t>("-10e-1").has_value(), false);
}

TEST(Number, FloatingPointWithoutDecimalPoint)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Number without decimal point can be treated as floating point too, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    EXPECT_EQ(ParseNumberAs<float>("-18446744073709551615").value_or(-999), -18446744073709551615.0f);
    EXPECT_EQ(ParseNumberAs<double>("18446744073709551615").value_or(-999), 18446744073709551615.0);
}

INSTANTIATE_TYPED_TEST_SUITE_P(Test, NumberTest, VajsonParser, /*unused*/);
}  // namespace
}  // namespace json
}  // namespace score
#else
GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(NumberTest);
#endif
