/********************************************************************************
 * Copyright (c) 2021 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2021 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_THREAD_PTHREAD_ATTR_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_THREAD_PTHREAD_ATTR_HPP

#include <cstddef>
#include <pthread.h>
#include <system_error>

#include <score/assert.hpp>

namespace score::cpp
{
namespace detail
{

/// Implements resource management for a pthread_attr_t (RAII).
class pthread_attr
{
public:
    pthread_attr()
    {
        const int status{::pthread_attr_init(&native_handle_)};
        if (status != 0)
        {
            throw std::system_error{status, std::system_category(), "pthread_attr_init"};
        }
    }

    pthread_attr(const pthread_attr&) = delete;

    pthread_attr(pthread_attr&&) = delete;

    pthread_attr& operator=(const pthread_attr&) = delete;

    pthread_attr& operator=(pthread_attr&&) = delete;

    ~pthread_attr() noexcept
    {
        // pthread_attr_destroy() always succeeds.
        // For additional information, see
        // https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_attr_destroy.html
        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(::pthread_attr_destroy(&native_handle_) == 0);
    }

    void set_stack_size(const std::size_t stack_size)
    {
        const int status{::pthread_attr_setstacksize(&native_handle_, stack_size)};
        if (status != 0)
        {
            throw std::system_error{status, std::system_category(), "pthread_attr_setstacksize"};
        }
    }

    ::pthread_attr_t& native_handle() { return *&native_handle_; }

private:
    ::pthread_attr_t native_handle_;
};

} // namespace detail
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_THREAD_PTHREAD_ATTR_HPP
