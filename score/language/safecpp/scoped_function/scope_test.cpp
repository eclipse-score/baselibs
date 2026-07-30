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
#include "score/language/safecpp/scoped_function/scope.h"

#include "score/language/safecpp/scoped_function/copyable_scoped_function.h"
#include "score/language/safecpp/scoped_function/details/instrumented_memory_resource.h"
#include "score/language/safecpp/scoped_function/move_only_scoped_function.h"
#include "score/quality/compiler_warnings/warnings.h"

#include <score/jthread.hpp>

#include <gtest/gtest.h>

#include <future>
#include <type_traits>

namespace score::safecpp
{
namespace
{

template <class T>
class ScopeTest : public ::testing::Test
{
};

using Types = ::testing::Types<CopyableScopedFunction<void()>, MoveOnlyScopedFunction<void()>>;
TYPED_TEST_SUITE(ScopeTest, Types, /* unused */);

TYPED_TEST(ScopeTest, CanConstruct)
{
    Scope<>{};
}

TYPED_TEST(ScopeTest, CanConstructWithAllocator)
{
    InstrumentedMemoryResource instrumented_memory_resource{};
    score::cpp::pmr::polymorphic_allocator<> allocator{&instrumented_memory_resource};

    Scope<decltype(allocator)> scope{allocator};

    EXPECT_EQ(instrumented_memory_resource.GetNumberOfAllocations(), 1);
}

TYPED_TEST(ScopeTest, DestructionExpiresAssociatedFunction)
{
    TypeParam function{};
    {
        Scope<> scope{};
        function = TypeParam{scope, []() noexcept {}};
    }

    EXPECT_FALSE(function());
}

TYPED_TEST(ScopeTest, DestructionIgnoresAlreadyDestructedFunctions)
{
    Scope<> scope{};
    {
        TypeParam{scope, []() noexcept {}};
    }
}

TYPED_TEST(ScopeTest, CanNotCopyConstruct)
{
    static_assert(!std::is_copy_constructible_v<Scope<>>, "Scope must not be copy-constructible");
}

TYPED_TEST(ScopeTest, CanNotCopyAssign)
{
    static_assert(!std::is_copy_assignable_v<Scope<>>, "Scope must not be copy-assignable");
}

TYPED_TEST(ScopeTest, CanMoveConstruct)
{
    Scope<> old_scope{};
    Scope<> new_scope{std::move(old_scope)};
}

TYPED_TEST(ScopeTest, MoveConstructionKeepsFunctionAttached)
{

    Scope<> old_scope{};
    TypeParam function{old_scope, []() noexcept {}};
    ASSERT_TRUE(function());

    Scope<> new_scope{std::move(old_scope)};

    EXPECT_TRUE(function());
}

TYPED_TEST(ScopeTest, MovedFromScopeDoesNotAffectLinksAnymore)
{
    Scope<> old_scope{};
    TypeParam function{old_scope, []() noexcept {}};
    Scope<> new_scope{std::move(old_scope)};
    ASSERT_TRUE(function());

    old_scope = Scope<>{};

    EXPECT_TRUE(function());
}

TYPED_TEST(ScopeTest, CanMoveAssign)
{
    Scope<> old_scope{};
    Scope<> new_scope{};
    new_scope = std::move(old_scope);
}

TYPED_TEST(ScopeTest, CanMoveAssignToItselfWithNoAdverseEffects)
{
    Scope<> scope{};
    TypeParam function{scope, []() noexcept {}};

    DISABLE_WARNING_PUSH
    DISABLE_WARNING_SELF_MOVE

    auto* scope_ptr = &scope;
    scope = std::move(*scope_ptr);

    DISABLE_WARNING_POP

    EXPECT_TRUE(function());
    scope.Expire();
    EXPECT_FALSE(function());
}

TYPED_TEST(ScopeTest, MoveAssignmentKeepsFunctionAttached)
{

    Scope<> old_scope{};
    TypeParam function{old_scope, []() noexcept {}};

    Scope<> new_scope{};
    new_scope = std::move(old_scope);

    EXPECT_TRUE(function());
}

TYPED_TEST(ScopeTest, MoveAssignedFromScopeDoesNotAffectLinksAnymore)
{
    Scope<> new_scope{};
    TypeParam function{};
    {
        Scope<> old_scope{};
        function = TypeParam{old_scope, []() noexcept {}};

        new_scope = std::move(old_scope);
    }

    EXPECT_TRUE(function());
}

TYPED_TEST(ScopeTest, AssociatedFunctionsOfOverwrittenScopeAreDiscardedWhenMoveAssigned)
{
    Scope<> old_scope{};
    TypeParam function{old_scope, []() noexcept {}};

    old_scope = Scope<>{};

    EXPECT_FALSE(function());
}

TYPED_TEST(ScopeTest, CanSimultaneouslyAttachFunctionsToScope)
{
    constexpr auto scope_size{10U};
    Scope<> scope{};
    std::promise<void> wait_promise{};
    auto future = wait_promise.get_future().share();

    constexpr auto num_functions_to_create_per_thread{scope_size / 2};

    auto function_creater = [&scope, future](const auto&, std::promise<void> promise) {
        bool called{false};
        std::vector<TypeParam> functions{};
        functions.reserve(num_functions_to_create_per_thread);
        for (auto i = 0U; i < num_functions_to_create_per_thread; ++i)
        {
            functions.emplace_back(scope, [&called]() noexcept {
                called = true;
            });
        }
        promise.set_value();
        future.wait();
        for (auto& function : functions)
        {
            function();
        }
        EXPECT_FALSE(called);
    };

    std::promise<void> thread_1_finished_work{};
    auto thread_1_finished_work_future{thread_1_finished_work.get_future()};
    score::cpp::jthread thread_1{function_creater, std::move(thread_1_finished_work)};

    std::promise<void> thread_2_finished_work{};
    auto thread_2_finished_work_future{thread_2_finished_work.get_future()};
    score::cpp::jthread thread_2{function_creater, std::move(thread_2_finished_work)};

    thread_1_finished_work_future.wait();
    thread_2_finished_work_future.wait();

    scope.Expire();

    wait_promise.set_value();
}

}  // namespace
}  // namespace score::safecpp
