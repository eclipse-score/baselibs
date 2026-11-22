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
#ifndef SCORE_LIB_OS_INOTIFY_H
#define SCORE_LIB_OS_INOTIFY_H

#include "score/bitmanipulation/bitmask_operators.h"
#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"

namespace score
{
namespace os
{

class Inotify : public ObjectSeam<Inotify>
{
  public:
    static Inotify& instance() noexcept;

    enum class EventMask : std::uint32_t
    {
        kUnknown = 0,     /* Unknown event */
        kAccess = 1,      /* File was accessed */
        kInMovedTo = 128, /* File was moved or renamed to the item being watched */
        kInCreate = 256,  /* File was created in a watched directory */
        kInDelete = 512,  /* File was deleted in a watched directory */
    };

    /// @brief Creation flags for inotify_init1(). They can be combined with the bitmask operators.
    enum class InitFlag : std::uint32_t
    {
        kNone = 0,
        kCloseOnExec = 1,  ///< The instance is not inherited by processes created via fork() + execve().
        kNonBlocking = 2,  ///< Reads on the instance do not block.
    };

    /// @brief Creates an inotify instance.
    /// It is created with close-on-exec (FD_CLOEXEC), as with inotify_init1(InitFlag::kCloseOnExec).
    /// This way it is not inherited by processes created via fork() + execve().
    /// To get an inheritable instance, use inotify_init1() with other flags.
    virtual score::cpp::expected<std::int32_t, Error> inotify_init() const noexcept = 0;
    /// @brief Creates an inotify instance with the given creation flags.
    /// The flags are applied atomically. InitFlag::kNone creates an inheritable, blocking instance.
    /// This method is not pure virtual, so that existing implementations of Inotify, for example fakes, keep compiling.
    /// The default implementation returns an error for ENOSYS. InotifyImpl overrides it.
    virtual score::cpp::expected<std::int32_t, Error> inotify_init1(const InitFlag flags) const noexcept;
    virtual score::cpp::expected<std::int32_t, Error> inotify_add_watch(const std::int32_t fd,
                                                                        const char* const pathname,
                                                                        const EventMask mask) const noexcept = 0;
    virtual score::cpp::expected<std::int32_t, Error> inotify_rm_watch(const std::int32_t fd,
                                                                       const std::int32_t wd) const noexcept = 0;

    ~Inotify() override = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Inotify(const Inotify&) = delete;
    Inotify& operator=(const Inotify&) = delete;
    Inotify(Inotify&& other) = delete;
    Inotify& operator=(Inotify&& other) = delete;

  protected:
    Inotify() = default;
};

}  // namespace os
}  // namespace score

namespace score
{
template <>
struct enable_bitmask_operators<::score::os::Inotify::EventMask> : public std::true_type
{
};

template <>
struct enable_bitmask_operators<::score::os::Inotify::InitFlag> : public std::true_type
{
};
}  // namespace score

#endif  // SCORE_LIB_OS_INOTIFY_H
