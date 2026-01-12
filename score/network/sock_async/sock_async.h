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
#ifndef SCORE_LIB_OS_NETWORK_SOCKET_ASYNC_H
#define SCORE_LIB_OS_NETWORK_SOCKET_ASYNC_H

#include <score/span.hpp>

#include "score/concurrency/thread_pool.h"
#include "score/network/sock_async/net_endpoint.h"
#include "score/network/sock_async/socket.h"
#include <score/callback.hpp>

namespace score
{
namespace os
{
using AsyncCallback = score::cpp::callback<void(std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>>, ssize_t)>;

using Endpoint = score::os::NetEndpoint;
constexpr const std::int32_t kExitSuccess{0};
constexpr const std::int32_t kExitFailure{-1};
constexpr const std::int32_t kExitNumOfSocketsExceeded{-2};
constexpr const std::int32_t kExitIncorrectDataBuffer{-3};
constexpr const std::int32_t kExitNotSupported{-4};
class SocketAsync : public SocketBase
{
  public:
    explicit SocketAsync(const Endpoint endpoint) noexcept;

    SocketAsync(SocketAsync&&) noexcept = delete;
    SocketAsync(const SocketAsync&) = delete;
    SocketAsync& operator=(SocketAsync&&) & noexcept = delete;
    SocketAsync& operator=(const SocketAsync&) & noexcept = delete;

    /// @brief Asynchronously reads data into a buffer and invokes a callback upon completion.
    ///
    /// @param data A mutable span representing the buffer where the read data will be stored.
    /// @param u_cb The callback function to be invoked upon completion of the read operation.
    ///
    /// @return An integer representing the status of the read operation.
    std::int32_t ReadAsync(std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> data,
                           AsyncCallback u_cb) noexcept override;

    /// @brief Asynchronously writes data from a buffer and invokes a callback upon completion.
    ///
    /// @param data A span representing the buffer containing the data to be written.
    /// @param u_cb The callback function to be invoked upon completion of the write operation.
    ///
    /// @return An integer representing the status of the write operation.
    std::int32_t WriteAsync(std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> data,
                            AsyncCallback u_cb) noexcept override;

    /// @brief Reads a data into a buffer and invokes a previously stored callback upon completion.
    ///
    /// @param messages A mutable span representing the buffer where the read data will be stored.
    /// @param u_cb The callback function to be invoked upon completion of the read operation.
    ///
    void Read(std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> messages, AsyncCallback u_cb);

    /// @brief Writes data from a buffer and invokes a previously stored callback upon completion.
    ///
    /// @param messages A span representing the buffer containing the data to be written.
    /// @param u_cb The callback function to be invoked upon completion of the write operation.
    ///
    void Write(std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> messages, AsyncCallback u_cb);

    bool GetReadStatus() const noexcept;
    void SetReadStatus(const bool value) noexcept;
    bool GetWriteStatus() const noexcept;
    void SetWriteStatus(const bool value) noexcept;

    /// @brief Asynchronously connects socket and invokes a previously stored callback upon completion.
    ///
    /// @param u_cb The callback function to be invoked upon completion of the connect operation.
    ///
    std::int32_t ConnectAsync(AsyncConnectCallback u_cb) noexcept override;

    /// @brief Connects socket and invokes a previously stored callback upon completion.
    ///
    /// @param u_cb The callback function to be invoked upon completion of the connect operation.
    ///
    void Connect(AsyncConnectCallback u_cb);

  private:
    bool read_in_progress;
    bool write_in_progress;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_NETWORK_SOCKET_ASYNC_H
