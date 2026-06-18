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
 *        \brief  Contains tests concerning the JsonOps class.
 *
 *********************************************************************************************************************/
/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <sstream>
#include <string>
#include <string_view>

#include "gtest/gtest.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/json_ops.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/json_data.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/json_error_domain.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/types.h"

namespace score
{
namespace json
{
namespace vajson
{
namespace unit_test
{

namespace
{
/// \brief Helper to create a JsonData + JsonOps pair from a string literal.
struct TestStream
{
    std::istringstream iss;
    JsonData data;
    internal::JsonOps ops;

    explicit TestStream(const std::string& input) : iss{input}, data{iss}, ops{data} {}
};
}  // namespace

/*!
 * \brief           Test that Skip can skip characters and handles eof.
 * \trace           score::json::vajson::internal::JsonOps::Skip
 */
TEST(UT__JsonOps, Skip)
{
    TestStream ts{"ab"};

    ASSERT_TRUE(ts.ops.Skip('a'));
    ASSERT_FALSE(ts.ops.Skip('a'));  // 'b' is next, not 'a'
    ASSERT_TRUE(ts.ops.Skip('b'));
    ASSERT_FALSE(ts.ops.Skip('b'));  // EOF
}

/*!
 * Test that Take can take characters.
 * \trace           score::json::vajson::internal::JsonOps::Take
 */
TEST(UT__JsonOps, Take)
{
    TestStream ts{"ab"};

    ASSERT_EQ(ts.ops.Take(), 'a');
}

/*!
 * Test that TryTake returns an error if position is at end of string.
 * \trace           score::json::vajson::internal::JsonOps::TryTake
 */
TEST(UT__JsonOps, TryTake__Error)
{
    TestStream ts{"ab"};

    {
        const auto r1 = ts.ops.TryTake();
        ASSERT_TRUE(r1.has_value());
        ASSERT_EQ(r1.value(), 'a');
    }
    {
        const auto r2 = ts.ops.TryTake();
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(r2.value(), 'b');
    }
    {
        const auto r3 = ts.ops.TryTake();
        ASSERT_FALSE(r3.has_value());
        ASSERT_EQ(r3.error(), JsonErrc::kStreamFailure);
    }
}

/*!
 * Test that Move moves the stream position and Tell returns the correct position.
 *
 * - Assert that the initial position returned by Tell is 0.
 * - Assert that Move succeeds if there are still characters in the stream.
 * - Assert that Tell returns the new position.
 * \trace           score::json::vajson::internal::JsonOps::Tell
 * \trace           score::json::vajson::internal::JsonOps::Move
 */
TEST(UT__JsonOps, MoveAndTell)
{
    TestStream ts{"ab"};

    {
        const auto tell_result = ts.ops.Tell();
        ASSERT_TRUE(tell_result.has_value());
        ASSERT_EQ(tell_result.value(), 0U);
    }
    ASSERT_TRUE(ts.ops.Move());
    {
        const auto tell_result = ts.ops.Tell();
        ASSERT_TRUE(tell_result.has_value());
        ASSERT_EQ(tell_result.value(), 1U);
    }
}

/*!
 * Test that CheckString can check the next characters.
 *
 * - Assert that CheckString succeeds if the expected string comes next.
 * - Assert that CheckString fails if the next string is not as expected.
 * \trace           score::json::vajson::internal::JsonOps::CheckString
 */
TEST(UT__JsonOps, CheckString)
{
    std::string_view const expected{"expected string"};

    {
        TestStream ts{std::string{expected}};
        const auto result = ts.ops.CheckString(expected, "No error");
        ASSERT_TRUE(result.has_value());
    }

    {
        TestStream ts{std::string{expected}};
        const auto result = ts.ops.CheckString("different string", "No error");
        ASSERT_FALSE(result.has_value());
        ASSERT_EQ(result.error(), JsonErrc::kStreamFailure);
    }
}

/*!
 * Test that ReadString aborts if the requested string is empty.
 * \trace           score::json::vajson::internal::JsonOps::ReadString
 */
TEST(UT__JsonOps, ReadStringAbortsOnEmpty)
{
    TestStream ts{"Does not matter"};

    ASSERT_DEATH(ts.ops.ReadString(""), "JsonOps::ReadString: Cannot check for empty string");
}

/*!
 * Test that SkipWhitespace skips whitespace characters.
 *
 * - Assert that all whitespace characters are skipped.
 * - Assert that an EOF after whitespace characters is recognized.
 * \trace           score::json::vajson::internal::JsonOps::SkipWhitespace
 */
TEST(UT__JsonOps, SkipWhitespace)
{
    {
        TestStream ts{"  \t\n\r  a"};
        ASSERT_TRUE(ts.ops.SkipWhitespace());
    }
    {
        TestStream ts{"  \t\n\r  "};
        ASSERT_FALSE(ts.ops.SkipWhitespace());
    }
}

/*!
 * Test that ReadExactly can read characters without buffering.
 *
 * - Create an input stream that is small enough to be read at once.
 * - Assert that the callback is invoked only once and gets the entire input.
 * \trace           score::json::vajson::internal::JsonOps::ReadExactly
 */
TEST(UT__JsonOps, ReadExactly__Unbuffered)
{
    std::string const input{"123456789"};
    TestStream ts{input};

    std::size_t count{0};
    std::size_t read{0};

    const auto result = ts.ops.ReadExactly(input.size(), [&count, &read](std::string_view view) noexcept {
        ++count;
        read = view.size();
    });
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(count, 1U);
    ASSERT_EQ(read, input.size());
}

/*!
 * Test that ReadExactly detects an unexpected EOF.
 *
 * - Assert that when requesting more characters than available in the stream the callback is never called and an error
 *   is returned instead.
 * \trace           score::json::vajson::internal::JsonOps::ReadExactly
 */
TEST(UT__JsonOps, ReadExactly__UnexpectedEof)
{
    std::string const input{"123456789"};
    TestStream ts{input};

    const auto result = ts.ops.ReadExactly(input.size() + 1U, [](std::string_view) noexcept {
        FAIL();
    });
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), JsonErrc::kInvalidJson);
    ASSERT_EQ(result.error().UserMessage(), std::string_view{"JsonOps::ReadExactly: Unexpected EOF."});
}

/*!
 * Test that ReadUntil reads until the specified delimiter.
 *
 * - Assert that the callback is invoked and gets all characters excluding the specified delimiter.
 * \trace           score::json::vajson::internal::JsonOps::ReadUntil
 */
TEST(UT__JsonOps, ReadUntil)
{
    std::string const input{"123456789:"};
    TestStream ts{input};

    std::string_view const expected_content{input.data(), input.size() - 1};

    const auto result = ts.ops.ReadUntil(":", [&expected_content](std::string_view view) noexcept {
        ASSERT_EQ(view, expected_content);
    });
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value().Value(), ':');
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
