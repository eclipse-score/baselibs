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
#ifndef SCORE_ANALYSIS_TRACING_COMMON_LOCKLESS_FLEXIBLE_CIRCULAR_ALLOCATOR_FACTORY_H
#define SCORE_ANALYSIS_TRACING_COMMON_LOCKLESS_FLEXIBLE_CIRCULAR_ALLOCATOR_FACTORY_H
#include "score/analysis/tracing/common/flexible_circular_allocator/error_codes/factory/error_code.h"
#include "score/analysis/tracing/common/flexible_circular_allocator/flexible_circular_allocator_factory_interface.h"
#include "score/analysis/tracing/common/flexible_circular_allocator/lockless_flexible_circular_allocator.h"
#include "score/memory/shared/atomic_indirector.h"
#include "score/memory/shared/managed_memory_resource.h"

namespace score
{
namespace analysis
{
namespace tracing
{

class LocklessFlexibleCircularAllocatorFactory final : public IFlexibleCircularAllocatorFactory
{
  public:
    ~LocklessFlexibleCircularAllocatorFactory() override = default;
    LocklessFlexibleCircularAllocatorFactory() = default;
    LocklessFlexibleCircularAllocatorFactory(const LocklessFlexibleCircularAllocatorFactory&) = delete;
    LocklessFlexibleCircularAllocatorFactory& operator=(const LocklessFlexibleCircularAllocatorFactory&) = delete;
    LocklessFlexibleCircularAllocatorFactory(LocklessFlexibleCircularAllocatorFactory&&) = delete;
    LocklessFlexibleCircularAllocatorFactory& operator=(LocklessFlexibleCircularAllocatorFactory&&) = delete;

    AllocatorResult CreateAllocator(void* base_address, std::size_t size) const override
    {
        if (base_address == nullptr)
        {
            return score::MakeUnexpected(FlexibleAllocatorFactoryErrorCode::kBaseAddressVoid);
        }

        if (size == static_cast<std::size_t>(0))
        {
            return score::MakeUnexpected(FlexibleAllocatorFactoryErrorCode::kSizeIsZero);
        }

        return std::make_shared<LocklessFlexibleCircularAllocator<score::memory::shared::AtomicIndirectorReal>>(
            base_address, size);
    }
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_COMMON_LOCKLESS_FLEXIBLE_CIRCULAR_ALLOCATOR_FACTORY_H
