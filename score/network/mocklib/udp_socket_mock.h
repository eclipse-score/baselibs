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
#ifndef SCORE_LIB_OS_UTILS_NETWORK_MOCKLIB_UDP_SOCKET_MOCK_H
#define SCORE_LIB_OS_UTILS_NETWORK_MOCKLIB_UDP_SOCKET_MOCK_H

#include "score/network/udp_socket.h"

#include <gmock/gmock.h>

namespace score
{
namespace os
{

class UdpSocketMock : public UdpSocket
{
  public:
    static constexpr auto UDP_SOCKET_MOCK_FILE_DESCRIPTOR = 42U;
    UdpSocketMock() : UdpSocket{UDP_SOCKET_MOCK_FILE_DESCRIPTOR} {};

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>), Bind, (const Ipv4Address&, std::uint16_t), (noexcept, override));

    MOCK_METHOD((score::cpp::expected<ssize_t, Error>), TryReceive, (unsigned char*, std::size_t), (noexcept, override));

    MOCK_METHOD((score::cpp::expected<std::tuple<ssize_t, score::os::Ipv4Address>, score::os::Error>),
                TryReceiveWithAddress,
                (unsigned char*, std::size_t),
                (noexcept, override));
    MOCK_METHOD((score::cpp::expected<score::cpp::pmr::vector<std::tuple<ssize_t, score::os::Ipv4Address>>, score::os::Error>),
                TryReceiveMultipleMessagesWithAddress,
                (unsigned char* const recv_bufs,
                 const std::size_t recv_buffer_size,
                 const std::size_t vec_length,
                 const std::size_t msg_length),
                (noexcept, override));

    MOCK_METHOD((score::cpp::expected<ssize_t, Error>),
                TrySendTo,
                (const Ipv4Address&, const std::uint16_t, const unsigned char*, std::size_t),
                (noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                SetSocketOption,
                (const std::int32_t, const std::int32_t, const void*, const socklen_t),
                (noexcept, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_NETWORK_MOCKLIB_UDP_SOCKET_MOCK_H
