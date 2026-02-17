/********************************************************************************
 * Copyright (c) 2020 Contributors to the Eclipse Foundation
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

///
/// \file
/// \copyright Copyright (c) 2020 Contributors to the Eclipse Foundation
///

#include <score/memory.hpp>
#include <score/memory.hpp> // check include guard

#include <score/assert_support.hpp>
#include <score/memory_resource.hpp>
#include <score/vector.hpp>

#include <array>
#include <exception>
#include <type_traits>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace
{

struct base
{
    virtual ~base() = default;
};

struct derived : public base
{
    derived() : data{} {}
    explicit derived(const int v) : data{v} {}
    derived(const derived&) = default;
    derived(derived&&) = default;
    derived& operator=(const derived&) = default;
    derived& operator=(derived&&) = default;
    ~derived() override = default;

    int data;
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18781254
TEST(MemoryTest, UniquePtr_DefaultConstructsDeleterWithNullptrMemoryResource)
{
    score::cpp::pmr::unique_ptr<int> pointer{};
    const auto& deleter = pointer.get_deleter();
    EXPECT_EQ(deleter.memory_resource(), nullptr);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18781254
TEST(MemoryTest, UniquePtr_DefaultConstructedDeleterInvocationViolatesContract)
{
    score::cpp::pmr::unique_ptr<int> pointer{};
    const auto& deleter = pointer.get_deleter();
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(deleter(pointer.get()));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18781254
TEST(MemoryTest, MakeShared_DefaultConstructsWithoutArguments)
{
    {
        const auto x = score::cpp::pmr::make_shared<int>(score::cpp::pmr::new_delete_resource());
        EXPECT_EQ(*x, 0);
    }
    {
        const auto x = score::cpp::pmr::make_shared<const int>(score::cpp::pmr::new_delete_resource());
        static_assert(std::is_same<const std::shared_ptr<const int>, decltype(x)>::value, "failed");
        EXPECT_EQ(*x, 0);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18781254
TEST(MemoryTest, MakeShared_ConstructsWithArguments)
{
    {
        const int expected{42};
        const auto x = score::cpp::pmr::make_shared<int>(score::cpp::pmr::new_delete_resource(), expected);
        EXPECT_EQ(*x, expected);
    }
    {
        const int expected{42};
        const auto x = score::cpp::pmr::make_shared<const int>(score::cpp::pmr::new_delete_resource(), expected);
        static_assert(std::is_same<const std::shared_ptr<const int>, decltype(x)>::value, "failed");
        EXPECT_EQ(*x, expected);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18781254
TEST(MemoryTest, MakeShared_AllocatesWithMemoryResource)
{
    EXPECT_THROW(score::cpp::pmr::make_shared<int>(score::cpp::pmr::null_memory_resource()), std::bad_alloc);
    EXPECT_THROW(score::cpp::pmr::make_shared<const int>(score::cpp::pmr::null_memory_resource()), std::bad_alloc);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18781254
TEST(MemoryTest, MakeUnique_DefaultConstructsWithoutArguments)
{
    auto x = score::cpp::pmr::make_unique<int>(score::cpp::pmr::new_delete_resource());
    EXPECT_EQ(*x, 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18781254
TEST(MemoryTest, MakeUnique_ConstructsWithArguments)
{
    const int expected{42};
    const auto x = score::cpp::pmr::make_unique<int>(score::cpp::pmr::new_delete_resource(), expected);
    EXPECT_EQ(*x, expected);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18781254
TEST(MemoryTest, MakeUnique_AllocatesWithMemoryResource)
{
    EXPECT_THROW(score::cpp::pmr::make_unique<int>(score::cpp::pmr::null_memory_resource()), std::bad_alloc);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18781254
TEST(MemoryTest, MakeUnique_PointerToDerivedIsImplicitlyConvertibleToPointerToBase)
{
    score::cpp::pmr::unique_ptr<base> pointer;
    EXPECT_EQ(pointer, nullptr);
    pointer = score::cpp::pmr::make_unique<derived>(score::cpp::pmr::new_delete_resource());
    EXPECT_NE(pointer, nullptr);
}

template <int Id>
struct test_memory_resource final : score::cpp::pmr::memory_resource
{
    void* do_allocate(const std::size_t bytes, const std::size_t alignment) override
    {
        ++allocations;
        last_allocated_bytes = bytes;
        last_allocated_alignment = alignment;
        v = score::cpp::pmr::new_delete_resource()->allocate(bytes, alignment);
        return v;
    }
    void do_deallocate(void* /*unused*/, const std::size_t bytes, const std::size_t alignment) override
    {
        ++deallocations;
        last_deallocated_bytes = bytes;
        last_deallocated_alignment = alignment;
        score::cpp::pmr::new_delete_resource()->deallocate(v, bytes, alignment);
    }
    bool do_is_equal(const memory_resource& other) const noexcept override { return &other == this; }

    int allocations{};
    std::size_t last_allocated_bytes{};
    std::size_t last_allocated_alignment{};

    int deallocations{};
    std::size_t last_deallocated_bytes{};
    std::size_t last_deallocated_alignment{};

    void* v{};
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18781254
TEST(MemoryTest, MakeUnique_WhenDeallocatingThroughBasePointer_ExpectSizeAndAlignmentOfDerived)
{
    test_memory_resource<0> resource{};

    {
        score::cpp::pmr::unique_ptr<base> x{score::cpp::pmr::make_unique<derived>(&resource)};
    }

    EXPECT_EQ(1, resource.allocations);
    EXPECT_EQ(sizeof(derived), resource.last_allocated_bytes);
    EXPECT_EQ(alignof(derived), resource.last_allocated_alignment);

    EXPECT_EQ(1, resource.deallocations);
    EXPECT_EQ(sizeof(derived), resource.last_deallocated_bytes);
    EXPECT_EQ(alignof(derived), resource.last_deallocated_alignment);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18781254
TEST(MemoryTest, MakeUnique_MoveConstruct)
{
    const int expected{42};
    test_memory_resource<0> resource_0{};
    auto x = score::cpp::pmr::make_unique<derived>(&resource_0, expected);
    EXPECT_EQ(expected, x->data);
    EXPECT_EQ(resource_0.v, x.get());

    auto y = std::move(x);

    EXPECT_EQ(expected, y->data);
    EXPECT_EQ(nullptr, x.get());
    EXPECT_EQ(resource_0.v, y.get());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18781254
TEST(MemoryTest, MakeUnique_CountAllocations_WhenMoveConstruct)
{
    test_memory_resource<0> resource_0{};
    auto x = score::cpp::pmr::make_unique<derived>(&resource_0);
    EXPECT_EQ(1, resource_0.allocations);
    EXPECT_EQ(0, resource_0.deallocations);

    auto y = std::move(x);

    EXPECT_EQ(1, resource_0.allocations);
    EXPECT_EQ(0, resource_0.deallocations);

    x.reset();

    EXPECT_EQ(1, resource_0.allocations);
    EXPECT_EQ(0, resource_0.deallocations);

    y.reset();

    EXPECT_EQ(1, resource_0.allocations);
    EXPECT_EQ(1, resource_0.deallocations);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18781254
TEST(MemoryTest, MakeUnique_MoveAssign)
{
    const int expected{42};
    test_memory_resource<0> resource_0{};
    auto x = score::cpp::pmr::make_unique<derived>(&resource_0, expected);
    EXPECT_EQ(resource_0.v, x.get());

    test_memory_resource<1> resource_1{};
    auto y = score::cpp::pmr::make_unique<derived>(&resource_1);
    EXPECT_EQ(resource_1.v, y.get());

    y = std::move(x);

    EXPECT_EQ(expected, y->data);
    EXPECT_EQ(nullptr, x.get());
    EXPECT_EQ(resource_0.v, y.get());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18781254
TEST(MemoryTest, MakeUnique_CountAllocations_WhenMoveAssign)
{
    test_memory_resource<0> resource_0{};
    auto x = score::cpp::pmr::make_unique<derived>(&resource_0);
    EXPECT_EQ(1, resource_0.allocations);
    EXPECT_EQ(0, resource_0.deallocations);

    test_memory_resource<1> resource_1{};
    auto y = score::cpp::pmr::make_unique<derived>(&resource_1);
    EXPECT_EQ(1, resource_1.allocations);
    EXPECT_EQ(0, resource_1.deallocations);

    y = std::move(x); // from this point on y should use resource_0

    EXPECT_EQ(1, resource_0.allocations);
    EXPECT_EQ(0, resource_0.deallocations);
    EXPECT_EQ(1, resource_1.allocations);
    EXPECT_EQ(1, resource_1.deallocations);

    x.reset();

    EXPECT_EQ(1, resource_0.allocations);
    EXPECT_EQ(0, resource_0.deallocations);
    EXPECT_EQ(1, resource_1.allocations);
    EXPECT_EQ(1, resource_1.deallocations);

    y.reset();

    EXPECT_EQ(1, resource_0.allocations);
    EXPECT_EQ(1, resource_0.deallocations);
    EXPECT_EQ(1, resource_1.allocations);
    EXPECT_EQ(1, resource_1.deallocations);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18781254
TEST(MemoryTest, MakeUnique_UsesAllocatorConstruction)
{
    score::cpp::pmr::memory_resource* const resource{score::cpp::pmr::new_delete_resource()};
    const auto pointer = score::cpp::pmr::make_unique<score::cpp::pmr::vector<int>>(resource);
    EXPECT_EQ(pointer->get_allocator(), score::cpp::pmr::polymorphic_allocator<int>(resource));
}

using trailing_archetype = score::cpp::pmr::vector<int>;

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18781254
TEST(MakeObjUsingAllocatorTest, WithLValue)
{
    using allocator_type = score::cpp::pmr::polymorphic_allocator<int>;
    allocator_type pma{score::cpp::pmr::new_delete_resource()};
    const trailing_archetype original{1, 23, pma};
    const auto result = score::cpp::make_obj_using_allocator<trailing_archetype>(pma, original);
    EXPECT_EQ(result.get_allocator(), pma);
    EXPECT_EQ(result.at(0), 23);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18781254
TEST(MakeObjUsingAllocatorTest, WithRValue)
{
    using allocator_type = score::cpp::pmr::polymorphic_allocator<int>;
    allocator_type pma{score::cpp::pmr::new_delete_resource()};
    trailing_archetype original{1, 23, pma};
    const auto result = score::cpp::make_obj_using_allocator<trailing_archetype>(pma, std::move(original));
    EXPECT_EQ(result.get_allocator(), pma);
    EXPECT_EQ(result.at(0), 23);
    EXPECT_TRUE(original.empty());
}

} // namespace
