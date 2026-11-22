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
#include "score/os/inotify.h"
#include "score/os/errno.h"
#include "score/os/mocklib/inotify_mock.h"

#include <fcntl.h>
#include <sys/inotify.h>
#include <unistd.h>

#include <gtest/gtest.h>

namespace score
{
namespace os
{
namespace test
{

struct InotifyTest : ::testing::Test
{
    void SetUp() override
    {
        char tmp_dir_template[] = "dirent_test.XXXXXX";
        char* result = ::mkdtemp(tmp_dir_template);
        ASSERT_NE(result, nullptr);
        temp_dir_ = result;

        fd = score::os::Inotify::instance().inotify_init();
        EXPECT_TRUE(fd.has_value());
    };

    void TearDown() override
    {
        if (fd.has_value())
        {
            ::close(fd.value());
        }
        ASSERT_EQ(::rmdir(temp_dir_.c_str()), 0);
    }

    std::string temp_dir_;
    score::cpp::expected<int, score::os::Error> fd;
};

TEST_F(InotifyTest, InotifyInstanceHasCloseOnExecSet)
{
    RecordProperty("PartiallyVerifies", "comp_req__os__fd_close_on_exec");
    RecordProperty(
        "Description",
        "An inotify instance has close-on-exec set, so it is not inherited by processes created via execve()");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    // Given an inotify instance created in SetUp()
    ASSERT_TRUE(fd.has_value());

    // Then it has close-on-exec set
    const int flags{::fcntl(fd.value(), F_GETFD)};
    ASSERT_NE(flags, -1);
    EXPECT_NE(flags & FD_CLOEXEC, 0);
}

TEST(InotifyDefaultImplementation, InotifyInit1ReportsNotSupportedWhenNotOverridden)
{
    RecordProperty("PartiallyVerifies", "comp_req__os__fd_close_on_exec");
    RecordProperty("Description", "The default inotify_init1 of the interface reports ENOSYS");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    // Given a mock, which does not use the default implementation
    score::os::InotifyMock mock{};

    // When calling the default implementation of the interface explicitly
    const auto result = mock.score::os::Inotify::inotify_init1(score::os::Inotify::InitFlag::kNone);

    // Then it reports that the operation is not supported
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::createFromErrno(ENOSYS));
}

TEST_F(InotifyTest, InotifyInit1WithNoFlagsCreatesInheritableBlockingInstance)
{
    RecordProperty("PartiallyVerifies", "comp_req__os__fd_close_on_exec");
    RecordProperty("Description", "inotify_init1 without flags leaves close-on-exec and non-blocking unset");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    // When creating an instance without flags
    const auto instance{score::os::Inotify::instance().inotify_init1(score::os::Inotify::InitFlag::kNone)};
    ASSERT_TRUE(instance.has_value());
    const int fd_flags{::fcntl(instance.value(), F_GETFD)};
    const int status_flags{::fcntl(instance.value(), F_GETFL)};
    ::close(instance.value());

    // Then neither close-on-exec nor non-blocking is set
    ASSERT_NE(fd_flags, -1);
    ASSERT_NE(status_flags, -1);
    EXPECT_EQ(fd_flags & FD_CLOEXEC, 0);
    EXPECT_EQ(status_flags & O_NONBLOCK, 0);
}

TEST_F(InotifyTest, InotifyInit1WithCloseOnExecAndNonBlockingSetsBothFlags)
{
    RecordProperty("PartiallyVerifies", "comp_req__os__fd_close_on_exec");
    RecordProperty("Description", "inotify_init1 applies the requested close-on-exec and non-blocking flags");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    // When creating an instance with close-on-exec and non-blocking
    const auto instance{score::os::Inotify::instance().inotify_init1(score::os::Inotify::InitFlag::kCloseOnExec |
                                                                     score::os::Inotify::InitFlag::kNonBlocking)};
    ASSERT_TRUE(instance.has_value());
    const int fd_flags{::fcntl(instance.value(), F_GETFD)};
    const int status_flags{::fcntl(instance.value(), F_GETFL)};
    ::close(instance.value());

    // Then both flags are set
    ASSERT_NE(fd_flags, -1);
    ASSERT_NE(status_flags, -1);
    EXPECT_NE(fd_flags & FD_CLOEXEC, 0);
    EXPECT_NE(status_flags & O_NONBLOCK, 0);
}

TEST_F(InotifyTest, AddWatchSuccessfull)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "InotifyTest Add Watch Successfull");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");  // equivalence classes

    const auto wd = score::os::Inotify::instance().inotify_add_watch(
        fd.value(), temp_dir_.c_str(), score::os::Inotify::EventMask::kAccess);
    ASSERT_TRUE(wd.has_value());
}

TEST_F(InotifyTest, AddWatchFailsIfDirectoryDoesNotExist)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "InotifyTest Add Watch Fails If Directory Does Not Exist");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");  // equivalence classes

    const auto wd =
        score::os::Inotify::instance().inotify_add_watch(fd.value(), "/blah", score::os::Inotify::EventMask::kInCreate);
    ASSERT_FALSE(wd.has_value());
    EXPECT_EQ(wd.error(), score::os::Error::Code::kNoSuchFileOrDirectory);
}

TEST_F(InotifyTest, AddWatchFailsWithoutInit)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "InotifyTest Add Watch Fails Without Init");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");  // equivalence classes

    const auto wd = score::os::Inotify::instance().inotify_add_watch(
        0, temp_dir_.c_str(), score::os::Inotify::EventMask::kInCreate);
    ASSERT_FALSE(wd.has_value());
    // Error is not specified and thus OS specific
}

TEST_F(InotifyTest, AddWatchFailsWithEBADF)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "InotifyTest Add Watch Fails With EBADF");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");  // equivalence classes

    const auto wd = score::os::Inotify::instance().inotify_add_watch(
        -1, temp_dir_.c_str(), score::os::Inotify::EventMask::kInCreate);
    ASSERT_FALSE(wd.has_value());
    // Error is not specified and thus OS specific
}

TEST_F(InotifyTest, RemoveWatchSuccessfull)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "InotifyTest Remove Watch Successfull");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");  // equivalence classes

    const auto wd = score::os::Inotify::instance().inotify_add_watch(
        fd.value(), temp_dir_.c_str(), score::os::Inotify::EventMask::kInMovedTo);
    ASSERT_TRUE(wd.has_value());
    const auto ret = score::os::Inotify::instance().inotify_rm_watch(fd.value(), wd.value());
    ASSERT_TRUE(ret.has_value());
}

TEST_F(InotifyTest, RemoveWatchFailsWithoutInit)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "InotifyTest Remove Watch Fails Without Init");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");  // equivalence classes

    const auto ret = score::os::Inotify::instance().inotify_rm_watch(1, 1);
    ASSERT_FALSE(ret.has_value());
    // Error is not specified and thus OS specific
}

TEST_F(InotifyTest, RemoveWatchFailsWithEBADF)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "InotifyTest Remove Watch Fails With EBADF");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");  // equivalence classes

    const auto ret = score::os::Inotify::instance().inotify_rm_watch(-1, 0);
    ASSERT_FALSE(ret.has_value());
    EXPECT_EQ(ret.error(), score::os::Error::Code::kBadFileDescriptor);
}

TEST_F(InotifyTest, RemoveWatchFailsWithInvalidArguments)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "InotifyTest Remove Watch Fails With Invalid Arguments");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");  // equivalence classes

    const auto ret = score::os::Inotify::instance().inotify_rm_watch(0, 0);
    ASSERT_FALSE(ret.has_value());
    // Error is not specified and thus OS specific
}

}  // namespace test
}  // namespace os
}  // namespace score
