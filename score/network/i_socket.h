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
#ifndef SCORE_LIB_NETWORK_SOCK_ASYNC_I_SOCKET_H_
#define SCORE_LIB_NETWORK_SOCK_ASYNC_I_SOCKET_H_

#include "score/network/sock_async/net_endpoint.h"
#include "score/os/socket.h"
#include <score/callback.hpp>
#include <score/span.hpp>
#include <score/variant.hpp>
#include <sys/time.h> /* KW_SUPPRESS:MISRA.INCL.UNSAFE:time.h libarary needed for TimeoutOption */
#include <cstdint>

namespace score
{

namespace os
{
/* KW_SUPPRESS_START:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False Positive */
/* KW_SUPPRESS_START:MISRA.ONEDEFRULE.VAR: False Positive */
/* KW_SUPPRESS_START:MISRA.VAR.NEEDS.CONST: False Positive */
constexpr const std::int32_t ENABLE_OPTION = 1;
/* KW_SUPPRESS_END:MISRA.VAR.NEEDS.CONST: False Positive */
/* KW_SUPPRESS_END:MISRA.ONEDEFRULE.VAR: False Positive */
/* KW_SUPPRESS_END:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False Positive */
class SockOption
{
  public:
    virtual ~SockOption() = default;
    virtual score::cpp::expected_blank<score::os::Error> apply(const std::int32_t sockfd) const = 0;
};

class ReuseAddrOption : public SockOption
{
  public:
    /* KW_SUPPRESS_START:MISRA.CTOR.BASE: Pure virtual base, no ctor */
    ReuseAddrOption() : optval_(ENABLE_OPTION) {}
    /* KW_SUPPRESS_END:MISRA.CTOR.BASE */

    score::cpp::expected_blank<score::os::Error> apply(const std::int32_t sockfd) const override
    {
        return score::os::Socket::instance().setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval_, sizeof(optval_));
    }

  private:
    std::int32_t optval_;
};

class TimeoutOption : public SockOption
{
  public:
    TimeoutOption(const std::int32_t seconds) : SockOption(), seconds_(seconds) {}

    score::cpp::expected_blank<score::os::Error> apply(const std::int32_t sockfd) const override
    {
        struct timeval timeout;
        timeout.tv_sec = seconds_;
        timeout.tv_usec = 0;
        return score::os::Socket::instance().setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    }

  private:
    std::int32_t seconds_;
};

class BufferSizeOption : public SockOption
{
  public:
    BufferSizeOption(const std::int32_t size) : SockOption(), size_(size) {}

    score::cpp::expected_blank<score::os::Error> apply(const std::int32_t sockfd) const override
    {
        return score::os::Socket::instance().setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &size_, sizeof(size_));
    }

  private:
    std::int32_t size_;
};

using AsyncCallback = score::cpp::callback<void(std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>>, ssize_t)>;
using AsyncConnectCallback = score::cpp::callback<void(std::int16_t)>;
using Endpoint = score::os::NetEndpoint;
using SockOptionVariant = score::cpp::variant<ReuseAddrOption, TimeoutOption, BufferSizeOption>;
class ISocket
{
  public:
    virtual void SetOption(const SockOptionVariant optionVariant) noexcept = 0;
    virtual std::int32_t WriteAsync(std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> buffer,
                                    AsyncCallback cb) noexcept = 0;
    virtual score::cpp::expected<ssize_t, score::os::Error> WriteSync(
        std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> buffer) const noexcept = 0;
    virtual std::int32_t ReadAsync(std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> buffer,
                                   AsyncCallback cb) noexcept = 0;
    virtual score::cpp::variant<ssize_t, std::tuple<ssize_t, score::os::Ipv4Address>, score::os::Error> ReadSync(
        std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> buffer) noexcept = 0;

    virtual std::int32_t ConnectAsync(AsyncConnectCallback cb) noexcept = 0;
    // virtual void Unblock() noexcept = 0;
    virtual ~ISocket() noexcept = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_NETWORK_SOCK_ASYNC_I_SOCKET_H_
