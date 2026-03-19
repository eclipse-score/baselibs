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
#ifndef SCORE_LIB_OS_MMAN_H
#define SCORE_LIB_OS_MMAN_H

#include "score/bitmanipulation/bitmask_operators.h"
#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"
#include "score/os/fcntl.h"
#include "score/os/static_destruction_guard.h"

#include "score/expected.hpp"

#include <cstdint>
#include <type_traits>

#include <sys/mman.h>

/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

namespace score
{
namespace os
{

class Mman : public ObjectSeam<Mman>
{
  public:
    /// \brief Creates a new instance of the production implementation.
    /// \details This is to enable the usage of OSAL without the Singleton instance(). Especially library code
    /// should avoid the Singleton instance() method as this would interfere the unit tests of user code that also uses
    /// the instance().
    static std::unique_ptr<Mman> Default() noexcept;
    /// \brief thread-safe singleton accessor
    /// \return Either concrete OS-dependent instance or respective set mock instance
    static Mman& instance() noexcept;

    static score::cpp::pmr::unique_ptr<Mman> Default(score::cpp::pmr::memory_resource* memory_resource) noexcept;

    enum class Protection : std::int32_t
    {
        kNone = 0,
        kRead = 1,
        kWrite = 2,
        kExec = 4,
        kNoCache = 2048,
    };

    enum class Map : std::int32_t
    {
        kShared = 1,
        kPrivate = 2,
        kFixed = 4,
        kPhys = 65536,
    };
// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// Rationale: Pre-processor commands are used to allow different implementations for POSIX and QNX to exist
// in the same file. It also prevents compiler errors in POSIX code when compiling for QNX and vice versa.
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__EXT_POSIX1_200112)
    enum class PosixTypedMem : std::int32_t
    {
        kAllocate = 1,
        kAllocateContig = 2,
        kMapAllocatable = 4,
    };
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif

    virtual score::cpp::expected<void*, Error> mmap(void* const addr,
                                             const std::size_t length,
                                             const Protection protection,
                                             const Map flags,
                                             const std::int32_t fd,
                                             const std::int64_t offset) const noexcept = 0;

    virtual score::cpp::expected_blank<Error> munmap(void* const addr, const std::size_t length) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, Error> shm_open(const char* const pathname,
                                                        const Fcntl::Open oflag,
                                                        const Stat::Mode mode) const noexcept = 0;

    virtual score::cpp::expected_blank<Error> shm_unlink(const char* const pathname) const noexcept = 0;

// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#if defined(__EXT_POSIX1_200112)
    virtual score::cpp::expected<std::int32_t, Error> posix_typed_mem_open(const char* name,
                                                                    const Fcntl::Open oflag,
                                                                    const PosixTypedMem tflag) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, Error> posix_typed_mem_get_info(
        const std::int32_t fd,
        struct posix_typed_mem_info* info) const noexcept = 0;
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif

    virtual ~Mman() = default;
    // Below five member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Mman(const Mman&) = delete;
    Mman& operator=(const Mman&) = delete;
    Mman(Mman&& other) = delete;
    Mman& operator=(Mman&& other) = delete;

  protected:
    Mman() = default;
};

namespace internal
{

class MmanImpl final : public Mman
{
  public:
    score::cpp::expected<void*, Error> mmap(void* const addr,
                                     const std::size_t length,
                                     const Protection protection,
                                     const Map flags,
                                     const std::int32_t fd,
                                     const std::int64_t offset) const noexcept override;

    score::cpp::expected_blank<Error> munmap(void* const addr, const std::size_t length) const noexcept override;

    score::cpp::expected<std::int32_t, Error> shm_open(const char* const pathname,
                                                const Fcntl::Open oflag,
                                                const Stat::Mode mode) const noexcept override;

    score::cpp::expected_blank<Error> shm_unlink(const char* const pathname) const noexcept override;

// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#if defined(__EXT_POSIX1_200112)
    score::cpp::expected<std::int32_t, Error> posix_typed_mem_open(const char* name,
                                                            const Fcntl::Open oflag,
                                                            const PosixTypedMem tflag) const noexcept override;

    score::cpp::expected<std::int32_t, Error> posix_typed_mem_get_info(
        const std::int32_t fd,
        struct posix_typed_mem_info* info) const noexcept override;
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif

  private: /* KW_SUPPRESS:MISRA.USE.EXPANSION:False postive as private is an access specifier, not macro*/
    std::int32_t ProtectionToInteger(const Protection protection) const noexcept;

    std::int32_t MapFlagsToInteger(const Map flags) const noexcept;

// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#if defined(__EXT_POSIX1_200112)
    std::int32_t PosixTypedMemFlagsToInteger(PosixTypedMem flags) const noexcept;
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif
};

// We have to apply Nifty Counter idiom, since mmap calls are performed in static destructors.
// Suppress "AUTOSAR C++14 A3-1-1", The rule states: "It shall be possible to include any header file
// in multiple translation units without violating the One Definition Rule."
// This is false positive. he static variable "nifty_counter" ensures ODR because of include guard of the header file
// Suppress "AUTOSAR C++14 A2-10-4", The rule states: "The identifier name of a non-member object with
// static storage duration or static function shall not be reused within a namespace."
// nifty_counter is unique and not reused elsewhere in score::os::internal
// Suppress "AUTOSAR C++14 A3-3-2", The rule states: "Static and thread-local objects shall be constant-initialized."
// Justification: templatized static, will be used elsewhere, cannot initialize
// coverity[autosar_cpp14_a3_1_1_violation]
// coverity[autosar_cpp14_a3_3_2_violation]
// coverity[autosar_cpp14_a2_10_4_violation]
static StaticDestructionGuard<MmanImpl> nifty_counter;

}  // namespace internal

/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */

}  // namespace os
}  // namespace score

namespace score
{
template <>
struct enable_bitmask_operators<::score::os::Mman::Protection> : public std::true_type
{
};

template <>
struct enable_bitmask_operators<::score::os::Mman::Map> : public std::true_type
{
};

#if defined(__EXT_POSIX1_200112)
template <>
struct enable_bitmask_operators<::score::os::Mman::PosixTypedMem> : public std::true_type
{
};
#endif
}  // namespace score

#endif  // SCORE_LIB_OS_MMAN_H
