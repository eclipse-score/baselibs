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
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        native_event_masks |= static_cast<std::uint32_t>(IN_ACCESS);
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    }
    if (static_cast<utype_eventmask>(event_mask & Inotify::EventMask::kInMovedTo) != 0U)
    {
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        native_event_masks |= static_cast<std::uint32_t>(IN_MOVED_TO);
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    }
    if (static_cast<utype_eventmask>(event_mask & Inotify::EventMask::kInCreate) != 0U)
    {
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        native_event_masks |= static_cast<std::uint32_t>(IN_CREATE);
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    }
    if (static_cast<utype_eventmask>(event_mask & Inotify::EventMask::kInDelete) != 0U)
    {
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        native_event_masks |= static_cast<std::uint32_t>(IN_DELETE);
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    }
    return native_event_masks;
}

}  // namespace internal

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> InotifyImpl::inotify_init() const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    // Manual code analysis:
    // inotify_init() fails when there is not enough memory, when the system/user has reached the maximum number of
    // inotify instances, or due to other kernel or file system limitations. There is no reliable way to simulate
    // above error cases within the scope of a unit test.
    const std::int32_t ret{::inotify_init()};
    if (ret < 0)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(Error::createFromErrno());  // LCOV_EXCL_LINE
    }
    return ret;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> InotifyImpl::inotify_add_watch(const std::int32_t fd,
                                                                         const char* const pathname,
                                                                         const EventMask mask) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::uint32_t native_event_mask{score::os::internal::EventMaskToInteger(mask)};
    const std::int32_t ret{::inotify_add_watch(fd, pathname, native_event_mask)};
    if (ret < 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> InotifyImpl::inotify_rm_watch(const std::int32_t fd,
                                                                        const std::int32_t wd) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
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
