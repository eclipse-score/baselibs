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
#include "score/network/sock_async/socket.h"
#include "score/os/socket.h"

#include "score/mw/log/logging.h"

namespace score
{
namespace os
{

namespace
{
constexpr const char* kLogContext{"soc"}; /* SocketBase */
/* KW_SUPPRESS_START:MISRA.VAR.NEEDS.CONST: False Positive */
/* KW_SUPPRESS_START:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False Positive */
constexpr const std::int32_t INVALID_SOCKET_ID = -1;
/* KW_SUPPRESS_END:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False Positive */
/* KW_SUPPRESS_END:MISRA.VAR.NEEDS.CONST: False Positive */
}  // namespace

SocketBase::SocketBase(const Endpoint endpoint) noexcept
    : socket_fd_(INVALID_SOCKET_ID), endpoint_(endpoint), is_bound_(false), option_variant_()
{
}

void SocketBase::SetOption(const SockOptionVariant optionVariant) noexcept
{
    if (score::cpp::holds_alternative<ReuseAddrOption>(optionVariant))
    {
        option_variant_ = optionVariant;
        const auto option_response = score::cpp::get<ReuseAddrOption>(option_variant_).apply(socket_fd_);

        if (!option_response.has_value())
        {
            mw::log::LogInfo(kLogContext) << "Failed to set option";
        }
    }
    else if (score::cpp::holds_alternative<TimeoutOption>(optionVariant))
    {
        option_variant_ = optionVariant;
        const auto option_response = score::cpp::get<TimeoutOption>(option_variant_).apply(socket_fd_);

        if (!option_response.has_value())
        {
            mw::log::LogInfo(kLogContext) << "Failed to set option";
        }
    }
    else if (score::cpp::holds_alternative<BufferSizeOption>(optionVariant))
    {
        option_variant_ = optionVariant;
        const auto option_response = score::cpp::get<BufferSizeOption>(option_variant_).apply(socket_fd_);

        if (!option_response.has_value())
        {
            mw::log::LogInfo(kLogContext) << "Failed to set option";
        }
    }
    else
    {
        mw::log::LogInfo(kLogContext) << "Failed option variant not valid";
    }
}
/* KW_SUPPRESS_START:MISRA.LINKAGE.EXTERN:False positive. */
score::cpp::expected<ssize_t, score::os::Error> SocketBase::WriteSync(
    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> buffer) const noexcept
{
    sockaddr_in sock_addr = endpoint_.ToSockaddr();
    /* KW_SUPPRESS_START:AUTOSAR.CAST.REINTERPRET:Needed for cast to sockaddr */
    const auto recipient_sockaddr = reinterpret_cast<const struct sockaddr*>(&sock_addr);
    /* KW_SUPPRESS_END:AUTOSAR.CAST.REINTERPRET:*/

    const void* const bufferPtr = static_cast<const void*>(buffer->data());
    return score::os::Socket::instance().sendto(socket_fd_,
                                              bufferPtr,
                                              buffer->size(),
                                              Socket::MessageFlag::kNone,
                                              recipient_sockaddr,
                                              sizeof(*recipient_sockaddr));
}
/* KW_SUPPRESS_END:MISRA.LINKAGE.EXTERN:False positive. */

/* KW_SUPPRESS_START:MISRA.LINKAGE.EXTERN:False positive. */
score::cpp::variant<ssize_t, std::tuple<ssize_t, score::os::Ipv4Address>, score::os::Error> SocketBase::ReadSync(
    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> buffer) noexcept
{
    if (endpoint_.getIpv4Address() == endpoint_.AnyAddress())
    {
        score::cpp::expected<ssize_t, score::os::Error> ret = score::os::Socket::instance().recvfrom(
            socket_fd_, buffer->data(), buffer->size(), Socket::MessageFlag::kNone, nullptr, nullptr);
        if (ret.has_value())
        {
            return ret.value();
        }
        else
        {
            return Error::createFromErrno();
        }
    }

    sockaddr_in sock_addr = endpoint_.ToSockaddr();
    /* KW_SUPPRESS_START:AUTOSAR.CAST.REINTERPRET:Needed for cast to sockaddr */
    const auto source_address = reinterpret_cast<struct sockaddr*>(&sock_addr);
    /* KW_SUPPRESS_END:AUTOSAR.CAST.REINTERPRET:*/
    socklen_t address_length = sizeof(source_address);

    const auto result = score::os::Socket::instance().recvfrom(
        socket_fd_, buffer->data(), buffer->size(), Socket::MessageFlag::kNone, source_address, &address_length);

    if (result.has_value())
    {
        const ssize_t numBytes = *result;
        const Ipv4Address senderAddress = Ipv4Address::CreateFromUint32NetOrder(endpoint_.ToSockaddr().sin_addr.s_addr);
        return std::make_tuple(numBytes, senderAddress);
    }

    return Error::createFromErrno();
}
/* KW_SUPPRESS_END:MISRA.LINKAGE.EXTERN:False positive. */

void SocketBase::Bind(const Endpoint endpoint) noexcept
{
    sockaddr_in sock_addr = endpoint.ToSockaddr();
    /* KW_SUPPRESS_START:AUTOSAR.CAST.REINTERPRET:Needed for cast to sockaddr */
    const auto recipient_sockaddr = reinterpret_cast<const struct sockaddr*>(&sock_addr);
    /* KW_SUPPRESS_END:AUTOSAR.CAST.REINTERPRET:*/

    const auto bind_ret = score::os::Socket::instance().bind(socket_fd_, recipient_sockaddr, sizeof(*recipient_sockaddr));

    if (!bind_ret.has_value())
    {
        mw::log::LogInfo(kLogContext) << "bind failed";
        is_bound_ = false;
    }
    else
    {
        is_bound_ = true;
    }
}

bool SocketBase::IsBound() const noexcept
{
    return is_bound_;
}

SockOptionVariant SocketBase::GetOptionVariant() const noexcept
{
    return option_variant_;
}

Endpoint SocketBase::GetEndpoint() const noexcept
{
    return endpoint_;
}

std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> SocketBase::GetReadBuffer() noexcept
{
    return std::move(read_buffer_);
}

void SocketBase::SetReadBuffer(std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> buf) noexcept
{
    read_buffer_ = std::move(buf);
}

AsyncCallback SocketBase::GetReadCb() noexcept
{
    return std::move(read_cb_);
}

void SocketBase::SetReadCb(AsyncCallback cb) noexcept
{
    read_cb_ = std::move(cb);
}

std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> SocketBase::GetWriteBuffer() noexcept
{
    return std::move(write_buffer_);
}

void SocketBase::SetWriteBuffer(std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> buf) noexcept
{
    write_buffer_ = std::move(buf);
}

AsyncCallback SocketBase::GetWriteCb() noexcept
{
    return std::move(write_cb_);
}

void SocketBase::SetWriteCb(AsyncCallback cb) noexcept
{
    write_cb_ = std::move(cb);
}

AsyncConnectCallback SocketBase::GetConnectCb() noexcept
{
    return std::move(connect_cb_);
}

void SocketBase::SetConnectCb(AsyncConnectCallback cb) noexcept
{
    connect_cb_ = std::move(cb);
}

}  // namespace os
}  // namespace score
