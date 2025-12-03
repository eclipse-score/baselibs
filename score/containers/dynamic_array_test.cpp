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
#include "score/containers/dynamic_array.h"

#include "score/containers/test/allocator_test_type_helpers.h"
#include "score/containers/test/container_test_types.h"
#include "score/containers/test/custom_allocator_mock.h"

#include "score/memory/shared/fake/my_memory_resource.h"
#include "score/memory/shared/polymorphic_offset_ptr_allocator.h"

#include <score/assert_support.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstddef>
#include <memory>

using namespace score::containers;
using namespace score::memory::shared;

constexpr std::size_t kNonEmptyArraySize{10U};
constexpr std::size_t kEmptyArraySize{0U};

template <typename Allocator>
class DynamicArrayTestFixture : public ::testing::Test
{
    void SetUp() override {}
    void TearDown() override
    {
        NonMoveableAndCopyableElementType::ResetDestructorCount();
    }

  protected:
    template <typename T, typename Alloc = Allocator>
    auto getTypeSpecificAllocator() -> typename std::allocator_traits<Alloc>::template rebind_alloc<T>
    {
        auto trivial_type_alloc = GetAllocator<T, Alloc>(memory_resource_);
        using traits_alloc = std::allocator_traits<decltype(trivial_type_alloc)>;
        using rebind_allocator_type = typename traits_alloc::template rebind_alloc<T>;
        rebind_allocator_type specific_type_alloc = trivial_type_alloc;
        return specific_type_alloc;
    }

    score::memory::shared::test::MyMemoryResource memory_resource_{};
};

using AllocatorTypes = ::testing::Types<std::allocator<TrivialType>, PolymorphicOffsetPtrAllocator<TrivialType>>;
TYPED_TEST_SUITE(DynamicArrayTestFixture, AllocatorTypes, );

TYPED_TEST(DynamicArrayTestFixture, CanConstructWithTrivialType)
{
    DynamicArray<TrivialType, TypeParam> unit{kNonEmptyArraySize,
                                              GetAllocator<TrivialType, TypeParam>(this->memory_resource_)};
    EXPECT_EQ(unit.size(), 10);

    for (auto i = 0U; i < unit.size(); ++i)
    {
        EXPECT_EQ(unit.at(i), 0U);
    }
}

TYPED_TEST(DynamicArrayTestFixture, ConstructTrivialEmpty)
{
    DynamicArray<TrivialType, TypeParam> unit{kEmptyArraySize,
                                              GetAllocator<TrivialType, TypeParam>(this->memory_resource_)};
    EXPECT_EQ(unit.size(), kEmptyArraySize);
}
TYPED_TEST(DynamicArrayTestFixture, ConstructNonTrivial)
{
    auto non_trivial_type_alloc = this->template getTypeSpecificAllocator<NonTrivialType>();

    DynamicArray<NonTrivialType, decltype(non_trivial_type_alloc)> unit{kNonEmptyArraySize, non_trivial_type_alloc};
    EXPECT_EQ(unit.size(), kNonEmptyArraySize);

    for (auto i = 0U; i < unit.size(); ++i)
    {
        EXPECT_EQ(unit.at(i).member_1_, 42U);
        EXPECT_EQ(unit.at(i).member_2_, 1.0f);
    }
}

TYPED_TEST(DynamicArrayTestFixture, CopyConstructTrivial)
{
    DynamicArray<TrivialType, TypeParam> source_unit{kNonEmptyArraySize,
                                                     GetAllocator<TrivialType, TypeParam>(this->memory_resource_)};

    for (auto i = 0U; i < source_unit.size(); ++i)
    {
        source_unit.at(i) = 177U + i;
    }

    DynamicArray<TrivialType, TypeParam> unit{source_unit};

    EXPECT_EQ(source_unit.size(), unit.size());

    for (auto i = 0U; i < unit.size(); ++i)
    {
        EXPECT_EQ(unit.at(i), 177U + i);
    }
}

TYPED_TEST(DynamicArrayTestFixture, CopyConstructNonTrivial)
{
    auto non_trivial_type_alloc = this->template getTypeSpecificAllocator<NonTrivialType>();
    DynamicArray<NonTrivialType, decltype(non_trivial_type_alloc)> source_unit{kNonEmptyArraySize,
                                                                               non_trivial_type_alloc};

    for (auto i = 0U; i < source_unit.size(); ++i)
    {
        source_unit.at(i).member_1_ = 177U + i;
        source_unit.at(i).member_2_ = 3.0f;
    }

    DynamicArray<NonTrivialType, decltype(non_trivial_type_alloc)> unit{source_unit};

    EXPECT_EQ(source_unit.size(), unit.size());

    for (auto i = 0U; i < unit.size(); ++i)
    {
        EXPECT_EQ(unit.at(i).member_1_, 177U + i);
        EXPECT_EQ(unit.at(i).member_2_, 3.0f);
    }
}

TYPED_TEST(DynamicArrayTestFixture, ConstructNonTrivialWithDefaultValue)
{
    auto non_trivial_type_alloc = this->template getTypeSpecificAllocator<NonTrivialType>();
    NonTrivialType default_value{99U, 2.0f};

    DynamicArray<NonTrivialType, decltype(non_trivial_type_alloc)> unit{
        kNonEmptyArraySize, default_value, non_trivial_type_alloc};
    EXPECT_EQ(unit.size(), kNonEmptyArraySize);

    for (auto i = 0U; i < unit.size(); ++i)
    {
        EXPECT_EQ(unit.at(i).member_1_, 99U);
        EXPECT_EQ(unit.at(i).member_2_, 2.0f);
    }
}

TYPED_TEST(DynamicArrayTestFixture, MoveConstructTrivial)
{
    DynamicArray<TrivialType, TypeParam> unit{kNonEmptyArraySize,
                                              GetAllocator<TrivialType, TypeParam>(this->memory_resource_)};

    // set element values specifically
    for (std::size_t i = 0; i < kNonEmptyArraySize; ++i)
    {
        unit.at(i) = static_cast<TrivialType>(i);
    }

    DynamicArray<TrivialType, TypeParam> unit2{std::move(unit)};
    EXPECT_EQ(unit2.size(), kNonEmptyArraySize);
    // and expect, that the element values are correct
    for (auto i = 0U; i < kNonEmptyArraySize; ++i)
    {
        EXPECT_EQ(unit2.at(i), i);
    }
}

TYPED_TEST(DynamicArrayTestFixture, MoveConstructNonTrivial)
{
    auto non_trivial_type_alloc = this->template getTypeSpecificAllocator<NonMoveableAndCopyableElementType>();

    // given a unit with non-trivial element type
    DynamicArray<NonMoveableAndCopyableElementType, decltype(non_trivial_type_alloc)> unit{kNonEmptyArraySize,
                                                                                           non_trivial_type_alloc};

    // set element values specifically
    for (std::size_t i = 0; i < kNonEmptyArraySize; ++i)
    {
        unit.at(i).i_ = static_cast<int>(i);
    }
    // and a 2nd unit move-constructed from the 1st unit
    DynamicArray<NonMoveableAndCopyableElementType, decltype(non_trivial_type_alloc)> unit2{std::move(unit)};

    // expect, that the size of the 2nd unit equals the size from the 1st unit
    EXPECT_EQ(unit2.size(), kNonEmptyArraySize);
    // and expect, that the element values are correct
    for (std::size_t i = 0; i < kNonEmptyArraySize; ++i)
    {
        EXPECT_EQ(unit2.at(i).i_, i);
    }

    // and expect that no elements have been destructed during move-construction
    EXPECT_EQ(NonMoveableAndCopyableElementType::GetDestructorCount(), 0);
}

TYPED_TEST(DynamicArrayTestFixture, MoveAssignTrivial)
{
    const std::size_t array_size1{10U};
    const std::size_t array_size2{20U};
    DynamicArray<TrivialType, TypeParam> unit{array_size1,
                                              GetAllocator<TrivialType, TypeParam>(this->memory_resource_)};
    DynamicArray<TrivialType, TypeParam> unit2{array_size2,
                                               GetAllocator<TrivialType, TypeParam>(this->memory_resource_)};

    // set element values specifically
    for (std::size_t i = 0; i < array_size1; ++i)
    {
        unit.at(i) = static_cast<TrivialType>(i);
    }

    unit2 = std::move(unit);
    EXPECT_EQ(unit2.size(), array_size1);

    // and expect, that the element values are correct
    for (std::size_t i = 0; i < array_size1; ++i)
    {
        EXPECT_EQ(unit2.at(i), i);
    }
}

TYPED_TEST(DynamicArrayTestFixture, MoveAssignNonTrivial)
{
    auto non_trivial_type_alloc = this->template getTypeSpecificAllocator<NonMoveableAndCopyableElementType>();
    const std::size_t array_size1{10U};
    const std::size_t array_size2{20U};
    {
        // given a unit with non-trivial element type
        DynamicArray<NonMoveableAndCopyableElementType, decltype(non_trivial_type_alloc)> unit{array_size1,
                                                                                               non_trivial_type_alloc};
        // set element values specifically
        for (std::size_t i = 0; i < array_size1; ++i)
        {
            unit.at(i).i_ = static_cast<int>(i);
        }
        // and a 2nd unit
        DynamicArray<NonMoveableAndCopyableElementType, decltype(non_trivial_type_alloc)> unit2{array_size2,
                                                                                                non_trivial_type_alloc};

        // when move assigning the 1st unit to the 2nd unit
        unit2 = std::move(unit);
        // expect, that the size of the 2nd unit equals the size from the 1st unit
        EXPECT_EQ(unit2.size(), array_size1);
        // and expect that no elements have been destructed during move-assignment
        EXPECT_EQ(NonMoveableAndCopyableElementType::GetDestructorCount(), 0);
        // and expect, that the element values are correct
        for (std::size_t i = 0; i < array_size1; ++i)
        {
            EXPECT_EQ(unit2.at(i).i_, i);
        }
    }
    // EXPECT, that after both units are out of scope, dtors have been called for all elements
    EXPECT_EQ(NonMoveableAndCopyableElementType::GetDestructorCount(), array_size1 + array_size2);
}

TYPED_TEST(DynamicArrayTestFixture, SelfMoveAssign)
{
    // given a DynamicArray of trivial type
    DynamicArray<TrivialType, TypeParam> unit{kNonEmptyArraySize,
                                              GetAllocator<TrivialType, TypeParam>(this->memory_resource_)};

    // when doing a self-move-assign
    unit = std::move(unit);

    // expect, that the unit afterward still has the same size
    EXPECT_EQ(unit.size(), kNonEmptyArraySize);

    // and expect, that the element values are all intact
    for (auto i = 0U; i < kNonEmptyArraySize; ++i)
    {
        EXPECT_EQ(unit.at(i), 0U);
    }
}

TYPED_TEST(DynamicArrayTestFixture, CanSetValueOfArrayElements)
{
    DynamicArray<TrivialType, TypeParam> unit{kNonEmptyArraySize,
                                              GetAllocator<TrivialType, TypeParam>(this->memory_resource_)};

    for (TrivialType i = 0; i < kNonEmptyArraySize; ++i)
    {
        unit.at(i) = i;
    }

    for (TrivialType i = 0; i < kNonEmptyArraySize; ++i)
    {
        const auto& element = unit.at(i);
        EXPECT_EQ(element, i);
    }
}

TYPED_TEST(DynamicArrayTestFixture, CanConstructWithNonMoveableOrCopyableElements)
{
    auto non_trivial_type_alloc = this->template getTypeSpecificAllocator<NonMoveableAndCopyableElementType>();

    DynamicArray<NonMoveableAndCopyableElementType, decltype(non_trivial_type_alloc)> unit{kNonEmptyArraySize,
                                                                                           non_trivial_type_alloc};
    // expect, that the unit has the expected size.
    EXPECT_EQ(unit.size(), kNonEmptyArraySize);
}

TYPED_TEST(DynamicArrayTestFixture, DestructorOfNonTrivialTypesCalled)
{
    auto non_trivial_type_alloc = this->template getTypeSpecificAllocator<NonMoveableAndCopyableElementType>();

    {
        DynamicArray<NonMoveableAndCopyableElementType, decltype(non_trivial_type_alloc)> unit{kNonEmptyArraySize,
                                                                                               non_trivial_type_alloc};
    }
    // expect, that the dtor of non trivial type gets called for each element.
    EXPECT_EQ(NonMoveableAndCopyableElementType::GetDestructorCount(), kNonEmptyArraySize);
}

TYPED_TEST(DynamicArrayTestFixture, CanConstructWithTriviallyConstructableDestructibleElements)
{
    auto trivially_constructible_destructible_alloc =
        this->template getTypeSpecificAllocator<TriviallyConstructibleDestructibleType>();

    DynamicArray<TriviallyConstructibleDestructibleType, decltype(trivially_constructible_destructible_alloc)> unit{
        kNonEmptyArraySize, trivially_constructible_destructible_alloc};
    EXPECT_EQ(unit.size(), 10);

    for (auto i = 0U; i < unit.size(); ++i)
    {
        EXPECT_EQ(unit.at(i).i, 0U);
        EXPECT_EQ(unit.at(i).j, 0U);
    }
}

TYPED_TEST(DynamicArrayTestFixture, ConstructingDynamicArrayWithTrivialTypeWithTooManyElementsTerminates)
{
    constexpr std::size_t array_size_exceeding_limit{std::numeric_limits<std::size_t>::max()};

    const auto initialise_dynamic_array = [this] {
        DynamicArray<TrivialType, TypeParam> unit{array_size_exceeding_limit,
                                                  GetAllocator<TrivialType, TypeParam>(this->memory_resource_)};
    };

    // Since a std::exception is thrown by std::allocator_traits<Allocator>::allocate(), rather than by an AMP
    // assertion / precondition, we capture this using the gtest framework instead of SCORE_LANGUAGE_FUTURECPP_ASSERT_CONTRACT_VIOLATED.
    EXPECT_THROW(initialise_dynamic_array(), std::exception);
}

TYPED_TEST(DynamicArrayTestFixture, AccessingConstRefArrayOutOfBoundsTerminates)
{
    DynamicArray<TrivialType> unit(kNonEmptyArraySize);

    const auto access_const_ref_out_of_bounds = [&unit]() {
        const auto& element = unit.at(kNonEmptyArraySize);
        (void)element;
    };

    SCORE_LANGUAGE_FUTURECPP_ASSERT_CONTRACT_VIOLATED(access_const_ref_out_of_bounds());
}

TYPED_TEST(DynamicArrayTestFixture, IteratingTrivialType)
{
    DynamicArray<TrivialType, TypeParam> unit{kNonEmptyArraySize,
                                              GetAllocator<TrivialType, TypeParam>(this->memory_resource_)};

    TrivialType value{0};
    for (auto it = unit.begin(); it != unit.end(); it++)
    {
        *it = value++;
    }

    value = 0;
    for (auto it = unit.begin(); it != unit.end(); it++)
    {
        EXPECT_EQ(*it, value++);
    }
    EXPECT_EQ(value, 10);
}

TYPED_TEST(DynamicArrayTestFixture, IteratingNonTrivialType)
{
    auto non_trivial_type_alloc = this->template getTypeSpecificAllocator<NonTrivialType>();

    DynamicArray<NonTrivialType, decltype(non_trivial_type_alloc)> unit{kNonEmptyArraySize, non_trivial_type_alloc};

    NonTrivialType value{};
    std::uint32_t member_1_value{0};

    for (auto it = unit.begin(); it != unit.end(); it++)
    {
        it->member_1_ = member_1_value++;
    }

    member_1_value = 0;
    for (auto it = unit.begin(); it != unit.end(); it++)
    {
        EXPECT_EQ(it->member_1_, member_1_value++);
    }
    EXPECT_EQ(member_1_value, 10);
}

TYPED_TEST(DynamicArrayTestFixture, ConstIteratingNonTrivialType)
{
    auto non_trivial_type_alloc = this->template getTypeSpecificAllocator<NonTrivialType>();

    const DynamicArray<NonTrivialType, decltype(non_trivial_type_alloc)> unit{kNonEmptyArraySize,
                                                                              non_trivial_type_alloc};

    for (auto it = unit.begin(); it != unit.end(); it++)
    {
        EXPECT_EQ(it->member_1_, 42);
        EXPECT_EQ(it->member_2_, 1.0f);
    }
}

TYPED_TEST(DynamicArrayTestFixture, ConstIteratingNonTrivialTypeVariation)
{
    auto non_trivial_type_alloc = this->template getTypeSpecificAllocator<NonTrivialType>();

    DynamicArray<NonTrivialType, decltype(non_trivial_type_alloc)> unit{kNonEmptyArraySize, non_trivial_type_alloc};

    auto counter{0U};

    for (auto it = unit.cbegin(); it != unit.cend(); it++, counter++)
    {
        EXPECT_EQ(it->member_1_, 42);
        EXPECT_EQ(it->member_2_, 1.0f);
    }
    EXPECT_EQ(counter, 10);
}

TYPED_TEST(DynamicArrayTestFixture, BracketOperatorAllowsSettingDataAtIndex)
{
    DynamicArray<TrivialType> unit{kNonEmptyArraySize};
    for (TrivialType i = 0; i < unit.size(); ++i)
    {
        unit[i] = i;
    }

    for (TrivialType i = 0; i < unit.size(); ++i)
    {
        ASSERT_EQ(unit.at(i), i);
    }
}

TYPED_TEST(DynamicArrayTestFixture, BracketOperatorAllowsGettingDataAtIndex)
{
    DynamicArray<TrivialType> unit{kNonEmptyArraySize};
    for (TrivialType i = 0; i < unit.size(); ++i)
    {
        unit.at(i) = i;
    }

    for (TrivialType i = 0; i < unit.size(); ++i)
    {
        ASSERT_EQ(unit[i], i);
    }
}

TYPED_TEST(DynamicArrayTestFixture, ConstBracketOperatorAllowsGettingDataAtIndex)
{
    DynamicArray<TrivialType> unit{kNonEmptyArraySize};
    for (TrivialType i = 0; i < unit.size(); ++i)
    {
        unit.at(i) = i;
    }

    auto test_const_bracket_operator = [](const DynamicArray<TrivialType>& const_unit) {
        for (TrivialType i = 0; i < const_unit.size(); ++i)
        {
            ASSERT_EQ(const_unit[i], i);
        }
    };
    test_const_bracket_operator(unit);
}

TYPED_TEST(DynamicArrayTestFixture, DataShouldReturnPointerToFirstElement)
{
    DynamicArray<TrivialType> unit{kNonEmptyArraySize};
    EXPECT_EQ(unit.data(), &unit.at(0));
}

TYPED_TEST(DynamicArrayTestFixture, ConstDataShouldReturnPointerToFirstElement)
{
    const DynamicArray<TrivialType> unit{kNonEmptyArraySize};
    EXPECT_EQ(unit.data(), &unit.at(0));
}

TYPED_TEST(DynamicArrayTestFixture, BeginIsEqualToEndWhenArrayIsEmpty)
{
    // Given an empty DynamicArray
    const DynamicArray<TrivialType> unit{0U};

    // When calling begin()
    const auto start_iterator = unit.begin();

    // Then the result is equal to end()
    EXPECT_EQ(start_iterator, unit.end());
}

TYPED_TEST(DynamicArrayTestFixture, CBeginIsEqualToCEndWhenArrayIsEmpty)
{
    // Given an empty DynamicArray
    const DynamicArray<TrivialType> unit{0U};

    // When calling cbegin()
    const auto start_iterator = unit.cbegin();

    // Then the result is equal to cend()
    EXPECT_EQ(start_iterator, unit.cend());
}

TYPED_TEST(DynamicArrayTestFixture, DataReturnsNullptrWhenArrayIsEmpty)
{
    // Given an empty DynamicArray
    const DynamicArray<TrivialType> unit{0U};

    // When calling data()
    const auto data_pointer = unit.data();

    // Then the result is a nullptr
    EXPECT_EQ(data_pointer, nullptr);
}

TYPED_TEST(DynamicArrayTestFixture, SizeReturnsZeroWhenArrayIsEmpty)
{
    // Given an empty DynamicArray
    const DynamicArray<TrivialType> unit{0U};

    // When calling size
    const auto array_size = unit.size();

    // Then the result is 0
    EXPECT_EQ(array_size, 0U);
}

TYPED_TEST(DynamicArrayTestFixture, SizeReturnsZeroWhenArrayIsEmptyWithValue)
{
    // Given an empty DynamicArray which takes an initial value
    constexpr std::size_t kNumberOfElements{0U};
    constexpr TrivialType kInitialValue{1};
    const DynamicArray<TrivialType> unit{kNumberOfElements, kInitialValue};

    // When calling size
    const auto array_size = unit.size();

    // Then the result is 0
    EXPECT_EQ(array_size, 0U);
}

TYPED_TEST(DynamicArrayTestFixture, AccessingElementWithAtWhenArrayIsEmptyTerminates)
{
    // Given an empty DynamicArray
    const DynamicArray<TrivialType> unit{0U};

    // When accessing an element with at
    // Then the program terminates
    SCORE_LANGUAGE_FUTURECPP_ASSERT_CONTRACT_VIOLATED(score::cpp::ignore = unit.at(0));
}

TYPED_TEST(DynamicArrayTestFixture, AccessingElementWithIndexOperatorWhenArrayIsEmptyTerminates)
{
    // Given an empty DynamicArray
    const DynamicArray<TrivialType> unit{0U};

    // When accessing an element with operator[]
    // Then the program terminates
    SCORE_LANGUAGE_FUTURECPP_ASSERT_CONTRACT_VIOLATED(score::cpp::ignore = unit[0]);
}

TYPED_TEST(DynamicArrayTestFixture, IteratingOverEmptyArrayIteratesZeroTimes)
{
    // Given an empty DynamicArray
    const DynamicArray<TrivialType> unit{0U};

    // When trying to iterate over the array
    std::size_t iteration_count{0U};
    for ([[maybe_unused]] auto i : unit)
    {
        iteration_count++;
    }

    // Then the program loops 0 times
    EXPECT_EQ(iteration_count, 0U);
}

TEST(EmptyDynamicArrayOfNonTrivialElementTypeMemoryTest,
     TestNeverFailsButMemcheckDoesIfEmptyArrayIsNotCleanedUpCorrectly)
{
    std::size_t array_size{0};
    DynamicArray<NonTrivialType> da(array_size);
    EXPECT_TRUE(true);
}

TEST(EmptyDynamicArrayOfTrivialElementTypeMemoryTest, TestNeverFailsButMemcheckDoesIfEmptyArrayIsNotCleanedUpCorrectly)
{
    std::size_t array_size{0};
    DynamicArray<TrivialType> da(array_size);
    EXPECT_TRUE(true);
}

// This test is to validates the fix for the memory leak issue where copying zero-size arrays would call allocate()
// but leave dynamic_array_ as nullptr
TEST(DynamicArrayCopyConstructorMemoryTest, CopyConstructorWithZeroSizeArrayDoesNotLeakMemory)
{
    // Given an empty source array
    DynamicArray<TrivialType> source_array{0U};
    EXPECT_EQ(source_array.size(), 0U);

    // When copy construct from the empty array, then this should not leak memory
    DynamicArray<TrivialType> copied_array{source_array};
    EXPECT_EQ(copied_array.size(), 0U);

    // Verify both arrays behave correctly as empty arrays
    EXPECT_EQ(source_array.data(), nullptr);
    EXPECT_EQ(copied_array.data(), nullptr);
    EXPECT_EQ(source_array.begin(), source_array.end());
    EXPECT_EQ(copied_array.begin(), copied_array.end());
}

// Test the same scenario with non-trivial types to ensure the fix works for both code paths
TEST(DynamicArrayCopyConstructorMemoryTest, CopyConstructorWithNonTrivialZeroSizeArrayDoesNotLeakMemory)
{
    // Given an empty source array of non-trivial type
    DynamicArray<NonTrivialType> source_array{0U};
    EXPECT_EQ(source_array.size(), 0U);

    // When copy construct from the empty array, then this should not leak memory
    DynamicArray<NonTrivialType> copied_array{source_array};
    EXPECT_EQ(copied_array.size(), 0U);

    // Verify both arrays behave correctly as empty arrays
    EXPECT_EQ(source_array.data(), nullptr);
    EXPECT_EQ(copied_array.data(), nullptr);
    EXPECT_EQ(source_array.begin(), source_array.end());
    EXPECT_EQ(copied_array.begin(), copied_array.end());
}
