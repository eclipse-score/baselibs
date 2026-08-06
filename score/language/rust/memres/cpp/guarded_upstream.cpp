/*******************************************************************************
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
 *******************************************************************************/

#include "score/language/rust/memres/cpp/guarded_upstream.h"

#include "score/mw/log/logging.h"

#include <score/memory_resource.hpp>
#include <cstdlib>

namespace score::language::rust::memres
{

GuardedUpstream::GuardedUpstream(bool hermetic) noexcept : hermetic_{hermetic} {}

void* GuardedUpstream::do_allocate(std::size_t bytes, std::size_t alignment)
{
    if (hermetic_)
    {
        score::mw::log::LogFatal("memres") << "Memory resource exhausted in hermetic mode: requested" << bytes
                                           << "bytes with alignment" << alignment << ". Aborting.";
        std::abort();
    }
    score::mw::log::LogWarn("memres") << "Memory resource exhausted: requested" << bytes << "bytes with alignment"
                                      << alignment << ". Falling back to system heap.";
    return score::cpp::pmr::new_delete_resource()->allocate(bytes, alignment);
}

void GuardedUpstream::do_deallocate(void* ptr, std::size_t bytes, std::size_t alignment)
{
    if (!hermetic_)
    {
        score::cpp::pmr::new_delete_resource()->deallocate(ptr, bytes, alignment);
    }
    // hermetic: do_allocate never returns (calls abort), so do_deallocate is
    // unreachable for hermetic-mode upstream allocations.
}

bool GuardedUpstream::do_is_equal(const score::cpp::pmr::memory_resource& other) const noexcept
{
    return this == &other;
}

}  // namespace score::language::rust::memres
