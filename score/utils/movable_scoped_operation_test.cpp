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
#include "score/utils/movable_scoped_operation.h"

#include <score/utility.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>

namespace score::utils
{
namespace
{

using namespace ::testing;

class MovableScopedOperationFixture : public ::testing::Test
{
  public:
    MovableScopedOperationFixture& GivenAMovableScopedOperation()
    {
        score::cpp::ignore = movable_scoped_operation_.emplace([this]() {
            destruction_handler_called_ = true;
        });
        return *this;
    }

    MovableScopedOperationFixture& GivenTwoMovableScopedOperations()
    {
        score::cpp::ignore = movable_scoped_operation_.emplace([this]() {
            destruction_handler_called_ = true;
        });
        score::cpp::ignore = movable_scoped_operation_2_.emplace([this]() {
            destruction_handler_called_2_ = true;
        });
        return *this;
    }

    std::optional<MovableScopedOperation<score::cpp::callback<void()>>> movable_scoped_operation_{};
    bool destruction_handler_called_{false};

    std::optional<MovableScopedOperation<score::cpp::callback<void()>>> movable_scoped_operation_2_{};
    bool destruction_handler_called_2_{false};
};

TEST_F(MovableScopedOperationFixture, CreatingGuardDoesNotCallDestructionHandler)
{
    // When creating a MovableScopedOperation
    GivenAMovableScopedOperation();

    // Then the destruction handler is not called
    EXPECT_FALSE(destruction_handler_called_);
}

TEST_F(MovableScopedOperationFixture, DestroyingGuardCallsDestructionHandler)
{
    GivenAMovableScopedOperation();

    // When destroying the MovableScopedOperation (by resetting the optional containing it)
    movable_scoped_operation_.reset();

    // Then the destruction handler is called
    EXPECT_TRUE(destruction_handler_called_);
}

TEST_F(MovableScopedOperationFixture, MoveConstructingGuardDoesNotCallDestructionHandler)
{
    GivenAMovableScopedOperation();

    // When move constructing a new MovableScopedOperation
    MovableScopedOperation moved_to_guard{std::move(movable_scoped_operation_).value()};

    // Then the destruction handler is not called
    EXPECT_FALSE(destruction_handler_called_);
}

TEST_F(MovableScopedOperationFixture, DestroyingMoveConstructedMovedFromGuardDoesNotCallDestructionHandler)
{
    GivenAMovableScopedOperation();

    // and given a new MovableScopedOperation move constructed from another
    MovableScopedOperation moved_to_guard{std::move(movable_scoped_operation_).value()};

    // When destroying the moved_from guard
    movable_scoped_operation_.reset();

    // Then the destruction handler is not called
    EXPECT_FALSE(destruction_handler_called_);
}

TEST_F(MovableScopedOperationFixture, DestroyingMoveConstructedMovedToGuardCallsDestructionHandler)
{
    GivenAMovableScopedOperation();

    // and given a new MovableScopedOperation move constructed from another
    std::optional<MovableScopedOperation<>> moved_to_guard{std::move(movable_scoped_operation_).value()};

    // When destroying the moved_to guard
    moved_to_guard.reset();

    // Then the destruction handler is called
    EXPECT_TRUE(destruction_handler_called_);
}

TEST_F(MovableScopedOperationFixture, MoveAssigningGuardCallsDestructionHandlerOnMovedToGuard)
{
    GivenTwoMovableScopedOperations();

    // When move assigning one MovableScopedOperation to another
    movable_scoped_operation_.value() = std::move(movable_scoped_operation_2_).value();

    // Then the destruction handler is only called on the moved-to guard
    EXPECT_TRUE(destruction_handler_called_);
    EXPECT_FALSE(destruction_handler_called_2_);
}

TEST_F(MovableScopedOperationFixture, DestroyingMoveAssignedMovedFromGuardDoesNotCallDestructionHandler)
{
    GivenTwoMovableScopedOperations();

    // and given that one MovableScopedOperation to was move assigned to another
    movable_scoped_operation_.value() = std::move(movable_scoped_operation_2_).value();

    // When destroying the moved-from guard
    movable_scoped_operation_2_.reset();

    // Then the destruction handler is not called on the moved-from guard
    EXPECT_FALSE(destruction_handler_called_2_);
}

TEST_F(MovableScopedOperationFixture, DestroyingMoveAssignedMovedToGuardCallsDestructionHandler)
{
    GivenTwoMovableScopedOperations();

    // and given that one MovableScopedOperation to was move assigned to another
    movable_scoped_operation_.value() = std::move(movable_scoped_operation_2_).value();

    // When destroying the moved-to guard
    movable_scoped_operation_.reset();

    // Then the destruction handler is not called on the moved-to guard
    EXPECT_TRUE(destruction_handler_called_2_);
}

}  // namespace
}  // namespace score::utils
