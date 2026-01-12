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
#include "score/network/sock_async_udp/socket_udp.h"
#include "score/os/unistd.h"
#include "score/mw/log/logging.h"

namespace score
{
namespace os
{

namespace
{
constexpr const char* kLogContext{"udpsoc"};
constexpr const std::int32_t INVALID_SOCKET_ID{-1};
}  // namespace

SocketUdp::SocketUdp(const std::shared_ptr<SocketCtrl> sock_ctrl, const Endpoint endpoint) : SocketAsync(endpoint)
{
    sock_ctrl_ = sock_ctrl;
    SocketAsync::SetReadStatus(false);
    SocketAsync::SetWriteStatus(false);
    score::cpp::expected<std::int32_t, Error> ret;
    ret = score::os::Socket::instance().socket(score::os::Socket::Domain::kIPv4, SOCK_DGRAM, 0);
    if (!ret.has_value())
    {
        score::mw::log::LogError(kLogContext) << "Failed to create udp socket";
    }
    else
    {
        socket_fd_ = ret.value();
    }
}

std::int32_t SocketUdp::GetSockFD() const noexcept
{
    return socket_fd_;
}

SocketUdp::~SocketUdp()
{
    if (socket_fd_ != INVALID_SOCKET_ID)
    {
        std::ignore = score::os::Unistd::instance().close(socket_fd_);
    }
}

/* KW_SUPPRESS_START:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False Positive */
std::int32_t SocketUdp::ReadAsync(std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> data,
                                  AsyncCallback u_cb) noexcept
/* KW_SUPPRESS_END:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False Positive */
{
    std::int32_t ret{kExitFailure};
    if (!SocketAsync::GetReadStatus())
    {
        ret = SocketAsync::ReadAsync(data, std::move(u_cb));
        if (!ret)
        {
            SocketAsync::SetReadStatus(true);
            if (sock_ctrl_.get() != nullptr)
            {
                return sock_ctrl_->RequestOperation(shared_from_this(), SockReq::READ);
            }
        }
    }
    return ret;
}

/* KW_SUPPRESS_START:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False Positive */
std::int32_t SocketUdp::WriteAsync(std::shared_ptr<std::vector<score::cpp::span<uint8_t>>> data, AsyncCallback u_cb) noexcept
/* KW_SUPPRESS_END:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False Positive */
{
    std::int32_t ret{kExitFailure};
    if (!SocketAsync::GetWriteStatus())
    {
        ret = SocketAsync::WriteAsync(data, std::move(u_cb));
        if (!ret)
        {
            SocketAsync::SetWriteStatus(true);
            if (sock_ctrl_.get() != nullptr)
            {
                return sock_ctrl_->RequestOperation(shared_from_this(), SockReq::WRITE);
            }
        }
    }
    return ret;
}

std::int32_t SocketUdp::ConnectAsync([[maybe_unused]] AsyncConnectCallback cb) noexcept
{
    return kExitNotSupported;
}

}  // namespace os
}  // namespace score
