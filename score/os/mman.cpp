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
#include "score/os/mman.h"

#include "score/os/stat.h"

#include <sys/mman.h>
#include <cerrno>
#include <type_traits>

/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
namespace score
{
namespace os
{

namespace internal
{

score::cpp::expected<void*, Error> MmanImpl::mmap(void* const addr,
                                           const std::size_t length,
                                           const Protection protection,
                                           const Map flags,
                                           const std::int32_t fd,
                                           const std::int64_t offset) const noexcept
{
    void* const ret{::mmap(addr, length, ProtectionToInteger(protection), MapFlagsToInteger(flags), fd, offset)};
    /* KW_SUPPRESS_START:AUTOSAR.CAST.CSTYLE:Cast is happening outside our code domain */
    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operations */
    // Suppress "autosar_cpp14_m5_2_9_violation" rule finding. This rule states: "A cast shall not
    // convert a pointer type to an integral type."
    // Rationale: Cast is happening outside our code domain
    // coverity[autosar_cpp14_m5_2_9_violation]
    // coverity[autosar_cpp14_a5_2_2_violation] MAP_FAILED is builtin macro
    if (ret == MAP_FAILED)
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operations */
    /* KW_SUPPRESS_END:AUTOSAR.CAST.CSTYLE:Cast is happening outside our code domain */ {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected_blank<Error> MmanImpl::munmap(void* const addr, const std::size_t length) const noexcept
{
    if (::munmap(addr, length) == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return {};
}

score::cpp::expected<std::int32_t, Error> MmanImpl::shm_open(const char* const pathname,
                                                      const Fcntl::Open oflag,
                                                      const Stat::Mode mode) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const std::int32_t ret{::shm_open(pathname, internal::fcntl_helper::OpenFlagToInteger(oflag), ModeToInteger(mode))};
    if (ret == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected_blank<Error> MmanImpl::shm_unlink(const char* const pathname) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    if (::shm_unlink(pathname) == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return {};
}
// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// Rationale: Pre-processor commands are used to allow different implementations for POSIX and QNX to exist
// in the same file. It also prevents compiler errors in POSIX code when compiling for QNX and vice versa.
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__EXT_POSIX1_200112)
score::cpp::expected<std::int32_t, Error> MmanImpl::posix_typed_mem_open(const char* name,
                                                                  const Fcntl::Open oflag,
                                                                  const PosixTypedMem tflag) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    std::int32_t ret{::posix_typed_mem_open(
        name, internal::fcntl_helper::OpenFlagToInteger(oflag), PosixTypedMemFlagsToInteger(tflag))};
    if (ret == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected<std::int32_t, Error> MmanImpl::posix_typed_mem_get_info(const std::int32_t fd,
                                                                      struct posix_typed_mem_info* info) const noexcept
{
    std::int32_t ret{::posix_typed_mem_get_info(fd, info)};
    if (ret != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif

std::int32_t MmanImpl::ProtectionToInteger(const Protection protection) const noexcept
{
    std::int32_t prot{};
    using utype_protection = std::underlying_type<score::os::Mman::Protection>::type;
    if (static_cast<utype_protection>(protection & Protection::kRead) != 0)
    {
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        // NOLINTBEGIN(hicpp-signed-bitwise): macro does not affect the sign of the result.
        // Suppress "AUTOSAR C++14 M5-0-21" rule findings. This rule declares: "Bitwise operators shall only be
        // applied to operands of unsigned underlying type."
        // Rationale: Macro does not affect the sign of the result
        // coverity[autosar_cpp14_m5_0_21_violation]
        prot |= PROT_READ;
        // NOLINTEND(hicpp-signed-bitwise): macro does not affect the sign of the result.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    }
    if (static_cast<utype_protection>(protection & Protection::kWrite) != 0)
    {
        /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED:Macro does not affect the sign of the result */
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        // NOLINTBEGIN(hicpp-signed-bitwise): macro does not affect the sign of the result.
        // coverity[autosar_cpp14_m5_0_21_violation] macro does not affect the sign of the result.
        prot |= PROT_WRITE;
        // NOLINTEND(hicpp-signed-bitwise): macro does not affect the sign of the result.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED:Macro does not affect the sign of the result */
    }
    if (static_cast<utype_protection>(protection & Protection::kExec) != 0)
    {
        /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED:Macro does not affect the sign of the result */
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        // NOLINTBEGIN(hicpp-signed-bitwise): macro does not affect the sign of the result.
        // coverity[autosar_cpp14_m5_0_21_violation] macro does not affect the sign of the result.
        prot |= PROT_EXEC;
        // NOLINTEND(hicpp-signed-bitwise): macro does not affect the sign of the result.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED:Macro does not affect the sign of the result */
    }
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#if defined(__QNX__)
    if (protection & Protection::kNoCache)
    {
        /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED:Macro does not affect the sign of the result */
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        // NOLINTBEGIN(hicpp-signed-bitwise): macro does not affect the sign of the result.
        // coverity[autosar_cpp14_m5_0_21_violation] macro does not affect the sign of the result.
        prot |= PROT_NOCACHE;
        // NOLINTEND(hicpp-signed-bitwise): macro does not affect the sign of the result.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED:Macro does not affect the sign of the result */
    }
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif
    return prot;
}

std::int32_t MmanImpl::MapFlagsToInteger(const Map flags) const noexcept
{
    std::int32_t map{};
    using utype_map = std::underlying_type<score::os::Mman::Map>::type;
    if (static_cast<utype_map>(flags & Map::kShared) != 0)
    {
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        // NOLINTBEGIN(hicpp-signed-bitwise): macro does not affect the sign of the result.
        // Suppress "AUTOSAR C++14 M5-0-21" rule findings. This rule declares: "Bitwise operators shall only be
        // applied to operands of unsigned underlying type."
        // Rationale: Macro does not affect the sign of the result
        // coverity[autosar_cpp14_m5_0_21_violation]
        map |= MAP_SHARED;
        // NOLINTEND(hicpp-signed-bitwise): macro does not affect the sign of the result.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    }
    if (static_cast<utype_map>(flags & Map::kPrivate) != 0)
    {
        /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED:Macro does not affect the sign of the result */
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        // NOLINTBEGIN(hicpp-signed-bitwise): macro does not affect the sign of the result.
        // coverity[autosar_cpp14_m5_0_21_violation] macro does not affect the sign of the result.
        map |= MAP_PRIVATE;
        // NOLINTEND(hicpp-signed-bitwise): macro does not affect the sign of the result.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED:Macro does not affect the sign of the result */
    }
    if (static_cast<utype_map>(flags & Map::kFixed) != 0)
    {
        /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED:Macro does not affect the sign of the result */
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        // NOLINTBEGIN(hicpp-signed-bitwise): macro does not affect the sign of the result.
        // coverity[autosar_cpp14_m5_0_21_violation] macro does not affect the sign of the result.
        map |= MAP_FIXED;
        // NOLINTEND(hicpp-signed-bitwise): macro does not affect the sign of the result.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED:Macro does not affect the sign of the result */
    }
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#if defined(__QNX__)
    if (flags & Map::kPhys)
    {
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        // NOLINTBEGIN(hicpp-signed-bitwise): macro does not affect the sign of the result.
        // Suppress "AUTOSAR C++14 M5-0-21" rule findings. This rule declares: "Bitwise operators shall only be
        // applied to operands of unsigned underlying type."
        // Rationale: Macro does not affect the sign of the result
        // coverity[autosar_cpp14_m5_0_21_violation]
        map |= MAP_PHYS;
        // NOLINTEND(hicpp-signed-bitwise): macro does not affect the sign of the result.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    }
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif
    return map;
}

// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#if defined(__EXT_POSIX1_200112)
std::int32_t MmanImpl::PosixTypedMemFlagsToInteger(PosixTypedMem flags) const noexcept
{
    std::int32_t posixTypedMem{};
    if (flags & PosixTypedMem::kAllocate)
    {
        /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED:Macro does not affect the sign of the result */
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        // NOLINTBEGIN(hicpp-signed-bitwise): macro does not affect the sign of the result.
        // coverity[autosar_cpp14_m5_0_21_violation] macro does not affect the sign of the result.
        posixTypedMem |= POSIX_TYPED_MEM_ALLOCATE;
        // NOLINTEND(hicpp-signed-bitwise): macro does not affect the sign of the result.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED:Macro does not affect the sign of the result */
    }
    if (flags & PosixTypedMem::kAllocateContig)
    {
        /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED:Macro does not affect the sign of the result */
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        // NOLINTBEGIN(hicpp-signed-bitwise): macro does not affect the sign of the result.
        // coverity[autosar_cpp14_m5_0_21_violation] macro does not affect the sign of the result.
        posixTypedMem |= POSIX_TYPED_MEM_ALLOCATE_CONTIG;
        // NOLINTEND(hicpp-signed-bitwise): macro does not affect the sign of the result.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED:Macro does not affect the sign of the result */
    }
    if (flags & PosixTypedMem::kMapAllocatable)
    {
        /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED:Macro does not affect the sign of the result */
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        // NOLINTBEGIN(hicpp-signed-bitwise): macro does not affect the sign of the result.
        // coverity[autosar_cpp14_m5_0_21_violation] macro does not affect the sign of the result.
        posixTypedMem |= POSIX_TYPED_MEM_MAP_ALLOCATABLE;
        // NOLINTEND(hicpp-signed-bitwise): macro does not affect the sign of the result.
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED:Macro does not affect the sign of the result */
    }

    return posixTypedMem;
}
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif

}  // namespace internal

std::unique_ptr<score::os::Mman> score::os::Mman::Default() noexcept
{
    return std::make_unique<internal::MmanImpl>();
}

/* KW_SUPPRESS_START:MISRA.PPARAM.NEEDS.CONST, MISRA.VAR.NEEDS.CONST: */
/* score::cpp::pmr::make_unique takes non-const memory_resource */
// Justification: The identifier name of a non-member object with static storage duration or
// static function shall not be reused within a namespace.
// static function here is overloaded and used as a wrapper.
// coverity[autosar_cpp14_a2_10_4_violation]
score::cpp::pmr::unique_ptr<score::os::Mman> score::os::Mman::Default(score::cpp::pmr::memory_resource* memory_resource) noexcept
/* KW_SUPPRESS_END:MISRA.PPARAM.NEEDS.CONST, MISRA.VAR.NEEDS.CONST */
{
    return score::cpp::pmr::make_unique<internal::MmanImpl>(memory_resource);
}

score::os::Mman& score::os::Mman::instance() noexcept
{
    // Suppress “AUTOSAR_Cpp14_A5_2_4” rule finding: “Reinterpret_cast shall not be used.”
    // Reinterpret_cast is used here to ensure proper type handling of the underlying storage
    // (StaticDestructionGuard<impl::MmanImpl>::GetStorage()), allowing correct object destruction. This usage is
    // considered safe in this context, as it involves casting an object from static storage with a well-defined type
    // relationship. Despite AUTOSAR A5-2-4 discouraging reinterpret_cast, it is necessary in this specific scenario.
    return select_instance(
        // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast): safe usage of reintrpret_cast
        // coverity[autosar_cpp14_a5_2_4_violation]
        reinterpret_cast<internal::MmanImpl&>(StaticDestructionGuard<internal::MmanImpl>::GetStorage()));
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast): safe usage of reintrpret_cast
}

/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC: Char is used in respect to the wrapped function's signature */

}  // namespace os
}  // namespace score
