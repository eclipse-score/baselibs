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
#ifndef SCORE_LIB_CONCURRENCY_FUTURE_TEST_TYPES_H
#define SCORE_LIB_CONCURRENCY_FUTURE_TEST_TYPES_H

namespace score
{
namespace concurrency
{
namespace testing
{

class CopyAndMovableType
{
  public:
    explicit CopyAndMovableType(int value);

    int GetValue() const noexcept;

  private:
    int value_;
};

class CopyOnlyType : public CopyAndMovableType
{
  public:
    using CopyAndMovableType::CopyAndMovableType;
    CopyOnlyType(const CopyOnlyType&) noexcept = default;
    CopyOnlyType& operator=(const CopyOnlyType&) noexcept = default;
    CopyOnlyType(CopyOnlyType&&) noexcept = delete;
    CopyOnlyType& operator=(CopyOnlyType&&) noexcept = delete;

    ~CopyOnlyType() noexcept = default;
};

class MoveOnlyType : public CopyAndMovableType
{
  public:
    using CopyAndMovableType::CopyAndMovableType;
    MoveOnlyType(const MoveOnlyType&) noexcept = delete;
    MoveOnlyType& operator=(const MoveOnlyType&) noexcept = delete;
    MoveOnlyType(MoveOnlyType&&) noexcept = default;
    MoveOnlyType& operator=(MoveOnlyType&&) noexcept = default;

    ~MoveOnlyType() noexcept = default;
};

/// @brief Constructible by copy and by move, but assignable by neither.
///
/// This is the shape that forces InterruptibleState to construct its result in place rather than
/// assign it: score::Result<NonAssignableType> inherits the missing assignment operators, so the
/// natural `value_ = ...` does not compile for it.
class NonAssignableType : public CopyAndMovableType
{
  public:
    using CopyAndMovableType::CopyAndMovableType;
    NonAssignableType(const NonAssignableType&) noexcept = default;
    NonAssignableType& operator=(const NonAssignableType&) noexcept = delete;
    NonAssignableType(NonAssignableType&&) noexcept = default;
    NonAssignableType& operator=(NonAssignableType&&) noexcept = delete;

    ~NonAssignableType() noexcept = default;
};

/// @brief Move constructible only, and its move constructor may throw.
///
/// score::Result<T>::emplace() is constrained on std::is_nothrow_constructible, whereas constructing
/// the Result accepts any constructible T. This type therefore sits exactly on the boundary: the
/// setters accept it today, and an implementation that emplaced instead of constructing would stop
/// accepting it.
class ThrowingMoveOnlyType : public CopyAndMovableType
{
  public:
    using CopyAndMovableType::CopyAndMovableType;
    ThrowingMoveOnlyType(const ThrowingMoveOnlyType&) = delete;
    ThrowingMoveOnlyType& operator=(const ThrowingMoveOnlyType&) = delete;
    // Deliberately not noexcept.
    ThrowingMoveOnlyType(ThrowingMoveOnlyType&& other) : CopyAndMovableType(other) {}
    ThrowingMoveOnlyType& operator=(ThrowingMoveOnlyType&&) = delete;

    ~ThrowingMoveOnlyType() = default;
};

}  // namespace testing
}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_FUTURE_TEST_TYPES_H
