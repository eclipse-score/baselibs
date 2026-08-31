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
#ifndef LIB_OS_LINUX_MOCKLIB_NONPOSIXWRAPPERMOCK_H_
#define LIB_OS_LINUX_MOCKLIB_NONPOSIXWRAPPERMOCK_H_

#include "score/os/linux/ifaddrs.h"
#include "score/os/linux/pthread.h"
#include "score/os/linux/shm.h"
#include "score/os/linux/statfs.h"
#include "score/os/linux/sysinfo.h"
#include "score/os/linux/timerfd.h"
#include "score/os/linux/timex.h"
#include "score/os/linux/unistd.h"

#include <gmock/gmock.h>
#include <string>

namespace score
{
namespace os
{

class NonPosixWrapperMock
{
  public:
    NonPosixWrapperMock();
    virtual ~NonPosixWrapperMock() = default;

    MOCK_METHOD(std::int32_t, daemon, (std::int32_t __nochdir, std::int32_t __noclose));
    MOCK_METHOD(std::int32_t, pipe2, (std::int32_t*, std::int32_t));
    MOCK_METHOD(std::int32_t, statfs, (const char* file, struct statfs* buf));
    MOCK_METHOD(std::int32_t, getifaddrs, (struct ifaddrs * *ifa));
    MOCK_METHOD(void, freeifaddrs, (struct ifaddrs * ifa));
    MOCK_METHOD(bool, pthread_setaffinity_np, (std::int32_t i));
    MOCK_METHOD(std::int32_t, timerfd_create, (__clockid_t clock_id, std::int32_t flags));
    MOCK_METHOD(std::int32_t, timerfd_gettime, (std::int32_t ufd, struct itimerspec* otmr));
    MOCK_METHOD(std::int32_t,
                timerfd_settime,
                (std::int32_t ufd, std::int32_t flags, const struct itimerspec* utmr, struct itimerspec* otmr));
    MOCK_METHOD(std::int32_t, clock_adjtime, (clockid_t clkid, struct timex* tx));
    MOCK_METHOD(int, sysinfo, (struct sysinfo * info));
    MOCK_METHOD(std::int32_t, shmctl, (std::int32_t shmid, std::int32_t cmd, struct shmid_ds* buf));
};

}  // namespace os
}  // namespace score

#endif  // LIB_OS_LINUX_MOCKLIB_NONPOSIXWRAPPERMOCK_H_
