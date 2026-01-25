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

#include <score/private/memory/uses_allocator_construction_args.hpp>
#include <score/private/memory/uses_allocator_construction_args.hpp> // check include guard

#include <score/memory_resource.hpp>
#include <score/vector.hpp>

#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

namespace
{

using trailing_archetype = score::cpp::pmr::vector<int>;
using leading_archetype = std::tuple<int>;

// NOTRACING
TEST(UsesAllocatorConstructionArgsTest, NonPairNoAllocatorConstructionWithLValue)
{
    using allocator_type = score::cpp::pmr::polymorphic_allocator<int>;
    allocator_type pma{score::cpp::pmr::new_delete_resource()};
    const int value{23};
    auto result = score::cpp::detail::uses_allocator_construction_args<int>(pma, value);
    using lvalue = std::tuple<const int&>;
    EXPECT_TRUE((std::is_same<decltype(result), lvalue>::value));
    EXPECT_EQ(result, std::make_tuple(value));
}

// NOTRACING
TEST(UsesAllocatorConstructionArgsTest, NonPairNoAllocatorConstructionWithRValue)
{
    using allocator_type = score::cpp::pmr::polymorphic_allocator<int>;
    allocator_type pma{score::cpp::pmr::new_delete_resource()};
    int value{23};
    auto result = score::cpp::detail::uses_allocator_construction_args<int>(pma, std::move(value));
    using rvalue = std::tuple<int&&>;
    EXPECT_TRUE((std::is_same<decltype(result), rvalue>::value));
    EXPECT_EQ(result, std::make_tuple(value));
}

// NOTRACING
TEST(UsesAllocatorConstructionArgsTest, NonPairLeadingAllocatorConstructionWithLValue)
{
    using allocator_type = score::cpp::pmr::polymorphic_allocator<int>;
    allocator_type pma{score::cpp::pmr::new_delete_resource()};
    const int value{23};
    auto result = score::cpp::detail::uses_allocator_construction_args<leading_archetype>(pma, value);
    using lvalue = std::tuple<std::allocator_arg_t, const allocator_type&, const int&>;
    EXPECT_TRUE((std::is_same<decltype(result), lvalue>::value));
    EXPECT_EQ(std::get<1>(result), pma);
    EXPECT_EQ(std::get<2>(result), value);
}

// NOTRACING
TEST(UsesAllocatorConstructionArgsTest, NonPairLeadingAllocatorConstructionWithRValue)
{
    using allocator_type = score::cpp::pmr::polymorphic_allocator<int>;
    allocator_type pma{score::cpp::pmr::new_delete_resource()};
    int value{23};
    auto result = score::cpp::detail::uses_allocator_construction_args<leading_archetype>(pma, std::move(value));
    using rvalue = std::tuple<std::allocator_arg_t, const allocator_type&, int&&>;
    EXPECT_TRUE((std::is_same<decltype(result), rvalue>::value));
    EXPECT_EQ(std::get<1>(result), pma);
    EXPECT_EQ(std::get<2>(result), value);
}

// NOTRACING
TEST(UsesAllocatorConstructionArgsTest, NonPairTrailingAllocatorConstructionWithLValue)
{
    using allocator_type = score::cpp::pmr::polymorphic_allocator<int>;
    allocator_type pma{score::cpp::pmr::new_delete_resource()};
    const int value{23};
    auto result = score::cpp::detail::uses_allocator_construction_args<trailing_archetype>(pma, value);
    using lvalue = std::tuple<const int&, const allocator_type&>;
    EXPECT_TRUE((std::is_same<decltype(result), lvalue>::value));
    EXPECT_EQ(result, std::make_tuple(value, pma));
}

// NOTRACING
TEST(UsesAllocatorConstructionArgsTest, NonPairTrailingAllocatorConstructionWithRValue)
{
    using allocator_type = score::cpp::pmr::polymorphic_allocator<int>;
    allocator_type pma{score::cpp::pmr::new_delete_resource()};
    int value{23};
    auto result = score::cpp::detail::uses_allocator_construction_args<trailing_archetype>(pma, std::move(value));
    using rvalue = std::tuple<int&&, const allocator_type&>;
    EXPECT_TRUE((std::is_same<decltype(result), rvalue>::value));
    EXPECT_EQ(result, std::make_tuple(value, pma));
}

// NOTRACING
TEST(UsesAllocatorConstructionArgsTest, PairNoAllocatorConstructionWithLValue)
{
    using allocator_type = score::cpp::pmr::polymorphic_allocator<int>;
    allocator_type pma{score::cpp::pmr::new_delete_resource()};
    const int value1{57};
    const int value2{58};
    auto result = score::cpp::detail::uses_allocator_construction_args<std::pair<int, int>>(pma, value1, value2);
    using lvalue = std::tuple<const int&>;
    EXPECT_TRUE((std::is_same<decltype(result), std::tuple<std::piecewise_construct_t, lvalue, lvalue>>::value));
    EXPECT_EQ(std::get<1>(result), std::make_tuple(value1));
    EXPECT_EQ(std::get<2>(result), std::make_tuple(value2));
}

// NOTRACING
TEST(UsesAllocatorConstructionArgsTest, PairNoAllocatorConstructionWithRValue)
{
    using allocator_type = score::cpp::pmr::polymorphic_allocator<int>;
    allocator_type pma{score::cpp::pmr::new_delete_resource()};
    int value1{57};
    int value2{58};
    auto result =
        score::cpp::detail::uses_allocator_construction_args<std::pair<int, int>>(pma, std::move(value1), std::move(value2));
    using rvalue = std::tuple<int&&>;
    EXPECT_TRUE((std::is_same<decltype(result), std::tuple<std::piecewise_construct_t, rvalue, rvalue>>::value));
    EXPECT_EQ(std::get<1>(result), std::make_tuple(value1));
    EXPECT_EQ(std::get<2>(result), std::make_tuple(value2));
}

// NOTRACING
TEST(UsesAllocatorConstructionArgsTest, PairTrailingAllocatorDefaultConstruction)
{
    using allocator_type = score::cpp::pmr::polymorphic_allocator<int>;
    allocator_type pma{score::cpp::pmr::new_delete_resource()};
    auto result = score::cpp::detail::uses_allocator_construction_args<std::pair<trailing_archetype, trailing_archetype>>(pma);
    using expected_type = std::tuple<const allocator_type&>;
    EXPECT_TRUE(
        (std::is_same<decltype(result), std::tuple<std::piecewise_construct_t, expected_type, expected_type>>::value));
    EXPECT_EQ(std::get<1>(result), std::make_tuple(pma));
    EXPECT_EQ(std::get<2>(result), std::make_tuple(pma));
}

// NOTRACING
TEST(UsesAllocatorConstructionArgsTest, PairTrailingAllocatorConstructionWithLValue)
{
    using allocator_type = score::cpp::pmr::polymorphic_allocator<int>;
    allocator_type pma{score::cpp::pmr::new_delete_resource()};
    const int value1{57};
    const int value2{58};
    auto result = score::cpp::detail::uses_allocator_construction_args<std::pair<trailing_archetype, trailing_archetype>>(
        pma, value1, value2);
    using lvalue = std::tuple<const int&, const allocator_type&>;
    EXPECT_TRUE((std::is_same<decltype(result), std::tuple<std::piecewise_construct_t, lvalue, lvalue>>::value));
    EXPECT_EQ(std::get<1>(result), std::make_tuple(value1, pma));
    EXPECT_EQ(std::get<2>(result), std::make_tuple(value2, pma));
}

// NOTRACING
TEST(UsesAllocatorConstructionArgsTest, PairTrailingAllocatorConstructionWithRValue)
{
    using allocator_type = score::cpp::pmr::polymorphic_allocator<int>;
    allocator_type pma{score::cpp::pmr::new_delete_resource()};
    int value1{57};
    int value2{58};
    auto result = score::cpp::detail::uses_allocator_construction_args<std::pair<trailing_archetype, trailing_archetype>>(
        pma, std::move(value1), std::move(value2));
    using rvalue = std::tuple<int&&, const allocator_type&>;
    EXPECT_TRUE((std::is_same<decltype(result), std::tuple<std::piecewise_construct_t, rvalue, rvalue>>::value));
    EXPECT_EQ(std::get<1>(result), std::make_tuple(value1, pma));
    EXPECT_EQ(std::get<2>(result), std::make_tuple(value2, pma));
}

// NOTRACING
TEST(UsesAllocatorConstructionArgsTest, PairLeadingAllocatorConstructionWithLValue)
{
    using allocator_type = score::cpp::pmr::polymorphic_allocator<int>;
    allocator_type pma{score::cpp::pmr::new_delete_resource()};
    const int value1{57};
    const int value2{58};
    auto result = score::cpp::detail::uses_allocator_construction_args<std::pair<leading_archetype, leading_archetype>>(
        pma, value1, value2);
    using lvalue = std::tuple<std::allocator_arg_t, const allocator_type&, const int&>;
    EXPECT_TRUE((std::is_same<decltype(result), std::tuple<std::piecewise_construct_t, lvalue, lvalue>>::value));
    EXPECT_EQ(std::get<1>(std::get<1>(result)), pma);
    EXPECT_EQ(std::get<2>(std::get<1>(result)), value1);
    EXPECT_EQ(std::get<1>(std::get<2>(result)), pma);
    EXPECT_EQ(std::get<2>(std::get<2>(result)), value2);
}

// NOTRACING
TEST(UsesAllocatorConstructionArgsTest, PairLeadingAllocatorConstructionWithRValue)
{
    using allocator_type = score::cpp::pmr::polymorphic_allocator<int>;
    allocator_type pma{score::cpp::pmr::new_delete_resource()};
    int value1{57};
    int value2{58};
    auto result = score::cpp::detail::uses_allocator_construction_args<std::pair<leading_archetype, leading_archetype>>(
        pma, std::move(value1), std::move(value2));
    using rvalue = std::tuple<std::allocator_arg_t, const allocator_type&, int&&>;
    EXPECT_TRUE((std::is_same<decltype(result), std::tuple<std::piecewise_construct_t, rvalue, rvalue>>::value));
    EXPECT_EQ(std::get<1>(std::get<1>(result)), pma);
    EXPECT_EQ(std::get<2>(std::get<1>(result)), value1);
    EXPECT_EQ(std::get<1>(std::get<2>(result)), pma);
    EXPECT_EQ(std::get<2>(std::get<2>(result)), value2);
}

struct distinct_resource final : score::cpp::pmr::memory_resource
{
    void* do_allocate(const std::size_t bytes, const std::size_t alignment) override
    {
        return score::cpp::pmr::new_delete_resource()->allocate(bytes, alignment);
    }
    void do_deallocate(void* p, const std::size_t bytes, const std::size_t alignment) override
    {
        score::cpp::pmr::new_delete_resource()->deallocate(p, bytes, alignment);
    }
    bool do_is_equal(const memory_resource& other) const noexcept override { return &other == this; }
};

// NOTRACING
TEST(UsesAllocatorConstructionArgsTest, PairTrailingAllocatorCopyConstruction)
{
    using allocator_type = score::cpp::pmr::polymorphic_allocator<int>;
    distinct_resource mr0{};
    distinct_resource mr1{};
    allocator_type pma0{&mr0};
    allocator_type pma1{&mr1};
    const std::pair<trailing_archetype, trailing_archetype> original{pma0.resource(), pma0.resource()};
    auto result = score::cpp::detail::uses_allocator_construction_args<std::pair<trailing_archetype, trailing_archetype>>(
        pma1, original);
    using expected_type = std::tuple<const trailing_archetype&, const allocator_type&>;
    EXPECT_TRUE(
        (std::is_same<decltype(result), std::tuple<std::piecewise_construct_t, expected_type, expected_type>>::value));
    EXPECT_EQ(std::get<1>(result), std::forward_as_tuple(original.first, pma1));
    EXPECT_EQ(std::get<2>(result), std::forward_as_tuple(original.second, pma1));
}

// NOTRACING
TEST(UsesAllocatorConstructionArgsTest, PairTrailingAllocatorMoveConstruction)
{
    using allocator_type = score::cpp::pmr::polymorphic_allocator<int>;
    distinct_resource mr0{};
    distinct_resource mr1{};
    allocator_type pma0{&mr0};
    allocator_type pma1{&mr1};
    std::pair<trailing_archetype, trailing_archetype> original{pma0.resource(), pma0.resource()};
    std::pair<trailing_archetype, trailing_archetype> expected{original};
    auto result = score::cpp::detail::uses_allocator_construction_args<std::pair<trailing_archetype, trailing_archetype>>(
        pma1, std::move(original));
    using expected_type = std::tuple<trailing_archetype&&, const allocator_type&>;
    EXPECT_TRUE(
        (std::is_same<decltype(result), std::tuple<std::piecewise_construct_t, expected_type, expected_type>>::value));
    EXPECT_EQ(std::get<1>(result), std::forward_as_tuple(expected.first, pma1));
    EXPECT_EQ(std::get<2>(result), std::forward_as_tuple(expected.second, pma1));
}

// NOTRACING
TEST(UsesAllocatorConstructionArgsTest, RecursivePairAllocatorConstruction)
{
    using allocator_type = score::cpp::pmr::polymorphic_allocator<int>;
    allocator_type pma{score::cpp::pmr::new_delete_resource()};
    const std::tuple<int> value1{57};
    const auto inner_pair_arguments =
        std::make_tuple(std::piecewise_construct, std::make_tuple(58), std::make_tuple(59));
    auto result =
        score::cpp::detail::uses_allocator_construction_args<std::pair<int, std::pair<trailing_archetype, leading_archetype>>>(
            pma, std::piecewise_construct, value1, inner_pair_arguments);
    EXPECT_TRUE((std::is_same<
                 decltype(result),
                 std::tuple<std::piecewise_construct_t,
                            std::tuple<const int&>,
                            std::tuple<std::piecewise_construct_t,
                                       std::tuple<const int&, const allocator_type&>,
                                       std::tuple<std::allocator_arg_t, const allocator_type&, const int&>>>>::value));
    const auto& result2 = std::get<2>(result);
    const auto& result22 = std::get<2>(result2);
    EXPECT_EQ(std::get<1>(result), std::make_tuple(57));
    EXPECT_EQ(std::get<1>(result2), std::make_tuple(58, pma));
    EXPECT_EQ(std::get<1>(result22), pma);
    EXPECT_EQ(std::get<2>(result22), 59);
}

} // namespace
