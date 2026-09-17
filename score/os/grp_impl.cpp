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
#include "score/os/grp_impl.h"

#include <score/utility.hpp>

#include <grp.h>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace score
{
namespace os
{
score::cpp::expected<GroupBuffer, Error> GrpImpl::getgrnam(const std::string& group) const noexcept
{
    if (group.size() > max_groupname_length)
    {
        return score::cpp::make_unexpected<>(score::os::Error::createFromErrno(ENOMEM));
    }

    std::lock_guard<std::mutex> lock{gr_buffer_mutex_};
    // Suppress "AUTOSAR C++14 M19-3-1", The rule states: "The error indicator errno shall not be used."
    // Using library-defined macro to ensure correct operation.
    // coverity[autosar_cpp14_m19_3_1_violation]
    errno = 0;

    // This is a wrapper over C banned function, thus the suppression is justified.
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const struct group* const native_group = ::getgrnam(group.c_str());
    if (native_group == nullptr)
    {
        // In case there is no error and the group is not found, ::getgrnam() will return a nullptr without
        // modifying errno. This is violated by Linux, which sets it to zero. In any case, we supply a better
        // fitting error number back to the user.
        //
        // Manual code analysis:
        // With above check for the case that no matching group was found, only a subset of the possible errors
        // remains. This subset consists of I/O errors or resource limitations in the kernel. Neither can be easily
        // triggered from with a unit test. In any of these cases ::getgrnam() will set an errno and return a
        // nullptr.

        // Suppress "AUTOSAR C++14 M19-3-1", The rule states: "The error indicator errno shall not be used."
        // Using library-defined macro to ensure correct operation.
        // coverity[autosar_cpp14_m19_3_1_violation]
        if (errno == 0)  // LCOV_EXCL_BR_LINE
        {
            return score::cpp::make_unexpected(score::os::Error::createFromErrno(ENOENT));
        }
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());  // LCOV_EXCL_LINE
    }
    return GroupToGroupBuffer(*native_group);
}

GroupBuffer GrpImpl::GroupToGroupBuffer(const struct group& native_group) noexcept
{
    GroupBuffer group_buffer{};
    // Suppressed here because usage of this OSAL method is on banned list
    score::cpp::ignore =  // NOLINTNEXTLINE(score-banned-function) see comment above
        std::memset(static_cast<char*>(group_buffer.name), 0, max_groupname_length + static_cast<std::size_t>(1));
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    score::cpp::ignore = std::memcpy(static_cast<char*>(group_buffer.name), native_group.gr_name, max_groupname_length);
    group_buffer.gid = native_group.gr_gid;
    return group_buffer;
}

}  // namespace os
}  // namespace score
