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

#include "score/language/rust/memres/cpp/memory_resource_handle.h"

namespace score::language::rust::memres
{

// ---- MonotonicHandle -------------------------------------------------------

MonotonicHandle::MonotonicHandle(std::size_t size, bool hermetic)
    : upstream_{hermetic}, buffer_{std::make_unique<std::byte[]>(size)}, inner_{buffer_.get(), size, &upstream_}
{
}

score::cpp::pmr::memory_resource* MonotonicHandle::resource() noexcept
{
    return &inner_;
}

// ---- UnsynchronizedPoolHandle ----------------------------------------------

UnsynchronizedPoolHandle::UnsynchronizedPoolHandle(score::cpp::pmr::pool_options opts, bool hermetic)
    : upstream_{hermetic}, inner_{opts, &upstream_}
{
}

score::cpp::pmr::memory_resource* UnsynchronizedPoolHandle::resource() noexcept
{
    return &inner_;
}

// ---- DefaultHandle ---------------------------------------------------------

DefaultHandle::DefaultHandle() noexcept : resource_{score::cpp::pmr::get_default_resource()} {}

score::cpp::pmr::memory_resource* DefaultHandle::resource() noexcept
{
    return resource_;
}

}  // namespace score::language::rust::memres
