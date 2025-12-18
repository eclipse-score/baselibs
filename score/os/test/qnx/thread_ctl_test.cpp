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
///
/// @file
/// @copyright Copyright (C) 2023, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///

#include "score/os/qnx/thread_ctl_impl.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace
{

struct ThreadCtlTest : ::testing::Test
{
  protected:
    void SetUp() override
    {
        thread_ctl_ = std::make_unique<score::os::qnx::ThreadCtlImpl>();
        ASSERT_TRUE(thread_ctl_);
    };

    std::unique_ptr<score::os::qnx::ThreadCtlImpl> thread_ctl_{};
};

TEST_F(ThreadCtlTest, succeed_reading_current_thread_name)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Succeed Reading Current Thread Name");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const pid_t pid{0};
    const int32_t tid{0};
    struct _thread_name tname = {};
    tname.name_buf_len = _NTO_THREAD_NAME_MAX;
    tname.new_name_len = -1;

    const auto result = thread_ctl_->ThreadCtlExt(pid, tid, _NTO_TCTL_NAME, &tname);
    EXPECT_TRUE(result.has_value());
}

TEST_F(ThreadCtlTest, fails_on_invalid_pid_tid_combination)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Fails On Invalid Pid Tid Combination");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const pid_t pid{1};
    const int32_t tid{0};
    struct _thread_name tname = {};

    const auto result = thread_ctl_->ThreadCtlExt(pid, tid, _NTO_TCTL_NAME, &tname);
    EXPECT_FALSE(result.has_value());
}

}  // namespace
