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
#include "score/language/safecpp/scoped_function/details/allocator_aware_type_erasure_pointer.h"

#include "score/language/safecpp/scoped_function/details/allocator_aware_erased_type.h"
#include "score/language/safecpp/scoped_function/details/instrumented_memory_resource.h"
#include "score/language/safecpp/scoped_function/details/testing_allocator.h"
#include "score/quality/compiler_warnings/warnings.h"

#include <score/memory.hpp>

#include <gtest/gtest.h>

namespace score::safecpp::details
{
namespace
{

class SomeInterface : public AllocatorAwareErasedType<SomeInterface>
{
  public:
    virtual ~SomeInterface() = default;
    [[nodiscard]] virtual std::int32_t GetData() const noexcept = 0;
    virtual void SetData(std::int32_t) noexcept = 0;
};

class SomeInterfaceImpl : public SomeInterface
{
  public:
    explicit SomeInterfaceImpl(const std::int32_t data) : data_{data} {}

    ~SomeInterfaceImpl() override = default;

    [[nodiscard]] TypeErasurePointer<SomeInterface> Copy(
        score::cpp::pmr::polymorphic_allocator<SomeInterface> allocator) const override
    {
        return MakeTypeErasurePointer<SomeInterfaceImpl>(allocator, data_);
    }

    [[nodiscard]] TypeErasurePointer<SomeInterface> Move(
        score::cpp::pmr::polymorphic_allocator<SomeInterface> allocator) noexcept override
    {
        return MakeTypeErasurePointer<SomeInterfaceImpl>(allocator, data_);
    }

    [[nodiscard]] std::int32_t GetData() const noexcept override
    {
        return data_;
    }
    void SetData(const std::int32_t data) noexcept override
    {
        data_ = data;
    }

  private:
    std::int32_t data_;
};

using NonPropagatingAllocator = TestingAllocator<std::byte, std::false_type>;
using PropagatingAllocator = TestingAllocator<std::byte, std::true_type>;

class AllocatorAwareTypeErasurePointerTest : public ::testing::Test
{
  public:
    PropagatingAllocator propagating_allocator_{};
    NonPropagatingAllocator non_propagating_allocator_{};
};

TEST_F(AllocatorAwareTypeErasurePointerTest, CanEmptyConstructWithoutAllocator)
{
    const AllocatorAwareTypeErasurePointer<SomeInterface, PropagatingAllocator> pointer{};
    ASSERT_EQ(pointer, nullptr);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, CanEmptyConstructWithAllocator)
{
    const AllocatorAwareTypeErasurePointer<SomeInterface, PropagatingAllocator> pointer{propagating_allocator_};
    ASSERT_EQ(pointer, nullptr);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, InstantiationOfErasedImplementationWorks)
{
    const std::int32_t expected_data{15};
    const auto pointer = MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, PropagatingAllocator>(
        propagating_allocator_, expected_data);
    EXPECT_EQ(pointer->GetData(), expected_data);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, InstantiationUsesSpecifiedAllocator)
{
    InstrumentedMemoryResource instrumented_memory_resource{};
    score::cpp::pmr::polymorphic_allocator<std::byte> allocator{&instrumented_memory_resource};

    const std::int32_t expected_data{15};
    const auto pointer =
        MakeAllocatorAwareTypeErasurePointer<SomeInterface,
                                             SomeInterfaceImpl,
                                             score::cpp::pmr::polymorphic_allocator<std::byte>>(allocator, expected_data);
    EXPECT_EQ(pointer->GetData(), expected_data);

    EXPECT_EQ(instrumented_memory_resource.GetNumberOfAllocations(), 1);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, CanCopyConstruct)
{
    const std::int32_t expected_data{15};
    auto pointer = MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, PropagatingAllocator>(
        propagating_allocator_, expected_data);
    const auto copied_pointer = pointer;

    pointer->SetData(14);
    EXPECT_EQ(copied_pointer->GetData(), expected_data);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, CanCopyConstructWithNewAllocator)
{
    InstrumentedMemoryResource instrumented_memory_resource{};
    score::cpp::pmr::polymorphic_allocator<std::byte> allocator{&instrumented_memory_resource};

    const std::int32_t expected_data{15};
    auto pointer = MakeAllocatorAwareTypeErasurePointer<SomeInterface,
                                                        SomeInterfaceImpl,
                                                        score::cpp::pmr::polymorphic_allocator<std::byte>>(
        score::cpp::pmr::polymorphic_allocator<std::byte>{}, expected_data);
    const AllocatorAwareTypeErasurePointer<SomeInterface, score::cpp::pmr::polymorphic_allocator<std::byte>> copied_pointer{
        std::allocator_arg, allocator, pointer};

    pointer->SetData(14);
    EXPECT_EQ(copied_pointer->GetData(), expected_data);
    EXPECT_EQ(instrumented_memory_resource.GetNumberOfAllocations(), 1);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, CanCopyAssign)
{
    const std::int32_t expected_data{15};
    auto pointer = MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, PropagatingAllocator>(
        propagating_allocator_, expected_data);
    AllocatorAwareTypeErasurePointer<SomeInterface, PropagatingAllocator> copied_pointer{};
    copied_pointer = pointer;

    pointer->SetData(14);
    EXPECT_EQ(copied_pointer->GetData(), expected_data);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, CanCopyAssignNullptr)
{
    const std::int32_t data{15};
    AllocatorAwareTypeErasurePointer<SomeInterface, PropagatingAllocator> source{};
    auto target = MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, PropagatingAllocator>(
        propagating_allocator_, data);
    target = source;

    EXPECT_EQ(target.get(), nullptr);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, PropagatingAllocatorIsPropagatedWithCopyAssignment)
{
    constexpr std::int32_t data{15};
    const auto pointer = MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, PropagatingAllocator>(
        propagating_allocator_, data);
    AllocatorAwareTypeErasurePointer<SomeInterface, PropagatingAllocator> copied_pointer{};
    copied_pointer = pointer;

    EXPECT_EQ(pointer.get_allocator(), copied_pointer.get_allocator());
}

TEST_F(AllocatorAwareTypeErasurePointerTest, CopyAssignmentHandlesPropagationOfAllocatorIfCurrentAllocatorIsEqual)
{
    constexpr std::int32_t data{15};
    PropagatingAllocator propagating_allocator{};
    const auto pointer = MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, PropagatingAllocator>(
        propagating_allocator, data);
    AllocatorAwareTypeErasurePointer<SomeInterface, PropagatingAllocator> copied_pointer{propagating_allocator};
    copied_pointer = pointer;

    EXPECT_EQ(pointer.get_allocator(), copied_pointer.get_allocator());
}

TEST_F(AllocatorAwareTypeErasurePointerTest, NonPropagatingAllocatorIsNotPropagatedWithCopyAssignment)
{
    constexpr std::int32_t data{15};
    const auto pointer =
        MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, NonPropagatingAllocator>(
            non_propagating_allocator_, data);
    AllocatorAwareTypeErasurePointer<SomeInterface, NonPropagatingAllocator> copied_pointer{};
    copied_pointer = pointer;

    EXPECT_NE(pointer.get_allocator(), copied_pointer.get_allocator());
}

TEST_F(AllocatorAwareTypeErasurePointerTest, CanMoveConstruct)
{
    const std::int32_t expected_data{15};
    auto pointer = MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, PropagatingAllocator>(
        propagating_allocator_, expected_data);
    const auto moved_pointer = std::move(pointer);
    EXPECT_EQ(moved_pointer->GetData(), expected_data);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, CanMoveConstructWithSameAllocatorAndNullptr)
{
    auto pointer = AllocatorAwareTypeErasurePointer<SomeInterface, PropagatingAllocator>{};
    const AllocatorAwareTypeErasurePointer<SomeInterface, PropagatingAllocator> moved_pointer{
        std::allocator_arg, propagating_allocator_, std::move(pointer)};

    EXPECT_EQ(moved_pointer.get(), nullptr);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, CanMoveConstructWithDifferentAllocatorAndNullptr)
{
    auto pointer = AllocatorAwareTypeErasurePointer<SomeInterface, std::allocator<std::byte>>{};
    const AllocatorAwareTypeErasurePointer<SomeInterface, PropagatingAllocator> moved_pointer{
        std::allocator_arg, propagating_allocator_, std::move(pointer)};

    EXPECT_EQ(moved_pointer.get(), nullptr);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, CanMoveConstructWithNewAllocator)
{
    InstrumentedMemoryResource instrumented_memory_resource{};
    score::cpp::pmr::polymorphic_allocator<std::byte> allocator{&instrumented_memory_resource};

    const std::int32_t expected_data{15};
    auto pointer = MakeAllocatorAwareTypeErasurePointer<SomeInterface,
                                                        SomeInterfaceImpl,
                                                        score::cpp::pmr::polymorphic_allocator<std::byte>>(
        score::cpp::pmr::polymorphic_allocator<std::byte>{}, expected_data);
    const AllocatorAwareTypeErasurePointer<SomeInterface, score::cpp::pmr::polymorphic_allocator<std::byte>> moved_pointer{
        std::allocator_arg, allocator, std::move(pointer)};

    EXPECT_EQ(moved_pointer->GetData(), expected_data);
    EXPECT_EQ(instrumented_memory_resource.GetNumberOfAllocations(), 1);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, CanMoveAssign)
{
    const std::int32_t expected_data{15};
    auto pointer = MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, PropagatingAllocator>(
        propagating_allocator_, expected_data);
    AllocatorAwareTypeErasurePointer<SomeInterface, PropagatingAllocator> copied_pointer{};
    copied_pointer = std::move(pointer);
    EXPECT_EQ(copied_pointer->GetData(), expected_data);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, CanMoveAssignNullptr)
{
    const std::int32_t data{15};
    AllocatorAwareTypeErasurePointer<SomeInterface, PropagatingAllocator> source{};
    auto target = MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, PropagatingAllocator>(
        propagating_allocator_, data);
    target = std::move(source);

    EXPECT_EQ(target.get(), nullptr);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, CanMoveAssignSelfWithoutAdverseEffects)
{
    const std::int32_t expected_data{15};
    auto target = MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, PropagatingAllocator>(
        propagating_allocator_, expected_data);

    DISABLE_WARNING_PUSH
    DISABLE_WARNING_SELF_MOVE

    auto* target_ptr = &target;
    target = std::move(*target_ptr);

    DISABLE_WARNING_POP

    EXPECT_EQ(target->GetData(), expected_data);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, PropagatingAllocatorIsPropagatedWithMoveAssignment)
{
    constexpr std::int32_t data{15};
    auto pointer = MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, PropagatingAllocator>(
        propagating_allocator_, data);
    AllocatorAwareTypeErasurePointer<SomeInterface, PropagatingAllocator> moved_pointer{};
    moved_pointer = std::move(pointer);

    EXPECT_EQ(propagating_allocator_, moved_pointer.get_allocator());
}

TEST_F(AllocatorAwareTypeErasurePointerTest, NonPropagatingAllocatorIsNotPropagatedWithMoveAssignment)
{
    constexpr std::int32_t data{15};
    auto pointer = MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, NonPropagatingAllocator>(
        non_propagating_allocator_, data);
    AllocatorAwareTypeErasurePointer<SomeInterface, NonPropagatingAllocator> moved_pointer{};
    moved_pointer = std::move(pointer);

    EXPECT_NE(non_propagating_allocator_, moved_pointer.get_allocator());
}

TEST_F(AllocatorAwareTypeErasurePointerTest, GetAllocatorReturnsUsedAllocator)
{
    const std::int32_t data{15};
    const auto pointer = MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, PropagatingAllocator>(
        propagating_allocator_, data);
    EXPECT_EQ(pointer.get_allocator(), propagating_allocator_);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, ConstGetReturnsRawPointerToInterface)
{
    const std::int32_t expected_data{15};
    const auto pointer = MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, PropagatingAllocator>(
        propagating_allocator_, expected_data);
    static_assert(std::is_const_v<std::remove_pointer_t<decltype(pointer.get())>>);
    const SomeInterface* raw_pointer = pointer.get();
    EXPECT_EQ(raw_pointer->GetData(), expected_data);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, GetReturnsRawPointerToInterface)
{
    const std::int32_t data{15};
    auto pointer = MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, PropagatingAllocator>(
        propagating_allocator_, data);
    static_assert(!std::is_const_v<std::remove_pointer_t<decltype(pointer.get())>>);
    SomeInterface* raw_pointer = pointer.get();
    const std::int32_t expected_data{16};
    raw_pointer->SetData(expected_data);
    EXPECT_EQ(raw_pointer->GetData(), expected_data);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, ConstDereferencingOperatorReturnsReferenceToInterface)
{
    const std::int32_t expected_data{15};
    const auto pointer = MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, PropagatingAllocator>(
        propagating_allocator_, expected_data);
    static_assert(std::is_const_v<std::remove_reference_t<decltype(*pointer)>>);
    const SomeInterface& raw_pointer = *pointer;
    EXPECT_EQ(raw_pointer.GetData(), expected_data);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, DereferencingOperatorReturnsReferenceToInterface)
{
    const std::int32_t data{15};
    auto pointer = MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, PropagatingAllocator>(
        propagating_allocator_, data);
    static_assert(!std::is_const_v<decltype(*pointer)>);
    SomeInterface& reference = *pointer;
    const std::int32_t expected_data{16};
    reference.SetData(expected_data);
    EXPECT_EQ(reference.GetData(), expected_data);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, ConstArrowOperatorReturnsConstRawPointerToInterface)
{
    const std::int32_t expected_data{15};
    const auto pointer = MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, PropagatingAllocator>(
        propagating_allocator_, expected_data);
    static_assert(std::is_const_v<std::remove_pointer_t<decltype(pointer.operator->())>>);
    const SomeInterface* raw_pointer = pointer.operator->();
    EXPECT_EQ(raw_pointer->GetData(), expected_data);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, ArrowOperatorReturnsRawPointerToInterface)
{
    const std::int32_t data{15};
    auto pointer = MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, PropagatingAllocator>(
        propagating_allocator_, data);
    static_assert(!std::is_const_v<decltype(pointer.operator->())>);
    SomeInterface* raw_pointer = pointer.operator->();

    const std::int32_t expected_data{16};
    raw_pointer->SetData(expected_data);
    EXPECT_EQ(raw_pointer->GetData(), expected_data);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, ComparesEqualWithEqualAllocatorAwareTypeErasurePointer)
{
    const std::int32_t expected_data{15};
    const auto pointer = MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, PropagatingAllocator>(
        propagating_allocator_, expected_data);

    EXPECT_TRUE(pointer == pointer);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, ComparesInequalWithDifferentAllocatorAwareTypeErasurePointerToEqualObject)
{
    const std::int32_t expected_data{15};
    const auto lhs_pointer =
        MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, PropagatingAllocator>(
            propagating_allocator_, expected_data);

    const auto rhs_pointer =
        MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, NonPropagatingAllocator>(
            non_propagating_allocator_, expected_data);

    EXPECT_FALSE(lhs_pointer == rhs_pointer);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, ComparesNotInequalWithEqualAllocatorAwareTypeErasurePointer)
{
    const std::int32_t expected_data{15};
    const auto pointer = MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, PropagatingAllocator>(
        propagating_allocator_, expected_data);

    EXPECT_FALSE(pointer != pointer);
}

TEST_F(AllocatorAwareTypeErasurePointerTest,
       ComparesNotInequalWithDifferentAllocatorAwareTypeErasurePointerToEqualObject)
{
    const std::int32_t expected_data{15};
    const auto lhs_pointer =
        MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, PropagatingAllocator>(
            propagating_allocator_, expected_data);

    const auto rhs_pointer =
        MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, NonPropagatingAllocator>(
            non_propagating_allocator_, expected_data);

    EXPECT_TRUE(lhs_pointer != rhs_pointer);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, ComparesEqualWithNullptrWhenInitializedEmpty)
{
    AllocatorAwareTypeErasurePointer<SomeInterface, PropagatingAllocator> pointer{};
    EXPECT_TRUE(pointer == nullptr);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, ComparesInequalWithNullptrWhenInitializedWithData)
{
    const std::int32_t data{15};
    const auto pointer = MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, PropagatingAllocator>(
        propagating_allocator_, data);
    EXPECT_FALSE(pointer == nullptr);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, ComparesNotInequalWithNullptrWhenInitializedEmpty)
{
    AllocatorAwareTypeErasurePointer<SomeInterface, PropagatingAllocator> pointer{};
    EXPECT_FALSE(pointer != nullptr);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, ComparesNotInequalWithNullptrWhenInitializedWithData)
{
    const std::int32_t data{15};
    const auto pointer = MakeAllocatorAwareTypeErasurePointer<SomeInterface, SomeInterfaceImpl, PropagatingAllocator>(
        propagating_allocator_, data);
    EXPECT_TRUE(pointer != nullptr);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, UsesAllocatorIsTrueForSameAllocator)
{
    static_assert(std::uses_allocator_v<AllocatorAwareTypeErasurePointer<SomeInterface, PropagatingAllocator>,
                                        PropagatingAllocator>);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, UsesAllocatorIsFalseForDifferentAllocator)
{
    static_assert(!std::uses_allocator_v<AllocatorAwareTypeErasurePointer<SomeInterface, PropagatingAllocator>,
                                         NonPropagatingAllocator>);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, DoesNotReallocateDuringMoveConstructionWithSameAllocator)
{
    InstrumentedMemoryResource instrumented_memory_resource{};
    score::cpp::pmr::polymorphic_allocator<std::byte> allocator{&instrumented_memory_resource};

    const std::int32_t data{15};
    auto pointer = MakeAllocatorAwareTypeErasurePointer<SomeInterface,
                                                        SomeInterfaceImpl,
                                                        score::cpp::pmr::polymorphic_allocator<std::byte>>(allocator, data);

    const AllocatorAwareTypeErasurePointer<SomeInterface, score::cpp::pmr::polymorphic_allocator<std::byte>> moved_pointer{
        std::allocator_arg, allocator, std::move(pointer)};

    EXPECT_EQ(instrumented_memory_resource.GetNumberOfAllocations(), 1);
}

TEST_F(AllocatorAwareTypeErasurePointerTest, DoesNotReallocateDuringMoveAssignmentWithSameAllocator)
{
    InstrumentedMemoryResource instrumented_memory_resource{};
    score::cpp::pmr::polymorphic_allocator<std::byte> allocator{&instrumented_memory_resource};

    const std::int32_t data{15};
    auto pointer = MakeAllocatorAwareTypeErasurePointer<SomeInterface,
                                                        SomeInterfaceImpl,
                                                        score::cpp::pmr::polymorphic_allocator<std::byte>>(allocator, data);

    AllocatorAwareTypeErasurePointer<SomeInterface, score::cpp::pmr::polymorphic_allocator<std::byte>> moved_pointer{
        allocator};
    moved_pointer = std::move(pointer);

    EXPECT_EQ(instrumented_memory_resource.GetNumberOfAllocations(), 1);
}

}  // namespace
}  // namespace score::safecpp::details
