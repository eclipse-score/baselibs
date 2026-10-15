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

#include <type_traits>
#include <utility>

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

/// @brief Constructible from one argument or from two, but neither copyable nor movable.
///
/// Such a value can only enter a score::Result by being constructed there, so SetValue() accepts it only when it is
/// handed the constructor arguments rather than an object. The two-argument constructor stores their sum, so a test
/// can tell whether both arguments arrived.
class NeitherCopyNorMoveType : public CopyAndMovableType
{
  public:
    using CopyAndMovableType::CopyAndMovableType;
    NeitherCopyNorMoveType(int first, int second) : CopyAndMovableType{first + second} {}
    NeitherCopyNorMoveType(const NeitherCopyNorMoveType&) = delete;
    NeitherCopyNorMoveType& operator=(const NeitherCopyNorMoveType&) = delete;
    NeitherCopyNorMoveType(NeitherCopyNorMoveType&&) = delete;
    NeitherCopyNorMoveType& operator=(NeitherCopyNorMoveType&&) = delete;

    ~NeitherCopyNorMoveType() = default;
};

/// @brief Default constructible, holding kDefaultValue when constructed without arguments.
///
/// kDefaultValue is deliberately not zero, so that a value which was never constructed cannot pass for one that was.
class DefaultConstructibleType : public CopyAndMovableType
{
  public:
    DefaultConstructibleType() : CopyAndMovableType{kDefaultValue} {}

    /// @brief The value held after default construction.
    static constexpr int kDefaultValue{17};
};

/// @brief Counts every move construction into a counter supplied at construction.
///
/// Used to show that handing SetValue() constructor arguments moves nothing, which is only meaningful next to the
/// contrasting case where handing it an object moves exactly once.
class MoveCountingType : public CopyAndMovableType
{
  public:
    MoveCountingType(int value, int& move_count) : CopyAndMovableType{value}, move_count_{&move_count} {}
    MoveCountingType(const MoveCountingType&) noexcept = default;
    MoveCountingType& operator=(const MoveCountingType&) noexcept = delete;
    MoveCountingType(MoveCountingType&& other) noexcept : CopyAndMovableType{other}, move_count_{other.move_count_}
    {
        ++(*move_count_);
    }
    MoveCountingType& operator=(MoveCountingType&&) noexcept = delete;

    ~MoveCountingType() noexcept = default;

  private:
    /// @brief Incremented on each move construction; owned by the test, which outlives this object.
    int* move_count_;
};

/// @brief Has a perfect-forwarding converting constructor that is guarded against its own type.
///
/// SetValue() preserves the value category of its arguments, so a non-const lvalue of this type is offered to overload
/// resolution as `GuardedForwardingType&`. Because the forwarding constructor excludes its own type, the copy
/// constructor is still chosen. Without that guard the forwarding constructor would be the better match and would win.
class GuardedForwardingType : public CopyAndMovableType
{
  public:
    template <typename T, std::enable_if_t<!std::is_same<std::decay_t<T>, GuardedForwardingType>::value, bool> = true>
    explicit GuardedForwardingType(T&& value) : CopyAndMovableType{std::forward<T>(value)}, was_copied_{false}
    {
    }
    GuardedForwardingType(const GuardedForwardingType& other) : CopyAndMovableType{other}, was_copied_{true} {}
    GuardedForwardingType& operator=(const GuardedForwardingType&) = delete;
    GuardedForwardingType(GuardedForwardingType&&) = delete;
    GuardedForwardingType& operator=(GuardedForwardingType&&) = delete;

    ~GuardedForwardingType() = default;

    bool WasCopyConstructed() const noexcept
    {
        return was_copied_;
    }

  private:
    /// @brief true if this object was produced by the copy constructor, false if by the forwarding one.
    bool was_copied_;
};

}  // namespace testing
}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_FUTURE_TEST_TYPES_H
