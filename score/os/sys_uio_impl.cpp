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
#include "score/os/sys_uio_impl.h"

namespace score
{
namespace os
{

score::cpp::expected<std::int64_t, Error> SysUioImpl::writev(const std::int32_t fd,
                                                             const struct iovec* iovec_ptr,
                                                             const std::int32_t count) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    std::int64_t ret{::writev(fd, iovec_ptr, count)};
    if (ret < 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}

}  // namespace os
}  // namespace score
