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

#include "score/flatbuffers/buffer_version_info.hpp"

#include <gtest/gtest.h>

#include <cstdint>

namespace score
{
namespace flatbuffers
{
namespace unit_test
{

struct InequalityParam
{
    score::flatbuffers::BufferVersionInfo lhs;
    score::flatbuffers::BufferVersionInfo rhs;
    bool expected_equal;
    const char* name;
};

class BufferVersionInfoEqualityTest : public ::testing::TestWithParam<InequalityParam>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    BoundaryValues,
    BufferVersionInfoEqualityTest,
    ::testing::Values(
        InequalityParam{{"DEMO", 2U,         3U}, {"DEMO", 2U,              3U}, true,  "EqualMidValues"},
        InequalityParam{{"    ", 2U,         3U}, {"DEMO", 2U,              3U}, false, "IdentifierMinVsMid"},
        InequalityParam{{"~~~~", 2U,         3U}, {"DEMO", 2U,              3U}, false, "IdentifierMaxVsMid"},
        InequalityParam{{"DEMO", 0U,         3U}, {"DEMO", 1U,              3U}, false, "MajorMinVsMinPlusOne"},
        InequalityParam{{"DEMO", UINT16_MAX, 3U}, {"DEMO", UINT16_MAX - 1U, 3U}, false, "MajorMaxVsMaxMinusOne"},
        InequalityParam{{"DEMO", 2U,         0U}, {"DEMO", 2U,              1U}, false, "MinorMinVsMinPlusOne"},
        InequalityParam{{"DEMO", 2U, UINT16_MAX}, {"DEMO", 2U, UINT16_MAX - 1U}, false, "MinorMaxVsMaxMinusOne"}
    ),
    [](const ::testing::TestParamInfo<InequalityParam>& info) { return info.param.name; }
);
// clang-format on

TEST_P(BufferVersionInfoEqualityTest, OperatorEquals)
{
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description", "operator== returns true when all fields are equal and false when any field differs");

    const auto& p = GetParam();
    EXPECT_EQ(p.lhs == p.rhs, p.expected_equal);
}

TEST_P(BufferVersionInfoEqualityTest, OperatorNotEquals)
{
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description",
                   "operator!= returns the logical negation of operator==: "
                   "false when all fields are equal and true when any field differs");

    const auto& p = GetParam();
    // operator!= must be the logical complement of operator==.
    EXPECT_EQ(p.lhs != p.rhs, !p.expected_equal);
}

}  // namespace unit_test
}  // namespace flatbuffers
}  // namespace score
