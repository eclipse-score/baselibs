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
#include "score/network/sock_async/sock_ctrl.h"

namespace score
{
namespace os
{

namespace
{
constexpr const char* kLogContext{"sock_async_mgr"};
/* Maximum number of sockets to monitor*/
constexpr const std::int32_t MAX_SOCKETS = 20;
/* control sockets for data reading*/
constexpr const std::int32_t CTRL_R_SOCK = 0;
/* control sockets for data writing*/
constexpr const std::int32_t CTRL_W_SOCK = 1;
constexpr const std::uint8_t kExecMaxTime = 2;

}  // namespace

SocketCtrl::SocketCtrl() noexcept : closeCtrl_{false}, read_pool_{1}, write_pool_{1}
{
    monitored_sockets_num_ = 0;
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, ctrl_sockets_.data()) < 0)
    {
        mw::log::LogError(kLogContext) << "Socketpair create error";
    }
    struct pollfd fds;
    fds.fd = ctrl_sockets_[CTRL_R_SOCK];
    fds.events = POLLIN;
    fds.revents = 0;
    fds_.push_back(fds);

    read_pool_.Post([this](const score::cpp::stop_token& token) mutable {
        this->HandlePoll(token);
    });
}

SocketCtrl::~SocketCtrl()
{
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait_for(lock, std::chrono::seconds(kExecMaxTime), [this]() -> bool {
        return (this->closeCtrl_.load() == true);
    });
    close(ctrl_sockets_[CTRL_R_SOCK]);
    close(ctrl_sockets_[CTRL_W_SOCK]);
}

std::int32_t SocketCtrl::RequestOperation(std::shared_ptr<SocketAsync> sock, const SockReq sock_req) noexcept
{
    CtrlMsg ctrl_msg;
    if (!sock)
    {
        return kExitFailure;
    }
    switch (sock_req)
    {
        case SockReq::READ:
            if (monitored_sockets_num_ >= MAX_SOCKETS)
            {
                mw::log::LogError(kLogContext) << "Supported sockets number exceeded";
                return kExitNumOfSocketsExceeded;
            }
            socket_list_.push_back(sock);
            ctrl_msg = CtrlMsg(CtrlMsg::OprType::ADD_OPR, sock->GetSockFD());
            StopPoll(ctrl_msg);
            break;
        case SockReq::WRITE:
            /* KW_SUPPRESS_START:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False Positive */
            write_pool_.Post([&, sock = std::move(sock)](const score::cpp::stop_token&) mutable {
                sock->Write(sock->GetWriteBuffer(), sock->GetWriteCb());
            });
            /* KW_SUPPRESS_END:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False Positive */
            break;
        case SockReq::CONNECT:
            /* KW_SUPPRESS_START:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False Positive */
            write_pool_.Post([&, sock = std::move(sock)](const score::cpp::stop_token&) mutable {
                sock->Connect(sock->GetConnectCb());
            });
            /* KW_SUPPRESS_END:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False Positive */
            break;
        case SockReq::DELETE:
            if (monitored_sockets_num_)
            {
                ctrl_msg = CtrlMsg(CtrlMsg::OprType::DEL_OPR, sock->GetSockFD());
                StopPoll(ctrl_msg);
            }
            else
            {
                mw::log::LogInfo(kLogContext) << "Nothing to delete. Poll was not running";
            }
            break;
        default:
            return kExitFailure;
    }
    return kExitSuccess;
}

void SocketCtrl::StopPoll(const CtrlMsg ctrl_msg)
{
    if (!closeCtrl_.load())
    {
        const ssize_t ret = write(ctrl_sockets_[CTRL_W_SOCK], &ctrl_msg, sizeof(CtrlMsg));
        if (ret <= 0)
        {
            mw::log::LogError(kLogContext) << "Writing stream message to S2 to unblock failed";
        }
    }
}

void SocketCtrl::HandlePoll(const score::cpp::stop_token token)
{
    score::cpp::expected<ssize_t, Error> result;
    score::cpp::stop_callback callback(token, [this]() noexcept {
        if (!closeCtrl_.load())
        {
            CtrlMsg ctrl_msg;
            ctrl_msg = CtrlMsg(CtrlMsg::OprType::STOP_OPR, 0);
            StopPoll(ctrl_msg);
        }
    });

    while (true)
    {
        const auto ret = score::os::SysPoll::instance().poll(fds_.data(), monitored_sockets_num_ + 1, -1);

        if (!ret.has_value())
        {
            if (errno == EINTR)
            {
                mw::log::LogError(kLogContext) << "Poll interrupted";
                for (auto& socket : socket_list_)
                {
                    socket->GetReadCb()(socket->GetReadBuffer(), kExitFailure);
                    RemoveSocket(socket->GetSockFD());
                }
                return;
            }
        }

        if (fds_[CTRL_R_SOCK].revents & POLLIN)
        {
            fds_[CTRL_R_SOCK].revents = 0;
            CtrlMsg ctrl_msg;
            const ssize_t bytes_received1 = read(fds_[CTRL_R_SOCK].fd, &ctrl_msg, sizeof(CtrlMsg));
            if (bytes_received1 == -1)
            {
                mw::log::LogError(kLogContext) << "Data shall be available at ctrl socket!!!";
                return;
            }
            else
            {
                switch (ctrl_msg.type_)
                {
                    case CtrlMsg::OprType::ADD_OPR:
                        struct pollfd fds;
                        fds.fd = ctrl_msg.socket_fd_;
                        fds.events = POLLIN;
                        fds.revents = 0;
                        fds_.push_back(fds);
                        monitored_sockets_num_++;
                        break;
                    case CtrlMsg::OprType::DEL_OPR:
                        RemoveSocket(ctrl_msg.socket_fd_);
                        break;
                    case CtrlMsg::OprType::STOP_OPR:
                    {
                        std::lock_guard<std::mutex> lock(mtx_);
                        closeCtrl_.store(true);
                        cv_.notify_all();
                    }
                        return;
                    case CtrlMsg::OprType::NONE:
                    // Intentional fall-through
                    default:
                        mw::log::LogError(kLogContext) << "Unsupported operation!!!";
                        {
                            std::lock_guard<std::mutex> lock(mtx_);
                            closeCtrl_.store(true);
                        }
                        return;
                }
            }
        }

        std::uint32_t i = 1;
        while (i <= monitored_sockets_num_)
        {
            if (fds_[i].revents & POLLIN)
            {
                fds_[i].revents = 0;
                socket_list_[i - 1]->Read(socket_list_[i - 1]->GetReadBuffer(), socket_list_[i - 1]->GetReadCb());
                RemoveSocket(socket_list_[i - 1]->GetSockFD());
            }
            i++;
        }
    }
}

void SocketCtrl::RemoveSocket(std::int32_t socket_fd)
{
    const auto it_socket = std::find_if(
        socket_list_.begin(), socket_list_.end(), [socket_fd](const std::shared_ptr<SocketAsync>& socket) -> bool {
            return socket->GetSockFD() == socket_fd;
        });
    if (it_socket != socket_list_.end())
    {
        socket_list_.erase(it_socket);
    }

    const auto it_fds = std::find_if(fds_.begin() + 1, fds_.end(), [socket_fd](const struct pollfd pfd) -> bool {
        return pfd.fd == socket_fd;
    });
    if (it_fds != fds_.end())
    {
        fds_.erase(it_fds);
        monitored_sockets_num_--;
    }
}

}  // namespace os
}  // namespace score
