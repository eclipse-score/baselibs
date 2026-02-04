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

#include "score/memory/shared/fake/my_memory_resource.h"
#include "score/memory/shared/polymorphic_offset_ptr_allocator.h"

#include <score/assert_support.hpp>
#include <score/utility.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>

namespace score::containers
{

using namespace score::memory::shared;
using namespace score::containers::test_types;

constexpr std::size_t kNonZeroNumberElements{10U};

template <typename ContainerTestTypes>
class NonRelocatableVectorFixture : public ::testing::Test
{
    void SetUp() override {}
    void TearDown() override
    {
        NonMoveableAndCopyableElementType::ResetDestructorCount();
    }

  protected:
    using ElementType = typename ContainerTestTypes::ElementType;
    using Allocator = typename ContainerTestTypes::Allocator;

    NonRelocatableVectorFixture& GivenANonRelocatableVectorConstructedWithNumberOfElements(
        const std::size_t number_of_elements)
    {
        unit_ = std::make_unique<NonRelocatableVector<ElementType, Allocator>>(
            number_of_elements, GetAllocator<ElementType, Allocator>(memory_resource_));
        return *this;
    }

    score::memory::shared::test::MyMemoryResource memory_resource_{};
    std::unique_ptr<NonRelocatableVector<ElementType, Allocator>> unit_{nullptr};
};

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

template <typename T>
using NonRelocatableVectorPolymorphicAllocatorFixture = NonRelocatableVectorFixture<T>;
TYPED_TEST_SUITE(NonRelocatableVectorPolymorphicAllocatorFixture, PolymorphicAllocatorTypes, );

TYPED_TEST(NonRelocatableVectorTrivialFixture, SwapSwapsAllElements)
{
    // Given a NonRelocatableVector which has been filled with elements
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        score::cpp::ignore = this->unit_->emplace_back(i);
    }

    // and a second NonRelocatableVector which has been filled with elements
    const auto second_vector_capacity = kNonZeroNumberElements - 2U;
    NonRelocatableVector<typename NonRelocatableVectorTrivialFixture<TypeParam>::ElementType,
                         typename NonRelocatableVectorTrivialFixture<TypeParam>::Allocator>
        new_vector{second_vector_capacity};
    for (std::size_t i = 0; i < second_vector_capacity; ++i)
    {
        score::cpp::ignore = new_vector.emplace_back(2 * i);
    }

    // When swapping the two vectors
    swap(*this->unit_, new_vector);

    // Then the first vector contains the second vectors elements and viceversa
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        EXPECT_EQ(new_vector.at(i), i);
    }

    for (std::size_t i = 0; i < second_vector_capacity; ++i)
    {
        EXPECT_EQ(this->unit_->at(i), 2 * i);
    }
}

TYPED_TEST(NonRelocatableVectorNonTrivialFixture, SwapSwapsAllElements)
{
    // Given a NonRelocatableVector which has been filled with elements
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        score::cpp::ignore = this->unit_->emplace_back(i, 1.5f);
    }

    // and a second NonRelocatableVector which has been filled with elements
    const auto second_vector_capacity = kNonZeroNumberElements - 2U;
    NonRelocatableVector<typename NonRelocatableVectorNonTrivialFixture<TypeParam>::ElementType,
                         typename NonRelocatableVectorNonTrivialFixture<TypeParam>::Allocator>
        new_vector{second_vector_capacity};
    for (std::size_t i = 0; i < second_vector_capacity; ++i)
    {
        score::cpp::ignore = new_vector.emplace_back(2 * i, 3.0f);
    }

    // When swapping the two vectors
    swap(*this->unit_, new_vector);

    // Then the first vector contains the second vectors elements and viceversa
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        auto& element = new_vector.at(i);
        EXPECT_EQ(element.member_1_, i);
        EXPECT_EQ(element.member_2_, 1.5f);
    }

    for (std::size_t i = 0; i < second_vector_capacity; ++i)
    {
        auto& element = this->unit_->at(i);
        EXPECT_EQ(element.member_1_, 2 * i);
        EXPECT_EQ(element.member_2_, 3.0f);
    }
}

TYPED_TEST(NonRelocatableVectorTriviallyConstructibleDestructibleTypeFixture, SwapSwapsAllElements)
{
    // Given a NonRelocatableVector which has been filled with elements
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        auto& element = this->unit_->emplace_back();
        element.i = static_cast<char>(i);
        element.j = i;
    }

    // and a second NonRelocatableVector which has been filled with elements
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

    // When swapping the two vectors
    swap(*this->unit_, new_vector);

    // Then the first vector contains the second vectors elements and viceversa
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        auto& element = new_vector.at(i);
        EXPECT_EQ(element.i, static_cast<char>(i));
        EXPECT_EQ(element.j, i);
    }

    for (std::size_t i = 0; i < second_vector_capacity; ++i)
    {
        auto& element = this->unit_->at(i);
        EXPECT_EQ(element.i, static_cast<char>(2 * i));
        EXPECT_EQ(element.j, 2 * i);
    }
}

TYPED_TEST(NonRelocatableVectorNonMoveableAndCopyableElementTypeFixture, SwapSwapsAllElements)
{
    // Given a NonRelocatableVector which has been filled with elements
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        auto& element = this->unit_->emplace_back();
        element.i_ = static_cast<int>(i);
    }

    // and a second NonRelocatableVector which has been filled with elements
    const auto second_vector_capacity = kNonZeroNumberElements - 2U;
    NonRelocatableVector<typename NonRelocatableVectorNonMoveableAndCopyableElementTypeFixture<TypeParam>::ElementType,
                         typename NonRelocatableVectorNonMoveableAndCopyableElementTypeFixture<TypeParam>::Allocator>
        new_vector{second_vector_capacity};
    for (std::size_t i = 0; i < second_vector_capacity; ++i)
    {
        auto& element = new_vector.emplace_back();
        element.i_ = static_cast<int>(2 * i);
    }

    // When swapping the two vectors
    swap(*this->unit_, new_vector);

    // Then the first vector contains the second vectors elements and viceversa
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        auto& element = new_vector.at(i);
        EXPECT_EQ(element.i_, static_cast<int>(i));
    }

    for (std::size_t i = 0; i < second_vector_capacity; ++i)
    {
        auto& element = this->unit_->at(i);
        EXPECT_EQ(element.i_, static_cast<int>(2 * i));
    }
}

}  // namespace score::containers
