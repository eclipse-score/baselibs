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
#ifndef SCORE_LIB_OS_UTILS_INOTIFY_INOTIFY_INSTANCE_IMPL_H
#define SCORE_LIB_OS_UTILS_INOTIFY_INOTIFY_INSTANCE_IMPL_H

#include "score/language/safecpp/string_view/zstring_view.h"
#include "score/os/utils/inotify/inotify_event.h"
#include "score/os/utils/inotify/inotify_instance.h"
#include "score/os/utils/inotify/inotify_watch_descriptor.h"

#include "score/os/utils/abortable_blocking_reader.h"

#include "score/os/errno.h"
#include "score/os/inotify.h"

#include <score/expected.hpp>

#include <memory>
#include <shared_mutex>

namespace score
{
namespace os
{

/**
 * @brief RAII-style wrapper around inotify subsystem.
 * Each InotifyInstanceImpl calls inotify_init() at construction and closes the returned file descriptor on shutdown.
 */
class InotifyInstanceImpl : public InotifyInstance
{
  public:
    /**
     * @brief The maximum amount of events a call to Read() can return
     */
    using InotifyInstance::max_events;

    /**
     * @brief Constructor for an Inotify-Instance
     */
    InotifyInstanceImpl() noexcept;

    ~InotifyInstanceImpl() noexcept override;

    /**
     * @brief Constructor for an Inotify-Instance used for injection of OSAL dependencies
     * Use the default constructor in production code
     */
    explicit InotifyInstanceImpl(const std::shared_ptr<Inotify>& inotify,
                                 const std::shared_ptr<Fcntl>& fcntl,
                                 const std::shared_ptr<SysPoll>& syspoll,
                                 const std::shared_ptr<Unistd>& unistd) noexcept;

    InotifyInstanceImpl(const InotifyInstanceImpl&) = delete;
    InotifyInstanceImpl& operator=(const InotifyInstanceImpl&) = delete;
    InotifyInstanceImpl(InotifyInstanceImpl&& other) noexcept = delete;
    InotifyInstanceImpl& operator=(InotifyInstanceImpl&& other) noexcept = delete;

    /**
     * @brief Returns whether construction was successful or errors that occurred
     * @return Blank on successful construction or the error
     */
    score::cpp::expected_blank<Error> IsValid() const noexcept override;

    /**
     * @brief Closes the inotify instance and unblocks all pending read operations
     */
    void Close() noexcept override;

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
    score::cpp::expected<InotifyWatchDescriptor, Error> AddWatch(safecpp::zstring_view pathname,
                                                          Inotify::EventMask event_mask) noexcept override;

    /**
     * @brief Removes a watch from the inotify instance
     * @param watch_descriptor The watch descriptor returned from a call to AddWatch() on the same inotify instance
     * @return Blank on success or errors
     */
    score::cpp::expected_blank<Error> RemoveWatch(InotifyWatchDescriptor watch_descriptor) noexcept override;

    /**
     * @brief Blocking read operation on the inotify instance to gather events from the watches
     *
     * The read blocks until there is at least one event to return, an interrupt occurred or the inotify instance was
     * destructed.
     *
     * @return An array with the events that occurred or an error.
     *         Returns at most max_events events, but may return less if the events contain a name.
     */
    score::cpp::expected<score::cpp::static_vector<InotifyEvent, max_events>, Error> Read() noexcept override;

  private:
    std::shared_ptr<Inotify> inotify_;
    score::cpp::expected_blank<Error> construction_error_;
    NonBlockingFileDescriptor inotify_file_descriptor_;
    AbortableBlockingReader reader_;

    /**
     * @brief Mutex to protect the inotify_file_descriptor_
     *
     * The inotify_file_descriptor_ is modified in Close() and read in AddWatch, RemoveWatch and Read. Therefore, it
     * should be acquired with a unique_lock in Close but can be acquired with a shared_lock in AddWatch, RemoveWatch
     * and Read to allow these 3 functions to be called concurrently.
     */
    std::shared_timed_mutex inotify_file_descriptor_mutex_;

    static score::cpp::expected<NonBlockingFileDescriptor, Error>
    InitializeInotify(Inotify& inotify, Fcntl& fcntl, const std::shared_ptr<Unistd>& unistd) noexcept;

    void InternalClose() noexcept;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_INOTIFY_INOTIFY_INSTANCE_IMPL_H
