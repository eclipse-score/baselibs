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
#include "score/mw/log/logging.h"
#include "score/os/socket.h"
#include "score/os/utils/tcp_keep_alive.h"

bool score::os::TcpKeepAlive(int sockfd)
{
    // Variable to enable the periodic transmission of messages on a connected socket.
    int enable = 1;

    // Enabling the periodic transmission of messages on a connected socket.
    const auto setsockopt_ret =
        score::os::Socket::instance().setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable));
    if (!setsockopt_ret.has_value())
    {
        mw::log::LogError() << "Set SO_KEEPALIVE failed" << setsockopt_ret.error().ToString();
        return false;
    }

    // Variable to set the  maximum number of keepalive probes TCP should send
    // before dropping the connection.
    int keep_connect = 1;
    const auto setsockopt_ret2 =
        score::os::Socket::instance().setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT, &keep_connect, sizeof(int));
    if (!setsockopt_ret2.has_value())
    {
        mw::log::LogError() << "Set TCP_KEEPCNT failed" << setsockopt_ret2.error().ToString();
        return false;
    }

    // Variable to set the time (in seconds) the connection needs to remain idle
    // before TCP starts sending keepalive probes
    int keep_idle = 1;
    const auto setsockopt_ret3 =
        score::os::Socket::instance().setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &keep_idle, sizeof(int));
    if (!setsockopt_ret3.has_value())
    {
        mw::log::LogError() << "Set TCP_KEEPIDLE failed" << setsockopt_ret3.error().ToString();
        return false;
    }

    // Variable to set the time (in seconds) between individual keepalive probes.
    int keep_interval = 2;
    const auto setsockopt_ret4 =
        score::os::Socket::instance().setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &keep_interval, sizeof(int));
    if (!setsockopt_ret4.has_value())
    {
        mw::log::LogError() << "Set TCP_KEEPINTVL failed" << setsockopt_ret4.error().ToString();
        return false;
    }
    return true;
}
