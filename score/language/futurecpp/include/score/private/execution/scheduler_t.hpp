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
/// @file
/// @copyright Copyright (c) 2024 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_SCHEDULER_T_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_SCHEDULER_T_HPP

#include <score/private/type_traits/remove_cvref.hpp>
#include <type_traits>

namespace score::cpp
{
namespace execution
{

struct scheduler_t
{
};

template <typename Scheduler>
using is_scheduler = std::is_same<scheduler_t, typename score::cpp::remove_cvref_t<Scheduler>::scheduler_concept>;

} // namespace execution
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_SCHEDULER_T_HPP
