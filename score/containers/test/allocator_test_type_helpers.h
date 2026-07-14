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
#ifndef SCORE_LIB_CONTAINERS_TEST_ALLOCATOR_TEST_TYPE_HELPERS
#define SCORE_LIB_CONTAINERS_TEST_ALLOCATOR_TEST_TYPE_HELPERS

#include "score/containers/test/container_test_types.h"
#include "score/containers/test/fancy_pointer_allocator.h"

#include <score/memory_resource.hpp>

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace score::containers
{

template <
    typename ElementType,
    typename Allocator,
    typename std::enable_if<!std::is_same<Allocator, test::FancyPointerAllocator<ElementType>>::value,
                            bool>::type = true>
Allocator GetAllocator(score::cpp::pmr::memory_resource&)
{
    return Allocator();
}

template <
    typename ElementType,
    typename Allocator,
    typename std::enable_if<std::is_same<Allocator, test::FancyPointerAllocator<ElementType>>::value,
                            bool>::type = true>
Allocator GetAllocator(score::cpp::pmr::memory_resource& memory_resource)
{
    return test::FancyPointerAllocator<ElementType>{memory_resource};
}

namespace test_types
{

using score::containers::test::FancyPointerAllocator;

template <typename ElementTypeIn, template <typename> typename AllocatorIn>
struct ContainerTestTypes
{
    using ElementType = ElementTypeIn;
    using Allocator = AllocatorIn<ElementTypeIn>;
};

using AllAllocatorTypes =
    ::testing::Types<ContainerTestTypes<TrivialType, std::allocator>,
                     ContainerTestTypes<TrivialType, FancyPointerAllocator>,

                     ContainerTestTypes<NonTrivialType, std::allocator>,
                     ContainerTestTypes<NonTrivialType, FancyPointerAllocator>,

                     ContainerTestTypes<TriviallyConstructibleDestructibleType, std::allocator>,
                     ContainerTestTypes<TriviallyConstructibleDestructibleType, FancyPointerAllocator>,

                     ContainerTestTypes<NonMoveableAndCopyableElementType, std::allocator>,
                     ContainerTestTypes<NonMoveableAndCopyableElementType, FancyPointerAllocator>>;

using TrivialAllocatorTypes = ::testing::Types<ContainerTestTypes<TrivialType, std::allocator>,
                                               ContainerTestTypes<TrivialType, FancyPointerAllocator>>;

using NonTrivialAllocatorTypes = ::testing::Types<ContainerTestTypes<NonTrivialType, std::allocator>,
                                                  ContainerTestTypes<NonTrivialType, FancyPointerAllocator>>;

using TriviallyConstructibleDestructibleTypeAllocatorTypes =
    ::testing::Types<ContainerTestTypes<TriviallyConstructibleDestructibleType, std::allocator>,
                     ContainerTestTypes<TriviallyConstructibleDestructibleType, FancyPointerAllocator>>;

using NonMoveableAndCopyableElementTypeAllocatorTypes =
    ::testing::Types<ContainerTestTypes<NonMoveableAndCopyableElementType, std::allocator>,
                     ContainerTestTypes<NonMoveableAndCopyableElementType, FancyPointerAllocator>>;

using PolymorphicAllocatorTypes =
    ::testing::Types<ContainerTestTypes<TrivialType, FancyPointerAllocator>,
                     ContainerTestTypes<NonTrivialType, FancyPointerAllocator>,
                     ContainerTestTypes<TriviallyConstructibleDestructibleType, FancyPointerAllocator>,
                     ContainerTestTypes<NonMoveableAndCopyableElementType, FancyPointerAllocator>>;

using CopyableAndMoveablePolymorphicAllocatorTypes =
    ::testing::Types<ContainerTestTypes<TrivialType, FancyPointerAllocator>,
                     ContainerTestTypes<NonTrivialType, FancyPointerAllocator>,
                     ContainerTestTypes<TriviallyConstructibleDestructibleType, FancyPointerAllocator>>;

}  // namespace test_types
}  // namespace score::containers

#endif  // SCORE_LIB_CONTAINERS_TEST_ALLOCATOR_TEST_TYPE_HELPERS
