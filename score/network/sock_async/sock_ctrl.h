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
#ifndef SCORE_LIB_NETWORK_NET_SOCK_CTRL_H
#define SCORE_LIB_NETWORK_NET_SOCK_CTRL_H

#include "score/concurrency/thread_pool.h"
#include "score/network/i_socket.h"
#include "score/network/sock_async/sock_async.h"
#include "score/os/socket.h"
#include "score/os/sys_poll.h"

#include "score/mw/log/logging.h"

#include <condition_variable>
#include <mutex>
namespace score
{
namespace os
{
struct CtrlMsg
{
    enum class OprType : std::uint32_t
    {
        NONE = 0,
        DEL_OPR = 1,
        ADD_OPR = 2,
        STOP_OPR = 3
    };

    CtrlMsg(const OprType type, const std::int32_t socket_fd) : type_(type), socket_fd_(socket_fd) {}

    CtrlMsg() : type_(OprType::NONE), socket_fd_(0) {}

    OprType type_;
    std::int32_t socket_fd_;
};

enum class SockReq : std::uint8_t
{
    READ = 0,
    WRITE = 1,
    CONNECT = 2,
    DELETE = 3
};

class SocketCtrl final
{
  public:
    SocketCtrl() noexcept;

    SocketCtrl(SocketCtrl&&) noexcept = delete;
    SocketCtrl(const SocketCtrl&) = delete;
    SocketCtrl& operator=(SocketCtrl&&) & noexcept = delete;
    SocketCtrl& operator=(const SocketCtrl&) & noexcept = delete;
    ~SocketCtrl();
    std::int32_t RequestOperation(std::shared_ptr<SocketAsync> sock, const SockReq sock_req) noexcept;
    void StopPoll(const CtrlMsg ctrl_msg);
    std::atomic_bool closeCtrl_;

  protected:
  private:
    void HandlePoll(const score::cpp::stop_token token);
    void RemoveSocket(std::int32_t socket_fd);
    concurrency::ThreadPool read_pool_;
    concurrency::ThreadPool write_pool_;
    std::array<std::int32_t, 2> ctrl_sockets_;
    std::vector<struct pollfd> fds_;
    std::uint32_t monitored_sockets_num_;
    std::vector<std::shared_ptr<SocketAsync>> socket_list_;
    std::condition_variable cv_;
    std::mutex mtx_;
};
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_NETWORK_NET_SOCK_CTRL_H
