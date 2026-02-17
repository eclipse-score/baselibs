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
#ifndef SCORE_ANALYSIS_TRACING_COMMON_FLEXIBLE_CIRCULAR_ALLOCATOR_MOCK_H
#define SCORE_ANALYSIS_TRACING_COMMON_FLEXIBLE_CIRCULAR_ALLOCATOR_MOCK_H

#include "score/analysis/tracing/common/flexible_circular_allocator/flexible_circular_allocator_interface.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
namespace score
{
namespace analysis
{
namespace tracing
{
class FlexibleCircularAllocatorMock : public IFlexibleCircularAllocator
{
  public:
    MOCK_METHOD(score::Result<void*>, Allocate, (const std::size_t, const std::size_t), (noexcept, override));
    MOCK_METHOD(ResultBlank, Deallocate, (void* const, const std::size_t), (noexcept, override));
    MOCK_METHOD(std::size_t, GetAvailableMemory, (), (noexcept, override));
    MOCK_METHOD(void, GetTmdMemUsage, (TmdStatistics&), (noexcept, override));
    MOCK_METHOD(void*, GetBaseAddress, (), (const, noexcept, override));
    MOCK_METHOD(std::size_t, GetSize, (), (const, noexcept, override));
    MOCK_METHOD(bool, IsInBounds, (const void* const, const std::size_t), (const, noexcept, override));
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_COMMON_FLEXIBLE_CIRCULAR_ALLOCATOR_MOCK_H
