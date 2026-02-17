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
#ifndef SCORE_LIB_OS_UTILS_INOTIFY_INOTIFY_INSTANCE_MOCK_H
#define SCORE_LIB_OS_UTILS_INOTIFY_INOTIFY_INSTANCE_MOCK_H

#include "score/os/utils/inotify/inotify_instance.h"

#include <gmock/gmock.h>

namespace score
{
namespace os
{

InotifyEvent MakeFakeEvent(const std::int32_t wd,
                           const std::uint32_t mask,
                           const std::uint32_t cookie,
                           const std::string_view name);

class InotifyInstanceMock : public InotifyInstance
{
  public:
    using InotifyInstance::max_events;

    MOCK_METHOD(score::cpp::expected_blank<Error>, IsValid, (), (const, noexcept, override));
    MOCK_METHOD(void, Close, (), (noexcept, override));
    MOCK_METHOD((score::cpp::expected<InotifyWatchDescriptor, Error>),
                AddWatch,
                (safecpp::zstring_view, Inotify::EventMask),
                (noexcept, override));
    MOCK_METHOD(score::cpp::expected_blank<Error>, RemoveWatch, (InotifyWatchDescriptor), (noexcept, override));
    MOCK_METHOD((score::cpp::expected<score::cpp::static_vector<InotifyEvent, max_events>, Error>), Read, (), (noexcept, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_INOTIFY_INOTIFY_INSTANCE_MOCK_H
