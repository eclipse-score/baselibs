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
#include "score/os/utils/detect_os.h"

#include "score/os/mocklib/uname_mock.h"

#include <gtest/gtest.h>

using namespace ::score::os;

using ::testing::Return;

class DetectOsTest : public ::testing::Test
{
  public:
    DetectOsTest() {}
    ~DetectOsTest() override {}

  protected:
    score::os::UnameMock unameMock;
    virtual void SetUp() override
    {
        score::os::Uname::set_testing_instance(unameMock);
    }
    virtual void TearDown() override {}
};

TEST_F(DetectOsTest, IsEmpty)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "DetectOsTest Is Empty");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    ::score::cpp::optional<SystemInfo> retVal{};
    EXPECT_CALL(unameMock, GetUname()).WillRepeatedly(Return(retVal));
    ASSERT_FALSE(IsLinux());
    ASSERT_FALSE(IsQnx());
}

TEST_F(DetectOsTest, IsLinux)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "DetectOsTest Is Linux");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    SystemInfo info{};
    info.sysname = "Linux";
    ::score::cpp::optional<SystemInfo> retVal;
    EXPECT_CALL(unameMock, GetUname()).WillRepeatedly(Return(retVal.emplace(info)));
    ASSERT_TRUE(IsLinux());
    ASSERT_FALSE(IsQnx());
}

TEST_F(DetectOsTest, IsQnx)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "DetectOsTest Is Qnx");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    SystemInfo info{};
    info.sysname = "QNX";
    ::score::cpp::optional<SystemInfo> retVal;
    EXPECT_CALL(unameMock, GetUname()).WillRepeatedly(Return(retVal.emplace(info)));
    ASSERT_FALSE(IsLinux());
    ASSERT_TRUE(IsQnx());
}

TEST_F(DetectOsTest, IsWindows)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "DetectOsTest Is Windows");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    SystemInfo info{};
    info.sysname = "Windows";
    ::score::cpp::optional<SystemInfo> retVal;
    EXPECT_CALL(unameMock, GetUname()).WillRepeatedly(Return(retVal.emplace(info)));
    ASSERT_FALSE(IsLinux());
    ASSERT_FALSE(IsQnx());
}
