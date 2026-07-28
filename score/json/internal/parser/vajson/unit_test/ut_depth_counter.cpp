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
 *        \brief  Unit tests for the depth counter
 *
 *********************************************************************************************************************/
#include <utility>

#include "gtest/gtest.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/depth_counter.h"
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
 * \brief           Test that DepthCounter can be move assigned
 * \trace           score::json::vajson::internal::DepthCounter::DepthCounter
 */
TEST(UT__DepthCounter, MoveAssignment)
{
    internal::DepthCounter dc{};
    score::cpp::ignore = dc.AddArray();
    score::cpp::ignore = dc.AddValue();
    auto result = dc.CheckEndOfFile();
    ASSERT_EQ(result.error(), JsonErrc::kInvalidJson);
    ASSERT_EQ(result.error().UserMessage(),
              std::string_view{"DepthCounter::CheckEndOfFile: Expected closing brackets."});

    internal::DepthCounter moved_dc{};
    moved_dc = std::move(dc);

    ASSERT_TRUE(moved_dc.PopArray().has_value());
    ASSERT_TRUE(moved_dc.CheckEndOfFile().has_value());
}

/*!
 * Verify that the move constructor of the DepthCounter works.
 *
 * - Create a DepthCounter.
 * - Add an Array and a Value.
 * - Move construct another DepthCounter from the first one.
 * - Assert that the array can be popped.
 *
 * \trace           score::json::vajson::internal::DepthCounter::DepthCounter
 */
TEST(UT__DepthCounter, MoveConstruction)
{
    internal::DepthCounter dc{};
    score::cpp::ignore = dc.AddArray();
    score::cpp::ignore = dc.AddValue();

    const auto result = dc.CheckEndOfFile();
    ASSERT_EQ(result.error(), JsonErrc::kInvalidJson);
    ASSERT_EQ(result.error().UserMessage(),
              std::string_view{"DepthCounter::CheckEndOfFile: Expected closing brackets."});

    internal::DepthCounter moved_dc{std::move(dc)};

    ASSERT_TRUE(moved_dc.PopArray().has_value());
    ASSERT_TRUE(moved_dc.CheckEndOfFile().has_value());
}

/*!
 * \brief           Test that CheckEndOfFile returns an error if the document is empty
 * \trace           CREQ-Json-Validation
 */
TEST(UT__DepthCounter, CheckEndOfFile__EmptyDocument)
{
    internal::DepthCounter dc{};
    const auto result = dc.CheckEndOfFile();
    ASSERT_EQ(result.error(), JsonErrc::kInvalidJson);
    ASSERT_EQ(result.error().UserMessage(), std::string_view{"DepthCounter::CheckEndOfFile: Empty document."});
}

/*!
 * \brief           Test that CheckEndOfFile returns an error if an array is left open
 * \trace           CREQ-Json-Validation
 */
TEST(UT__DepthCounter, CheckEndOfFile__ArrayOpen)
{
    internal::DepthCounter dc{};
    ASSERT_TRUE(dc.AddArray().has_value());
    const auto result = dc.CheckEndOfFile();
    ASSERT_EQ(result.error(), JsonErrc::kInvalidJson);
    ASSERT_EQ(result.error().UserMessage(),
              std::string_view{"DepthCounter::CheckEndOfFile: Expected closing brackets."});
}

/*!
 * \brief           Test that CheckEndOfFile returns an error if an object is left open
 * \trace           CREQ-Json-Validation
 */
TEST(UT__DepthCounter, CheckEndOfFile__ObjectOpen)
{
    internal::DepthCounter dc{};
    ASSERT_TRUE(dc.AddObject().has_value());
    const auto result = dc.CheckEndOfFile();
    ASSERT_EQ(result.error(), JsonErrc::kInvalidJson);
    ASSERT_EQ(result.error().UserMessage(), std::string_view{"DepthCounter::CheckEndOfFile: Expected closing braces."});
}

/*!
 * \brief           Test that CheckEndOfFile succeeds if all elements are closed
 * \trace           CREQ-Json-Validation
 */
TEST(UT__DepthCounter, CheckEndOfFile__Valid)
{
    internal::DepthCounter dc{};
    ASSERT_TRUE(dc.AddArray().has_value());
    ASSERT_TRUE(dc.AddObject().has_value());
    ASSERT_TRUE(dc.PopObject().has_value());
    ASSERT_TRUE(dc.PopArray().has_value());
    ASSERT_TRUE(dc.CheckEndOfFile().has_value());
}

/*!
 * \brief           Test that CheckEndOfFile succeeds if the document contains only a single value
 * \trace           CREQ-Json-Validation
 */
TEST(UT__DepthCounter, CheckEndOfFile__SingleValue)
{
    internal::DepthCounter dc{};
    ASSERT_TRUE(dc.AddValue().has_value());
    ASSERT_TRUE(dc.CheckEndOfFile().has_value());
}

/*!
 * \brief           Test that AddKey returns an error if no element is open
 * \trace           CREQ-Json-Validation
 */
TEST(UT__DepthCounter, AddKey__OnlyKey)
{
    internal::DepthCounter dc{};
    const auto result = dc.AddKey();
    ASSERT_EQ(result.error(), JsonErrc::kInvalidJson);
    ASSERT_EQ(result.error().UserMessage(), std::string_view{"DepthCounter::AddKey: Expected a value."});
}

/*!
 * \brief           Test that AddKey succeeds inside elements
 * \trace           CREQ-Json-Validation
 */
TEST(UT__DepthCounter, AddKey__Valid)
{
    internal::DepthCounter dc{};
    ASSERT_TRUE(dc.AddObject().has_value());
    ASSERT_TRUE(dc.AddKey().has_value());
}

/*!
 * \brief           Test that AddKey returns an error if a comma is expected but not encountered
 * \trace           CREQ-Json-Validation
 */
TEST(UT__DepthCounter, AddKey__NoComma)
{
    internal::DepthCounter dc{};
    ASSERT_TRUE(dc.AddObject().has_value());
    ASSERT_TRUE(dc.AddKey().has_value());
    ASSERT_TRUE(dc.AddValue().has_value());
    const auto result = dc.AddKey();
    ASSERT_EQ(result.error(), JsonErrc::kInvalidJson);
    ASSERT_EQ(result.error().UserMessage(), std::string_view{"DepthCounter::AddKey: Missing comma."});
}

/*!
 * \brief           Test that AddKey returns an error if there are multiple top level elements
 * \trace           CREQ-Json-Validation
 */
TEST(UT__DepthCounter, AddKey__MultipleTopLevel)
{
    internal::DepthCounter dc{};
    ASSERT_TRUE(dc.AddObject().has_value());
    ASSERT_TRUE(dc.PopObject().has_value());
    const auto result = dc.AddKey();
    ASSERT_EQ(result.error(), JsonErrc::kInvalidJson);
    ASSERT_EQ(result.error().UserMessage(), std::string_view{"DepthCounter::AddKey: Multiple top level elements."});
}

/*!
 * \brief           Test that AddValue succeeds initially and fails consecutively
 * \trace           CREQ-Json-Validation
 */
TEST(UT__DepthCounter, AddValue__SingleValue)
{
    internal::DepthCounter dc{};
    ASSERT_TRUE(dc.AddValue().has_value());
    const auto result = dc.AddValue();
    ASSERT_EQ(result.error(), JsonErrc::kInvalidJson);
    ASSERT_EQ(result.error().UserMessage(), std::string_view{"DepthCounter::AddValue: Multiple top level elements."});
}

/*!
 * \brief           Test that AddValue succeeds inside elements
 * \trace           CREQ-Json-Validation
 */
TEST(UT__DepthCounter, AddValue__InsideArray)
{
    internal::DepthCounter dc{};
    ASSERT_TRUE(dc.AddArray().has_value());
    ASSERT_TRUE(dc.AddValue().has_value());
}

/*!
 * \brief           Test that AddValue returns an error if no key comes first
 * \trace           CREQ-Json-Validation
 */
TEST(UT__DepthCounter, AddValue__NoKey)
{
    internal::DepthCounter dc{};
    ASSERT_TRUE(dc.AddObject().has_value());
    const auto result = dc.AddValue();
    ASSERT_EQ(result.error(), JsonErrc::kInvalidJson);
    ASSERT_EQ(result.error().UserMessage(), std::string_view{"DepthCounter::AddValue: Expected a key."});
}

/*!
 * \brief           Test that AddValue returns an error if a comma is expected but not encountered
 * \trace           CREQ-Json-Validation
 */
TEST(UT__DepthCounter, AddValue__NoComma)
{
    internal::DepthCounter dc{};
    ASSERT_TRUE(dc.AddArray().has_value());
    ASSERT_TRUE(dc.AddValue().has_value());
    const auto result = dc.AddValue();
    ASSERT_EQ(result.error(), JsonErrc::kInvalidJson);
    ASSERT_EQ(result.error().UserMessage(), std::string_view{"DepthCounter::AddValue: Missing comma."});
}

/*!
 * \brief           Test that AddValue returns an error if there are multiple top level elements
 * \trace           CREQ-Json-Validation
 */
TEST(UT__DepthCounter, AddValue__MultipleTopLevel)
{
    internal::DepthCounter dc{};
    ASSERT_TRUE(dc.AddValue().has_value());
    const auto result = dc.AddValue();
    ASSERT_EQ(result.error(), JsonErrc::kInvalidJson);
    ASSERT_EQ(result.error().UserMessage(), std::string_view{"DepthCounter::AddValue: Multiple top level elements."});
}

/*!
 * \brief           Test that AddElement returns an error if there are multiple top level elements
 * \trace           CREQ-Json-Validation
 */
TEST(UT__DepthCounter, AddElement__MultipleTopLevel)
{
    internal::DepthCounter dc{};
    ASSERT_TRUE(dc.AddValue().has_value());
    const auto result = dc.AddArray();
    ASSERT_EQ(result.error(), JsonErrc::kInvalidJson);
    ASSERT_EQ(result.error().UserMessage(), std::string_view{"DepthCounter::AddElement: Multiple top level elements."});
}

/*!
 * \brief           Test that AddElement returns an error if no key comes next
 * \trace           CREQ-Json-Validation
 */
TEST(UT__DepthCounter, AddElement__NoKey)
{
    internal::DepthCounter dc{};
    ASSERT_TRUE(dc.AddObject().has_value());
    const auto result = dc.AddArray();
    ASSERT_EQ(result.error(), JsonErrc::kInvalidJson);
    ASSERT_EQ(result.error().UserMessage(), std::string_view{"DepthCounter::AddElement: Expected a key."});
}

/*!
 * \brief           Test that AddElement returns an error if a comma is expected but not encountered
 * \trace           CREQ-Json-Validation
 */
TEST(UT__DepthCounter, AddElement__NoComma)
{
    internal::DepthCounter dc{};
    ASSERT_TRUE(dc.AddArray().has_value());
    ASSERT_TRUE(dc.AddObject().has_value());
    ASSERT_TRUE(dc.PopObject().has_value());
    const auto result = dc.AddObject();
    ASSERT_EQ(result.error(), JsonErrc::kInvalidJson);
    ASSERT_EQ(result.error().UserMessage(), std::string_view{"DepthCounter::AddElement: Missing comma."});
}

/*!
 * \brief           Test that AddElement succeeds for a valid combination of elements
 * \trace           CREQ-Json-Validation
 */
TEST(UT__DepthCounter, AddElement__Valid)
{
    internal::DepthCounter dc{};
    ASSERT_TRUE(dc.AddObject().has_value());
    ASSERT_TRUE(dc.AddKey().has_value());
    ASSERT_TRUE(dc.AddArray().has_value());
}

/*!
 * \brief           Test that Pop method increments the counter of the outer container
 * \trace           CREQ-Json-Validation
 */
TEST(UT__DepthCounter, Pop)
{
    internal::DepthCounter dc{};
    ASSERT_TRUE(dc.AddObject().has_value());
    ASSERT_TRUE(dc.AddKey().has_value());
    ASSERT_TRUE(dc.AddObject().has_value());
    const auto pop_result1 = dc.PopObject();
    ASSERT_TRUE(pop_result1.has_value());
    ASSERT_EQ(pop_result1.value(), 0);
    const auto pop_result2 = dc.PopObject();
    ASSERT_TRUE(pop_result2.has_value());
    ASSERT_EQ(pop_result2.value(), 1);
}

/*!
 * \brief           Test AddComma method
 * \trace           CREQ-Json-Validation
 */
TEST(UT__DepthCounter, AddComma)
{
    internal::DepthCounter dc{};
    ASSERT_FALSE(dc.AddComma());

    ASSERT_TRUE(dc.AddObject().has_value());
    ASSERT_TRUE(dc.AddKey().has_value());
    ASSERT_FALSE(dc.AddComma());

    ASSERT_TRUE(dc.AddValue().has_value());
    ASSERT_TRUE(dc.AddComma());
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
