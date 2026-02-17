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
/// \copyright Copyright (c) 2019 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_ASSERT_ASSERT_IMPL_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_ASSERT_ASSERT_IMPL_HPP

#ifndef SCORE_LANGUAGE_FUTURECPP_ASSERT_HPP
#error "One does not directly depend on the detail impl header"
#endif

namespace score::cpp
{

struct handler_parameters;

namespace detail
{

struct assert_system_handler_tag
{
};

[[noreturn]] void assertion_failed(const handler_parameters& param, assert_system_handler_tag /*unused*/);

} // namespace detail
} // namespace score::cpp

/// \cond
#if defined _MSC_VER
#define SCORE_LANGUAGE_FUTURECPP_INTERNAL_PRETTY_FUNCTION __FUNCSIG__
#elif defined __clang__
#define SCORE_LANGUAGE_FUTURECPP_INTERNAL_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined __GNUC__
#define SCORE_LANGUAGE_FUTURECPP_INTERNAL_PRETTY_FUNCTION __PRETTY_FUNCTION__
#else
#define SCORE_LANGUAGE_FUTURECPP_INTERNAL_PRETTY_FUNCTION __func__
#endif

#define SCORE_LANGUAGE_FUTURECPP_INTERNAL_HELPER_DUMMY_FOR_SEMICOLON static_cast<void>(0)

#define SCORE_LANGUAGE_FUTURECPP_INTERNAL_SIGNAL_ASSERTION_FAILURE(cond, msg)                                                               \
    score::cpp::detail::assertion_failed(score::cpp::handler_parameters{__FILE__,                                                    \
                                                          __LINE__,                                                    \
                                                          static_cast<const char*>(SCORE_LANGUAGE_FUTURECPP_INTERNAL_PRETTY_FUNCTION),      \
                                                          #cond,                                                       \
                                                          msg,                                                         \
                                                          score::cpp::get_user_data()},                                       \
                                  score::cpp::detail::assert_system_handler_tag{})

#define SCORE_LANGUAGE_FUTURECPP_INTERNAL_ASSERT_IMPL(cond, msg)                                                                            \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(cond))                                                                                                   \
        {                                                                                                              \
            SCORE_LANGUAGE_FUTURECPP_INTERNAL_SIGNAL_ASSERTION_FAILURE(cond, msg);                                                          \
        }                                                                                                              \
    } while (false)

#define SCORE_LANGUAGE_FUTURECPP_INTERNAL_PRECONDITION_IMPL(x, msg) SCORE_LANGUAGE_FUTURECPP_INTERNAL_ASSERT_IMPL(x, msg)

#define SCORE_LANGUAGE_FUTURECPP_INTERNAL_UNREACHABLE_IMPL(msg) SCORE_LANGUAGE_FUTURECPP_INTERNAL_SIGNAL_ASSERTION_FAILURE(Unreachable_Code, msg)
  /// \endcond

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_ASSERT_ASSERT_IMPL_HPP
