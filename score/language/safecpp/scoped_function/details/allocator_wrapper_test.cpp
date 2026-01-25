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
#include "score/language/safecpp/scoped_function/details/allocator_wrapper.h"

#include "score/language/safecpp/scoped_function/details/testing_allocator.h"
#include "score/quality/compiler_warnings/warnings.h"

#include <score/memory.hpp>

#include <gtest/gtest.h>

namespace score::safecpp::details
{
namespace
{

class AllocatorWrapperTest : public ::testing::Test
{
  public:
    using Allocator = TestingAllocator<std::byte, std::false_type>;
};

TEST_F(AllocatorWrapperTest, CanCopyConstruct)
{
    const AllocatorWrapperTest::Allocator testing_allocator{};
    const AllocatorWrapper<AllocatorWrapperTest::Allocator> allocator_wrapper{testing_allocator};
    const auto copied_allocator = allocator_wrapper;
    ASSERT_EQ(copied_allocator, allocator_wrapper);

    std::byte* memory = copied_allocator->allocate(4);
    ASSERT_NE(memory, nullptr);
    copied_allocator->deallocate(memory, 4);
}

TEST_F(AllocatorWrapperTest, CanCopyAssign)
{
    const AllocatorWrapperTest::Allocator testing_allocator_1{};
    const AllocatorWrapper<AllocatorWrapperTest::Allocator> allocator_wrapper_1{testing_allocator_1};

    const AllocatorWrapperTest::Allocator testing_allocator_2{};
    AllocatorWrapper<AllocatorWrapperTest::Allocator> allocator_wrapper_2{testing_allocator_2};
    allocator_wrapper_2 = allocator_wrapper_1;
    ASSERT_EQ(allocator_wrapper_1, allocator_wrapper_2);

    std::byte* memory = allocator_wrapper_2->allocate(4);
    ASSERT_NE(memory, nullptr);
    allocator_wrapper_2->deallocate(memory, 4);
}

TEST_F(AllocatorWrapperTest, CorrectlyHandlesSelfCopyAssignment)
{
    const AllocatorWrapperTest::Allocator testing_allocator{};
    AllocatorWrapper<AllocatorWrapperTest::Allocator> allocator_wrapper{testing_allocator};

    DISABLE_WARNING_PUSH
    DISABLE_WARNING_SELF_ASSIGN_OVERLOADED

    allocator_wrapper = allocator_wrapper;

    DISABLE_WARNING_POP

    std::byte* memory = allocator_wrapper->allocate(4);
    ASSERT_NE(memory, nullptr);
    allocator_wrapper->deallocate(memory, 4);
}

TEST_F(AllocatorWrapperTest, CanMoveConstruct)
{
    const AllocatorWrapperTest::Allocator testing_allocator{};
    const AllocatorWrapper<AllocatorWrapperTest::Allocator> allocator_wrapper{testing_allocator};
    const auto moved_allocator = std::move(allocator_wrapper);
    ASSERT_EQ(*moved_allocator, testing_allocator);

    std::byte* memory = moved_allocator->allocate(4);
    ASSERT_NE(memory, nullptr);
    moved_allocator->deallocate(memory, 4);
}

TEST_F(AllocatorWrapperTest, CanMoveAssign)
{
    const AllocatorWrapperTest::Allocator testing_allocator_1{};
    const AllocatorWrapper<AllocatorWrapperTest::Allocator> allocator_wrapper_1{testing_allocator_1};

    const AllocatorWrapperTest::Allocator testing_allocator_2{};
    AllocatorWrapper<AllocatorWrapperTest::Allocator> allocator_wrapper_2{testing_allocator_2};
    allocator_wrapper_2 = std::move(allocator_wrapper_1);
    ASSERT_EQ(*allocator_wrapper_2, testing_allocator_1);

    std::byte* memory = allocator_wrapper_2->allocate(4);
    ASSERT_NE(memory, nullptr);
    allocator_wrapper_2->deallocate(memory, 4);
}

TEST_F(AllocatorWrapperTest, CorrectlyHandlesSelfMoveAssignment)
{
    const AllocatorWrapperTest::Allocator testing_allocator{};
    AllocatorWrapper<AllocatorWrapperTest::Allocator> allocator_wrapper{testing_allocator};

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
#endif
    allocator_wrapper = std::move(allocator_wrapper);
#ifdef __clang__
#pragma clang diagnostic pop
#endif

    std::byte* memory = allocator_wrapper->allocate(4);
    ASSERT_NE(memory, nullptr);
    allocator_wrapper->deallocate(memory, 4);
}

TEST_F(AllocatorWrapperTest, DereferencingOperatorReturnsAllocator)
{
    const AllocatorWrapperTest::Allocator testing_allocator{};
    const AllocatorWrapper<AllocatorWrapperTest::Allocator> allocator_wrapper{testing_allocator};
    ASSERT_EQ(*allocator_wrapper, testing_allocator);
}

TEST_F(AllocatorWrapperTest, ArrowOperatorReturnsPointerToAllocator)
{
    const AllocatorWrapperTest::Allocator testing_allocator{};
    const AllocatorWrapper<AllocatorWrapperTest::Allocator> allocator_wrapper{testing_allocator};
    ASSERT_EQ(allocator_wrapper.operator->(), &*allocator_wrapper);
}

TEST_F(AllocatorWrapperTest, GetPolymorphicAllocatorReturnsPolymorphicVersionOfAllocator)
{
    const AllocatorWrapperTest::Allocator testing_allocator{};
    score::cpp::pmr::resource_adaptor<AllocatorWrapperTest::Allocator> resource_adaptor{testing_allocator};
    score::cpp::pmr::polymorphic_allocator<std::byte> expected_polymorphic_allocator{&resource_adaptor};

    const AllocatorWrapper<AllocatorWrapperTest::Allocator> allocator_wrapper{testing_allocator};
    auto polymorphic_allocator = allocator_wrapper.GetPolymorphicAllocator();
    static_assert(std::is_same_v<decltype(polymorphic_allocator), score::cpp::pmr::polymorphic_allocator<std::byte>>);
    ASSERT_EQ(polymorphic_allocator, expected_polymorphic_allocator);
}

TEST_F(AllocatorWrapperTest, ComparesEqual)
{
    const AllocatorWrapperTest::Allocator testing_allocator_1{};
    const AllocatorWrapper<AllocatorWrapperTest::Allocator> allocator_wrapper_1{testing_allocator_1};

    const AllocatorWrapper<AllocatorWrapperTest::Allocator> allocator_wrapper_2{testing_allocator_1};
    ASSERT_TRUE(allocator_wrapper_2 == allocator_wrapper_1);
}

TEST_F(AllocatorWrapperTest, ComparesInequal)
{
    const AllocatorWrapperTest::Allocator testing_allocator_1{};
    const AllocatorWrapper<AllocatorWrapperTest::Allocator> allocator_wrapper_1{testing_allocator_1};

    const AllocatorWrapperTest::Allocator testing_allocator_2{};
    const AllocatorWrapper<AllocatorWrapperTest::Allocator> allocator_wrapper_2{testing_allocator_2};
    ASSERT_FALSE(allocator_wrapper_2 == allocator_wrapper_1);
}

}  // namespace
}  // namespace score::safecpp::details
