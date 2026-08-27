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
#include "score/os/linux/utils/network_interface.h"
#include "score/os/linux/utils/mocklib/network_interface_mock.h"
#include "score/os/mocklib/nonposixwrappermock.h"
#include "score/os/mocklib/stat_mock.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace score::os;

using ::testing::_;
using ::testing::ByMove;
using ::testing::Exactly;
using ::testing::Return;

namespace
{
NonPosixWrapperMock* nonPosixWrappermock_ = nullptr;
StatMock statmock;
LinuxNetworkInterfaceMock* LinuxNetworkInterfaceMock_ = nullptr;
}  // namespace

class LinuxNetworkInterfaceTest : public ::testing::Test
{
  public:
    LinuxNetworkInterfaceTest() {}
    ~LinuxNetworkInterfaceTest() override {}

  protected:
    virtual void SetUp() override
    {
        nonPosixWrappermock_ = new NonPosixWrapperMock();
        LinuxNetworkInterfaceMock_ = new LinuxNetworkInterfaceMock();
        Stat::set_testing_instance(statmock);
    }
    virtual void TearDown() override
    {
        delete nonPosixWrappermock_;
        nonPosixWrappermock_ = nullptr;
        delete LinuxNetworkInterfaceMock_;
        LinuxNetworkInterfaceMock_ = nullptr;
    }
};

TEST_F(LinuxNetworkInterfaceTest, IsInterfaceBridge_InexistentIface_ShouldFail)
{
    static const char* br_name = "madison";

    EXPECT_CALL(statmock, stat(_, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EBADF))));
    ASSERT_FALSE(score::os::bIsInterfaceBridge(br_name));
}

TEST_F(LinuxNetworkInterfaceTest, IsInterfaceBridge_ExistentIface_ShouldPass)
{
    static const char* br_name = "br0";

    EXPECT_CALL(statmock, stat(_, _, _)).WillOnce(Return(score::cpp::expected_blank<score::os::Error>{}));
    ASSERT_TRUE(score::os::bIsInterfaceBridge(br_name));
}

TEST_F(LinuxNetworkInterfaceTest, IsInterfaceBridge_EmptyName_ShouldFail)
{
    EXPECT_CALL(statmock, stat(_, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EACCES))));
    ASSERT_FALSE(score::os::bIsInterfaceBridge(""));
}

TEST_F(LinuxNetworkInterfaceTest, IsInterfacePhy_InexistentIface_ShouldFail)
{
    static const char* if_name = "Sockete";

    EXPECT_CALL(statmock, stat(_, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EACCES))));
    ASSERT_FALSE(score::os::bIsInterfacePhyDevice(if_name));
}

TEST_F(LinuxNetworkInterfaceTest, IsInterfacePhy_ExistentIface_ShouldPass)
{
    static const char* if_name = "eth0";

    EXPECT_CALL(statmock, stat(_, _, _)).WillOnce(Return(score::cpp::expected_blank<score::os::Error>{}));
    ASSERT_TRUE(score::os::bIsInterfacePhyDevice(if_name));
}

TEST_F(LinuxNetworkInterfaceTest, IsInterfacePhy_EmptyName_ShouldFail)
{
    EXPECT_CALL(statmock, stat(_, _, _))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EACCES))));
    ASSERT_FALSE(score::os::bIsInterfacePhyDevice(""));
}

TEST_F(LinuxNetworkInterfaceTest, bFillInterfaceInformation_ShouldFailOnEmpty)
{
    score::os::network_settings ns{};

    EXPECT_CALL(*nonPosixWrappermock_, freeifaddrs(_)).Times(0);
    EXPECT_CALL(*nonPosixWrappermock_, getifaddrs(_)).WillOnce(Return(-1));
    ASSERT_FALSE(score::os::bFillInterfaceInformation(ns));
}
