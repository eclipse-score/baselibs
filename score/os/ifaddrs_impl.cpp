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
#include "score/os/ifaddrs_impl.h"
#include "score/os/errno.h"

namespace score
{
namespace os
{
// LCOV_EXCL_START
// It appears to be impossible to test it using a unit test.
// Cannot not be mocked
score::cpp::expected<ifaddrs*, Error> IfaddrsImpl::getifaddrs() const noexcept
{
    ifaddrs* ifa{nullptr};
    const std::int32_t ret = ::getifaddrs(&ifa);
    if (ret == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ifa;
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
// It appears to be impossible to test it using a unit test.
// It is not possible to check if memory has been properly freed
void IfaddrsImpl::freeifaddrs(ifaddrs* ifa) const noexcept
{
    ::freeifaddrs(ifa);
}
// LCOV_EXCL_STOP

}  // namespace os
}  // namespace score
