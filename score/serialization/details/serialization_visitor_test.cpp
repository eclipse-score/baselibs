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

#include "score/serialization/details/serialization_visitor.h"

#include <gtest/gtest.h>

#include <cstddef>
#include <limits>

namespace score
{
namespace serialization
{
namespace
{

TEST(CommonVisitorLogicTest, DefaultsToNeitherFirstNorLastElement)
{
    CommonVisitorLogic logic{};
    const auto& meta = logic.GetMetaInformation();
    EXPECT_FALSE(meta.is_first_element_in_struct_);
    EXPECT_FALSE(meta.is_last_element_in_struct_);
    EXPECT_EQ(meta.field_index_, 0U);
}

TEST(CommonVisitorLogicTest, MarkAsFirstElementSetsFirstFlagOnly)
{
    CommonVisitorLogic logic{};
    logic.MarkAsLastElementInStruct();
    logic.MarkAsFirstElementInStruct();
    const auto& meta = logic.GetMetaInformation();
    EXPECT_TRUE(meta.is_first_element_in_struct_);
    EXPECT_FALSE(meta.is_last_element_in_struct_);
}

TEST(CommonVisitorLogicTest, MarkAsElementClearsBothFlags)
{
    CommonVisitorLogic logic{};
    logic.MarkAsFirstElementInStruct();
    logic.MarkAsElementInStruct();
    const auto& meta = logic.GetMetaInformation();
    EXPECT_FALSE(meta.is_first_element_in_struct_);
    EXPECT_FALSE(meta.is_last_element_in_struct_);
}

TEST(CommonVisitorLogicTest, MarkAsLastElementSetsLastFlagOnly)
{
    CommonVisitorLogic logic{};
    logic.MarkAsFirstElementInStruct();
    logic.MarkAsLastElementInStruct();
    const auto& meta = logic.GetMetaInformation();
    EXPECT_FALSE(meta.is_first_element_in_struct_);
    EXPECT_TRUE(meta.is_last_element_in_struct_);
}

TEST(CommonVisitorLogicTest, SetFieldIndexIsReflectedInMetaInformation)
{
    CommonVisitorLogic logic{};
    logic.SetFieldIndex(7U);
    EXPECT_EQ(logic.GetMetaInformation().field_index_, 7U);
}

TEST(CommonVisitorLogicTest, FieldDepthStartsAtZeroWithEmptyPaths)
{
    CommonVisitorLogic logic{};
    EXPECT_EQ(logic.FieldDepth(), 0U);
    EXPECT_EQ(logic.FieldPath().size(), 0);
    EXPECT_EQ(logic.LastPath().size(), 0);
}

TEST(CommonVisitorLogicTest, PushFieldRecordsIndexAndLastFlag)
{
    CommonVisitorLogic logic{};
    logic.PushField(3U, false);
    logic.PushField(5U, true);

    ASSERT_EQ(logic.FieldDepth(), 2U);

    const auto field_path = logic.FieldPath();
    ASSERT_EQ(field_path.size(), 2);
    EXPECT_EQ(field_path[0], 3U);
    EXPECT_EQ(field_path[1], 5U);

    const auto last_path = logic.LastPath();
    ASSERT_EQ(last_path.size(), 2);
    EXPECT_FALSE(last_path[0]);
    EXPECT_TRUE(last_path[1]);
}

TEST(CommonVisitorLogicTest, PopFieldRemovesMostRecentEntry)
{
    CommonVisitorLogic logic{};
    logic.PushField(3U, true);
    logic.PushField(5U, false);
    logic.PopField();

    ASSERT_EQ(logic.FieldDepth(), 1U);
    const auto field_path = logic.FieldPath();
    ASSERT_EQ(field_path.size(), 1);
    EXPECT_EQ(field_path[0], 3U);
    EXPECT_TRUE(logic.LastPath()[0]);
}

TEST(CommonVisitorLogicTest, PushArrayElementUsesArrayElementMarker)
{
    CommonVisitorLogic logic{};
    logic.PushArrayElement(true);

    ASSERT_EQ(logic.FieldDepth(), 1U);
    EXPECT_EQ(logic.FieldPath()[0], CommonVisitorLogic::kArrayElementMarker);
    EXPECT_EQ(CommonVisitorLogic::kArrayElementMarker, std::numeric_limits<std::size_t>::max());
    EXPECT_TRUE(logic.LastPath()[0]);
}

TEST(CommonVisitorLogicTest, PathsExposeOnlyValidDepth)
{
    CommonVisitorLogic logic{};
    for (std::size_t i = 0U; i < CommonVisitorLogic::kMaxNestingDepth; ++i)
    {
        logic.PushField(i, false);
    }
    EXPECT_EQ(logic.FieldDepth(), CommonVisitorLogic::kMaxNestingDepth);
    EXPECT_EQ(logic.FieldPath().size(), static_cast<std::ptrdiff_t>(CommonVisitorLogic::kMaxNestingDepth));
    EXPECT_EQ(logic.LastPath().size(), static_cast<std::ptrdiff_t>(CommonVisitorLogic::kMaxNestingDepth));

    logic.PopField();
    EXPECT_EQ(logic.FieldPath().size(), static_cast<std::ptrdiff_t>(CommonVisitorLogic::kMaxNestingDepth - 1U));
}

}  // namespace
}  // namespace serialization
}  // namespace score
