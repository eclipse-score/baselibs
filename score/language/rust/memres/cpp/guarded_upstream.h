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

#ifndef SCORE_LANGUAGE_RUST_MEMRES_CPP_GUARDED_UPSTREAM_H
#define SCORE_LANGUAGE_RUST_MEMRES_CPP_GUARDED_UPSTREAM_H

#include <score/memory_resource.hpp>
#include <cstddef>

namespace score::language::rust::memres
{

/// An `score::cpp::pmr::memory_resource` that intercepts upstream allocation
/// requests from an inner PMR resource and enforces a hermetic boundary.
///
/// When constructed with `hermetic = true`:
///   - Any `do_allocate` call logs a fatal message and calls `std::abort()`.
///     This guarantees that no allocations beyond the pre-allocated pool ever
///     reach the system heap.
///
/// When constructed with `hermetic = false`:
///   - Any `do_allocate` call logs a warning and then delegates to
///     `score::cpp::pmr::new_delete_resource()`, allowing fallback to the heap.
class GuardedUpstream final : public score::cpp::pmr::memory_resource
{
  public:
    explicit GuardedUpstream(bool hermetic) noexcept;
    ~GuardedUpstream() override = default;

    GuardedUpstream(const GuardedUpstream&) = delete;
    GuardedUpstream& operator=(const GuardedUpstream&) = delete;
    GuardedUpstream(GuardedUpstream&&) = delete;
    GuardedUpstream& operator=(GuardedUpstream&&) = delete;

  private:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override;
    void do_deallocate(void* ptr, std::size_t bytes, std::size_t alignment) override;
    bool do_is_equal(const score::cpp::pmr::memory_resource& other) const noexcept override;

    bool hermetic_;
};

}  // namespace score::language::rust::memres

#endif  // SCORE_LANGUAGE_RUST_MEMRES_CPP_GUARDED_UPSTREAM_H
