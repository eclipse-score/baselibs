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
#ifndef SCORE_LIB_OS_UTILS_NETWORK_UDP_SOCKET_H
#define SCORE_LIB_OS_UTILS_NETWORK_UDP_SOCKET_H

#include "score/network/ipv4_address.h"
#include "score/os/errno.h"
#include "score/os/socket.h"

#include <score/expected.hpp>
#include <score/vector.hpp>

#include <arpa/inet.h>

#include <cstdint>
#include <iterator>

namespace score
{
namespace os
{

score::cpp::expected<sockaddr_in, score::os::Error> GetSockAddrInFromIpAndPort(const score::os::Ipv4Address& address,
                                                                      const std::uint16_t port);

class Ipv4Address;

class UdpSocket
{
  protected:
    std::int32_t file_descriptor_{};

    explicit UdpSocket(std::int32_t file_descriptor) noexcept;

  public:
    UdpSocket(const UdpSocket& other) = delete;
    UdpSocket& operator=(const UdpSocket& other) = delete;

    UdpSocket(UdpSocket&& other) noexcept;
    UdpSocket& operator=(UdpSocket&& other) noexcept;

    virtual ~UdpSocket() noexcept;

    static score::cpp::expected<UdpSocket, score::os::Error> Make() noexcept;

    virtual score::cpp::expected_blank<score::os::Error> Bind(const Ipv4Address& address, std::uint16_t port) noexcept;

    virtual score::cpp::expected<ssize_t, Error> TryReceive(unsigned char* const buffer, const std::size_t length) noexcept;

    virtual score::cpp::expected<std::tuple<ssize_t, score::os::Ipv4Address>, Error> TryReceiveWithAddress(
        unsigned char* const buffer,
        const std::size_t length) noexcept;

    virtual score::cpp::expected<score::cpp::pmr::vector<std::tuple<ssize_t, score::os::Ipv4Address>>, score::os::Error>
    TryReceiveMultipleMessagesWithAddress(unsigned char* const recv_bufs,
                                          const std::size_t recv_buffer_size,
                                          const std::size_t vlen,
                                          const std::size_t msg_length) noexcept;

    virtual score::cpp::expected<ssize_t, Error> TrySendTo(const Ipv4Address& recipient,
                                                    const std::uint16_t port,
                                                    const unsigned char* const buffer,
                                                    const std::size_t length) noexcept;

    virtual score::cpp::expected_blank<score::os::Error> SetSocketOption(const std::int32_t level,
                                                                const std::int32_t optname,
                                                                const void* optval,
                                                                const socklen_t optlen) noexcept;

    std::int32_t GetFileDescriptor() const noexcept;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_NETWORK_UDP_SOCKET_H
