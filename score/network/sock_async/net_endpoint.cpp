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

#include "score/network/sock_async/net_endpoint.h"

#include <sys/socket.h>
#include <cstring>

namespace score
{
namespace os
{

/* KW_SUPPRESS_START:MISRA.USE.EXPANSION:This is the way gcc/qcc recommend to figure out endianness */
/* KW_SUPPRESS_START:MISRA.LINKAGE.EXTERN,MISRA.NS.GLOBAL: false positive */
#if defined(__BYTE_ORDER__)
constexpr bool kIsLittleEndian{__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__};
#else  // defined(__BYTE_ORDER__)
#error __BYTE_ORDER__ is not defined
#endif  // defined(__BYTE_ORDER__)
/* KW_SUPPRESS_END:MISRA.LINKAGE.EXTERN,MISRA.NS.GLOBAL */
/* KW_SUPPRESS_END:MISRA.USE.EXPANSION */

NetEndpoint::NetEndpoint() noexcept : ip_addr_(AnyAddress()), port_(AnyPort()) {}

NetEndpoint::NetEndpoint(const Ipv4Address& address, const std::uint16_t port) noexcept : ip_addr_(address), port_(port)
{
}

Ipv4Address NetEndpoint::BroadcastIp()
{
    return Ipv4Address::Broadcast();
}

Ipv4Address NetEndpoint::AnyAddress()
{
    return Ipv4Address(0, 0, 0, 0);
}

std::uint16_t NetEndpoint::AnyPort()
{
    return 0;
}

bool NetEndpoint::IsAnyAddress() const noexcept
{
    return ip_addr_ == Ipv4Address(0, 0, 0, 0);
}
std::string NetEndpoint::ToString() const noexcept
{
    return ip_addr_.ToString() + ":" + std::to_string(static_cast<std::uint32_t>(port_));
}

sockaddr_in NetEndpoint::ToSockaddr() const noexcept
{
    sockaddr_in sock_addr{};

    sock_addr.sin_family = AF_INET;
    Ipv4Address::AddressBytes ip_array = ip_addr_.ToIpv4Bytes();
    if (kIsLittleEndian)
    {

        std::memcpy(&sock_addr.sin_addr.s_addr, ip_array.data(), sizeof(sock_addr.sin_addr.s_addr));
    }
    else
    {
        // Convert from Big-Endian to Little-Endian
        sock_addr.sin_addr.s_addr =
            (static_cast<std::uint32_t>(ip_array[0]) << 24U) | (static_cast<std::uint32_t>(ip_array[1]) << 16U) |
            (static_cast<std::uint32_t>(ip_array[2]) << 8U) | (static_cast<std::uint32_t>(ip_array[3]));
    }
    sock_addr.sin_port = htons(port_);

    return sock_addr;
}

Ipv4Address NetEndpoint::getIpv4Address() const noexcept
{
    return ip_addr_;
}
}  // namespace os
}  // namespace score
