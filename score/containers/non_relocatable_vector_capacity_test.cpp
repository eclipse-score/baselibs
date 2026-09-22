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

#include "score/containers/test/fake_memory_resource.h"
#include "score/containers/test/fancy_pointer_allocator.h"

#include <score/assert_support.hpp>
#include <score/utility.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstddef>

namespace score::containers
{

using namespace score::containers::test_types;

constexpr std::size_t kNonZeroNumberElements{10U};

template <typename ContainerTestTypes>
class NonRelocatableVectorFixture : public ::testing::Test
{
    void TearDown() override
    {
        NonMoveableAndCopyableElementType::ResetDestructorCount();
    }

  protected:
    using ElementType = typename ContainerTestTypes::ElementType;
    using Allocator = typename ContainerTestTypes::Allocator;
    using Vector = NonRelocatableVector<ElementType, Allocator>;

    Vector MakeVector(const std::size_t number_of_elements)
    {
        return Vector(number_of_elements, GetAllocator<ElementType, Allocator>(memory_resource_));
    }

    score::containers::test::FakeMemoryResource memory_resource_{};
};

TYPED_TEST_SUITE(NonRelocatableVectorFixture, AllAllocatorTypes, );

template <typename T>
using NonRelocatableVectorTrivialFixture = NonRelocatableVectorFixture<T>;
TYPED_TEST_SUITE(NonRelocatableVectorTrivialFixture, TrivialAllocatorTypes, );

template <typename T>
using NonRelocatableVectorPolymorphicAllocatorFixture = NonRelocatableVectorFixture<T>;
TYPED_TEST_SUITE(NonRelocatableVectorPolymorphicAllocatorFixture, PolymorphicAllocatorTypes, );

class NonRelocatableVectorSpecialMemberFunctionRecorderFixture
    : public NonRelocatableVectorFixture<
          ContainerTestTypes<SpecialMemberFunctionRecordingType, test::FancyPointerAllocator>>
{
  protected:
    void SetUp() override
    {
        SpecialMemberFunctionRecordingType::Reset();
    }
    void TearDown() override
    {
        SpecialMemberFunctionRecordingType::Reset();
    }
};

TYPED_TEST(NonRelocatableVectorFixture, EmptyIsTrueWhenConstructedWithZeroCapacity)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__non_relocatable_vector");
    this->RecordProperty("Description", "Check that empty() returns true for a vector constructed with zero capacity.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "boundary-values");

    // When constructing a NonRelocatableVector with zero capacity
    auto vector = this->MakeVector(0U);

    // Then the vector is empty
    EXPECT_TRUE(vector.empty());
}

TYPED_TEST(NonRelocatableVectorFixture, EmptyIsTrueWhenConstructedWithNonZeroCapacity)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__non_relocatable_vector");
    this->RecordProperty(
        "Description",
        "Check that empty() returns true for a vector constructed with non-zero capacity but no emplaced elements.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "requirements-analysis");

    // When constructing a NonRelocatableVector with a non-zero capacity but no emplaced elements
    auto vector = this->MakeVector(kNonZeroNumberElements);

    // Then the vector is still empty
    EXPECT_TRUE(vector.empty());
}

TYPED_TEST(NonRelocatableVectorFixture, EmptyIsFalseAfterEmplaceBack)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__non_relocatable_vector");
    this->RecordProperty("Description", "Check that empty() returns false after an element has been emplaced.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "requirements-analysis");

    auto vector = this->MakeVector(kNonZeroNumberElements);

    // When emplacing an element
    score::cpp::ignore = vector.emplace_back();

    // Then the vector is no longer empty
    EXPECT_FALSE(vector.empty());
}

TYPED_TEST(NonRelocatableVectorFixture, EmptyIsTrueAgainAfterPopBackEmptiesSingleElementVector)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__non_relocatable_vector");
    this->RecordProperty("Description",
                         "Check that empty() returns true again after pop_back() removes the only element.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "boundary-values");

    auto vector = this->MakeVector(1U);
    score::cpp::ignore = vector.emplace_back();

    // When popping the only element
    vector.pop_back();

    // Then the vector is empty again
    EXPECT_TRUE(vector.empty());
}

TYPED_TEST(NonRelocatableVectorFixture, PopBackDecrementsSize)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__non_relocatable_vector");
    this->RecordProperty("Description", "Check that pop_back() reduces size() by one.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "requirements-analysis");

    auto vector = this->MakeVector(kNonZeroNumberElements);
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        score::cpp::ignore = vector.emplace_back();
    }

    // When popping one element
    vector.pop_back();

    // Then the size is reduced by one
    EXPECT_EQ(vector.size(), kNonZeroNumberElements - 1U);
}

TYPED_TEST(NonRelocatableVectorTrivialFixture, PopBackLeavesRemainingElementsUnchanged)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__non_relocatable_vector");
    this->RecordProperty("Description", "Check that pop_back() does not alter the values of the remaining elements.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "requirements-analysis");

    auto vector = this->MakeVector(kNonZeroNumberElements);
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        score::cpp::ignore = vector.emplace_back(i);
    }

    // When popping the last element
    vector.pop_back();

    // Then every remaining element still holds its original value
    for (std::size_t i = 0; i < kNonZeroNumberElements - 1U; ++i)
    {
        EXPECT_EQ(vector.at(i), i);
    }
}

TEST_F(NonRelocatableVectorSpecialMemberFunctionRecorderFixture, PopBackDestroysRemovedElement)
{
    RecordProperty("PartiallyVerifies", "comp_req__containers__non_relocatable_vector");
    RecordProperty("Description", "Check that pop_back() calls the destructor of the removed element exactly once.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    auto vector = this->MakeVector(kNonZeroNumberElements);
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        vector.emplace_back();
    }

    // When popping one element
    vector.pop_back();

    // Then exactly one destructor call happened
    EXPECT_EQ(SpecialMemberFunctionRecordingType::destructor_count, 1U);
}

TYPED_TEST(NonRelocatableVectorFixture, PopBackOnEmptyVectorTerminates)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__non_relocatable_vector");
    this->RecordProperty("Description", "Check that pop_back() on an empty vector terminates the program.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "error-guessing");

    auto vector = this->MakeVector(kNonZeroNumberElements);

    // When calling pop_back on an empty vector
    // Then the program terminates
    SCORE_LANGUAGE_FUTURECPP_ASSERT_CONTRACT_VIOLATED(vector.pop_back());
}

TYPED_TEST(NonRelocatableVectorFixture, PopBackDoesNotChangeCapacity)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__non_relocatable_vector");
    this->RecordProperty("Description", "Check that pop_back() does not change capacity().");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "requirements-analysis");

    auto vector = this->MakeVector(kNonZeroNumberElements);
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        score::cpp::ignore = vector.emplace_back();
    }

    // When popping one element
    vector.pop_back();

    // Then the capacity is unaffected
    EXPECT_EQ(vector.capacity(), kNonZeroNumberElements);
}

TYPED_TEST(NonRelocatableVectorPolymorphicAllocatorFixture, PopBackDoesNotDeallocateOrAllocateMemory)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__non_relocatable_vector");
    this->RecordProperty("Description", "Check that pop_back() does not allocate or deallocate memory.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "requirements-analysis");

    auto vector = this->MakeVector(kNonZeroNumberElements);
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        score::cpp::ignore = vector.emplace_back();
    }
    const auto allocated_bytes_before_pop_back = this->memory_resource_.GetUserAllocatedBytes();

    // When popping one element
    vector.pop_back();

    // Then no memory is allocated or deallocated
    EXPECT_EQ(this->memory_resource_.GetUserAllocatedBytes(), allocated_bytes_before_pop_back);
    EXPECT_EQ(this->memory_resource_.GetUserDeallocatedBytes(), 0U);
}

TYPED_TEST(NonRelocatableVectorFixture, EmplaceBackAfterPopBackReusesFreedSlot)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__non_relocatable_vector");
    this->RecordProperty(
        "Description", "Check that emplace_back() can reuse the slot pop_back() freed, up to the original capacity().");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "requirements-analysis");

    auto vector = this->MakeVector(kNonZeroNumberElements);
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        score::cpp::ignore = vector.emplace_back();
    }
    vector.pop_back();

    // When emplacing again into the slot pop_back freed
    score::cpp::ignore = vector.emplace_back();

    // Then the vector holds capacity() elements again
    EXPECT_EQ(vector.size(), kNonZeroNumberElements);
}

TYPED_TEST(NonRelocatableVectorFixture, PoppingAllElementsOneByOneEmptiesTheVector)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__non_relocatable_vector");
    this->RecordProperty(
        "Description",
        "Check that repeatedly calling pop_back() empties a multi-element vector one element at a time.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "boundary-values");

    auto vector = this->MakeVector(kNonZeroNumberElements);
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        score::cpp::ignore = vector.emplace_back();
    }

    // When popping every element but the last
    for (std::size_t i = 0; i < kNonZeroNumberElements - 1U; ++i)
    {
        vector.pop_back();
        // Then the vector is not yet empty
        EXPECT_FALSE(vector.empty());
    }

    // When popping the last remaining element
    vector.pop_back();

    // Then the vector is empty
    EXPECT_TRUE(vector.empty());
}

TYPED_TEST(NonRelocatableVectorFixture, ClearSetsSizeToZero)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__non_relocatable_vector");
    this->RecordProperty("Description", "Check that clear() reduces size() to zero.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "requirements-analysis");

    auto vector = this->MakeVector(kNonZeroNumberElements);
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        score::cpp::ignore = vector.emplace_back();
    }

    // When clearing the vector
    vector.clear();

    // Then the size is zero
    EXPECT_EQ(vector.size(), 0U);
    EXPECT_TRUE(vector.empty());
}

TEST_F(NonRelocatableVectorSpecialMemberFunctionRecorderFixture, ClearDestroysAllElements)
{
    RecordProperty("PartiallyVerifies", "comp_req__containers__non_relocatable_vector");
    RecordProperty("Description", "Check that clear() calls the destructor of every element exactly once.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    auto vector = this->MakeVector(kNonZeroNumberElements);
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        vector.emplace_back();
    }

    // When clearing the vector
    vector.clear();

    // Then every emplaced element's destructor was called exactly once
    EXPECT_EQ(SpecialMemberFunctionRecordingType::destructor_count, kNonZeroNumberElements);
}

TEST_F(NonRelocatableVectorSpecialMemberFunctionRecorderFixture, ClearOnEmptyVectorIsANoOp)
{
    RecordProperty("PartiallyVerifies", "comp_req__containers__non_relocatable_vector");
    RecordProperty("Description",
                   "Check that calling clear() on an already-empty vector performs no further destructor calls.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    auto vector = this->MakeVector(kNonZeroNumberElements);
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        vector.emplace_back();
    }
    vector.clear();
    const auto destructor_count_after_first_clear = SpecialMemberFunctionRecordingType::destructor_count;

    // When clearing the already-empty vector again
    vector.clear();

    // Then no further destructor calls happen and the size stays zero
    EXPECT_EQ(SpecialMemberFunctionRecordingType::destructor_count, destructor_count_after_first_clear);
    EXPECT_EQ(vector.size(), 0U);
}

TYPED_TEST(NonRelocatableVectorFixture, ClearDoesNotChangeCapacity)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__non_relocatable_vector");
    this->RecordProperty("Description", "Check that clear() does not change capacity().");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "requirements-analysis");

    auto vector = this->MakeVector(kNonZeroNumberElements);
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        score::cpp::ignore = vector.emplace_back();
    }

    // When clearing the vector
    vector.clear();

    // Then the capacity is unaffected
    EXPECT_EQ(vector.capacity(), kNonZeroNumberElements);
}

TYPED_TEST(NonRelocatableVectorPolymorphicAllocatorFixture, ClearDoesNotDeallocateOrAllocateMemory)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__non_relocatable_vector");
    this->RecordProperty("Description", "Check that clear() does not allocate or deallocate memory.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "requirements-analysis");

    auto vector = this->MakeVector(kNonZeroNumberElements);
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        score::cpp::ignore = vector.emplace_back();
    }
    const auto allocated_bytes_before_clear = this->memory_resource_.GetUserAllocatedBytes();

    // When clearing the vector
    vector.clear();

    // Then no memory is allocated or deallocated
    EXPECT_EQ(this->memory_resource_.GetUserAllocatedBytes(), allocated_bytes_before_clear);
    EXPECT_EQ(this->memory_resource_.GetUserDeallocatedBytes(), 0U);
}

TYPED_TEST(NonRelocatableVectorFixture, EmplaceBackAfterClearRefillsUpToCapacity)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__non_relocatable_vector");
    this->RecordProperty("Description",
                         "Check that emplace_back() can refill a cleared vector up to its original capacity().");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "requirements-analysis");

    auto vector = this->MakeVector(kNonZeroNumberElements);
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        score::cpp::ignore = vector.emplace_back();
    }
    vector.clear();

    // When refilling the vector up to its original capacity
    for (std::size_t i = 0; i < kNonZeroNumberElements; ++i)
    {
        score::cpp::ignore = vector.emplace_back();
    }

    // Then the vector holds capacity() elements again
    EXPECT_EQ(vector.size(), kNonZeroNumberElements);
}

}  // namespace score::containers
