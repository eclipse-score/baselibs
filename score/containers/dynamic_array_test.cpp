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

#include "score/containers/test/fake_memory_resource.h"
#include "score/containers/test/fancy_pointer_allocator.h"

#include <score/assert_support.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstddef>
#include <memory>

using namespace score::containers;

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

    score::containers::test::FakeMemoryResource memory_resource_{};
};

using AllocatorTypes = ::testing::Types<std::allocator<TrivialType>, test::FancyPointerAllocator<TrivialType>>;
TYPED_TEST_SUITE(DynamicArrayTestFixture, AllocatorTypes, );

TYPED_TEST(DynamicArrayTestFixture, CanConstructWithTrivialType)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty(
        "Description",
        "Check that constructing a DynamicArray of trivial-type elements yields the requested size with "
        "value-initialized elements.");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Description", "Check that constructing a DynamicArray with size zero yields size() == 0.");

    DynamicArray<TrivialType, TypeParam> unit{kEmptyArraySize,
                                              GetAllocator<TrivialType, TypeParam>(this->memory_resource_)};
    EXPECT_EQ(unit.size(), kEmptyArraySize);
}
TYPED_TEST(DynamicArrayTestFixture, ConstructNonTrivial)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty("Description",
                         "Check that constructing a DynamicArray of non-trivial-type elements value-initializes each "
                         "element.");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty("Description",
                         "Check that copy-constructing a DynamicArray of trivial elements duplicates size and element "
                         "values.");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty("Description", "Check that copy construction duplicates non-trivial element values.");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty("Description",
                         "Check that constructing with an explicit default value initializes every element to that "
                         "value.");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty("Description",
                         "Check that move-constructing transfers size and element values from the source.");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty("Description",
                         "Check that move-construction transfers non-trivial element values without invoking element "
                         "destructors.");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty("Description",
                         "Check that move-assignment replaces size and element values with those of the moved-from "
                         "array.");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty("Description",
                         "Check that move-assignment of non-trivial elements transfers values and destroys only the "
                         "previously held elements exactly once.");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "error-guessing");
    this->RecordProperty("Description",
                         "Check that self-move-assignment leaves the array's size and element values unchanged.");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty("Description", "Check that elements can be written via at() and read back correctly.");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty("Description",
                         "Check that a DynamicArray can hold an element type that is neither movable nor copyable.");

    auto non_trivial_type_alloc = this->template getTypeSpecificAllocator<NonMoveableAndCopyableElementType>();

    DynamicArray<NonMoveableAndCopyableElementType, decltype(non_trivial_type_alloc)> unit{kNonEmptyArraySize,
                                                                                           non_trivial_type_alloc};
    // expect, that the unit has the expected size.
    EXPECT_EQ(unit.size(), kNonEmptyArraySize);
}

TYPED_TEST(DynamicArrayTestFixture, DestructorOfNonTrivialTypesCalled)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty("Description",
                         "Check that destroying a DynamicArray invokes the destructor of every non-trivial element "
                         "exactly once.");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty(
        "Description",
        "Check construction with a trivially-constructible/destructible element type value-initializes "
        "elements.");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "fault-injection");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Description",
                         "Check that constructing a DynamicArray whose requested size overflows allocation capacity "
                         "throws or terminates instead of succeeding silently.");

    constexpr std::size_t array_size_exceeding_limit{std::numeric_limits<std::size_t>::max()};

    const auto initialise_dynamic_array = [this] {
        DynamicArray<TrivialType, TypeParam> unit{array_size_exceeding_limit,
                                                  GetAllocator<TrivialType, TypeParam>(this->memory_resource_)};
    };

    if constexpr (std::is_same_v<TypeParam, std::allocator<TrivialType>>)
    {
        // Since a std::exception is thrown by std::allocator_traits<Allocator>::allocate(), rather than by an AMP
        // assertion / precondition, we capture this using the gtest framework instead of
        // SCORE_LANGUAGE_FUTURECPP_ASSERT_CONTRACT_VIOLATED.
        EXPECT_THROW(initialise_dynamic_array(), std::exception);
    }
    else
    {
        // FancyPointerAllocator, mirroring PolymorphicOffsetPtrAllocator, detects the overflow via an AMP
        // precondition rather than a thrown std::exception, so we must use
        // SCORE_LANGUAGE_FUTURECPP_ASSERT_CONTRACT_VIOLATED here.
        SCORE_LANGUAGE_FUTURECPP_ASSERT_CONTRACT_VIOLATED(initialise_dynamic_array());
    }
}

TYPED_TEST(DynamicArrayTestFixture, AccessingConstRefArrayOutOfBoundsTerminates)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "fault-injection");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Description",
                         "Check that accessing a const element one past the last valid index terminates via contract "
                         "violation.");

    DynamicArray<TrivialType> unit(kNonEmptyArraySize);

    const auto access_const_ref_out_of_bounds = [&unit]() {
        const auto& element = unit.at(kNonEmptyArraySize);
        (void)element;
    };

    SCORE_LANGUAGE_FUTURECPP_ASSERT_CONTRACT_VIOLATED(access_const_ref_out_of_bounds());
}

TYPED_TEST(DynamicArrayTestFixture, IteratingTrivialType)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty("Description", "Check that begin()/end() iteration visits every trivial element in order.");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty(
        "Description",
        "Check that begin()/end() iteration visits and allows mutation of every non-trivial element in "
        "order.");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty("Description",
                         "Check that const begin()/end() iteration exposes read-only access to non-trivial elements.");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty("Description",
                         "Check that cbegin()/cend() iteration visits every non-trivial element exactly once.");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty("Description", "Check that operator[] allows writing element values at a given index.");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty("Description", "Check that operator[] allows reading back previously written element values.");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty("Description", "Check that the const operator[] returns previously written element values.");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty("Description", "Check that data() returns a pointer to the first element.");

    DynamicArray<TrivialType> unit{kNonEmptyArraySize};
    EXPECT_EQ(unit.data(), &unit.at(0));
}

TYPED_TEST(DynamicArrayTestFixture, ConstDataShouldReturnPointerToFirstElement)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty("Description", "Check that the const data() returns a pointer to the first element.");

    const DynamicArray<TrivialType> unit{kNonEmptyArraySize};
    EXPECT_EQ(unit.data(), &unit.at(0));
}

TYPED_TEST(DynamicArrayTestFixture, BeginIsEqualToEndWhenArrayIsEmpty)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Description", "Check that begin() equals end() for an empty DynamicArray.");

    // Given an empty DynamicArray
    const DynamicArray<TrivialType> unit{0U};

    // When calling begin()
    const auto start_iterator = unit.begin();

    // Then the result is equal to end()
    EXPECT_EQ(start_iterator, unit.end());
}

TYPED_TEST(DynamicArrayTestFixture, CBeginIsEqualToCEndWhenArrayIsEmpty)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Description", "Check that cbegin() equals cend() for an empty DynamicArray.");

    // Given an empty DynamicArray
    const DynamicArray<TrivialType> unit{0U};

    // When calling cbegin()
    const auto start_iterator = unit.cbegin();

    // Then the result is equal to cend()
    EXPECT_EQ(start_iterator, unit.cend());
}

TYPED_TEST(DynamicArrayTestFixture, DataReturnsNullptrWhenArrayIsEmpty)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Description", "Check that data() returns nullptr for an empty DynamicArray.");

    // Given an empty DynamicArray
    const DynamicArray<TrivialType> unit{0U};

    // When calling data()
    const auto data_pointer = unit.data();

    // Then the result is a nullptr
    EXPECT_EQ(data_pointer, nullptr);
}

TYPED_TEST(DynamicArrayTestFixture, SizeReturnsZeroWhenArrayIsEmpty)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Description", "Check that size() is 0 for a DynamicArray constructed with zero elements.");

    // Given an empty DynamicArray
    const DynamicArray<TrivialType> unit{0U};

    // When calling size
    const auto array_size = unit.size();

    // Then the result is 0
    EXPECT_EQ(array_size, 0U);
}

TYPED_TEST(DynamicArrayTestFixture, SizeReturnsZeroWhenArrayIsEmptyWithValue)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Description",
                         "Check that size() is 0 for a zero-size DynamicArray constructed with an explicit initial "
                         "value.");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "fault-injection");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Description",
                         "Check that at() on an empty DynamicArray terminates via contract violation instead of "
                         "returning.");

    // Given an empty DynamicArray
    const DynamicArray<TrivialType> unit{0U};

    // When accessing an element with at
    // Then the program terminates
    SCORE_LANGUAGE_FUTURECPP_ASSERT_CONTRACT_VIOLATED(score::cpp::ignore = unit.at(0));
}

TYPED_TEST(DynamicArrayTestFixture, AccessingElementWithIndexOperatorWhenArrayIsEmptyTerminates)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "fault-injection");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Description",
                         "Check that operator[] on an empty DynamicArray terminates via contract violation instead of "
                         "returning.");

    // Given an empty DynamicArray
    const DynamicArray<TrivialType> unit{0U};

    // When accessing an element with operator[]
    // Then the program terminates
    SCORE_LANGUAGE_FUTURECPP_ASSERT_CONTRACT_VIOLATED(score::cpp::ignore = unit[0]);
}

TYPED_TEST(DynamicArrayTestFixture, IteratingOverEmptyArrayIteratesZeroTimes)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Description", "Check that iterating an empty DynamicArray performs zero loop iterations.");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__deterministic_behavior");
    this->RecordProperty("TestType", "resource-usage");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Description",
                         "Check under external memory-checking tooling that constructing and destroying an empty "
                         "DynamicArray of a non-trivial element type does not leak memory; this test has no gtest "
                         "assertion and relies on valgrind/ASan.");

    std::size_t array_size{0};
    DynamicArray<NonTrivialType> da(array_size);
    EXPECT_TRUE(true);
}

TEST(EmptyDynamicArrayOfTrivialElementTypeMemoryTest, TestNeverFailsButMemcheckDoesIfEmptyArrayIsNotCleanedUpCorrectly)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__deterministic_behavior");
    this->RecordProperty("TestType", "resource-usage");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty(
        "Description",
        "Check under external memory-checking tooling that constructing and destroying an empty "
        "DynamicArray of a trivial element type does not leak memory; this test has no gtest assertion "
        "and relies on valgrind/ASan.");

    std::size_t array_size{0};
    DynamicArray<TrivialType> da(array_size);
    EXPECT_TRUE(true);
}

// This test is to validates the fix for the memory leak issue where copying zero-size arrays would call allocate()
// but leave dynamic_array_ as nullptr
TEST(DynamicArrayCopyConstructorMemoryTest, CopyConstructorWithZeroSizeArrayDoesNotLeakMemory)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "fault-injection");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty(
        "Description",
        "Check that copy-constructing from a zero-size DynamicArray yields a valid empty array (nullptr "
        "data, equal begin/end) instead of leaking memory.");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__containers__dynamic_array");
    this->RecordProperty("TestType", "fault-injection");
    this->RecordProperty("DerivationTechnique", "boundary-values");
    this->RecordProperty("Description",
                         "Check that copy-constructing from a zero-size DynamicArray of a non-trivial element type "
                         "yields a valid empty array instead of leaking memory.");

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
