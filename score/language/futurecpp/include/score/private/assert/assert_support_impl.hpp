/********************************************************************************
 * Copyright (c) 2020 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2020 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_ASSERT_ASSERT_SUPPORT_IMPL_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_ASSERT_ASSERT_SUPPORT_IMPL_HPP

#ifndef SCORE_LANGUAGE_FUTURECPP_ASSERT_SUPPORT_HPP
#error "One does not directly depend on the detail impl header"
#endif

#include <score/assert.hpp>

namespace score::cpp
{
namespace detail
{

///
/// @brief Sets an assertion handler that throws a contract_violation exception. It restores the previous
/// assertion handler on destruction.
///
class score_future_cpp_internal_throwing_assertion_handler
{
public:
    struct contract_violation
    {
    };

    score_future_cpp_internal_throwing_assertion_handler() : previous_assertion_handler_{get_assertion_handler()}
    {
        set_assertion_handler([](const handler_parameters&) { throw contract_violation{}; });
    }
    score_future_cpp_internal_throwing_assertion_handler(const score_future_cpp_internal_throwing_assertion_handler&) = delete;
    score_future_cpp_internal_throwing_assertion_handler& operator=(const score_future_cpp_internal_throwing_assertion_handler&) = delete;
    score_future_cpp_internal_throwing_assertion_handler(score_future_cpp_internal_throwing_assertion_handler&&) = delete;
    score_future_cpp_internal_throwing_assertion_handler& operator=(score_future_cpp_internal_throwing_assertion_handler&&) = delete;
    ~score_future_cpp_internal_throwing_assertion_handler() noexcept { set_assertion_handler(previous_assertion_handler_); }

private:
    handler previous_assertion_handler_;
};

} // namespace detail
} // namespace score::cpp

#define SCORE_LANGUAGE_FUTURECPP_TEST_CONTRACT_VIOLATED(statement, fail)                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        bool score_future_cpp_test_contract_violated{false};                                                                        \
        try                                                                                                            \
        {                                                                                                              \
            score::cpp::detail::score_future_cpp_internal_throwing_assertion_handler h{};                                                  \
            statement;                                                                                                 \
        }                                                                                                              \
        catch (const score::cpp::detail::score_future_cpp_internal_throwing_assertion_handler::contract_violation&)                        \
        {                                                                                                              \
            /* NOLINTNEXTLINE(bmw-no-exceptions) For testing we can use exceptions, just not in product code */        \
            score_future_cpp_test_contract_violated = true;                                                                         \
        }                                                                                                              \
        if (!score_future_cpp_test_contract_violated)                                                                               \
        {                                                                                                              \
            fail() << "Contract not violated: " << #statement;                                                         \
        }                                                                                                              \
    } while (false)

#define SCORE_LANGUAGE_FUTURECPP_TEST_CONTRACT_NOT_VIOLATED(statement, fail)                                                                \
    do                                                                                                                 \
    {                                                                                                                  \
        try                                                                                                            \
        {                                                                                                              \
            score::cpp::detail::score_future_cpp_internal_throwing_assertion_handler h{};                                                  \
            statement;                                                                                                 \
        }                                                                                                              \
        catch (const score::cpp::detail::score_future_cpp_internal_throwing_assertion_handler::contract_violation&)                        \
        {                                                                                                              \
            /* NOLINTNEXTLINE(bmw-no-exceptions) For testing we can use exceptions, just not in product code */        \
            fail() << "Contract violated: " << #statement;                                                             \
        }                                                                                                              \
    } while (false)

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_ASSERT_ASSERT_SUPPORT_IMPL_HPP
