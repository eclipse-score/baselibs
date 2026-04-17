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
#include "score/language/safecpp/scoped_function/details/instrumented_memory_resource.h"

#include <score/assert.hpp>

namespace score
{
namespace safecpp
{

std::int32_t InstrumentedMemoryResource::GetNumberOfAllocations() const noexcept
{
    return active_allocations_;
}

void* InstrumentedMemoryResource::do_allocate(std::size_t bytes, std::size_t alignment)
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(
        active_allocations_ < std::numeric_limits<decltype(active_allocations_)>::max(),
        "[InstrumentedMemoryResource] Number of active allocations exceeds the maximum number of active allocations.");
    active_allocations_++;
    return memory_resource_->allocate(bytes, alignment);
}

void InstrumentedMemoryResource::do_deallocate(void* p, std::size_t bytes, std::size_t alignment)
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(
        active_allocations_ > std::numeric_limits<decltype(active_allocations_)>::min(),
        "[InstrumentedMemoryResource] Number of active allocations exceeds minimum number of active allocations.");
    active_allocations_--;
    memory_resource_->deallocate(p, bytes, alignment);
}

bool InstrumentedMemoryResource::do_is_equal(const memory_resource& resource) const noexcept
{
    const auto* instrumented_memory_resource = dynamic_cast<const InstrumentedMemoryResource*>(&resource);
    if (instrumented_memory_resource != nullptr)
    {
        return id_ == instrumented_memory_resource->id_;
    }
    return false;
}

}  // namespace safecpp
}  // namespace score
