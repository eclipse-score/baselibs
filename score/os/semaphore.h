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
#ifndef SCORE_LIB_OS_SEMAPHORE_H
#define SCORE_LIB_OS_SEMAPHORE_H

#include "score/bitmanipulation/bitmask_operators.h"
#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"

#include <semaphore.h>
#include <unistd.h>

/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
namespace score
{
namespace os
{

class Semaphore : public ObjectSeam<Semaphore>
{
  public:
    static Semaphore& instance() noexcept;

    enum class OpenFlag : std::int32_t
    {
        kCreate = 1,
        kExclusive = 2,
    };

    enum class ModeFlag : std::int32_t
    {
        kReadUser = 1,
        kWriteUser = 2,
        kExecUser = 4,
        kReadGroup = 8,
        kWriteGroup = 16,
        kExecGroup = 32,
        kReadOthers = 64,
        kWriteOthers = 128,
        kExecOthers = 256,
    };

    virtual score::cpp::expected_blank<Error> sem_init(sem_t* const sem,
                                                const std::int32_t pshared,
                                                const std::uint32_t value) const noexcept = 0;
    virtual score::cpp::expected<sem_t*, Error> sem_open(const char* const pathname,
                                                  const OpenFlag oflag,
                                                  const ModeFlag mode,
                                                  const std::uint32_t value) const noexcept = 0;
    virtual score::cpp::expected<sem_t*, Error> sem_open(const char* const pathname, const OpenFlag oflag) const noexcept = 0;
    virtual score::cpp::expected_blank<Error> sem_wait(sem_t* const sem) const noexcept = 0;
    virtual score::cpp::expected_blank<Error> sem_post(sem_t* const sem) const noexcept = 0;
    virtual score::cpp::expected_blank<Error> sem_close(sem_t* const sem) const noexcept = 0;
    virtual score::cpp::expected_blank<Error> sem_unlink(const char* const pathname) const noexcept = 0;
    virtual score::cpp::expected_blank<Error> sem_timedwait(sem_t* const sem,
                                                     const struct timespec* const abs_time) const noexcept = 0;
    virtual score::cpp::expected_blank<Error> sem_getvalue(sem_t* const sem, std::int32_t* const sval) const noexcept = 0;

    virtual ~Semaphore() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    Semaphore(Semaphore&& other) = delete;
    Semaphore& operator=(Semaphore&& other) = delete;

  protected:
    Semaphore() = default;
};

/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
}  // namespace os
}  // namespace score

namespace score
{
template <>
struct enable_bitmask_operators<::score::os::Semaphore::OpenFlag> : public std::true_type
{
};

template <>
struct enable_bitmask_operators<::score::os::Semaphore::ModeFlag> : public std::true_type
{
};
}  // namespace score

#endif  // SCORE_LIB_OS_SEMAPHORE_H
