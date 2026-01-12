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
#ifndef SCORE_LIB_OS_UTILS_MQUEUE_H
#define SCORE_LIB_OS_UTILS_MQUEUE_H

#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "score/expected.hpp"
#include "score/os/errno.h"

namespace score
{
namespace common
{

std::size_t get_string_as_hash(const std::string& name);

}
namespace os
{

enum class AccessMode : std::int32_t
{
    kCreate = 1,
    kUse = 2,
    kCreateNonBlocking = 3,
    kIfExistUseOthCreate = 4
};

class MQueue
{
    class MQueuePrivate;
    std::unique_ptr<MQueuePrivate> m_pointer;

  public:
    explicit MQueue(const std::string& name,
                    const AccessMode mode,
                    const size_t max_msg_size = 100UL,
                    const size_t max_msgs = 10UL);
    explicit MQueue(const std::string& name);
    explicit MQueue(const size_t id);
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    void send(const std::string& msg) const;
    /* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
    score::cpp::expected_blank<score::os::Error> send(const char* const msg, const size_t length) const;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    ssize_t timed_send(const char* const msg, const size_t length, const std::chrono::milliseconds timeout) const;
    void receive(char* const msg) const;
    /* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
    std::int32_t get_msg_size() const;
    std::string receive() const;
    /* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
    std::pair<ssize_t, bool> timed_receive(char* const msg, const std::chrono::milliseconds timeout) const;
    /* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
    std::pair<std::string, bool> timed_receive(const std::chrono::milliseconds timeout) const;
    score::cpp::expected_blank<score::os::Error> unlink() const;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    size_t get_id() const;

    score::cpp::expected<std::uint32_t, Error> get_mq_st_mode() const noexcept;

    MQueue(MQueue&&) noexcept;

    MQueue& operator=(MQueue&&) noexcept;

    MQueue(MQueue&) = delete;

    MQueue& operator=(MQueue&) = delete;

    ~MQueue();
};

}  // namespace os
}  // namespace score

#endif
