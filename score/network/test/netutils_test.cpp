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

using ::testing::_;
using ::testing::Invoke;
using ::testing::Matcher;
using ::testing::Return;

struct NetUtilsTestFixture : ::testing::Test
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

TEST_F(NetUtilsTestFixture, GetIfcIpAddress_GetifadrsFail)
{
    EXPECT_CALL(ifaddrs_mock_, getifaddrs()).WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno())));
    EXPECT_FALSE(score::os::Netutils::instance().get_ifcip_address("eth0").has_value());
}

TEST_F(NetUtilsTestFixture, GetIfcIpAddress_EmptyList)
{
    EXPECT_CALL(ifaddrs_mock_, getifaddrs());
    EXPECT_FALSE(score::os::Netutils::instance().get_ifcip_address("eth0").has_value());
}

TEST_F(NetUtilsTestFixture, GetIfcIpAddress_InterfaceNotFound)
{
    ifaddrs_mock_.AddIfaddrsListEntry("lo", 0);
    auto ifa = ifaddrs_mock_.GetIfaddrsList().value();

    EXPECT_CALL(ifaddrs_mock_, getifaddrs());
    EXPECT_CALL(ifaddrs_mock_, freeifaddrs(ifa));
    EXPECT_FALSE(score::os::Netutils::instance().get_ifcip_address("eth0").has_value());
}

TEST_F(NetUtilsTestFixture, GetIfcIpAddress_OneAddr_Success)
{
    ifaddrs_mock_.AddIfaddrsListEntry("eth0", 0x0100a8c0);
    auto ifa = ifaddrs_mock_.GetIfaddrsList().value();

    EXPECT_CALL(ifaddrs_mock_, getifaddrs());
    EXPECT_CALL(ifaddrs_mock_, freeifaddrs(ifa));
    EXPECT_TRUE(score::os::Netutils::instance().get_ifcip_address("eth0").has_value());
}

TEST_F(NetUtilsTestFixture, GetIfcIpAddress_TwoAddrs_Success)
{
    ifaddrs_mock_.AddIfaddrsListEntry("lo", 0);
    ifaddrs_mock_.AddIfaddrsListEntry("eth0", 0x0100a8c0);
    auto ifa = ifaddrs_mock_.GetIfaddrsList().value();

    EXPECT_CALL(ifaddrs_mock_, getifaddrs());
    EXPECT_CALL(ifaddrs_mock_, freeifaddrs(ifa));
    auto ret = score::os::Netutils::instance().get_ifcip_address("eth0");
    EXPECT_TRUE(ret.has_value());
    EXPECT_EQ(ret.value(), 0x0100a8c0);
}

TEST_F(NetUtilsTestFixture, GetNetMask_GetifadrsFail)
{
    EXPECT_CALL(ifaddrs_mock_, getifaddrs()).WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno())));
    EXPECT_FALSE(score::os::Netutils::instance().get_net_mask("eth0").has_value());
}

TEST_F(NetUtilsTestFixture, GetNetMask_EmptyList)
{
    EXPECT_CALL(ifaddrs_mock_, getifaddrs());
    EXPECT_FALSE(score::os::Netutils::instance().get_net_mask("eth0").has_value());
}

TEST_F(NetUtilsTestFixture, GetNetMask_InterfaceNotFound)
{
    ifaddrs_mock_.AddIfaddrsListEntry("lo", 0);
    auto ifa = ifaddrs_mock_.GetIfaddrsList().value();

    EXPECT_CALL(ifaddrs_mock_, getifaddrs());
    EXPECT_CALL(ifaddrs_mock_, freeifaddrs(ifa));
    EXPECT_FALSE(score::os::Netutils::instance().get_net_mask("eth0").has_value());
}

TEST_F(NetUtilsTestFixture, GetNetMask_OneAddrNoNetmask)
{
    ifaddrs_mock_.AddIfaddrsListEntry("eth0", 0x0100a8c0);
    auto ifa = ifaddrs_mock_.GetIfaddrsList().value();

    EXPECT_CALL(ifaddrs_mock_, getifaddrs());
    EXPECT_CALL(ifaddrs_mock_, freeifaddrs(ifa));
    EXPECT_FALSE(score::os::Netutils::instance().get_net_mask("eth0").has_value());
}

TEST_F(NetUtilsTestFixture, GetNetMask_OneAddr_Success)
{
    ifaddrs_mock_.AddIfaddrsListEntry("eth0", 0x0100a8c0, true, 0xffff);
    auto ifa = ifaddrs_mock_.GetIfaddrsList().value();

    EXPECT_CALL(ifaddrs_mock_, getifaddrs());
    EXPECT_CALL(ifaddrs_mock_, freeifaddrs(ifa));
    EXPECT_TRUE(score::os::Netutils::instance().get_net_mask("eth0").has_value());
}

TEST_F(NetUtilsTestFixture, GetNetMask_TwoAddrs_Success)
{
    ifaddrs_mock_.AddIfaddrsListEntry("lo", 0);
    ifaddrs_mock_.AddIfaddrsListEntry("eth0", 0x0100a8c0, true, 0xffff);
    auto ifa = ifaddrs_mock_.GetIfaddrsList().value();

    EXPECT_CALL(ifaddrs_mock_, getifaddrs());
    EXPECT_CALL(ifaddrs_mock_, freeifaddrs(ifa));
    auto ret = score::os::Netutils::instance().get_net_mask("eth0");
    EXPECT_TRUE(ret.has_value());
    EXPECT_EQ(ret.value(), 0xffff);
}

TEST_F(NetUtilsTestFixture, GetIfcIpAddressNetMask_GetifadrsFail)
{
    EXPECT_CALL(ifaddrs_mock_, getifaddrs()).WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno())));
    auto ret = score::os::Netutils::instance().get_ifc_ip_address_net_mask("eth0");
    EXPECT_FALSE(ret.has_value());
}

TEST_F(NetUtilsTestFixture, GetIfcIpAddressNetMask_EmptyList)
{
    EXPECT_CALL(ifaddrs_mock_, getifaddrs());
    auto ret = score::os::Netutils::instance().get_ifc_ip_address_net_mask("eth0");
    EXPECT_FALSE(ret.has_value());
}

TEST_F(NetUtilsTestFixture, GetIfcIpAddressNetMask_InterfaceNotFound)
{
    ifaddrs_mock_.AddIfaddrsListEntry("lo", 0);
    auto ifa = ifaddrs_mock_.GetIfaddrsList().value();

    EXPECT_CALL(ifaddrs_mock_, getifaddrs());
    EXPECT_CALL(ifaddrs_mock_, freeifaddrs(ifa));
    auto ret = score::os::Netutils::instance().get_ifc_ip_address_net_mask("eth0");
    EXPECT_FALSE(ret.has_value());
}

TEST_F(NetUtilsTestFixture, GetIfcIpAddressNetMask_OneAddr_Success)
{
    ifaddrs_mock_.AddIfaddrsListEntry("eth0", 0x0100a8c0, true, 0xffff);
    auto ifa = ifaddrs_mock_.GetIfaddrsList().value();

    EXPECT_CALL(ifaddrs_mock_, getifaddrs());
    EXPECT_CALL(ifaddrs_mock_, freeifaddrs(ifa));
    auto ret = score::os::Netutils::instance().get_ifc_ip_address_net_mask("eth0");
    EXPECT_TRUE(ret.has_value());
    EXPECT_EQ(ret.value().ToStringWithNetmask(), "192.168.0.1/16");
}

TEST_F(NetUtilsTestFixture, GetIfcIpAddressNetMask_OneAddrNoNetmask_Success)
{
    ifaddrs_mock_.AddIfaddrsListEntry("eth0", 0x0100a8c0);
    auto ifa = ifaddrs_mock_.GetIfaddrsList().value();

    EXPECT_CALL(ifaddrs_mock_, getifaddrs());
    EXPECT_CALL(ifaddrs_mock_, freeifaddrs(ifa));
    auto ret = score::os::Netutils::instance().get_ifc_ip_address_net_mask("eth0");
    EXPECT_TRUE(ret.has_value());
    EXPECT_EQ(ret.value().ToStringWithNetmask(), "192.168.0.1/24");
}

TEST_F(NetUtilsTestFixture, GetIfcIpAddressNetMask_TwoAddrs_Success)
{
    ifaddrs_mock_.AddIfaddrsListEntry("lo", 0, 0);
    ifaddrs_mock_.AddIfaddrsListEntry("eth0", 0x0100a8c0, true, 0xffff);
    auto ifa = ifaddrs_mock_.GetIfaddrsList().value();

    EXPECT_CALL(ifaddrs_mock_, getifaddrs());
    EXPECT_CALL(ifaddrs_mock_, freeifaddrs(ifa));
    auto ret = score::os::Netutils::instance().get_ifc_ip_address_net_mask("eth0");
    EXPECT_TRUE(ret.has_value());
    EXPECT_EQ(ret.value().ToStringWithNetmask(), "192.168.0.1/16");
}

TEST_F(NetUtilsTestFixture, SetIpAddress_Success)
{
    std::int32_t sock_fd{1};
    std::string if_name{"if_test"};
    Ipv4Address ip_expected{"199.88.77.53"};
    ifreq request{};

    EXPECT_CALL(socket_mock_, socket(Socket::Domain::kIPv4, SOCK_DGRAM, 0)).WillOnce(Return(sock_fd));
    EXPECT_CALL(ioctl_mock_, ioctl(sock_fd, SIOCSIFADDR, Matcher<void*>(_)))
        .WillOnce(Invoke([&request](const std::int32_t, const std::int32_t, void* const req_ptr) {
            ifreq* tmp_ptr = static_cast<ifreq*>(req_ptr);
            request = *tmp_ptr;
            return score::cpp::blank{};
        }));
    EXPECT_CALL(unistd_mock_, close(sock_fd)).WillOnce(Return(score::cpp::blank{}));

    EXPECT_TRUE(score::os::Netutils::instance().set_ip_address(if_name, ip_expected).has_value());
    EXPECT_EQ(std::string(request.ifr_name), if_name);
    sockaddr_in* ptr_addr = reinterpret_cast<sockaddr_in*>(&request.ifr_addr);
    EXPECT_EQ(Ipv4Address::CreateFromUint32NetOrder(ptr_addr->sin_addr.s_addr), ip_expected);
}

TEST_F(NetUtilsTestFixture, SetIpAddress_SuccessButCloseSocketFails)
{
    std::int32_t sock_fd{1};
    std::string if_name{"if_test"};
    Ipv4Address ip_expected{"199.88.77.53"};
    ifreq request{};

    EXPECT_CALL(socket_mock_, socket(Socket::Domain::kIPv4, SOCK_DGRAM, 0)).WillOnce(Return(sock_fd));
    EXPECT_CALL(ioctl_mock_, ioctl(sock_fd, SIOCSIFADDR, Matcher<void*>(_)))
        .WillOnce(Invoke([&request](const std::int32_t, const std::int32_t, void* const req_ptr) {
            ifreq* tmp_ptr = static_cast<ifreq*>(req_ptr);
            request = *tmp_ptr;
            return score::cpp::blank{};
        }));
    EXPECT_CALL(unistd_mock_, close(sock_fd)).WillOnce(Return(kEAcessUnExpectedError));

    EXPECT_TRUE(score::os::Netutils::instance().set_ip_address(if_name, ip_expected).has_value());
    EXPECT_EQ(std::string(request.ifr_name), if_name);
    sockaddr_in* ptr_addr = reinterpret_cast<sockaddr_in*>(&request.ifr_addr);
    EXPECT_EQ(Ipv4Address::CreateFromUint32NetOrder(ptr_addr->sin_addr.s_addr), ip_expected);
}

TEST_F(NetUtilsTestFixture, SetIpAddress_ReturnsError_When_SocketCreationFails)
{
    std::string if_name{"if_test"};
    Ipv4Address ip_expected{"199.88.77.53"};

    EXPECT_CALL(socket_mock_, socket(Socket::Domain::kIPv4, SOCK_DGRAM, 0)).WillOnce(Return(kEAcessUnExpectedError));
    const auto result = score::os::Netutils::instance().set_ip_address(if_name, ip_expected);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kPermissionDenied);
}

TEST_F(NetUtilsTestFixture, SetIpAddress_ReturnsError_When_SettingIPAddressFails)
{
    std::int32_t sock_fd{1};
    std::string if_name{"if_test"};
    Ipv4Address ip_expected{"199.88.77.53"};

    EXPECT_CALL(socket_mock_, socket(Socket::Domain::kIPv4, SOCK_DGRAM, 0)).WillOnce(Return(sock_fd));
    EXPECT_CALL(ioctl_mock_, ioctl(sock_fd, SIOCSIFADDR, Matcher<void*>(_))).WillOnce(Return(kEAcessUnExpectedError));

    const auto result = score::os::Netutils::instance().set_ip_address(if_name, ip_expected);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kPermissionDenied);
}

}  // namespace
}  // namespace os
}  // namespace score
