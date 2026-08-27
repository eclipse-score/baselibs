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
 *        \brief  Contains tests concerning the LevelValidator class.
 *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "gtest/gtest.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/level_validator.h"
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
 * Test that the LevelValidator can be used.
 * \trace           CREQ-Json-Validation
 */
TEST(UT__Parser__LevelValidator, EnterLeave)
{
    internal::LevelValidator lv{};
    ASSERT_TRUE(lv.Enter().has_value());
    ASSERT_TRUE(lv.IsInside());
    ASSERT_TRUE(lv.Leave().has_value());
    ASSERT_FALSE(lv.IsInside());
}

/*!
 * Test that the LevelValidator can be used on an already open object.
 * \trace           CREQ-Json-Validation
 */
TEST(UT__Parser__LevelValidator, EnterAlreadyOpen)
{
    internal::LevelValidator lv{true};
    ASSERT_TRUE(lv.IsInside());
    ASSERT_TRUE(lv.Leave().has_value());
    ASSERT_FALSE(lv.IsInside());
}

/*!
 * Test that entering a level twice leads to an error
 * \trace           CREQ-Json-Validation
 */
TEST(UT__Parser__LevelValidator, EnterAgain)
{
    internal::LevelValidator lv{};
    ASSERT_TRUE(lv.Enter().has_value());
    const auto result = lv.Enter();
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), JsonErrc::kUserValidationFailed);
}

/*!
 * Test that leaving an empty level leads to an error
 * \trace           CREQ-Json-Validation
 */
TEST(UT__Parser__LevelValidator, LeaveOnEmpty)
{
    internal::LevelValidator lv{};
    const auto result = lv.Leave();
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), JsonErrc::kUserValidationFailed);
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
