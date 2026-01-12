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
#include "score/os/cpuid.h"

#if defined(__QNX__) && defined(__x86_64__)
#include <x86_64/inline.h>
#elif defined(__QNX__) && defined(__aarch64__)
#include "score/utility.hpp"
#include <aarch64/inline.h>
#include <arm_acle.h>
#elif defined(__linux__) && defined(__x86_64__)
#include <cpuid.h>
#elif defined(__linux__) && defined(__aarch64__)
#include "score/utility.hpp"
#else
#error "Target platform not supported"
#endif

namespace score
{
namespace os
{

namespace
{

// To test all supported hardware platform and gather the coverage data
// is either impossible or too much effort.
// Suppress until we find a way to do this.
// LCOV_EXCL_START
class CpuIdImpl final : public CpuId
{
  public:
    /* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
    void cpuid(std::uint32_t leaf,
               std::uint32_t& eax,
               std::uint32_t& ebx,
               std::uint32_t& ecx,
               std::uint32_t& edx) const noexcept override
    /* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
    {
#if defined(__QNX__) && defined(__x86_64__)
        x86_cpuid1(leaf, &eax, &ebx, &ecx, &edx);

#elif defined(__QNX__) && defined(__aarch64__)
#if 0  // TODO: Ticket-16842 check why it doesn't work
        std::uint32_t arm_cpuid;
        arm_cpuid = __arm_rsr64("MIDR_EL1"); // Read MIDR_EL1 register
        score::cpp::ignore = leaf;
        score::cpp::ignore = eax;
        ebx = arm_cpuid;
        ecx = arm_cpuid;
        edx = arm_cpuid;
#else
        score::cpp::ignore = leaf;
        eax = ebx = ecx = edx = 0;
#endif

#elif defined(__linux__) && defined(__x86_64__)
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:OS library macros */
        __cpuid(leaf, eax, ebx, ecx, edx);
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:OS library macros */

#elif defined(__linux__) && defined(__aarch64__)
        // ARM64/aarch64 doesn't have CPUID instruction like x86
        // Return dummy values for compatibility
        score::cpp::ignore = leaf;
        eax = ebx = ecx = edx = 0;
#endif
    }
};
// LCOV_EXCL_END

}  // namespace
}  // namespace os
}  // namespace score

score::os::CpuId& score::os::CpuId::instance() noexcept
{
    static score::os::CpuIdImpl implInstance;
    return select_instance(implInstance);
}
