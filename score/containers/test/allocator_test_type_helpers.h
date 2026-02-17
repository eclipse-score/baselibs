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

#include "score/memory/shared/managed_memory_resource.h"
#include "score/memory/shared/polymorphic_offset_ptr_allocator.h"

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace score::containers
{

template <
    typename ElementType,
    typename Allocator,
    typename std::enable_if<!std::is_same<Allocator, memory::shared::PolymorphicOffsetPtrAllocator<ElementType>>::value,
                            bool>::type = true>
Allocator GetAllocator(memory::shared::ManagedMemoryResource&)
{
    return Allocator();
}

template <
    typename ElementType,
    typename Allocator,
    typename std::enable_if<std::is_same<Allocator, memory::shared::PolymorphicOffsetPtrAllocator<ElementType>>::value,
                            bool>::type = true>
Allocator GetAllocator(memory::shared::ManagedMemoryResource& memory_resource)
{
    return memory::shared::PolymorphicOffsetPtrAllocator<ElementType>{memory_resource};
}

namespace test_types
{

using score::memory::shared::PolymorphicOffsetPtrAllocator;

template <typename ElementTypeIn, template <typename> typename AllocatorIn>
struct ContainerTestTypes
{
    using ElementType = ElementTypeIn;
    using Allocator = AllocatorIn<ElementTypeIn>;
};

using AllAllocatorTypes =
    ::testing::Types<ContainerTestTypes<TrivialType, std::allocator>,
                     ContainerTestTypes<TrivialType, PolymorphicOffsetPtrAllocator>,

                     ContainerTestTypes<NonTrivialType, std::allocator>,
                     ContainerTestTypes<NonTrivialType, PolymorphicOffsetPtrAllocator>,

                     ContainerTestTypes<TriviallyConstructibleDestructibleType, std::allocator>,
                     ContainerTestTypes<TriviallyConstructibleDestructibleType, PolymorphicOffsetPtrAllocator>,

                     ContainerTestTypes<NonMoveableAndCopyableElementType, std::allocator>,
                     ContainerTestTypes<NonMoveableAndCopyableElementType, PolymorphicOffsetPtrAllocator>>;

using TrivialAllocatorTypes = ::testing::Types<ContainerTestTypes<TrivialType, std::allocator>,
                                               ContainerTestTypes<TrivialType, PolymorphicOffsetPtrAllocator>>;

using NonTrivialAllocatorTypes = ::testing::Types<ContainerTestTypes<NonTrivialType, std::allocator>,
                                                  ContainerTestTypes<NonTrivialType, PolymorphicOffsetPtrAllocator>>;

using TriviallyConstructibleDestructibleTypeAllocatorTypes =
    ::testing::Types<ContainerTestTypes<TriviallyConstructibleDestructibleType, std::allocator>,
                     ContainerTestTypes<TriviallyConstructibleDestructibleType, PolymorphicOffsetPtrAllocator>>;

using NonMoveableAndCopyableElementTypeAllocatorTypes =
    ::testing::Types<ContainerTestTypes<NonMoveableAndCopyableElementType, std::allocator>,
                     ContainerTestTypes<NonMoveableAndCopyableElementType, PolymorphicOffsetPtrAllocator>>;

using PolymorphicAllocatorTypes =
    ::testing::Types<ContainerTestTypes<TrivialType, PolymorphicOffsetPtrAllocator>,
                     ContainerTestTypes<NonTrivialType, PolymorphicOffsetPtrAllocator>,
                     ContainerTestTypes<TriviallyConstructibleDestructibleType, PolymorphicOffsetPtrAllocator>,
                     ContainerTestTypes<NonMoveableAndCopyableElementType, PolymorphicOffsetPtrAllocator>>;

using CopyableAndMoveablePolymorphicAllocatorTypes =
    ::testing::Types<ContainerTestTypes<TrivialType, PolymorphicOffsetPtrAllocator>,
                     ContainerTestTypes<NonTrivialType, PolymorphicOffsetPtrAllocator>,
                     ContainerTestTypes<TriviallyConstructibleDestructibleType, PolymorphicOffsetPtrAllocator>>;

}  // namespace test_types
}  // namespace score::containers

#endif  // SCORE_LIB_CONTAINERS_TEST_ALLOCATOR_TEST_TYPE_HELPERS
