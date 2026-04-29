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
#include "score/os/utils/inotify/inotify_instance_mock.h"
#include <score/assert.hpp>

namespace score
{
namespace os
{

InotifyEvent MakeFakeEvent(const std::int32_t wd,
                           const std::uint32_t mask,
                           const std::uint32_t cookie,
                           const std::string_view name)
{
    constexpr auto max_name_length{NAME_MAX};
    constexpr auto terminator_length{1};
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(name.size() <= max_name_length, "POSIX restricts name to size NAME_MAX");

    alignas(struct ::inotify_event)
        std::array<char, sizeof(struct ::inotify_event) + max_name_length + terminator_length>
            storage{};
    struct ::inotify_event* c_event{new (&storage)::inotify_event{}};

    c_event->wd = wd;
    c_event->mask = mask;
    c_event->cookie = cookie;
    c_event->len = static_cast<std::uint32_t>(name.size());
    std::copy(name.cbegin(), name.cend(), c_event->name);
    *(c_event->name + name.size()) = '\0';

    os::InotifyEvent event{*c_event};
    c_event->~inotify_event();
    return event;
}

}  // namespace os
}  // namespace score
