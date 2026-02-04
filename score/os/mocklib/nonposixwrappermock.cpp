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
#include "score/os/mocklib/nonposixwrappermock.h"
#include <functional>

namespace score
{
namespace os
{

namespace
{

std::function<std::int32_t(std::int32_t __nochdir, std::int32_t __noclose)> daemon_callback;
std::function<std::int32_t(std::int32_t*, std::int32_t)> pipe2_callback;
std::function<std::int32_t(const char*, struct statfs*)> statfs2_callback;
std::function<std::int32_t(struct ifaddrs** ifa)> getifaddrs_callback;
std::function<void(struct ifaddrs* ifa)> freeifaddrs_callback;
std::function<bool(std::int32_t i)> pthread_setaffinity_np_callback;
std::function<std::int32_t(__clockid_t, std::int32_t)> timerfd_create_callback;
std::function<std::int32_t(std::int32_t, struct itimerspec*)> timerfd_gettime_callback;
std::function<std::int32_t(std::int32_t, std::int32_t, const struct itimerspec*, struct itimerspec*)>
    timerfd_settime_callback;
std::function<std::int32_t(clockid_t, struct timex*)> clock_adjtime_callback;
std::function<int(struct sysinfo* info)> sysinfo_callback;
std::function<std::int32_t(std::int32_t shmid, std::int32_t cmd, struct shmid_ds* buf)> shmctl_callback;

} /* namespace */

NonPosixWrapperMock::NonPosixWrapperMock()
{
    daemon_callback = [this](std::int32_t __nochdir, std::int32_t __noclose) {
        return this->daemon(__nochdir, __noclose);
    };

    pipe2_callback = [this](std::int32_t* pipedes, std::int32_t flags) {
        return this->pipe2(pipedes, flags);
    };
    statfs2_callback = [this](const char* file, struct statfs* buf) {
        return this->statfs(file, buf);
    };
    getifaddrs_callback = [this](struct ifaddrs** ifa) {
        return this->getifaddrs(ifa);
    };
    freeifaddrs_callback = [this](struct ifaddrs* ifa) {
        return this->freeifaddrs(ifa);
    };
    pthread_setaffinity_np_callback = [this](std::int32_t i) {
        return this->pthread_setaffinity_np(i);
    };
    timerfd_create_callback = [this](__clockid_t clock_id, std::int32_t flags) {
        return this->timerfd_create(clock_id, flags);
    };

    timerfd_gettime_callback = [this](std::int32_t ufd, struct itimerspec* otmr) {
        return this->timerfd_gettime(ufd, otmr);
    };

    timerfd_settime_callback =
        [this](std::int32_t ufd, std::int32_t flags, const struct itimerspec* utmr, struct itimerspec* otmr) {
            return this->timerfd_settime(ufd, flags, utmr, otmr);
        };

    clock_adjtime_callback = [this](clockid_t clk, struct timex* tx) {
        return this->clock_adjtime(clk, tx);
    };
    sysinfo_callback = [this](struct sysinfo* info) -> int {
        return this->sysinfo(info);
    };
    shmctl_callback = [this](std::int32_t shmid, std::int32_t cmd, struct shmid_ds* buf) {
        return this->shmctl(shmid, cmd, buf);
    };
}

std::int32_t daemon(std::int32_t _nochdir, std::int32_t _noclose)
{
    return daemon_callback(_nochdir, _noclose);
}

std::int32_t pipe2(std::int32_t pipedes[2], std::int32_t flags)
{
    return pipe2_callback(pipedes, flags);
}

std::int32_t statfs(const char* file, struct statfs* buf)
{
    return statfs2_callback(file, buf);
}

std::int32_t getifaddrs(struct ifaddrs** ifa)
{
    return getifaddrs_callback(ifa);
}

void freeifaddrs(struct ifaddrs* ifa)
{
    return freeifaddrs_callback(ifa);
}

bool pthread_setaffinity_np(std::int32_t i)
{
    return pthread_setaffinity_np_callback(i);
}

std::int32_t timerfd_create(__clockid_t clock_id, std::int32_t flags)
{
    return timerfd_create_callback(clock_id, flags);
}

std::int32_t timerfd_gettime(std::int32_t ufd, struct itimerspec* otmr)
{
    return timerfd_gettime_callback(ufd, otmr);
}

std::int32_t timerfd_settime(std::int32_t ufd,
                             std::int32_t flags,
                             const struct itimerspec* utmr,
                             struct itimerspec* otmr)
{
    return timerfd_settime_callback(ufd, flags, utmr, otmr);
}

std::int32_t clock_adjtime(clockid_t clkid, struct timex* tx)
{
    return clock_adjtime_callback(clkid, tx);
}

int sysinfo(struct sysinfo* info)
{
    return sysinfo_callback(info);
}

std::int32_t shmctl(std::int32_t shmid, std::int32_t cmd, struct shmid_ds* buf)
{
    return shmctl_callback(shmid, cmd, buf);
}

} /* namespace os */
} /* namespace score */
