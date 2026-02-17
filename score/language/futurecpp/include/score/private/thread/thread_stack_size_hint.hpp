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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_THREAD_THREAD_STACK_SIZE_HINT_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_THREAD_THREAD_STACK_SIZE_HINT_HPP

#include <cstddef>

namespace score::cpp
{
namespace detail
{

/// \brief `thread_stack_size_hint` is a constructor option for threads including `score::cpp::jthread` and `score::cpp::thread_pool`.
///
/// A thread constructed with `thread_stack_size_hint` will configure a desired stack size as if by POSIX
/// `pthread_attr_setstacksize()`. The stack size set by the implementation may be adjusted up or down to meet
/// platform-specific requirements.
class thread_stack_size_hint
{
public:
    /// \brief Constructs a desired stack size.
    ///
    /// If `size` is 0 the thread attribute will be ignored.
    constexpr explicit thread_stack_size_hint(const std::size_t size) noexcept : size_{size} {}

    /// \brief Returns the desired stack size.
    ///
    /// This size does not reflect the actual used size by the implementation.
    constexpr std::size_t value() const noexcept { return size_; }

private:
    std::size_t size_;
};

constexpr bool operator==(const thread_stack_size_hint lhs, const thread_stack_size_hint rhs) noexcept
{
    return lhs.value() == rhs.value();
}
constexpr bool operator!=(const thread_stack_size_hint lhs, const thread_stack_size_hint rhs) noexcept
{
    return !(lhs == rhs);
}

} // namespace detail
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_THREAD_THREAD_STACK_SIZE_HINT_HPP
