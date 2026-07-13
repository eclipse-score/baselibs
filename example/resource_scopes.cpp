/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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

/// Demonstrates the failure-handling decision rule of score::nothrow:
/// the same operation, allocation, is a *violation* (OrAbort) where the memory
/// budget is co-located with its use, and an *error* (Result) where the budget
/// is dislocated -- provisioned centrally, out of sight of the consuming code.
///
/// See docs/nothrow/architecture/failure_handling.rst and
/// docs/nothrow/architecture/usage_guidelines.rst.

#include "score/nothrow/memory_resource.h"
#include "score/nothrow/monotonic_buffer_resource.h"
#include "score/nothrow/vector.h"

#include <cstddef>
#include <cstdint>
#include <iostream>

namespace
{

using score::nothrow::GetNullMemoryResource;
using score::nothrow::MonotonicBufferResource;
using score::nothrow::PolymorphicAllocator;
using score::nothrow::SetDefaultResource;
using score::nothrow::Vector;

/// Co-located budget: buffer, resource, and consumption share one scope.
///
/// The budget is established two lines above its use: this scope owns the
/// buffer, sizes it for exactly kMaxReadings elements, and never exceeds that
/// count. An allocation failure here can only be a bug -- a broken design
/// assumption -- so the OrAbort variants are the correct choice: they trap at
/// the defect site instead of forcing callers to handle an error that cannot
/// legitimately occur.
void CoLocatedBudget()
{
    constexpr std::size_t kMaxReadings = 8U;
    alignas(std::int32_t) std::byte buffer[kMaxReadings * sizeof(std::int32_t)];
    MonotonicBufferResource resource{buffer, sizeof(buffer), GetNullMemoryResource()};
    PolymorphicAllocator<std::int32_t> allocator{&resource};

    Vector<std::int32_t> readings =
        Vector<std::int32_t>::CreateWithCapacityOrAbort(kMaxReadings, allocator);
    for (std::size_t i = 0U; i < kMaxReadings; ++i)
    {
        readings.PushBackOrAbort(static_cast<std::int32_t>(i * i));
    }

    std::cerr << "co-located: stored " << readings.size() << " readings, "
              << resource.remaining() << " bytes left in local buffer" << std::endl;
}

/// Dislocated budget: business logic that knows nothing about provisioning.
///
/// This function receives no resource and no size hint about the budget. The
/// memory it draws on is whatever the integrator configured centrally (see
/// main()), shared with every other default-allocated container in the
/// process. Exhaustion is therefore a reachable runtime condition -- an error
/// to be returned, not a bug -- and the failure path is testable by
/// configuring an undersized resource.
score::Result<Vector<std::int64_t>> BuildSquares(const std::size_t count)
{
    score::Result<Vector<std::int64_t>> squares =
        Vector<std::int64_t>::CreateWithCapacity(count);
    if (!squares.has_value())
    {
        return squares;  // propagate: only the caller knows how to react
    }

    for (std::size_t i = 0U; i < count; ++i)
    {
        // Capacity was reserved above, so this cannot allocate; within this
        // function the budget *is* co-located and OrAbort is legitimate.
        squares.value().PushBackOrAbort(static_cast<std::int64_t>(i * i));
    }
    return squares;
}

}  // namespace

int main()
{
    // Central configuration, standing in for integration time: every
    // default-allocated score::nothrow container in this process draws from
    // this pool. BuildSquares() neither sees this code nor its sizing.
    static std::byte pool[1024U];
    static MonotonicBufferResource pool_resource{pool, sizeof(pool), GetNullMemoryResource()};
    SetDefaultResource(&pool_resource);

    CoLocatedBudget();

    // A request that fits the configured budget succeeds.
    const auto small = BuildSquares(16U);
    std::cerr << "dislocated: request(16) "
              << (small.has_value() ? "served" : "rejected") << std::endl;

    // An oversized -- possibly attacker-sized -- request is rejected as a
    // contained error; the process keeps running and serves the next request.
    const auto huge = BuildSquares(1000000U);
    std::cerr << "dislocated: request(1000000) "
              << (huge.has_value() ? "served" : "rejected") << std::endl;

    const auto next = BuildSquares(4U);
    std::cerr << "dislocated: request(4) "
              << (next.has_value() ? "served" : "rejected")
              << " -- process survived the oversized request" << std::endl;

    return 0;
}
