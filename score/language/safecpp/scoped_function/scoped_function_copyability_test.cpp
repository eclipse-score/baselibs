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

#include "score/language/safecpp/scoped_function/details/instrumented_memory_resource.h"
#include "score/language/safecpp/scoped_function/details/testing_allocator.h"
#include "score/language/safecpp/scoped_function/scope.h"
#include "score/quality/compiler_warnings/warnings.h"

#include <score/memory.hpp>

#include <gtest/gtest.h>

namespace score::safecpp
{
namespace
{

class CopyabilityScopedFunctionTest : public ::testing::Test
{
};

TEST_F(CopyabilityScopedFunctionTest, CanCopyConstructWithoutScope)
{
    CopyableScopedFunction<void()> function{};
    CopyableScopedFunction<void()> function_copy{function};
}

TEST_F(CopyabilityScopedFunctionTest, CanCopyConstructWithScope)
{
    std::uint32_t called{0U};
    Scope<> scope{};
    CopyableScopedFunction<void()> function{scope, [&called]() noexcept {
                                                called++;
                                            }};
    CopyableScopedFunction<void()> function_copy{function};

    function();
    EXPECT_EQ(called, 1U);
    function_copy();
    EXPECT_EQ(called, 2U);
}

TEST_F(CopyabilityScopedFunctionTest, CopyConstructionUsesSuppliedAllocator)
{
    using allocator_type = score::cpp::pmr::polymorphic_allocator<>;
    static_assert(std::uses_allocator_v<CopyableScopedFunction<void(), allocator_type>, allocator_type>);

    InstrumentedMemoryResource resource{};
    allocator_type allocator{&resource};

    Scope<> scope{};
    bool called{false};

    auto function =
        score::cpp::make_obj_using_allocator<CopyableScopedFunction<void(), allocator_type>>(allocator, scope, [&called]() {
            called = true;
        });

    ASSERT_EQ(resource.GetNumberOfAllocations(), 1);

    InstrumentedMemoryResource other_resource{};
    score::cpp::pmr::polymorphic_allocator<> other_allocator{&other_resource};
    CopyableScopedFunction<void(), allocator_type> function_copy{function, other_allocator};

    EXPECT_EQ(other_resource.GetNumberOfAllocations(), 1);

    function();
    EXPECT_TRUE(called);
}

TEST_F(CopyabilityScopedFunctionTest, CanCopyAssignToItself)
{
    bool called{false};

    Scope<> scope{};
    CopyableScopedFunction<void()> function{scope, [&called]() noexcept {
                                                called = true;
                                            }};

    DISABLE_WARNING_PUSH
    DISABLE_WARNING_SELF_ASSIGN_OVERLOADED

    function = function;

    DISABLE_WARNING_POP

    function();

    EXPECT_TRUE(called);
}

TEST_F(CopyabilityScopedFunctionTest, CopyAssignmentPropagatesAllocatorIfItIsPOCCA)
{
    Scope<> scope{};

    using Allocator = details::TestingAllocator<std::aligned_storage<16, 16>::type, std::true_type>;
    CopyableScopedFunction<void(), Allocator> function{scope, []() noexcept {}};

    CopyableScopedFunction<void(), Allocator> function_copy{};

    function_copy = function;

    EXPECT_EQ(function_copy.get_allocator(), function.get_allocator());
}

TEST_F(CopyabilityScopedFunctionTest, CopyAssignmentDoesNotPropagateAllocatorIfItIsNotPOCCA)
{
    Scope<> scope{};

    using Allocator = details::TestingAllocator<std::aligned_storage<16, 16>::type, std::false_type>;
    CopyableScopedFunction<void(), Allocator> function{scope, []() noexcept {}};

    CopyableScopedFunction<void(), Allocator> function_copy{};

    function_copy = function;

    EXPECT_NE(function_copy.get_allocator(), function.get_allocator());
}

TEST_F(CopyabilityScopedFunctionTest, CanCopyAssignToFunctionWithoutScope)
{
    bool called{false};

    Scope<> scope{};
    CopyableScopedFunction<void()> function{scope, [&called]() noexcept {
                                                called = true;
                                            }};

    CopyableScopedFunction<void()> function_copy{};

    function_copy = function;

    function();

    EXPECT_TRUE(called);
}

class CopyOnlyType
{
  public:
    explicit CopyOnlyType(std::int32_t& call_count) : call_count_{&call_count} {}

    CopyOnlyType(const CopyOnlyType&) = default;
    CopyOnlyType& operator=(const CopyOnlyType&) = default;

    CopyOnlyType(CopyOnlyType&&) noexcept = delete;
    CopyOnlyType& operator=(CopyOnlyType&&) noexcept = delete;

    void operator()() noexcept
    {
        (*call_count_)++;
    }

  private:
    std::int32_t* call_count_;
};

TEST_F(CopyabilityScopedFunctionTest, CanConstructWithCopyOnlyLambda)
{
    std::int32_t call_count{0};
    CopyOnlyType copy_only_type{call_count};

    static_assert(!std::is_move_constructible_v<CopyOnlyType> && !std::is_move_assignable_v<CopyOnlyType>,
                  "Make sure CopyOnlyType can not be moved");

    static_assert(std::is_copy_constructible_v<CopyOnlyType> && std::is_copy_assignable_v<CopyOnlyType>,
                  "Make sure CopyOnlyType can be copied");

    Scope<> scope{};
    CopyableScopedFunction<void()> function{scope, copy_only_type};
    function();
    EXPECT_EQ(call_count, 1);
    CopyableScopedFunction<void()> function_copy{function};

    function();
    EXPECT_EQ(call_count, 2);

    function_copy();
    EXPECT_EQ(call_count, 3);
}

}  // namespace
}  // namespace score::safecpp
