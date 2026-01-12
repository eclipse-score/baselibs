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
#include "score/os/qnx/sysctl.h"
#include "score/os/mocklib/qnx/mock_sysctl.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <cstdint>

namespace score::os::qnx
{
namespace
{
using ::testing::_;
using ::testing::Eq;
using ::testing::Pointee;
using ::testing::Return;

// Unit tests for mocked class
struct SysctlTestMock : ::testing::Test
{
    void SetUp() override
    {
        score::os::Sysctl::set_testing_instance(mock_sysctl);
    };
    void TearDown() override
    {
        score::os::Sysctl::restore_instance();
    };

    score::os::MockSysctl mock_sysctl;
};

TEST_F(SysctlTestMock, TestFunction_sysctl)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Function sysctl");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const std::size_t RETURN_LENGTH = 100;
    std::int32_t sys_name[6] = {1, 2, 3, 4, 5, 6};
    auto& sys_name_ptr = sys_name;
    std::size_t sys_len{};
    std::int32_t* arg_ptr{};

    auto& expect_sysctl = EXPECT_CALL(
        mock_sysctl, sysctl(Eq(sys_name_ptr), Eq(sizeof(sys_name) / sizeof(std::int32_t)), _, &sys_len, _, _));
    expect_sysctl.WillOnce([&arg_ptr, RETURN_LENGTH](std::int32_t* const sys_name_ptr,
                                                     const std::size_t,
                                                     void* const oldp,
                                                     std::size_t* const oldlenp,
                                                     void* const,
                                                     const std::int32_t) {
        arg_ptr = sys_name_ptr;
        *oldlenp = RETURN_LENGTH;
        return score::cpp::expected_blank<score::os::Error>{};
    });

    expect_sysctl.WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EOVERFLOW))));

    auto res = score::os::Sysctl::instance().sysctl(
        sys_name, sizeof(sys_name) / sizeof(std::int32_t), nullptr, &sys_len, nullptr, 0);
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(arg_ptr, static_cast<const std::int32_t*>(sys_name));
    ASSERT_EQ(sys_len, RETURN_LENGTH);

    res = score::os::Sysctl::instance().sysctl(
        sys_name, sizeof(sys_name) / sizeof(std::int32_t), nullptr, &sys_len, nullptr, 0);
    ASSERT_FALSE(res.has_value());
}

TEST_F(SysctlTestMock, TestFunction_sysctlbyname)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Function sysctlbyname");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const std::size_t RETURN_LENGTH = 100;
    const char* sys_name = "some.dummy.parameter";
    std::size_t sys_len{RETURN_LENGTH};

    auto& expect_sysctl = EXPECT_CALL(mock_sysctl, sysctlbyname(Eq(sys_name), _, Pointee(sys_len), _, _));
    expect_sysctl.WillOnce(
        [RETURN_LENGTH](
            const char* sname, void* const oldp, std::size_t* const oldlenp, void* const, const std::size_t) {
            *oldlenp = RETURN_LENGTH;
            return score::cpp::expected_blank<score::os::Error>{};
        });

    expect_sysctl.WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EOVERFLOW))));

    auto res = score::os::Sysctl::instance().sysctlbyname(sys_name, nullptr, &sys_len, nullptr, 0);
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(sys_len, RETURN_LENGTH);

    res = score::os::Sysctl::instance().sysctlbyname(sys_name, nullptr, &sys_len, nullptr, 0);
    ASSERT_FALSE(res.has_value());
}

struct SysctlImplTest : ::testing::Test
{
    score::os::Sysctl& instance_{score::os::Sysctl::instance()};
};

constexpr std::string_view kSysName{"kern.clockrate"};

TEST_F(SysctlImplTest, TestFunction_sysctl_Success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Function sysctl Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::array<std::int32_t, 2> sys_name = {
        CTL_KERN,
        KERN_CLOCKRATE,
    };
    int64_t out;
    std::size_t sys_len{sizeof(out)};

    auto res = instance_.sysctl(sys_name.data(), sys_name.size(), &out, &sys_len, nullptr, 0);
    ASSERT_TRUE(res.has_value()) << "sysctrl failed, error: " << res.error().ToString();
    ASSERT_GT(sys_len, 0);
}

TEST_F(SysctlImplTest, TestFunction_sysctl_Failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Function sysctl Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::array<std::int32_t, 6> sys_name = {1, 2, 3, 4, 5, 6};
    std::array<std::int8_t, 1> buf_small{};

    auto res = instance_.sysctl(sys_name.data(), sys_name.size(), nullptr, nullptr, buf_small.data(), 0);
    ASSERT_FALSE(res.has_value());
}

TEST_F(SysctlImplTest, TestFunction_sysctlbyname_Success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Function sysctlbyname Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::size_t sys_len{};

    auto res = instance_.sysctlbyname(kSysName.data(), nullptr, &sys_len, nullptr, 0);
    ASSERT_TRUE(res.has_value());
    ASSERT_GT(sys_len, 0);
    std::vector<std::int8_t> buf(sys_len);
    res = instance_.sysctlbyname(kSysName.data(), buf.data(), &sys_len, nullptr, 0);
    ASSERT_TRUE(res.has_value());
    ASSERT_GT(sys_len, 0);
}

TEST_F(SysctlImplTest, TestFunction_sysctlbyname_Failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Function sysctlbyname Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto res = instance_.sysctlbyname("", nullptr, nullptr, nullptr, 0);
    ASSERT_FALSE(res.has_value());
}

TEST_F(SysctlTestMock, TestFunction_sysctlnametomib)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "Test Function sysctlnametomib");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const char* sys_name = "some.dummy.parameter";
    int mib[4] = {0};
    size_t mib_len = 4;

    auto& expect_sysctl = EXPECT_CALL(mock_sysctl, sysctlnametomib(Eq(sys_name), _, Pointee(mib_len)));
    expect_sysctl.WillOnce([](const char*, int* mibp, size_t* sizep) {
        mibp[0] = 1;
        *sizep = 4;
        return score::cpp::expected_blank<score::os::Error>{};
    });

    expect_sysctl.WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EINVAL))));

    auto res = score::os::Sysctl::instance().sysctlnametomib(sys_name, mib, &mib_len);
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(mib_len, 4);
    ASSERT_EQ(mib[0], 1);

    res = score::os::Sysctl::instance().sysctlnametomib(sys_name, mib, &mib_len);
    ASSERT_FALSE(res.has_value());
}

TEST_F(SysctlImplTest, TestFunction_sysctlnametomib_Success)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "Test Function sysctlnametomib Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    int mib[4] = {0};
    size_t mib_len = 4;

    auto res = instance_.sysctlnametomib(kSysName.data(), mib, &mib_len);
    ASSERT_TRUE(res.has_value());
    ASSERT_GT(mib_len, 0);
}

TEST_F(SysctlImplTest, TestFunction_sysctlnametomib_Failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "QM");
    RecordProperty("Description", "Test Function sysctlnametomib Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    int mib[4] = {0};
    size_t mib_len = 4;

    auto res = instance_.sysctlnametomib("", mib, &mib_len);
    ASSERT_FALSE(res.has_value());
}

}  // namespace
}  // namespace score::os::qnx
