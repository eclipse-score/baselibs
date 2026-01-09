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
#include "score/os/utils/tcp_keep_alive.h"
#include "score/expected.hpp"
#include "score/os/mocklib/socketmock.h"
#include <gtest/gtest.h>

using namespace ::score::os;

using ::testing::_;
using ::testing::Return;

class TcpKeepAliveTest : public ::testing::Test
{
  public:
    TcpKeepAliveTest() {}
    ~TcpKeepAliveTest() override {}

  protected:
    score::os::SocketMock socketMock;
    virtual void SetUp() override
    {
        score::os::Socket::set_testing_instance(socketMock);
    }
    virtual void TearDown() override {}
};

TEST_F(TcpKeepAliveTest, TcpKeepAlive)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TcpKeepAliveTest Tcp Keep Alive");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    int socketfd = 0;
    EXPECT_CALL(socketMock, setsockopt(socketfd, _, _, _, _)).WillRepeatedly(Return(score::cpp::expected_blank<Error>{}));
    TcpKeepAlive(socketfd);
}
