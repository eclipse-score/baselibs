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
#ifndef SCORE_MW_LOG_DETAIL_THREAD_LOCAL_GUARD_H
#define SCORE_MW_LOG_DETAIL_THREAD_LOCAL_GUARD_H

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

/// \brief RAII Pattern to manipulate thread-local variable, to indicate if we are within logging stack or not
///
/// \details Whenever we call a complex recorder, it could happen that this complex recorder uses common libraries that
///  on the other side again use logging. This would lead to a recursive call stack - ending in a crash of an
///  application. The idea is the following, that we figure if a logging command has been called within a complex
///  recorder. We can do this by relying on the stack - meaning, we have to-do this per thread, since each thread has a
///  custom stack. For that, we utilize a thread-local variable. For each call to a recorder, we mark this as true, once
///  the call left, we mark it as false. This way we are also able to support recursive calls to logging within the user
///  space. Whenever we then are logging in the logging stack, we need to fallback to a simpler recorder (e.g. console)
///  which does not rely on any common libraries.
class ThreadLocalGuard
{
  public:
    explicit ThreadLocalGuard();

    ~ThreadLocalGuard();

    static bool IsWithingLogging() noexcept;

    ThreadLocalGuard(ThreadLocalGuard&) = delete;
    ThreadLocalGuard(ThreadLocalGuard&&) = delete;

    ThreadLocalGuard& operator=(ThreadLocalGuard&) = delete;
    ThreadLocalGuard& operator=(ThreadLocalGuard&&) = delete;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_DETAIL_THREAD_LOCAL_GUARD_H
