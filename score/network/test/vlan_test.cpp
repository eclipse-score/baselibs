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
#include "score/network/vlan.h"

#include "score/os/mocklib/socketmock.h"
#include "score/os/version.h"

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
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

#if defined(SPP_OS_QNX_VERSION) && SPP_OS_QNX_VERSION
#if SPP_OS_VERSION_GTE(SPP_OS_QNX_VERSION, 8, 0)
constexpr auto kVlanPrioOption = -1;  // SO_VLANPRIO is not available in QNX 8.0
#else
constexpr auto kVlanPrioOption = SO_VLANPRIO;
#endif
#else  // defined(SPP_OS_QNX_VERSION) && SPP_OS_QNX_VERSION
constexpr auto kVlanPrioOption = SO_PRIORITY;
#endif

class VlanLinuxTestFixture : public Test
{
  public:
    os::MockGuard<NiceMock<SocketMock>> socket_mock{};
};

TEST_F(VlanLinuxTestFixture, SetVlanPriorityOfSocketShallConfigureSocket)
{
    constexpr std::uint8_t kPcpPriority = 7u;
    constexpr std::uint8_t kFileDescriptor = 42;
    EXPECT_CALL(*socket_mock, setsockopt(kFileDescriptor, SOL_SOCKET, kVlanPrioOption, _, sizeof(kFileDescriptor)))
        .Times(1)
        .WillOnce(testing::Invoke(
            [kPcpPriority](int /*fd*/, int /*level*/, int /*optname*/, const void* optval, socklen_t /*optlen*/)
                -> score::cpp::expected_blank<Error> {
                EXPECT_EQ(*reinterpret_cast<const std::uint8_t*>(optval), kPcpPriority);
                return score::cpp::blank{};
            }));
    Vlan::instance().SetVlanPriorityOfSocket(kPcpPriority, kFileDescriptor);
}

}  // namespace

}  // namespace os
}  // namespace score
