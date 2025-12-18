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
#include "score/os/semaphore_impl.h"
#include "score/os/utils/test/lib/random_string.h"

#include <fcntl.h>
#include <gtest/gtest.h>
#include <limits.h>

namespace score
{
namespace os
{
namespace test
{

class SemaphoreTestFixture : public ::testing::Test
{
  public:
    void TearDown() override
    {
        unit_.sem_unlink(m_name_.c_str());
    }

    const std::uint16_t random_len_str_{31U};
    std::string m_name_{random_string(random_len_str_)};

    score::os::SemaphoreImpl unit_{};
    const std::uint32_t value_{1U};
};

// Positive test for sem_open()
TEST_F(SemaphoreTestFixture, SuccessSemOpen)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SemaphoreTestFixture Success Sem Open");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto ret_open_create = unit_.sem_open(m_name_.c_str(),
                                                Semaphore::OpenFlag::kCreate,
                                                Semaphore::ModeFlag::kWriteUser | Semaphore::ModeFlag::kReadUser,
                                                value_);
    ASSERT_TRUE(ret_open_create.has_value());
    ASSERT_TRUE(ret_open_create.value());
    unit_.sem_close(ret_open_create.value());

    const auto ret_open = unit_.sem_open(m_name_.c_str(), Semaphore::OpenFlag{});
    ASSERT_TRUE(ret_open.has_value());
    ASSERT_TRUE(ret_open.value());
    unit_.sem_close(ret_open.value());
}

// Negative test for sem_open()
TEST_F(SemaphoreTestFixture, FailureSemOpen)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SemaphoreTestFixture Failure Sem Open");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const char* invalid_path = "/invalid_path";
    const auto ret =
        unit_.sem_open(invalid_path, Semaphore::OpenFlag::kExclusive, Semaphore::ModeFlag::kReadUser, value_);
    EXPECT_FALSE(ret.has_value());
}

TEST_F(SemaphoreTestFixture, FailureSemOpenWithoutCreateFlag)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SemaphoreTestFixture Failure Sem Open Without Create Flag");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const char* invalid_path = "/invalid_path";
    const auto ret = unit_.sem_open(invalid_path, Semaphore::OpenFlag::kExclusive);
    EXPECT_FALSE(ret.has_value());
}

// Positive test for sem_getvalue()
TEST_F(SemaphoreTestFixture, SuccessGetValue)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SemaphoreTestFixture Success Get Value");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::int32_t sval;
    const std::int32_t pshared{0};

    const auto ret =
        unit_.sem_open(m_name_.c_str(), Semaphore::OpenFlag::kCreate, Semaphore::ModeFlag::kReadUser, value_);
    ASSERT_TRUE(ret.has_value());
    ASSERT_TRUE(ret.value() != nullptr);
    ASSERT_TRUE(unit_.sem_init(ret.value(), pshared, value_).has_value());
    ASSERT_TRUE(unit_.sem_getvalue(ret.value(), &sval).has_value());
    unit_.sem_close(ret.value());
}

// Negative test for sem_post()
TEST_F(SemaphoreTestFixture, FailureSemPost)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SemaphoreTestFixture Failure Sem Post");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto valid_sem =
        unit_.sem_open(m_name_.c_str(), Semaphore::OpenFlag::kCreate, Semaphore::ModeFlag::kReadUser, value_);
    EXPECT_TRUE(unit_.sem_init(valid_sem.value(), 0, SEM_VALUE_MAX));

    const auto post_result = unit_.sem_post(valid_sem.value());
    EXPECT_FALSE(post_result.has_value());
    std::int32_t sval{};

    ASSERT_TRUE(unit_.sem_getvalue(valid_sem.value(), &sval).has_value());
    EXPECT_EQ(sval, SEM_VALUE_MAX);
}

// Positive test for sem_timedwait()
TEST_F(SemaphoreTestFixture, SuccessTimedWait)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SemaphoreTestFixture Success Timed Wait");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    timespec abs_time{};
    abs_time.tv_sec = 5;
    const auto ret =
        unit_.sem_open(m_name_.c_str(), Semaphore::OpenFlag::kCreate, Semaphore::ModeFlag::kReadUser, value_);
    ASSERT_TRUE(ret.has_value());
    ASSERT_TRUE(ret.value() != nullptr);
    ASSERT_TRUE(unit_.sem_timedwait(ret.value(), &abs_time).has_value());
    unit_.sem_close(ret.value());
}

// Testing all modes in for Loop
TEST_F(SemaphoreTestFixture, SuccessSemOpenAllModes)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SemaphoreTestFixture Success Sem Open All Modes");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    std::vector<score::os::Semaphore::ModeFlag> mode_vector = {Semaphore::ModeFlag::kReadUser,
                                                             Semaphore::ModeFlag::kWriteUser,
                                                             Semaphore::ModeFlag::kExecUser,
                                                             Semaphore::ModeFlag::kReadGroup,
                                                             Semaphore::ModeFlag::kWriteGroup,
                                                             Semaphore::ModeFlag::kExecGroup,
                                                             Semaphore::ModeFlag::kReadOthers,
                                                             Semaphore::ModeFlag::kWriteOthers,
                                                             Semaphore::ModeFlag::kExecOthers};

    for (const auto& mode : mode_vector)
    {
        const auto ret = unit_.sem_open(m_name_.c_str(), Semaphore::OpenFlag::kCreate, mode, value_);
        EXPECT_TRUE(ret.has_value());
        EXPECT_TRUE(ret.value() != nullptr);
        unit_.sem_unlink(m_name_.c_str());
        unit_.sem_close(ret.value());
    }
}

// Positive test for sem_wait()
TEST_F(SemaphoreTestFixture, SuccessSemWait)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SemaphoreTestFixture Success Sem Wait");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto sem = ::sem_open(m_name_.c_str(), O_CREAT, S_IRUSR, value_);
    ASSERT_NE(sem, SEM_FAILED);

    int sem_value{};
    ASSERT_EQ(::sem_getvalue(sem, &sem_value), 0);
    ASSERT_EQ(sem_value, value_);

    const auto wait_result = unit_.sem_wait(sem);
    ASSERT_TRUE(wait_result.has_value());

    ASSERT_EQ(::sem_getvalue(sem, &sem_value), 0);
    ASSERT_EQ(sem_value, value_ - 1);

    unit_.sem_close(sem);
}

// Positive test for sem_post()
TEST_F(SemaphoreTestFixture, SuccessSemPost)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SemaphoreTestFixture Success Sem Post");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto sem = ::sem_open(m_name_.c_str(), O_CREAT, S_IRUSR, value_);
    ASSERT_NE(sem, SEM_FAILED);

    int sem_value{};
    ASSERT_EQ(::sem_getvalue(sem, &sem_value), 0);
    ASSERT_EQ(sem_value, value_);

    const auto post_result = unit_.sem_post(sem);
    ASSERT_TRUE(post_result.has_value());

    ASSERT_EQ(::sem_getvalue(sem, &sem_value), 0);
    ASSERT_EQ(sem_value, value_ + 1);

    unit_.sem_close(sem);
}

// Positive Negative for sem_timedwait()
TEST_F(SemaphoreTestFixture, SuccessTimedWaitFailure)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SemaphoreTestFixture Success Timed Wait Failure");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    timespec abs_time{0, -1};
    const auto sem = ::sem_open(m_name_.c_str(), O_CREAT, S_IRUSR, value_);
    ASSERT_NE(sem, SEM_FAILED);
    ASSERT_TRUE(sem != nullptr);
    ASSERT_FALSE(unit_.sem_timedwait(sem, &abs_time).has_value());
    unit_.sem_close(sem);
}

TEST(Semaphore, get_instance)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Semaphore get_instance");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_NO_FATAL_FAILURE(Semaphore::instance());
}

}  // namespace test
}  // namespace os
}  // namespace score
