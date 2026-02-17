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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_THREAD_THREAD_NAME_HINT_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_THREAD_THREAD_NAME_HINT_HPP

#include <algorithm>
#include <score/string_view.hpp>

#if defined(__QNX__)
#include <sys/neutrino.h>
#endif

namespace score::cpp
{
namespace detail
{

/// \brief `thread_name_hint` is a constructor option for `score::cpp::jthread`.
///
/// A thread constructed with `thread_name_hint` will have a desired name as if set by `pthread_setname_np()`.
/// The restrictions from `pthread_setname_np` has to be followed. Otherwise the name is not set and no error is
/// reported.
// cppcoreguidelines-special-member-functions: Follows literally the C++ standard see https://wg21.link/p2019 for
// details.
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
class thread_name_hint
{
public:
    /// \brief Constructs a desired thread name.
    constexpr explicit thread_name_hint(const score::cpp::string_view name) noexcept
        : name_{name.data(), std::min(get_max_thread_name_length() - 1U, name.size())}
    {
    }

    thread_name_hint(thread_name_hint&&) = delete;
    thread_name_hint(const thread_name_hint&) = delete;

    /// \brief Returns the desired thread name.
    constexpr score::cpp::string_view value() const noexcept { return name_; }

    /// \brief Returns the maximum length of the thread name
    static constexpr std::size_t get_max_thread_name_length() noexcept
    {
#if defined(__QNX__)
        // https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/p/pthread_setname_np.html
        return _NTO_THREAD_NAME_MAX;
#elif defined(__EMSCRIPTEN__)
        // https://github.com/emscripten-core/emscripten/blob/main/system/lib/libc/musl/src/thread/pthread_setname_np.c
        return 16U;
#elif defined(__linux__)
        // https://man7.org/linux/man-pages/man3/pthread_setname_np.3.html
        return 16U;
#else
#error "unknown platform"
#endif
    }

private:
    score::cpp::string_view name_;
};

} // namespace detail
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_THREAD_THREAD_NAME_HINT_HPP
