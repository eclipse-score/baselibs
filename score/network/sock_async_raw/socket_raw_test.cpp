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
#include "score/network/sock_async_raw/socket_raw.h"
#include "score/network/sock_async/net_endpoint.h"
#include "score/network/sock_async/sock_factory.h"
#include "score/os/mocklib/socketmock.h"
#include <score/callback.hpp>
#include <score/expected.hpp>
#include <atomic>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
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
struct SocketRAWTest : ::testing::Test
{
    void CallbackFn(std::int16_t ret)
    {
        stored_result_.store(ret);
    }

    void SetUp() override
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        score::os::Socket::set_testing_instance(sock_mock_);
        stored_result_ = 0;
    }

    void TearDown() override
    {
        delete factory;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    score::os::SocketMock sock_mock_;
    std::atomic<std::int32_t> stored_result_;
    SocketFactory* factory;
};

TEST_F(SocketRAWTest, CreationSuccess)
{
    RecordProperty("Verifies", "SCR-21202526");
    RecordProperty("ASIL", "B");
    RecordProperty("Priority", "3");
    RecordProperty("Description", "Verifies that creation of RAW async socket is successful");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    EXPECT_CALL(sock_mock_, socket(_, _, _)).Times(Exactly(1)).WillOnce(Return(kSocketFD));
    factory = new SocketFactory();
    std::shared_ptr<SocketAsync> socket_raw = factory->CreateSocket(SockType::RAW, Endpoint{}, 1);
    ASSERT_EQ(socket_raw->GetSockFD(), kSocketFD);
}

TEST_F(SocketRAWTest, CreationFailed)
{
    RecordProperty("Verifies", "SCR-21202526");
    RecordProperty("ASIL", "B");
    RecordProperty("Priority", "3");
    RecordProperty("Description", "Verifies that creation of RAW async socket fails");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    EXPECT_CALL(sock_mock_, socket(_, _, _))
        .Times(Exactly(1))
        .WillOnce(Return(score::cpp::make_unexpected(Error::createFromErrno(93))));
    factory = new SocketFactory();
    std::shared_ptr<SocketAsync> socket_raw = factory->CreateSocket(SockType::RAW, Endpoint{}, 100);

    ASSERT_EQ(socket_raw->GetSockFD(), kExitFailure);
}

TEST_F(SocketRAWTest, ConnectFailed)
{
    RecordProperty("Verifies", "SCR-21202526");
    RecordProperty("ASIL", "B");
    RecordProperty("Priority", "3");
    RecordProperty("Description", "Verifies that connection on RAW async socket fails");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    EXPECT_CALL(sock_mock_, socket(_, _, _)).Times(Exactly(1)).WillOnce(Return(kSocketFD));
    factory = new SocketFactory();
    std::shared_ptr<SocketAsync> socket_async = factory->CreateSocket(SockType::RAW, Endpoint{});

    auto lambda = [&](std::int16_t ret) noexcept {
        CallbackFn(ret);
    };
    auto result = socket_async->ConnectAsync(std::move(lambda));

    ASSERT_EQ(result, kExitNotSupported);
}

}  // namespace

}  // namespace os
}  // namespace score
