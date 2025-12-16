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
#include "score/os/sys_poll_impl.h"

#include "gtest/gtest.h"

namespace score
{
namespace os
{
namespace test
{

class SysPollImplTest : public ::testing::Test
{
  protected:
    int pipe_fd[2];

    void SetUp() override
    {
        ASSERT_EQ(::pipe(pipe_fd), 0) << "Failed to create pipe";
    }

    void TearDown() override
    {
        ::close(pipe_fd[0]);
        ::close(pipe_fd[1]);
    }
};

TEST_F(SysPollImplTest, PollSucceeds)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SysPollImplTest Poll Succeeds");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    struct pollfd fds[1];
    fds[0].fd = pipe_fd[0];
    fds[0].events = POLLIN;

    const std::int32_t timeout{1};  // 1 millisecond

    auto result = score::os::SysPoll::instance().poll(fds, 1, timeout);

    EXPECT_TRUE(result.has_value()) << "Expected poll to succeed";
    EXPECT_EQ(result.value(), 0) << "Expected poll to return 0 for timeout";
}

TEST_F(SysPollImplTest, PMRDefaultShallReturnImplInstance)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SysPollImplTest PMRDefault Shall Return Impl Instance");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    score::cpp::pmr::memory_resource* memory_resource = score::cpp::pmr::get_default_resource();
    const auto instance = score::os::SysPoll::Default(memory_resource);
    ASSERT_TRUE(instance != nullptr);
    EXPECT_NO_THROW(std::ignore = dynamic_cast<score::os::SysPollImpl*>(instance.get()));
}

}  // namespace test
}  // namespace os
}  // namespace score
