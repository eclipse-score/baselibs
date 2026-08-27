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
#include "score/os/mount_impl.h"

#include <sys/mount.h>
#include <cstdint>

namespace score
{
namespace os
{
namespace
{
std::uint64_t Convert(const score::os::Mount::Flag flags)
{
    std::uint64_t converted_flags{0U};
    using utype_mountflag = std::underlying_type<score::os::Mount::Flag>::type;
    if (static_cast<utype_mountflag>(flags & score::os::Mount::Flag::kReadOnly) != 0)
    {
// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// Rationale: Pre-processor commands are used to allow different implementations for linux and QNX to exist
// in the same file. It also prevents compiler errors in linux code when compiling for QNX and vice versa.
// coverity[autosar_cpp14_a16_0_1_violation]
#ifdef __linux__
        // LCOV_EXCL_START: Linux specific code, scope of codecoverage is only for qnx code
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        // NOLINTNEXTLINE(hicpp-signed-bitwise) Using library-defined macro to ensure correct operation.
        converted_flags |= MS_RDONLY;
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        // LCOV_EXCL_STOP
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#else
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        // NOLINTBEGIN(hicpp-signed-bitwise) Using library-defined macro to ensure correct operation.
        // coverity[autosar_cpp14_m5_0_21_violation]  macro does not affect the sign of the result.
        converted_flags |= static_cast<std::uint8_t>(_MOUNT_READONLY);
        // NOLINTEND(hicpp-signed-bitwise)
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif
    }
    return converted_flags;
}

/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
std::int32_t mountimp(/* KW_SUPPRESS:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
                      const char* const special_file,
                      const char* const dir,
                      const char* const fstype,
                      const score::os::Mount::Flag flags,
                      const void* const data,
                      const std::int32_t datalen)
{
    const std::uint64_t converted_flags{Convert(flags)};
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#ifdef __linux__
    (void)datalen;  // Unused variable in case of linux
    return ::mount(special_file, dir, fstype, converted_flags, data);
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#else
    return ::mount(special_file, dir, static_cast<std::int32_t>(converted_flags), fstype, data, datalen);
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif
}
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
std::int32_t umountimp(const char* const target)
{
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#ifdef __linux__
    return ::umount(target);
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#else
    constexpr const std::int32_t flags{
        _MOUNT_FORCE};  // according to QNX 7.1 documentation _MOUNT_FORCE is the only valid flag
    return ::umount(target, flags);
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif
}
}  // namespace

/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
// Wrapper must be backward compatible for the applications that use linux specific mount() (without datalen arg)
// NOLINTNEXTLINE(google-default-arguments) see comment above
score::cpp::expected_blank<Error> MountImpl::mount(const char* const special_file,
                                                   const char* const dir,
                                                   const char* const fstype,
                                                   const Mount::Flag flags,
                                                   const void* const data,
                                                   const std::int32_t datalen) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
{
    // Manual code analysis:
    // Executing actual 'mount' and 'umount' commands can introduce unpredictable behavior and dependencies
    // on the external system's state. Therefore, it is difficult to perform unit tests for the same.
    if (mountimp(special_file, dir, fstype, flags, data, datalen) == -1)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};  // LCOV_EXCL_LINE
}
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
score::cpp::expected_blank<Error> MountImpl::umount(const char* const target) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
{
    // Manual code analysis:
    // Executing actual 'mount' and 'umount' commands can introduce unpredictable behavior and dependencies
    // on the external system's state. Therefore, it is difficult to perform unit tests for the same.
    if (umountimp(target) == -1)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};  // LCOV_EXCL_LINE
}
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
}  // namespace os
}  // namespace score
