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
#include "score/os/qnx/unistd_impl.h"

#include <array>
#include <climits>
#include <cstddef>

score::cpp::expected<std::int32_t, score::os::Error> score::os::qnx::QnxUnistdImpl::setgroupspid(
    const score::cpp::span<const GroupId> grouplist,
    const pid_t pid) const noexcept
{
    constexpr std::size_t kMaxGroups{static_cast<std::size_t>(NGROUPS_MAX)};
    if (grouplist.size() > kMaxGroups)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno(EINVAL));
    }

    // Copy instead of reinterpret_cast: GroupId[] is not guaranteed to alias gid_t[].
    std::array<gid_t, kMaxGroups> native_groups{};
    std::size_t count{0U};
    for (const GroupId group : grouplist)
    {
        native_groups.at(count) = group.native();
        ++count;
    }

    const std::int32_t result = ::setgroupspid(static_cast<std::int32_t>(count), native_groups.data(), pid);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}
