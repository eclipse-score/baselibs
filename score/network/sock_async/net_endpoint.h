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
///
/// @file net_endpoint.h
/// @copyright Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief NetEndpoint class incapsulates IP address for socket
///

#ifndef SCORE_LIB_NETWORK_NET_ENDPOINT_H
#define SCORE_LIB_NETWORK_NET_ENDPOINT_H

#include "score/network/ipv4_address.h"
#include <netinet/in.h>

namespace score
{
namespace os
{
class NetEndpoint
{
  public:
    NetEndpoint() noexcept;
    explicit NetEndpoint(const Ipv4Address& address, const std::uint16_t port) noexcept;

    static Ipv4Address BroadcastIp();
    static Ipv4Address AnyAddress();
    static std::uint16_t AnyPort();

    std::string ToString() const noexcept;
    sockaddr_in ToSockaddr() const noexcept;
    Ipv4Address getIpv4Address() const noexcept;
    bool IsAnyAddress() const noexcept;

  private:
    Ipv4Address ip_addr_;
    std::uint16_t port_;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_NETWORK_NET_ENDPOINT_H
