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
#include "score/containers/test/fake_memory_resource.h"

#include <new>

namespace score::containers::test
{

void* FakeMemoryResource::do_allocate(std::size_t bytes, std::size_t alignment)
{
    void* const memory{::operator new(bytes, std::align_val_t{alignment})};
    allocated_bytes_ += bytes;
    return memory;
}

void FakeMemoryResource::do_deallocate(void* memory, std::size_t bytes, std::size_t alignment)
{
    deallocated_bytes_ += bytes;
    ::operator delete(memory, bytes, std::align_val_t{alignment});
}

bool FakeMemoryResource::do_is_equal(const score::cpp::pmr::memory_resource& other) const noexcept
{
    return this == &other;
}

}  // namespace score::containers::test
