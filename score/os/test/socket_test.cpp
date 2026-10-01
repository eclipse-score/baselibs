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
#include "score/os/socket.h"

#include "score/os/unistd.h"
#include "score/os/version.h"

#include <arpa/inet.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <limits>
#include <thread>

#include "score/latch.hpp"

namespace score
{
namespace os
{
namespace
{

using namespace ::testing;
constexpr std::uint16_t kDefaultPortForTesting{61012U};
constexpr std::string_view kDefaultAddrForTesting{"127.0.0.1"};
constexpr std::string_view kTextForTesting{"Hello, BMW!"};

TEST(MessageFlag, IsDefaultConstructedWithSameValueAsNone)
{
    ASSERT_THAT(Socket::MessageFlag{}, ::testing::Eq(Socket::MessageFlag::kNone));
}

TEST(Domain, IsDefaultConstructedWithSameValueAsInvalid)
{
    // Enum class can be default initialized, and it would be initialized to 0
    // We want people to take a conscious decision on what domain to use, that is
    // why an Invalid value is added that is the same if it would be default constructed
    ASSERT_THAT(Socket::Domain{}, ::testing::Eq(Socket::Domain::kInvalid));
}

struct SocketTestFixture : public Test
{
  public:
    void TearDown() override
    {
        if (server_fd_ != -1)
        {
            unistd_.close(server_fd_);
        }
        if (client_fd_ != -1)
        {
            unistd_.close(client_fd_);
        }
        if (accepted_fd_ != -1)
        {
            unistd_.close(accepted_fd_);
        }
    }

    void TestSocket(Socket::Domain domain = Socket::Domain::kIPv4, std::int32_t type = SOCK_STREAM)
    {
        auto result = instance_.socket(domain, type, 0);
        ASSERT_TRUE(result.has_value());
        server_fd_ = result.value();
        EXPECT_GT(server_fd_, 0);
    }

    void InitAddr(std::string_view addr = std::string_view{}, std::uint16_t port = 0)
    {
        addr_.sin_family = AF_INET;
        addr_.sin_port = htons(port);
        if (addr.empty())
        {
            addr_.sin_addr.s_addr = INADDR_ANY;
        }
        else
        {
            inet_pton(AF_INET, addr.data(), &addr_.sin_addr);
        }
    }

    void TestBind(bool expect_succeed = true)
    {
        auto bind_result = instance_.bind(server_fd_, reinterpret_cast<sockaddr*>(&addr_), sizeof(sockaddr_in));
        ASSERT_EQ(bind_result.has_value(), expect_succeed);
        socklen_t addr_len{sizeof(addr_)};
        getsockname(server_fd_, reinterpret_cast<sockaddr*>(&addr_), &addr_len);
    }

    void TestListen(bool expect_succeed = true)
    {
        auto listen_result = instance_.listen(server_fd_, 2);
        ASSERT_EQ(listen_result.has_value(), expect_succeed);
    }

    void TestClientSocket(Socket::Domain domain = Socket::Domain::kIPv4, int type = SOCK_STREAM)
    {
        auto result = instance_.socket(domain, type, 0);
        ASSERT_TRUE(result.has_value());
        client_fd_ = result.value();
        EXPECT_GT(client_fd_, 0);
    }

    void InitServerAddr(std::string_view addr = kDefaultAddrForTesting, std::uint16_t port = 0)
    {
        server_addr_.sin_family = AF_INET;
        if (port == 0)
        {
            server_addr_.sin_port = addr_.sin_port;
        }
        else
        {
            server_addr_.sin_port = htons(port);
        }
        inet_pton(AF_INET, addr.data(), &server_addr_.sin_addr);
    }

    void TestConnect(bool expect_success = true)
    {
        auto connect_result =
            instance_.connect(client_fd_, reinterpret_cast<sockaddr*>(&server_addr_), sizeof(sockaddr_in));
        ASSERT_EQ(connect_result.has_value(), expect_success);
    }

    void TestAccept(bool expect_success = true)
    {
        unsigned int sock_size = sizeof(sockaddr_in);
        auto accept_result = instance_.accept(server_fd_, reinterpret_cast<sockaddr*>(&client_addr_), &sock_size);
        ASSERT_EQ(accept_result.has_value(), expect_success);
        if (expect_success)
        {
            accepted_fd_ = accept_result.value();
            EXPECT_GT(accepted_fd_, 0);
        }
    }

    void CreateServerAndClientSockets(int32_t type = SOCK_STREAM)
    {
        TestSocket(Socket::Domain::kIPv4, type);
        InitAddr();
        TestBind();
        TestListen();

        TestClientSocket(Socket::Domain::kIPv4, type);
        InitServerAddr();
        TestConnect();

        TestAccept();
    }

    sockaddr_in addr_{};
    sockaddr_in server_addr_{};
    sockaddr_in client_addr_{};
    int server_fd_{-1};
    int client_fd_{-1};
    int accepted_fd_{-1};
    Socket& instance_{Socket::instance()};
    Unistd& unistd_{Unistd::instance()};
    std::array<std::int8_t, 128> recvbuf_{0};
    std::vector<std::int8_t> msg_{kTextForTesting.begin(), kTextForTesting.end()};
};

TEST_F(SocketTestFixture, SocketShouldSucceedOnValidInput)
{
    TestSocket();
}

TEST_F(SocketTestFixture, SocketShouldFailOnInvalidInput)
{
    auto result = instance_.socket(Socket::Domain::kIPv4, SOCK_SEQPACKET, std::numeric_limits<std::int32_t>::max());
    ASSERT_FALSE(result.has_value());
#if defined(__linux__)
    EXPECT_EQ(result.error(), Error::createFromErrno(EINVAL));
#endif
}

TEST_F(SocketTestFixture, BindShouldSucceed)
{
    TestSocket();
    InitAddr();
    TestBind();
}

TEST_F(SocketTestFixture, BindWithInvalidAddrShouldFail)
{
    TestSocket();
    InitAddr(std::string_view{"10.0.0.1"}, 65535U);
    bool expect_succeed = false;
    TestBind(expect_succeed);
}

TEST_F(SocketTestFixture, BindWithInvalidSocketFdShouldFail)
{
    InitAddr();
    bool expect_succeed = false;
    TestBind(expect_succeed);
}

TEST_F(SocketTestFixture, ListenShouldSucceed)
{
    TestSocket();
    InitAddr();
    TestBind();
    TestListen();
}

TEST_F(SocketTestFixture, ListenShouldFailOnInvalidSocketFd)
{
    InitAddr();
    bool expect_succeed = false;
    TestListen(expect_succeed);
}

TEST_F(SocketTestFixture, TestAcceptAndConnect)
{
    CreateServerAndClientSockets();
}

TEST_F(SocketTestFixture, ConnectToInvalidAddressShouldFail)
{
    TestClientSocket();
    InitServerAddr("255.255.255.255", kDefaultPortForTesting);
    TestConnect(false);
}

struct SocketTestSetSockOptFixture : public SocketTestFixture, public ::testing::WithParamInterface<std::int32_t>
{
};
INSTANTIATE_TEST_SUITE_P(SocketOptions,
                         SocketTestSetSockOptFixture,
                         ::testing::Values(SO_REUSEADDR, SO_KEEPALIVE, SO_REUSEPORT));
TEST_P(SocketTestSetSockOptFixture, SetAndGetOption)
{
    std::int32_t optval{1};
    socklen_t optlen{sizeof(optval)};
    std::int32_t option = GetParam();
    TestSocket();

    auto result = instance_.setsockopt(server_fd_, SOL_SOCKET, option, &optval, optlen);
    ASSERT_TRUE(result.has_value());

    std::int32_t retrieved_optval{0};
    result = instance_.getsockopt(server_fd_, SOL_SOCKET, option, &retrieved_optval, &optlen);
    ASSERT_TRUE(result.has_value());

    EXPECT_GT(retrieved_optval, 0);
    EXPECT_EQ(optlen, sizeof(optval));
}

TEST_P(SocketTestSetSockOptFixture, TurnOffFlag)
{
    std::int32_t optval{0};
    socklen_t optlen{sizeof(optval)};
    std::int32_t option{GetParam()};
    TestSocket();

    auto result = instance_.setsockopt(server_fd_, SOL_SOCKET, option, &optval, optlen);
    ASSERT_TRUE(result.has_value());

    std::int32_t retrieved_optval{0};
    result = instance_.getsockopt(server_fd_, SOL_SOCKET, option, &retrieved_optval, &optlen);
    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(retrieved_optval, 0);
    EXPECT_EQ(optlen, sizeof(optval));
}

TEST_F(SocketTestFixture, SetInvalidOption)
{
    TestSocket();
    std::int32_t optval{1};
    socklen_t optlen{sizeof(optval)};
    std::int32_t invalid_opt{-1};
    auto result = instance_.setsockopt(server_fd_, SOL_SOCKET, invalid_opt, &optval, optlen);
    ASSERT_FALSE(result.has_value());
}

TEST_F(SocketTestFixture, GetOptionInsufficientBuffer)
{
    std::int32_t retrieved_optval{0};
    socklen_t optlen{1U};
    auto result = instance_.getsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &retrieved_optval, &optlen);
    ASSERT_FALSE(result.has_value());
}

TEST_F(SocketTestFixture, TestSendAndRecv)
{
    CreateServerAndClientSockets();

    auto result = instance_.send(client_fd_, msg_.data(), msg_.size(), Socket::MessageFlag::kNone);
    ASSERT_TRUE(result.has_value()) << "Failed to call ::send(): " << strerror(errno);
    EXPECT_EQ(result.value(), msg_.size());

    result = instance_.recv(accepted_fd_, recvbuf_.data(), sizeof(recvbuf_), Socket::MessageFlag::kNone);
    ASSERT_TRUE(result.has_value()) << "Failed to call ::recv(): " << strerror(errno);
    EXPECT_EQ(result.value(), msg_.size());
    EXPECT_EQ(std::string_view(reinterpret_cast<const char*>(recvbuf_.data()), msg_.size()), kTextForTesting);
}

TEST_F(SocketTestFixture, TestSendToClosedConnection)
{
    CreateServerAndClientSockets();
    unistd_.close(client_fd_);

    auto result = instance_.send(client_fd_, msg_.data(), msg_.size(), Socket::MessageFlag::kNone);
    ASSERT_FALSE(result.has_value());
}

TEST_F(SocketTestFixture, TestRecvFromClosedConnection)
{
    CreateServerAndClientSockets();
    unistd_.close(accepted_fd_);

    auto result = instance_.recv(accepted_fd_, recvbuf_.data(), sizeof(recvbuf_), Socket::MessageFlag::kNone);
    ASSERT_FALSE(result.has_value());
}

TEST_F(SocketTestFixture, TestSendToAndRecvFrom)
{
    TestSocket(Socket::Domain::kIPv4, SOCK_DGRAM);
    InitAddr();
    TestBind();

    TestClientSocket(Socket::Domain::kIPv4, SOCK_DGRAM);
    InitServerAddr();
    auto send_result = instance_.sendto(client_fd_,
                                        msg_.data(),
                                        msg_.size(),
                                        Socket::MessageFlag::kNone,
                                        reinterpret_cast<sockaddr*>(&server_addr_),
                                        sizeof(server_addr_));
    ASSERT_TRUE(send_result.has_value());
    EXPECT_EQ(send_result.value(), msg_.size());

    sockaddr_in sender_addr{};
    socklen_t sender_addr_len{sizeof(sender_addr)};
    auto receive_result = instance_.recvfrom(server_fd_,
                                             recvbuf_.data(),
                                             sizeof(recvbuf_),
                                             Socket::MessageFlag::kNone,
                                             reinterpret_cast<sockaddr*>(&sender_addr),
                                             &sender_addr_len);
    ASSERT_TRUE(receive_result.has_value());
    EXPECT_EQ(receive_result.value(), msg_.size());
    EXPECT_EQ(std::string_view(reinterpret_cast<const char*>(recvbuf_.data()), msg_.size()), kTextForTesting);
}

TEST_F(SocketTestFixture, TestSendZeroLengthData)
{
    TestSocket(Socket::Domain::kIPv4, SOCK_DGRAM);
    InitAddr();
    TestBind();

    TestClientSocket(Socket::Domain::kIPv4, SOCK_DGRAM);
    InitServerAddr();
    auto send_result = instance_.sendto(client_fd_,
                                        msg_.data(),
                                        0U,
                                        Socket::MessageFlag::kNone,
                                        reinterpret_cast<sockaddr*>(&server_addr_),
                                        sizeof(server_addr_));
    ASSERT_TRUE(send_result.has_value());
    EXPECT_EQ(send_result.value(), 0);
}

TEST_F(SocketTestFixture, RecvFromZeroLengthBuffer)
{
    TestSocket(Socket::Domain::kIPv4, SOCK_DGRAM);
    InitAddr();
    TestBind();

    TestClientSocket(Socket::Domain::kIPv4, SOCK_DGRAM);
    InitServerAddr();
    auto send_result = instance_.sendto(client_fd_,
                                        msg_.data(),
                                        msg_.size(),
                                        Socket::MessageFlag::kNone,
                                        reinterpret_cast<sockaddr*>(&server_addr_),
                                        sizeof(server_addr_));
    ASSERT_TRUE(send_result.has_value());
    EXPECT_EQ(send_result.value(), msg_.size());

    sockaddr_in sender_addr{};
    socklen_t sender_addr_len{sizeof(sender_addr)};
    auto receive_result = instance_.recvfrom(server_fd_,
                                             recvbuf_.data(),
                                             0,
                                             Socket::MessageFlag::kNone,
                                             reinterpret_cast<sockaddr*>(&sender_addr),
                                             &sender_addr_len);
    ASSERT_TRUE(receive_result.has_value());
    EXPECT_EQ(receive_result.value(), 0);
}

TEST_F(SocketTestFixture, RecvFromClosedSocket)
{
    TestSocket(Socket::Domain::kIPv4, SOCK_DGRAM);
    InitAddr();
    TestBind();

    TestClientSocket(Socket::Domain::kIPv4, SOCK_DGRAM);
    InitServerAddr();
    auto send_result = instance_.sendto(client_fd_,
                                        msg_.data(),
                                        msg_.size(),
                                        Socket::MessageFlag::kNone,
                                        reinterpret_cast<sockaddr*>(&server_addr_),
                                        sizeof(server_addr_));
    ASSERT_TRUE(send_result.has_value());
    EXPECT_EQ(send_result.value(), msg_.size());

    unistd_.close(server_fd_);
    sockaddr_in sender_addr{};
    socklen_t sender_addr_len{sizeof(sender_addr)};
    auto receive_result = instance_.recvfrom(server_fd_,
                                             recvbuf_.data(),
                                             0,
                                             Socket::MessageFlag::kNone,
                                             reinterpret_cast<sockaddr*>(&sender_addr),
                                             &sender_addr_len);
    ASSERT_FALSE(receive_result.has_value());
}

TEST_F(SocketTestFixture, SendToInvalidFd)
{
    const auto INVALID_FD = -1;
    TestClientSocket(Socket::Domain::kIPv4, SOCK_DGRAM);
    InitServerAddr("255.255.255.255", kDefaultPortForTesting);
    auto send_result = instance_.sendto(INVALID_FD,
                                        msg_.data(),
                                        msg_.size(),
                                        Socket::MessageFlag::kNone,
                                        reinterpret_cast<sockaddr*>(&server_addr_),
                                        sizeof(server_addr_));
    ASSERT_FALSE(send_result.has_value());
}

TEST_F(SocketTestFixture, TestSendMsgAndRecvMsg)
{
    CreateServerAndClientSockets();

    msghdr msg_to_send{};
    iovec iov_to_send{};
    iov_to_send.iov_base = msg_.data();
    iov_to_send.iov_len = msg_.size();
    msg_to_send.msg_iov = &iov_to_send;
    msg_to_send.msg_iovlen = 1;

    auto result = instance_.sendmsg(client_fd_, &msg_to_send, Socket::MessageFlag::kNone);
    ASSERT_TRUE(result.has_value()) << "Failed to call ::sendmsg(): " << strerror(errno);
    EXPECT_EQ(result.value(), msg_.size());

    msghdr msg{};
    iovec iov{};
    iov.iov_base = recvbuf_.data();
    iov.iov_len = recvbuf_.size();
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    result = instance_.recvmsg(accepted_fd_, &msg, Socket::MessageFlag::kNone);
    ASSERT_TRUE(result.has_value()) << "Failed to call ::recvmsg(): " << strerror(errno);
    EXPECT_EQ(result.value(), msg_.size());
}

TEST_F(SocketTestFixture, TestSendMsgToClosedConnection)
{
    CreateServerAndClientSockets();
    unistd_.close(client_fd_);

    msghdr msg{};
    iovec iov{};
    iov.iov_base = msg_.data();
    iov.iov_len = msg_.size();
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    auto result = instance_.sendmsg(client_fd_, &msg, Socket::MessageFlag::kNone);
    ASSERT_FALSE(result.has_value());
}

TEST_F(SocketTestFixture, TestSendmmsgToClosedConnection)
{
    CreateServerAndClientSockets();
    unistd_.close(client_fd_);

    iovec iov{};
    iov.iov_base = msg_.data();
    iov.iov_len = msg_.size();
    std::array<mmsghdr, 1> msgvec{{{{}, 0}}};
    msgvec[0].msg_hdr.msg_iov = &iov;
    msgvec[0].msg_hdr.msg_iovlen = 1;

    auto result = instance_.sendmmsg(client_fd_, msgvec.data(), 1, Socket::MessageFlag::kNone);
    ASSERT_FALSE(result.has_value());
}

TEST_F(SocketTestFixture, TestRecvMsgFromClosedSocket)
{
    CreateServerAndClientSockets();
    unistd_.close(accepted_fd_);

    msghdr msg{};
    iovec iov{};
    iov.iov_base = recvbuf_.data();
    iov.iov_len = recvbuf_.size();
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    auto result = instance_.recvmsg(accepted_fd_, &msg, Socket::MessageFlag::kNone);
    ASSERT_FALSE(result.has_value());
}

TEST_F(SocketTestFixture, TestRecvMmsgFromClosedSocket)
{
    CreateServerAndClientSockets();
    unistd_.close(accepted_fd_);

    iovec iov{};
    iov.iov_base = recvbuf_.data();
    iov.iov_len = recvbuf_.size();
    std::array<mmsghdr, 1> msgvec{{{{}, 0}}};
    msgvec[0].msg_hdr.msg_iov = &iov;
    msgvec[0].msg_hdr.msg_iovlen = 1;

    auto result = instance_.recvmmsg(accepted_fd_, msgvec.data(), 1, Socket::MessageFlag::kNone, nullptr);
    ASSERT_FALSE(result.has_value());
}

struct SocketTypeTestFixture : public SocketTestFixture, public ::testing::WithParamInterface<Socket::Domain>
{
};
INSTANTIATE_TEST_SUITE_P(SocketDomains,
                         SocketTypeTestFixture,
                         ::testing::Values(Socket::Domain::kIPv4, Socket::Domain::kIPv6, Socket::Domain::kUnix));
TEST_P(SocketTypeTestFixture, CreateSocket)
{
    TestSocket(GetParam());
}

TEST_F(SocketTestFixture, CreateAfRouteSocketShouldFail)
{
    // QNX does not support AF_ROUTE
    auto result = instance_.socket(Socket::Domain::kRoute, SOCK_STREAM, 0);
    ASSERT_FALSE(result.has_value());
}

void* global_instance_{nullptr};
TEST_F(SocketTestFixture, CreateSocketWithInvalidDomain)
{
    global_instance_ = this;
    std::set_terminate([]() {
        SocketTestFixture_CreateSocketWithInvalidDomain_Test* ptr =
            reinterpret_cast<SocketTestFixture_CreateSocketWithInvalidDomain_Test*>(global_instance_);
        ptr->~SocketTestFixture_CreateSocketWithInvalidDomain_Test();
        std::exit(1);
    });
    EXPECT_DEATH(instance_.socket(Socket::Domain::kInvalid, SOCK_STREAM, 0), ".*");
}

TEST_F(SocketTestFixture, CreateSocketWithDefaultDomain)
{
    global_instance_ = this;
    std::set_terminate([]() {
        SocketTestFixture_CreateSocketWithDefaultDomain_Test* ptr =
            reinterpret_cast<SocketTestFixture_CreateSocketWithDefaultDomain_Test*>(global_instance_);
        ptr->~SocketTestFixture_CreateSocketWithDefaultDomain_Test();
        std::exit(1);
    });
    EXPECT_DEATH(instance_.socket(Socket::Domain{876}, SOCK_STREAM, 0), ".*");
}

struct SocketTestFlagFixture : public SocketTestFixture, public ::testing::WithParamInterface<Socket::MessageFlag>
{
};
INSTANTIATE_TEST_SUITE_P(SocketFlags,
                         SocketTestFlagFixture,
                         ::testing::Values(Socket::MessageFlag::kPeek,
                                           Socket::MessageFlag::kWaitAll,
                                           Socket::MessageFlag::kOutOfBand,
                                           Socket::MessageFlag::kDONTUseGateway,
                                           Socket::MessageFlag::kNoSignal,
                                           Socket::MessageFlag::kTerminateRecord,
                                           Socket::MessageFlag::kWaitForOne));

TEST_P(SocketTestFlagFixture, TestSendWithFlags)
{
    CreateServerAndClientSockets();

    auto flag = GetParam();
    auto result = instance_.send(client_fd_, msg_.data(), msg_.size(), flag);
#if defined(SPP_OS_QNX8)
    // MSG_EOR (kTerminateRecord) is only supported with SOCK_SEQPACKET on QNX 8
    // (FreeBSD-based io-sock), not with SOCK_STREAM - so expect failure and return early
    if (flag == Socket::MessageFlag::kTerminateRecord)
    {
        ASSERT_FALSE(result.has_value()) << "MSG_EOR should not be supported with SOCK_STREAM on QNX 8";
        return;
    }
#endif
    ASSERT_TRUE(result.has_value()) << "Failed to call ::send(): " << strerror(errno);
    EXPECT_EQ(result.value(), msg_.size());
}

TEST_F(SocketTestFixture, TestSendmmsgAndRecvmmsg)
{
    TestSocket(Socket::Domain::kIPv4, SOCK_DGRAM);
    InitAddr();
    TestBind();
    TestClientSocket(Socket::Domain::kIPv4, SOCK_DGRAM);
    InitServerAddr();

    constexpr size_t num_messages = 3;
    std::array<std::string, num_messages> messages{{"Hello", "World", "BMW!"}};

    std::array<mmsghdr, num_messages> msgs{};
    std::array<iovec, num_messages> iov{};
    for (size_t i = 0; i < num_messages; ++i)
    {
        iov[i].iov_base = reinterpret_cast<void*>(messages[i].data());
        iov[i].iov_len = messages[i].size();

        msgs[i].msg_hdr.msg_name = &server_addr_;
        msgs[i].msg_hdr.msg_namelen = sizeof(server_addr_);
        msgs[i].msg_hdr.msg_iov = &iov[i];
        msgs[i].msg_hdr.msg_iovlen = 1;
        msgs[i].msg_hdr.msg_control = nullptr;
        msgs[i].msg_hdr.msg_controllen = 0;
        msgs[i].msg_hdr.msg_flags = 0;
    }

    // Send messages
    auto send_result = instance_.sendmmsg(client_fd_, msgs.data(), num_messages, Socket::MessageFlag::kNone);
    ASSERT_TRUE(send_result.has_value()) << "Failed to send messages: " << send_result.error().ToString();
    ASSERT_EQ(send_result.value(), num_messages);

    struct mmsghdr recv_msgs[num_messages];
    struct iovec recv_iov[num_messages];
    char recv_buffers[num_messages][1024];
    for (size_t i = 0; i < num_messages; ++i)
    {
        recv_iov[i].iov_base = recv_buffers[i];
        recv_iov[i].iov_len = sizeof(recv_buffers[i]);

        recv_msgs[i].msg_hdr.msg_name = nullptr;
        recv_msgs[i].msg_hdr.msg_namelen = 0;
        recv_msgs[i].msg_hdr.msg_iov = &recv_iov[i];
        recv_msgs[i].msg_hdr.msg_iovlen = 1;
        recv_msgs[i].msg_hdr.msg_control = nullptr;
        recv_msgs[i].msg_hdr.msg_controllen = 0;
        recv_msgs[i].msg_hdr.msg_flags = 0;
    }

    auto recv_result = instance_.recvmmsg(server_fd_, recv_msgs, num_messages, Socket::MessageFlag::kNone, nullptr);
    ASSERT_TRUE(recv_result.has_value()) << "Failed to receive messages: " << recv_result.error().ToString();
    ASSERT_EQ(recv_result.value(), num_messages);
    for (size_t i = 0; i < num_messages; ++i)
    {
        std::string received_message(recv_buffers[i], static_cast<size_t>(recv_msgs[i].msg_len));
        EXPECT_EQ(received_message, messages[i]);
    }
}

// Test sending zero messages
TEST_F(SocketTestFixture, SendZeroMessages)
{
    TestClientSocket(Socket::Domain::kIPv4, SOCK_DGRAM);
    InitServerAddr();
    struct mmsghdr msgs[1];
    auto send_result = instance_.sendmmsg(client_fd_, msgs, 0, Socket::MessageFlag::kNone);
#if defined(SPP_OS_QNX8)
    // sendmmsg with vlen=0 is not supported on QNX 8
    EXPECT_FALSE(send_result.has_value()) << "sendmmsg with zero messages should fail on QNX 8";
#else
    EXPECT_TRUE(send_result.has_value()) << "sendmmsg failed to send zero messages";
#endif
}

// Test receiving zero messages
TEST_F(SocketTestFixture, ReceiveZeroMessages)
{
    TestSocket(Socket::Domain::kIPv4, SOCK_DGRAM);
    InitAddr();
    TestBind();
    struct mmsghdr recv_msgs[1];
    auto recv_result = instance_.recvmmsg(accepted_fd_, recv_msgs, 0, Socket::MessageFlag::kNone, nullptr);
#if defined(__linux__)
    EXPECT_FALSE(recv_result.has_value()) << "recvmmsg should not receive zero messages in Linux";
#elif defined(__QNX__)
    EXPECT_TRUE(recv_result.has_value()) << "recvmmsg should receive zero messages in QNX";
#endif
}

TEST_F(SocketTestFixture, TestAcceptFailure)
{
    CreateServerAndClientSockets();
    unistd_.close(server_fd_);

    unsigned int sock_size = sizeof(sockaddr_in) - 4;
    auto accept_result = instance_.accept(server_fd_, reinterpret_cast<sockaddr*>(&client_addr_), &sock_size);
    server_fd_ = -1;
    ASSERT_FALSE(accept_result.has_value());
}

}  // namespace
}  // namespace os
}  // namespace score
