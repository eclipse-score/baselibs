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

///
/// \file
/// \copyright Copyright (c) 2026 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_THREAD_THREAD_PRIORITY_HINT_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_THREAD_THREAD_PRIORITY_HINT_HPP

#include <cstdint>

namespace score::cpp
{
namespace detail
{

/// \brief `thread_priority_hint` is a constructor option for threads including `score::cpp::jthread` and `score::cpp::thread_pool`.
///
/// A thread constructed with `thread_priority_hint` will configure a desired thread priority as if by POSIX
/// `pthread_attr_setschedparam()`.
///
/// The priority must follow the platform dependent restrictions. On some platforms setting the priority needs special
/// rights. There could be further restrictions coming from the platform safety manual, e.g., RST-0160 on QNX8.
class thread_priority_hint
{
public:
    /// \brief Constructs a desired priority.
    constexpr explicit thread_priority_hint(const std::int32_t prio) noexcept : prio_{prio} {}

    /// \brief Returns the desired priority.
    constexpr std::int32_t value() const noexcept { return prio_; }

private:
    std::int32_t prio_;
};

} // namespace detail
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_THREAD_THREAD_PRIORITY_HINT_HPP
