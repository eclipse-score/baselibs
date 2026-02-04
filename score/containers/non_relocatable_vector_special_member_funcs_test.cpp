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
#include "score/containers/non_relocatable_vector.h"

#include "score/containers/test/allocator_test_type_helpers.h"
#include "score/containers/test/container_test_types.h"

#include "score/memory/shared/fake/my_bounded_memory_resource.h"
#include "score/memory/shared/polymorphic_offset_ptr_allocator.h"

#include <score/assert_support.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <type_traits>

namespace score::containers
{

using namespace score::memory::shared;
using namespace score::containers::test_types;

constexpr std::size_t kNonZeroNumberElements{10U};

template <typename TestTypes>
class NonRelocatableVectorFixture : public ::testing::Test
{
    void SetUp() override {}
    void TearDown() override
    {
        NonMoveableAndCopyableElementType::ResetDestructorCount();
    }

  protected:
    using ElementType = typename TestTypes::ElementType;
    using Allocator = typename TestTypes::Allocator;

    NonRelocatableVectorFixture& GivenANonRelocatableVectorConstructedWithNumberOfElements(
        const std::size_t number_of_elements)
    {
        unit_ = std::make_unique<NonRelocatableVector<ElementType, Allocator>>(
            number_of_elements, GetAllocator<ElementType, Allocator>(memory_resource_));
        return *this;
    }

    score::memory::shared::test::MyBoundedMemoryResource memory_resource_{2000U};
    std::unique_ptr<NonRelocatableVector<ElementType, Allocator>> unit_{nullptr};
};

TYPED_TEST_SUITE(NonRelocatableVectorFixture, AllAllocatorTypes, );

template <typename T>
using NonRelocatableVectorPolymorphicAllocatorFixture = NonRelocatableVectorFixture<T>;
TYPED_TEST_SUITE(NonRelocatableVectorPolymorphicAllocatorFixture, PolymorphicAllocatorTypes, );

template <typename T>
using NonRelocatableVectorCopyableAndMoveablePolymorphicAllocatorFixture = NonRelocatableVectorFixture<T>;
TYPED_TEST_SUITE(NonRelocatableVectorCopyableAndMoveablePolymorphicAllocatorFixture,
                 CopyableAndMoveablePolymorphicAllocatorTypes, );

template <typename T>
using NonRelocatableVectorTrivialFixture = NonRelocatableVectorFixture<T>;
TYPED_TEST_SUITE(NonRelocatableVectorTrivialFixture, TrivialAllocatorTypes, );

template <typename T>
using NonRelocatableVectorNonTrivialFixture = NonRelocatableVectorFixture<T>;
TYPED_TEST_SUITE(NonRelocatableVectorNonTrivialFixture, NonTrivialAllocatorTypes, );

template <typename T>
using NonRelocatableVectorTriviallyConstructibleDestructibleTypeFixture = NonRelocatableVectorFixture<T>;
TYPED_TEST_SUITE(NonRelocatableVectorTriviallyConstructibleDestructibleTypeFixture,
                 TriviallyConstructibleDestructibleTypeAllocatorTypes, );

template <typename T>
using NonRelocatableVectorNonMoveableAndCopyableElementTypeFixture = NonRelocatableVectorFixture<T>;
TYPED_TEST_SUITE(NonRelocatableVectorNonMoveableAndCopyableElementTypeFixture,
                 NonMoveableAndCopyableElementTypeAllocatorTypes, );

using NonRelocatableVectorSpecialMemberFunctionRecorderFixture = NonRelocatableVectorFixture<
    ContainerTestTypes<SpecialMemberFunctionRecordingType, memory::shared::PolymorphicOffsetPtrAllocator>>;

TYPED_TEST(NonRelocatableVectorFixture, ConstructingWithZeroElementsSetsSizeAndCapacityToZero)
{
    // When constructing a NonRelocatableVector with zero elements
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(0U);

    // Then the size and capacity is 0
    EXPECT_EQ(this->unit_->size(), 0U);
    EXPECT_EQ(this->unit_->capacity(), 0U);
}

TYPED_TEST(NonRelocatableVectorPolymorphicAllocatorFixture, ConstructingWithZeroElementsDoesNotAllocate)
{
    // When constructing a NonRelocatableVector with zero elements
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(0U);

    // Then no memory is allocated
    EXPECT_EQ(this->memory_resource_.GetUserAllocatedBytes(), 0U);
}

TYPED_TEST(NonRelocatableVectorFixture, ConstructingWithNonZeroElementsSetsCapacity)
{
    // When constructing a NonRelocatableVector with kNonZeroNumberElements elements
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);

    // Then only the capacity is updated
    EXPECT_EQ(this->unit_->size(), 0U);
    EXPECT_EQ(this->unit_->capacity(), kNonZeroNumberElements);
}

TYPED_TEST(NonRelocatableVectorPolymorphicAllocatorFixture, ConstructingWithNonZeroElementsAllocatesAllElements)
{
    // When constructing a NonRelocatableVector with kNonZeroNumberElements elements
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);

    // Then memory is allocated for each element
    const auto expected_memory_allocated =
        sizeof(typename NonRelocatableVectorPolymorphicAllocatorFixture<TypeParam>::ElementType) *
        kNonZeroNumberElements;
    EXPECT_EQ(this->memory_resource_.GetUserAllocatedBytes(), expected_memory_allocated);
}

TEST_F(NonRelocatableVectorSpecialMemberFunctionRecorderFixture,
       ConstructingWithNonZeroElementsDoesNotCallElementConstructors)
{
    // When constructing a NonRelocatableVector with kNonZeroNumberElements elements
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);

    // Then no element constructors are called
    EXPECT_EQ(SpecialMemberFunctionRecordingType::constructor_count, 0U);
}

TYPED_TEST(NonRelocatableVectorFixture, DestructingWithZeroElementsDoesNotDeallocate)
{
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(0U);

    // When destructing the NonRelocatableVector with zero elements
    this->unit_.reset();

    // Then no memory is deallocated
    EXPECT_EQ(this->memory_resource_.GetUserDeAllocatedBytes(), 0U);
}

TYPED_TEST(NonRelocatableVectorPolymorphicAllocatorFixture, DestructingWithNonZeroElementsDeallocatesAllElements)
{
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);

    // When destructing the NonRelocatableVector with non-zero elements
    this->unit_.reset();

    // Then memory is deallocated for all elements
    const auto expected_memory_deallocated =
        sizeof(typename NonRelocatableVectorPolymorphicAllocatorFixture<TypeParam>::ElementType) *
        kNonZeroNumberElements;
    EXPECT_EQ(this->memory_resource_.GetUserDeAllocatedBytes(), expected_memory_deallocated);
}

TEST_F(NonRelocatableVectorSpecialMemberFunctionRecorderFixture,
       DestructingWithNonZeroElementsDoesNotCallAnyDestructors)
{
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);

    // When destructing the NonRelocatableVector with non-zero elements (although no emplaced elements)
    this->unit_.reset();

    // Then no destructor are called for any elements
    EXPECT_EQ(SpecialMemberFunctionRecordingType::destructor_count, 0U);
}

TEST_F(NonRelocatableVectorSpecialMemberFunctionRecorderFixture,
       DestructingWithNonZeroEmplacedElementsCallsDestructorAllElements)
{
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);

    // and given that kNonZeroNumberElements have been emplaced
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        this->unit_->emplace_back();
    }

    // When destructing the NonRelocatableVector with non-zero elements
    this->unit_.reset();

    // Then the destructor is called for each element
    EXPECT_EQ(SpecialMemberFunctionRecordingType::destructor_count, kNonZeroNumberElements);
}

TYPED_TEST(NonRelocatableVectorTrivialFixture, CopyConstructingCopiesAllElements)
{
    // Given a NonRelocatableVector which has been filled with elements
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);

    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        score::cpp::ignore = this->unit_->emplace_back(i);
    }

    // When copy constructing a new NonRelocatableVector
    NonRelocatableVector<typename NonRelocatableVectorTrivialFixture<TypeParam>::ElementType,
                         typename NonRelocatableVectorTrivialFixture<TypeParam>::Allocator>
        new_vector{*this->unit_};

    // Then the copied to vector will contain the same elements
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        EXPECT_EQ(new_vector.at(i), i);
    }
}

TYPED_TEST(NonRelocatableVectorNonTrivialFixture, CopyConstructingCopiesAllElements)
{
    // Given a NonRelocatableVector which has been filled with elements
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);

    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        score::cpp::ignore = this->unit_->emplace_back(i, 1.5f);
    }

    // When copy constructing a new NonRelocatableVector
    NonRelocatableVector<typename NonRelocatableVectorNonTrivialFixture<TypeParam>::ElementType,
                         typename NonRelocatableVectorNonTrivialFixture<TypeParam>::Allocator>
        new_vector{*this->unit_};

    // Then the copied to vector will contain the same elements
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        auto& element = new_vector.at(i);
        EXPECT_EQ(element.member_1_, i);
        EXPECT_EQ(element.member_2_, 1.5f);
    }
}

TYPED_TEST(NonRelocatableVectorTriviallyConstructibleDestructibleTypeFixture, CopyConstructingCopiesAllElements)
{
    // Given a NonRelocatableVector which has been filled with elements
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);

    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        auto& element = this->unit_->emplace_back();
        element.i = static_cast<char>(i);
        element.j = i;
    }

    // When copy constructing a new NonRelocatableVector
    NonRelocatableVector<
        typename NonRelocatableVectorTriviallyConstructibleDestructibleTypeFixture<TypeParam>::ElementType,
        typename NonRelocatableVectorTriviallyConstructibleDestructibleTypeFixture<TypeParam>::Allocator>
        new_vector{*this->unit_};

    // Then the copied to vector will contain the same elements
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        auto& element = new_vector.at(i);
        EXPECT_EQ(element.i, static_cast<char>(i));
        EXPECT_EQ(element.j, i);
    }
}

TYPED_TEST(NonRelocatableVectorNonMoveableAndCopyableElementTypeFixture, CannotCopyVectorContainingNonCopyableType)
{
    // Currently, we have no way of checking whether a NonRelocatableVector can be copied when it contains an
    // ElementType which is non-copyable. This is because the type_trait std::is_copy_constructible only checks if a
    // copy constructor is declared, not that it compiles
    // (https://stackoverflow.com/questions/77220469/why-is-an-stdvector-of-noncopyable-elements-copyable-according-to-concepts).
    // If we try to copy such a vector, we will get a compiler error.
    //
    // We leave this test here to make it explicit that we don't have a copy constructor test for
    // NonRelocatableVectorNonMoveableAndCopyableElementTypeFixture.
}

TYPED_TEST(NonRelocatableVectorCopyableAndMoveablePolymorphicAllocatorFixture, CopyConstructingAllocatesBasedOnCapacity)
{
    // Given a NonRelocatableVector which has been filled with less elements than its capacity
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);
    const auto memory_allocated_for_first_vector = this->memory_resource_.GetUserAllocatedBytes();

    const std::size_t number_of_elements_to_emplace = kNonZeroNumberElements - 2U;
    for (std::size_t i = 0; i < number_of_elements_to_emplace; ++i)
    {
        score::cpp::ignore = this->unit_->emplace_back();
    }

    // When copy constructing a new NonRelocatableVector
    NonRelocatableVector<
        typename NonRelocatableVectorCopyableAndMoveablePolymorphicAllocatorFixture<TypeParam>::ElementType,
        typename NonRelocatableVectorCopyableAndMoveablePolymorphicAllocatorFixture<TypeParam>::Allocator>
        new_vector{*this->unit_};

    // Then memory is allocated based on the capacity, not the number of elements emplaced.
    const auto expected_memory_allocated =
        sizeof(typename NonRelocatableVectorCopyableAndMoveablePolymorphicAllocatorFixture<TypeParam>::ElementType) *
        kNonZeroNumberElements;
    const auto memory_allocated_for_second_vector =
        this->memory_resource_.GetUserAllocatedBytes() - memory_allocated_for_first_vector;
    EXPECT_EQ(memory_allocated_for_second_vector, expected_memory_allocated);
}

TYPED_TEST(NonRelocatableVectorCopyableAndMoveablePolymorphicAllocatorFixture, CopyConstructingDoesNotDeallocateMemory)
{
    // Given a NonRelocatableVector which has been filled
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);

    // When copy constructing a new NonRelocatableVector
    NonRelocatableVector<
        typename NonRelocatableVectorCopyableAndMoveablePolymorphicAllocatorFixture<TypeParam>::ElementType,
        typename NonRelocatableVectorCopyableAndMoveablePolymorphicAllocatorFixture<TypeParam>::Allocator>
        new_vector{*this->unit_};

    // Then memory no memory is deallocated
    EXPECT_EQ(this->memory_resource_.GetUserDeAllocatedBytes(), 0U);
}

TYPED_TEST(NonRelocatableVectorTrivialFixture, MoveConstructingMovesAllElements)
{
    // Given a NonRelocatableVector which has been filled with elements
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);

    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        score::cpp::ignore = this->unit_->emplace_back(i);
    }

    // When move constructing a new NonRelocatableVector
    NonRelocatableVector<typename NonRelocatableVectorTrivialFixture<TypeParam>::ElementType,
                         typename NonRelocatableVectorTrivialFixture<TypeParam>::Allocator>
        new_vector{std::move(*this->unit_)};

    // Then the moved to vector will contain the same elements
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        EXPECT_EQ(new_vector.at(i), i);
    }
}

TYPED_TEST(NonRelocatableVectorNonTrivialFixture, MoveConstructingMovesAllElements)
{
    // Given a NonRelocatableVector which has been filled with elements
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);

    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        score::cpp::ignore = this->unit_->emplace_back(i, 1.5f);
    }

    // When move constructing a new NonRelocatableVector
    NonRelocatableVector<typename NonRelocatableVectorNonTrivialFixture<TypeParam>::ElementType,
                         typename NonRelocatableVectorNonTrivialFixture<TypeParam>::Allocator>
        new_vector{std::move(*this->unit_)};

    // Then the moved to vector will contain the same elements
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        auto& element = new_vector.at(i);
        EXPECT_EQ(element.member_1_, i);
        EXPECT_EQ(element.member_2_, 1.5f);
    }
}

TYPED_TEST(NonRelocatableVectorTriviallyConstructibleDestructibleTypeFixture, MoveConstructingMovesAllElements)
{
    // Given a NonRelocatableVector which has been filled with elements
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);

    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        auto& element = this->unit_->emplace_back();
        element.i = static_cast<char>(i);
        element.j = i;
    }

    // When copy constructing a new NonRelocatableVector
    NonRelocatableVector<
        typename NonRelocatableVectorTriviallyConstructibleDestructibleTypeFixture<TypeParam>::ElementType,
        typename NonRelocatableVectorTriviallyConstructibleDestructibleTypeFixture<TypeParam>::Allocator>
        new_vector{std::move(*this->unit_)};

    // Then the moved to vector will contain the same elements
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        auto& element = new_vector.at(i);
        EXPECT_EQ(element.i, static_cast<char>(i));
        EXPECT_EQ(element.j, i);
    }
}

TYPED_TEST(NonRelocatableVectorNonMoveableAndCopyableElementTypeFixture, MoveConstructingMovesAllElements)
{
    // Currently, we have no way of checking whether a NonRelocatableVector can be move constructed when it contains an
    // ElementType which is non-moveable. This is because the type_trait std::is_move_constructible only checks if a
    // move constructor is declared, not that it compiles
    // (https://stackoverflow.com/questions/77220469/why-is-an-stdvector-of-noncopyable-elements-copyable-according-to-concepts).
    // If we try to move such a vector, we will get a compiler error.
    //
    // We leave this test here to make it explicit that we don't have a move constructor test for
    // NonRelocatableVectorNonMoveableAndmoveableElementTypeFixture.
}

TYPED_TEST(NonRelocatableVectorCopyableAndMoveablePolymorphicAllocatorFixture, MoveConstructingDoesNotAllocateNewMemory)
{
    // Given a NonRelocatableVector which has been filled with less elements than its capacity
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);
    const auto memory_allocated_for_first_vector = this->memory_resource_.GetUserAllocatedBytes();

    const std::size_t number_of_elements_to_emplace = kNonZeroNumberElements - 2U;
    for (std::size_t i = 0; i < number_of_elements_to_emplace; ++i)
    {
        score::cpp::ignore = this->unit_->emplace_back();
    }

    // When move constructing a new NonRelocatableVector
    NonRelocatableVector<
        typename NonRelocatableVectorCopyableAndMoveablePolymorphicAllocatorFixture<TypeParam>::ElementType,
        typename NonRelocatableVectorCopyableAndMoveablePolymorphicAllocatorFixture<TypeParam>::Allocator>
        new_vector{std::move(*this->unit_)};

    // Then memory no new memory is allocated
    const auto memory_allocated_for_second_vector =
        this->memory_resource_.GetUserAllocatedBytes() - memory_allocated_for_first_vector;
    EXPECT_EQ(memory_allocated_for_second_vector, 0U);
}

TYPED_TEST(NonRelocatableVectorCopyableAndMoveablePolymorphicAllocatorFixture, MoveConstructingDoesNotDeallocateMemory)
{
    // Given a NonRelocatableVector which has been filled
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);

    // When move constructing a new NonRelocatableVector
    NonRelocatableVector<
        typename NonRelocatableVectorCopyableAndMoveablePolymorphicAllocatorFixture<TypeParam>::ElementType,
        typename NonRelocatableVectorCopyableAndMoveablePolymorphicAllocatorFixture<TypeParam>::Allocator>
        new_vector{std::move(*this->unit_)};

    // Then memory no memory is deallocated
    EXPECT_EQ(this->memory_resource_.GetUserDeAllocatedBytes(), 0U);
}

TYPED_TEST(NonRelocatableVectorTrivialFixture, MoveAssigningMovesAllElements)
{
    // Given a NonRelocatableVector which has been filled with elements
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        score::cpp::ignore = this->unit_->emplace_back(i);
    }

    // and a second NonRelocatableVector which has been filled with different elements
    const auto second_vector_capacity = kNonZeroNumberElements - 2U;
    NonRelocatableVector<typename NonRelocatableVectorTrivialFixture<TypeParam>::ElementType,
                         typename NonRelocatableVectorTrivialFixture<TypeParam>::Allocator>
        new_vector{second_vector_capacity};
    for (std::size_t i = 0; i < second_vector_capacity; ++i)
    {
        score::cpp::ignore = new_vector.emplace_back(2 * i);
    }

    // When move assigning the first vector to the second
    new_vector = std::move(*this->unit_);

    // Then the moved to vector will contain the same elements as the first
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        EXPECT_EQ(new_vector.at(i), i);
    }
}

TYPED_TEST(NonRelocatableVectorNonTrivialFixture, MoveAssigningMovesAllElements)
{
    // Given a NonRelocatableVector which has been filled with elements
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        score::cpp::ignore = this->unit_->emplace_back(i, 1.5f);
    }

    // and a second NonRelocatableVector which has been filled with different elements
    const auto second_vector_capacity = kNonZeroNumberElements - 2U;
    NonRelocatableVector<typename NonRelocatableVectorNonTrivialFixture<TypeParam>::ElementType,
                         typename NonRelocatableVectorNonTrivialFixture<TypeParam>::Allocator>
        new_vector{second_vector_capacity};
    for (std::size_t i = 0; i < second_vector_capacity; ++i)
    {
        score::cpp::ignore = new_vector.emplace_back(2 * i, 3.0f);
    }

    // When move assigning the first vector to the second
    new_vector = std::move(*this->unit_);

    // Then the moved to vector will contain the same elements as the first
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        auto& element = new_vector.at(i);
        EXPECT_EQ(element.member_1_, i);
        EXPECT_EQ(element.member_2_, 1.5f);
    }
}

TYPED_TEST(NonRelocatableVectorTriviallyConstructibleDestructibleTypeFixture, MoveAssigningMovesAllElements)
{
    // Given a NonRelocatableVector which has been filled with elements
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        auto& element = this->unit_->emplace_back();
        element.i = static_cast<char>(i);
        element.j = i;
    }

    // and a second NonRelocatableVector which has been filled with different elements
    const auto second_vector_capacity = kNonZeroNumberElements - 2U;
    NonRelocatableVector<
        typename NonRelocatableVectorTriviallyConstructibleDestructibleTypeFixture<TypeParam>::ElementType,
        typename NonRelocatableVectorTriviallyConstructibleDestructibleTypeFixture<TypeParam>::Allocator>
        new_vector{second_vector_capacity};
    for (std::size_t i = 0; i < second_vector_capacity; ++i)
    {
        auto& element = new_vector.emplace_back();
        element.i = static_cast<char>(2 * i);
        element.j = 2 * i;
    }

    // When move assigning the first vector to the second
    new_vector = std::move(*this->unit_);

    // Then the moved to vector will contain the same elements as the first
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        auto& element = new_vector.at(i);
        EXPECT_EQ(element.i, static_cast<char>(i));
        EXPECT_EQ(element.j, i);
    }
}

TYPED_TEST(NonRelocatableVectorNonMoveableAndCopyableElementTypeFixture, MoveAssigningMovesAllElements)
{
    // Currently, we have no way of checking whether a NonRelocatableVector can be move assigned when it contains an
    // ElementType which is non-moveable. This is because the type_trait std::is_move_assignable only checks if a
    // move constructor is declared, not that it compiles
    // (https://stackoverflow.com/questions/77220469/why-is-an-stdvector-of-noncopyable-elements-copyable-according-to-concepts).
    // If we try to move such a vector, we will get a compiler error.
    //
    // We leave this test here to make it explicit that we don't have a move assignment test for
    // NonRelocatableVectorNonMoveableAndmoveableElementTypeFixture.
}

TYPED_TEST(NonRelocatableVectorCopyableAndMoveablePolymorphicAllocatorFixture, MoveAssigningAllocatesBasedOnCapacity)
{
    // Given a NonRelocatableVector which has been filled with elements
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);

    const std::size_t number_of_elements_to_emplace = kNonZeroNumberElements - 2U;
    for (std::size_t i = 0; i < number_of_elements_to_emplace; ++i)
    {
        score::cpp::ignore = this->unit_->emplace_back();
    }

    // and a second NonRelocatableVector which has been filled with elements
    const auto second_vector_capacity = kNonZeroNumberElements - 2U;
    NonRelocatableVector<
        typename NonRelocatableVectorCopyableAndMoveablePolymorphicAllocatorFixture<TypeParam>::ElementType,
        typename NonRelocatableVectorCopyableAndMoveablePolymorphicAllocatorFixture<TypeParam>::Allocator>
        new_vector{second_vector_capacity};
    for (std::size_t i = 0; i < second_vector_capacity; ++i)
    {
        score::cpp::ignore = new_vector.emplace_back();
    }
    const auto memory_allocated_for_vectors = this->memory_resource_.GetUserAllocatedBytes();

    // When move assigning the first vector to the second
    new_vector = std::move(*this->unit_);

    // Then memory no new memory is allocated
    const auto memory_allocated_for_move_assignment =
        this->memory_resource_.GetUserAllocatedBytes() - memory_allocated_for_vectors;
    EXPECT_EQ(memory_allocated_for_move_assignment, 0U);
}

}  // namespace score::containers
