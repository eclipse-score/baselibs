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
/// @file
/// @brief  Contains tests concerning the JsonOps class.
#include "score/json/internal/parser/vajson/json_ops.h"

#include <string>
#include <string_view>
#include <utility>

#include "gtest/gtest.h"
#include "score/json/internal/parser/vajson/json_data.h"
#include "score/json/internal/parser/vajson/json_error_domain.h"
#include "score/json/internal/parser/vajson/types.h"

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
/// @brief Helper to create a JsonData + JsonOps pair from a string.
struct TestDocument
{
    std::string input;
    JsonData data;
    internal::JsonOps ops;

    explicit TestDocument(std::string document) : input{std::move(document)}, data{input}, ops{data} {}
};
}  // namespace

/// @brief           Test that Skip can skip characters and handles eof.
/// @trace           score::json::vajson::internal::JsonOps::Skip
TEST(JsonOpsTest, SkipConsumesOnlyMatchingCharacter)
{
    TestDocument ts{"ab"};

    ASSERT_TRUE(ts.ops.Skip('a'));
    ASSERT_FALSE(ts.ops.Skip('a'));  // 'b' is next, not 'a'
    ASSERT_TRUE(ts.ops.Skip('b'));
    ASSERT_FALSE(ts.ops.Skip('b'));  // EOF
}

/// Test that Take can take characters.
/// @trace           score::json::vajson::internal::JsonOps::Take
TEST(JsonOpsTest, TakeReturnsNextCharacter)
{
    TestDocument ts{"ab"};

    ASSERT_EQ(ts.ops.Take(), 'a');
}

/// Test that TryTake returns an error if position is at end of string.
/// @trace           score::json::vajson::internal::JsonOps::TryTake
TEST(JsonOpsTest, TryTakeReturnsInvalidJsonAtEndOfInput)
{
    TestDocument ts{"ab"};

    {
        const auto r1{ts.ops.TryTake()};
        ASSERT_TRUE(r1.has_value());
        ASSERT_EQ(r1.value(), 'a');
    }
    {
        const auto r2{ts.ops.TryTake()};
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(r2.value(), 'b');
    }
    {
        const auto r3{ts.ops.TryTake()};
        ASSERT_FALSE(r3.has_value());
        ASSERT_EQ(r3.error(), JsonErrc::kInvalidJson);
    }
}

/// Test that Move skips the current character.
/// @trace           score::json::vajson::internal::JsonOps::Move
TEST(JsonOpsTest, MoveAdvancesToNextCharacter)
{
    TestDocument ts{"ab"};

    ASSERT_TRUE(ts.ops.Move());
    ASSERT_EQ(ts.ops.Take(), 'b');
}

/// Test that CheckString can check the next characters.
///
/// - Assert that CheckString succeeds if the expected string comes next.
/// - Assert that CheckString fails if the next string is not as expected.
/// @trace           score::json::vajson::internal::JsonOps::CheckString
TEST(JsonOpsTest, CheckStringAcceptsOnlyExpectedString)
{
    const std::string_view expected{"expected string"};

    {
        TestDocument ts{std::string{expected}};
        const auto result{ts.ops.CheckString(expected, "No error")};
        ASSERT_TRUE(result.has_value());
    }

    {
        TestDocument ts{std::string{expected}};
        const auto result{ts.ops.CheckString("different string", "No error")};
        ASSERT_FALSE(result.has_value());
        ASSERT_EQ(result.error(), JsonErrc::kInvalidJson);
    }
}

/// Test that ReadString aborts if the requested string is empty.
/// @trace           score::json::vajson::internal::JsonOps::ReadString
TEST(JsonOpsDeathTest, ReadStringAbortsOnEmptyString)
{
    TestDocument ts{"Does not matter"};

    ASSERT_DEATH(score::cpp::ignore = ts.ops.ReadString(""), "");
}

/// Test that SkipWhitespace skips whitespace characters.
///
/// - Assert that all whitespace characters are skipped.
/// - Assert that an EOF after whitespace characters is recognized.
/// @trace           score::json::vajson::internal::JsonOps::SkipWhitespace
TEST(JsonOpsTest, SkipWhitespaceReturnsWhetherNonWhitespaceFollows)
{
    {
        TestDocument ts{"  \t\n\r  a"};
        ASSERT_TRUE(ts.ops.SkipWhitespace());
    }
    {
        TestDocument ts{"  \t\n\r  "};
        ASSERT_FALSE(ts.ops.SkipWhitespace());
    }
}

/// Test that ReadUntil reads until the specified delimiter.
///
/// - Assert that the callback is invoked and gets all characters excluding the specified delimiter.
/// @trace           score::json::vajson::internal::JsonOps::ReadUntil
TEST(JsonOpsTest, ReadUntilPassesContentBeforeDelimiter)
{
    const std::string input{"123456789:"};
    TestDocument ts{input};

    const std::string_view expected_content{input.data(), input.size() - 1};

    const auto result{ts.ops.ReadUntil(":", [&expected_content](std::string_view view) noexcept {
        ASSERT_EQ(view, expected_content);
    })};
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value().Value(), ':');
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
