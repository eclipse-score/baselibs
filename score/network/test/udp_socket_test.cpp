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
#include "score/network/udp_socket.h"

#include "score/network/ipv4_address.h"
#include "score/os/mocklib/fcntl_mock.h"
#include "score/os/mocklib/socketmock.h"
#include "score/os/mocklib/unistdmock.h"

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
using ::testing::Eq;
using ::testing::Exactly;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

const auto kAcessUnExpectedError = score::cpp::make_unexpected(score::os::Error::createFromErrno(EACCES));

class AUdpSocketWithMockedPosix : public Test
{
  public:
    os::MockGuard<NiceMock<FcntlMock>> fcntl_mock{};
    os::MockGuard<NiceMock<SocketMock>> socket_mock{};
    os::MockGuard<NiceMock<UnistdMock>> unistd_mock{};
};

TEST_F(AUdpSocketWithMockedPosix, OnContructionCreatesTheUnderlyingSocket)
{
    std::int32_t socket_id{25};
    EXPECT_CALL(*socket_mock, socket(_, _, _)).WillOnce(Return(socket_id));
    auto socket_expected = UdpSocket::Make();
    ASSERT_TRUE(socket_expected.has_value());
    EXPECT_EQ(socket_expected.value().GetFileDescriptor(), socket_id);
}

TEST_F(AUdpSocketWithMockedPosix, FailsToGetSocketStatusFlags)
{
    Fcntl::Open flags = Fcntl::Open::kReadWrite;
    testing::InSequence seq;
    EXPECT_CALL(*fcntl_mock, fcntl(_, Fcntl::Command::kFileGetStatusFlags)).Times(1).WillOnce(Return(flags));
    EXPECT_CALL(*fcntl_mock, fcntl(_, Fcntl::Command::kFileSetStatusFlags, flags | Fcntl::Open::kNonBlocking)).Times(1);
    auto socket_expected = UdpSocket::Make();
}

TEST_F(AUdpSocketWithMockedPosix, FailsToSetSocketNonBlocking)
{
    testing::InSequence seq;
    EXPECT_CALL(*fcntl_mock, fcntl(_, Fcntl::Command::kFileGetStatusFlags))
        .Times(1)
        .WillOnce(Return(kAcessUnExpectedError));
    auto socket_expected = UdpSocket::Make();
    EXPECT_FALSE(socket_expected.has_value());
}

TEST_F(AUdpSocketWithMockedPosix, OnContructionSetsNonBlockingFlag)
{
    Fcntl::Open flags = Fcntl::Open::kReadWrite;
    testing::InSequence seq;
    EXPECT_CALL(*fcntl_mock, fcntl(_, Fcntl::Command::kFileGetStatusFlags)).Times(1).WillOnce(Return(flags));
    EXPECT_CALL(*fcntl_mock, fcntl(_, Fcntl::Command::kFileSetStatusFlags, flags | Fcntl::Open::kNonBlocking))
        .WillOnce(Return(kAcessUnExpectedError));
    auto socket_expected = UdpSocket::Make();
    EXPECT_FALSE(socket_expected.has_value());
}

TEST_F(AUdpSocketWithMockedPosix, OnContructionHasNoErrors)
{
    auto socket_expected = UdpSocket::Make();
    ASSERT_THAT(socket_expected.has_value(), Eq(true));
    UdpSocket socket = std::move(socket_expected.value());
}

TEST_F(AUdpSocketWithMockedPosix, WhenMovedConstructedWillCallTheUnderlyingCloseOnlyOnce)
{
    // For the socket destructor
    EXPECT_CALL(*unistd_mock, close(_)).Times(1);

    auto socket_expected = UdpSocket::Make();
    ASSERT_THAT(socket_expected.has_value(), Eq(true));
    UdpSocket socket = std::move(socket_expected.value());
}

TEST_F(AUdpSocketWithMockedPosix, WhenMovedAssignedWillCallTheUnderlyingCloseOnlyOnce)
{
    // For the moved_assigned in the moment of being assigned to
    // and for the moved_assigned in the moment of being destructed
    EXPECT_CALL(*unistd_mock, close(_)).Times(2);

    auto socket_expected = UdpSocket::Make();
    ASSERT_THAT(socket_expected.has_value(), Eq(true));
    UdpSocket socket = std::move(socket_expected.value());

    auto moved_assigned_expected = UdpSocket::Make();
    ASSERT_THAT(moved_assigned_expected.has_value(), Eq(true));
    UdpSocket moved_assigned = std::move(socket_expected.value());
    moved_assigned = std::move(socket);
}

TEST_F(AUdpSocketWithMockedPosix, CanBeConstructedWithErrorsAndItCanBeCheckIfHasErrors)
{
    const auto ARBITRARY_ERROR = score::os::Error::createFromErrno(42);
    EXPECT_CALL(*socket_mock, socket(_, _, _)).WillOnce(Return(score::cpp::make_unexpected(ARBITRARY_ERROR)));

    auto socket_expected = UdpSocket::Make();
    EXPECT_THAT(socket_expected.has_value(), Eq(false));
    EXPECT_THAT(socket_expected.error(), Eq(ARBITRARY_ERROR));
}

TEST_F(AUdpSocketWithMockedPosix, OnDestructionClosesTheUnderlyingSocket)
{
    const auto VALID_FILE_DESCRIPTOR = 10;
    EXPECT_CALL(*socket_mock, socket(_, _, _)).WillOnce(Return(VALID_FILE_DESCRIPTOR));

    EXPECT_CALL(*unistd_mock, close(VALID_FILE_DESCRIPTOR)).WillOnce(Return(score::cpp::expected_blank<score::os::Error>{}));

    auto socket_expected = UdpSocket::Make();
    ASSERT_THAT(socket_expected.has_value(), Eq(true));
    UdpSocket socket = std::move(socket_expected.value());
}

TEST_F(AUdpSocketWithMockedPosix, WhenBoundCallsTheUnderlyingBind)
{
    EXPECT_CALL(*socket_mock, bind(_, _, _)).WillOnce(Return(score::cpp::expected_blank<score::os::Error>{}));

    auto socket_expected = UdpSocket::Make();
    ASSERT_THAT(socket_expected.has_value(), Eq(true));
    UdpSocket socket = std::move(socket_expected.value());
    socket.Bind(Ipv4Address{"1.12.123.12"}, 42);
}

TEST_F(AUdpSocketWithMockedPosix,
       WhenBondTwiceWithSameAddressAndPortCallsTheUnderlyingBindTwiceWithoutCheckingIfAlreadyBound)
{
    EXPECT_CALL(*socket_mock, bind(_, _, _)).Times(Exactly(2));

    auto socket_expected = UdpSocket::Make();
    ASSERT_THAT(socket_expected.has_value(), Eq(true));
    UdpSocket socket = std::move(socket_expected.value());
    socket.Bind(Ipv4Address{"1.12.123.12"}, 42);
    socket.Bind(Ipv4Address{"1.12.123.12"}, 42);
}

TEST_F(AUdpSocketWithMockedPosix, CallsPosixReceiveFromWhenCallingReceiveFromWithUnsignedCharVector)
{
    constexpr auto BUFFER_SIZE{512};
    std::vector<unsigned char> buffer(BUFFER_SIZE, 0);

    EXPECT_CALL(*socket_mock, recvfrom(_, buffer.data(), BUFFER_SIZE, _, _, _)).Times(Exactly(1));

    auto socket_expected = UdpSocket::Make();
    ASSERT_THAT(socket_expected.has_value(), Eq(true));
    UdpSocket socket = std::move(socket_expected.value());
    socket.Bind(Ipv4Address{"1.12.123.12"}, 42);
    socket.TryReceive(buffer.begin().base(), buffer.size());
}

TEST_F(AUdpSocketWithMockedPosix, CallsPosixReceiveFromWhenCallingReceiveFromWithUnsignedCharArray)
{
    constexpr auto BUFFER_SIZE{512};
    std::array<unsigned char, BUFFER_SIZE> buffer{};
    buffer.fill(0);

    EXPECT_CALL(*socket_mock, recvfrom(_, buffer.data(), BUFFER_SIZE, _, _, _)).Times(Exactly(1));

    auto socket_expected = UdpSocket::Make();
    ASSERT_THAT(socket_expected.has_value(), Eq(true));
    UdpSocket socket = std::move(socket_expected.value());
    socket.Bind(Ipv4Address{"1.12.123.12"}, 42);
    socket.TryReceive(buffer.begin(), buffer.size());
}

TEST_F(AUdpSocketWithMockedPosix, CallsPosixReceiveFromWhenCallingReceiveFromWithUnsignedCharCArray)
{
    constexpr auto BUFFER_SIZE{512};
    unsigned char buffer[BUFFER_SIZE];

    EXPECT_CALL(*socket_mock, recvfrom(_, buffer, BUFFER_SIZE, _, _, _)).Times(Exactly(1));

    auto socket_expected = UdpSocket::Make();
    ASSERT_THAT(socket_expected.has_value(), Eq(true));
    UdpSocket socket = std::move(socket_expected.value());
    socket.Bind(Ipv4Address{"1.12.123.12"}, 42);
    socket.TryReceive(buffer, BUFFER_SIZE);
}

TEST_F(AUdpSocketWithMockedPosix, CallsPosixReceiveFromWhenCallingReceiveFromWithAddressWithUnsignedCharVector)
{
    constexpr auto BUFFER_SIZE{512};
    std::vector<unsigned char> buffer(BUFFER_SIZE, 0);
    sockaddr_in source_address;
    source_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    socklen_t address_length = sizeof(source_address);
    Ipv4Address test_ip = {127U, 0U, 0U, 1U};

    EXPECT_CALL(*socket_mock, recvfrom(_, buffer.data(), BUFFER_SIZE, _, _, _))
        .Times(Exactly(1))
        .WillOnce([&](const std::int32_t,
                      void*,
                      const size_t len,
                      const Socket::MessageFlag,
                      sockaddr* addr,
                      socklen_t* addrlen) {
            memcpy(addr, &source_address, sizeof(source_address));
            *addrlen = address_length;
            return static_cast<ssize_t>(len);
        });

    auto socket_expected = UdpSocket::Make();
    ASSERT_TRUE(socket_expected.has_value());
    UdpSocket socket = std::move(socket_expected.value());
    socket.Bind(Ipv4Address{"1.12.123.12"}, 42);

    auto ret = socket.TryReceiveWithAddress(buffer.begin().base(), buffer.size());
    ASSERT_TRUE(ret.has_value());

    ssize_t numBytes = std::get<0>(ret.value());
    score::os::Ipv4Address senderAddress = std::get<1>(ret.value());

    ASSERT_EQ(senderAddress.ToString(), test_ip.ToString());
    ASSERT_EQ(numBytes, BUFFER_SIZE);
}

TEST_F(AUdpSocketWithMockedPosix, CallsPosixReceiveFromWhenCallingReceiveFromWithAddressWithUnsignedCharArray)
{
    constexpr auto BUFFER_SIZE{512};
    std::array<unsigned char, BUFFER_SIZE> buffer{};
    buffer.fill(0);
    sockaddr_in source_address;
    source_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    socklen_t address_length = sizeof(source_address);
    Ipv4Address test_ip = {127U, 0U, 0U, 1U};

    EXPECT_CALL(*socket_mock, recvfrom(_, buffer.data(), BUFFER_SIZE, _, _, _))
        .Times(Exactly(1))
        .WillOnce([&](const std::int32_t,
                      void*,
                      const size_t len,
                      const Socket::MessageFlag,
                      sockaddr* addr,
                      socklen_t* addrlen) {
            memcpy(addr, &source_address, sizeof(source_address));
            *addrlen = address_length;
            return static_cast<ssize_t>(len);
        });

    auto socket_expected = UdpSocket::Make();
    ASSERT_TRUE(socket_expected.has_value());
    UdpSocket socket = std::move(socket_expected.value());
    socket.Bind(Ipv4Address{"1.12.123.12"}, 42);

    auto ret = socket.TryReceiveWithAddress(buffer.begin(), buffer.size());
    ASSERT_TRUE(ret.has_value());

    ssize_t numBytes = std::get<0>(ret.value());
    score::os::Ipv4Address senderAddress = std::get<1>(ret.value());

    ASSERT_EQ(senderAddress.ToString(), test_ip.ToString());
    ASSERT_EQ(numBytes, BUFFER_SIZE);
}

TEST_F(AUdpSocketWithMockedPosix, TryReceiveMultipleMessagesAssertsWhenProvidedBufferIsNotSufficient)
{
    constexpr auto kVlen = 2U;
    constexpr auto kMaxMsgSize = sizeof(std::uint32_t);

    using Msg = std::array<unsigned char, kMaxMsgSize>;
    using Msgs = std::array<Msg, kVlen>;

    auto socket_expected = UdpSocket::Make();
    ASSERT_TRUE(socket_expected.has_value());
    UdpSocket socket = std::move(socket_expected.value());

    Msgs buffer;
    EXPECT_DEATH(socket.TryReceiveMultipleMessagesWithAddress(buffer.at(0).begin(), 4, kVlen, kMaxMsgSize), "");
}

TEST_F(AUdpSocketWithMockedPosix,
       CallsPosixRecvmmsgWhenCallingTryReceiveMultipleMessagesWithAddressWithUnsignedCharArray)
{
    constexpr auto kVlen = 2U;
    constexpr auto kMaxMsgSize = sizeof(std::uint32_t);

    using Msg = std::array<unsigned char, kMaxMsgSize>;
    using Msgs = std::array<Msg, kVlen>;

    Msgs buffer;
    ::memset(buffer.data(), 0, kVlen * kMaxMsgSize);

    sockaddr_in source_address{};
    source_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    const Ipv4Address expected_ip = {127U, 0U, 0U, 1U};

    EXPECT_CALL(*socket_mock, recvmmsg(_, _, kVlen, _, _))
        .Times(Exactly(1))
        .WillOnce([source_address](const std::int32_t /*sockfd*/,
                                   mmsghdr* msgvec,
                                   const unsigned int vlen,
                                   const Socket::MessageFlag /*flags*/,
                                   timespec*) {
            for (auto i = 0U; i < vlen; ++i)
            {
                mmsghdr& msg = msgvec[i];
                msg.msg_len = kMaxMsgSize;
                const std::uint32_t payload{i + 1U};
                ::memcpy(msg.msg_hdr.msg_iov->iov_base, &payload, sizeof(payload));
                ::memcpy(msg.msg_hdr.msg_name, &source_address, sizeof(source_address));
            }
            return vlen;
        });

    auto socket_expected = UdpSocket::Make();
    ASSERT_TRUE(socket_expected.has_value());
    UdpSocket socket = std::move(socket_expected.value());
    socket.Bind(Ipv4Address{"1.12.123.12"}, 42);

    auto ret = socket.TryReceiveMultipleMessagesWithAddress(buffer.at(0).begin(), sizeof(buffer), kVlen, kMaxMsgSize);
    ASSERT_TRUE(ret.has_value());

    const auto recv_msgs = ret.value();
    ASSERT_EQ(recv_msgs.size(), kVlen);

    for (auto i = 0U; i < kVlen; ++i)
    {
        const auto& [recv_n_bytes, sender_addr] = recv_msgs.at(i);

        std::uint32_t payload{};
        ::memcpy(&payload, &buffer[i], static_cast<size_t>(recv_n_bytes));
        EXPECT_EQ(payload, i + 1);

        EXPECT_EQ(sender_addr.ToString(), expected_ip.ToString());
        EXPECT_EQ(recv_n_bytes, kMaxMsgSize);
    }
}

TEST_F(AUdpSocketWithMockedPosix, TryReceiveMultipleMessagesWithAddressFailsWhenRecvmmsgErrors)
{
    constexpr auto kVlen = 2U;
    constexpr auto kMaxMsgSize = sizeof(std::uint32_t);

    using Msg = std::array<unsigned char, kMaxMsgSize>;
    using Msgs = std::array<Msg, kVlen>;

    Msgs buffer;
    ::memset(buffer.data(), 0, kVlen * kMaxMsgSize);

    EXPECT_CALL(*socket_mock, recvmmsg).WillOnce(Return(kAcessUnExpectedError));
    auto socket_expected = UdpSocket::Make();
    ASSERT_TRUE(socket_expected.has_value());
    UdpSocket socket = std::move(socket_expected.value());
    socket.Bind(Ipv4Address{"1.12.123.12"}, 42);

    auto ret = socket.TryReceiveMultipleMessagesWithAddress(buffer.at(0).begin(), sizeof(buffer), kVlen, kMaxMsgSize);
    ASSERT_FALSE(ret.has_value());
}

TEST_F(AUdpSocketWithMockedPosix, CallsPosixReceiveFromWhenCallingReceiveFromWithAddressWithUnsignedCharCArray)
{
    constexpr auto BUFFER_SIZE{512};
    unsigned char buffer[BUFFER_SIZE];
    sockaddr_in source_address;
    source_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    socklen_t address_length = sizeof(source_address);
    Ipv4Address test_ip = {127U, 0U, 0U, 1U};

    EXPECT_CALL(*socket_mock, recvfrom(_, buffer, BUFFER_SIZE, _, _, _))
        .Times(Exactly(1))
        .WillOnce([&](const std::int32_t,
                      void*,
                      const size_t len,
                      const Socket::MessageFlag,
                      sockaddr* addr,
                      socklen_t* addrlen) {
            memcpy(addr, &source_address, sizeof(source_address));
            *addrlen = address_length;
            return static_cast<ssize_t>(len);
        });

    auto socket_expected = UdpSocket::Make();
    ASSERT_TRUE(socket_expected.has_value());
    UdpSocket socket = std::move(socket_expected.value());
    socket.Bind(Ipv4Address{"1.12.123.12"}, 42);

    auto ret = socket.TryReceiveWithAddress(buffer, BUFFER_SIZE);
    ASSERT_TRUE(ret.has_value());

    ssize_t numBytes = std::get<0>(ret.value());
    score::os::Ipv4Address senderAddress = std::get<1>(ret.value());

    ASSERT_EQ(senderAddress.ToString(), test_ip.ToString());
    ASSERT_EQ(numBytes, BUFFER_SIZE);
}

TEST_F(AUdpSocketWithMockedPosix, TryReceiveWithAddressFailure)
{
    constexpr auto BUFFER_SIZE{512};
    unsigned char buffer[BUFFER_SIZE];

    EXPECT_CALL(*socket_mock, recvfrom).WillOnce(Return(kAcessUnExpectedError));
    auto socket_expected = UdpSocket::Make();
    ASSERT_TRUE(socket_expected.has_value());
    UdpSocket socket = std::move(socket_expected.value());
    socket.Bind(Ipv4Address{"1.12.123.12"}, 42);

    auto ret = socket.TryReceiveWithAddress(buffer, BUFFER_SIZE);
    ASSERT_FALSE(ret.has_value());
}

TEST_F(AUdpSocketWithMockedPosix, CallsPosixSendToWhenCallingSendToWithUnsignedCharVector)
{
    constexpr auto BUFFER_SIZE{512};
    std::vector<unsigned char> buffer(BUFFER_SIZE, 0);

    EXPECT_CALL(*socket_mock, sendto(_, buffer.data(), BUFFER_SIZE, _, _, _)).Times(Exactly(1));

    auto socket_expected = UdpSocket::Make();
    ASSERT_THAT(socket_expected.has_value(), Eq(true));
    UdpSocket socket = std::move(socket_expected.value());
    socket.Bind(Ipv4Address{"1.12.123.12"}, 42);
    socket.TrySendTo(Ipv4Address{"1.12.123.13"}, 42, buffer.begin().base(), buffer.size());
}

TEST_F(AUdpSocketWithMockedPosix, CallsPosixSendToWhenCallingSendToWithUnsignedCharArray)
{
    constexpr auto BUFFER_SIZE{512};
    std::array<unsigned char, BUFFER_SIZE> buffer{};
    buffer.fill(0);

    EXPECT_CALL(*socket_mock, sendto(_, buffer.data(), BUFFER_SIZE, _, _, _)).Times(Exactly(1));

    auto socket_expected = UdpSocket::Make();
    ASSERT_THAT(socket_expected.has_value(), Eq(true));
    UdpSocket socket = std::move(socket_expected.value());
    socket.Bind(Ipv4Address{"1.12.123.12"}, 42);
    socket.TrySendTo(Ipv4Address{"1.12.123.13"}, 42, buffer.begin(), buffer.size());
}

TEST_F(AUdpSocketWithMockedPosix, CallsPosixSendToWhenCallingSendToWithUnsignedCharCStyleArray)
{
    constexpr auto BUFFER_SIZE{512};
    unsigned char buffer[BUFFER_SIZE];

    EXPECT_CALL(*socket_mock, sendto(_, buffer, BUFFER_SIZE, _, _, _)).Times(Exactly(1));

    auto socket_expected = UdpSocket::Make();
    ASSERT_THAT(socket_expected.has_value(), Eq(true));
    UdpSocket socket = std::move(socket_expected.value());
    socket.Bind(Ipv4Address{"1.12.123.12"}, 42);
    socket.TrySendTo(Ipv4Address{"1.12.123.13"}, 42, buffer, BUFFER_SIZE);
}

TEST_F(AUdpSocketWithMockedPosix, TrySendToFailureWhenSocketMessageSendFails)
{
    constexpr auto BUFFER_SIZE{512};
    unsigned char buffer[BUFFER_SIZE];

    EXPECT_CALL(*socket_mock, sendto(_, buffer, BUFFER_SIZE, _, _, _))
        .Times(Exactly(1))
        .WillOnce(Return(kAcessUnExpectedError));

    auto socket_expected = UdpSocket::Make();
    ASSERT_THAT(socket_expected.has_value(), Eq(true));
    UdpSocket socket = std::move(socket_expected.value());

    const auto result = socket.TrySendTo(Ipv4Address{"192.32.1.1"}, 42, buffer, BUFFER_SIZE);
    ASSERT_FALSE(result.has_value());
}

TEST_F(AUdpSocketWithMockedPosix, BindFailsWhenIpAddressIsInvalid)
{
    auto socket_expected = UdpSocket::Make();
    ASSERT_THAT(socket_expected.has_value(), Eq(true));
    UdpSocket socket = std::move(socket_expected.value());
    const auto result = socket.Bind(Ipv4Address{"1.268.123.12"}, 42);
    ASSERT_FALSE(result.has_value());
}

TEST_F(AUdpSocketWithMockedPosix, CallsPosixSetSockOptWithCorrectArgsWhenCallingSetSocketOption)
{
    const std::int32_t OPT_LEVEL = IPPROTO_IP;
    const std::int32_t OPT_NAME = IP_ADD_MEMBERSHIP;
    ip_mreq opt_value{{}, {htonl(INADDR_ANY)}};
    ASSERT_NE(inet_aton("192.168.4.0", &opt_value.imr_multiaddr), 0);
    const socklen_t OPT_LEN = sizeof(opt_value);

    EXPECT_CALL(*socket_mock, setsockopt(_, OPT_LEVEL, OPT_NAME, &opt_value, OPT_LEN));

    auto socket_expected = UdpSocket::Make();
    ASSERT_THAT(socket_expected.has_value(), Eq(true));
    UdpSocket socket = std::move(socket_expected.value());

    auto status = socket.SetSocketOption(OPT_LEVEL, OPT_NAME, &opt_value, OPT_LEN);
    ASSERT_THAT(status.has_value(), Eq(true));
}

TEST_F(AUdpSocketWithMockedPosix, HandlesTheReturnedErrorFromSetSockOptWhenCallingSetSocketOption)
{
    const auto ERROR = score::os::Error::createFromErrno(EPERM);
    EXPECT_CALL(*socket_mock, setsockopt(_, _, _, _, _)).WillOnce(Return(score::cpp::make_unexpected(ERROR)));

    auto socket_expected = UdpSocket::Make();
    ASSERT_THAT(socket_expected.has_value(), Eq(true));
    UdpSocket socket = std::move(socket_expected.value());

    auto status = socket.SetSocketOption(IPPROTO_IP, IP_ADD_MEMBERSHIP, nullptr, 0);
    ASSERT_THAT(status.error(), Eq(ERROR));
}

}  // namespace

}  // namespace os
}  // namespace score
