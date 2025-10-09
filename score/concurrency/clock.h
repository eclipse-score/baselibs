/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
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
#ifndef SCORE_LIB_CONCURRENCY_CLOCK_H
#define SCORE_LIB_CONCURRENCY_CLOCK_H

#include <chrono>
#include <exception>
#include <mutex>

namespace score
{
namespace concurrency
{
namespace testing
{
namespace detail
{
/// \brief C++ Clock that can be used for testing purposes where strict timing requirements need to be taken into
/// account.
/// \note Please be aware that this clock is marked as being steady. This means that algorithms assume that the
/// timestamp is never decreasing and there is a constant time between ticks. It is up to the tester to adhere to this
/// as far as required for the code under test.
template <bool steady>
class Clock
{
  public:
    using duration = std::chrono::duration<std::intmax_t, std::nano>;
    using rep = duration::rep;
    using period = duration::period;
    using time_point = std::chrono::time_point<Clock, duration>;

    static const bool is_steady;

    /// \brief Modifies the current time by adding the specified offset.
    /// \note For non-steady clocks the offset may also be negative. Providing a negative offset for steady clocks will
    /// trigger termination.
    /// \param offset The offset to add
    static void modify_time(duration offset) noexcept
    {
        // coverity[autosar_cpp14_m0_1_9_violation] false-positive
        // coverity[autosar_cpp14_m0_1_3_violation] false-positive
        std::lock_guard<std::mutex> lock{current_time_mutex_};
        if (is_steady && (offset < duration::zero()))
        {
            std::terminate();
        }
        current_time_ += offset;
    }

    /// \brief The current time of this clock
    static time_point now() noexcept
    {
        // coverity[autosar_cpp14_m0_1_9_violation] false-positive
        // coverity[autosar_cpp14_m0_1_3_violation] false-positive
        std::lock_guard<std::mutex> lock{current_time_mutex_};
        return current_time_;
    }

  private:
    static std::mutex current_time_mutex_;
    static time_point current_time_;
};

template <bool steady>
// Suppress "AUTOSAR C++14 A3-1-1" rule finding: "It shall be possible to include any header file in multiple
// translation units without violating the One Definition Rule.".
// Justification: The static data member 'current_time_mutex_' is defined in this header due to template instantiation
// requirements. Each instantiation of 'Clock<steady>' needs its own static instance of 'current_time_mutex_'.
// This is allowed under the ODR exception for templates.
// coverity[autosar_cpp14_a3_1_1_violation]
std::mutex Clock<steady>::current_time_mutex_{};

template <bool steady>
// coverity[autosar_cpp14_a3_1_1_violation]: the same reason as for 'current_time_mutex_'
typename Clock<steady>::time_point Clock<steady>::current_time_{};

template <bool steady>
// coverity[autosar_cpp14_a3_1_1_violation]: the same reason as for 'current_time_mutex_'
constexpr bool Clock<steady>::is_steady{steady};

}  // namespace detail

using SteadyClock = detail::Clock<true>;
using NonSteadyClock = detail::Clock<false>;

}  // namespace testing
}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_CLOCK_H
