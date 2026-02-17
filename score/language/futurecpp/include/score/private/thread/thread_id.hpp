//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

///
/// \file
/// \copyright Copyright (c) 2021 Contributors to the Eclipse Foundation
///
/// The implementation is based on \c std::thread from https://github.com/llvm/llvm-project/tree/main/libcxx with
/// modifications listed in \c NOTICE.
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_THREAD_THREAD_ID_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_THREAD_THREAD_ID_HPP

#include <functional>
#include <ostream>
#include <pthread.h>

namespace score::cpp
{

class jthread;

namespace detail
{

class thread_id;

} // namespace detail

namespace this_thread
{

score::cpp::detail::thread_id get_id() noexcept;

} // namespace this_thread

namespace detail
{

// https://en.cppreference.com/w/cpp/thread/thread/id
class thread_id
{
public:
    using native_handle_type = ::pthread_t;

    // https://en.cppreference.com/w/cpp/thread/thread/id/id
    thread_id() noexcept : native_handle_{native_handle_type{}} {}

    // https://en.cppreference.com/w/cpp/thread/thread/id/operator_cmp
    friend bool operator==(thread_id lhs, thread_id rhs) noexcept { return lhs.native_handle_ == rhs.native_handle_; }
    friend bool operator!=(thread_id lhs, thread_id rhs) noexcept { return lhs.native_handle_ != rhs.native_handle_; }
    friend bool operator<(thread_id lhs, thread_id rhs) noexcept { return lhs.native_handle_ < rhs.native_handle_; }
    friend bool operator<=(thread_id lhs, thread_id rhs) noexcept { return lhs.native_handle_ <= rhs.native_handle_; }
    friend bool operator>(thread_id lhs, thread_id rhs) noexcept { return lhs.native_handle_ > rhs.native_handle_; }
    friend bool operator>=(thread_id lhs, thread_id rhs) noexcept { return lhs.native_handle_ >= rhs.native_handle_; }

    // https://en.cppreference.com/w/cpp/thread/thread/id/operator_ltlt
    template <class CharT, class Traits>
    friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& ost, thread_id id)
    {
        return ost << id.native_handle_;
    }

private:
    friend class score::cpp::jthread;
    friend struct std::hash<thread_id>;
    friend thread_id score::cpp::this_thread::get_id() noexcept;

    thread_id(native_handle_type native_handle) : native_handle_{native_handle} {}

    native_handle_type native_handle_;
};

} // namespace detail
} // namespace score::cpp

namespace std
{

// https://en.cppreference.com/w/cpp/utility/hash
// https://en.cppreference.com/w/cpp/thread/thread/id/hash
template <>
struct hash<score::cpp::detail::thread_id>
{
    size_t operator()(const score::cpp::detail::thread_id id) const noexcept
    {
        return hash<score::cpp::detail::thread_id::native_handle_type>()(id.native_handle_);
    }
};

} // namespace std

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_THREAD_THREAD_ID_HPP
