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
#include "score/language/safecpp/scoped_function/copyable_scoped_function.h"
#include "score/language/safecpp/scoped_function/move_only_scoped_function.h"

#include "score/language/safecpp/scoped_function/details/instrumented_memory_resource.h"
#include "score/language/safecpp/scoped_function/details/parametrization_helper.h"
#include "score/language/safecpp/scoped_function/details/testing_allocator.h"
#include "score/language/safecpp/scoped_function/scope.h"

#include <gtest/gtest.h>

#include <type_traits>

namespace score::safecpp
{
namespace
{

template <class T>
class ScopedFunctionMovabilityTest : public ::testing::Test
{
};

using Types =
    ::testing::Types<ParametrizationHelper<CopyableScopedFunction>, ParametrizationHelper<MoveOnlyScopedFunction>>;
TYPED_TEST_SUITE(ScopedFunctionMovabilityTest, Types, /* unused */);

TYPED_TEST(ScopedFunctionMovabilityTest, CanMoveConstructWithoutScope)
{
    using ScopedFunction = typename TypeParam::template Type<void()>;
    ScopedFunction function{};
    ScopedFunction moved_function{std::move(function)};
}

TYPED_TEST(ScopedFunctionMovabilityTest, CanMoveConstructWithScope)
{
    bool called{false};
    Scope<> scope{};
    using ScopedFunction = typename TypeParam::template Type<void()>;
    ScopedFunction function{scope, [&called]() noexcept {
                                called = true;
                            }};
    ScopedFunction moved_function{std::move(function)};

    moved_function();
    EXPECT_TRUE(called);
}

TYPED_TEST(ScopedFunctionMovabilityTest, MoveConstructionKeepsFunctionAttached)
{
    Scope<> scope{};
    using ScopedFunction = typename TypeParam::template Type<void()>;
    ScopedFunction function{scope, []() noexcept {}};
    ScopedFunction moved_function{std::move(function)};

    EXPECT_TRUE(moved_function());
}

TYPED_TEST(ScopedFunctionMovabilityTest, MoveConstructionUsesSuppliedAllocator)
{
    using allocator_type = score::cpp::pmr::polymorphic_allocator<>;
    using ScopedFunction = typename TypeParam::template Type<void(), allocator_type>;
    static_assert(std::uses_allocator_v<ScopedFunction, allocator_type>);

    InstrumentedMemoryResource resource{};
    allocator_type allocator{&resource};

    Scope<> scope{};
    bool called{false};

    auto function = score::cpp::make_obj_using_allocator<ScopedFunction>(allocator, scope, [&called]() {
        called = true;
    });

    ASSERT_EQ(resource.GetNumberOfAllocations(), 1);

    InstrumentedMemoryResource other_resource{};
    score::cpp::pmr::polymorphic_allocator<> other_allocator{&other_resource};
    ScopedFunction moved_function{std::move(function), other_allocator};

    EXPECT_EQ(other_resource.GetNumberOfAllocations(), 1);

    moved_function();
    EXPECT_TRUE(called);
}

TYPED_TEST(ScopedFunctionMovabilityTest, MoveAssignmentPropagatesAllocatorIfItIsPOCMA)
{
    Scope<> scope{};

    using Allocator = details::TestingAllocator<std::byte, std::true_type>;
    using ScopedFunction = typename TypeParam::template Type<void(), Allocator>;
    ScopedFunction function{scope, []() noexcept {}};
    const auto expected_allocator = function.get_allocator();

    ScopedFunction moved_function{};

    moved_function = std::move(function);

    EXPECT_EQ(moved_function.get_allocator(), expected_allocator);
}

TYPED_TEST(ScopedFunctionMovabilityTest, MoveAssignmentDoesNotPropagateAllocatorIfItIsNotPOCMA)
{
    Scope<> scope{};

    using Allocator = details::TestingAllocator<std::byte, std::false_type>;
    using ScopedFunction = typename TypeParam::template Type<void(), Allocator>;
    ScopedFunction function{scope, []() noexcept {}};
    const auto expected_allocator = function.get_allocator();

    ScopedFunction moved_function{};

    moved_function = std::move(function);

    EXPECT_NE(moved_function.get_allocator(), expected_allocator);
}

class MoveOnlyScopedFunctionMovabilityTest : public ::testing::Test
{
};

class MoveOnlyType
{
  public:
    explicit MoveOnlyType(std::int32_t& call_count) : call_count_{&call_count} {}

    MoveOnlyType(const MoveOnlyType&) = delete;
    MoveOnlyType& operator=(const MoveOnlyType&) = delete;

    MoveOnlyType(MoveOnlyType&&) noexcept = default;
    MoveOnlyType& operator=(MoveOnlyType&&) noexcept = default;

    void operator()() noexcept
    {
        (*call_count_)++;
    }

  private:
    std::int32_t* call_count_;
};

TEST_F(MoveOnlyScopedFunctionMovabilityTest, CanConstructWithMoveOnlyLambda)
{
    std::int32_t call_count{0};
    MoveOnlyType move_only_type{call_count};
    static_assert(!std::is_copy_constructible_v<MoveOnlyType> && !std::is_copy_assignable_v<MoveOnlyType>,
                  "Make sure lambda can not be copied");
    static_assert(std::is_move_constructible_v<MoveOnlyType> && std::is_move_assignable_v<MoveOnlyType>,
                  "Make sure lambda can be moved");

    Scope<> scope{};
    MoveOnlyScopedFunction<void()> function{scope, std::move(move_only_type)};
    function();
    EXPECT_EQ(call_count, 1);
    MoveOnlyScopedFunction<void()> function_move{std::move(function)};

    function_move();
    EXPECT_EQ(call_count, 2);
}

TEST_F(MoveOnlyScopedFunctionMovabilityTest,
       DoesNotReallocateWhenCastingFromCopyableScopedFunctionToMoveOnlyScopedFunctionIfAllocatorEqual)
{
    InstrumentedMemoryResource instrumented_memory_resource{};
    score::cpp::pmr::polymorphic_allocator<std::byte> allocator{&instrumented_memory_resource};

    Scope<> scope{};
    CopyableScopedFunction<void(), score::cpp::pmr::polymorphic_allocator<std::byte>> function{
        std::allocator_arg, allocator, scope, []() noexcept {}};
    MoveOnlyScopedFunction<void(), score::cpp::pmr::polymorphic_allocator<std::byte>> function_move{std::move(function),
                                                                                                    allocator};

    EXPECT_EQ(instrumented_memory_resource.GetNumberOfAllocations(), 1);
}

}  // namespace
}  // namespace score::safecpp
