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
#ifndef SCORE_ANALYSIS_TRACING_COMMON_I_FLEXIBLE_CIRCULAR_ALLOCATOR_H
#define SCORE_ANALYSIS_TRACING_COMMON_I_FLEXIBLE_CIRCULAR_ALLOCATOR_H

#include "score/analysis/tracing/common/interface_types/types.h"
#include <cstdlib>

namespace score
{
namespace analysis
{
namespace tracing
{

class IFlexibleCircularAllocator
{
  protected:
    /// @brief IFlexibleCircularAllocator default copy constructor.
    IFlexibleCircularAllocator(const IFlexibleCircularAllocator&) noexcept = default;
    /// @brief IFlexibleCircularAllocator default move constructor.
    IFlexibleCircularAllocator(IFlexibleCircularAllocator&&) noexcept = default;
    /// @brief IFlexibleCircularAllocator default copy operator.
    IFlexibleCircularAllocator& operator=(const IFlexibleCircularAllocator&) = default;
    /// @brief IFlexibleCircularAllocator default move operator.
    IFlexibleCircularAllocator& operator=(IFlexibleCircularAllocator&&) = default;

  public:
    /// @brief IFlexibleCircularAllocator default constructor.
    IFlexibleCircularAllocator() noexcept = default;
    /// @brief IFlexibleCircularAllocator default destructor.
    virtual ~IFlexibleCircularAllocator() = default;
    // This is intented, we don't enforce users to specify align unless needed
    // NOLINTNEXTLINE(google-default-arguments) see comment above
    virtual score::Result<void*> Allocate(const std::size_t, const std::size_t alignment = alignment::kBlockSize) = 0;

    virtual ResultBlank Deallocate(void* const, const std::size_t) = 0;

    virtual std::size_t GetAvailableMemory() noexcept = 0;

    virtual void GetTmdMemUsage(TmdStatistics& tmd_stats) noexcept = 0;

    virtual void* GetBaseAddress() const noexcept = 0;

    virtual std::size_t GetSize() const noexcept = 0;

    virtual bool IsInBounds(const void* const, const std::size_t) const noexcept = 0;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_COMMON_I_FLEXIBLE_CIRCULAR_ALLOCATOR_H
