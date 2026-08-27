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
#include "score/os/qnx/secpol_impl.h"
#include "score/os/qnx/secpol.h"

namespace score
{
namespace os
{
namespace qnx
{

namespace details
{

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<secpol_file_t*, score::os::Error> SecpolImpl::secpol_open(const char* const path,
                                                                               const std::uint32_t flags) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    secpol_file_t* const result = ::secpol_open(path, flags);
    if (result == nullptr)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected_blank<score::os::Error> SecpolImpl::secpol_close(secpol_file_t* const handle) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    ::secpol_close(handle);

    return {};
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, score::os::Error> SecpolImpl::secpol_posix_spawnattr_settypeid(
    secpol_file_t* const handle,
    posix_spawnattr_t* const attrp,
    const char* const name,
    const std::uint32_t flags) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::secpol_posix_spawnattr_settypeid(handle, attrp, name, flags);
    if (result != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, score::os::Error> SecpolImpl::secpol_transition_type(
    secpol_file_t* const handle,
    const char* const name,
    const std::uint32_t flags) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::secpol_transition_type(handle, name, flags);
    if (result != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}

}  // namespace details
}  // namespace qnx
}  // namespace os
}  // namespace score
