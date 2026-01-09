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
#include "score/os/unistd.h"
#include "score/os/utils/signal_impl.h"

#include <score/callback.hpp>
#include <score/memory.hpp>

#include "gtest/gtest.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <chrono>
#include <climits>
#include <thread>

#if defined(__QNX__)
#include <sys/procmgr.h>
#endif

namespace score
{
namespace os
{
namespace test
{

namespace
{
constexpr int kInvalidFd = -1;

/// \brief File guard, handles opening specified file in constructor and releasing opened file
class OpenFileGuard
{
  public:
    OpenFileGuard(const std::string& path, const int open_flags, const int access) : path_{path}
    {
        fd_ = ::open(path.c_str(), open_flags, access);

        struct stat buffer{};
        stat_ = stat(path.c_str(), &buffer);  // file exists == 0
    }

    ~OpenFileGuard()
    {
        Release();
    }

    void Release()
    {
        ::close(fd_);
        ::unlink(path_.c_str());
    }

    int Fd() const noexcept
    {
        return fd_;
    }
    int Stat() const noexcept
    {
        return stat_;
    };
    std::string Path() const
    {
        return path_;
    }

  private:
    const std::string path_;
    int stat_;
    int fd_;
};

/// \brief Runs \p test in a child process created by \c fork()
/// \param test Callback, includes test body. Should return bool value: true - if test succeeded, false - test doesn't
///             match expectations
void ForkAndExpectTrue(score::cpp::callback<bool()> test)
{
    constexpr auto kForkFailed = -1;   // in case ::fork() returns failure
    constexpr auto kChildProcess = 0;  // ::fork() succeeds and takeover the control to child process

    const auto pid = ::fork();
    switch (pid)
    {
        case kForkFailed:
            GTEST_FAIL() << "Error when forking process. Could not run test.";
        case kChildProcess:
        {
            ::exit(test());
        }
        default:  // parent process
        {
            int status{};
            ::waitpid(pid, &status, 0);                              // wait for child completion
            ASSERT_TRUE(WIFEXITED(status));                          // check whether child process exit normally
            EXPECT_EQ(WEXITSTATUS(status), static_cast<int>(true));  // check exit code of child process
        }
    }
}

// Checks file descriptor
bool IsValidFd(std::int32_t fd)
{
    return fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}

class UnistdFixture : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        unit_ = score::cpp::pmr::make_unique<score::os::internal::UnistdImpl>(score::cpp::pmr::get_default_resource());
    }

    score::cpp::pmr::unique_ptr<score::os::Unistd> unit_{};
};

TEST_F(UnistdFixture, CloseFileDescriptor)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Close File Descriptor");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // Given some file
    constexpr auto path = "close_test_file";
    const auto fd = ::open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    struct stat buffer{};
    ASSERT_EQ(stat(path, &buffer), 0);  // file exists
    ASSERT_TRUE(IsValidFd(fd));

    // When closing the file descriptor
    unit_->close(fd);

    // Then the file descriptor gets invalidated
    ASSERT_FALSE(IsValidFd(fd));
}

TEST_F(UnistdFixture, UnlinkRemovesFile)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Unlink Removes File");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // Given some file without a reference count
    constexpr auto path = "unlink_test_file";
    const auto fd = ::open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    struct stat buffer{};
    ASSERT_EQ(stat(path, &buffer), 0);  // file exists
    close(fd);

    // When calling unlink
    unit_->unlink(path);

    // Then the file gets removed
    ASSERT_EQ(stat(path, &buffer), -1);  // file does not exist
}

TEST_F(UnistdFixture, UnlinkReturnsErrorIfNonExistingPath)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Unlink Returns Error If Non Existing Path");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // Given some non-existing file
    constexpr auto path = "/tmp/some_non_existing_file";

    const auto val = unit_->unlink(path);
    const auto expected = score::os::Error::createFromErrno(ENOENT);  // score::os::Error::Code::kNoSuchFileOrDirectory

    EXPECT_EQ(val.error(), expected);
}

TEST_F(UnistdFixture, PipeOpensWithoutError)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Pipe Opens Without Error");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    int32_t fds[2] = {};
    const auto val = unit_->pipe(&fds[0]);
    if (val.has_value())
    {
        ::close(fds[0]);
        ::close(fds[1]);
    }
    EXPECT_TRUE(val.has_value());
}

TEST_F(UnistdFixture, DupReturnsErrorIfPassInvalidFd)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Dup Returns Error If Pass Invalid Fd");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto val = unit_->dup(kInvalidFd);
    EXPECT_FALSE(val.has_value());
}

TEST_F(UnistdFixture, DupReturnsNoErrorIfPassValidFd)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Dup Returns No Error If Pass Valid Fd");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::int32_t fds = 1;
    const auto val = unit_->dup(fds);
    EXPECT_TRUE(val.has_value());
    EXPECT_GT(val.value(), fds);
}

TEST_F(UnistdFixture, Dup2ReturnsErrorIfPassInvalidFd)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Dup2Returns Error If Pass Invalid Fd");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto val = unit_->dup2(kInvalidFd, kInvalidFd);
    EXPECT_FALSE(val.has_value());
}

TEST_F(UnistdFixture, ReadReturnsErrorIfPassInvalidFd)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Read Returns Error If Pass Invalid Fd");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr size_t buf_size = 32;
    char buf[buf_size] = {};

    const auto val = unit_->read(kInvalidFd, static_cast<void*>(buf), buf_size);
    EXPECT_FALSE(val.has_value());
}

TEST_F(UnistdFixture, PReadReturnsErrorIfPassInvalidFd)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture PRead Returns Error If Pass Invalid Fd");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr size_t buf_size = 32;
    char buf[buf_size] = {};

    const auto val = unit_->pread(kInvalidFd, static_cast<void*>(buf), buf_size, 0);
    EXPECT_FALSE(val.has_value());
}

TEST_F(UnistdFixture, PReadReturnsNonErrorIfPassValidFd)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture PRead Returns Non Error If Pass Valid Fd");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr size_t buf_size = 32;
    char buf[buf_size] = {};
    constexpr auto path = "pread_test_file";

    const OpenFileGuard file_guard{path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR};
    ASSERT_EQ(file_guard.Stat(), 0);  // file exists

    const auto val = unit_->pread(file_guard.Fd(), static_cast<void*>(buf), buf_size, 0);

    EXPECT_TRUE(val.has_value());
}

TEST_F(UnistdFixture, WriteReturnsErrorIfPassInvalidFd)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Write Returns Error If Pass Invalid Fd");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr size_t buf_size = 32;
    char buf[buf_size] = {};

    const auto val = unit_->write(kInvalidFd, static_cast<void*>(buf), buf_size);
    EXPECT_FALSE(val.has_value());
}

TEST_F(UnistdFixture, PWriteReturnsNonErrorIfPassPassValidFd)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture PWrite Returns Non Error If Pass Pass Valid Fd");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr size_t buf_size = 32;
    char buf[buf_size] = {};
    constexpr auto path = "pwrite_test_file";

    const OpenFileGuard file_guard{path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR};
    ASSERT_EQ(file_guard.Stat(), 0);  // file exists

    const auto val = unit_->pwrite(file_guard.Fd(), static_cast<void*>(buf), buf_size, 0);

    EXPECT_TRUE(val.has_value());
}

TEST_F(UnistdFixture, PWriteReturnsErrorIfPassInvalidFd)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture PWrite Returns Error If Pass Invalid Fd");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr size_t buf_size = 32;
    char buf[buf_size] = {};
    const auto val = unit_->pwrite(kInvalidFd, static_cast<void*>(buf), buf_size, 0);

    EXPECT_FALSE(val.has_value());
}

TEST_F(UnistdFixture, LSeekReturnsErrorIfPassInvalidFd)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture LSeek Returns Error If Pass Invalid Fd");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto val = unit_->lseek(kInvalidFd, 0, 0);
    EXPECT_FALSE(val.has_value());
}

TEST_F(UnistdFixture, LSeekReturnsNonErrorIfPassValidFd)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture LSeek Returns Non Error If Pass Valid Fd");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr auto path = "lseek_test_file";

    const OpenFileGuard file_guard{path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR};
    ASSERT_EQ(file_guard.Stat(), 0);  // file exists

    const auto val = unit_->lseek(file_guard.Fd(), 0, 0);

    EXPECT_TRUE(val.has_value());
}

TEST_F(UnistdFixture, FTruncateReturnsErrorIfPassInvalidFd)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture FTruncate Returns Error If Pass Invalid Fd");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto val = unit_->ftruncate(-1, 0);
    EXPECT_FALSE(val.has_value());
}

TEST_F(UnistdFixture, FTruncateNonErrorIfPassValidFd)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture FTruncate Non Error If Pass Valid Fd");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr auto path = "ftruncate_test_file";
    const OpenFileGuard file_guard{path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR};
    ASSERT_EQ(file_guard.Stat(), 0);  // file exists

    const auto val = unit_->ftruncate(file_guard.Fd(), 0);

    EXPECT_TRUE(val.has_value());
}

TEST_F(UnistdFixture, GetUiIdMatchSystemGetuid)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Get Ui Id Match System Getuid");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_EQ(unit_->getuid(), ::getuid());
}

TEST_F(UnistdFixture, GetGidMatchSystemGetGid)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Get Gid Match System Get Gid");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_EQ(unit_->getgid(), ::getgid());
}

TEST_F(UnistdFixture, GetPidMatchSystemGetPid)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Get Pid Match System Get Pid");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_EQ(unit_->getpid(), ::getpid());
}

TEST_F(UnistdFixture, GetPpidMatchSystemGetPpid)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Get Ppid Match System Get Ppid");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_EQ(unit_->getppid(), ::getppid());
}

TEST_F(UnistdFixture, SetuidNotChangesUidIfPassInvalidId)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Setuid Not Changes Uid If Pass Invalid Id");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    ForkAndExpectTrue([this]() noexcept {
#if defined(__QNX__)
        ::setuid(1);
#endif
        uid_t expected_uid{::getuid()};
        const auto val = unit_->setuid(0);
        return !val.has_value() && (::getuid() == expected_uid);
    });
}

TEST_F(UnistdFixture, SetGidNotChangesGidIfPassInvalidId)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Set Gid Not Changes Gid If Pass Invalid Id");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    ForkAndExpectTrue([this]() noexcept {
        const gid_t expected_gid{::getgid()};
#if defined(__QNX__)
        const auto remove_setgid_capability = PROCMGR_AID_SETGID | PROCMGR_AOP_DENY | PROCMGR_ADN_ROOT;
        const auto cap_result = procmgr_ability(0, remove_setgid_capability, PROCMGR_AID_EOL);
        EXPECT_EQ(cap_result, 0);
#endif
        const auto val = unit_->setgid(expected_gid + 1);
        return !val.has_value() && (::getgid() == expected_gid);
    });
}

TEST_F(UnistdFixture, ReadLinkReturnsErrorIfPassEmptyPath)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Read Link Returns Error If Pass Empty Path");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    char buf[4096] = {};
    const auto val = unit_->readlink("", &buf[0], sizeof(buf));

    EXPECT_FALSE(val.has_value());
}

// Test case for the readlink function
TEST_F(UnistdFixture, ReadLinkReturnsNoErrorIfPassValidPath)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Read Link Returns No Error If Pass Valid Path");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const char* target = "/etc/passwd";
    const char* symlinkPath = "test_symlink";

    // Create a symbolic link
    int result = symlink(target, symlinkPath);
    ASSERT_EQ(result, 0) << "Failed to create symlink: " << strerror(errno);

    // Read the symbolic link
    char buffer[4096];
    const auto val = unit_->readlink(symlinkPath, buffer, sizeof(buffer) - 1);
    EXPECT_TRUE(val.has_value());

    // Null-terminate the string
    buffer[val.value()] = '\0';

    // Verify the target of the symbolic link
    EXPECT_STREQ(buffer, target);

    // Clean up
    unlink(symlinkPath);
}

TEST_F(UnistdFixture, FSyncReturnsErrorIfPassInvalidFd)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture FSync Returns Error If Pass Invalid Fd");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto val = unit_->fsync(-1);
    EXPECT_FALSE(val.has_value());
}

TEST_F(UnistdFixture, FSyncReturnsNonErrorIfPassValidFd)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture FSync Returns Non Error If Pass Valid Fd");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr auto path = "fsync_test_file";
    const OpenFileGuard file_guard{path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR};
    ASSERT_EQ(file_guard.Stat(), 0);  // file exists

    const auto val = unit_->fsync(file_guard.Fd());

    EXPECT_TRUE(val.has_value());
}

TEST_F(UnistdFixture, FDataSyncReturnsNonErrorIfPassInvalidFd)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture FData Sync Returns Non Error If Pass Invalid Fd");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto val = unit_->fdatasync(-1);
    EXPECT_FALSE(val.has_value());
}

TEST_F(UnistdFixture, FDataSyncReturnsNonErrorIfPassValidFd)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture FData Sync Returns Non Error If Pass Valid Fd");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr auto path = "fdata_sync_test_file";
    const OpenFileGuard file_guard{path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR};
    ASSERT_EQ(file_guard.Stat(), 0);  // file exists

    const auto val = unit_->fdatasync(file_guard.Fd());

    EXPECT_TRUE(val.has_value());
}

TEST_F(UnistdFixture, NanosleepReturnsNonErrorIfPassValidSleepParam)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Nanosleep Returns Non Error If Pass Valid Sleep Param");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    timespec req{0, 10};
    const auto val = unit_->nanosleep(&req, nullptr);

    EXPECT_TRUE(val.has_value());
}

TEST_F(UnistdFixture, NanosleepReturnsErrorIfPassInvalidSleepParam)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Nanosleep Returns Error If Pass Invalid Sleep Param");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    timespec req{0, -10};
    const auto val = unit_->nanosleep(&req, nullptr);
    EXPECT_FALSE(val.has_value());
}

TEST_F(UnistdFixture, SysconfReturnsErrorIfPassInvalidParam)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Sysconf Returns Error If Pass Invalid Param");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto val = unit_->sysconf(kInvalidFd);
    EXPECT_FALSE(val.has_value());
}

TEST_F(UnistdFixture, SysconfReturnsNonErrorIfPassValidParam)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Sysconf Returns Non Error If Pass Valid Param");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto val = unit_->sysconf(_SC_ARG_MAX);
    EXPECT_TRUE(val.has_value());
}

TEST_F(UnistdFixture, LinkReturnsErrorIfPassEmptyPath)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Link Returns Error If Pass Empty Path");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto val = unit_->link("", "");
    EXPECT_FALSE(val.has_value());
}

TEST_F(UnistdFixture, LinkReturnsNonErrorIfPassValidPath)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Link Returns Non Error If Pass Valid Path");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr auto path = "link_test_file";
    constexpr auto path_link = "link_test_file_link";
    const OpenFileGuard file_guard{path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR};
    ASSERT_EQ(file_guard.Stat(), 0);  // file exists

    const auto val = unit_->link(path, path_link);
    ASSERT_TRUE(val.has_value());
    ::unlink(path_link);
}

TEST_F(UnistdFixture, SymlinkReturnsErrorIfPassEmptyPath)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Symlink Returns Error If Pass Empty Path");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto val = unit_->symlink("", "");
    EXPECT_FALSE(val.has_value());
}

TEST_F(UnistdFixture, SymlinkReturnsNonErrorIfPassValidPath)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Symlink Returns Non Error If Pass Valid Path");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr auto path_link = "symlink_test_file_link";
    // create file
    const OpenFileGuard file_guard{"symlink_test_file", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR};
    ASSERT_EQ(file_guard.Stat(), 0);

    const auto val = unit_->symlink(file_guard.Path().c_str(), path_link);
    ASSERT_TRUE(val.has_value());
    ::unlink(path_link);
}

TEST_F(UnistdFixture, ChdirReturnsErrorIfPassEmptyPath)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Chdir Returns Error If Pass Empty Path");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto val = unit_->chdir("");
    EXPECT_FALSE(val.has_value());
}

TEST_F(UnistdFixture, ChdirReturnsNonErrorIfPassValidPath)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Chdir Returns Non Error If Pass Valid Path");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto val = unit_->chdir(".");
    EXPECT_TRUE(val.has_value());
}

TEST_F(UnistdFixture, ChownReturnsErrorIfPassInvalidParams)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Chown Returns Error If Pass Invalid Params");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto val = unit_->chown("", 0, 0);
    EXPECT_FALSE(val.has_value());
}

TEST_F(UnistdFixture, ChownReturnsNonErrorIfPassValidParams)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Chown Returns Non Error If Pass Valid Params");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const OpenFileGuard file_guard{"chown_test_file", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR};
    ASSERT_EQ(file_guard.Stat(), 0);

    const auto uid = ::getuid();
    const auto gid = ::getgid();

    const auto val = unit_->chown(file_guard.Path().c_str(), uid, gid);
    EXPECT_TRUE(val.has_value());
}

TEST_F(UnistdFixture, GetcwdReturnsErrorIfPassNullBuffer)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Getcwd Returns Error If Pass Null Buffer");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    char buffer{};
    const auto val = unit_->getcwd(&buffer, 0);
    EXPECT_FALSE(val.has_value());
}

TEST_F(UnistdFixture, GetcwdReturnsNonErrorIfPassAllocatedBuffer)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Getcwd Returns Non Error If Pass Allocated Buffer");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    char buf[4096] = {};
    const auto val = unit_->getcwd(buf, sizeof(buf));
    EXPECT_TRUE(val.has_value());
}

TEST_F(UnistdFixture, AccessMatchesReadWriteAccessForExistingFile)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Access Matches Read Write Access For Existing File");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // Given some file
    constexpr auto path = "access_test_file";
    const OpenFileGuard file_guard{path, O_RDWR | O_CREAT, 0600};
    ASSERT_EQ(file_guard.Stat(), 0);  // file exists

    ASSERT_TRUE(IsValidFd(file_guard.Fd()));

    // access with F_OK should succeed
    EXPECT_TRUE(unit_->access(path, score::os::Unistd::AccessMode::kExists).has_value());
    // access with R_OK should succeed
    EXPECT_TRUE(unit_->access(path, score::os::Unistd::AccessMode::kRead).has_value());
    // access with W_OK should succeed
    EXPECT_TRUE(unit_->access(path, score::os::Unistd::AccessMode::kWrite).has_value());
    // access with R_OK and W_OK should succeed
    EXPECT_TRUE(
        unit_->access(path, score::os::Unistd::AccessMode::kRead | score::os::Unistd::AccessMode::kWrite).has_value());
    // access with X_OK should fail
    EXPECT_FALSE(unit_->access(path, score::os::Unistd::AccessMode::kExec).has_value());
}

TEST_F(UnistdFixture, AccessReturnsErrorIfPassNonExistingFile)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Access Returns Error If Pass Non Existing File");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // Given some non existing file
    constexpr auto path = "non_existing_file";

    struct stat buffer{};
    ASSERT_NE(stat(path, &buffer), 0);  // file does not exist

    // access with F_OK should fail
    EXPECT_FALSE(unit_->access(path, score::os::Unistd::AccessMode::kExists).has_value());
    // access with R_OK should fail
    EXPECT_FALSE(unit_->access(path, score::os::Unistd::AccessMode::kRead).has_value());
    // access with W_OK should fail
    EXPECT_FALSE(unit_->access(path, score::os::Unistd::AccessMode::kWrite).has_value());
    // access with X_OK should fail
    EXPECT_FALSE(unit_->access(path, score::os::Unistd::AccessMode::kExec).has_value());
}

TEST_F(UnistdFixture, AccessReturnsNonErrorForExistingFileWithReadWriteAccess)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Access Returns Non Error For Existing File With Read Write Access");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // Given some file
    constexpr auto path = "unistd_access_file";
    const OpenFileGuard file_guard{path, O_RDWR | O_CREAT, 0600};

    ASSERT_EQ(file_guard.Stat(), 0);  // file exists
    ASSERT_TRUE(IsValidFd(file_guard.Fd()));

    // access with kExists should succeed
    EXPECT_TRUE(unit_->access(path, score::os::Unistd::AccessMode::kExists).has_value());
    // access with kRead should succeed
    EXPECT_TRUE(unit_->access(path, score::os::Unistd::AccessMode::kRead).has_value());
    // access with kWrite should succeed
    EXPECT_TRUE(unit_->access(path, score::os::Unistd::AccessMode::kWrite).has_value());
    // access with kRead|kWrite should succeed
    EXPECT_TRUE(
        unit_->access(path, score::os::Unistd::AccessMode::kRead | score::os::Unistd::AccessMode::kWrite).has_value());
    // access with kExec should fail
    EXPECT_FALSE(unit_->access(path, score::os::Unistd::AccessMode::kExec).has_value());
}

TEST_F(UnistdFixture, UnistdAccessReturnsErrorIfPassNonExistingFile)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Unistd Access Returns Error If Pass Non Existing File");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // Given some non existing file
    constexpr auto path = "non_existent_file";

    struct stat buffer{};
    ASSERT_NE(stat(path, &buffer), 0);  // file does not exist

    // access with kExists should fail
    EXPECT_FALSE(unit_->access(path, score::os::Unistd::AccessMode::kExists).has_value());
    // access with kRead should fail
    EXPECT_FALSE(unit_->access(path, score::os::Unistd::AccessMode::kRead).has_value());
    // access with kWrite should fail
    EXPECT_FALSE(unit_->access(path, score::os::Unistd::AccessMode::kWrite).has_value());
    // access with kExec should fail
    EXPECT_FALSE(unit_->access(path, score::os::Unistd::AccessMode::kExec).has_value());
}

TEST_F(UnistdFixture, UnistdGettidReturnsPositiveTid)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Unistd Gettid Returns Positive Tid");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_GT(unit_->gettid(), 0);
}

TEST_F(UnistdFixture, UnistdAlarmSetsAndReportsPendingAlarm)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Unistd Alarm Sets And Reports Pending Alarm");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const std::uint32_t seconds = 10;
    EXPECT_EQ(unit_->alarm(seconds), 0);
    const auto result = unit_->alarm(0);
    // The value returned may be rounded down to the nearest second
    EXPECT_TRUE((result == seconds - 1) || (result == seconds));
}

TEST_F(UnistdFixture, UnistdAlarmTriggersInExpectedTime)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Unistd Alarm Triggers In Expected Time");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const std::uint32_t seconds = 1;
    static bool triggered = false;

    score::os::SignalImpl sig{};
    sig.signal(SIGALRM, [](int) {
        triggered = true;
    });
    EXPECT_EQ(unit_->alarm(seconds), 0);
    std::this_thread::sleep_for(std::chrono::seconds{seconds} + std::chrono::milliseconds{100});
    EXPECT_TRUE(triggered);

    triggered = false;
}

TEST(Unistd, DefaultShallReturnImplInstance)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Unistd Default Shall Return Impl Instance");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto default_instance = score::os::Unistd::Default();
    ASSERT_TRUE(default_instance != nullptr);
    EXPECT_NO_THROW(std::ignore = dynamic_cast<score::os::internal::UnistdImpl*>(default_instance.get()));
}

TEST_F(UnistdFixture, CloseReturnsErrIfPassInvalidParam)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Close Returns Err If Pass Invalid Param");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto val = unit_->close(kInvalidFd);
    EXPECT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), score::os::Error::Code::kBadFileDescriptor);
}

TEST_F(UnistdFixture, Dup2ReturnsNoErrorIfPassValidParam)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Dup2Returns No Error If Pass Valid Param");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::int32_t fds[2] = {1, 2};
    const auto val_1 = unit_->pipe(&fds[0]);
    const auto val_2 = unit_->dup2(fds[0], fds[1]);
    if (val_1.has_value())
    {
        ::close(fds[0]);
        ::close(fds[1]);
    }
    EXPECT_TRUE(val_2.has_value());
    EXPECT_EQ(val_2.value(), fds[1]);
}

TEST_F(UnistdFixture, ReadReturnsNoErrorIfPassValidFd)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Read Returns No Error If Pass Valid Fd");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // first write something to the file

    constexpr auto path = "read_test_file";
    const auto fd_write = ::open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    struct stat write_buffer{};
    ASSERT_EQ(stat(path, &write_buffer), 0);  // file exists

    constexpr size_t buf_size = 32;
    char buf[buf_size] = {};
    strcpy(buf, "writing to file");

    const auto write_val = ::write(fd_write, buf, buf_size);
    EXPECT_EQ(write_val, buf_size);
    ::close(fd_write);

    // Now open the same file to read the content written earlier
    const auto fd_read = ::open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    struct stat read_buffer{};
    ASSERT_EQ(stat(path, &read_buffer), 0);  // file exists

    char read_value[buf_size] = {};
    const auto val = unit_->read(fd_read, read_value, buf_size);

    ::close(fd_read);
    ::unlink(path);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), buf_size);
    EXPECT_EQ(strcmp(read_value, buf), 0);
}

TEST_F(UnistdFixture, WriteReturnNoErrorIfPassValidFd)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Write Return No Error If Pass Valid Fd");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr auto path = "write_test_file";
    const auto fd_write = ::open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    struct stat write_buffer{};
    ASSERT_EQ(stat(path, &write_buffer), 0);

    constexpr size_t buf_size = 32;
    char write_val[buf_size] = {};
    strcpy(write_val, "writing to file");

    const auto val = unit_->write(fd_write, write_val, buf_size);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), buf_size);

    ::close(fd_write);

    const auto fd_read = ::open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    char read_value[buf_size] = {};
    ::read(fd_read, read_value, buf_size);
    ::close(fd_read);
    ::unlink(path);
    EXPECT_EQ(strcmp(read_value, write_val), 0);
}

TEST_F(UnistdFixture, SetuidReturnsErrorIfPassInvalidUid)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Setuid Returns Error If Pass Invalid Uid");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const uid_t uid_before_set = unit_->getuid();
    const uid_t invalid_id = UINT_MAX;
    const auto val = unit_->setuid(invalid_id);
    EXPECT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), score::os::Error::Code::kInvalidArgument);
    const uid_t uid_after_set = unit_->getuid();
    EXPECT_EQ(uid_after_set, uid_before_set);
}

TEST_F(UnistdFixture, SetuidReturnsNoErrorIfPassValidID)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Setuid Returns No Error If Pass Valid ID");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

#if defined __QNX__
    ForkAndExpectTrue([this]() noexcept {
        ::setuid(0);
        uid_t uid_before_set = ::getuid();
        uid_t expected_uid{10};
        const auto val = unit_->setuid(expected_uid);
        uid_t uid_after_set = ::getuid();
        return val.has_value() && (uid_after_set == expected_uid) && (uid_before_set != uid_after_set);
    });
#endif
}

TEST_F(UnistdFixture, WriteReturnNoErrorAndSyncIfPassValidFd)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "UnistdFixture Write Return No Error And Sync If Pass Valid Fd");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr auto path = "write_test_file";
    const auto fd_write = ::open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    struct stat write_buffer{};
    ASSERT_EQ(stat(path, &write_buffer), 0);

    constexpr size_t buf_size = 32;
    char write_val[buf_size] = {};
    strcpy(write_val, "writing to file");

    const auto val = unit_->write(fd_write, write_val, buf_size);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), buf_size);
    auto result = unit_->sync();
    EXPECT_EQ(result, score::cpp::expected_blank<score::os::Error>{});
    ::close(fd_write);

    const auto fd_read = ::open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    char read_value[buf_size] = {};
    ::read(fd_read, read_value, buf_size);
    ::close(fd_read);
    ::unlink(path);
    EXPECT_EQ(strcmp(read_value, write_val), 0);
}

TEST(Unistd, PMRDefaultShallReturnImplInstance)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Unistd PMRDefault Shall Return Impl Instance");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    score::cpp::pmr::memory_resource* memory_resource = score::cpp::pmr::get_default_resource();
    const auto default_instance = score::os::Unistd::Default(memory_resource);
    ASSERT_TRUE(default_instance != nullptr);
    EXPECT_NO_THROW(std::ignore = dynamic_cast<score::os::internal::UnistdImpl*>(default_instance.get()));
}

}  // namespace
}  // namespace test
}  // namespace os
}  // namespace score
