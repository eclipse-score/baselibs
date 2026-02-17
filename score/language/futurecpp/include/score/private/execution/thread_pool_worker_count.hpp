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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_THREAD_POOL_WORKER_COUNT_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_THREAD_POOL_WORKER_COUNT_HPP

#include <cstdint>

namespace score::cpp
{
namespace execution
{
namespace detail
{

class thread_pool_worker_count
{
public:
    constexpr explicit thread_pool_worker_count(const std::int32_t count) noexcept : count_{count} {}
    constexpr std::int32_t value() const noexcept { return count_; }

private:
    std::int32_t count_;
};

} // namespace detail
} // namespace execution
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_THREAD_POOL_WORKER_COUNT_HPP
