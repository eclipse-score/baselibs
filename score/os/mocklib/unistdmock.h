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
#ifndef SCORE_LIB_OS_MOCKLIB_UNISTDMOCK_H
#define SCORE_LIB_OS_MOCKLIB_UNISTDMOCK_H

#include "score/os/unistd.h"

#include <gmock/gmock.h>
#include <cstddef>

namespace score
{
namespace os
{

class UnistdMock : public Unistd
{
  public:
    MOCK_METHOD(score::cpp::expected_blank<score::os::Error>, close, (const std::int32_t fd), (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<score::os::Error>, unlink, (const char* pathname), (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<score::os::Error>,
                access,
                (const char* pathname, AccessMode mode),
                (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<score::os::Error>, pipe, (std::int32_t pipefd[2]), (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                dup,
                (const std::int32_t oldfd),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                dup2,
                (const std::int32_t oldfd, const std::int32_t newfd),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<ssize_t, score::os::Error>),
                read,
                (const std::int32_t fd, void* buf, const size_t count),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<ssize_t, score::os::Error>),
                pread,
                (const std::int32_t fd, void* buf, const size_t count, const off_t offset),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<ssize_t, score::os::Error>),
                write,
                (const std::int32_t fd, const void* buf, const size_t count),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<ssize_t, score::os::Error>),
                pwrite,
                (const std::int32_t fd, const void* buf, const size_t count, const off_t offset),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<off_t, score::os::Error>),
                lseek,
                (const std::int32_t fd, const off_t offset, const std::int32_t whence),
                (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<score::os::Error>,
                ftruncate,
                (const std::int32_t fd, const off_t length),
                (const, noexcept, override));

    MOCK_METHOD(pid_t, getpid, (), (const, noexcept, override));

    MOCK_METHOD((std::int64_t), gettid, (), (const, noexcept, override));

    MOCK_METHOD(uid_t, getuid, (), (const, noexcept, override));

    MOCK_METHOD(gid_t, getgid, (), (const, noexcept, override));

    MOCK_METHOD(pid_t, getppid, (), (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<score::os::Error>, setuid, (const uid_t uid), (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<score::os::Error>, setgid, (const gid_t gid), (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<ssize_t, score::os::Error>),
                readlink,
                (const char* path, char* buf, const size_t bufsize),
                (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<score::os::Error>, fsync, (const std::int32_t fd), (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<score::os::Error>, fdatasync, (const std::int32_t fd), (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<score::os::Error>,
                nanosleep,
                (const struct timespec* req, struct timespec* rem),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<std::int64_t, score::os::Error>),
                sysconf,
                (const std::int32_t name),
                (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<score::os::Error>,
                link,
                (const char* oldpath, const char* newpath),
                (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<score::os::Error>,
                symlink,
                (const char* path1, const char* path2),
                (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<score::os::Error>, chdir, (const char* path), (const, noexcept, override));
    MOCK_METHOD(score::cpp::expected_blank<score::os::Error>,
                chown,
                (const char* path, uid_t uid, gid_t gid),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<char*, score::os::Error>), getcwd, (char* buf, size_t size), (const, noexcept, override));

    MOCK_METHOD((std::uint32_t), alarm, (std::uint32_t), (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<score::os::Error>, sync, (), (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                getpwnam_r,
                (const char* name, struct passwd* pwd, char* buffer, size_t bufsize, struct passwd** result),
                (const, noexcept, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_UNISTDMOCK_H
