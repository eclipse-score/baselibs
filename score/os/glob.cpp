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
#include "score/os/glob.h"
#include "score/os/glob_impl.h"

namespace score
{
namespace os
{

std::unique_ptr<Glob> Glob::Default() noexcept
{
    return std::make_unique<GlobImpl>();
}

/* score::cpp::pmr::make_unique takes non-const memory_resource */
score::cpp::pmr::unique_ptr<Glob> Glob::Default(score::cpp::pmr::memory_resource* memory_resource) noexcept
{
    return score::cpp::pmr::make_unique<GlobImpl>(memory_resource);
}

Glob::FlagType internal::glob_helper::FlagToInteger(const Glob::Flag flags) noexcept
{
    using Flag = score::os::Glob::Flag;
    Glob::FlagType combined_flag = 0U;
    // Suppress "AUTOSAR C++14 A0-1-1" rule finding. This rule states: "A project shall not contain instances of
    // non-volatile variables being given values that are not subsequently used".
    // The variable flags_count is used here for std::array signature declaration.
    // coverity[autosar_cpp14_a0_1_1_violation]
    constexpr uint8_t flags_count = 15U;

    constexpr std::array<std::pair<Flag, Glob::FlagType>, flags_count> flag_map = {
        {{Flag::kAppend, GLOB_APPEND},
         {Flag::kDooFFs, GLOB_DOOFFS},
         {Flag::kErr, GLOB_ERR},
         {Flag::kMark, GLOB_MARK},
         {Flag::kNoCheck, GLOB_NOCHECK},
         {Flag::kNoEscape, GLOB_NOESCAPE},
         {Flag::kNoSort, GLOB_NOSORT},
         {Flag::kPeriod, GLOB_PERIOD},
         {Flag::kMagChar, GLOB_MAGCHAR},
         {Flag::kAltDirFunc, GLOB_ALTDIRFUNC},
         {Flag::kBrace, GLOB_BRACE},
         {Flag::kNoMagic, GLOB_NOMAGIC},
         {Flag::kTilde, GLOB_TILDE},
// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// Rationale: Pre-processor commands are used to allow different implementations for linux and QNX to exist
// in the same file. It also prevents compiler errors in linux code when compiling for QNX and vice versa.
// coverity[autosar_cpp14_a16_0_1_violation]
#ifdef __linux__
         {Flag::kTildeCheck, GLOB_TILDE_CHECK},
         {Flag::kOnlyDir, GLOB_ONLYDIR}
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#else
         {Flag::kNoDotDir, GLOB_NO_DOTDIRS},
         {Flag::kLimit, GLOB_LIMIT}
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif
        }};

    for (const auto& [enum_value, glob_value] : flag_map)
    {
        if ((static_cast<Glob::FlagType>(flags) & static_cast<Glob::FlagType>(enum_value)) != 0U)
        {
            combined_flag |= glob_value;
        }
    }

    return combined_flag;
}

}  // namespace os
}  // namespace score
