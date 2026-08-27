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
#include "score/os/qnx/sysctl_impl.h"

namespace score
{
namespace os
{

score::cpp::expected_blank<Error> SysctlImpl::sysctl(std::int32_t* const name,
                                                     const std::size_t namelen,
                                                     void* const oldp,
                                                     std::size_t* const oldlenp,
                                                     void* const newp,
                                                     const std::size_t newlen) const noexcept
{
    const auto err = ::sysctl(name, static_cast<std::uint32_t>(namelen), oldp, oldlenp, newp, newlen);
    if (0 != err)
    {
        return score::cpp::make_unexpected(Error::createFromErrno(err));
    }
    return {};
}

score::cpp::expected_blank<Error> SysctlImpl::sysctlbyname(const char* sname,
                                                           void* const oldp,
                                                           std::size_t* const oldlenp,
                                                           void* const newp,
                                                           const std::size_t newlen) const noexcept
{
    const auto err = ::sysctlbyname(sname, oldp, oldlenp, newp, newlen);
    if (0 != err)
    {
        return score::cpp::make_unexpected(Error::createFromErrno(err));
    }
    return {};
}

score::cpp::expected_blank<Error> SysctlImpl::sysctlnametomib(const char* sname,
                                                              int* mibp,
                                                              size_t* sizep) const noexcept
{
    const auto err = ::sysctlnametomib(sname, mibp, sizep);
    if (0 != err)
    {
        return score::cpp::make_unexpected(Error::createFromErrno(err));
    }
    return {};
}

}  // namespace os
}  // namespace score
