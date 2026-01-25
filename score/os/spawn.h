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
#ifndef SCORE_LIB_OS_SPAWN_H
#define SCORE_LIB_OS_SPAWN_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"

#include <spawn.h>

#if defined(__QNX__)
#include <sys/nto_version.h>
#endif  //__QNX__

namespace score
{
namespace os
{

class Spawn : public ObjectSeam<Spawn>
{
  public:
    static Spawn& instance() noexcept;
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_init(
        posix_spawnattr_t* const attrp) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_destroy(
        posix_spawnattr_t* const attrp) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_getflags(
        const posix_spawnattr_t* const attrp,
        std::int16_t* const flags_p) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_setflags(
        posix_spawnattr_t* const attrp,
        const std::int16_t flags) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_getsigdefault(
        const posix_spawnattr_t* const attrp,
        sigset_t* const sigset_p) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_setsigdefault(
        posix_spawnattr_t* const attrp,
        const sigset_t* const sigset_p) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_getsigmask(
        const posix_spawnattr_t* const attrp,
        sigset_t* const sigset_p) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_setsigmask(
        posix_spawnattr_t* const attrp,
        const sigset_t* const sigset_p) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_getpgroup(
        const posix_spawnattr_t* const attrp,
        pid_t* const pid_p) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_setpgroup(posix_spawnattr_t* const attrp,
                                                                                  const pid_t pid) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_getschedparam(
        const posix_spawnattr_t* const attrp,
        struct sched_param* const sched_p) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_setschedparam(
        posix_spawnattr_t* const attrp,
        const struct sched_param* const sched_p) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_getschedpolicy(
        const posix_spawnattr_t* const attrp,
        std::int32_t* const policy_p) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_setschedpolicy(
        posix_spawnattr_t* const attrp,
        const std::int32_t policy) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawn_file_actions_init(
        posix_spawn_file_actions_t* const fact_p) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawn_file_actions_destroy(
        posix_spawn_file_actions_t* const fact_p) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawn_file_actions_addclose(
        posix_spawn_file_actions_t* const fact_p,
        const std::int32_t fd) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawn_file_actions_adddup2(
        posix_spawn_file_actions_t* const fact_p,
        const std::int32_t fd,
        const std::int32_t new_fd) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawn_file_actions_addopen(
        posix_spawn_file_actions_t* const fact_p,
        const std::int32_t new_fd,
        const char* const path,
        const std::int32_t oflags,
        const mode_t omode) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:AUTOSAR.ARRAY.CSTYLE:Wrapped function's signature requires C-style array */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    // The AUTOSAR Rule A18-1-1 prohibits use of c-style arrays
    // Justification: This is a wrapper and the OS function needs C-style arrays
    // Changing it might cause issues
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawn(
        pid_t* const pid,
        const char* const path,
        const posix_spawn_file_actions_t* const file_actions,
        const posix_spawnattr_t* const attrp,
        char* const argv[],                      // NOLINT(modernize-avoid-c-arrays) see comment above
        char* const envp[]) const noexcept = 0;  // NOLINT(modernize-avoid-c-arrays) see comment above
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_START:AUTOSAR.ARRAY.CSTYLE:Wrapped function's signature requires C-style array */
    // The AUTOSAR Rule A18-1-1 prohibits use of c-style arrays
    // Justification: This is a wrapper and the OS function needs C-style arrays
    // Changing it might cause issues
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnp(
        pid_t* const pid,
        const char* const file,
        const posix_spawn_file_actions_t* const file_actions,
        const posix_spawnattr_t* const attrp,
        char* const argv[],                      // NOLINT(modernize-avoid-c-arrays) see comment above
        char* const envp[]) const noexcept = 0;  // NOLINT(modernize-avoid-c-arrays) see comment above
    /* KW_SUPPRESS_END:AUTOSAR.ARRAY.CSTYLE:Wrapped function's signature requires C-style array */
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// Rationale: Pre-processor commands are used to allow different implementations for linux and QNX to exist
// in the same file. It also prevents compiler errors in linux code when compiling for QNX and vice versa.
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__QNX__)
// coverity[autosar_cpp14_a16_0_1_violation] see rationale above
#if __QNX__ >= 800
    virtual score::cpp::expected<std::int32_t, score::os::Error> pthread_spawnattr_getrunmask_np(
        const posix_spawnattr_t* attrp,
        std::uint64_t* runmask_p) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, score::os::Error> pthread_spawnattr_setrunmask_np(
        posix_spawnattr_t* attrp,
        std::uint64_t runmask) const noexcept = 0;
// coverity[autosar_cpp14_a16_0_1_violation] see rationale above
#else
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_getrunmask(
        const posix_spawnattr_t* attrp,
        std::uint32_t* runmask_p) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_setrunmask(
        posix_spawnattr_t* attrp,
        std::uint32_t runmask) const noexcept = 0;
// coverity[autosar_cpp14_a16_0_1_violation] see rationale above
#endif
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_getxflags(
        const posix_spawnattr_t* attrp,
        std::uint32_t* flags_p) const noexcept = 0;
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_setxflags(
        posix_spawnattr_t* attrp,
        std::uint32_t flags) const noexcept = 0;
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_getsigignore(
        const posix_spawnattr_t* attrp,
        sigset_t* sigset_p) const noexcept = 0;
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_setsigignore(
        posix_spawnattr_t* attrp,
        const sigset_t* sigset_p) const noexcept = 0;
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_getstackmax(
        const posix_spawnattr_t* attrp,
        std::uint32_t* size_p) const noexcept = 0;
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_setstackmax(
        posix_spawnattr_t* attrp,
        std::uint32_t size) const noexcept = 0;
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_getnode(
        const posix_spawnattr_t* attrp,
        std::uint32_t* node_p) const noexcept = 0;
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_setnode(posix_spawnattr_t* attrp,
                                                                                std::uint32_t node) const noexcept = 0;
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_getcred(const posix_spawnattr_t* attrp,
                                                                                uid_t* uid_p,
                                                                                gid_t* gid_p) const noexcept = 0;
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_setcred(posix_spawnattr_t* attrp,
                                                                                uid_t uid,
                                                                                gid_t gid) const noexcept = 0;
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_gettypeid(
        const posix_spawnattr_t* attrp,
        std::uint32_t* type_id_p) const noexcept = 0;
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_settypeid(
        posix_spawnattr_t* attrp,
        std::uint32_t type_id) const noexcept = 0;
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_setasid(posix_spawnattr_t* attrp,
                                                                                std::uint32_t asid) const noexcept = 0;
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_getaslr(
        const posix_spawnattr_t* const attrp,
        posix_spawnattr_aslr_t* aslr) const noexcept = 0;
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_setaslr(
        posix_spawnattr_t* const attrp,
        const posix_spawnattr_aslr_t aslr) const noexcept = 0;
    virtual score::cpp::expected<std::int32_t, score::os::Error> posix_spawnattr_setcwd_np(
        posix_spawnattr_t* const attrp,
        const std::int32_t dirfd) const noexcept = 0;

    // The AUTOSAR Rule A18-1-1 prohibits use of c-style arrays
    // Justification: This is a wrapper and the OS function needs C-style arrays
    // Changing it might cause issues
    virtual score::cpp::expected<pid_t, score::os::Error> spawn(
        const char* path,
        std::int32_t fd_count,
        const std::int32_t fd_map[],  // NOLINT(modernize-avoid-c-arrays) see comment above
        const struct inheritance* inherit,
        char* const argv[],                      // NOLINT(modernize-avoid-c-arrays) see comment above
        char* const envp[]) const noexcept = 0;  // NOLINT(modernize-avoid-c-arrays) see comment above
    virtual score::cpp::expected<pid_t, score::os::Error> spawnp(
        const char* file,
        std::int32_t fd_count,
        const std::int32_t fd_map[],  // NOLINT(modernize-avoid-c-arrays) see comment above
        const struct inheritance* inherit,
        char* const argv[],                      // NOLINT(modernize-avoid-c-arrays) see comment above
        char* const envp[]) const noexcept = 0;  // NOLINT(modernize-avoid-c-arrays) see comment above
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif  //__QNX__

    virtual ~Spawn() = default;

  protected:
    Spawn() = default;
    Spawn(const Spawn&) = default;
    Spawn(Spawn&&) = default;
    Spawn& operator=(const Spawn&) = default;
    Spawn& operator=(Spawn&&) = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_SPAWN_H
