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

#include "score/os/spawn_impl.h"
#include <cstdint>

namespace score
{
namespace os
{

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_init(posix_spawnattr_t* const attrp) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    // Manual code analysis:
    // Failure only happens when insufficient memory exists to initialize the spawn attributes object. There is no way
    // to reliably create an error case in the scope of a unit test. Error is communicated via non-zero return value.
    // In case of an error the error code is provided by the return value of the function call (POSIX 1003.1).
    const std::int32_t result = ::posix_spawnattr_init(attrp);
    if (result != 0)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(Error::createFromErrno());  // LCOV_EXCL_LINE
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_destroy(posix_spawnattr_t* const attrp) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    // Manual code analysis:
    // Failure only happens when the value specified by attrp is invalid. Error is communicated via non-zero return
    // value. In case of an error the error code is provided by the return value of the function call. Negative Test:
    // Results are undefined on calling posix_spawn_file_actions_destroy() on an already destroyed posix_spawnattr_t
    // object. There is no way to reliably create an invalid fact_p in the scope of a unit test (POSIX 1003.1).
    const std::int32_t result = ::posix_spawnattr_destroy(attrp);
    if (result != 0)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(Error::createFromErrno());  // LCOV_EXCL_LINE
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_getflags(const posix_spawnattr_t* const attrp,
                                                                       std::int16_t* const flags_p) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::posix_spawnattr_getflags(attrp, flags_p);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_setflags(posix_spawnattr_t* const attrp,
                                                                       const std::int16_t flags) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::posix_spawnattr_setflags(attrp, flags);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_getsigdefault(const posix_spawnattr_t* const attrp,
                                                                            sigset_t* const sigset_p) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::posix_spawnattr_getsigdefault(attrp, sigset_p);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_setsigdefault(
    posix_spawnattr_t* const attrp,
    const sigset_t* const sigset_p) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::posix_spawnattr_setsigdefault(attrp, sigset_p);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_getsigmask(const posix_spawnattr_t* const attrp,
                                                                         sigset_t* const sigset_p) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::posix_spawnattr_getsigmask(attrp, sigset_p);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_setsigmask(posix_spawnattr_t* const attrp,
                                                                         const sigset_t* const sigset_p) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::posix_spawnattr_setsigmask(attrp, sigset_p);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_getpgroup(const posix_spawnattr_t* const attrp,
                                                                        pid_t* const pid_p) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::posix_spawnattr_getpgroup(attrp, pid_p);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_setpgroup(posix_spawnattr_t* const attrp,
                                                                        const pid_t pid) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::posix_spawnattr_setpgroup(attrp, pid);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_getschedparam(
    const posix_spawnattr_t* const attrp,
    struct sched_param* const sched_p) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::posix_spawnattr_getschedparam(attrp, sched_p);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_setschedparam(
    posix_spawnattr_t* const attrp,
    const struct sched_param* const sched_p) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::posix_spawnattr_setschedparam(attrp, sched_p);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_getschedpolicy(
    const posix_spawnattr_t* const attrp,
    std::int32_t* const policy_p) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::posix_spawnattr_getschedpolicy(attrp, policy_p);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_setschedpolicy(posix_spawnattr_t* const attrp,
                                                                             const std::int32_t policy) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::posix_spawnattr_setschedpolicy(attrp, policy);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawn_file_actions_init(
    posix_spawn_file_actions_t* const fact_p) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::posix_spawn_file_actions_init(fact_p);
    if (result != 0)  // LCOV_EXCL_BR_LINE
    {
        // Manual code analysis:
        // Failure only happens when insufficient memory exists to initialize the spawn attributes object. There is no
        // way to reliably create an error case in the scope of a unit test. Error is communicated via non-zero return
        // value. In case of an error the error code is provided by the return value of the function call.
        return score::cpp::make_unexpected(Error::createFromErrno());  // LCOV_EXCL_LINE
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawn_file_actions_destroy(
    posix_spawn_file_actions_t* const fact_p) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    // Manual code analysis:
    // Failure only happens when the value specified by fact_p is invalid. Error is communicated via non-zero return
    // value. In case of an error the error code is provided by the return value of the function call. Negative Test:
    // Results are undefined when on calling posix_spawn_file_actions_destroy() on an already destroyed
    // posix_spawnattr_t object. There is no way to reliably create an invalid fact_p in the scope of a unit test (POSIX
    // 1003.1).
    const std::int32_t result = ::posix_spawn_file_actions_destroy(fact_p);
    if (result != 0)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(Error::createFromErrno());  // LCOV_EXCL_LINE
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawn_file_actions_addclose(
    posix_spawn_file_actions_t* const fact_p,
    const std::int32_t fd) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::posix_spawn_file_actions_addclose(fact_p, fd);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t* const fact_p,
                                                                               const std::int32_t fd,
                                                                               const std::int32_t new_fd) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::posix_spawn_file_actions_adddup2(fact_p, fd, new_fd);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawn_file_actions_addopen(posix_spawn_file_actions_t* const fact_p,
                                                                               const std::int32_t new_fd,
                                                                               const char* const path,
                                                                               const std::int32_t oflags,
                                                                               const mode_t omode) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::posix_spawn_file_actions_addopen(fact_p, new_fd, path, oflags, omode);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.ARRAY.CSTYLE:Wrapped function requires C-style array param */
// The AUTOSAR Rule A18-1-1 prohibits use of c-style arrays
// Justification: This is a wrapper and the OS function needs C-style arrays
// Changing it might cause issues
score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawn(
    pid_t* const pid,
    const char* const path,
    const posix_spawn_file_actions_t* const file_actions,
    const posix_spawnattr_t* const attrp,
    char* const argv[],                 // NOLINT(modernize-avoid-c-arrays) see comment above
    char* const envp[]) const noexcept  // NOLINT(modernize-avoid-c-arrays) see comment above
/* KW_SUPPRESS_END:AUTOSAR.ARRAY.CSTYLE:Wrapped function requires C-style array param */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::posix_spawn(pid, path, file_actions, attrp, argv, envp);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.ARRAY.CSTYLE:Wrapped function requires C-style array param */
// The AUTOSAR Rule A18-1-1 prohibits use of c-style arrays
// Justification: This is a wrapper and the OS function needs C-style arrays
// Changing it might cause issues
score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnp(
    pid_t* const pid,
    const char* const file,
    const posix_spawn_file_actions_t* const file_actions,
    const posix_spawnattr_t* const attrp,
    char* const argv[],                 // NOLINT(modernize-avoid-c-arrays) see comment above
    char* const envp[]) const noexcept  // NOLINT(modernize-avoid-c-arrays) see comment above
/* KW_SUPPRESS_END:AUTOSAR.ARRAY.CSTYLE:Wrapped function requires C-style array param */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::posix_spawnp(pid, file, file_actions, attrp, argv, envp);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}
// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// Rationale: Pre-processor commands are used to allow different implementations for linux and QNX to exist
// in the same file. It also prevents compiler errors in linux code when compiling for QNX and vice versa.
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__QNX__)
// coverity[autosar_cpp14_a16_0_1_violation] see rationale above
#if __QNX__ >= 800
score::cpp::expected<std::int32_t, Error> SpawnImpl::pthread_spawnattr_getrunmask_np(const posix_spawnattr_t* attrp,
                                                                              std::uint64_t* runmask_p) const noexcept
{
    const std::int32_t result = ::pthread_spawnattr_getrunmask_np(attrp, runmask_p);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

score::cpp::expected<std::int32_t, Error> SpawnImpl::pthread_spawnattr_setrunmask_np(posix_spawnattr_t* attrp,
                                                                              std::uint64_t runmask) const noexcept
{
    const std::int32_t result = ::pthread_spawnattr_setrunmask_np(attrp, runmask);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}
// coverity[autosar_cpp14_a16_0_1_violation] see rationale above
#else
score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_getrunmask(const posix_spawnattr_t* attrp,
                                                                         std::uint32_t* runmask_p) const noexcept
{
    const std::int32_t result = ::posix_spawnattr_getrunmask(attrp, runmask_p);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_setrunmask(posix_spawnattr_t* attrp,
                                                                         std::uint32_t runmask) const noexcept
{
    const std::int32_t result = ::posix_spawnattr_setrunmask(attrp, runmask);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}
// coverity[autosar_cpp14_a16_0_1_violation] see rationale above
#endif
score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_getxflags(const posix_spawnattr_t* attrp,
                                                                        std::uint32_t* flags_p) const noexcept
{
    const std::int32_t result = ::posix_spawnattr_getxflags(attrp, flags_p);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_setxflags(posix_spawnattr_t* attrp,
                                                                        std::uint32_t flags) const noexcept
{
    const std::int32_t result = ::posix_spawnattr_setxflags(attrp, flags);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_getsigignore(const posix_spawnattr_t* attrp,
                                                                           sigset_t* sigset_p) const noexcept
{
    const std::int32_t result = ::posix_spawnattr_getsigignore(attrp, sigset_p);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_setsigignore(posix_spawnattr_t* attrp,
                                                                           const sigset_t* sigset_p) const noexcept
{
    const std::int32_t result = ::posix_spawnattr_setsigignore(attrp, sigset_p);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_getstackmax(const posix_spawnattr_t* attrp,
                                                                          std::uint32_t* size_p) const noexcept
{
    const std::int32_t result = ::posix_spawnattr_getstackmax(attrp, size_p);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_setstackmax(posix_spawnattr_t* attrp,
                                                                          std::uint32_t size) const noexcept
{
    const std::int32_t result = ::posix_spawnattr_setstackmax(attrp, size);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_getnode(const posix_spawnattr_t* attrp,
                                                                      std::uint32_t* node_p) const noexcept
{
    // coverity[autosar_cpp14_a16_0_1_violation] see rationale above
#if (_NTO_VERSION <= 710)
    const std::int32_t result = ::posix_spawnattr_getnode(attrp, node_p);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
    // coverity[autosar_cpp14_a16_0_1_violation] see rationale above
#else
    (void)attrp;
    (void)node_p;
    return score::cpp::make_unexpected(Error::createFromErrno(ENOTSUP));
    // coverity[autosar_cpp14_a16_0_1_violation] see rationale above
#endif
}

score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_setnode(posix_spawnattr_t* attrp,
                                                                      std::uint32_t node) const noexcept
{
    // coverity[autosar_cpp14_a16_0_1_violation] see rationale above
#if (_NTO_VERSION <= 710)
    const std::int32_t result = ::posix_spawnattr_setnode(attrp, node);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
    // coverity[autosar_cpp14_a16_0_1_violation] see rationale above
#else
    (void)attrp;
    (void)node;
    return score::cpp::make_unexpected(Error::createFromErrno(ENOTSUP));
    // coverity[autosar_cpp14_a16_0_1_violation] see rationale above
#endif
}

score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_getcred(const posix_spawnattr_t* attrp,
                                                                      uid_t* uid_p,
                                                                      gid_t* gid_p) const noexcept
{
    const std::int32_t result = ::posix_spawnattr_getcred(attrp, uid_p, gid_p);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_setcred(posix_spawnattr_t* attrp,
                                                                      uid_t uid,
                                                                      gid_t gid) const noexcept
{
    const std::int32_t result = ::posix_spawnattr_setcred(attrp, uid, gid);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_gettypeid(const posix_spawnattr_t* attrp,
                                                                        std::uint32_t* type_id_p) const noexcept
{
    const std::int32_t result = ::posix_spawnattr_gettypeid(attrp, type_id_p);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_settypeid(posix_spawnattr_t* attrp,
                                                                        std::uint32_t type_id) const noexcept
{
    const std::int32_t result = ::posix_spawnattr_settypeid(attrp, type_id);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_setasid(posix_spawnattr_t* attrp,
                                                                      std::uint32_t asid) const noexcept
{
    const std::int32_t result = ::posix_spawnattr_setasid(attrp, asid);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_getaslr(const posix_spawnattr_t* const attrp,
                                                                      posix_spawnattr_aslr_t* aslr) const noexcept
{
    const std::int32_t result = ::posix_spawnattr_getaslr(attrp, aslr);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_setaslr(posix_spawnattr_t* const attrp,
                                                                      const posix_spawnattr_aslr_t aslr) const noexcept
{
    const std::int32_t result = ::posix_spawnattr_setaslr(attrp, aslr);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

score::cpp::expected<std::int32_t, Error> SpawnImpl::posix_spawnattr_setcwd_np(posix_spawnattr_t* const attrp,
                                                                        const std::int32_t dirfd) const noexcept
{
    const std::int32_t result = ::posix_spawnattr_setcwd_np(attrp, dirfd);
    if (result != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

// The AUTOSAR Rule A18-1-1 prohibits use of c-style arrays
// Justification: This is a wrapper and the OS function needs C-style arrays
// Changing it might cause issues
score::cpp::expected<pid_t, Error> SpawnImpl::spawn(
    const char* path,
    std::int32_t fd_count,
    const std::int32_t fd_map[],  // NOLINT(modernize-avoid-c-arrays) see comment above
    const struct inheritance* inherit,
    char* const argv[],                 // NOLINT(modernize-avoid-c-arrays) see comment above
    char* const envp[]) const noexcept  // NOLINT(modernize-avoid-c-arrays) see comment above
{
    const pid_t result = ::spawn(path, fd_count, fd_map, inherit, argv, envp);
    if (result == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

// The AUTOSAR Rule A18-1-1 prohibits use of c-style arrays
// Justification: This is a wrapper and the OS function needs C-style arrays
// Changing it might cause issues
score::cpp::expected<pid_t, Error> SpawnImpl::spawnp(
    const char* file,
    std::int32_t fd_count,
    const std::int32_t fd_map[],  // NOLINT(modernize-avoid-c-arrays) see comment above
    const struct inheritance* inherit,
    char* const argv[],                 // NOLINT(modernize-avoid-c-arrays) see comment above
    char* const envp[]) const noexcept  // NOLINT(modernize-avoid-c-arrays) see comment above
{
    const pid_t result = ::spawnp(file, fd_count, fd_map, inherit, argv, envp);
    if (result == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif  //__QNX__

}  // namespace os
}  // namespace score
