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

/*!
 * \brief           Tests if IsDigit works as expected.
 * \trace           CREQ-Json-ProvidedSerializers
 */
TEST(UT__NumberParser, IsDigit)
{
    ASSERT_FALSE(internal::util::IsDigit('/'));
    ASSERT_TRUE(internal::util::IsDigit('0'));
    ASSERT_TRUE(internal::util::IsDigit('9'));
    ASSERT_FALSE(internal::util::IsDigit(':'));
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
