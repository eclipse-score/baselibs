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

#ifndef SCORE_LANGUAGE_RUST_MEMRES_CPP_MEMORY_RESOURCE_HANDLE_H
#define SCORE_LANGUAGE_RUST_MEMRES_CPP_MEMORY_RESOURCE_HANDLE_H

#include "score/language/rust/memres/cpp/guarded_upstream.h"

#include <score/memory_resource.hpp>
#include <cstddef>
#include <memory>

namespace score::language::rust::memres
{

/// Abstract base for owned memory resource handles.
///
/// Concrete subclasses hold a `GuardedUpstream` and the inner
/// `score::cpp::pmr::memory_resource` together. Destruction via a base pointer
/// is safe because the destructor is virtual.
class MemResHandle
{
  public:
    virtual ~MemResHandle() = default;
    virtual score::cpp::pmr::memory_resource* resource() noexcept = 0;

  protected:
    MemResHandle() = default;
    MemResHandle(const MemResHandle&) = delete;
    MemResHandle& operator=(const MemResHandle&) = delete;
    MemResHandle(MemResHandle&&) = delete;
    MemResHandle& operator=(MemResHandle&&) = delete;
};

/// Handle for `score::cpp::pmr::monotonic_buffer_resource`.
///
/// Member declaration order is significant for construction/destruction:
///   1. `upstream_`  — constructed first, destroyed last
///   2. `buffer_` — backing storage; must outlive `inner_`
///   3. `inner_`  — constructed last, destroyed first; calls `upstream_` on overflow
class MonotonicHandle final : public MemResHandle
{
  public:
    explicit MonotonicHandle(std::size_t size, bool hermetic);
    score::cpp::pmr::memory_resource* resource() noexcept override;

  private:
    GuardedUpstream upstream_;
    std::unique_ptr<std::byte[]> buffer_;
    score::cpp::pmr::monotonic_buffer_resource inner_;
};

/// Handle for `score::cpp::pmr::unsynchronized_pool_resource` (single-threaded).
///
/// Member declaration order: `upstream_` before `inner_` so that `inner_` is
/// destroyed first and can release blocks back to `upstream_` safely.
class UnsynchronizedPoolHandle final : public MemResHandle
{
  public:
    explicit UnsynchronizedPoolHandle(score::cpp::pmr::pool_options opts, bool hermetic);
    score::cpp::pmr::memory_resource* resource() noexcept override;

  private:
    GuardedUpstream upstream_;
    score::cpp::pmr::unsynchronized_pool_resource inner_;
};

/// Handle for the process-global default memory resource returned by
/// `score::cpp::pmr::get_default_resource()`.
///
/// This handle does **not** own the resource; destroying it only frees the
/// thin wrapper object.  The resource itself has process lifetime and must not
/// be deleted.
class DefaultHandle final : public MemResHandle
{
  public:
    DefaultHandle() noexcept;
    score::cpp::pmr::memory_resource* resource() noexcept override;

  private:
    score::cpp::pmr::memory_resource* resource_;
};

}  // namespace score::language::rust::memres

#endif  // SCORE_LANGUAGE_RUST_MEMRES_CPP_MEMORY_RESOURCE_HANDLE_H
