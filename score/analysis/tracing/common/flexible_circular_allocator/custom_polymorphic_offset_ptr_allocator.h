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
#ifndef SCORE_ANALYSIS_TRACING_COMMON_CUSTOM_POLYMORPHIC_OFFSET_PTR_ALLOCATOR_H
#define SCORE_ANALYSIS_TRACING_COMMON_CUSTOM_POLYMORPHIC_OFFSET_PTR_ALLOCATOR_H

#include "score/analysis/tracing/common/flexible_circular_allocator/flexible_circular_allocator_interface.h"
#include "score/memory/shared/offset_ptr.h"

namespace score
{
namespace analysis
{
namespace tracing
{

// custom ploymorphic allocator uses our internal memory allocator wrt offset pointers.
template <typename T>
class CustomPolymorphicOffsetPtrAllocator
{
  public:
    // Type definitions required by the allocator
    using value_type = T;
    using pointer = score::memory::shared::OffsetPtr<T>;
    using size_type = std::size_t;
    // Constructor
    CustomPolymorphicOffsetPtrAllocator() noexcept = default;
    explicit CustomPolymorphicOffsetPtrAllocator(
        std::shared_ptr<IFlexibleCircularAllocator> flexible_allocator) noexcept
    {
        flexible_allocator_ = flexible_allocator;
    }

    pointer allocate(const size_type size, const size_type alignment = alignof(T))
    {
        // No possibility for data loss (a4-7-1)
        //  coverity[autosar_cpp14_a4_7_1_violation]
        auto allocated_memory_result = flexible_allocator_->Allocate(size * sizeof(T), alignment);
        if (!allocated_memory_result.has_value())
        {
            return nullptr;
        }
        // Aware of  casting void pointer "allocated_memory" to object pointer of struct
        //  coverity[autosar_cpp14_m5_2_8_violation]
        return pointer{static_cast<value_type*>(allocated_memory_result.value())};
    }
    // False positive here as the justification itself not correct : The One Definition Rule shall not be violated.
    //  coverity[autosar_cpp14_m3_2_2_violation]
    void deallocate(pointer p, size_type size)
    {
        // No possibility for data loss (a4-7-1)
        //  coverity[autosar_cpp14_a4_7_1_violation]
        std::ignore = flexible_allocator_->Deallocate(static_cast<void*>(&*p), size * sizeof(T));
    }
    // No harm here to define the pointer here in public section
    //  coverity[autosar_cpp14_m11_0_1_violation]
    std::shared_ptr<IFlexibleCircularAllocator> flexible_allocator_;
};
}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_COMMON_CUSTOM_POLYMORPHIC_OFFSET_PTR_ALLOCATOR_H
