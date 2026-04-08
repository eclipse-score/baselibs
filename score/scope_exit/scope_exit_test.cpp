/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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
#include "score/scope_exit/scope_exit.h"

#include "score/language/safecpp/scoped_function/move_only_scoped_function.h"
#include "score/language/safecpp/scoped_function/scope.h"

#include <score/callback.hpp>
#include <score/utility.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>

namespace score::utils
{
namespace
{

using namespace ::testing;

class ScopeExitFixture : public ::testing::Test
{
  public:
    ScopeExitFixture& GivenAScopeExit()
    {
        score::cpp::ignore = scope_exit_1_.emplace([this]() {
            destruction_handler_called_1_ = true;
        });
        return *this;
    }

    ScopeExitFixture& GivenTwoScopeExits()
    {
        score::cpp::ignore = scope_exit_1_.emplace([this]() {
            destruction_handler_called_1_ = true;
        });
        score::cpp::ignore = scope_exit_2_.emplace([this]() {
            destruction_handler_called_2_ = true;
        });
        return *this;
    }

    std::optional<ScopeExit<score::cpp::callback<void()>>> scope_exit_1_{};
    bool destruction_handler_called_1_{false};

    std::optional<ScopeExit<score::cpp::callback<void()>>> scope_exit_2_{};
    bool destruction_handler_called_2_{false};
};

// Note. The first two tests are written to demonstrate how a ScopeExit can actually be used. For that reason, we don't
// use GivenAScopeExit.
TEST_F(ScopeExitFixture, CreatingDoesNotCallDestructionHandler)
{
    // When creating a ScopeExit
    bool destruction_handler_called{false};
    ScopeExit<> scope_exit{[&destruction_handler_called]() noexcept {
        destruction_handler_called = true;
    }};

    // Then the destruction handler is not called after construction and before being destroyed
    EXPECT_FALSE(destruction_handler_called);
}

TEST_F(ScopeExitFixture, DestroyingCallsDestructionHandler)
{
    bool destruction_handler_called{false};
    {
        // Given a ScopeExit
        ScopeExit<> scope_exit{[&destruction_handler_called]() noexcept {
            destruction_handler_called = true;
        }};

        // When destroying the ScopeExit
    }

    // Then the destruction handler is called
    EXPECT_TRUE(destruction_handler_called);
}

TEST_F(ScopeExitFixture, DestroyingWithScopedFunctionCallsDestructionHandler)
{
    safecpp::Scope<> scope{};
    bool destruction_handler_called{false};
    {
        // Given a ScopeExit which is created with a scoped function
        ScopeExit<safecpp::MoveOnlyScopedFunction<void()>> scope_exit{{scope, [&destruction_handler_called]() noexcept {
                                                                           destruction_handler_called = true;
                                                                       }}};

        // When destroying the ScopeExit
    }

    // Then the destruction handler is called
    EXPECT_TRUE(destruction_handler_called);
}

TEST_F(ScopeExitFixture, DestroyingAfterCallingReleaseDoesNotCallDestructionHandler)
{
    GivenAScopeExit();

    // and given that Release has been called on the ScopeExit
    scope_exit_1_->Release();

    // When destroying the ScopeExit
    scope_exit_1_.reset();

    // Then the destruction handler is not called
    EXPECT_FALSE(destruction_handler_called_1_);
}

TEST_F(ScopeExitFixture, MoveConstructingGuardDoesNotCallDestructionHandler)
{
    GivenAScopeExit();

    // When move constructing a new ScopeExit
    ScopeExit moved_to_guard{std::move(scope_exit_1_).value()};

    // Then the destruction handler of the moved-from object is not called during move construction
    EXPECT_FALSE(destruction_handler_called_1_);
}

TEST_F(ScopeExitFixture, DestroyingMoveConstructedMovedFromGuardDoesNotCallDestructionHandler)
{
    GivenAScopeExit();

    // and given a new ScopeExit move constructed from another
    ScopeExit moved_to_guard{std::move(scope_exit_1_).value()};

    // When destroying the moved_from guard
    scope_exit_1_.reset();

    // Then the destruction handler is not called
    EXPECT_FALSE(destruction_handler_called_1_);
}

TEST_F(ScopeExitFixture, DestroyingMoveConstructedMovedToGuardCallsDestructionHandler)
{
    GivenAScopeExit();

    // and given a new ScopeExit move constructed from another
    std::optional<ScopeExit<>> moved_to_guard{std::move(scope_exit_1_).value()};

    // When destroying the moved_to guard
    moved_to_guard.reset();

    // Then the destruction handler is called
    EXPECT_TRUE(destruction_handler_called_1_);
}

TEST_F(ScopeExitFixture, MoveAssigningGuardCallsDestructionHandlerOnMovedToGuard)
{
    GivenTwoScopeExits();

    // When move assigning one ScopeExit to another
    scope_exit_1_.value() = std::move(scope_exit_2_).value();

    // Then the destruction handler is only called on the moved-to guard
    EXPECT_TRUE(destruction_handler_called_1_);
    EXPECT_FALSE(destruction_handler_called_2_);
}

TEST_F(ScopeExitFixture, DestroyingMoveAssignedMovedFromGuardDoesNotCallDestructionHandler)
{
    GivenTwoScopeExits();

    // and given that one ScopeExit was move assigned to another
    scope_exit_1_.value() = std::move(scope_exit_2_).value();

    // When destroying the moved-from guard
    scope_exit_2_.reset();

    // Then the destruction handler is not called on the moved-from guard
    EXPECT_FALSE(destruction_handler_called_2_);
}

TEST_F(ScopeExitFixture, DestroyingMoveAssignedMovedToGuardCallsDestructionHandler)
{
    GivenTwoScopeExits();

    // and given that one ScopeExit was move assigned to another
    scope_exit_1_.value() = std::move(scope_exit_2_).value();

    // When destroying the moved-to guard
    scope_exit_1_.reset();

    // Then the destruction handler that was transferred to the moved-to guard is called
    EXPECT_TRUE(destruction_handler_called_2_);
}

}  // namespace
}  // namespace score::utils
