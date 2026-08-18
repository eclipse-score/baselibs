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
#ifndef SCORE_LIB_OS_LINUX_PTHREAD_H
#define SCORE_LIB_OS_LINUX_PTHREAD_H

#include "score/os/pthread.h"
#include <pthread.h>
#include <cstddef>
#include <cstdint>

namespace score
{
namespace os
{

class LinuxPthread final : public score::os::Pthread
{
  public:
    score::cpp::expected_blank<Error> getcpuclockid(const pthread_t id, clockid_t* clock_id) const noexcept override;

    score::cpp::expected_blank<Error> setname_np(const pthread_t thread,
                                                 const char* const name) const noexcept override;

    score::cpp::expected_blank<Error> getname_np(const pthread_t thread,
                                                 char* const name,
                                                 const std::size_t length) const noexcept override;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_LINUX_PTHREAD_H
