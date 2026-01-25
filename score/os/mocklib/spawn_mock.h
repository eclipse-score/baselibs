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
#ifndef SCORE_LIB_OS_MOCKLIB_SPAWN_MOCK_H
#define SCORE_LIB_OS_MOCKLIB_SPAWN_MOCK_H

#include "score/os/spawn.h"

#include <gmock/gmock.h>

namespace score
{
namespace os
{

class SpawnMock : public Spawn
{
  public:
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_init,
                (posix_spawnattr_t * attrp),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_destroy,
                (posix_spawnattr_t * attrp),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_getflags,
                (const posix_spawnattr_t* attrp, short* flags_p),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_setflags,
                (posix_spawnattr_t * attrp, short flags),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_getsigdefault,
                (const posix_spawnattr_t* attrp, sigset_t* sigset_p),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_setsigdefault,
                (posix_spawnattr_t * attrp, const sigset_t* sigset_p),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_getsigmask,
                (const posix_spawnattr_t* attrp, sigset_t* sigset_p),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_setsigmask,
                (posix_spawnattr_t * attrp, const sigset_t* sigset_p),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_getpgroup,
                (const posix_spawnattr_t* attrp, pid_t* pid_p),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_setpgroup,
                (posix_spawnattr_t * attrp, pid_t pid),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_getschedparam,
                (const posix_spawnattr_t* attrp, struct sched_param* sched_p),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_setschedparam,
                (posix_spawnattr_t * attrp, const struct sched_param* sched_p),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_getschedpolicy,
                (const posix_spawnattr_t* attrp, std::int32_t* policy_p),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_setschedpolicy,
                (posix_spawnattr_t * attrp, std::int32_t policy),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawn_file_actions_init,
                (posix_spawn_file_actions_t * fact_p),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawn_file_actions_destroy,
                (posix_spawn_file_actions_t * fact_p),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawn_file_actions_addclose,
                (posix_spawn_file_actions_t * fact_p, std::int32_t fd),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawn_file_actions_adddup2,
                (posix_spawn_file_actions_t * fact_p, std::int32_t fd, std::int32_t new_fd),
                (override, const, noexcept));
    MOCK_METHOD(
        (score::cpp::expected<std::int32_t, score::os::Error>),
        posix_spawn_file_actions_addopen,
        (posix_spawn_file_actions_t * fact_p, std::int32_t new_fd, const char* path, std::int32_t oflags, mode_t omode),
        (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawn,
                (pid_t * pid,
                 const char* path,
                 const posix_spawn_file_actions_t* file_actions,
                 const posix_spawnattr_t* attrp,
                 char* const argv[],
                 char* const envp[]),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnp,
                (pid_t * pid,
                 const char* file,
                 const posix_spawn_file_actions_t* file_actions,
                 const posix_spawnattr_t* attrp,
                 char* const argv[],
                 char* const envp[]),
                (override, const, noexcept));
// coverity[autosar_cpp14_a16_0_1_violation] see rationale above
#if defined(__QNX__)
// coverity[autosar_cpp14_a16_0_1_violation] see rationale above
#if __QNX__ >= 800
    // QNX 8 ONLY APIs (native pthread runmask API)
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                pthread_spawnattr_getrunmask_np,
                (const posix_spawnattr_t* attrp, std::uint64_t* runmask_p),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                pthread_spawnattr_setrunmask_np,
                (posix_spawnattr_t * attrp, std::uint64_t runmask),
                (override, const, noexcept));
// coverity[autosar_cpp14_a16_0_1_violation] see rationale above
#else
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_getrunmask,
                (const posix_spawnattr_t* attrp, std::uint32_t* runmask_p),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_setrunmask,
                (posix_spawnattr_t * attrp, std::uint32_t runmask),
                (override, const, noexcept));
// coverity[autosar_cpp14_a16_0_1_violation] see rationale above
#endif  // __QNX__ >= 800
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_getxflags,
                (const posix_spawnattr_t* attrp, std::uint32_t* flags_p),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_setxflags,
                (posix_spawnattr_t * attrp, std::uint32_t flags),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_getsigignore,
                (const posix_spawnattr_t* attrp, sigset_t* sigset_p),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_setsigignore,
                (posix_spawnattr_t * attrp, const sigset_t* sigset_p),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_getstackmax,
                (const posix_spawnattr_t* attrp, std::uint32_t* size_p),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_setstackmax,
                (posix_spawnattr_t * attrp, std::uint32_t size),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_getnode,
                (const posix_spawnattr_t* attrp, std::uint32_t* node_p),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_setnode,
                (posix_spawnattr_t * attrp, std::uint32_t node),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_getcred,
                (const posix_spawnattr_t* attrp, uid_t* uid_p, gid_t* gid_p),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_setcred,
                (posix_spawnattr_t * attrp, uid_t uid, gid_t gid),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_gettypeid,
                (const posix_spawnattr_t* attrp, std::uint32_t* type_id_p),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_settypeid,
                (posix_spawnattr_t * attrp, std::uint32_t type_id),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_setasid,
                (posix_spawnattr_t * attrp, std::uint32_t asid),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_getaslr,
                (const posix_spawnattr_t* const attrp, posix_spawnattr_aslr_t* aslr),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_setaslr,
                (posix_spawnattr_t* const attrp, const posix_spawnattr_aslr_t aslr),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                posix_spawnattr_setcwd_np,
                (posix_spawnattr_t* const attrp, const std::int32_t dirfd),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<pid_t, score::os::Error>),
                spawn,
                (const char* path,
                 std::int32_t fd_count,
                 const std::int32_t fd_map[],
                 const struct inheritance* inherit,
                 char* const argv[],
                 char* const envp[]),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<pid_t, score::os::Error>),
                spawnp,
                (const char* file,
                 std::int32_t fd_count,
                 const std::int32_t fd_map[],
                 const struct inheritance* inherit,
                 char* const argv[],
                 char* const envp[]),
                (override, const, noexcept));
// coverity[autosar_cpp14_a16_0_1_violation] see rationale above
#endif  //__QNX__
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_SPAWN_MOCK_H
