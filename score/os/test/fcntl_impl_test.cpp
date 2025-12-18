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
#include "score/os/fcntl_impl.h"
#include "score/os/errno.h"
#include "score/os/fcntl.h"
#include "score/utils/src/scoped_operation.h"

#include "gtest/gtest.h"

#include <sys/wait.h>
#include <unistd.h>
#include <cstdint>
#include <iostream>

namespace score
{
namespace os
{
namespace test
{

class FcntlImplTest : public ::testing::Test
{
  public:
    void SetUp() noexcept override
    {
        file_descriptor_ = ::open(filename_, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        ASSERT_NE(file_descriptor_, -1);
        // pipes are used as synchronisation mechanism between parent and child processes.
        ASSERT_NE(pipe(parent_pipe), -1);
        ASSERT_NE(pipe(child_pipe), -1);
    }

    void TearDown() noexcept override
    {
        ::close(file_descriptor_);
        std::remove(filename_);
        ::close(parent_pipe[0]);
        ::close(parent_pipe[1]);
        ::close(child_pipe[0]);
        ::close(child_pipe[1]);
    }

    void Wait(int pipe[2])
    {
        const size_t buffer_size{80};
        char read_buffer[buffer_size];
        score::cpp::ignore = read(pipe[0], read_buffer, sizeof(read_buffer));
    }
    void Signal(int pipe[2])
    {
        char string[] = "Test";
        write(pipe[1], string, (strlen(string) + 1));
    }

  protected:
    static constexpr auto filename_{"test"};
    int file_descriptor_;
    int parent_pipe[2], child_pipe[2];
};

TEST_F(FcntlImplTest, KFileSetStatusFlagsFailsWithWrongCommand)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FcntlImplTest KFile Set Status Flags Fails With Wrong Command");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto command{Fcntl::Command::kFileGetStatusFlags};
    const Fcntl::Open open_flags{};
    const auto fcntl_result = score::os::Fcntl::instance().fcntl(file_descriptor_, command, open_flags);
    ASSERT_FALSE(fcntl_result.has_value());
    EXPECT_EQ(fcntl_result.error(), Error::Code::kInvalidArgument);
}

TEST_F(FcntlImplTest, CommandKFileSetStatusFlagsSetsFlags)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FcntlImplTest Command KFile Set Status Flags Sets Flags");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const Fcntl::Command command{Fcntl::Command::kFileSetStatusFlags};
    const auto open_flags{Fcntl::Open::kNonBlocking};
    const auto fcntl_result = score::os::Fcntl::instance().fcntl(file_descriptor_, command, open_flags);
    ASSERT_TRUE(fcntl_result.has_value());

    std::int32_t flags = ::fcntl(file_descriptor_, F_GETFL);
    ASSERT_NE(flags, -1);
    EXPECT_TRUE((static_cast<std::uint32_t>(flags) & static_cast<std::uint32_t>(O_NONBLOCK)) != 0U);
}

TEST_F(FcntlImplTest, CommandKFileSetStatusFlagsFailsWithInvalidFileDescriptor)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FcntlImplTest Command KFile Set Status Flags Fails With Invalid File Descriptor");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    ::close(file_descriptor_);

    const auto command{Fcntl::Command::kFileSetStatusFlags};
    const Fcntl::Open open_flags{};
    const auto fcntl_result = score::os::Fcntl::instance().fcntl(file_descriptor_, command, open_flags);
    ASSERT_FALSE(fcntl_result.has_value());
    EXPECT_EQ(fcntl_result.error(), Error::Code::kBadFileDescriptor);
}

TEST_F(FcntlImplTest, KFileGetStatusFlagsFailsWithWrongCommand)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FcntlImplTest KFile Get Status Flags Fails With Wrong Command");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto command{Fcntl::Command::kFileSetStatusFlags};
    const auto fcntl_result = score::os::Fcntl::instance().fcntl(file_descriptor_, command);
    ASSERT_FALSE(fcntl_result.has_value());
    EXPECT_EQ(fcntl_result.error(), Error::Code::kInvalidArgument);
}

TEST_F(FcntlImplTest, CommandKFileSetStatusFlagsGetsFlags)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FcntlImplTest Command KFile Set Status Flags Gets Flags");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto command{Fcntl::Command::kFileGetStatusFlags};
    const auto fcntl_result = score::os::Fcntl::instance().fcntl(file_descriptor_, command);
    ASSERT_TRUE(fcntl_result.has_value());

    std::int32_t flags = ::fcntl(file_descriptor_, F_GETFL);
    ASSERT_NE(flags, -1);
    EXPECT_EQ(fcntl_result.value(), internal::fcntl_helper::IntegerToOpenFlag(flags));
}

TEST_F(FcntlImplTest, CommandKFileGetStatusFlagsFailsWithInvalidFileDescriptor)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FcntlImplTest Command KFile Get Status Flags Fails With Invalid File Descriptor");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    ::close(file_descriptor_);

    const auto command{Fcntl::Command::kFileGetStatusFlags};
    const auto fcntl_result = score::os::Fcntl::instance().fcntl(file_descriptor_, command);
    ASSERT_FALSE(fcntl_result.has_value());
    EXPECT_EQ(fcntl_result.error(), Error::Code::kBadFileDescriptor);
}

TEST_F(FcntlImplTest, OpenSucceeds)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FcntlImplTest Open Succeeds");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto open_flags{Fcntl::Open::kReadOnly};
    const auto result = score::os::Fcntl::instance().open(filename_, open_flags);
    ASSERT_TRUE(result.has_value());
    EXPECT_NE(result.value(), -1);

    ::close(result.value());
}

TEST_F(FcntlImplTest, OpenFailsWithInvalidPath)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FcntlImplTest Open Fails With Invalid Path");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto open_flags{Fcntl::Open::kReadOnly};
    const auto invalid_filename{"invalid"};
    const auto result = score::os::Fcntl::instance().open(invalid_filename, open_flags);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Error::Code::kNoSuchFileOrDirectory);
}

TEST_F(FcntlImplTest, OpenWithModeSucceeds)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FcntlImplTest Open With Mode Succeeds");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto open_flags{Fcntl::Open::kReadOnly | Fcntl::Open::kCreate | Fcntl::Open::kExclusive};
    const auto status_flags{Stat::Mode::kReadUser | Stat::Mode::kWriteUser};
    const auto new_filename{"test2"};
    const auto result = score::os::Fcntl::instance().open(new_filename, open_flags, status_flags);
    ASSERT_TRUE(result.has_value());
    EXPECT_NE(result.value(), -1);

    ::close(result.value());
    std::remove(new_filename);
}

TEST_F(FcntlImplTest, OpenWithModeFailsWithInvalidPath)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FcntlImplTest Open With Mode Fails With Invalid Path");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto open_flags{Fcntl::Open::kReadOnly | Fcntl::Open::kCreate | Fcntl::Open::kExclusive};
    const auto status_flags{Stat::Mode::kReadUser | Stat::Mode::kWriteUser};
    const auto result = score::os::Fcntl::instance().open(filename_, open_flags, status_flags);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Error::Code::kObjectExists);
}

TEST_F(FcntlImplTest, PosixFallocateSucceedsWithValidFileDescriptor)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FcntlImplTest Posix Fallocate Succeeds With Valid File Descriptor");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const off_t offset{0};
    const off_t length{200};
    const auto result = score::os::Fcntl::instance().posix_fallocate(file_descriptor_, offset, length);
    ASSERT_TRUE(result.has_value());

    const auto seek_result = lseek(file_descriptor_, 0L, SEEK_END);
    EXPECT_EQ(seek_result, length);
}

TEST_F(FcntlImplTest, PosixFallocateFailsWithInvalidFileDescriptor)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FcntlImplTest Posix Fallocate Fails With Invalid File Descriptor");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    ::close(file_descriptor_);
    const off_t offset{0};
    const off_t length{200};
    const auto result = score::os::Fcntl::instance().posix_fallocate(file_descriptor_, offset, length);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Error::Code::kBadFileDescriptor);
}

TEST_F(FcntlImplTest, FlockFailsWithInvalidFileDescriptor)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FcntlImplTest Flock Fails With Invalid File Descriptor");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    ::close(file_descriptor_);
    const auto result = score::os::Fcntl::instance().flock(file_descriptor_, Fcntl::Operation::kLockShared);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Error::Code::kBadFileDescriptor);
}

TEST_F(FcntlImplTest, FlockFailsWithKUnLock)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FcntlImplTest Flock Fails With KUn Lock");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    ::close(file_descriptor_);
    const auto result = score::os::Fcntl::instance().flock(file_descriptor_, Fcntl::Operation::kUnLock);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Error::Code::kBadFileDescriptor);
}

TEST_F(FcntlImplTest, FlockSucceedsWithValidFileDescriptor)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FcntlImplTest Flock Succeeds With Valid File Descriptor");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto result = score::os::Fcntl::instance().flock(file_descriptor_, Fcntl::Operation::kLockShared);
    ASSERT_TRUE(result.has_value());
}

TEST_F(FcntlImplTest, FlockSucceedsWithValidFileDescriptorAndWithExclusiveUnLockCombination)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "FcntlImplTest Flock Succeeds With Valid File Descriptor And With Exclusive Un Lock Combination");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto result = score::os::Fcntl::instance().flock(file_descriptor_,
                                                         Fcntl::Operation::kLockExclusive | Fcntl::Operation::kLockNB);
    ASSERT_TRUE(result.has_value());
}

TEST_F(FcntlImplTest, FlockFailsWhenTryToObtainExclusiveLockTwice)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FcntlImplTest Flock Fails When Try To Obtain Exclusive Lock Twice");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto ret = fork();
    ASSERT_NE(ret, -1) << "Fork failed";
    if (ret == 0)
    {
        auto file_descriptor = ::open(filename_, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        ASSERT_NE(file_descriptor, -1);
        score::utils::ScopedOperation<> scoped_operation_open{[&file_descriptor]() noexcept {
            ::close(file_descriptor);
        }};
        auto result = score::os::Fcntl::instance().flock(file_descriptor,
                                                       Fcntl::Operation::kLockExclusive | Fcntl::Operation::kLockNB);
        // unblock parent process.
        Signal(parent_pipe);
        // wait for parent process to finish.
        Wait(child_pipe);
    }
    else
    {
        // wait till child process has performed flock operation.
        Wait(parent_pipe);
        const auto result = score::os::Fcntl::instance().flock(
            file_descriptor_, Fcntl::Operation::kLockExclusive | Fcntl::Operation::kLockNB);
        ASSERT_FALSE(result.has_value());
        // unblock child process
        Signal(child_pipe);
        // wait for child process to exit. If parent process exits before child. Child process could become zombie.
        wait(nullptr);
    }
}

TEST_F(FcntlImplTest, FlockFailsWhenTryToObtainExclusiveLockAndSharedLock)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FcntlImplTest Flock Fails When Try To Obtain Exclusive Lock And Shared Lock");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto ret = fork();
    ASSERT_NE(ret, -1) << "Fork failed";
    if (ret == 0)
    {
        auto file_descriptor = ::open(filename_, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        ASSERT_NE(file_descriptor, -1);
        score::utils::ScopedOperation<> scoped_operation_open{[&file_descriptor]() noexcept {
            ::close(file_descriptor);
        }};
        auto result = score::os::Fcntl::instance().flock(file_descriptor,
                                                       Fcntl::Operation::kLockExclusive | Fcntl::Operation::kLockNB);
        // unblock parent process.
        Signal(parent_pipe);
        // wait for parent process to finish.
        Wait(child_pipe);
    }
    else
    {
        // wait till child process has performed flock operation.
        Wait(parent_pipe);
        const auto result = score::os::Fcntl::instance().flock(file_descriptor_,
                                                             Fcntl::Operation::kLockShared | Fcntl::Operation::kLockNB);
        ASSERT_FALSE(result.has_value());
        // unblock child process
        Signal(child_pipe);
        // wait for child process to exit. If parent process exits before child. Child process could become zombie.
        wait(nullptr);
    }
}

TEST_F(FcntlImplTest, FlockSucceedsWhenTryToObtainSharedLockTwice)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FcntlImplTest Flock Succeeds When Try To Obtain Shared Lock Twice");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto ret = fork();
    ASSERT_NE(ret, -1) << "Fork failed";
    if (ret == 0)
    {
        auto file_descriptor = ::open(filename_, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        ASSERT_NE(file_descriptor, -1);
        score::utils::ScopedOperation<> scoped_operation_open{[&file_descriptor]() noexcept {
            ::close(file_descriptor);
        }};
        auto result = score::os::Fcntl::instance().flock(file_descriptor,
                                                       Fcntl::Operation::kLockShared | Fcntl::Operation::kLockNB);
        // unblock parent process.
        Signal(parent_pipe);
        // wait for parent process to finish.
        Wait(child_pipe);
    }
    else
    {
        // wait till child process has performed flock operation.
        Wait(parent_pipe);
        const auto result = score::os::Fcntl::instance().flock(file_descriptor_,
                                                             Fcntl::Operation::kLockShared | Fcntl::Operation::kLockNB);
        ASSERT_TRUE(result.has_value());
        // unblock child process
        Signal(child_pipe);
        // wait for child process to exit. If parent process exits before child. Child process could become zombie.
        wait(nullptr);
    }
}

}  // namespace test
}  // namespace os
}  // namespace score
