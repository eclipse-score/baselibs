/********************************************************************************
 * Copyright (c) 2016 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2016 Contributors to the Eclipse Foundation
///

#include <score/static_vector.hpp>
#include <score/static_vector.hpp> // test include guard

#include <score/type_traits.hpp>

#include <gtest/gtest.h>

namespace
{

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(static_vector_test, backward_compatible)
{
    static_assert(std::is_same<score::cpp::static_vector<bool, 1>, score::cpp::inplace_vector<bool, 1>>::value, "failed");
    static_assert(score::cpp::is_inplace_vector<score::cpp::static_vector<bool, 1>>::value, "failed");
    static_assert(score::cpp::is_inplace_vector<score::cpp::inplace_vector<bool, 1>>::value, "failed");
    static_assert(score::cpp::is_static_vector<score::cpp::static_vector<bool, 1>>::value, "failed");
    static_assert(score::cpp::is_static_vector<score::cpp::inplace_vector<bool, 1>>::value, "failed");
}

} // namespace
