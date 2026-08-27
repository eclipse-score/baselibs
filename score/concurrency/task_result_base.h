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
#ifndef SCORE_LIB_CONCURRENCY_TASK_RESULT_BASE_H
#define SCORE_LIB_CONCURRENCY_TASK_RESULT_BASE_H

#include "score/assert.hpp"
#include "score/stop_token.hpp"
#include "score/utility.hpp"

namespace score
{
namespace concurrency
{

/// \brief The TaskResultBase acts as a base class for TaskResult and SharedTaskResult
///
/// \details In order to do so, it encapsulates std::stop_source and has common API's
///          to be used to abort.

class TaskResultBase
{
  public:
    TaskResultBase() = default;

  protected:
    TaskResultBase(const TaskResultBase&) = default;
    TaskResultBase& operator=(const TaskResultBase&) = default;

    TaskResultBase(TaskResultBase&&) = default;
    TaskResultBase& operator=(TaskResultBase&&) = default;

  public:
    explicit TaskResultBase(score::cpp::stop_source stop_source) noexcept;
    virtual ~TaskResultBase() noexcept = default;
    /// \brief abort will notify the underlying task via score::cpp::stop_token that it should stop its execution. There
    /// is no guarantee that the execution will also be aborted, since we can only use a cooperative manner.
    ///
    /// The behavior is undefined if valid() is false before the call to this function.
    void Abort() noexcept
    {
        score::cpp::ignore = stop_source_.request_stop();
    }

    /// \brief Indicates whether the task was requested to abort
    /// \return true if abort() was previously called on *this, false otherwise.
    bool Aborted() const noexcept
    {
        return stop_source_.stop_requested();
    }

  protected:
    // coverity[autosar_cpp14_m11_0_1_violation] protected for inheritance
    score::cpp::stop_source stop_source_;
};

}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_TASK_RESULT_BASE_H
