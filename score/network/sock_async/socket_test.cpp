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
#include "score/network/sock_async/socket.h"
#include "score/network/sock_async/net_endpoint.h"
#include "score/os/mocklib/socketmock.h"
#include <score/expected.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace os
{

namespace
{

using ::testing::_;
using namespace testing;
using Endpoint = score::os::NetEndpoint;

struct MockBase : public SocketBase
{
  public:
    MockBase(NetEndpoint endpoint) : SocketBase(endpoint) {}
    MOCK_METHOD((std::int32_t),
                WriteAsync,
                (std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>>, score::os::AsyncCallback),
                (noexcept, override));
    MOCK_METHOD((std::int32_t),
                ReadAsync,
                (std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>>, score::os::AsyncCallback),
                (noexcept, override));
    MOCK_METHOD((std::int32_t), ConnectAsync, (score::os::AsyncConnectCallback), (noexcept, override));
    MOCK_METHOD(std::int32_t, GetSockFD, (), (const, noexcept, override));
};

struct SocketBaseTest : ::testing::Test
{
    void SetUp() override
    {
        score::os::Socket::set_testing_instance(sock_mock_);
    }

    score::os::SocketMock sock_mock_;
};

TEST_F(SocketBaseTest, Constructor)
{
    Ipv4Address addr_1(1, 2, 0, 4);
    std::uint16_t port_1{32321};
    NetEndpoint endp_1(addr_1, port_1);
    MockBase socketBase(endp_1);
    ASSERT_EQ(false, socketBase.IsBound());
}

TEST_F(SocketBaseTest, SetOptionReuseAddr)
{
    Ipv4Address addr_1(1, 2, 0, 4);
    std::uint16_t port_1{32321};
    NetEndpoint endp_1(addr_1, port_1);
    MockBase socketBase(endp_1);
    ASSERT_EQ(false, socketBase.IsBound());
    EXPECT_CALL(sock_mock_, setsockopt(_, _, _, _, _)).Times(1);
    socketBase.SetOption(ReuseAddrOption());
    ASSERT_TRUE(score::cpp::holds_alternative<ReuseAddrOption>(socketBase.GetOptionVariant()));
}

TEST_F(SocketBaseTest, SetOptionTimeout)
{
    Ipv4Address addr_1(1, 2, 0, 4);
    std::uint16_t port_1{32321};
    NetEndpoint endp_1(addr_1, port_1);
    MockBase socketBase(endp_1);
    ASSERT_EQ(false, socketBase.IsBound());
    EXPECT_CALL(sock_mock_, setsockopt(_, _, _, _, _)).Times(1);
    socketBase.SetOption(TimeoutOption(1));
    ASSERT_TRUE(score::cpp::holds_alternative<TimeoutOption>(socketBase.GetOptionVariant()));
}

TEST_F(SocketBaseTest, SetOptionBufferSize)
{
    Ipv4Address addr_1(1, 2, 0, 4);
    std::uint16_t port_1{32321};
    NetEndpoint endp_1(addr_1, port_1);
    MockBase socketBase(endp_1);
    ASSERT_EQ(false, socketBase.IsBound());
    EXPECT_CALL(sock_mock_, setsockopt(_, _, _, _, _)).Times(1);
    socketBase.SetOption(BufferSizeOption(1));
    ASSERT_TRUE(score::cpp::holds_alternative<BufferSizeOption>(socketBase.GetOptionVariant()));
}

template <typename... Ts>
bool variant_has_value(const score::cpp::variant<Ts...>& var)
{
    return var.index() != static_cast<std::ptrdiff_t>(-1);
}

TEST_F(SocketBaseTest, SetOptionNoOption)
{
    Ipv4Address addr_1(1, 2, 0, 4);
    std::uint16_t port_1{32321};
    NetEndpoint endp_1(addr_1, port_1);
    MockBase socketBase(endp_1);
    ASSERT_EQ(false, socketBase.IsBound());
    score::cpp::variant<ReuseAddrOption, TimeoutOption, BufferSizeOption> optionVariant = socketBase.GetOptionVariant();
    ASSERT_TRUE(variant_has_value(optionVariant));
}

TEST_F(SocketBaseTest, SetOptionWriteSyncFalse)
{
    Ipv4Address addr_1(1, 2, 0, 4);
    std::uint16_t port_1{32321};
    NetEndpoint endp_1(addr_1, port_1);
    MockBase socketBase(endp_1);
    ASSERT_EQ(false, socketBase.IsBound());
    std::vector<std::uint8_t> DataRecord_Data{1, 2, 3};
    score::cpp::span<std::uint8_t> buffer(DataRecord_Data);
    std::vector<score::cpp::span<std::uint8_t>> test_vector;
    test_vector.push_back(buffer);
    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> span_ptr =
        std::make_shared<std::vector<score::cpp::span<std::uint8_t>>>(test_vector);
    EXPECT_CALL(sock_mock_, sendto(_, _, _, _, _, _))
        .WillOnce(::testing::Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EINVAL))));
    auto ret = socketBase.WriteSync(span_ptr);
    ASSERT_THAT(ret.has_value(), Eq(false));
}

TEST_F(SocketBaseTest, SetOptionWriteSyncTrue)
{
    Ipv4Address addr_1(1, 2, 0, 4);
    std::uint16_t port_1{32321};
    NetEndpoint endp_1(addr_1, port_1);
    MockBase socketBase(endp_1);
    ASSERT_EQ(false, socketBase.IsBound());
    std::vector<std::uint8_t> DataRecord_Data{1, 2, 3};
    score::cpp::span<std::uint8_t> buffer(DataRecord_Data);
    std::vector<score::cpp::span<std::uint8_t>> test_vector;
    test_vector.push_back(buffer);
    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> span_ptr =
        std::make_shared<std::vector<score::cpp::span<std::uint8_t>>>(test_vector);
    EXPECT_CALL(sock_mock_, sendto(_, _, _, _, _, _)).WillOnce(::testing::Return(10));
    auto ret = socketBase.WriteSync(span_ptr);
    ASSERT_THAT(ret.has_value(), Eq(true));
}

TEST_F(SocketBaseTest, ReadSyncWithoutAddress)
{
    NetEndpoint endp_1;
    MockBase socketBase(endp_1);

    std::vector<std::uint8_t> DataRecord_Data{1, 2, 3};
    score::cpp::span<std::uint8_t> buffer(DataRecord_Data);
    std::vector<score::cpp::span<std::uint8_t>> test_vector;
    test_vector.push_back(buffer);
    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> span_ptr =
        std::make_shared<std::vector<score::cpp::span<std::uint8_t>>>(test_vector);

    EXPECT_CALL(sock_mock_, recvfrom(_, _, _, _, _, _))
        .Times(Exactly(1))
        .WillOnce([&](const std::int32_t, void*, const size_t len, const Socket::MessageFlag, sockaddr*, socklen_t*) {
            return static_cast<ssize_t>(len);
        });

    auto ret = socketBase.ReadSync(span_ptr);
    ASSERT_TRUE(score::cpp::holds_alternative<ssize_t>(ret));
}

TEST_F(SocketBaseTest, ReadSyncWithoutAddressError)
{
    NetEndpoint endp_1;
    MockBase socketBase(endp_1);

    std::vector<std::uint8_t> DataRecord_Data{1, 2, 3};
    score::cpp::span<std::uint8_t> buffer(DataRecord_Data);
    std::vector<score::cpp::span<std::uint8_t>> test_vector;
    test_vector.push_back(buffer);
    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> span_ptr =
        std::make_shared<std::vector<score::cpp::span<std::uint8_t>>>(test_vector);

    EXPECT_CALL(sock_mock_, recvfrom(_, _, _, _, _, _))
        .Times(Exactly(1))
        .WillOnce([&](const std::int32_t, void*, const size_t, const Socket::MessageFlag, sockaddr*, socklen_t*) {
            return score::cpp::make_unexpected(Error::createFromErrno());
        });

    auto ret = socketBase.ReadSync(span_ptr);
    ASSERT_TRUE(score::cpp::holds_alternative<Error>(ret));
}

TEST_F(SocketBaseTest, ReadSyncWithAddress)
{
    Ipv4Address addr_1(1, 2, 0, 4);
    std::uint16_t port_1{32321};
    NetEndpoint endp_1(addr_1, port_1);
    MockBase socketBase(endp_1);

    std::vector<std::uint8_t> DataRecord_Data{1, 2, 3};
    score::cpp::span<std::uint8_t> buffer(DataRecord_Data);
    std::vector<score::cpp::span<std::uint8_t>> test_vector;
    test_vector.push_back(buffer);
    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> span_ptr =
        std::make_shared<std::vector<score::cpp::span<std::uint8_t>>>(test_vector);

    EXPECT_CALL(sock_mock_, recvfrom(_, _, _, _, _, _))
        .Times(Exactly(1))
        .WillOnce([&](const std::int32_t, void*, const size_t len, const Socket::MessageFlag, sockaddr*, socklen_t*) {
            return static_cast<ssize_t>(len);
        });

    auto ret = socketBase.ReadSync(span_ptr);
    using type = std::tuple<ssize_t, score::os::Ipv4Address>;
    ASSERT_TRUE(score::cpp::holds_alternative<type>(ret));
}

TEST_F(SocketBaseTest, ReadSyncWithAddressError)
{
    Ipv4Address addr_1(1, 2, 0, 4);
    std::uint16_t port_1{32321};
    NetEndpoint endp_1(addr_1, port_1);
    MockBase socketBase(endp_1);

    std::vector<std::uint8_t> DataRecord_Data{1, 2, 3};
    score::cpp::span<std::uint8_t> buffer(DataRecord_Data);
    std::vector<score::cpp::span<std::uint8_t>> test_vector;
    test_vector.push_back(buffer);
    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> span_ptr =
        std::make_shared<std::vector<score::cpp::span<std::uint8_t>>>(test_vector);

    EXPECT_CALL(sock_mock_, recvfrom(_, _, _, _, _, _))
        .Times(Exactly(1))
        .WillOnce([&](const std::int32_t, void*, const size_t, const Socket::MessageFlag, sockaddr*, socklen_t*) {
            return score::cpp::make_unexpected(Error::createFromErrno());
        });

    auto ret = socketBase.ReadSync(span_ptr);
    ASSERT_TRUE(score::cpp::holds_alternative<Error>(ret));
}

TEST_F(SocketBaseTest, Bind)
{
    Ipv4Address addr_1(1, 2, 0, 4);
    std::uint16_t port_1{32321};
    NetEndpoint endp_1(addr_1, port_1);
    MockBase socketBase(endp_1);

    EXPECT_CALL(sock_mock_, bind(_, _, _)).WillOnce(Return(score::cpp::blank{}));

    socketBase.Bind(endp_1);
    ASSERT_TRUE(socketBase.IsBound());
}

TEST_F(SocketBaseTest, BindFailed)
{
    Ipv4Address addr_1(1, 2, 0, 4);
    std::uint16_t port_1{32321};
    NetEndpoint endp_1(addr_1, port_1);
    MockBase socketBase(endp_1);

    EXPECT_CALL(sock_mock_, bind(_, _, _)).WillOnce(Return(score::cpp::make_unexpected(Error::createFromErrno())));

    socketBase.Bind(endp_1);
    ASSERT_FALSE(socketBase.IsBound());
}

}  // namespace

}  // namespace os
}  // namespace score
