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
#include "score/json/internal/parser/vajson/number_parser.h"

#include "gtest/gtest.h"

namespace score
{
namespace json
{
namespace vajson
{
namespace unit_test
{

/// @brief           Tests if IsDigit works as expected.
/// @trace           CREQ-Json-ProvidedSerializers
TEST(NumberParserTest, IsDigitAcceptsOnlyDecimalDigits)
{
    ASSERT_FALSE(internal::util::IsDigit('/'));
    ASSERT_TRUE(internal::util::IsDigit('0'));
    ASSERT_TRUE(internal::util::IsDigit('9'));
    ASSERT_FALSE(internal::util::IsDigit(':'));
}

/// @brief           Tests if IsHexDigit works as expected.
TEST(NumberParserTest, IsHexDigitAcceptsOnlyHexadecimalDigits)
{
    RecordProperty("PartiallyVerifies", "comp_req__vajson__hex_integers");
    RecordProperty("Description",
                   "Check that decimal digits and uppercase and lowercase hexadecimal letters are accepted as "
                   "hexadecimal digits, while other characters are rejected.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    ASSERT_TRUE(internal::util::IsHexDigit('0'));
    ASSERT_TRUE(internal::util::IsHexDigit('9'));
    ASSERT_TRUE(internal::util::IsHexDigit('a'));
    ASSERT_TRUE(internal::util::IsHexDigit('f'));
    ASSERT_TRUE(internal::util::IsHexDigit('A'));
    ASSERT_TRUE(internal::util::IsHexDigit('F'));
    ASSERT_FALSE(internal::util::IsHexDigit('g'));
    ASSERT_FALSE(internal::util::IsHexDigit('G'));
    ASSERT_FALSE(internal::util::IsHexDigit('x'));
    ASSERT_FALSE(internal::util::IsHexDigit('\0'));
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
