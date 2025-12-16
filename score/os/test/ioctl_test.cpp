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
#include "score/os/mocklib/ioctl_mock.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "score/os/ioctl.h"
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

using ::testing::_;
using ::testing::An;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::Return;

TEST(IoctlTest, ReadNumberOfCharactorsWaitingToBeRead)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "The operating system shall support ioctl");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    const auto sockfd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(sockfd, -1) << "Error creating socket: " << errno;

    std::int32_t number_of_char{};
    EXPECT_TRUE(score::os::Ioctl::instance().ioctl(sockfd, FIONREAD, &number_of_char).has_value());

    std::int32_t value{};
    ::ioctl(sockfd, FIONREAD, &value);
    EXPECT_EQ(number_of_char, value);

    ::close(sockfd);
}

TEST(IoctlTest, InvalidFd)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "The operating system shall support ioctl for invalid file descriptor");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    const auto invalid_fd = -1;
    std::int32_t number_of_char{};

    EXPECT_FALSE(score::os::Ioctl::instance().ioctl(invalid_fd, FIONREAD, &number_of_char).has_value());
}

struct IoctlMockTest : ::testing::Test
{
    void SetUp() override
    {
        score::os::Ioctl::set_testing_instance(ioctlmock);
    };

    score::os::IoctlMock ioctlmock;
};

TEST_F(IoctlMockTest, NoAdditionalArgument)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "The operating system shall support ioctl");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    EXPECT_CALL(ioctlmock, ioctl(1, 2, An<void*>()));
    score::os::Ioctl::instance().ioctl(1, 2, nullptr);
}

TEST_F(IoctlMockTest, Integer)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "The operating system shall support ioctl");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    auto value{0};
    EXPECT_CALL(ioctlmock, ioctl(1, 2, &value));
    score::os::Ioctl::instance().ioctl(1, 2, &value);
}

TEST_F(IoctlMockTest, Pointer)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "The operating system shall support ioctl");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    EXPECT_CALL(ioctlmock, ioctl(1, 2, An<void*>()))
        .WillOnce(DoAll(Invoke([](std::int32_t, std::int32_t, void* arg) {
                            ASSERT_TRUE(arg != nullptr);
                            timespec* t = reinterpret_cast<timespec*>(arg);
                            EXPECT_EQ(t->tv_sec, 123);
                            EXPECT_EQ(t->tv_nsec, 456);
                        }),
                        Return(score::cpp::expected_blank<score::os::Error>{})));
    timespec t{123, 456};
    EXPECT_TRUE(score::os::Ioctl::instance().ioctl(1, 2, &t));
}
