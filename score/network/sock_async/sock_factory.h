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
#ifndef SCORE_LIB_NETWORK_NET_SOCK_FACTORY_H
#define SCORE_LIB_NETWORK_NET_SOCK_FACTORY_H

#include "score/network/sock_async/sock_ctrl.h"
#include "score/network/sock_async_raw/socket_raw.h"
#include "score/network/sock_async_tcp/socket_tcp.h"
#include "score/network/sock_async_udp/socket_udp.h"

namespace score
{
namespace os
{

enum class SockType : std::uint8_t
{
    UDP = 0,
    RAW,
    TCP
};

class SocketFactory final
{
  public:
    SocketFactory() noexcept;

    SocketFactory(SocketFactory&&) noexcept = delete;
    SocketFactory(const SocketFactory&) = delete;
    SocketFactory& operator=(SocketFactory&&) & noexcept = delete;
    SocketFactory& operator=(const SocketFactory&) & noexcept = delete;
    ~SocketFactory();

    std::shared_ptr<SocketAsync> CreateSocket(score::os::SockType sock_type,
                                              Endpoint endp,
                                              std::int32_t protocol = 0) noexcept;

  private:
    std::shared_ptr<SocketCtrl> sock_ctrl_;
};
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_NETWORK_NET_SOCK_FACTORY_H
