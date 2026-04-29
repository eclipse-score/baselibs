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
#ifndef SCORE_LIB_OS_MQUEUE_H
#define SCORE_LIB_OS_MQUEUE_H

#include "score/bitmanipulation/bitmask_operators.h"
#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"
#include "score/os/static_destruction_guard.h"

#include "score/expected.hpp"
#include "score/memory.hpp"

#include <mqueue.h>
#include <unistd.h>
#include <ctime>

/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */

namespace score
{
namespace os
{

class Mqueue : public ObjectSeam<Mqueue>
{
  public:
    static Mqueue& instance() noexcept;

    static score::cpp::pmr::unique_ptr<Mqueue> Default(score::cpp::pmr::memory_resource* memory_resource) noexcept;

    enum class OpenFlag : std::int32_t
    {
        kReadOnly = 0x01,
        kWriteOnly = 0x02,
        kReadWrite = 0x04,
        kCreate = 0x08,
        kCloseOnExec = 0x10,
        kNonBlocking = 0x20,
        kExclusive = 0x40,
    };

    enum class ModeFlag : std::int32_t
    {
        kReadUser = 0x01,
        kWriteUser = 0x02,
        kExecUser = 0x04,
        kReadGroup = 0x08,
        kWriteGroup = 0x10,
        kExecGroup = 0x20,
        kReadOthers = 0x40,
        kWriteOthers = 0x80,
        kExecOthers = 0x100,
    };

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:system functions are not hidden as far these counterparts are part of class */

    virtual score::cpp::expected<std::int32_t, Error> mq_open(const char* const name,
                                                       const OpenFlag flags,
                                                       const ModeFlag perm,
                                                       mq_attr* const attr) const noexcept = 0;
    virtual score::cpp::expected<std::int32_t, Error> mq_open(const char* const name, const OpenFlag flags) const noexcept = 0;
    virtual score::cpp::expected_blank<Error> mq_unlink(const char* const name) const noexcept = 0;
    virtual score::cpp::expected_blank<Error> mq_send(const mqd_t mqdes,
                                               const char* const msg_ptr,
                                               const size_t msg_len,
                                               const std::uint32_t msg_prio) const noexcept = 0;
    virtual score::cpp::expected_blank<Error> mq_timedsend(const mqd_t mqdes,
                                                    const char* const msg_ptr,
                                                    const size_t msg_len,
                                                    const std::uint32_t msg_prio,
                                                    const struct timespec* const timeout) const noexcept = 0;
    virtual score::cpp::expected<ssize_t, Error> mq_receive(const mqd_t mqdes,
                                                     char* const msg_ptr,
                                                     const size_t msg_len,
                                                     std::uint32_t* const msg_prio) const noexcept = 0;
    virtual score::cpp::expected<ssize_t, Error> mq_timedreceive(const mqd_t mqdes,
                                                          char* const msg_ptr,
                                                          const size_t msg_len,
                                                          std::uint32_t* const msg_prio,
                                                          const struct timespec* const timeout) const noexcept = 0;
    virtual score::cpp::expected_blank<Error> mq_close(const mqd_t mqdes) const noexcept = 0;
    virtual score::cpp::expected_blank<Error> mq_getattr(const mqd_t mqdes, mq_attr& mqstat) const noexcept = 0;

    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: system functions are not hidden as far these counterparts are part of class */

    virtual ~Mqueue() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Mqueue(const Mqueue&) = delete;
    Mqueue& operator=(const Mqueue&) = delete;
    Mqueue(Mqueue&& other) = delete;
    Mqueue& operator=(Mqueue&& other) = delete;

  protected:
    Mqueue() = default;
};

namespace impl
{

class MqueueImpl final : public Mqueue
{
  public:
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:system functions are not hidden as far these counterparts are part of class */
    score::cpp::expected<std::int32_t, Error> mq_open(const char* const name,
                                               const OpenFlag flags,
                                               const ModeFlag perm,
                                               mq_attr* const attr) const noexcept override;

    score::cpp::expected<std::int32_t, Error> mq_open(const char* const name, const OpenFlag flags) const noexcept override;

    score::cpp::expected_blank<Error> mq_unlink(const char* const name) const noexcept override;

    score::cpp::expected_blank<Error> mq_send(const mqd_t mqdes,
                                       const char* const msg_ptr,
                                       const size_t msg_len,
                                       const std::uint32_t msg_prio) const noexcept override;

    score::cpp::expected_blank<Error> mq_timedsend(const mqd_t mqdes,
                                            const char* const msg_ptr,
                                            const size_t msg_len,
                                            const std::uint32_t msg_prio,
                                            const struct timespec* const timeout) const noexcept override;

    score::cpp::expected<ssize_t, Error> mq_receive(const mqd_t mqdes,
                                             char* const msg_ptr,
                                             const size_t msg_len,
                                             std::uint32_t* const msg_prio) const noexcept override;

    score::cpp::expected<ssize_t, Error> mq_timedreceive(const mqd_t mqdes,
                                                  char* const msg_ptr,
                                                  const size_t msg_len,
                                                  std::uint32_t* const msg_prio,
                                                  const struct timespec* const timeout) const noexcept override;

    score::cpp::expected_blank<Error> mq_close(const mqd_t mqdes) const noexcept override;

    score::cpp::expected_blank<Error> mq_getattr(const mqd_t mqdes, mq_attr& mqstat) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: system functions are not hidden as far these counterparts are part of class */

  private:
    std::int32_t openflag_to_nativeflag(const OpenFlag flags) const noexcept;

    mode_t modeflag_to_nativeflag(const ModeFlag flags) const noexcept;
};

// We have to apply Nifty Counter idiom, since mqueue calls are performed in static destructors.
// Suppress "AUTOSAR C++14 A3-1-1", The rule states: "It shall be possible to include any header file
// in multiple translation units without violating the One Definition Rule."
// This is false positive, Static variable "nifty_counter_mqueue" ensures ODR
// because of include guard of the header file.
// Suppress "AUTOSAR C++14 A2-10-4", The rule states: "The identifier name of a non-member object with
// static storage duration or static function shall not be reused within a namespace."
// nifty_counter_mqueue is unique and not reused elsewhere in score::os::impl
// Suppress "AUTOSAR C++14 A3-3-2", The rule states: "Static and thread-local objects shall be constant-initialized."
// Justification: templatized static, will be used elsewhere, cannot initialize
// coverity[autosar_cpp14_a3_1_1_violation]
// coverity[autosar_cpp14_a3_3_2_violation]
// coverity[autosar_cpp14_a2_10_4_violation]
static StaticDestructionGuard<MqueueImpl> nifty_counter_mqueue;

}  // namespace impl
}  // namespace os
}  // namespace score

namespace score
{
template <>
struct enable_bitmask_operators<::score::os::Mqueue::OpenFlag> : public std::true_type
{
};

template <>
struct enable_bitmask_operators<::score::os::Mqueue::ModeFlag> : public std::true_type
{
};

}  // namespace score

#endif  // SCORE_LIB_OS_MQUEUE_H
