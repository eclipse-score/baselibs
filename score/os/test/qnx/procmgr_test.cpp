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
#include "score/os/mocklib/qnx/mock_procmgr.h"
#include "score/os/mocklib/qnx/mock_procmgr_daemon_raw.h"
#include "score/os/qnx/procmgr_impl.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace
{

using ::testing::Return;
using ::testing::StrictMock;
using ::testing::Test;

constexpr pid_t kCurrentPid{0};
constexpr pid_t kValidHandle{30U};
constexpr pid_t kInvalidPid{INT_MAX};

// Mock test – uses ObjectSeam to inject a full MockProcMgr (tests the mock hook-up).
// Real-impl test – uses the ProcMgrImpl to test the procmgr behavior.
class ProcMgrImplTest : public Test
{
    void SetUp() override
    {
        score::os::ProcMgr::set_testing_instance(procmgrmock);
    };
    void TearDown() override
    {
        score::os::ProcMgr::restore_instance();
    };

  protected:
    score::os::ProcMgrImpl procmgr;
    score::os::MockProcMgr procmgrmock;
};

// Real-impl test – uses the link-time seam to mock only the raw ::procmgr_daemon
// QNX syscall, so ProcMgrImpl::procmgr_daemon is exercised end-to-end.
struct ProcMgrRealImplDaemonTest : public Test
{
    void SetUp() override
    {
        score::os::SetProcMgrDaemonRawMock(&daemon_raw_mock_);
    };
    void TearDown() override
    {
        score::os::SetProcMgrDaemonRawMock(nullptr);
    };

    StrictMock<score::os::MockProcMgrDaemonRaw> daemon_raw_mock_;
};

TEST_F(ProcMgrImplTest, procmgr_ability)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Ability");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    EXPECT_CALL(procmgrmock, procmgr_ability(kCurrentPid, PROCMGR_AID_EOL));
    score::os::ProcMgr::instance().procmgr_ability(kCurrentPid, PROCMGR_AID_EOL);
}

TEST_F(ProcMgrImplTest, procmgr_subrange_fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Subrange Fails");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    auto result = procmgr.procmgr_ability(kCurrentPid, PROCMGR_AID_EOL | PROCMGR_AOP_SUBRANGE);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ProcMgrImplTest, procmgr_generic_invalid_pid_fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Generic Invalid Pid Fails");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    auto result = procmgr.procmgr_ability(kInvalidPid, PROCMGR_AID_EOL);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ProcMgrImplTest, procmgr_invalid_ability_fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Invalid Ability Fails");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    auto result = procmgr.procmgr_ability(
        kCurrentPid, PROCMGR_ADN_ROOT | PROCMGR_AOP_ALLOW | PROCMGR_AID_PUBLIC_CHANNEL | PROCMGR_AID_UNCREATED);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ProcMgrImplTest, procmgr_generic_succeeds)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Generic Succeeds");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    auto result = procmgr.procmgr_ability(kCurrentPid, PROCMGR_ADN_ROOT | PROCMGR_AOP_ALLOW | PROCMGR_AID_EOL);
    EXPECT_TRUE(result.has_value());
}

TEST_F(ProcMgrImplTest, procmgr_specific_succeeds)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Specific Succeeds");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    auto result =
        procmgr.procmgr_ability(kCurrentPid, PROCMGR_ADN_ROOT | PROCMGR_AOP_ALLOW | PROCMGR_AID_PUBLIC_CHANNEL);
    EXPECT_TRUE(result.has_value());
}

TEST_F(ProcMgrImplTest, procmgr_ability_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Ability Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    auto result = procmgr.procmgr_ability(
        kCurrentPid,
        PROCMGR_ADN_NONROOT | PROCMGR_AOP_ALLOW | PROCMGR_AID_SPAWN_SETUID,
        PROCMGR_ADN_NONROOT | PROCMGR_AOP_SUBRANGE | PROCMGR_AOP_LOCK | PROCMGR_AID_SPAWN_SETUID,
        10000U,
        ~0U,
        PROCMGR_ADN_ROOT | PROCMGR_AOP_DENY | PROCMGR_AOP_LOCK | PROCMGR_AID_EOL);
    EXPECT_TRUE(result.has_value());
}

TEST_F(ProcMgrImplTest, procmgr_ability_failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Ability Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    auto result = procmgr.procmgr_ability(kInvalidPid, PROCMGR_AID_EOL, 0U, 0U, 0U, PROCMGR_AID_EOL);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kNoSuchProcess);
}

TEST_F(ProcMgrRealImplDaemonTest, procmgr_daemon_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Daemon Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    EXPECT_CALL(daemon_raw_mock_,
                procmgr_daemon(kCurrentPid,
                               PROCMGR_DAEMON_KEEPUMASK | PROCMGR_DAEMON_NOCHDIR | PROCMGR_DAEMON_NOCLOSE |
                                   PROCMGR_DAEMON_NODEVNULL))
        .WillOnce(Return(0));

    auto result = score::os::ProcMgr::instance().procmgr_daemon(
        kCurrentPid,
        PROCMGR_DAEMON_KEEPUMASK | PROCMGR_DAEMON_NOCHDIR | PROCMGR_DAEMON_NOCLOSE | PROCMGR_DAEMON_NODEVNULL);
    EXPECT_TRUE(result.has_value());
}

TEST_F(ProcMgrRealImplDaemonTest, procmgr_daemon_failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Daemon Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    EXPECT_CALL(daemon_raw_mock_, procmgr_daemon(kCurrentPid, 0U)).WillOnce(Return(-1));

    auto result = score::os::ProcMgr::instance().procmgr_daemon(kCurrentPid, 0U);
    EXPECT_FALSE(result.has_value());
    // ProcMgrImpl passes the return value (-1) to createFromErrno rather than errno,
    // so the error code maps to kUnexpected.
    EXPECT_EQ(result.error(), score::os::Error::Code::kUnexpected);
}

TEST_F(ProcMgrRealImplDaemonTest, procmgr_event_notify_add_failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Event Notify Add Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    sigevent event{};
    event.sigev_notify = SIGEV_PULSE;

    EXPECT_CALL(daemon_raw_mock_, procmgr_event_notify_add(0xFFFFFFFF, ::testing::_)).WillOnce(Return(-1));

    auto result = score::os::ProcMgr::instance().procmgr_event_notify_add(0xFFFFFFFF, &event);
    EXPECT_FALSE(result.has_value());
    // ProcMgrImpl passes the return value (-1) to createFromErrno rather than errno,
    // so the error code maps to kUnexpected.
    EXPECT_EQ(result.error(), score::os::Error::Code::kUnexpected);
}

TEST_F(ProcMgrRealImplDaemonTest, procmgr_event_notify_add_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Event Notify Add Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    sigevent event{};
    event.sigev_notify = SIGEV_PULSE;

    EXPECT_CALL(daemon_raw_mock_, procmgr_event_notify_add(PROCMGR_EVENT_PROCESS_CREATE, ::testing::_))
        .WillOnce(Return(kValidHandle));

    auto result = score::os::ProcMgr::instance().procmgr_event_notify_add(PROCMGR_EVENT_PROCESS_CREATE, &event);
    EXPECT_TRUE(result.has_value());
    if (result.has_value())
    {
        EXPECT_CALL(daemon_raw_mock_, procmgr_event_notify_delete(kValidHandle)).WillOnce(Return(0));

        // Clean up by deleting the notification
        score::os::ProcMgr::instance().procmgr_event_notify_delete(result.value());
    }
}

TEST_F(ProcMgrRealImplDaemonTest, procmgr_event_notify_delete_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Event Notify Delete Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    sigevent event{};
    event.sigev_notify = SIGEV_PULSE;

    EXPECT_CALL(daemon_raw_mock_, procmgr_event_notify_add(PROCMGR_EVENT_PROCESS_DEATH, ::testing::_))
        .WillOnce(Return(kValidHandle));
    EXPECT_CALL(daemon_raw_mock_, procmgr_event_notify_delete(kValidHandle)).WillOnce(Return(0));

    auto add_result = score::os::ProcMgr::instance().procmgr_event_notify_add(PROCMGR_EVENT_PROCESS_DEATH, &event);
    ASSERT_TRUE(add_result.has_value());

    auto delete_result = score::os::ProcMgr::instance().procmgr_event_notify_delete(add_result.value());
    EXPECT_TRUE(delete_result.has_value());
}

TEST_F(ProcMgrRealImplDaemonTest, procmgr_event_notify_delete_failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Event Notify Delete Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    sigevent event{};
    event.sigev_notify = SIGEV_PULSE;

    EXPECT_CALL(daemon_raw_mock_, procmgr_event_notify_add(PROCMGR_EVENT_PROCESS_DEATH, ::testing::_))
        .WillOnce(Return(kValidHandle));
    EXPECT_CALL(daemon_raw_mock_, procmgr_event_notify_delete(kValidHandle)).WillOnce(Return(0)).WillOnce(Return(-1));

    auto add_result = score::os::ProcMgr::instance().procmgr_event_notify_add(PROCMGR_EVENT_PROCESS_DEATH, &event);
    ASSERT_TRUE(add_result.has_value());

    // First delete should succeed
    auto first_delete = score::os::ProcMgr::instance().procmgr_event_notify_delete(add_result.value());
    EXPECT_TRUE(first_delete.has_value());

    // Second delete of same handle should fail (already deleted)
    auto second_delete = score::os::ProcMgr::instance().procmgr_event_notify_delete(add_result.value());
    EXPECT_FALSE(second_delete.has_value());
}

TEST_F(ProcMgrImplTest, procmgr_event_notify_add_failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Event Notify Add Failure with Nullptr Event");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    // Pass nullptr to trigger error
    auto result = procmgr.procmgr_event_notify_add(PROCMGR_EVENT_PROCESS_DEATH, nullptr);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ProcMgrImplTest, procmgr_value_notify_add_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Value Notify Add Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    sigevent event{};
    event.sigev_notify = SIGEV_PULSE;

    auto result = procmgr.procmgr_value_notify_add(PROCMGR_VALUE_FREE_MEM, 0, 0, &event);
    EXPECT_TRUE(result.has_value());
}

TEST_F(ProcMgrImplTest, procmgr_value_notify_add_failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Value Notify Add Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    sigevent event{};
    event.sigev_notify = SIGEV_PULSE;

    // Use invalid type value to trigger error
    auto result = procmgr.procmgr_value_notify_add(0xFFFFFFFF, 0, 0, &event);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ProcMgrImplTest, procmgr_value_notify_add_failure_nullptr_event)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Value Notify Add Failure with Nullptr Event");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    auto result = procmgr.procmgr_value_notify_add(PROCMGR_VALUE_FREE_MEM, 0, 0, nullptr);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ProcMgrImplTest, procmgr_value_current_success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Value Current Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    sigevent event{};
    event.sigev_notify = SIGEV_PULSE;

    auto add_result = procmgr.procmgr_value_notify_add(PROCMGR_VALUE_FREE_MEM, 0, 0, &event);
    ASSERT_TRUE(add_result.has_value());

    auto current_result = procmgr.procmgr_value_current(add_result.value());
    EXPECT_TRUE(current_result.has_value());
}

TEST_F(ProcMgrImplTest, procmgr_value_current_failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Procmgr Value Current Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    // Use that invalid ID to trigger error
    auto result = procmgr.procmgr_value_current(-999);
    EXPECT_FALSE(result.has_value());
}

}  // namespace
