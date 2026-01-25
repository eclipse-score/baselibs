/********************************************************************************
 * Copyright (c) 2022 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2022 Contributors to the Eclipse Foundation
///

#include <score/private/utility/static_const.hpp>
#include <score/private/utility/static_const.hpp> // test include guard

// By including this header, this compilation unit defines a static score::cpp::static_const_test_type_instance.
// static_const_test_helper.cpp also includes the header, so it defines a separate static
// score::cpp::static_const_test_type_instance. Both are linked into the test binary without linker errors.
#include "static_const_test_helper.hpp"

#include <gtest/gtest.h>

namespace
{

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40955959
TEST(static_const, same_address_in_different_compilation_units)
{
    EXPECT_EQ(score::cpp::static_const_test_type_instance_address(), &score::cpp::static_const_test_type_instance);
}

} // namespace
