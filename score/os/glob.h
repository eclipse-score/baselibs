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
#ifndef SCORE_LIB_OS_GLOB_H
#define SCORE_LIB_OS_GLOB_H

#include "score/bitmanipulation/bitmask_operators.h"
#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"
#include "score/memory.hpp"

#include <glob.h>
#include <cstdint>
#include <memory>
#include <vector>

namespace score
{
namespace os
{

class Glob : public ObjectSeam<Glob>
{
  public:
    using FlagType = std::uint32_t;

    static std::unique_ptr<Glob> Default() noexcept;
    static score::cpp::pmr::unique_ptr<Glob> Default(score::cpp::pmr::memory_resource* memory_resource) noexcept;

    enum class Flag : FlagType
    {
        kAppend = GLOB_APPEND,
        kDooFFs = GLOB_DOOFFS,
        kErr = GLOB_ERR,
        kMark = GLOB_MARK,
        kNoCheck = GLOB_NOCHECK,
        kNoEscape = GLOB_NOESCAPE,
        kNoSort = GLOB_NOSORT,
        kPeriod = GLOB_PERIOD,
        kMagChar = GLOB_MAGCHAR,
        kAltDirFunc = GLOB_ALTDIRFUNC,
        kBrace = GLOB_BRACE,
        kNoMagic = GLOB_NOMAGIC,
        kTilde = GLOB_TILDE,
// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// Rationale: Pre-processor commands are used to allow different implementations for linux and QNX to exist
// in the same file. It also prevents compiler errors in linux code when compiling for QNX and vice versa.
// coverity[autosar_cpp14_a16_0_1_violation]
#ifdef __linux__
        kTildeCheck = GLOB_TILDE_CHECK,
        kOnlyDir = GLOB_ONLYDIR,
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#else
        kNoDotDir = GLOB_NO_DOTDIRS,
        kLimit = GLOB_LIMIT
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif
    };

    struct MatchResult
    {
        // No harm to define it as public
        // coverity[autosar_cpp14_m11_0_1_violation]
        std::size_t count;
        // No harm to define it as public
        // coverity[autosar_cpp14_m11_0_1_violation]
        std::vector<std::string> paths;
    };

    virtual score::cpp::expected<MatchResult, Error> Match(const std::string& pattern, const Glob::Flag flags) noexcept = 0;
    virtual ~Glob() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Glob() = default;
    Glob(const Glob&) = delete;
    Glob& operator=(const Glob&) = delete;
    Glob(Glob&& other) = delete;
    Glob& operator=(Glob&& other) = delete;
};

namespace internal
{
namespace glob_helper
{

Glob::FlagType FlagToInteger(const score::os::Glob::Flag flags) noexcept;

}  // namespace glob_helper
}  // namespace internal

}  // namespace os
}  // namespace score

namespace score
{
template <>
struct enable_bitmask_operators<::score::os::Glob::Flag> : public std::true_type
{
};
}  // namespace score

#endif  // SCORE_LIB_OS_GLOB_H
