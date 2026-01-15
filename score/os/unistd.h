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
#ifndef SCORE_LIB_OS_UNISTD_H
#define SCORE_LIB_OS_UNISTD_H

#include "score/bitmanipulation/bitmask_operators.h"
#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"
#include "score/os/static_destruction_guard.h"

#include "score/expected.hpp"
#include "score/memory.hpp"

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>  // remove after transition
#include <cstdint>
#include <memory>

namespace score
{
namespace os
{

/// \brief OS-independent abstraction of https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/unistd.h.html
class Unistd : public ObjectSeam<Unistd>
{
  public:
    /// \brief thread-safe singleton accessor
    /// \return Either concrete OS-dependent instance or respective set mock instance
    static Unistd& instance() noexcept;

    /// \brief Creates a new instance of the production implementation.
    /// \details This is to enable the usage of OSAL without the Singleton instance(). Especially library code
    /// should avoid the Singleton instance() method as this would interfere the unit tests of user code that also uses
    /// the instance().
    static std::unique_ptr<Unistd> Default() noexcept;

    static score::cpp::pmr::unique_ptr<Unistd> Default(score::cpp::pmr::memory_resource* memory_resource) noexcept;

    /// \brief Mode declaration for access() system call
    enum class AccessMode : std::int32_t
    {
        kRead = 1,
        kWrite = 2,
        kExec = 4,
        kExists = 8,
    };

    virtual score::cpp::expected_blank<score::os::Error> close(const std::int32_t fd) const noexcept = 0;
    virtual score::cpp::expected_blank<score::os::Error> unlink(const char* const pathname) const noexcept = 0;
    virtual score::cpp::expected_blank<score::os::Error> access(const char* const pathname,
                                                       const AccessMode mode) const noexcept = 0;
    // Wrapped function requires C-style array param
    // NOLINTNEXTLINE(modernize-avoid-c-arrays) see comment above
    virtual score::cpp::expected_blank<score::os::Error> pipe(std::int32_t pipefd[2]) const noexcept = 0;
    virtual score::cpp::expected<std::int32_t, score::os::Error> dup(const std::int32_t oldfd) const noexcept = 0;
    virtual score::cpp::expected<std::int32_t, score::os::Error> dup2(const std::int32_t oldfd,
                                                             const std::int32_t newfd) const noexcept = 0;
    virtual score::cpp::expected<ssize_t, score::os::Error> read(const std::int32_t fd,
                                                        void* const buf,
                                                        const size_t count) const noexcept = 0;
    virtual score::cpp::expected<ssize_t, score::os::Error> pread(const std::int32_t fd,
                                                         void* const buf,
                                                         const size_t count,
                                                         const off_t offset) const noexcept = 0;
    virtual score::cpp::expected<ssize_t, score::os::Error> write(const std::int32_t fd,
                                                         const void* const buf,
                                                         const size_t count) const noexcept = 0;
    virtual score::cpp::expected<ssize_t, score::os::Error> pwrite(const std::int32_t fd,
                                                          const void* const buf,
                                                          const size_t count,
                                                          const off_t offset) const noexcept = 0;
    virtual score::cpp::expected<off_t, score::os::Error> lseek(const std::int32_t fd,
                                                       const off_t offset,
                                                       const std::int32_t whence) const noexcept = 0;
    virtual score::cpp::expected_blank<score::os::Error> ftruncate(const std::int32_t fd, const off_t length) const noexcept = 0;
    virtual pid_t getpid() const noexcept = 0;

    /// @returns the ID of the current thread.
    virtual std::int64_t gettid() const noexcept = 0;

    virtual uid_t getuid() const noexcept = 0;
    virtual gid_t getgid() const noexcept = 0;
    virtual pid_t getppid() const noexcept = 0;
    virtual score::cpp::expected_blank<score::os::Error> setuid(const uid_t uid) const noexcept = 0;
    virtual score::cpp::expected_blank<score::os::Error> setgid(const gid_t gid) const noexcept = 0;

    virtual score::cpp::expected<ssize_t, score::os::Error> readlink(const char* const path,
                                                            char* const buf,
                                                            const size_t bufsize) const noexcept = 0;
    virtual score::cpp::expected_blank<score::os::Error> fsync(const std::int32_t fd) const noexcept = 0;
    virtual score::cpp::expected_blank<score::os::Error> fdatasync(const std::int32_t fd) const noexcept = 0;
    virtual score::cpp::expected_blank<score::os::Error> nanosleep(const struct timespec* const req,
                                                          struct timespec* const rem) const noexcept = 0;
    virtual score::cpp::expected<std::int64_t, score::os::Error> sysconf(const std::int32_t name) const noexcept = 0;
    virtual score::cpp::expected_blank<score::os::Error> link(const char* const oldpath,
                                                     const char* const newpath) const noexcept = 0;
    virtual score::cpp::expected_blank<score::os::Error> symlink(const char* const path1,
                                                        const char* const path2) const noexcept = 0;

    virtual score::cpp::expected_blank<score::os::Error> chdir(const char* const path) const noexcept = 0;

    virtual score::cpp::expected_blank<score::os::Error> chown(const char* const path,
                                                      const uid_t uid,
                                                      const gid_t gid) const noexcept = 0;

    virtual score::cpp::expected<char*, score::os::Error> getcwd(char* const buf, const size_t size) const noexcept = 0;

    virtual std::uint32_t alarm(const std::uint32_t seconds) const noexcept = 0;

    virtual score::cpp::expected_blank<score::os::Error> sync() const noexcept = 0;

    virtual score::cpp::expected_blank<score::os::Error> getpwnam_r(const char* name,
                                                           struct passwd* pwd,
                                                           char* buffer,
                                                           size_t bufsize,
                                                           struct passwd** result) const noexcept = 0;

    virtual ~Unistd() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Unistd(const Unistd&) = delete;
    Unistd& operator=(const Unistd&) = delete;
    Unistd(Unistd&& other) = delete;
    Unistd& operator=(Unistd&& other) = delete;

  protected:
    Unistd() = default;
};

namespace internal
{

class UnistdImpl final : public Unistd
{
  public:
    score::cpp::expected_blank<score::os::Error> close(const std::int32_t fd) const noexcept override;
    score::cpp::expected_blank<score::os::Error> unlink(const char* const pathname) const noexcept override;
    score::cpp::expected_blank<score::os::Error> access(const char* const pathname,
                                               const AccessMode mode) const noexcept override;

    // Wrapped function requires C-style array param
    // NOLINTNEXTLINE(modernize-avoid-c-arrays) see comment above
    score::cpp::expected_blank<score::os::Error> pipe(std::int32_t pipefd[2]) const noexcept override;

    score::cpp::expected<std::int32_t, score::os::Error> dup(const std::int32_t oldfd) const noexcept override;

    score::cpp::expected<std::int32_t, score::os::Error> dup2(const std::int32_t oldfd,
                                                     const std::int32_t newfd) const noexcept override;

    score::cpp::expected<ssize_t, score::os::Error> read(const std::int32_t fd,
                                                void* const buf,
                                                const size_t count) const noexcept override;

    score::cpp::expected<ssize_t, score::os::Error> pread(const std::int32_t fd,
                                                 void* const buf,
                                                 const size_t count,
                                                 const off_t offset) const noexcept override;

    score::cpp::expected<ssize_t, score::os::Error> write(const std::int32_t fd,
                                                 const void* const buf,
                                                 const size_t count) const noexcept override;

    score::cpp::expected<ssize_t, score::os::Error> pwrite(const std::int32_t fd,
                                                  const void* const buf,
                                                  const size_t count,
                                                  const off_t offset) const noexcept override;
    score::cpp::expected<off_t, score::os::Error> lseek(const std::int32_t fd,
                                               const off_t offset,
                                               const std::int32_t whence) const noexcept override;
    score::cpp::expected_blank<score::os::Error> ftruncate(const std::int32_t fd, const off_t length) const noexcept override;

    pid_t getpid() const noexcept override;

    std::int64_t gettid() const noexcept override;
    uid_t getuid() const noexcept override;

    gid_t getgid() const noexcept override;

    pid_t getppid() const noexcept override;

    score::cpp::expected_blank<score::os::Error> setuid(const uid_t uid) const noexcept override;
    score::cpp::expected_blank<score::os::Error> setgid(const gid_t gid) const noexcept override;
    score::cpp::expected<ssize_t, score::os::Error> readlink(const char* const path,
                                                    char* const buf,
                                                    const size_t bufsize) const noexcept override;

    score::cpp::expected_blank<score::os::Error> fsync(const std::int32_t fd) const noexcept override;

    score::cpp::expected_blank<score::os::Error> fdatasync(const std::int32_t fd) const noexcept override;

    score::cpp::expected_blank<score::os::Error> nanosleep(const struct timespec* const req,
                                                  struct timespec* const rem) const noexcept override;

    score::cpp::expected<std::int64_t, score::os::Error> sysconf(const std::int32_t name) const noexcept override;

    score::cpp::expected_blank<score::os::Error> link(const char* const oldpath,
                                             const char* const newpath) const noexcept override;

    score::cpp::expected_blank<score::os::Error> symlink(const char* const path1,
                                                const char* const path2) const noexcept override;

    score::cpp::expected_blank<score::os::Error> chdir(const char* const path) const noexcept override;

    score::cpp::expected_blank<score::os::Error> chown(const char* const path,
                                              const uid_t uid,
                                              const gid_t gid) const noexcept override;

    score::cpp::expected<char*, score::os::Error> getcwd(char* const buf, const size_t size) const noexcept override;

    std::uint32_t alarm(const std::uint32_t seconds) const noexcept override;

    score::cpp::expected_blank<score::os::Error> sync() const noexcept override;

    score::cpp::expected_blank<score::os::Error> getpwnam_r(const char* name,
                                                   struct passwd* pwd,
                                                   char* buffer,
                                                   size_t bufsize,
                                                   struct passwd** result) const noexcept override;
};

}  // namespace internal

// Suppress "AUTOSAR C++14 A3-1-1", The rule states: "It shall be possible to include any header file
// in multiple translation units without violating the One Definition Rule."
// This is false positive. he static variable "nifty_counter" ensures ODR because of include guard of the header file
// Suppress "AUTOSAR C++14 A2-10-4", The rule states: "The identifier name of a non-member object with
// static storage duration or static function shall not be reused within a namespace."
// nifty_counter is unique and not reused elsewhere in score::os
// coverity[autosar_cpp14_a3_1_1_violation]
// coverity[autosar_cpp14_a2_10_4_violation]
static StaticDestructionGuard<internal::UnistdImpl> nifty_counter;

}  // namespace os

template <>
struct enable_bitmask_operators<score::os::Unistd::AccessMode>
{
    // Suppress "AUTOSAR C++14 A0-1-1" rule finding: "A project shall not contain instances of
    // non-volatile variables being given values that are not subsequently used.”.
    // Here, variable value is used as part of templatized struct, hence false positive.
    // coverity[autosar_cpp14_a0_1_1_violation]
    static constexpr bool value{true};
};

}  // namespace score

#endif  // SCORE_LIB_OS_UNISTD_H
