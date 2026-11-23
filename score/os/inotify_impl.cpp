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
#include "score/os/inotify_impl.h"

#include <sys/inotify.h>

#include <cerrno>
#include <type_traits>

namespace score
{
namespace os
{

namespace internal
{

std::uint32_t EventMaskToInteger(const Inotify::EventMask event_mask) noexcept
{
    std::uint32_t native_event_masks{};
    using utype_eventmask = std::underlying_type<Inotify::EventMask>::type;
    if (static_cast<utype_eventmask>(event_mask & Inotify::EventMask::kAccess) != 0U)
    {
        native_event_masks |= static_cast<std::uint32_t>(IN_ACCESS);
    }
    if (static_cast<utype_eventmask>(event_mask & Inotify::EventMask::kInMovedTo) != 0U)
    {
        native_event_masks |= static_cast<std::uint32_t>(IN_MOVED_TO);
    }
    if (static_cast<utype_eventmask>(event_mask & Inotify::EventMask::kInCreate) != 0U)
    {
        native_event_masks |= static_cast<std::uint32_t>(IN_CREATE);
    }
    if (static_cast<utype_eventmask>(event_mask & Inotify::EventMask::kInDelete) != 0U)
    {
        native_event_masks |= static_cast<std::uint32_t>(IN_DELETE);
    }
    return native_event_masks;
}

}  // namespace internal

score::cpp::expected<std::int32_t, Error> InotifyImpl::inotify_init() const noexcept
{
    return this->inotify_init1(InitFlag::kCloseOnExec);
}

score::cpp::expected<std::int32_t, Error> InotifyImpl::inotify_init1(const InitFlag flags) const noexcept
{
    // Unknown flags are rejected like the OS does, instead of being dropped.
    constexpr InitFlag kSupportedFlags{InitFlag::kCloseOnExec | InitFlag::kNonBlocking};
    if (static_cast<std::uint32_t>(flags & ~kSupportedFlags) != 0U)
    {
        return score::cpp::make_unexpected(Error::createFromErrno(EINVAL));
    }
    // The flags are applied by inotify_init1() atomically, so no concurrent fork() can leak the descriptor.
    std::uint32_t native_flags{0U};
    if (static_cast<std::uint32_t>(flags & InitFlag::kCloseOnExec) != 0U)
    {
        native_flags |= static_cast<std::uint32_t>(IN_CLOEXEC);
    }
    if (static_cast<std::uint32_t>(flags & InitFlag::kNonBlocking) != 0U)
    {
        native_flags |= static_cast<std::uint32_t>(IN_NONBLOCK);
    }
    // Manual code analysis:
    // inotify_init1() fails when there is not enough memory, when the system/user has reached the maximum number of
    // inotify instances, or due to other kernel or file system limitations. There is no reliable way to simulate
    // above error cases within the scope of a unit test.
    const std::int32_t ret{::inotify_init1(static_cast<std::int32_t>(native_flags))};
    if (ret < 0)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(Error::createFromErrno());  // LCOV_EXCL_LINE
    }
    return ret;
}

score::cpp::expected<std::int32_t, Error> InotifyImpl::inotify_add_watch(const std::int32_t fd,
                                                                         const char* const pathname,
                                                                         const EventMask mask) const noexcept
{
    const std::uint32_t native_event_mask{score::os::internal::EventMaskToInteger(mask)};
    const std::int32_t ret{::inotify_add_watch(fd, pathname, native_event_mask)};
    if (ret < 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected<std::int32_t, Error> InotifyImpl::inotify_rm_watch(const std::int32_t fd,
                                                                        const std::int32_t wd) const noexcept
{
    const std::int32_t ret{::inotify_rm_watch(fd, wd)};
    if (ret < 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}

}  // namespace os
}  // namespace score
