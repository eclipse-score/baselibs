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
#ifndef SCORE_LIB_OS_UTILS_INOTIFY_INOTIFY_EVENT_H
#define SCORE_LIB_OS_UTILS_INOTIFY_INOTIFY_EVENT_H

#include "score/os/utils/inotify/inotify_watch_descriptor.h"

#include "score/bitmanipulation/bitmask_operators.h"
#include "score/os/inotify.h"

#include <score/static_vector.hpp>

#include <sys/inotify.h>

#include <climits>
#include <cstdint>
#include <string_view>

namespace score
{
namespace os
{

class InotifyEvent
{
  public:
    enum class ReadMask : std::uint32_t
    {
        kUnknown = 0U,        /* Unknown event */
        kInAccess = 1U,       /* File was accessed */
        kInMovedTo = 128U,    /* File was moved or renamed to the item being watched */
        kInCreate = 256U,     /* File was created in a watched directory */
        kInDelete = 512U,     /* File was deleted in a watched directory */
        kInIgnored = 1024U,   /* Watch was removed */
        kInIsDir = 2048U,     /* Subject of this event is a directory */
        kInQOverflow = 4096U, /* Event queue overflowed */
    };

    explicit InotifyEvent(const struct ::inotify_event& event);

    [[nodiscard]] InotifyWatchDescriptor GetWatchDescriptor() const noexcept;

    [[nodiscard]] ReadMask GetMask() const noexcept;

    [[nodiscard]] std::uint32_t GetCookie() const noexcept;

    [[nodiscard]] std::string_view GetName() const noexcept;

  private:
    InotifyWatchDescriptor watch_descriptor_;
    ReadMask mask_;
    std::uint32_t cookie_;
    score::cpp::static_vector<char, NAME_MAX + 1> name_;

    static ReadMask IntegerToReadMask(const std::uint32_t native_event_mask) noexcept;
};

bool operator==(const InotifyEvent& lhs, const InotifyEvent& rhs) noexcept;

}  // namespace os
}  // namespace score

namespace score
{
template <>
struct enable_bitmask_operators<::score::os::InotifyEvent::ReadMask> : public std::true_type
{
};
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_INOTIFY_INOTIFY_EVENT_H
