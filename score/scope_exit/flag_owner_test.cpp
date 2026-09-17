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
#include "score/scope_exit/flag_owner.h"

#include <gtest/gtest.h>
#include <optional>
#include <utility>

namespace score::utils
{
namespace
{

TEST(FlagOwnerTest, CreatingFlagOwnerWithTrueInitialValueWillSetTheFlag)
{
    // When creating the FlagOwner with the initial value set to true
    FlagOwner flag_owner{true};

    // Then the value of the flag owner will be set
    EXPECT_TRUE(flag_owner.IsSet());
}

TEST(FlagOwnerTest, CreatingFlagOwnerWithFalseInitialValueWillClearTheFlag)
{
    // When creating the FlagOwner with the initial value set to false
    FlagOwner flag_owner{false};

    // Then the value of the flag owner will be cleared
    EXPECT_FALSE(flag_owner.IsSet());
}

TEST(FlagOwnerTest, MoveConstructingAFlagOwnerWillTransferTheFlagValue)
{
    // Given a FlagOwner with the flag set
    FlagOwner flag_owner_1{true};

    // When move constructing a new FlagOwner from the first one
    FlagOwner flag_owner_2{std::move(flag_owner_1)};

    // Then the value of the first flag owner will be transferred to the second and cleared in the first
    EXPECT_FALSE(flag_owner_1.IsSet());
    EXPECT_TRUE(flag_owner_2.IsSet());
}

TEST(FlagOwnerTest, MoveAssigningAFlagOwnerWillTransferTheFlagValue)
{
    // Given a FlagOwner with the flag set and another the the flag cleraed
    FlagOwner flag_owner_1{true};
    FlagOwner flag_owner_2{false};

    // When moving assigning the FlagOwner with true to the FlagOwner with false
    flag_owner_2 = std::move(flag_owner_1);

    // Then the value of the first flag owner will be transferred to the second
    EXPECT_FALSE(flag_owner_1.IsSet());
    EXPECT_TRUE(flag_owner_2.IsSet());
}

TEST(FlagOwnerTest, SelfMoveAssigningAFlagOwnerDoesNotChangeFlagValue)
{
    // Given a FlagOwner with the flag set
    const bool initial_flag_value{true};
    // Note. we use a std::optional to avoid a clang compiler warning -Wself-move
    std::optional<FlagOwner> flag_owner{initial_flag_value};

    // When self moving assigning the FlagOwner
    flag_owner = std::move(flag_owner.value());

    // Then the value of the flag owner will be unchanged
    EXPECT_EQ(flag_owner.value().IsSet(), initial_flag_value);
}

}  // namespace
}  // namespace score::utils
