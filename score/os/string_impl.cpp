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
#include "score/os/string_impl.h"

#include <cstring>

namespace score
{
namespace os
{

void* StringImpl::memcpy(void* const dest, const void* const src, const std::size_t n) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    return ::memcpy(dest, src, n);
}

char* StringImpl::strerror(const std::int32_t errnum) const noexcept
{
    return ::strerror(errnum);
}

void* StringImpl::memset(void* const dest, const std::int32_t c, const std::size_t n) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    return ::memset(dest, c, n);
}

}  // namespace os
}  // namespace score
