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
#include "score/os/qnx/devctl_impl.h"

namespace score
{
namespace os
{

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected_blank<Error> DevctlImpl::devctl(const std::int32_t fd,
                                                     const std::int32_t dev_cmd,
                                                     void* const dev_data_ptr,
                                                     const std::size_t n_bytes,
                                                     std::int32_t* const dev_info_ptr) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t err = ::devctl(fd, dev_cmd, dev_data_ptr, n_bytes, dev_info_ptr);
    if (0 != err)
    {
        return score::cpp::make_unexpected(Error::createFromErrno(err));
    }
    return {};
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected_blank<Error> DevctlImpl::devctlv(const std::int32_t fd,
                                                      const std::int32_t dev_cmd,
                                                      const std::int32_t sparts,
                                                      const std::int32_t rparts,
                                                      const iovec* const sv,
                                                      const iovec* const rv,
                                                      std::int32_t* const dev_info_ptr) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t err = ::devctlv(fd, dev_cmd, sparts, rparts, sv, rv, dev_info_ptr);
    if (0 != err)
    {
        return score::cpp::make_unexpected(Error::createFromErrno(err));
    }
    return {};
}

}  // namespace os
}  // namespace score
