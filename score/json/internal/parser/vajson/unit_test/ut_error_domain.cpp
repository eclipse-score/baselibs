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
/*!        \file
 *        \brief  Contains tests concerning the vaJson error domain.
 *
 *********************************************************************************************************************/
/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <string>
#include <utility>

#include "gtest/gtest.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/json_error_domain.h"

namespace score
{
namespace json
{
namespace vajson
{
namespace unit_test
{

/*!
 * Test vaJson error domain messages.
 */
TEST(UT__ErrorDomain, General)
{
    // Unknown error code (below range)
    ASSERT_EQ(GetJsonDomain().MessageFor(-1), std::string_view{"Unknown error."});

    // Ordinary error code for kInvalidJson (value 2)
    ASSERT_EQ(GetJsonDomain().MessageFor(static_cast<score::result::ErrorCode>(JsonErrc::kInvalidJson)),
              std::string_view{"kInvalidJson: Invalid JSON was encountered."});

    // Too large error code
    ASSERT_EQ(GetJsonDomain().MessageFor(4242), std::string_view{"Unknown error."});
}

/*!
 * Test that MakeResult creates an error from an empty Optional, and a value from a non-empty Optional.
 *
 * \trace           score::json::vajson::MakeResult
 */
TEST(UT__ErrorDomain, MakeResult__FromOptional)
{
    {
        Optional<std::string> opt{};
        const Result<std::string> result{
            MakeResult(std::move(opt), MakeError(JsonErrc::kNotInitialized, "This is my error message"))};

        ASSERT_FALSE(result.has_value());
        ASSERT_EQ(result.error(), JsonErrc::kNotInitialized);
        ASSERT_EQ(result.error().UserMessage(), std::string_view{"This is my error message"});
    }
    {
        Optional<std::string> opt_2{};
        opt_2.emplace(std::string("SomeString"));
        const Result<std::string> result_2{
            MakeResult(opt_2, MakeError(JsonErrc::kNotInitialized, "This is my error message"))};
        ASSERT_TRUE(result_2.has_value());
        ASSERT_EQ(result_2.value(), "SomeString");
    }
}

/*!
 * Test that MakeResult creates an error from boolean false, and succeeds from boolean true.
 *
 * \trace           score::json::vajson::MakeResult
 */
TEST(UT__ErrorDomain, MakeResult__FromValue)
{
    const Result<void> result{MakeResult(false, MakeError(JsonErrc::kNotInitialized, "This is my error message"))};

    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), JsonErrc::kNotInitialized);
    ASSERT_EQ(result.error().UserMessage(), std::string_view{"This is my error message"});

    const Result<void> result2{MakeResult(true, MakeError(JsonErrc::kNotInitialized, "This is my error message"))};
    ASSERT_TRUE(result2.has_value());
}

/*!
 * Test that AssertCondition aborts with the correct message if passed 'false' and does not abort if passed 'true'.
 *
 * \trace           score::json::vajson::AssertCondition
 */
TEST(UT__ErrorDomain__DeathTest, AssertCondition)
{
    ASSERT_DEATH(AssertCondition(false, "Message to abort with"), "Message to abort with");
    ASSERT_NO_FATAL_FAILURE(AssertCondition(true));
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
