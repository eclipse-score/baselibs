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
#include "score/containers/test/mockable_allocator.h"
#include "score/containers/test/mockable_pointer_mock_guard.h"

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
#include <vector>

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
        element.j = static_cast<std::uint64_t>(i);
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
        element.j = static_cast<std::uint64_t>(2 * i);
    }

    // When swapping the two vectors
    swap(*this->unit_, new_vector);

    // Then the first vector contains the second vectors elements and viceversa
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        auto& element = new_vector.at(i);
        EXPECT_EQ(element.i, static_cast<char>(i));
        EXPECT_EQ(element.j, static_cast<std::uint64_t>(i));
    }

    for (std::size_t i = 0; i < second_vector_capacity; ++i)
    {
        auto& element = this->unit_->at(i);
        EXPECT_EQ(element.i, static_cast<char>(2 * i));
        EXPECT_EQ(element.j, static_cast<std::uint64_t>(2 * i));
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

class NonRelocatableVectorPointerInteractionFixture : public ::testing::Test
{

  protected:
    using ElementType = std::uint32_t;
    using Allocator = typename test::MockableAllocator<ElementType>;

    NonRelocatableVectorPointerInteractionFixture& GivenANonRelocatableVectorContainingNumberOfElements(
        const std::size_t number_of_elements)
    {
        unit_ = std::make_unique<NonRelocatableVector<ElementType, Allocator>>(number_of_elements);
        for (std::size_t i = 0; i < number_of_elements; ++i)
        {
            score::cpp::ignore = this->unit_->emplace_back(i);
        }
        return *this;
    }

    // Installs a pointer-spy on the vector's element pointer, invokes the given accessor and captures every
    // dereferenced address. Returns the pointer/iterator returned by the accessor together with the captured
    // dereference arguments so that each test can assert their relation.
    template <typename Accessor>
    std::pair<ElementType*, std::vector<ElementType*>> WhenAccessingWhileSpyingOnPointer(Accessor&& access)
    {
        testing::StrictMock<test::PointerSpyMock<ElementType>> pointer_spy;
        std::vector<ElementType*> arrow_args;

        ElementType* result = nullptr;
        {
            test::MockablePointerMockGuard<ElementType> guard{&pointer_spy};

            // expect, that the pointer to the start of the data is advanced to the last element (size - 1) ...
            EXPECT_CALL(pointer_spy, PlusAssign(static_cast<std::ptrdiff_t>(kNonZeroNumberElements - 1)));
            // ... and two pointer dereferences take place, capturing which addresses are dereferenced.
            EXPECT_CALL(pointer_spy, Arrow(testing::_))
                .Times(2)
                .WillRepeatedly(testing::Invoke([&arrow_args](ElementType* ptr) { arrow_args.push_back(ptr); }));

            // when calling the accessor on the NonRelocatableVector
            result = access(*unit_);
            EXPECT_NE(result, nullptr);
        }
        // Guard destructor clears the spy - destruction proceeds with normal pointer behavior!

        return {result, arrow_args};
    }

    std::unique_ptr<NonRelocatableVector<ElementType, Allocator>> unit_{nullptr};
};

using testing::_;

TEST_F(NonRelocatableVectorPointerInteractionFixture, DataDereferencesBeginAndEnd)
{
    // Given a NonRelocatableVector which has been filled with elements
    GivenANonRelocatableVectorContainingNumberOfElements(kNonZeroNumberElements);

    // when calling data() while spying on the element pointer
    const auto [data_ptr, arrow_args] =
        WhenAccessingWhileSpyingOnPointer([](auto& vector) { return vector.data(); });

    // Then two pointer-dereferences happened
    ASSERT_EQ(arrow_args.size(), 2U);
    // where the 1st deref comes from GetLastElement (pointer advanced to last element)
    EXPECT_EQ(arrow_args[0], data_ptr + (kNonZeroNumberElements - 1));
    // and the 2nd deref call comes from GetFirstElement (pointer to first element)
    EXPECT_EQ(arrow_args[1], data_ptr);
}

TEST_F(NonRelocatableVectorPointerInteractionFixture, BeginDereferencesBeginAndEnd)
{
    // Given a NonRelocatableVector which has been filled with elements
    GivenANonRelocatableVectorContainingNumberOfElements(kNonZeroNumberElements);

    // when calling begin() while spying on the element pointer
    const auto [begin_it, arrow_args] =
        WhenAccessingWhileSpyingOnPointer([](auto& vector) { return vector.begin(); });

    // Then two pointer-dereferences happened
    ASSERT_EQ(arrow_args.size(), 2U);
    // where the 1st deref comes from GetLastElement (pointer advanced to last element)
    EXPECT_EQ(arrow_args[0], begin_it + (kNonZeroNumberElements - 1));
    // and the 2nd deref call comes from GetFirstElement (pointer to first element)
    EXPECT_EQ(arrow_args[1], begin_it);
}

TEST_F(NonRelocatableVectorPointerInteractionFixture, EndDereferencesBeginAndEnd)
{
    // Given a NonRelocatableVector which has been filled with elements
    GivenANonRelocatableVectorContainingNumberOfElements(kNonZeroNumberElements);

    // when calling end() while spying on the element pointer
    const auto [end_it, arrow_args] =
        WhenAccessingWhileSpyingOnPointer([](auto& vector) { return vector.end(); });

    // Then two pointer-dereferences happened
    ASSERT_EQ(arrow_args.size(), 2U);
    // where the 1st deref comes from GetFirstElement (pointer to first element)
    EXPECT_EQ(arrow_args[0], end_it - kNonZeroNumberElements);
    // and the 2nd deref call comes from GetLastElement (pointer advanced to last element)
    EXPECT_EQ(arrow_args[1], end_it - 1);
}

}  // namespace score::containers
