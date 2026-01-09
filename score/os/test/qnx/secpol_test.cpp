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
#include "score/os/qnx/secpol.h"
#include "score/os/qnx/secpol_impl.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <secpol/secpol.h>

namespace
{

/// This \c instance() call is necessary for providing coverage of the instance method, we can remove it when we remove
/// the instance function from the class
TEST(SecpolTest, InstanceCall)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Instance Call");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // check whether instance() returns injection
    score::os::qnx::details::SecpolImpl mock{};
    score::os::qnx::Secpol::set_testing_instance(mock);
    const auto& mocked_unit = score::os::qnx::Secpol::instance();
    EXPECT_EQ(&mocked_unit, &mock);

    // restore instance and should return non-mock object - implementation object
    score::os::qnx::Secpol::restore_instance();
    const auto& impl_unit = score::os::qnx::Secpol::instance();
    EXPECT_NE(&impl_unit, &mock);
}

class SecpolFixture : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        unit_ = std::make_unique<score::os::qnx::details::SecpolImpl>();
        ASSERT_NE(unit_, nullptr);
    }

    std::unique_ptr<score::os::qnx::Secpol> unit_{nullptr};
};

TEST_F(SecpolFixture, secpol_openOpenNullPath)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Secpol Open Open Null Path");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const char* const path{nullptr};

    auto handle = unit_->secpol_open(path, secpol_open_flags_e::SECPOL_USE_AS_DEFAULT);
    ASSERT_TRUE(handle) << "got error: " << handle.error().ToString();

    EXPECT_TRUE(unit_->secpol_close(handle.value()));
}

TEST_F(SecpolFixture, secpol_openOpenDoubleCallFails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Secpol Open Open Double Call Fails");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const char* const path{nullptr};

    auto handle = unit_->secpol_open(path, secpol_open_flags_e::SECPOL_USE_AS_DEFAULT);
    ASSERT_TRUE(handle) << "got error: " << handle.error().ToString();

    auto handle2 = unit_->secpol_open(path, secpol_open_flags_e::SECPOL_USE_AS_DEFAULT);
    EXPECT_FALSE(handle2);

    EXPECT_TRUE(unit_->secpol_close(handle.value()));
}

TEST_F(SecpolFixture, secpol_posix_spawnattr_settypeid)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Secpol Posix Spawnattr Settypeid");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    posix_spawnattr_t attr{};
    secpol_file_t* handle_null{nullptr};
    // positive case
    auto result = unit_->secpol_posix_spawnattr_settypeid(handle_null, &attr, "low_priv", SECPOL_TYPE_NAME);
    EXPECT_TRUE(result) << "got error: " << result.error().ToString();
    // negative case
    EXPECT_FALSE(unit_->secpol_posix_spawnattr_settypeid(handle_null, nullptr, nullptr, SECPOL_TYPE_NAME));
}

TEST_F(SecpolFixture, secpol_transition_type)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Secpol Transition Type");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    secpol_file_t* handle{nullptr};
    const char* const name{nullptr};
    // positive case
    auto result = unit_->secpol_transition_type(handle, name, SECPOL_TYPE_NAME);
    EXPECT_TRUE(result);
    // negative case
    result = unit_->secpol_transition_type(handle, name, 0U);
    EXPECT_FALSE(result);  // returns error if no type change was performed.
}

}  // namespace
