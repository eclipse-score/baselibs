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
#include "score/mw/log/detail/helper_functions.h"
#include <tuple>

#include "gtest/gtest.h"

namespace helper
{
using testing::Types;

template <typename TypeTuple>
class ClampToTestFixture : public ::testing::Test
{
  public:
    ClampToTestFixture() {}
};

TYPED_TEST_SUITE_P(ClampToTestFixture);

TYPED_TEST_P(ClampToTestFixture, UpClampTo)
{
    using UpSourceType = typename std::tuple_element<0, TypeParam>::type;
    using UpTargetType = typename std::tuple_element<1, TypeParam>::type;

    const UpSourceType expected_value = std::numeric_limits<UpSourceType>::max();
    UpSourceType input = std::numeric_limits<UpSourceType>::max();
    UpTargetType v = ClampTo<UpTargetType>(input);
    EXPECT_EQ(v, expected_value);
    EXPECT_GE(std::numeric_limits<UpTargetType>::max(), v);
}

TYPED_TEST_P(ClampToTestFixture, DownClampTo)
{
    using DownSourceType = typename std::tuple_element<1, TypeParam>::type;
    using DownTargetType = typename std::tuple_element<0, TypeParam>::type;

    EXPECT_GT(std::numeric_limits<DownSourceType>::max(), std::numeric_limits<DownTargetType>::max());

    const DownTargetType expected_value_no_loss = std::numeric_limits<DownTargetType>::max() - 1;
    DownSourceType input_no_loss = std::numeric_limits<DownTargetType>::max() - 1;
    DownTargetType v_no_loss = ClampTo<DownTargetType>(input_no_loss);
    EXPECT_EQ(v_no_loss, expected_value_no_loss);
    EXPECT_GE(std::numeric_limits<DownSourceType>::max(), v_no_loss);

    const DownTargetType expected_value_loss = std::numeric_limits<DownTargetType>::max();
    DownSourceType input_loss = std::numeric_limits<DownSourceType>::max() - 1;
    DownTargetType v_loss = ClampTo<DownTargetType>(input_loss);
    EXPECT_EQ(v_loss, expected_value_loss);
}

using ClampToTypes = ::testing::Types<std::tuple<uint8_t, uint16_t>,
                                      std::tuple<uint16_t, uint32_t>,
                                      std::tuple<uint32_t, uint64_t>,
                                      std::tuple<uint8_t, uint64_t>>;

REGISTER_TYPED_TEST_SUITE_P(ClampToTestFixture, UpClampTo, DownClampTo);
INSTANTIATE_TYPED_TEST_SUITE_P(ClampToTestSuite, ClampToTestFixture, ClampToTypes, );

template <typename T>
class HandleAddOverflowTestFixture : public ::testing::Test
{
};

TYPED_TEST_SUITE_P(HandleAddOverflowTestFixture);

TYPED_TEST_P(HandleAddOverflowTestFixture, HandleAddOverflowTestNoOverflow)
{
    TypeParam input1 = 100;
    TypeParam input2 = 10;
    const TypeParam expected_value = 110;
    TypeParam v = HandleAddOverflow<TypeParam>(input1, input2);

    EXPECT_EQ(v, expected_value);
}

TYPED_TEST_P(HandleAddOverflowTestFixture, HandleAddOverflowTestOverflow)
{
    TypeParam input1 = std::numeric_limits<TypeParam>::max() - 1;
    TypeParam input2 = 10;
    const TypeParam expected_value = std::numeric_limits<TypeParam>::max();
    TypeParam v = HandleAddOverflow<TypeParam>(input1, input2);

    EXPECT_EQ(v, expected_value);
}

using HandleAddOverflowTestTypes = ::testing::Types<uint8_t, uint16_t, uint32_t, uint64_t>;

REGISTER_TYPED_TEST_SUITE_P(HandleAddOverflowTestFixture,
                            HandleAddOverflowTestNoOverflow,
                            HandleAddOverflowTestOverflow);
INSTANTIATE_TYPED_TEST_SUITE_P(HandleAddOverflowTestSuite, HandleAddOverflowTestFixture, HandleAddOverflowTestTypes, );

}  // namespace helper
