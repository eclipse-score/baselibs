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

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/route.h>
#include <sys/ioctl.h>

namespace score
{
namespace os
{
namespace
{

const auto kEAcessUnExpectedError = score::cpp::make_unexpected(score::os::Error::createFromErrno(EACCES));

using ::testing::_;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::Matcher;
using ::testing::Return;
using ::testing::WithArgs;

struct routing_table
{
    struct rt_msghdr header;
    struct sockaddr_in dst;
    struct sockaddr_in gw;
    struct sockaddr_in mask;
};

struct NetUtilsQnxTestFixture : ::testing::Test
{
  public:
    void SetUp() override
    {
        score::os::Socket::set_testing_instance(socket_mock_);
        score::os::Ioctl::set_testing_instance(ioctl_mock_);
        score::os::Unistd::set_testing_instance(unistd_mock_);
        score::os::Ifaddrs::set_testing_instance(ifaddrs_mock_);
    };

    void TearDown() override
    {
        score::os::Socket::restore_instance();
        score::os::Ioctl::restore_instance();
        score::os::Unistd::restore_instance();
        score::os::Ifaddrs::restore_instance();
    };

  protected:
    constexpr static std::uint16_t SEQ = 1234U;
    constexpr static pid_t pid = 5345U;
    score::os::SocketMock socket_mock_;
    score::os::IoctlMock ioctl_mock_;
    score::os::UnistdMock unistd_mock_;
    score::os::IfaddrsMock ifaddrs_mock_;
};

TEST_F(NetUtilsQnxTestFixture, GetMacAddress_GetifaddrsFail)
{
    EXPECT_CALL(ifaddrs_mock_, getifaddrs()).WillOnce(Return(kEAcessUnExpectedError));
    EXPECT_FALSE(score::os::Netutils::instance().get_mac_address("eth0").has_value());
}

TEST_F(NetUtilsQnxTestFixture, GetMacAddress_GetifaddrsReturnNull)
{
    EXPECT_CALL(ifaddrs_mock_, getifaddrs()).WillOnce(Return(nullptr));
    EXPECT_CALL(ifaddrs_mock_, freeifaddrs(_)).Times(0);
    EXPECT_FALSE(score::os::Netutils::instance().get_mac_address("eth0").has_value());
}

TEST_F(NetUtilsQnxTestFixture, GetMacAddress_NoEthernetInterfaceExitsOnTheMachine)
{
    struct ifaddrs ifa1 = {};

    ifa1.ifa_name = strdup("eth0");
    ifa1.ifa_flags = IFF_UP;
    ifa1.ifa_next = nullptr;

    EXPECT_CALL(ifaddrs_mock_, getifaddrs()).WillOnce(Return(&ifa1));
    EXPECT_CALL(ifaddrs_mock_, freeifaddrs(&ifa1));
    EXPECT_FALSE(score::os::Netutils::instance().get_mac_address("eth0").has_value());
}

TEST_F(NetUtilsQnxTestFixture, GetMacAddress_AddressFamilyIsNotLinkLayerAddresses)
{
    struct ifaddrs ifa1 = {0};
    struct ifaddrs ifa2 = {0};
    struct sockaddr_in ipv4_addr = {0};

    ifa1.ifa_name = strdup("eth0");
    ifa1.ifa_flags = IFF_UP;
    ifa1.ifa_next = &ifa2;

    ifa2.ifa_name = strdup("wlan0");
    ifa2.ifa_flags = IFF_UP | IFF_RUNNING;
    ifa2.ifa_next = nullptr;

    ipv4_addr.sin_family = AF_INET;
    ifa2.ifa_addr = (struct sockaddr*)&ipv4_addr;

    EXPECT_CALL(ifaddrs_mock_, getifaddrs()).WillOnce(Return(&ifa1));
    EXPECT_CALL(ifaddrs_mock_, freeifaddrs(&ifa1));
    EXPECT_FALSE(score::os::Netutils::instance().get_mac_address("eth0").has_value());
}

TEST_F(NetUtilsQnxTestFixture, GetMacAddress_CouldNotFindTheSpecificEthernetInterface)
{
    struct ifaddrs ifa1 = {0};
    struct sockaddr_dl eth1_addr = {0};
    const auto mac_address = std::array<std::uint8_t, 6U>{0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E};

    ifa1.ifa_name = strdup("eth1");
    ifa1.ifa_flags = IFF_UP | IFF_RUNNING;
    ifa1.ifa_next = nullptr;

    eth1_addr.sdl_family = AF_LINK;
    eth1_addr.sdl_len = sizeof(struct sockaddr_dl);
    eth1_addr.sdl_alen = 6;
    eth1_addr.sdl_nlen = 4;
    memcpy(eth1_addr.sdl_data, "eth1", 4);
    memcpy(eth1_addr.sdl_data + eth1_addr.sdl_nlen, mac_address.data(), mac_address.size());
    ifa1.ifa_addr = (struct sockaddr*)&eth1_addr;

    EXPECT_CALL(ifaddrs_mock_, getifaddrs()).WillOnce(Return(&ifa1));
    EXPECT_CALL(ifaddrs_mock_, freeifaddrs(&ifa1));
    EXPECT_FALSE(score::os::Netutils::instance().get_mac_address("eth0").has_value());
}

TEST_F(NetUtilsQnxTestFixture, GetMacAddress_ReturnsCorrectMacAddress)
{
    struct ifaddrs ifa1 = {0};
    struct ifaddrs ifa2 = {0};
    struct sockaddr_dl eth0_addr = {0};
    struct sockaddr_in ipv4_addr = {0};
    const auto expected_mac_address = std::array<std::uint8_t, 6U>{0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E};

    ifa1.ifa_name = strdup("wlan0");
    ifa1.ifa_flags = IFF_UP;
    ifa1.ifa_next = &ifa2;

    ipv4_addr.sin_family = AF_INET;
    ifa1.ifa_addr = (struct sockaddr*)&ipv4_addr;

    ifa2.ifa_name = strdup("eth0");
    ifa2.ifa_flags = IFF_UP | IFF_RUNNING;
    ifa2.ifa_next = nullptr;
    eth0_addr.sdl_family = AF_LINK;
    eth0_addr.sdl_len = sizeof(struct sockaddr_dl);
    eth0_addr.sdl_alen = 6;
    eth0_addr.sdl_nlen = 4;
    memcpy(eth0_addr.sdl_data, "eth0", 4);
    memcpy(eth0_addr.sdl_data + eth0_addr.sdl_nlen, expected_mac_address.data(), expected_mac_address.size());
    ifa2.ifa_addr = (struct sockaddr*)&eth0_addr;

    EXPECT_CALL(ifaddrs_mock_, getifaddrs()).WillOnce(Return(&ifa1));
    EXPECT_CALL(ifaddrs_mock_, freeifaddrs(&ifa1));
    const auto result = score::os::Netutils::instance().get_mac_address("eth0");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), expected_mac_address);
}

TEST_F(NetUtilsQnxTestFixture, GetDefaultGatewayIp4_SocketCreationFailure)
{
    EXPECT_CALL(socket_mock_, socket(score::os::Socket::Domain::kRoute, SOCK_RAW, 0))
        .WillOnce(Return(kEAcessUnExpectedError));
    const auto result = score::os::Netutils::instance().get_default_gateway_ip4();
    EXPECT_FALSE(result.has_value());
}

TEST_F(NetUtilsQnxTestFixture, GetDefaultGatewayIp4_SocketWriteFailure)
{
    std::int32_t sock_fd{1};
    EXPECT_CALL(socket_mock_, socket(Socket::Domain::kRoute, SOCK_RAW, 0)).WillOnce(Return(sock_fd));
    EXPECT_CALL(unistd_mock_, write(sock_fd, _, _)).WillOnce(Return(kEAcessUnExpectedError));
    const auto result = score::os::Netutils::instance().get_default_gateway_ip4();
    EXPECT_FALSE(result.has_value());
}

TEST_F(NetUtilsQnxTestFixture, GetDefaultGatewayIp4_SocketReadFailure)
{
    std::int32_t sock_fd{1};
    const auto expected_size = sizeof(struct rt_msghdr) + sizeof(struct sockaddr_in);

    EXPECT_CALL(unistd_mock_, getpid()).WillOnce(Return(pid));
    EXPECT_CALL(socket_mock_, socket(Socket::Domain::kRoute, SOCK_RAW, 0)).WillOnce(Return(sock_fd));
    EXPECT_CALL(unistd_mock_, write(sock_fd, _, _))
        .WillOnce([expected_size, pid_ = pid](const std::int32_t,
                                              const void* buf,
                                              const size_t size) -> score::cpp::expected<ssize_t, score::os::Error> {
            const auto* r_table = static_cast<const routing_table*>(buf);
            EXPECT_EQ(r_table->header.rtm_type, RTM_GET);
            EXPECT_EQ(r_table->header.rtm_pid, pid);
            EXPECT_EQ(r_table->header.rtm_flags, RTF_UP | RTF_GATEWAY);
            EXPECT_EQ(r_table->header.rtm_addrs, RTA_DST | RTA_NETMASK);
            EXPECT_EQ(r_table->header.rtm_seq, SEQ);

            EXPECT_EQ(r_table->dst.sin_family, AF_INET);
            EXPECT_EQ(r_table->gw.sin_family, AF_INET);
            EXPECT_EQ(r_table->mask.sin_family, AF_INET);
            EXPECT_EQ(size, expected_size);
            return 6U;
        });

    EXPECT_CALL(unistd_mock_, read(sock_fd, _, _)).WillOnce(Return(kEAcessUnExpectedError));
    const auto result = score::os::Netutils::instance().get_default_gateway_ip4();
    EXPECT_FALSE(result.has_value());
}

TEST_F(NetUtilsQnxTestFixture, GetDefaultGatewayIp4_SocketReadRTMGetCommandFailed)
{
    std::int32_t sock_fd{1};
    routing_table r_table;
    r_table.header.rtm_type = RTM_GET;
    r_table.header.rtm_seq = SEQ;
    r_table.header.rtm_pid = pid;
    r_table.header.rtm_errno = 5;

    EXPECT_CALL(unistd_mock_, getpid()).WillOnce(Return(pid));
    EXPECT_CALL(socket_mock_, socket(Socket::Domain::kRoute, SOCK_RAW, 0)).WillOnce(Return(sock_fd));
    EXPECT_CALL(unistd_mock_, write(sock_fd, _, _)).WillOnce(Return(6U));
    EXPECT_CALL(unistd_mock_, read(sock_fd, _, _))
        .WillOnce(DoAll(WithArgs<1, 2>([&r_table](void* buf, size_t size) {
                            r_table.header.rtm_seq = 3333;
                            std::memcpy(buf, &r_table, sizeof(r_table));
                            EXPECT_EQ(size, sizeof(r_table));
                        }),
                        Return(score::cpp::expected<ssize_t, score::os::Error>(sizeof(r_table)))))
        .WillOnce(DoAll(WithArgs<1, 2>([&r_table](void* buf, size_t size) {
                            r_table.header.rtm_type = RTM_MISS;
                            std::memcpy(buf, &r_table, sizeof(r_table));
                            EXPECT_EQ(size, sizeof(r_table));
                        }),
                        Return(score::cpp::expected<ssize_t, score::os::Error>(sizeof(r_table)))))
        .WillOnce(DoAll(WithArgs<1, 2>([&r_table](void* buf, size_t size) {
                            r_table.header.rtm_pid = 4444U;
                            std::memcpy(buf, &r_table, sizeof(r_table));
                            EXPECT_EQ(size, sizeof(r_table));
                        }),
                        Return(score::cpp::expected<ssize_t, score::os::Error>(sizeof(r_table)))))
        .WillOnce(DoAll(WithArgs<1, 2>([&r_table](void* buf, size_t size) {
                            r_table.header.rtm_type = RTM_GET;
                            r_table.header.rtm_seq = SEQ;
                            r_table.header.rtm_pid = pid;
                            std::memcpy(buf, &r_table, sizeof(r_table));
                            EXPECT_EQ(size, sizeof(r_table));
                        }),
                        Return(score::cpp::expected<ssize_t, score::os::Error>(sizeof(r_table)))));

    const auto result = score::os::Netutils::instance().get_default_gateway_ip4();
    EXPECT_FALSE(result.has_value());
}

TEST_F(NetUtilsQnxTestFixture, GetDefaultGatewayIp4_ReadGatewayAddressSuccess)
{
    std::int32_t sock_fd{1};
    routing_table r_table;
    r_table.header.rtm_type = RTM_GET;
    r_table.header.rtm_seq = SEQ;
    r_table.header.rtm_pid = pid;
    r_table.header.rtm_errno = 0;
    r_table.gw.sin_addr.s_addr = 0x0101A8C0;

    EXPECT_CALL(unistd_mock_, getpid()).WillOnce(Return(pid));
    EXPECT_CALL(socket_mock_, socket(Socket::Domain::kRoute, SOCK_RAW, 0)).WillOnce(Return(sock_fd));
    EXPECT_CALL(unistd_mock_, write(sock_fd, _, _)).WillOnce(Return(6U));
    EXPECT_CALL(unistd_mock_, read(sock_fd, _, _))
        .WillOnce(DoAll(WithArgs<1, 2>([&r_table](void* buf, size_t size) {
                            std::memcpy(buf, &r_table, sizeof(r_table));
                            EXPECT_EQ(size, sizeof(r_table));
                        }),
                        Return(score::cpp::expected<ssize_t, score::os::Error>(sizeof(r_table)))));

    const auto result = score::os::Netutils::instance().get_default_gateway_ip4();
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), r_table.gw.sin_addr.s_addr);
}

TEST_F(NetUtilsQnxTestFixture, SetAliasIpAddress_ReturnsErrorWhenSocketCreationFails)
{
    EXPECT_CALL(socket_mock_, socket(Socket::Domain::kIPv4, SOCK_DGRAM, 0)).WillOnce(Return(kEAcessUnExpectedError));

    const auto result = score::os::Netutils::instance().set_alias_ip_address("", {});
    EXPECT_FALSE(result.has_value());
}

TEST_F(NetUtilsQnxTestFixture, SetAliasIpAddress_ReturnsErrorWhenAddingInterfaceAliasFails)
{
    std::int32_t sock_fd{1};

    EXPECT_CALL(socket_mock_, socket(Socket::Domain::kIPv4, SOCK_DGRAM, 0)).WillOnce(Return(sock_fd));
    EXPECT_CALL(ioctl_mock_, ioctl(sock_fd, SIOCAIFADDR, Matcher<void*>(_))).WillOnce(Return(kEAcessUnExpectedError));

    const auto result = score::os::Netutils::instance().set_alias_ip_address("", {});
    EXPECT_FALSE(result.has_value());
}

TEST_F(NetUtilsQnxTestFixture, SetAliasIpAddress_CloseSocketFailure)
{
    std::int32_t sock_fd{1};

    EXPECT_CALL(socket_mock_, socket(Socket::Domain::kIPv4, SOCK_DGRAM, 0)).WillOnce(Return(sock_fd));
    EXPECT_CALL(ioctl_mock_, ioctl(sock_fd, SIOCAIFADDR, Matcher<void*>(_))).WillOnce(Return(score::cpp::blank{}));
    EXPECT_CALL(unistd_mock_, close(_)).WillOnce(Return(kEAcessUnExpectedError));

    const auto result = score::os::Netutils::instance().set_alias_ip_address("", {});
    EXPECT_TRUE(result.has_value());
}

TEST_F(NetUtilsQnxTestFixture, SetAliasIpAddress_Success)
{
    std::int32_t sock_fd{1};
    const std::string ifc_name = "alis_ip_test";
    const Ipv4Address ip_addr{"10.0.0.1"};
    const auto ip_addr_in_dec = 16777226;  // todo: verify

    EXPECT_CALL(socket_mock_, socket(Socket::Domain::kIPv4, SOCK_DGRAM, 0)).WillOnce(Return(sock_fd));
    EXPECT_CALL(ioctl_mock_, ioctl(sock_fd, SIOCAIFADDR, _))
        .WillOnce([&ifc_name, ip_addr_in_dec](auto, auto, void* arg) {
            const auto* ifar = static_cast<ifaliasreq*>(arg);
            EXPECT_STREQ(ifar->ifra_name, ifc_name.c_str());
            EXPECT_EQ(ifar->ifra_addr.sa_family, AF_INET);

            const auto& addr_in = reinterpret_cast<const struct sockaddr_in*>(&ifar->ifra_addr);
            EXPECT_EQ(addr_in->sin_addr.s_addr, ip_addr_in_dec);
            return score::cpp::blank{};
        });
    EXPECT_CALL(unistd_mock_, close(_)).WillOnce(Return(score::cpp::blank{}));

    const auto result = score::os::Netutils::instance().set_alias_ip_address(ifc_name, ip_addr);
    EXPECT_TRUE(result.has_value());
}

}  // namespace
}  // namespace os
}  // namespace score
