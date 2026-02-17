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
#include "score/network/netutils.h"

#include "score/os/mocklib/ifaddrs_mock.h"
#include "score/os/mocklib/ioctl_mock.h"
#include "score/os/mocklib/socketmock.h"
#include "score/os/mocklib/unistdmock.h"

#include "gtest/gtest.h"

#include <ifaddrs.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>

namespace score
{
namespace os
{
namespace
{

const auto kEAcessUnExpectedError = score::cpp::make_unexpected(score::os::Error::createFromErrno(EACCES));
constexpr std::uint16_t NETLINK_BUFFER_SIZE = 8192U;

using ::testing::_;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::Matcher;
using ::testing::Return;
using ::testing::SaveArg;
using ::testing::WithArg;

struct NetUtilsLinuxTestFixture : ::testing::Test
{
  public:
    void SetUp() override
    {
        score::os::Socket::set_testing_instance(socket_mock_);
        score::os::Ioctl::set_testing_instance(ioctl_mock_);
        score::os::Unistd::set_testing_instance(unistd_mock_);
        score::os::Ifaddrs::set_testing_instance(ifaddrs_mock_);
    };

  protected:
    score::os::SocketMock socket_mock_;
    score::os::IoctlMock ioctl_mock_;
    score::os::UnistdMock unistd_mock_;
    score::os::IfaddrsMock ifaddrs_mock_;
};

TEST_F(NetUtilsLinuxTestFixture, GetMacAddress_SocketFail)
{
    EXPECT_CALL(socket_mock_, socket(_, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno())));
    EXPECT_FALSE(score::os::Netutils::instance().get_mac_address("eth0").has_value());
}

TEST_F(NetUtilsLinuxTestFixture, GetMacAddress_IoctlFail)
{
    EXPECT_CALL(socket_mock_, socket(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(ioctl_mock_, ioctl(_, _, Matcher<void*>(_)))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno())));
    EXPECT_CALL(unistd_mock_, close(_)).WillOnce(Return(score::cpp::expected_blank<score::os::Error>{}));
    EXPECT_FALSE(score::os::Netutils::instance().get_mac_address("eth0").has_value());
}

TEST_F(NetUtilsLinuxTestFixture, GetMacAddress_CloseFail)
{
    EXPECT_CALL(socket_mock_, socket(_, _, _)).WillOnce(Return(1));
    EXPECT_CALL(ioctl_mock_, ioctl(_, _, Matcher<void*>(_))).WillOnce(Return(score::cpp::expected_blank<score::os::Error>{}));
    EXPECT_CALL(unistd_mock_, close(_)).WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno())));
    EXPECT_FALSE(score::os::Netutils::instance().get_mac_address("eth0").has_value());
}

TEST_F(NetUtilsLinuxTestFixture, GetMacAddress_success)
{
    EXPECT_CALL(socket_mock_, socket(_, _, _)).WillOnce(Return(1));
    EXPECT_CALL(ioctl_mock_, ioctl(_, _, Matcher<void*>(_))).WillOnce(Return(score::cpp::expected_blank<score::os::Error>{}));
    EXPECT_CALL(unistd_mock_, close(_)).WillOnce(Return(score::cpp::expected_blank<score::os::Error>{}));
    EXPECT_TRUE(score::os::Netutils::instance().get_mac_address("eth0").has_value());
}

TEST_F(NetUtilsLinuxTestFixture, GetDefaultGatewayIp4_SocketCreationFails)
{
    EXPECT_CALL(socket_mock_, socket(score::os::Socket::Domain::kNetlink, SOCK_RAW, NETLINK_ROUTE))
        .WillOnce(Return(kEAcessUnExpectedError));
    const auto result = score::os::Netutils::instance().get_default_gateway_ip4();
    EXPECT_FALSE(result.has_value());
}

TEST_F(NetUtilsLinuxTestFixture, GetDefaultGatewayIp4_SendMessageFails)
{
    std::int32_t sock_fd{1};
    pid_t process_id{2};
    const void* captured_ptr = nullptr;
    nlmsghdr netlink_message = {};
    netlink_message.nlmsg_len = NLMSG_LENGTH(sizeof(netlink_message));
    netlink_message.nlmsg_type = RTM_GETROUTE;
    netlink_message.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    netlink_message.nlmsg_pid = process_id;

    EXPECT_CALL(socket_mock_, socket(score::os::Socket::Domain::kNetlink, SOCK_RAW, NETLINK_ROUTE))
        .WillOnce(Return(sock_fd));
    EXPECT_CALL(unistd_mock_, getpid).WillOnce(Return(process_id));
    EXPECT_CALL(socket_mock_,
                send(sock_fd, _, NLMSG_LENGTH(sizeof(netlink_message)), score::os::Socket::MessageFlag::kNone))
        .WillOnce(DoAll(SaveArg<1>(&captured_ptr), Return(kEAcessUnExpectedError)));
    EXPECT_CALL(unistd_mock_, close(sock_fd)).WillOnce(Return(score::cpp::blank{}));

    const auto result = score::os::Netutils::instance().get_default_gateway_ip4();
    EXPECT_FALSE(result.has_value());
}

TEST_F(NetUtilsLinuxTestFixture, GetDefaultGatewayIp4_RecvMessageFails)
{
    std::int32_t sock_fd{1};
    pid_t process_id{2};
    nlmsghdr netlink_message = {};

    EXPECT_CALL(socket_mock_, socket(score::os::Socket::Domain::kNetlink, SOCK_RAW, NETLINK_ROUTE))
        .WillOnce(Return(sock_fd));
    EXPECT_CALL(unistd_mock_, getpid).WillOnce(Return(process_id));
    EXPECT_CALL(socket_mock_,
                send(sock_fd, _, NLMSG_LENGTH(sizeof(netlink_message)), score::os::Socket::MessageFlag::kNone))
        .WillOnce(Return(NLMSG_LENGTH(sizeof(netlink_message))));
    EXPECT_CALL(socket_mock_, recv(sock_fd, _, NETLINK_BUFFER_SIZE, score::os::Socket::MessageFlag::kNone))
        .WillOnce(Return(kEAcessUnExpectedError));
    EXPECT_CALL(unistd_mock_, close(sock_fd)).WillOnce(Return(score::cpp::blank{}));

    const auto result = score::os::Netutils::instance().get_default_gateway_ip4();
    EXPECT_FALSE(result.has_value());
}

TEST_F(NetUtilsLinuxTestFixture, GetDefaultGatewayIp4_CloseSocketFails)
{
    std::int32_t sock_fd{1};
    pid_t process_id{2};
    nlmsghdr netlink_message = {};
    char expected_buf[] = "test_data";

    EXPECT_CALL(socket_mock_, socket(score::os::Socket::Domain::kNetlink, SOCK_RAW, NETLINK_ROUTE))
        .WillOnce(Return(sock_fd));
    EXPECT_CALL(unistd_mock_, getpid).WillOnce(Return(process_id));
    EXPECT_CALL(socket_mock_,
                send(sock_fd, _, NLMSG_LENGTH(sizeof(netlink_message)), score::os::Socket::MessageFlag::kNone))
        .WillOnce(Return(NLMSG_LENGTH(sizeof(netlink_message))));
    EXPECT_CALL(socket_mock_, recv(_, _, _, _))
        .WillOnce(DoAll(WithArg<1>([&](void* buf) {
                            std::memcpy(buf, expected_buf, sizeof(expected_buf));
                        }),
                        Return(sizeof(expected_buf))));
    EXPECT_CALL(unistd_mock_, close(sock_fd)).WillOnce(Return(kEAcessUnExpectedError));

    const auto result = score::os::Netutils::instance().get_default_gateway_ip4();
    EXPECT_FALSE(result.has_value());
}

TEST_F(NetUtilsLinuxTestFixture, GetDefaultGatewayIp4_ReceivedInvalidNetlinkMsg)
{
    std::int32_t sock_fd{1};
    pid_t process_id{2};
    nlmsghdr netlink_message = {};
    char expected_buf[] = "test_data";

    EXPECT_CALL(socket_mock_, socket(score::os::Socket::Domain::kNetlink, SOCK_RAW, NETLINK_ROUTE))
        .WillOnce(Return(sock_fd));
    EXPECT_CALL(unistd_mock_, getpid).WillOnce(Return(process_id));
    EXPECT_CALL(socket_mock_,
                send(sock_fd, _, NLMSG_LENGTH(sizeof(netlink_message)), score::os::Socket::MessageFlag::kNone))
        .WillOnce(Return(NLMSG_LENGTH(sizeof(netlink_message))));
    EXPECT_CALL(socket_mock_, recv(_, _, _, _))
        .WillOnce(DoAll(WithArg<1>([&](void* buf) {
                            std::memcpy(buf, expected_buf, sizeof(expected_buf));
                        }),
                        Return(sizeof(expected_buf))));
    EXPECT_CALL(unistd_mock_, close(sock_fd)).WillOnce(Return(score::cpp::blank{}));

    const auto result = score::os::Netutils::instance().get_default_gateway_ip4();
    EXPECT_FALSE(result.has_value());
}

TEST_F(NetUtilsLinuxTestFixture, GetDefaultGatewayIp4_ReceivedMessageSizeIsWrong)
{
    std::int32_t sock_fd{1};
    pid_t process_id{2};
    nlmsghdr netlink_message = {};
    netlink_message.nlmsg_len = NLMSG_LENGTH(sizeof(netlink_message));
    netlink_message.nlmsg_type = NLMSG_ERROR;
    netlink_message.nlmsg_flags = NLM_F_MULTI;
    netlink_message.nlmsg_pid = process_id;

    EXPECT_CALL(socket_mock_, socket(score::os::Socket::Domain::kNetlink, SOCK_RAW, NETLINK_ROUTE))
        .WillOnce(Return(sock_fd));
    EXPECT_CALL(unistd_mock_, getpid).WillOnce(Return(process_id));
    EXPECT_CALL(socket_mock_,
                send(sock_fd, _, NLMSG_LENGTH(sizeof(netlink_message)), score::os::Socket::MessageFlag::kNone))
        .WillOnce(Return(NLMSG_LENGTH(sizeof(netlink_message))));
    EXPECT_CALL(socket_mock_, recv(_, _, _, _))
        .WillOnce(DoAll(WithArg<1>([&](void* buf) {
                            std::memcpy(buf, &netlink_message, sizeof(netlink_message));
                        }),
                        Return(1U)));
    EXPECT_CALL(unistd_mock_, close(sock_fd)).WillOnce(Return(score::cpp::blank{}));

    const auto result = score::os::Netutils::instance().get_default_gateway_ip4();
    EXPECT_FALSE(result.has_value());
}

TEST_F(NetUtilsLinuxTestFixture, GetDefaultGatewayIp4_ReceivedErrorMessageType)
{
    std::int32_t sock_fd{1};
    pid_t process_id{2};
    nlmsghdr netlink_message = {};
    netlink_message.nlmsg_len = NLMSG_LENGTH(sizeof(netlink_message));
    netlink_message.nlmsg_type = NLMSG_ERROR;
    netlink_message.nlmsg_flags = NLM_F_MULTI;
    netlink_message.nlmsg_pid = process_id;

    EXPECT_CALL(socket_mock_, socket(score::os::Socket::Domain::kNetlink, SOCK_RAW, NETLINK_ROUTE))
        .WillOnce(Return(sock_fd));
    EXPECT_CALL(unistd_mock_, getpid).WillOnce(Return(process_id));
    EXPECT_CALL(socket_mock_,
                send(sock_fd, _, NLMSG_LENGTH(sizeof(netlink_message)), score::os::Socket::MessageFlag::kNone))
        .WillOnce(Return(NLMSG_LENGTH(sizeof(netlink_message))));
    EXPECT_CALL(socket_mock_, recv(_, _, _, _))
        .WillOnce(DoAll(WithArg<1>([&](void* buf) {
                            std::memcpy(buf, &netlink_message, sizeof(netlink_message));
                        }),
                        Return(NLMSG_LENGTH(sizeof(netlink_message)))));
    EXPECT_CALL(unistd_mock_, close(sock_fd)).WillOnce(Return(score::cpp::blank{}));

    const auto result = score::os::Netutils::instance().get_default_gateway_ip4();
    EXPECT_FALSE(result.has_value());
}

TEST_F(NetUtilsLinuxTestFixture, GetDefaultGatewayIp4_MessagePayloadIsEmpty)
{
    std::int32_t sock_fd{1};
    pid_t process_id{2};
    nlmsghdr netlink_message = {};
    netlink_message.nlmsg_len = NLMSG_LENGTH(sizeof(netlink_message));
    netlink_message.nlmsg_type = RTM_GETROUTE;
    netlink_message.nlmsg_flags = NLM_F_MULTI;
    netlink_message.nlmsg_pid = process_id;

    EXPECT_CALL(socket_mock_, socket(score::os::Socket::Domain::kNetlink, SOCK_RAW, NETLINK_ROUTE))
        .WillOnce(Return(sock_fd));
    EXPECT_CALL(unistd_mock_, getpid).WillOnce(Return(process_id));
    EXPECT_CALL(socket_mock_,
                send(sock_fd, _, NLMSG_LENGTH(sizeof(netlink_message)), score::os::Socket::MessageFlag::kNone))
        .WillOnce(Return(NLMSG_LENGTH(sizeof(netlink_message))));
    EXPECT_CALL(socket_mock_, recv(_, _, _, _))
        .WillOnce(DoAll(WithArg<1>([&](void* buf) {
                            std::memcpy(buf, &netlink_message, sizeof(netlink_message));
                        }),
                        Return(NLMSG_LENGTH(sizeof(netlink_message)))));
    EXPECT_CALL(unistd_mock_, close(sock_fd)).WillOnce(Return(score::cpp::blank{}));

    const auto result = score::os::Netutils::instance().get_default_gateway_ip4();
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 0);
}

TEST_F(NetUtilsLinuxTestFixture, SetAliasIpAddress_ReturnsError)
{
    const auto result = score::os::Netutils::instance().set_alias_ip_address("", {});
    EXPECT_FALSE(result.has_value());
}

}  // namespace
}  // namespace os
}  // namespace score
