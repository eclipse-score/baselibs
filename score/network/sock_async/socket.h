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
#ifndef SCORE_LIB_NETWORK_NET_SOCKET_H
#define SCORE_LIB_NETWORK_NET_SOCKET_H

#include "score/network/i_socket.h"
#include <score/variant.hpp>

namespace score
{
namespace os
{

class SocketBase : public ISocket
{
  public:
    SocketBase(const Endpoint endpoint) noexcept;
    void SetOption(const SockOptionVariant optionVariant) noexcept override;
    score::cpp::expected<ssize_t, score::os::Error> WriteSync(
        std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> buffer) const noexcept override;
    score::cpp::variant<ssize_t, std::tuple<ssize_t, score::os::Ipv4Address>, score::os::Error> ReadSync(
        std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> buffer) noexcept override;

    SocketBase(SocketBase&&) noexcept = delete;
    SocketBase(const SocketBase&) = delete;
    SocketBase& operator=(SocketBase&&) & noexcept = delete;
    SocketBase& operator=(const SocketBase&) & noexcept = delete;

    virtual std::int32_t GetSockFD() const noexcept = 0;

    void Bind(const Endpoint endpoint) noexcept;
    bool IsBound() const noexcept;
    SockOptionVariant GetOptionVariant() const noexcept;
    Endpoint GetEndpoint() const noexcept;

    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> GetReadBuffer() noexcept;
    void SetReadBuffer(std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> buf) noexcept;
    AsyncCallback GetReadCb() noexcept;
    void SetReadCb(AsyncCallback cb) noexcept;

    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> GetWriteBuffer() noexcept;
    void SetWriteBuffer(std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> buf) noexcept;
    AsyncCallback GetWriteCb() noexcept;
    void SetWriteCb(AsyncCallback cb) noexcept;

    AsyncConnectCallback GetConnectCb() noexcept;
    void SetConnectCb(AsyncConnectCallback cb) noexcept;

  protected:
    std::int32_t socket_fd_; /* KW_SUPPRESS:MISRA.MEMB.NOT_PRIVATE:False Needed in derived socket*/

  private:
    Endpoint endpoint_;
    bool is_bound_;
    SockOptionVariant option_variant_;

    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> read_buffer_;
    AsyncCallback read_cb_;
    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> write_buffer_;
    AsyncCallback write_cb_;
    AsyncConnectCallback connect_cb_;
};
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_NETWORK_NET_SOCKET_H
