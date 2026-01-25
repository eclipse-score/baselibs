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
#ifndef SCORE_LIB_OS_UTILS_INOTIFY_INOTIFY_INSTANCE_FACADE_H
#define SCORE_LIB_OS_UTILS_INOTIFY_INOTIFY_INSTANCE_FACADE_H

#include "score/os/utils/inotify/inotify_instance.h"
#include "score/os/utils/inotify/inotify_instance_mock.h"

#include <gmock/gmock.h>

namespace score::os
{

/// \brief Facade class which dispatches to a mock object which is owned by the caller
///
/// Such a facade class is useful when a test needs to mock an object for which ownership must be passed to the
/// class under test e.g. via a unique_ptr. In this case, the test requires that the mock survives until the end of the
/// test, however, this cannot be guaranteed when handing ownership to the class under test. Therefore, the test can
/// create the mock object and provide a facade object to the class under test (which will dispatch any calls to the
/// mock object).
class InotifyInstanceFacade : public InotifyInstance
{
  public:
    InotifyInstanceFacade(InotifyInstanceMock& inotify_instance_mock) : inotify_instance_mock_{inotify_instance_mock} {}

    score::cpp::expected_blank<Error> IsValid() const noexcept override
    {
        return inotify_instance_mock_.IsValid();
    }

    void Close() noexcept override
    {
        inotify_instance_mock_.Close();
    }

    score::cpp::expected<InotifyWatchDescriptor, Error> AddWatch(safecpp::zstring_view pathname,
                                                          Inotify::EventMask event_mask) noexcept override
    {
        return inotify_instance_mock_.AddWatch(pathname, event_mask);
    }

    score::cpp::expected_blank<Error> RemoveWatch(InotifyWatchDescriptor watch_descriptor) noexcept override
    {
        return inotify_instance_mock_.RemoveWatch(watch_descriptor);
    }

    score::cpp::expected<score::cpp::static_vector<InotifyEvent, max_events>, Error> Read() noexcept override
    {
        return inotify_instance_mock_.Read();
    }

  private:
    InotifyInstanceMock& inotify_instance_mock_;
};

}  // namespace score::os

#endif  // SCORE_LIB_OS_UTILS_INOTIFY_INOTIFY_INSTANCE_FACADE_H
