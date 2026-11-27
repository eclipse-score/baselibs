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

#include <cstdint>
#include <type_traits>

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

/// @brief Neither copyable nor movable, and implicitly constructible from one int or from two.
///
/// The two-argument constructor stores the sum of its arguments.
class NeitherCopyNorMoveType
{
  public:
    // coverity[autosar_cpp14_a12_1_4_violation] implicit on purpose, see above
    NeitherCopyNorMoveType(int value) : value_{value} {}
    NeitherCopyNorMoveType(int first, int second) : value_{first + second} {}
    NeitherCopyNorMoveType(const NeitherCopyNorMoveType&) = delete;
    NeitherCopyNorMoveType& operator=(const NeitherCopyNorMoveType&) = delete;
    NeitherCopyNorMoveType(NeitherCopyNorMoveType&&) = delete;
    NeitherCopyNorMoveType& operator=(NeitherCopyNorMoveType&&) = delete;

    ~NeitherCopyNorMoveType() = default;

    /// @brief The value given at construction.
    [[nodiscard]] int GetValue() const noexcept
    {
        return value_;
    }

  private:
    /// @brief The value given at construction.
    int value_;
};

/// @brief Default constructible, holding kDefaultValue afterwards.
///
/// kDefaultValue is deliberately not zero, so that zeroed storage cannot pass for a constructed object.
class DefaultConstructibleType
{
  public:
    /// @brief The value held after default construction.
    [[nodiscard]] int GetValue() const noexcept
    {
        return value_;
    }

    /// @brief The value held after default construction.
    static constexpr int kDefaultValue{17};

  private:
    /// @brief Holds kDefaultValue from construction on.
    int value_{kDefaultValue};
};

/// @brief Move only, incrementing a counter supplied at construction each time it is move constructed.
class MoveCountingType
{
  public:
    MoveCountingType(int value, int& move_count) : value_{value}, move_count_{&move_count} {}
    MoveCountingType(const MoveCountingType&) = delete;
    MoveCountingType& operator=(const MoveCountingType&) = delete;
    MoveCountingType(MoveCountingType&& other) noexcept : value_{other.value_}, move_count_{other.move_count_}
    {
        ++(*move_count_);
    }
    MoveCountingType& operator=(MoveCountingType&&) = delete;

    ~MoveCountingType() = default;

    /// @brief The value given at construction.
    [[nodiscard]] int GetValue() const noexcept
    {
        return value_;
    }

  private:
    /// @brief The value given at construction.
    int value_;
    /// @brief Incremented on each move construction; owned by the caller, and has to outlive this object.
    int* move_count_;
};

/// @brief Has a forwarding constructor that excludes exactly this type, and records if it was copy constructed.
///
/// An object of a derived type is not excluded, so direct-initialisation from one takes the forwarding constructor.
class ForwardingCtorExcludingSelfType
{
  public:
    template <typename T,
              std::enable_if_t<!std::is_same_v<std::decay_t<T>, ForwardingCtorExcludingSelfType>, bool> = true>
    explicit ForwardingCtorExcludingSelfType(T&&)
    {
    }
    ForwardingCtorExcludingSelfType(const ForwardingCtorExcludingSelfType&) : was_copy_constructed_{true} {}
    ForwardingCtorExcludingSelfType& operator=(const ForwardingCtorExcludingSelfType&) = delete;
    ForwardingCtorExcludingSelfType(ForwardingCtorExcludingSelfType&&) = delete;
    ForwardingCtorExcludingSelfType& operator=(ForwardingCtorExcludingSelfType&&) = delete;

    ~ForwardingCtorExcludingSelfType() = default;

    /// @brief Whether the copy constructor produced this object.
    [[nodiscard]] bool WasCopyConstructed() const noexcept
    {
        return was_copy_constructed_;
    }

  private:
    /// @brief true if the copy constructor produced this object, false if the forwarding one did.
    bool was_copy_constructed_{false};
};

/// @brief Derived from ForwardingCtorExcludingSelfType, whose forwarding constructor therefore does not exclude it.
class DerivedFromForwardingCtorExcludingSelfType : public ForwardingCtorExcludingSelfType
{
  public:
    using ForwardingCtorExcludingSelfType::ForwardingCtorExcludingSelfType;
};

/// @brief Has a forwarding constructor that also takes this type, and records if it was copy constructed.
///
/// Direct-initialisation from a non-const lvalue of this type takes the forwarding constructor.
class ForwardingCtorAcceptingSelfType
{
  public:
    // Takes its own type on purpose, which is the hazard that bugprone-forwarding-reference-overload describes.
    // NOLINTBEGIN(bugprone-forwarding-reference-overload)
    template <typename T>
    explicit ForwardingCtorAcceptingSelfType(T&&)
    {
    }
    // NOLINTEND(bugprone-forwarding-reference-overload)
    ForwardingCtorAcceptingSelfType(const ForwardingCtorAcceptingSelfType&) : was_copy_constructed_{true} {}
    ForwardingCtorAcceptingSelfType& operator=(const ForwardingCtorAcceptingSelfType&) = delete;
    ForwardingCtorAcceptingSelfType(ForwardingCtorAcceptingSelfType&&) = delete;
    ForwardingCtorAcceptingSelfType& operator=(ForwardingCtorAcceptingSelfType&&) = delete;

    ~ForwardingCtorAcceptingSelfType() = default;

    /// @brief Whether the copy constructor produced this object.
    [[nodiscard]] bool WasCopyConstructed() const noexcept
    {
        return was_copy_constructed_;
    }

  private:
    /// @brief true if the copy constructor produced this object, false if the forwarding one did.
    bool was_copy_constructed_{false};
};

/// @brief Copyable and movable, with a non-explicit constructor taking two arguments.
///
/// It can therefore be copy-list-initialised from a braced-init-list such as `{1, 2}`.
/// The constructor stores the sum of its arguments.
class ListInitialisableType
{
  public:
    ListInitialisableType(int first, int second) : value_{first + second} {}

    /// @brief The sum of the constructor arguments.
    [[nodiscard]] int GetValue() const noexcept
    {
        return value_;
    }

  private:
    /// @brief The sum of the constructor arguments.
    int value_;
};

/// @brief Move only, with an implicit constructor from std::int64_t and an explicit one from int, recording which ran.
///
/// Copy-initialisation from int can only use the first, while direct-initialisation from int picks the second.
class ImplicitlyAndExplicitlyConstructibleType
{
  public:
    // coverity[autosar_cpp14_a12_1_4_violation] implicit on purpose, see above
    ImplicitlyAndExplicitlyConstructibleType(std::int64_t) : was_converted_implicitly_{true} {}
    explicit ImplicitlyAndExplicitlyConstructibleType(int) : was_converted_implicitly_{false} {}
    ImplicitlyAndExplicitlyConstructibleType(const ImplicitlyAndExplicitlyConstructibleType&) = delete;
    ImplicitlyAndExplicitlyConstructibleType& operator=(const ImplicitlyAndExplicitlyConstructibleType&) = delete;
    ImplicitlyAndExplicitlyConstructibleType(ImplicitlyAndExplicitlyConstructibleType&&) noexcept = default;
    ImplicitlyAndExplicitlyConstructibleType& operator=(ImplicitlyAndExplicitlyConstructibleType&&) = delete;

    ~ImplicitlyAndExplicitlyConstructibleType() = default;

    /// @brief Whether the implicit constructor ran.
    [[nodiscard]] bool WasConvertedImplicitly() const noexcept
    {
        return was_converted_implicitly_;
    }

  private:
    /// @brief true if the implicit constructor ran, false if the explicit one did.
    bool was_converted_implicitly_;
};

}  // namespace testing
}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_FUTURE_TEST_TYPES_H
