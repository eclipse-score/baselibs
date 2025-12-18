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
#include "score/os/qnx/devctl.h"
#include "score/os/fcntl.h"
#include "score/os/mocklib/qnx/mock_devctl.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <array>

#include <sys/dcmd_all.h>

using ::testing::_;
using ::testing::Eq;
using ::testing::Pointee;
using ::testing::Return;

// Unit tests for mocked class
struct DevctlTestMock : ::testing::Test
{
    void SetUp() override
    {
        score::os::Devctl::set_testing_instance(mock_devctl);
    };
    void TearDown() override
    {
        score::os::Devctl::restore_instance();
    };

    score::os::MockDevctl mock_devctl;
};

TEST_F(DevctlTestMock, TestFunction_Devctl)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Function Devctl");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::array<std::uint8_t, 3> dev_data{1, 2, 3};
    std::int32_t dev_info{};
    std::array<std::uint8_t, 3>* arg_ptr{};

    auto& expect_devctl = EXPECT_CALL(mock_devctl, devctl(Eq(1), Eq(2), _, Eq(3), Pointee(dev_info)));
    expect_devctl.WillOnce(
        [&arg_ptr](
            const std::int32_t, const std::int32_t, void* const dev_data_ptr, const std::size_t, std::int32_t* const) {
            arg_ptr = static_cast<std::array<std::uint8_t, 3>*>(dev_data_ptr);
            return score::cpp::expected_blank<score::os::Error>{};
        });

    expect_devctl.WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EOVERFLOW))));

    auto res = score::os::Devctl::instance().devctl(1, 2, &dev_data, 3, &dev_info);
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(*arg_ptr, dev_data);

    res = score::os::Devctl::instance().devctl(1, 2, &dev_data, 3, &dev_info);
    ASSERT_FALSE(res.has_value());
}

TEST_F(DevctlTestMock, TestFunction_Devctlv)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Function Devctlv");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::array<std::uint8_t, 3> vec_data_1{1, 2, 3};
    iovec s_vec[1];
    iovec r_vec{};
    SETIOV(&s_vec[0], &vec_data_1, sizeof(vec_data_1));
    std::int32_t dev_info{};

    auto& expect_devctl =
        EXPECT_CALL(mock_devctl, devctlv(Eq(1), Eq(2), Eq(3), Eq(4), Eq(s_vec), Eq(&r_vec), Pointee(dev_info)));
    expect_devctl.WillOnce(Return(score::cpp::expected_blank<score::os::Error>{}));
    expect_devctl.WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EOVERFLOW))));

    auto res = score::os::Devctl::instance().devctlv(1, 2, 3, 4, s_vec, &r_vec, &dev_info);
    ASSERT_TRUE(res.has_value());

    res = score::os::Devctl::instance().devctlv(1, 2, 3, 4, s_vec, &r_vec, &dev_info);
    ASSERT_FALSE(res.has_value());
}

// Unit tests for QNX devctl() calls
using OpenFlag = score::os::Fcntl::Open;
using ModeFlag = score::os::Stat::Mode;

struct DevctlTestQnx : ::testing::Test
{
    void SetUp() override
    {
        if (score::os::Fcntl::instance().open(kTestFilePath.c_str(), OpenFlag::kReadOnly))
        {
            ASSERT_EQ(std::remove(kTestFilePath.c_str()), 0);
        }
    };
    void TearDown() override
    {
        if (score::os::Fcntl::instance().open(kTestFilePath.c_str(), OpenFlag::kReadOnly))
        {
            ASSERT_EQ(std::remove(kTestFilePath.c_str()), 0);
        }
    };
    const std::string kTestFilePath{"/tmp/devctl_unit_test_file"};
};

TEST_F(DevctlTestQnx, TestDevctl_GetFlags_Success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Devctl: Get Flags Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const OpenFlag open_flags{OpenFlag::kReadOnly | OpenFlag::kNonBlocking | OpenFlag::kCreate};
    const ModeFlag mode_flags{ModeFlag::kReadUser | ModeFlag::kWriteUser};
    std::int32_t expect_flags_set{O_NONBLOCK};
    std::int32_t result_flags{};

    auto file_desc = score::os::Fcntl::instance().open(kTestFilePath.c_str(), open_flags, mode_flags);
    ASSERT_TRUE(file_desc.has_value());

    auto res_devctl = score::os::Devctl::instance().devctl(
        file_desc.value(), DCMD_ALL_GETFLAGS, &result_flags, sizeof(std::int32_t), nullptr);
    ASSERT_TRUE(res_devctl.has_value());

    result_flags &= O_SETFLAG;
    ASSERT_TRUE(result_flags & expect_flags_set);
}

TEST_F(DevctlTestQnx, TestDevctlv_GetFlags_Success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Devctl:v Get Flags Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const OpenFlag open_flags{OpenFlag::kReadOnly | OpenFlag::kNonBlocking | OpenFlag::kCreate};
    const ModeFlag mode_flags{ModeFlag::kReadUser | ModeFlag::kWriteUser};
    std::int32_t expect_flags_set{O_NONBLOCK};
    std::int32_t result_flags{};

    auto file_desc = score::os::Fcntl::instance().open(kTestFilePath.c_str(), open_flags, mode_flags);
    ASSERT_TRUE(file_desc.has_value());

    iov_t sv[1];
    iov_t rv[1];

    SETIOV(&sv[0], nullptr, 0);
    SETIOV(&rv[0], &result_flags, sizeof(result_flags));

    int dev_info = 0;
    auto res_devctlv =
        score::os::Devctl::instance().devctlv(file_desc.value(), DCMD_ALL_GETFLAGS, 1, 1, sv, rv, &dev_info);

    ASSERT_TRUE(res_devctlv.has_value());

    result_flags &= O_SETFLAG;
    ASSERT_TRUE(result_flags & expect_flags_set);
}

TEST_F(DevctlTestQnx, TestDevctl_SetGetFlags_Success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Devctl: Set Get Flags Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const OpenFlag open_flags{OpenFlag::kReadOnly | OpenFlag::kNonBlocking | OpenFlag::kCreate};
    const ModeFlag mode_flags{ModeFlag::kReadUser | ModeFlag::kWriteUser};
    std::int32_t set_flags{O_APPEND | O_LARGEFILE};
    std::int32_t result_flags{};

    auto file_desc = score::os::Fcntl::instance().open(kTestFilePath.c_str(), open_flags, mode_flags);
    ASSERT_TRUE(file_desc.has_value());

    auto res_devctl = score::os::Devctl::instance().devctl(
        file_desc.value(), DCMD_ALL_SETFLAGS, &set_flags, sizeof(set_flags), nullptr);
    ASSERT_TRUE(res_devctl.has_value());

    res_devctl = score::os::Devctl::instance().devctl(
        file_desc.value(), DCMD_ALL_GETFLAGS, &result_flags, sizeof(std::int32_t), nullptr);
    ASSERT_TRUE(res_devctl.has_value());

    result_flags &= O_SETFLAG;
    ASSERT_EQ(result_flags, set_flags);
}

TEST_F(DevctlTestQnx, TestDevctl_SetFlagsWrongFd_Fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Devctl: Set Flags Wrong Fd Fails");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::int32_t set_flags{O_APPEND | O_LARGEFILE};
    auto res_devctl = score::os::Devctl::instance().devctl(-1, DCMD_ALL_SETFLAGS, &set_flags, sizeof(set_flags), nullptr);
    ASSERT_FALSE(res_devctl.has_value());
}

TEST_F(DevctlTestQnx, TestDevctlv_SetFlagsWrongFd_Fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Devctl:v Set Flags Wrong Fd Fails");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto res_devctl = score::os::Devctl::instance().devctlv(-1, DCMD_ALL_SETFLAGS, 0, 0, nullptr, nullptr, nullptr);
    ASSERT_FALSE(res_devctl.has_value());
}

TEST_F(DevctlTestQnx, TestDevctl_GetFlagsNullptr_Fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Devctl: Get Flags Nullptr Fails");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const OpenFlag open_flags{OpenFlag::kReadOnly | OpenFlag::kNonBlocking | OpenFlag::kCreate};
    const ModeFlag mode_flags{ModeFlag::kReadUser | ModeFlag::kWriteUser};

    auto file_desc = score::os::Fcntl::instance().open(kTestFilePath.c_str(), open_flags, mode_flags);
    ASSERT_TRUE(file_desc.has_value());

    auto res_devctl = score::os::Devctl::instance().devctl(
        file_desc.value(), DCMD_ALL_GETFLAGS, nullptr, sizeof(std::int32_t), nullptr);
    ASSERT_FALSE(res_devctl.has_value());
}
