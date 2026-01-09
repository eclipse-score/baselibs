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
#include "score/os/posix/ftw.h"
#include "score/os/mocklib/mock_ftw.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory>

namespace
{

constexpr std::int32_t kDepth{10U};
constexpr std::int32_t kUserFlag{123U};

// Mock test

struct FtwMockTest : ::testing::Test
{
    score::os::MockFtw ftwmock;
};

TEST_F(FtwMockTest, ftw)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FtwMockTest ftw");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    EXPECT_CALL(ftwmock, ftw);
    ftwmock.ftw("/invalid_path", nullptr, 0);
}

// Tests of the real stuff

std::int32_t walk(const char*, const struct stat*, std::int32_t)
{
    return kUserFlag;
}

TEST(FtwTest, ftw_walk)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FtwTest ftw_walk");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::unique_ptr<score::os::Ftw> ftw_object = std::make_unique<score::os::FtwPosix>();
    constexpr auto dir_path = "/tmp/ftw_dir";
    mkdir(dir_path, 0755);
    close(open((std::string(dir_path) + "/file").c_str(), O_RDWR | O_CREAT, 0600));
    EXPECT_EQ(ftw_object->ftw(dir_path, walk, kDepth).value(), kUserFlag);
}

TEST(FtwTest, ftw_fail)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FtwTest ftw_fail");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::unique_ptr<score::os::Ftw> ftw_object = std::make_unique<score::os::FtwPosix>();
    constexpr auto dir_path = "/tmp/ftw_invalid_dir";
    EXPECT_EQ(ftw_object->ftw(dir_path, walk, kDepth).error(), score::os::Error::Code::kNoSuchFileOrDirectory);
}

}  // namespace
