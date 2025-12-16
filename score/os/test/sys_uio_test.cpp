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
#include "score/os/sys_uio_impl.h"

#include "gtest/gtest.h"

namespace score
{
namespace os
{
namespace test
{

class SysUioImplTest : public ::testing::Test
{
  protected:
    std::array<std::int32_t, 2> pipe_fd;

    void SetUp() override
    {
        ASSERT_EQ(::pipe(pipe_fd.data()), 0) << "Failed to create pipe";
    }

    void TearDown() override
    {
        ::close(pipe_fd[0]);
        ::close(pipe_fd[1]);
    }
};

TEST_F(SysUioImplTest, WritevFailsBadFd)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SysUioImplTest Writev Fails Bad Fd");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    constexpr std::int32_t kInvalidFd{-1};
    std::array<iovec, 2UL> io{};
    std::uint8_t byte1{};
    std::uint8_t byte2{};
    io[0].iov_base = &byte1;
    io[0].iov_len = sizeof(byte1);
    io[1].iov_base = &byte2;
    io[1].iov_len = sizeof(byte2);

    auto result = score::os::SysUio::instance().writev(kInvalidFd, io.data(), io.size());

    EXPECT_FALSE(result.has_value()) << "Expected writev to fail";
    EXPECT_EQ(result.error(), score::os::Error::createFromErrno(EBADF)) << "Expected writev to fail with EBADF";
}

TEST_F(SysUioImplTest, WritevSucceeds)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SysUioImplTest Writev Succeeds");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::array<iovec, 2UL> io{};
    std::uint8_t byte1{1};
    std::uint8_t byte2{2};
    io[0].iov_base = &byte1;
    io[0].iov_len = sizeof(byte1);
    io[1].iov_base = &byte2;
    io[1].iov_len = sizeof(byte2);

    auto result = score::os::SysUio::instance().writev(pipe_fd[1], io.data(), io.size());

    EXPECT_TRUE(result.has_value()) << "Expected writev to succeed";
    EXPECT_EQ(result.value(), 2) << "Expected writev to return 2 for number of bytes written";

    std::array<std::uint8_t, 2> bytes{};
    ::read(pipe_fd[0], bytes.data(), bytes.size());
    EXPECT_TRUE((bytes[0] == 1) && (bytes[1] == 2)) << "Expected read to get byte values 1 and 2";
}

TEST_F(SysUioImplTest, PMRDefaultShallReturnImplInstance)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SysUioImplTest PMRDefault Shall Return Impl Instance");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    score::cpp::pmr::memory_resource* memory_resource = score::cpp::pmr::get_default_resource();
    const auto instance = score::os::SysUio::Default(memory_resource);
    ASSERT_TRUE(instance != nullptr);
    EXPECT_NO_THROW(std::ignore = dynamic_cast<score::os::SysUioImpl*>(instance.get()));
}

}  // namespace test
}  // namespace os
}  // namespace score
