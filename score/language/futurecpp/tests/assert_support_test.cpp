/********************************************************************************
 * Copyright (c) 2020 Contributors to the Eclipse Foundation
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

///
/// \file
/// \copyright Copyright (c) 2020 Contributors to the Eclipse Foundation
///

#include <score/assert_support.hpp>
#include <score/assert_support.hpp> // include guard test

#include <gtest/gtest.h>

namespace
{

void my_function(const int* const pointer) { SCORE_LANGUAGE_FUTURECPP_PRECONDITION(pointer != nullptr); }

// NOTRACING
TEST(assert_support_test, ContractViolated)
{
    const int* const pointer{nullptr};

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(my_function(pointer));
    SCORE_LANGUAGE_FUTURECPP_ASSERT_CONTRACT_VIOLATED(my_function(pointer));
}

// NOTRACING
TEST(assert_support_test, ContractNotViolated)
{
    const int i{1};
    const int* const pointer{&i};

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(my_function(pointer));
    SCORE_LANGUAGE_FUTURECPP_ASSERT_CONTRACT_NOT_VIOLATED(my_function(pointer));
}

} // namespace
