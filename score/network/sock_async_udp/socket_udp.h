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
#ifndef SCORE_LIB_NETWORK_UDP_SOCKET_H
#define SCORE_LIB_NETWORK_UDP_SOCKET_H

#include "score/network/sock_async/sock_async.h"
#include "score/network/sock_async/sock_ctrl.h"

namespace score
{
namespace os
{
class SocketCtrl;
class SocketUdp final : public SocketAsync, public std::enable_shared_from_this<SocketUdp>
{
  public:
    SocketUdp(const std::shared_ptr<SocketCtrl> sock_ctrl, const Endpoint endpoint);
    std::int32_t GetSockFD() const noexcept override;
    ~SocketUdp();
    std::int32_t ReadAsync(std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> data,
                           AsyncCallback u_cb) noexcept override;
    std::int32_t WriteAsync(std::shared_ptr<std::vector<score::cpp::span<uint8_t>>> data,
                            AsyncCallback u_cb) noexcept override;
    std::int32_t ConnectAsync(AsyncConnectCallback cb) noexcept override;

  private:
    std::shared_ptr<SocketCtrl> sock_ctrl_;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_NETWORK_UDP_SOCKET_H
