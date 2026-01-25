/********************************************************************************
 * Copyright (c) 2024 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2024 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_THREAD_THREAD_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_THREAD_THREAD_HPP

#include <score/private/thread/thread_id.hpp>
#include <score/private/thread/thread_name_hint.hpp>
#include <score/private/thread/thread_stack_size_hint.hpp>

namespace score::cpp
{

/// \brief Represents a single thread of execution.
///
/// The class only defines the 3 aliases `id`, `stack_size_hint` and `native_handle_type`.
///
/// The class shall not be used. Please use `score::cpp::jthread` as a full replacement.
class thread
{
public:
    using id = score::cpp::detail::thread_id;
    using stack_size_hint = score::cpp::detail::thread_stack_size_hint;
    using name_hint = score::cpp::detail::thread_name_hint;
    using native_handle_type = score::cpp::detail::thread_id::native_handle_type;

    thread() = delete;
    thread(const thread&) = delete;
    thread& operator=(const thread&) = delete;
    thread(thread&&) = delete;
    thread& operator=(thread&&) = delete;
    ~thread() = delete;
};

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_THREAD_THREAD_HPP
