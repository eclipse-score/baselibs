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
#include "score/os/ioctl_impl.h"
#include <sys/ioctl.h>

namespace score
{
namespace os
{
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: Wrapper Function is identifiable through namespace usage */
score::cpp::expected_blank<Error> IoctlImpl::ioctl(const std::int32_t d,
                                                   const std::int32_t request,
                                                   void* const arg) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper Function is identifiable through namespace usage */
{
    // This is a wrapper over C banned function, thus the suppression is justified.
    // POSIX method accepts c-style vararg. For linux function accepts unsigned long
    // NOLINTNEXTLINE(*pro-type-vararg, *narrowing-conversions, score-banned-function) see comment above
    if (::ioctl(d, static_cast<std::uint64_t>(request), arg) == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return {};
}
}  // namespace os
}  // namespace score
