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
#ifndef SCORE_LIB_OS_SEMAPHORE_IMPL
#define SCORE_LIB_OS_SEMAPHORE_IMPL

#include "score/os/semaphore.h"

namespace score
{
namespace os
{

class SemaphoreImpl final : public Semaphore
{
  public:
    constexpr SemaphoreImpl() = default;
    score::cpp::expected_blank<Error> sem_init(sem_t* const sem,
                                               const std::int32_t pshared,
                                               const std::uint32_t value) const noexcept override;

    score::cpp::expected<sem_t*, Error> sem_open(const char* const pathname,
                                                 const Semaphore::OpenFlag oflag,
                                                 const Semaphore::ModeFlag mode,
                                                 const std::uint32_t value) const noexcept override;

    score::cpp::expected<sem_t*, Error> sem_open(const char* const pathname,
                                                 const Semaphore::OpenFlag oflag) const noexcept override;

    score::cpp::expected_blank<Error> sem_wait(sem_t* const sem) const noexcept override;

    score::cpp::expected_blank<Error> sem_post(sem_t* const sem) const noexcept override;

    score::cpp::expected_blank<Error> sem_close(sem_t* const sem) const noexcept override;

    score::cpp::expected_blank<Error> sem_unlink(const char* const pathname) const noexcept override;

    score::cpp::expected_blank<Error> sem_timedwait(sem_t* const sem,
                                                    const struct timespec* const abs_time) const noexcept override;

    score::cpp::expected_blank<Error> sem_timedwait_monotonic(sem_t* const sem, const struct timespec* const abs_time)
        const noexcept override;

    score::cpp::expected_blank<Error> sem_getvalue(sem_t* const sem, std::int32_t* const sval) const noexcept override;

  private:
    std::int32_t openflag_to_nativeflag(const Semaphore::OpenFlag flags) const noexcept;
    std::int32_t modeflag_to_nativeflag(const Semaphore::ModeFlag flags) const noexcept;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_SEMAPHORE_IMPL
