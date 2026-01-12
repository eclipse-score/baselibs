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
#include "score/network/sock_async/sock_factory.h"

namespace score
{
namespace os
{
SocketFactory::SocketFactory() noexcept : sock_ctrl_(std::make_shared<SocketCtrl>()) {}

SocketFactory::~SocketFactory()
{
    CtrlMsg ctrl_msg = CtrlMsg(CtrlMsg::OprType::STOP_OPR, 0);
    sock_ctrl_->StopPoll(ctrl_msg);
}

std::shared_ptr<SocketAsync> SocketFactory::CreateSocket(SockType sock_type,
                                                         Endpoint endp,
                                                         std::int32_t protocol) noexcept
{
    switch (sock_type)
    {
        case SockType::UDP:
            return std::make_shared<score::os::SocketUdp>(sock_ctrl_, endp);
        case SockType::RAW:
            return std::make_shared<score::os::SocketRaw>(sock_ctrl_, endp, protocol);
        case SockType::TCP:
            return std::make_shared<score::os::SocketTcp>(sock_ctrl_, endp);  // TODO Ticket-111529
        default:
            return std::make_shared<score::os::SocketUdp>(sock_ctrl_, endp);
    }
    return std::make_shared<score::os::SocketUdp>(sock_ctrl_, endp);
}

}  // namespace os
}  // namespace score
