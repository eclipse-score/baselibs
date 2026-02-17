/********************************************************************************
 * Copyright (c) 2019 Contributors to the Eclipse Foundation
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
/// @copyright Copyright (c) 2019 Contributors to the Eclipse Foundation
///

#include <score/assert.hpp>

#include <atomic>
#include <cstdlib>

namespace score::cpp
{
namespace
{

void null_handler(const handler_parameters& /*unused*/) noexcept {} // LCOV_EXCL_LINE

std::atomic<handler>& get_assertion_handler_singleton() noexcept
{
    static std::atomic<handler> handler{&null_handler};
    return handler;
}

std::atomic<void*>& get_user_data_singleton() noexcept
{
    static std::atomic<void*> user_data{nullptr};
    return user_data;
}

} // namespace

handler get_assertion_handler() noexcept { return score::cpp::get_assertion_handler_singleton().load(); }

void set_assertion_handler(handler new_handler) noexcept
{
    if (new_handler == nullptr)
    {
        new_handler = &null_handler;
    }

    score::cpp::get_assertion_handler_singleton().store(new_handler);
}

void* get_user_data() noexcept { return score::cpp::get_user_data_singleton().load(); }

void set_user_data(void* const user_data) noexcept { score::cpp::get_user_data_singleton().store(user_data); }

namespace detail
{

void assertion_failed(const handler_parameters& param, assert_system_handler_tag /*unused*/)
{
    score::cpp::get_assertion_handler()(param);
    // MISRA rule describes the following exception "The call to `abort` that occurs due to the macro expansion of
    // `assert` is not considered to be an explicit call, as it is not expected to be reachable. This function behaves
    // similar to `assert`.
    // coverity[misra_cpp_2023_rule_18_5_2_violation : SUPPRESS]
    std::abort(); // LCOV_EXCL_LINE
                  // DEATH test does not cover std::abort--presumably because of DEATH test run in a subprocess
}

} // namespace detail
} // namespace score::cpp
