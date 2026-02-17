/********************************************************************************
 * Copyright (c) 2024 Contributors to the Eclipse Foundation
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

///
/// \file
/// \copyright Copyright (c) 2024 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_CPU_RELAX_AARCH64_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_CPU_RELAX_AARCH64_HPP

#include <cstdint>

// "__always_inline__" usable on QNX for ASIL B software. covered by broken_link_c/issue/4049789

namespace score::cpp
{
namespace execution
{
namespace detail
{

/// \brief Provide a hint to the processor that the code sequence is a spin-wait loop
///
/// This can help improve the performance and power consumption of spin-wait loops.
/// https://stackoverflow.com/questions/25189839/cpu-relax-instruction-and-c11-primitives
/// https://stackoverflow.com/questions/47975191/gcc-and-cpu-relax-smb-mb-etc
__attribute__((__always_inline__)) inline void cpu_relax(std::int32_t count) noexcept
{
    while (count-- > 0)
    {
        // `__asm__` stolen from file `target/qnx7/usr/include/aarch64/cpuinline.h` (QNX SDP) as agreed in
        // broken_link_j/Ticket-124589?focusedId=13199881&page=com.atlassian.jira.plugin.system.issuetabpanels%3Acomment-tabpanel#comment-13199881
        __asm__ __volatile__("yield" : : : "memory");
    }
}

/// \brief Returns the maximum count of repeated `cpu_relax` instructions
///
/// The value should be roughly equivalent to the time of a context switch.
__attribute__((__always_inline__)) inline std::int32_t get_platform_specific_cpu_relaxation_count() noexcept
{
    return 256;
}

} // namespace detail
} // namespace execution
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_CPU_RELAX_AARCH64_HPP
