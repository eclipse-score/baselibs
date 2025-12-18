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
#include "score/os/qnx/unistd_impl.h"

#include <unistd.h>
#include <algorithm>
#include <vector>

#include "score/callback.hpp"
#include "score/memory.hpp"

#include <gtest/gtest.h>

namespace
{

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

// ------- Coverage of score/os/qnx/unistd_impl.h ------
class QnxUnistdImplFixture : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        unit_ = score::cpp::pmr::make_unique<score::os::qnx::QnxUnistdImpl>(score::cpp::pmr::get_default_resource());
    }

    score::cpp::pmr::unique_ptr<score::os::qnx::QnxUnistdImpl> unit_{};
};

TEST_F(QnxUnistdImplFixture, SetgroupspidReturnsErrorIfPassInvalidParams)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Set groups pid returns Error If Pass Invalid Params");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto val = unit_->setgroupspid(-1, nullptr, 0);
    ASSERT_FALSE(val.has_value());
}

TEST_F(QnxUnistdImplFixture, SetgroupspidNewGroupAdded)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Set groups pid New Group Added");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    ForkAndExpectTrue([]() noexcept {
        score::os::qnx::QnxUnistdImpl unistd_inst{};

        // getting the groups to pass them into setgroupspid
        const auto n_groups = ::getgroups(0, nullptr);
        if (n_groups == -1)
        {
            return false;
        }

        std::vector<gid_t> groups;

        if (n_groups > 0)
        {
            groups.resize(n_groups);
            ::getgroups(n_groups, groups.data());  // fill groups with data
        }

        const gid_t supplied_group_id = n_groups > 0 ? *std::max_element(groups.begin(), groups.end()) + 1 : 1;
        groups.push_back(supplied_group_id);
        // set +1 group in addition to existing
        const auto val = unistd_inst.setgroupspid(n_groups + 1, groups.data(), 0);
        if (val.has_value() == false)
        {
            return false;
        }
        // get updated amount of set groups
        const auto new_n_groups = ::getgroups(0, nullptr);
        return new_n_groups == (n_groups + 1);
    });
}

// ------- Coverage of score/os/unistd.h ------
class QnxUnistdFixture : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        unit_ = score::cpp::pmr::make_unique<score::os::internal::UnistdImpl>(score::cpp::pmr::get_default_resource());
    }

    score::cpp::pmr::unique_ptr<score::os::Unistd> unit_{};
};

// This UT isn't applicable for linux,
// reason: "Elevated privileges required for this test which are not available in Linux";
TEST_F(QnxUnistdFixture, SetuidChangesUidIfPassValidId)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Set uid Changes Uid If Pass Valid Id");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    ASSERT_EQ(::getuid(), 0);

    ForkAndExpectTrue([this]() noexcept {
        uid_t expected_uid{1};
        const auto val = unit_->setuid(expected_uid);
        return val.has_value() && (::getuid() == expected_uid);
    });
}

// This UT isn't applicable for linux,
// reason: "Elevated privileges required for this test which are not available in Linux";
TEST_F(QnxUnistdFixture, SetGidSetsGidIfPassValidId)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Set Gid Sets Gid If Pass Valid Id");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    ASSERT_EQ(::getuid(), 0);

    ForkAndExpectTrue([this]() noexcept {
        gid_t expected_gid{1};
        const auto val = unit_->setgid(expected_gid);
        return val.has_value() && (::getgid() == expected_gid);
    });
}

}  // namespace
