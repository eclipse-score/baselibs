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
///
/// @file test_ip_address.cpp
/// @copyright Copyright (C) 2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Unit tests for the ip address class
///

#include "gmock/gmock.h"
#include <gtest/gtest.h>

#include "score/network/ipv4_address.h"

namespace score
{
namespace os
{
namespace
{

using ::testing::_;

struct IpAddressTest : ::testing::Test
{
};

TEST_F(IpAddressTest, TestIpv4AddressComparison)
{
    Ipv4Address address1(std::array<uint8_t, 4>{1, 2, 0, 4});
    ASSERT_TRUE(address1.IsValid());
    Ipv4Address address2(std::array<uint8_t, 4>{255, 254, 253, 252}, 14);
    ASSERT_TRUE(address2.IsValid());
    Ipv4Address address3(std::array<uint8_t, 4>{1, 2, 0, 4});
    ASSERT_TRUE(address3.IsValid());
    Ipv4Address address4(std::array<uint8_t, 4>{255, 254, 253, 252}, 25);
    ASSERT_TRUE(address4.IsValid());

    ASSERT_THAT(address1 != address2, true);
    ASSERT_THAT(address2 == address1, false);
    ASSERT_THAT(address1 == address3, true);
    ASSERT_THAT(address2 != address4, true);

    ASSERT_TRUE(address2 < address4);
    ASSERT_FALSE(address4 < address1);
}

TEST_F(IpAddressTest, TestIpv4AddressConversions_FromArray)
{
    Ipv4Address address2(std::array<uint8_t, 4>{1, 2, 0, 4});
    ASSERT_TRUE(address2.IsValid());
    ASSERT_EQ(address2.ToString(), "1.2.0.4");
    ASSERT_EQ(address2.ToBytes().size(), 4);
    ASSERT_EQ(address2.ToBytes()[0], 1);
    ASSERT_EQ(address2.ToBytes()[1], 2);
    ASSERT_EQ(address2.ToBytes()[2], 0);
    ASSERT_EQ(address2.ToBytes()[3], 4);
    EXPECT_EQ(address2.ToIpv4Bytes(), (Ipv4Address::AddressBytes{1, 2, 0, 4}));

    Ipv4Address address4(std::array<uint8_t, 4>{255, 254, 253, 252});
    ASSERT_TRUE(address4.IsValid());
    ASSERT_EQ(address4.ToString(), "255.254.253.252");
    ASSERT_EQ(address4.ToBytes().size(), 4);
    ASSERT_EQ(address4.ToBytes()[0], 255);
    ASSERT_EQ(address4.ToBytes()[1], 254);
    ASSERT_EQ(address4.ToBytes()[2], 253);
    ASSERT_EQ(address4.ToBytes()[3], 252);

    ASSERT_FALSE(address4.IsIpv6());
}

TEST_F(IpAddressTest, TestIpv4AddressConversions_FromUint32)
{
    uint32_t addr_from = 0x7f000001;
    Ipv4Address address1 = Ipv4Address::CreateFromUint32HostOrder(addr_from);
    ASSERT_TRUE(address1.IsValid());
    ASSERT_EQ(address1.ToString(), "127.0.0.1");
    ASSERT_EQ(address1.ToBytes().size(), 4);
    ASSERT_EQ(address1.ToBytes()[0], 127);
    ASSERT_EQ(address1.ToBytes()[1], 0);
    ASSERT_EQ(address1.ToBytes()[2], 0);
    ASSERT_EQ(address1.ToBytes()[3], 1);

    addr_from = 0xfffefdfc;
    Ipv4Address address2 = Ipv4Address::CreateFromUint32HostOrder(addr_from);
    ASSERT_TRUE(address2.IsValid());
    ASSERT_EQ(address2.ToString(), "255.254.253.252");
    ASSERT_EQ(address2.ToBytes().size(), 4);
    ASSERT_EQ(address2.ToBytes()[0], 255);
    ASSERT_EQ(address2.ToBytes()[1], 254);
    ASSERT_EQ(address2.ToBytes()[2], 253);
    ASSERT_EQ(address2.ToBytes()[3], 252);

    uint8_t addr_array3[4] = {1, 2, 0, 4};
    addr_from = *reinterpret_cast<uint32_t*>(addr_array3);
    Ipv4Address address3 = Ipv4Address::CreateFromUint32NetOrder(addr_from);
    ASSERT_TRUE(address3.IsValid());
    ASSERT_EQ(address3.ToString(), "1.2.0.4");
    ASSERT_EQ(address3.ToBytes().size(), 4);
    ASSERT_EQ(address3.ToBytes()[0], 1);
    ASSERT_EQ(address3.ToBytes()[1], 2);
    ASSERT_EQ(address3.ToBytes()[2], 0);
    ASSERT_EQ(address3.ToBytes()[3], 4);

    uint8_t addr_array4[4] = {255, 254, 253, 252};
    addr_from = *reinterpret_cast<uint32_t*>(addr_array4);
    Ipv4Address address4 = Ipv4Address::CreateFromUint32NetOrder(addr_from);
    ASSERT_TRUE(address4.IsValid());
    ASSERT_EQ(address4.ToString(), "255.254.253.252");
    ASSERT_EQ(address4.ToBytes().size(), 4);
    ASSERT_EQ(address4.ToBytes()[0], 255);
    ASSERT_EQ(address4.ToBytes()[1], 254);
    ASSERT_EQ(address4.ToBytes()[2], 253);
    ASSERT_EQ(address4.ToBytes()[3], 252);
}

TEST_F(IpAddressTest, TestIpv4AddressConversions_FromString)
{
    Ipv4Address address1("1.2.0.4");
    ASSERT_TRUE(address1.IsValid());
    ASSERT_EQ(address1.ToString(), "1.2.0.4");
    ASSERT_EQ(address1.ToBytes().size(), 4);
    ASSERT_EQ(address1.ToBytes()[0], 1);
    ASSERT_EQ(address1.ToBytes()[1], 2);
    ASSERT_EQ(address1.ToBytes()[2], 0);
    ASSERT_EQ(address1.ToBytes()[3], 4);

    Ipv4Address address3("127.255.131.145");
    ASSERT_TRUE(address3.IsValid());
    ASSERT_EQ(address3.ToString(), "127.255.131.145");
    ASSERT_EQ(address3.ToBytes().size(), 4);
    ASSERT_EQ(address3.ToBytes()[0], 127);
    ASSERT_EQ(address3.ToBytes()[1], 255);
    ASSERT_EQ(address3.ToBytes()[2], 131);
    ASSERT_EQ(address3.ToBytes()[3], 145);

    Ipv4Address address5("1272.255.131.145");
    ASSERT_FALSE(address5.IsValid());
    ASSERT_EQ(address5.ToString(), "0.0.0.0");

    Ipv4Address address6("asd");
    ASSERT_FALSE(address6.IsValid());
    ASSERT_EQ(address5.ToString(), "0.0.0.0");

    Ipv4Address address7("1.2.3.256");
    ASSERT_FALSE(address7.IsValid());
    ASSERT_EQ(address7.ToString(), "0.0.0.0");

    Ipv4Address address8("-1.2.3.4");
    ASSERT_FALSE(address8.IsValid());
    ASSERT_EQ(address8.ToString(), "0.0.0.0");
}

TEST_F(IpAddressTest, TestIpv4AddressConversions_FromString_WithNetmask)
{
    Ipv4Address address1("1.2.0.4/24");
    ASSERT_TRUE(address1.IsValid());
    ASSERT_EQ(address1.ToString(), "1.2.0.4");
    ASSERT_EQ(address1.ToStringWithNetmask(), "1.2.0.4/24");
    ASSERT_EQ(address1.ToBytes().size(), 4);
    ASSERT_EQ(address1.ToBytes()[0], 1);
    ASSERT_EQ(address1.ToBytes()[1], 2);
    ASSERT_EQ(address1.ToBytes()[2], 0);
    ASSERT_EQ(address1.ToBytes()[3], 4);
    ASSERT_EQ(address1.GetNetmask(), 24);

    Ipv4Address address2("1.2.0.4/24a");
    ASSERT_FALSE(address2.IsValid());
    ASSERT_EQ(address2.ToString(), "0.0.0.0");
    ASSERT_EQ(address2.ToStringWithNetmask(), "0.0.0.0/24");
    ASSERT_EQ(address2.GetNetmask(), 24);

    Ipv4Address address3("1.2.0.4/45");
    ASSERT_TRUE(address3.IsValid());
    ASSERT_EQ(address3.ToString(), "1.2.0.4");
    ASSERT_EQ(address3.ToStringWithNetmask(), "1.2.0.4/24");
    ASSERT_EQ(address3.GetNetmask(), 24);
}

TEST_F(IpAddressTest, TestIpv4AddressConversions_FromVector)
{
    std::vector<uint8_t> v1;
    std::vector<uint8_t> v2{1, 2, 3, 4};
    std::vector<uint8_t> v3{1, 2, 3, 4, 5};

    Ipv4Address address1(v1);
    ASSERT_FALSE(address1.IsValid());
    ASSERT_EQ(address1.GetNetmask(), 24);

    Ipv4Address address2(v2);
    ASSERT_TRUE(address2.IsValid());
    ASSERT_EQ(address2.ToString(), "1.2.3.4");
    ASSERT_EQ(address2.ToStringWithNetmask(), "1.2.3.4/24");
    ASSERT_EQ(address2.GetNetmask(), 24);

    Ipv4Address address2b(v2, 20);
    ASSERT_TRUE(address2b.IsValid());
    ASSERT_EQ(address2b.ToString(), "1.2.3.4");
    ASSERT_EQ(address2b.ToStringWithNetmask(), "1.2.3.4/20");
    ASSERT_EQ(address2b.GetNetmask(), 20);

    Ipv4Address address3(v3);
    ASSERT_FALSE(address3.IsValid());
    ASSERT_EQ(address3.GetNetmask(), 24);

    Ipv4Address address3b(v3, 20);
    ASSERT_FALSE(address3.IsValid());
    ASSERT_EQ(address3.GetNetmask(), 24);
}

TEST_F(IpAddressTest, TestIpv4AddressConversions_NetmaskToPrefix)
{
    uint8_t netmask_0[4] = {0, 0, 0, 0};
    ASSERT_EQ(Ipv4Address::ConvertNetmaskUint32NetOrderToPrefix(*reinterpret_cast<uint32_t*>(netmask_0)), 0);
    uint8_t netmask_1[4] = {128, 0, 0, 0};
    ASSERT_EQ(Ipv4Address::ConvertNetmaskUint32NetOrderToPrefix(*reinterpret_cast<uint32_t*>(netmask_1)), 1);
    uint8_t netmask_7[4] = {254, 0, 0, 0};
    ASSERT_EQ(Ipv4Address::ConvertNetmaskUint32NetOrderToPrefix(*reinterpret_cast<uint32_t*>(netmask_7)), 7);
    uint8_t netmask_15[4] = {255, 254, 0, 0};
    ASSERT_EQ(Ipv4Address::ConvertNetmaskUint32NetOrderToPrefix(*reinterpret_cast<uint32_t*>(netmask_15)), 15);
    uint8_t netmask_16[4] = {255, 255, 0, 0};
    ASSERT_EQ(Ipv4Address::ConvertNetmaskUint32NetOrderToPrefix(*reinterpret_cast<uint32_t*>(netmask_16)), 16);
    uint8_t netmask_18[4] = {255, 255, 192, 0};
    ASSERT_EQ(Ipv4Address::ConvertNetmaskUint32NetOrderToPrefix(*reinterpret_cast<uint32_t*>(netmask_18)), 18);
    uint8_t netmask_20[4] = {255, 255, 240, 0};
    ASSERT_EQ(Ipv4Address::ConvertNetmaskUint32NetOrderToPrefix(*reinterpret_cast<uint32_t*>(netmask_20)), 20);
    uint8_t netmask_31[4] = {255, 255, 255, 254};
    ASSERT_EQ(Ipv4Address::ConvertNetmaskUint32NetOrderToPrefix(*reinterpret_cast<uint32_t*>(netmask_31)), 31);
    uint8_t netmask_0w[4] = {0, 255, 255, 254};
    ASSERT_EQ(Ipv4Address::ConvertNetmaskUint32NetOrderToPrefix(*reinterpret_cast<uint32_t*>(netmask_0w)), 0);
    uint8_t netmask_0w2[4] = {127, 255, 255, 254};
    ASSERT_EQ(Ipv4Address::ConvertNetmaskUint32NetOrderToPrefix(*reinterpret_cast<uint32_t*>(netmask_0w2)), 0);
    uint8_t netmask_12w[4] = {255, 247, 255, 255};
    ASSERT_EQ(Ipv4Address::ConvertNetmaskUint32NetOrderToPrefix(*reinterpret_cast<uint32_t*>(netmask_12w)), 12);
    uint8_t netmask_32w[4] = {255, 255, 255, 255};
    ASSERT_EQ(Ipv4Address::ConvertNetmaskUint32NetOrderToPrefix(*reinterpret_cast<uint32_t*>(netmask_32w)), 32);
}

TEST_F(IpAddressTest, TestIpv4AddressConversions_SetNetmask)
{
    Ipv4Address address("1.2.0.4");
    ASSERT_TRUE(address.IsValid());
    ASSERT_EQ(address.ToString(), "1.2.0.4");

    address.SetNetmask(13);
    ASSERT_EQ(address.ToStringWithNetmask(), "1.2.0.4/13");
    ASSERT_TRUE(address.IsValid());

    address.SetNetmask(14);
    ASSERT_TRUE(address.IsValid());
    ASSERT_EQ(address.ToStringWithNetmask(), "1.2.0.4/14");
}

TEST_F(IpAddressTest, TestIpv4AddressConversions_IsValid)
{
    Ipv4Address address1("0.0.0.0");
    ASSERT_FALSE(address1.IsValid());

    Ipv4Address address2("1.0.0.0");
    ASSERT_TRUE(address2.IsValid());

    Ipv4Address address3("0.1.0.0");
    ASSERT_TRUE(address3.IsValid());

    Ipv4Address address4("0.0.1.0");
    ASSERT_TRUE(address4.IsValid());

    Ipv4Address address5("0.0.0.1");
    ASSERT_TRUE(address5.IsValid());

    Ipv4Address address6("121.0.1");
    ASSERT_FALSE(address6.IsValid());

    Ipv4Address address7("ab.1.2.3");
    ASSERT_FALSE(address7.IsValid());
}

TEST_F(IpAddressTest, TestGetNetmaskBytes)
{
    Ipv4Address::AddressBytes netmask{};
    netmask = {255, 255, 255, 255};
    ASSERT_EQ(Ipv4Address("1.2.3.4/32").GetNetmaskBytes(), netmask);
    netmask = {255, 255, 255, 254};
    ASSERT_EQ(Ipv4Address("1.2.3.4/31").GetNetmaskBytes(), netmask);
    netmask = {255, 255, 255, 128};
    ASSERT_EQ(Ipv4Address("1.2.3.4/25").GetNetmaskBytes(), netmask);
    netmask = {255, 255, 255, 0};
    ASSERT_EQ(Ipv4Address("1.2.3.4/24").GetNetmaskBytes(), netmask);
    netmask = {255, 255, 254, 0};
    ASSERT_EQ(Ipv4Address("1.2.3.4/23").GetNetmaskBytes(), netmask);
    netmask = {255, 255, 128, 0};
    ASSERT_EQ(Ipv4Address("1.2.3.4/17").GetNetmaskBytes(), netmask);
    netmask = {255, 255, 0, 0};
    ASSERT_EQ(Ipv4Address("1.2.3.4/16").GetNetmaskBytes(), netmask);
    netmask = {255, 248, 0, 0};
    ASSERT_EQ(Ipv4Address("1.2.3.4/13").GetNetmaskBytes(), netmask);
    netmask = {255, 240, 0, 0};
    ASSERT_EQ(Ipv4Address("1.2.3.4/12").GetNetmaskBytes(), netmask);
    netmask = {128, 0, 0, 0};
    ASSERT_EQ(Ipv4Address("1.2.3.4/1").GetNetmaskBytes(), netmask);
}

}  // namespace
}  // namespace os
}  // namespace score
