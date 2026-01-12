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

TYPED_TEST_SUITE(NonRelocatableVectorFixture, AllAllocatorTypes, );

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

TYPED_TEST(NonRelocatableVectorFixture, EmplaceBackUpdatesSize)
{
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);

    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        // When calling emplace_back
        score::cpp::ignore = this->unit_->emplace_back();

        // Then the size is incremented
        EXPECT_EQ(this->unit_->size(), i + 1U);
    }
}

TYPED_TEST(NonRelocatableVectorTrivialFixture, EmplaceBackAllocatesAndReturnsElement)
{
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);

    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        // When calling emplace_back
        auto& element = this->unit_->emplace_back(i);

        // Then the returned element is equal to the element that was emplaced
        const auto expected_element = static_cast<TrivialType>(i);
        EXPECT_EQ(element, expected_element);

        // and that element is allocated in the NonRelocatableVector
        EXPECT_EQ(&this->unit_->at(i), &element);
    }
}

TYPED_TEST(NonRelocatableVectorNonTrivialFixture, EmplaceBackAllocatesAndReturnsElement)
{
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);

    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        // When calling emplace_back
        auto& element = this->unit_->emplace_back(i, 1.5f);

        // Then the returned element is equal to the element that was emplaced
        const NonTrivialType expected_element{static_cast<std::uint32_t>(i), 1.5f};
        EXPECT_EQ(element.member_1_, expected_element.member_1_);
        EXPECT_EQ(element.member_2_, expected_element.member_2_);

        // and that element is allocated in the NonRelocatableVector
        EXPECT_EQ(&this->unit_->at(i), &element);
    }
}

TYPED_TEST(NonRelocatableVectorTriviallyConstructibleDestructibleTypeFixture, EmplaceBackAllocatesAndReturnsElement)
{
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);

    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        // When calling emplace_back
        auto& element = this->unit_->emplace_back();

        // Then the returned element is equal to the element that was emplaced
        const TriviallyConstructibleDestructibleType expected_element{};
        EXPECT_EQ(element.i, expected_element.i);
        EXPECT_EQ(element.j, expected_element.j);

        // and that element is allocated in the NonRelocatableVector
        EXPECT_EQ(&this->unit_->at(i), &element);
    }
}

TYPED_TEST(NonRelocatableVectorNonMoveableAndCopyableElementTypeFixture, EmplaceBackAllocatesAndReturnsElement)
{
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);

    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        // When calling emplace_back
        auto& element = this->unit_->emplace_back();

        // Then the returned element is equal to the element that was emplaced
        const NonMoveableAndCopyableElementType expected_element{};
        EXPECT_EQ(element.i_, expected_element.i_);

        // and that element is allocated in the NonRelocatableVector
        EXPECT_EQ(&this->unit_->at(i), &element);
    }
}

TYPED_TEST(NonRelocatableVectorFixture, CallingEmplaceBackMoreTimesThanWereReservedTerminates)
{
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);

    // and given that emplace_back was called size() - 1 times
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        score::cpp::ignore = this->unit_->emplace_back();
    }

    // When calling emplace_back again
    // Then the program terminates
    SCORE_LANGUAGE_FUTURECPP_ASSERT_CONTRACT_VIOLATED(score::cpp::ignore = this->unit_->emplace_back());
}

TYPED_TEST(NonRelocatableVectorPolymorphicAllocatorFixture, EmplaceBackDoesNotAllocate)
{
    // Given a NonRelocatableVector which has allocated n bytes on construction
    this->GivenANonRelocatableVectorConstructedWithNumberOfElements(kNonZeroNumberElements);
    const auto allocated_bytes_after_construction = this->memory_resource_.GetUserAllocatedBytes();

    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        // When calling emplace back
        score::cpp::ignore = this->unit_->emplace_back();

        // Then no additional memory is allocated
        EXPECT_EQ(this->memory_resource_.GetUserAllocatedBytes(), allocated_bytes_after_construction);
    }
}

}  // namespace score::containers
