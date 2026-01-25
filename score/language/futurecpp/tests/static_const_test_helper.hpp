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

#ifndef TESTS_STATIC_CONST_TEST_HELPER_HPP // NOLINT(llvm-header-guard)
#define TESTS_STATIC_CONST_TEST_HELPER_HPP

#include <score/private/utility/static_const.hpp>

namespace score::cpp
{

struct static_const_test_type
{
    explicit static_const_test_type() = default;
};

namespace
{
constexpr auto& static_const_test_type_instance = static_const<static_const_test_type>::value;
}

const static_const_test_type* static_const_test_type_instance_address();

} // namespace score::cpp

#endif // TESTS_STATIC_CONST_TEST_HELPER_HPP
