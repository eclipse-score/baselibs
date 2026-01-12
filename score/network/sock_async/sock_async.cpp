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
#include "score/network/sock_async/sock_async.h"
#include "score/os/socket.h"
#include "score/mw/log/logging.h"

#include <iostream>
namespace score
{
namespace os
{

namespace
{
constexpr const char* kLogContext{"sock_async"};
}  // namespace

SocketAsync::SocketAsync(const Endpoint endpoint) noexcept
    : SocketBase(endpoint), read_in_progress(false), write_in_progress(false)
{
}

/* KW_SUPPRESS_START:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False Positive */
std::int32_t SocketAsync::ReadAsync(std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> data,
                                    AsyncCallback u_cb) noexcept
/* KW_SUPPRESS_END:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False Positive */
{
    /* KW_SUPPRESS_START:MISRA.LOGIC.POSTFIX: False Positive */
    if (data->empty() || !data->at(0).size())
    /* KW_SUPPRESS_END:MISRA.LOGIC.POSTFIX: False Positive */
    {
        score::mw::log::LogError(kLogContext) << "Incorrect buffer provided";
        return kExitNumOfSocketsExceeded;
    }
    this->SetReadCb(std::move(u_cb));
    this->SetReadBuffer(data);

    return kExitSuccess;
}

/* KW_SUPPRESS_START:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False Positive */
std::int32_t SocketAsync::WriteAsync(std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> data,
                                     AsyncCallback u_cb) noexcept
/* KW_SUPPRESS_END:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False Positive */
{
    /* KW_SUPPRESS_START:MISRA.LOGIC.POSTFIX: False Positive */
    if (data->empty() || !data->at(0).size())
    /* KW_SUPPRESS_END:MISRA.LOGIC.POSTFIX: False Positive */
    {
        score::mw::log::LogError(kLogContext) << "Incorrect buffer provided";
        return kExitNumOfSocketsExceeded;
    }
    this->SetWriteCb(std::move(u_cb));
    this->SetWriteBuffer(data);

    return kExitSuccess;
}

void SocketAsync::Read(std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> messages, AsyncCallback u_cb)
{
    AsyncCallback u_cb_ = std::move(u_cb);
    score::cpp::expected<ssize_t, Error> ret;
    const std::size_t msg_count = messages->size();

    if (msg_count == 1)
    {
        struct iovec iov;
        iov.iov_base = messages->at(0).data();
        iov.iov_len = static_cast<size_t>(messages->at(0).size());

        struct msghdr msg;
        memset(&msg, 0, sizeof(msg));
        if (this->GetEndpoint().IsAnyAddress())
        {
            msg.msg_name = nullptr;
            msg.msg_namelen = 0;
        }
        else
        {
            struct sockaddr_in server_addr = this->GetEndpoint().ToSockaddr();
            msg.msg_name = &server_addr;
            msg.msg_namelen = sizeof(server_addr);
        }
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        ret = score::os::Socket::instance().recvmsg(socket_fd_, &msg, Socket::MessageFlag::kNone);
    }
    else
    {
        std::vector<struct mmsghdr> msgs(msg_count);
        std::vector<struct iovec> iovs(msg_count);

        for (size_t i = 0; i < msg_count; ++i)
        {
            iovs[i].iov_base = messages->at(i).data();
            iovs[i].iov_len = static_cast<size_t>(messages->at(i).size());

            memset(&msgs[i], 0, sizeof(msgs[i]));
            if (this->GetEndpoint().IsAnyAddress())
            {
                msgs[i].msg_hdr.msg_name = nullptr;
                msgs[i].msg_hdr.msg_namelen = 0;
            }
            else
            {
                struct sockaddr_in server_addr = this->GetEndpoint().ToSockaddr();
                msgs[i].msg_hdr.msg_name = &server_addr;
                msgs[i].msg_hdr.msg_namelen = sizeof(server_addr);
            }
            msgs[i].msg_hdr.msg_iov = &iovs[i];
            msgs[i].msg_hdr.msg_iovlen = 1;
        }

        ret = score::os::Socket::instance().recvmmsg(
            socket_fd_, msgs.data(), static_cast<std::uint32_t>(msg_count), Socket::MessageFlag::kNone, nullptr);
    }
    if ((ret.has_value() == false) || (ret.value() < 0))
    {
        score::mw::log::LogError(kLogContext) << "Failed to read data";
    }

    u_cb_(std::move(messages), ret.has_value() ? ret.value() : -1);
    read_in_progress = false;
}

void SocketAsync::Write(std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> messages, AsyncCallback u_cb)
{
    AsyncCallback u_cb_ = std::move(u_cb);
    score::cpp::expected<ssize_t, Error> ret;
    const std::size_t msg_count = messages->size();

    if (msg_count == 1)
    {
        struct iovec iov;
        iov.iov_base = messages->at(0).data();
        iov.iov_len = static_cast<size_t>(messages->at(0).size());

        struct msghdr msg;
        memset(&msg, 0, sizeof(msg));
        if (this->GetEndpoint().IsAnyAddress())
        {
            msg.msg_name = nullptr;
            msg.msg_namelen = 0;
        }
        else
        {
            struct sockaddr_in server_addr = this->GetEndpoint().ToSockaddr();
            msg.msg_name = &server_addr;
            msg.msg_namelen = sizeof(server_addr);
        }
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        ret = score::os::Socket::instance().sendmsg(socket_fd_, &msg, Socket::MessageFlag::kNone);
    }
    if ((ret.has_value() == false) || (ret.value() < 0))
    {
        score::mw::log::LogError(kLogContext) << "Failed to write data";
    }
    u_cb_(std::move(messages), ret.has_value() ? ret.value() : -1);
    write_in_progress = false;
}

bool SocketAsync::GetReadStatus() const noexcept
{
    return read_in_progress;
}

void SocketAsync::SetReadStatus(const bool value) noexcept
{
    read_in_progress = value;
}

bool SocketAsync::GetWriteStatus() const noexcept
{
    return write_in_progress;
}

void SocketAsync::SetWriteStatus(const bool value) noexcept
{
    write_in_progress = value;
}

std::int32_t SocketAsync::ConnectAsync(AsyncConnectCallback u_cb) noexcept
{
    this->SetConnectCb(std::move(u_cb));

    return kExitSuccess;
}

void SocketAsync::Connect(AsyncConnectCallback u_cb)
{
    const AsyncConnectCallback cb = std::move(u_cb);

    sockaddr_in sock_addr = this->GetEndpoint().ToSockaddr();
    /* KW_SUPPRESS_START:AUTOSAR.CAST.REINTERPRET:Needed for cast to sockaddr */
    const auto recipient_sockaddr = reinterpret_cast<const struct sockaddr*>(&sock_addr);
    /* KW_SUPPRESS_END:AUTOSAR.CAST.REINTERPRET:*/

    const auto ret = score::os::Socket::instance().connect(socket_fd_, recipient_sockaddr, sizeof(recipient_sockaddr));

    if (!ret.has_value())
    {
        score::mw::log::LogError(kLogContext) << "Failed to connect";
        cb(kExitFailure);
        return;
    }
    cb(kExitSuccess);
    write_in_progress = false;
}

}  // namespace os
}  // namespace score
