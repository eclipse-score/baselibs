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
#include "score/network/sock_async_udp/socket_udp.h"
#include "score/network/sock_async/net_endpoint.h"
#include "score/network/sock_async/sock_factory.h"
#include "score/os/mocklib/socketmock.h"
#include "score/os/mocklib/sys_poll_mock.h"
#include <score/callback.hpp>
#include <score/expected.hpp>
#include <atomic>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>
namespace score
{
namespace os
{

namespace
{

using ::testing::_;
using namespace testing;
using Endpoint = score::os::NetEndpoint;

constexpr const std::int32_t kSocketFD{1234};

struct SocketUDPTest : ::testing::Test
{
    void CallbackFn(std::int16_t ret)
    {
        stored_result_.store(ret);
        counter.fetch_add(1);
    }

    void SetUp() override
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        score::os::Socket::set_testing_instance(sock_mock_);
        score::os::SysPoll::set_testing_instance(sysPollMock);
        stored_result_ = 0;
        counter.store(0);
    }
    void TearDown() override
    {
        delete factory;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    score::os::SocketMock sock_mock_;
    score::os::SysPollMock sysPollMock;
    std::atomic<std::int32_t> stored_result_;
    std::atomic<std::int32_t> counter;
    SocketFactory* factory;
};

TEST_F(SocketUDPTest, CreationSuccess)
{
    RecordProperty("Verifies", "SCR-21202526");
    RecordProperty("ASIL", "B");
    RecordProperty("Priority", "3");
    RecordProperty("Description", "Verifies that creation of UDP async socket is successful");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    EXPECT_CALL(sock_mock_, socket(_, _, _)).Times(Exactly(1)).WillOnce(Return(kSocketFD));
    EXPECT_CALL(sysPollMock, poll(_, _, _)).WillRepeatedly([](struct pollfd* in_pollfd, nfds_t, int) {
        in_pollfd[0].revents = POLLIN;
        return 1;
    });
    factory = new SocketFactory();
    std::shared_ptr<SocketAsync> socket_udp = factory->CreateSocket(SockType::UDP, Endpoint{});

    ASSERT_EQ(socket_udp->GetSockFD(), kSocketFD);
}

TEST_F(SocketUDPTest, CreationFailed)
{
    RecordProperty("Verifies", "SCR-21202526");
    RecordProperty("ASIL", "B");
    RecordProperty("Priority", "3");
    RecordProperty("Description", "Verifies that creation of UDP async socket fails");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    EXPECT_CALL(sock_mock_, socket(_, _, _))
        .Times(Exactly(1))
        .WillOnce(Return(score::cpp::make_unexpected(Error::createFromErrno(0))));
    EXPECT_CALL(sysPollMock, poll(_, _, _)).WillRepeatedly([](struct pollfd* in_pollfd, nfds_t, int) {
        in_pollfd[0].revents = POLLIN;
        return 1;
    });
    factory = new SocketFactory();
    std::shared_ptr<SocketAsync> socket_udp = factory->CreateSocket(SockType::UDP, Endpoint{});

    ASSERT_EQ(socket_udp->GetSockFD(), kExitFailure);
}

TEST_F(SocketUDPTest, ConnectFailed)
{
    RecordProperty("Verifies", "SCR-21202526");
    RecordProperty("ASIL", "B");
    RecordProperty("Priority", "3");
    RecordProperty("Description", "Verifies that connection on UDP async socket fails");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    EXPECT_CALL(sock_mock_, socket(_, _, _)).Times(Exactly(1)).WillOnce(Return(kSocketFD));
    EXPECT_CALL(sysPollMock, poll(_, _, _)).WillRepeatedly([](struct pollfd* in_pollfd, nfds_t, int) {
        in_pollfd[0].revents = POLLIN;
        return 1;
    });
    factory = new SocketFactory();
    std::shared_ptr<SocketAsync> socket_async = factory->CreateSocket(SockType::UDP, Endpoint{});

    auto lambda = [&](std::int16_t ret) noexcept {
        CallbackFn(ret);
    };
    auto result = socket_async->ConnectAsync(std::move(lambda));

    ASSERT_EQ(result, kExitNotSupported);
}

}  // namespace

}  // namespace os
}  // namespace score
