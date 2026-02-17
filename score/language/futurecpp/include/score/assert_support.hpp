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
/// \brief `score_future_cpp_test_support` adds macros for testing contract violations.
///
///  * `SCORE_LANGUAGE_FUTURECPP_{ASSERT|EXPECT}_CONTRACT_VIOLATED(statement)` tests that
///     the statement violates the contract.
///  * `SCORE_LANGUAGE_FUTURECPP_{ASSERT|EXPECT}_CONTRACT_NOT_VIOLATED(statement)` tests
///     that the statement doesn't violate the contract.
///
/// The motivation for this PR is the code example `foo_unit_test.cpp` in DES-51 v1.1.
/// Google Test supports death tests for testing such kinds of things. However,
/// death tests are costly because the process is terminated and restarted.
/// Also, `std::terminate` triggers a core dump which adds further costs. In the
/// end it doesn't scale. This is why the linked DES suggests to use an exception,
/// which is not so disruptive and cheap for the unit tests.
///
/// Because the approach of example `foo_unit_test.cpp` introduces a lot of
/// boilerplate code, i.e., creating a custom exception class, setting a custom
/// assertion handler, restoring the original assertion handler, the macros
/// introduced by this pull request simplify the implementation of unit tests
/// that test contract violations and make those tests more expressive.
///
/// So instead of
///
/// \code{.cpp}
/// struct ContractViolation
/// {
/// };
///
/// const auto current_handler = score::cpp::get_assertion_handler();
/// score::cpp::set_assertion_handler([](const score::cpp::handler_parameters&) {
///    throw ContractViolation{};
/// });
///
/// EXPECT_THROW(my_function(nullptr), ContractViolation);
///
/// score::cpp::set_assertion_handler(current_handler);
/// \endcode
///
/// you can now write
///
/// \code{.cpp}
/// SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(my_function(nullptr));
/// \endcode
///
/// Depending on your system and how it handles exceptions it might be required
/// to take additional configurations to work properly with exceptions. E.g within
/// PiL environment it might be required to re-enable exceptions.

#ifndef SCORE_LANGUAGE_FUTURECPP_ASSERT_SUPPORT_HPP
#define SCORE_LANGUAGE_FUTURECPP_ASSERT_SUPPORT_HPP

#include <score/private/assert/assert_support_impl.hpp>

#define SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(statement) SCORE_LANGUAGE_FUTURECPP_TEST_CONTRACT_VIOLATED(statement, ADD_FAILURE)
#define SCORE_LANGUAGE_FUTURECPP_ASSERT_CONTRACT_VIOLATED(statement) SCORE_LANGUAGE_FUTURECPP_TEST_CONTRACT_VIOLATED(statement, FAIL)

#define SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(statement) SCORE_LANGUAGE_FUTURECPP_TEST_CONTRACT_NOT_VIOLATED(statement, ADD_FAILURE)
#define SCORE_LANGUAGE_FUTURECPP_ASSERT_CONTRACT_NOT_VIOLATED(statement) SCORE_LANGUAGE_FUTURECPP_TEST_CONTRACT_NOT_VIOLATED(statement, FAIL)

#endif // SCORE_LANGUAGE_FUTURECPP_ASSERT_SUPPORT_HPP
