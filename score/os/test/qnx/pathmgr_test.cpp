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
#include "score/os/qnx/pathmgr.h"

#include "score/os/mocklib/qnx/mock_pathmgr.h"

#include <gtest/gtest.h>

namespace score
{
namespace os
{
namespace
{

TEST(PathmgrTest, PathmgrUnlinkReturnsErrorForMissingLink)
{
    const auto result = Pathmgr::instance().pathmgr_unlink("/tmp/non_existing_procmgr_link_for_test");
    ASSERT_FALSE(result.has_value());

    // QNX8 returns ENOENT; QNX7 returns ENOSYS for pathmgr_unlink on a non-existent link
    EXPECT_TRUE(result.error() == Error::Code::kNoSuchFileOrDirectory ||
                result.error() == Error::Code::kFileSystemDoesNotSupportTheOperation);
}

TEST(PathmgrTest, PathmgrSymlinkAndUnlink)
{
    constexpr auto kTarget = "/dev/shmem";
    constexpr auto kLink = "/tmp/pathmgr_symlink_test";

    const auto cleanup_result = Pathmgr::instance().pathmgr_unlink(kLink);
    static_cast<void>(cleanup_result);

    const auto symlink_result = Pathmgr::instance().pathmgr_symlink(kTarget, kLink);
    ASSERT_TRUE(symlink_result.has_value());

    const auto unlink_result = Pathmgr::instance().pathmgr_unlink(kLink);
    EXPECT_TRUE(unlink_result.has_value());
}

TEST(PathmgrTest, PathmgrSymlinkReturnsEbusyWhenAlreadyExists)
{
    constexpr auto kTarget = "/dev/shmem";
    constexpr auto kLink = "/tmp/pathmgr_symlink_ebusy_test";

    // Clean up any leftover from a previous run
    static_cast<void>(Pathmgr::instance().pathmgr_unlink(kLink));

    // First call must succeed
    const auto first = Pathmgr::instance().pathmgr_symlink(kTarget, kLink);
    ASSERT_TRUE(first.has_value());

    // Second call with the same link name must fail with EBUSY
    const auto second = Pathmgr::instance().pathmgr_symlink(kTarget, kLink);
    ASSERT_FALSE(second.has_value());
    EXPECT_EQ(second.error(), Error::Code::kDeviceOrResourceBusy);

    // Clean up
    static_cast<void>(Pathmgr::instance().pathmgr_unlink(kLink));
}

TEST(PathmgrTest, PathmgrSymlinkReturnsErrorForInvalidPath)
{
    // On QNX8, an empty path string causes ::pathmgr_symlink to fail with EINVAL.
    // On QNX7, it may succeed (different validation behavior).
    const auto result = Pathmgr::instance().pathmgr_symlink("", "/tmp/pathmgr_error_test");

    if (result.has_value())
    {
        // QNX7 behavior: empty path succeeded, clean up
        static_cast<void>(Pathmgr::instance().pathmgr_unlink("/tmp/pathmgr_error_test"));
    }
    else
    {
        // QNX8 behavior: empty path failed with EINVAL
        EXPECT_EQ(result.error(), Error::Code::kInvalidArgument);
    }
}

TEST(PathmgrTest, PathmgrSymlinkReturnsErrorViaObjSeam)
{
    // Use MockPathmgr injected via ObjectSeam to test error path coverage on both QNX7 and QNX8.
    MockPathmgr mock;
    EXPECT_CALL(mock, pathmgr_symlink(testing::_, testing::_))
        .WillOnce(testing::Return(score::cpp::make_unexpected(Error::createFromErrno(EINVAL))));

    // Set mock as testing instance
    Pathmgr::set_testing_instance(mock);

    const auto result = Pathmgr::instance().pathmgr_symlink("/dev/shmem", "/tmp/test");
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Error::Code::kInvalidArgument);

    // Restore real instance
    Pathmgr::restore_instance();
}

}  // namespace
}  // namespace os
}  // namespace score
