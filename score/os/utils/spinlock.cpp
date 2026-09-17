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
#include "spinlock.h"
#include <atomic>

#if (defined(__x86_64__) || defined(__i386__)) && __has_include("emmintrin.h")
#include <emmintrin.h>
#define SPINLOCK_PAUSE_INSTR _mm_pause()
// NOLINTNEXTLINE(score-banned-preprocessor-directives) Suppressed here because usage of `#pragma` is forbidden
#pragma message( \
    "Spinlock info: x86 platform with intrinsics support detected -> using _mm_pause() instruction while spinning.")
#elif defined(__arm__)
#define SPINLOCK_PAUSE_INSTR __yield()
#pragma message("Spinlock info: ARM platform detected -> using __yield() instruction while spinning.")
#else
#define SPINLOCK_PAUSE_INSTR (static_cast<void>(0))
// NOLINTNEXTLINE(score-banned-preprocessor-directives)  uppressed here because usage of `#pragma` is forbidden
#pragma message( \
    "Spinlock info: Unknown architecture -> no special pause instruction used while spinning. Maybe provide one?")
#endif

namespace score
{
namespace os
{

Spinlock::Spinlock() noexcept : atomic_lock(false) {}

Spinlock::~Spinlock() noexcept = default;

bool Spinlock::try_lock() noexcept
{
    // First do a relaxed load to check if lock is free in order to prevent
    // unnecessary cache misses if someone does while(!try_lock())
    return (!atomic_lock.load(std::memory_order_relaxed)) && (!atomic_lock.exchange(true, std::memory_order_acquire));
}

void Spinlock::lock() noexcept
{
    for (;;)
    {
        if (!atomic_lock.exchange(true, std::memory_order_acquire))
        {
            break;
        }
        while (atomic_lock.load(std::memory_order_relaxed))
        {
            SPINLOCK_PAUSE_INSTR;
        }
    }
}

void Spinlock::unlock() noexcept
{
    atomic_lock.store(false, std::memory_order_release);
}

}  // namespace os
}  // namespace score
