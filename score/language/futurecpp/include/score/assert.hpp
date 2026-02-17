/********************************************************************************
 * Copyright (c) 2016 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2016 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.Assert component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_ASSERT_HPP
#define SCORE_LANGUAGE_FUTURECPP_ASSERT_HPP

#if defined SCORE_LANGUAGE_FUTURECPP_ASSERT_LEVEL_DEBUG && defined SCORE_LANGUAGE_FUTURECPP_ASSERT_LEVEL_PRODUCTION
#error Assert level debug and production cannot be set at the same time.
#endif

#include <score/private/assert/assert_impl.hpp>

/// Default-level assert.
/// If the asserted condition evaluates to false at runtime, the assertion handler will get invoked. \see score::cpp::handler.
///
/// Each macro has a normal and a `_MESSAGE` form, where the latter takes a user supplied string as an additional
/// argument that will be passed on to the assertion handler.
///
/// It is possible to introduce state in assertion handling which manifests in storing a void* pointer pointing to a
/// "User Data" structure of unspecified format. Proper creation, destruction, and overall handling of this structure is
/// entirely on caller's responsibility. Setting this pointer is possible with set_user_data() while retrieving can be
/// done with get_user_data(). Upon assertion struct handler_parameters is automatically populated with that value.
///
/// The amount of assertions that get compiled into the code depends on the configured assertion level:
///  * Default level checks will be compiled unless `SCORE_LANGUAGE_FUTURECPP_ASSERT_LEVEL_PRODUCTION` is defined.
///  * Debug level checks will only be compiled if `SCORE_LANGUAGE_FUTURECPP_ASSERT_LEVEL_DEBUG` is defined.
///  * Production level checks will always be compiled.
///
///
#define SCORE_LANGUAGE_FUTURECPP_ASSERT(x) SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE(x, nullptr)
/// Debug-level assert.
/// \copydetails SCORE_LANGUAGE_FUTURECPP_ASSERT
///
#define SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(x) SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG_MESSAGE(x, nullptr)
/// Production-level assert.
/// \copydetails SCORE_LANGUAGE_FUTURECPP_ASSERT
///
#define SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(x) SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(x, nullptr)

/// Default-level precondition check.
/// \copydetails SCORE_LANGUAGE_FUTURECPP_ASSERT
///
#define SCORE_LANGUAGE_FUTURECPP_PRECONDITION(x) SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(x, nullptr)
/// Debug-level precondition check.
/// \copydetails SCORE_LANGUAGE_FUTURECPP_ASSERT
///
#define SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(x) SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG_MESSAGE(x, nullptr)
/// Production-level precondition check.
/// \copydetails SCORE_LANGUAGE_FUTURECPP_ASSERT
///
#define SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(x) SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(x, nullptr)

#if defined SCORE_LANGUAGE_FUTURECPP_ASSERT_LEVEL_DEBUG
#define SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE(x, msg) SCORE_LANGUAGE_FUTURECPP_INTERNAL_ASSERT_IMPL(x, msg)
#define SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG_MESSAGE(x, msg) SCORE_LANGUAGE_FUTURECPP_INTERNAL_ASSERT_IMPL(x, msg)
#define SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(x, msg) SCORE_LANGUAGE_FUTURECPP_INTERNAL_ASSERT_IMPL(x, msg)

#define SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(x, msg) SCORE_LANGUAGE_FUTURECPP_INTERNAL_PRECONDITION_IMPL(x, msg)
#define SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG_MESSAGE(x, msg) SCORE_LANGUAGE_FUTURECPP_INTERNAL_PRECONDITION_IMPL(x, msg)
#define SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(x, msg) SCORE_LANGUAGE_FUTURECPP_INTERNAL_PRECONDITION_IMPL(x, msg)
#elif defined SCORE_LANGUAGE_FUTURECPP_ASSERT_LEVEL_PRODUCTION
#define SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE(x, msg) SCORE_LANGUAGE_FUTURECPP_INTERNAL_HELPER_DUMMY_FOR_SEMICOLON
#define SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG_MESSAGE(x, msg) SCORE_LANGUAGE_FUTURECPP_INTERNAL_HELPER_DUMMY_FOR_SEMICOLON
#define SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(x, msg) SCORE_LANGUAGE_FUTURECPP_INTERNAL_ASSERT_IMPL(x, msg)

#define SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(x, msg) SCORE_LANGUAGE_FUTURECPP_INTERNAL_HELPER_DUMMY_FOR_SEMICOLON
#define SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG_MESSAGE(x, msg) SCORE_LANGUAGE_FUTURECPP_INTERNAL_HELPER_DUMMY_FOR_SEMICOLON
#define SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(x, msg) SCORE_LANGUAGE_FUTURECPP_INTERNAL_PRECONDITION_IMPL(x, msg)
#else
/// Default-level assert with custom message.
/// \copydetails SCORE_LANGUAGE_FUTURECPP_ASSERT
///
#define SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE(x, msg) SCORE_LANGUAGE_FUTURECPP_INTERNAL_ASSERT_IMPL(x, msg)
/// Debug-level assert with custom message.
/// \copydetails SCORE_LANGUAGE_FUTURECPP_ASSERT
///
#define SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG_MESSAGE(x, msg) SCORE_LANGUAGE_FUTURECPP_INTERNAL_HELPER_DUMMY_FOR_SEMICOLON
/// Production-level assert with custom message.
/// \copydetails SCORE_LANGUAGE_FUTURECPP_ASSERT
///
#define SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(x, msg) SCORE_LANGUAGE_FUTURECPP_INTERNAL_ASSERT_IMPL(x, msg)

/// Default-level precondition check with custom message.
/// \copydetails SCORE_LANGUAGE_FUTURECPP_ASSERT
///
#define SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(x, msg) SCORE_LANGUAGE_FUTURECPP_INTERNAL_PRECONDITION_IMPL(x, msg)
/// Debug-level precondition check with custom message.
/// \copydetails SCORE_LANGUAGE_FUTURECPP_ASSERT
///
#define SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG_MESSAGE(x, msg) SCORE_LANGUAGE_FUTURECPP_INTERNAL_HELPER_DUMMY_FOR_SEMICOLON
/// Production-level precondition check with custom message.
/// \copydetails SCORE_LANGUAGE_FUTURECPP_ASSERT
///
#define SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(x, msg) SCORE_LANGUAGE_FUTURECPP_INTERNAL_PRECONDITION_IMPL(x, msg)
#endif

/// Check to mark unreachable code.
/// When control flow passes this macro, it will trigger an assertion failure.
///
#define SCORE_LANGUAGE_FUTURECPP_UNREACHABLE() SCORE_LANGUAGE_FUTURECPP_INTERNAL_UNREACHABLE_IMPL(nullptr)
/// Check to mark unreachable code with custom message.
/// \copydetails SCORE_LANGUAGE_FUTURECPP_UNREACHABLE
///
#define SCORE_LANGUAGE_FUTURECPP_UNREACHABLE_MESSAGE(msg) SCORE_LANGUAGE_FUTURECPP_INTERNAL_UNREACHABLE_IMPL(msg)

namespace score::cpp
{
struct handler_parameters
{
    char const* file;      //!< Name of the source file that triggered the failing assertion.
    long line;             //!< Line in the source file that triggered the failing assertion.
    char const* function;  //!< Name of the function that contains the failing assertion.
    char const* condition; //!< Textual representation of the condition that failed the assertion.
    char const* message;   //!< Optional user-provided description. NULL if none provided.
    void* user_data;       //!< Optional user-provided data.
};

/// User-defined handler signature.
///
/// Functions of this type can be registered with set_assertion_handler() to be additionally invoked by a failed
/// assertion. User-defined handlers must not return to their caller once they have been invoked. If a handler does
/// return, a failed assertion will nevertheless call std::abort() to avoid further undefined behavior.
///
using handler = void (*)(const handler_parameters&);

/// Determine the additional user-defined behavior in case of failing assertions.
///
/// The default assertion handler is doing nothing.
///
/// \param[in] new_handler Function to be invoked in case of a failing assertion.
///
void set_assertion_handler(handler new_handler) noexcept;

/// Retrieve the current assertion handler.
///
handler get_assertion_handler() noexcept;

/// Sets user data i.e. a pointer to an arbitrary memory area.
///
/// user_data is a memory area completely handled by the user of this library. Proper creation, destruction,
/// fill-up, and, generally, overall handling of this structure is entirely on caller's responsibility.
///
/// \param[in] user_data pointer to a memory address.
///
void set_user_data(void* const user_data) noexcept;

/// Retrieves the current user data.
///
void* get_user_data() noexcept;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_ASSERT_HPP
