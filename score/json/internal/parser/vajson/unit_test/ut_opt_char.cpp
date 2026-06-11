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
 *        \brief  Contains tests concerning the OptChar class.
 *
 *********************************************************************************************************************/
/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "gtest/gtest.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/json_ops.h"

namespace score
{
namespace json
{
namespace vajson
{
namespace unit_test
{

/*!
 * Test OptChar with a valid char.
 */
TEST(UT__JsonOps__OptChar, ValidChar)
{
    char constexpr value{'a'};
    internal::OptChar const opt{static_cast<std::int64_t>(value)};
    ASSERT_TRUE(opt.HasValue());
    ASSERT_FALSE(opt.EofFound());
    ASSERT_EQ(opt.Value(), value);
}

/*!
 * Test OptChar with an EOF char.
 */
TEST(UT__JsonOps__OptChar, EofChar)
{
    internal::OptChar const opt{static_cast<std::int64_t>(-1)};
    ASSERT_FALSE(opt.HasValue());
    ASSERT_TRUE(opt.EofFound());
    ASSERT_DEATH(opt.Value(), ".*");
}

/*!
 * Test OptChar comparison operator.
 */
TEST(UT__JsonOps__OptChar, Comparison)
{
    internal::OptChar const opt1{'a'};
    internal::OptChar const opt2{-1};
    // NOLINTNEXTLINE(readability/check)
    ASSERT_TRUE(opt1 == 'a');
    ASSERT_FALSE(opt1 == 'b');  // NOLINT(readability/check)
    ASSERT_FALSE(opt2 == 'c');  // NOLINT(readability/check)
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
