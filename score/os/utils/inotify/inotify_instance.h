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
#ifndef SCORE_LIB_OS_UTILS_INOTIFY_INOTIFY_INSTANCE_H
#define SCORE_LIB_OS_UTILS_INOTIFY_INOTIFY_INSTANCE_H

#include "score/language/safecpp/string_view/zstring_view.h"
#include "score/os/utils/inotify/inotify_event.h"
#include "score/os/utils/inotify/inotify_watch_descriptor.h"

#include <score/expected.hpp>
#include <score/static_vector.hpp>

namespace score
{
namespace os
{

class InotifyInstance
{
  public:
    /**
     * @brief The maximum amount of events a call to Read() can return
     */
    static constexpr const unsigned int max_events{10U};

    virtual ~InotifyInstance() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    InotifyInstance(const InotifyInstance&) = delete;
    InotifyInstance& operator=(const InotifyInstance&) = delete;
    InotifyInstance(InotifyInstance&& other) = delete;
    InotifyInstance& operator=(InotifyInstance&& other) = delete;

    /**
     * @brief Returns whether construction was successful or errors that occurred
     * @return Blank on successful construction or the error
     */
    virtual score::cpp::expected_blank<Error> IsValid() const noexcept = 0;

    /**
     * @brief Closes the inotify instance and unblocks all pending read operations
     */
    virtual void Close() noexcept = 0;

    /**
     * @brief Adds a watch to the inotify instance
     *
     * Consider the influence of the event_mask on the pathname.
     * For more details check https://man7.org/linux/man-pages/man7/inotify.7.html
     *
     * Capabilities and behavior of this operation depend on the operating system and its configuration.
     * Differences between operating systems can not be fully hidden in this abstraction layer.
     * Please consider the system manual.
     *
     * @param pathname Path to an __existing__ inode
     * @param event_mask The mask of events to watch for
     * @return Descriptor to identify the watch and its associated events
     */
    virtual score::cpp::expected<InotifyWatchDescriptor, Error> AddWatch(safecpp::zstring_view pathname,
                                                                  Inotify::EventMask event_mask) noexcept = 0;

    /**
     * @brief Removes a watch from the inotify instance
     * @param watch_descriptor The watch descriptor returned from a call to AddWatch() on the same inotify instance
     * @return Blank on success or errors
     */
    virtual score::cpp::expected_blank<Error> RemoveWatch(InotifyWatchDescriptor watch_descriptor) noexcept = 0;

    /**
     * @brief Blocking read operation on the inotify instance to gather events from the watches
     *
     * The read blocks until there is at least one event to return, an interrupt occurred or the inotify instance was
     * destructed.
     *
     * @return An array with the events that occurred or an error.
     *         Returns at most max_events events, but may return less if the events contain a name.
     */
    virtual score::cpp::expected<score::cpp::static_vector<InotifyEvent, max_events>, Error> Read() noexcept = 0;

  protected:
    InotifyInstance() = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_INOTIFY_INOTIFY_INSTANCE_H
