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
#include "score/os/unistd.h"

#if defined(__QNX__)
#include <process.h>
#elif defined(__linux__)
#include <sys/syscall.h>
#include <unistd.h>
#endif  // __QNX__

score::cpp::expected_blank<score::os::Error> score::os::internal::UnistdImpl::close(const std::int32_t fd) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    if (::close(fd) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected_blank<score::os::Error> score::os::internal::UnistdImpl::unlink(const char* const pathname) const noexcept
{
    if (::unlink(pathname) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected_blank<score::os::Error> score::os::internal::UnistdImpl::access(const char* const pathname,
                                                                          const AccessMode mode) const noexcept
{
    std::uint32_t native_mode{};
    if (static_cast<std::int32_t>(mode & Unistd::AccessMode::kRead) != 0)
    {
        native_mode |= static_cast<std::uint32_t>(R_OK);
    }
    if (static_cast<std::int32_t>(mode & Unistd::AccessMode::kWrite) != 0)
    {
        native_mode |= static_cast<std::uint32_t>(W_OK);
    }
    if (static_cast<std::int32_t>(mode & Unistd::AccessMode::kExec) != 0)
    {
        native_mode |= static_cast<std::uint32_t>(X_OK);
    }
    if (static_cast<std::int32_t>(mode & Unistd::AccessMode::kExists) != 0)
    {
        native_mode |= static_cast<std::uint32_t>(F_OK);
    }

    if (::access(pathname, static_cast<int>(native_mode)) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

// Wrapped function requires C-style array param
// NOLINTNEXTLINE(modernize-avoid-c-arrays) see comment above
score::cpp::expected_blank<score::os::Error> score::os::internal::UnistdImpl::pipe(std::int32_t pipefd[2]) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    /* It is immposible to cover the return statement */
    /* Hence, added suppression */
    // NOLINTNEXTLINE(score-banned-function) see comment above
    if (::pipe(pipefd) == -1)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno()); /* LCOV_EXCL_LINE */
        /*Error can not be reproduced in scope of unit tests. Incorrect value is prohibited, causes undefined behavior*/
    }
    return {};
}

score::cpp::expected<std::int32_t, score::os::Error> score::os::internal::UnistdImpl::dup(const std::int32_t oldfd) const noexcept
{
    const std::int32_t output = ::dup(oldfd);
    if (output == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return output;
}

score::cpp::expected<std::int32_t, score::os::Error> score::os::internal::UnistdImpl::dup2(const std::int32_t oldfd,
                                                                                const std::int32_t newfd) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const std::int32_t output = ::dup2(oldfd, newfd);
    if (output == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return output;
}

score::cpp::expected<ssize_t, score::os::Error> score::os::internal::UnistdImpl::read(const std::int32_t fd,
                                                                           void* const buf,
                                                                           const size_t count) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const ssize_t output = ::read(fd, buf, count);
    if (output == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return output;
}

score::cpp::expected<ssize_t, score::os::Error> score::os::internal::UnistdImpl::pread(const std::int32_t fd,
                                                                            void* const buf,
                                                                            const size_t count,
                                                                            const off_t offset) const noexcept
{
    const ssize_t output = ::pread(fd, buf, count, offset);
    if (output == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return output;
}

score::cpp::expected<ssize_t, score::os::Error> score::os::internal::UnistdImpl::write(const std::int32_t fd,
                                                                            const void* const buf,
                                                                            const size_t count) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const ssize_t output = ::write(fd, buf, count);
    if (output == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return output;
}

score::cpp::expected<ssize_t, score::os::Error> score::os::internal::UnistdImpl::pwrite(const std::int32_t fd,
                                                                             const void* const buf,
                                                                             const size_t count,
                                                                             const off_t offset) const noexcept
{
    const ssize_t output = ::pwrite(fd, buf, count, offset);
    if (output == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return output;
}

score::cpp::expected<off_t, score::os::Error> score::os::internal::UnistdImpl::lseek(const std::int32_t fd,
                                                                          const off_t offset,
                                                                          const std::int32_t whence) const noexcept
{
    const off_t output = ::lseek(fd, offset, whence);
    if (output == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return output;
}

score::cpp::expected_blank<score::os::Error> score::os::internal::UnistdImpl::ftruncate(const std::int32_t fd,
                                                                             const off_t length) const noexcept
{
    if (::ftruncate(fd, length) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

pid_t score::os::internal::UnistdImpl::getpid() const noexcept
{
    return ::getpid();
}

std::int64_t score::os::internal::UnistdImpl::gettid() const noexcept
{
// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// Rationale: Pre-processor commands are used to allow different implementations for linux and QNX to exist
// in the same file. It also prevents compiler errors in linux code when compiling for QNX and vice versa.
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__QNX__)
    return static_cast<std::int64_t>(::gettid());
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#elif defined(__linux__)
    // Can't use gettid() because our glibc version < 2.30.
    // https://man7.org/linux/man-pages/man2/gettid.2.html#VERSIONS
    return ::syscall(SYS_gettid);
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif  // __QNX__
}

uid_t score::os::internal::UnistdImpl::getuid() const noexcept
{
    return ::getuid();
}

gid_t score::os::internal::UnistdImpl::getgid() const noexcept
{
    return ::getgid();
}

pid_t score::os::internal::UnistdImpl::getppid() const noexcept
{
    return ::getppid();
}

score::cpp::expected_blank<score::os::Error> score::os::internal::UnistdImpl::setuid(const uid_t uid) const noexcept
{
    /* It is immposible to cover the return statement */
    /* Hence, added suppression */
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    if (::setuid(uid) == -1)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    /* This path is covered within QNX tests, but not for Linux: score/os/test/qnx/unistd_test.cpp */

    /* A test case is written to achieve the positive scenario of setuid using ForkAndExpectTrue. */
    /* As ForkAndExpectTrue executes the verification of positive scenario in a child process, */
    /* LCOV  fails to get the  consolidated coverage */

    return {};  // LCOV_EXCL_LINE
}

score::cpp::expected_blank<score::os::Error> score::os::internal::UnistdImpl::setgid(const gid_t gid) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    if (::setgid(gid) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    /* This path is covered within QNX tests, but not for Linux: score/os/test/qnx/unistd_test.cpp */
    return {};
}

score::cpp::expected<ssize_t, score::os::Error> score::os::internal::UnistdImpl::readlink(const char* const path,
                                                                               char* const buf,
                                                                               const size_t bufsize) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const ssize_t output = ::readlink(path, buf, bufsize);
    if (output == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return output;
}

score::cpp::expected_blank<score::os::Error> score::os::internal::UnistdImpl::fsync(const std::int32_t fd) const noexcept
{
    if (::fsync(fd) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected_blank<score::os::Error> score::os::internal::UnistdImpl::fdatasync(const std::int32_t fd) const noexcept
{
    if (::fdatasync(fd) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected_blank<score::os::Error> score::os::internal::UnistdImpl::nanosleep(const struct timespec* const req,
                                                                             struct timespec* const rem) const noexcept
{
    if (::nanosleep(req, rem) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected<std::int64_t, score::os::Error> score::os::internal::UnistdImpl::sysconf(
    const std::int32_t name) const noexcept
{
    const std::int64_t output = ::sysconf(name);
    if (output == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return output;
}

score::cpp::expected_blank<score::os::Error> score::os::internal::UnistdImpl::link(const char* const oldpath,
                                                                        const char* const newpath) const noexcept
{
    if (::link(oldpath, newpath) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected_blank<score::os::Error> score::os::internal::UnistdImpl::symlink(const char* const path1,
                                                                           const char* const path2) const noexcept
{
    if (::symlink(path1, path2) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected_blank<score::os::Error> score::os::internal::UnistdImpl::chdir(const char* const path) const noexcept
{
    if (::chdir(path) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected_blank<score::os::Error> score::os::internal::UnistdImpl::chown(const char* const path,
                                                                         const uid_t uid,
                                                                         const gid_t gid) const noexcept
{
    if (0 != ::chown(path, uid, gid))
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::cpp::expected<char*, score::os::Error> score::os::internal::UnistdImpl::getcwd(char* const buf,
                                                                           const size_t size) const noexcept
{
    char* const result = ::getcwd(buf, size);
    if (nullptr == result)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}

std::uint32_t score::os::internal::UnistdImpl::alarm(const std::uint32_t seconds) const noexcept
{
    return ::alarm(seconds);
}

score::cpp::expected_blank<score::os::Error> score::os::internal::UnistdImpl::getpwnam_r(const char* name,
                                                                              struct passwd* pwd,
                                                                              char* buffer,
                                                                              size_t bufsize,
                                                                              struct passwd** result) const noexcept
{
    if (0 != ::getpwnam_r(name, pwd, buffer, bufsize, result))
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

score::os::Unistd& score::os::Unistd::instance() noexcept
{
    return select_instance(
        // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast) see rationale below
        // Suppress “AUTOSAR_Cpp14_A5_2_4” rule finding: “Reinterpret_cast shall not be used.”
        // Rationale: Have to use reinterpret_cast here because of the use of GetStorage
        // coverity[autosar_cpp14_a5_2_4_violation]
        reinterpret_cast<internal::UnistdImpl&>(StaticDestructionGuard<internal::UnistdImpl>::GetStorage())
        // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
    );
}

std::unique_ptr<score::os::Unistd> score::os::Unistd::Default() noexcept
{
    return std::make_unique<internal::UnistdImpl>();
}

score::cpp::pmr::unique_ptr<score::os::Unistd> score::os::Unistd::Default(score::cpp::pmr::memory_resource* memory_resource) noexcept
{
    return score::cpp::pmr::make_unique<internal::UnistdImpl>(memory_resource);
}

score::cpp::expected_blank<score::os::Error> score::os::internal::UnistdImpl::sync() const noexcept
{
    ::sync();
    return {};
}
