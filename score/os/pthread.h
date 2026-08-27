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
#ifndef SCORE_LIB_OS_PTHREAD_H
#define SCORE_LIB_OS_PTHREAD_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/memory.hpp"

#include <pthread.h>
#include <sys/types.h>
#include <cstddef>
#include <cstdint>
#include <memory>

namespace score
{
namespace os
{

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Shaddowing function name is intended. */
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Use char to keep function signature. */

class Pthread : public ObjectSeam<Pthread>
{
  public:
    /// \brief thread-safe singleton accessor
    /// \return Either concrete OS-dependent instance or respective set mock instance
    static Pthread& instance() noexcept;

    /// \brief Creates a new instance of the production implementation.
    /// \details This is to enable the usage of OSAL without the Singleton instance(). Especially library code
    /// should avoid the Singleton instance() method as this would interfere the unit tests of user code that also uses
    /// the instance().
    static std::unique_ptr<Pthread> Default() noexcept;

    static score::cpp::pmr::unique_ptr<Pthread> Default(score::cpp::pmr::memory_resource* memory_resource) noexcept;

    virtual ~Pthread() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Pthread(const Pthread&) = delete;
    Pthread& operator=(const Pthread&) = delete;
    Pthread(Pthread&& other) = delete;
    Pthread& operator=(Pthread&& other) = delete;

    ///
    /// https://man7.org/linux/man-pages/man3/pthread_self.3p.html
    ///
    virtual pthread_t self();

    ///
    /// https://man7.org/linux/man-pages/man3/pthread_getcpuclockid.3.html
    ///
    virtual score::cpp::expected_blank<Error> getcpuclockid(const pthread_t id, clockid_t* clock_id) const noexcept = 0;

    ///
    /// https://man7.org/linux/man-pages/man3/pthread_setname_np.3.html
    ///
    virtual score::cpp::expected_blank<Error> setname_np(const pthread_t thread,
                                                         const char* const name) const noexcept = 0;

    ///
    /// https://man7.org/linux/man-pages/man3/pthread_setname_np.3.html
    ///
    virtual score::cpp::expected_blank<Error> getname_np(const pthread_t thread,
                                                         char* const name,
                                                         const std::size_t length) const noexcept = 0;
    ///
    /// https://man7.org/linux/man-pages/man3/pthread_condattr_init.3p.html
    ///
    virtual score::cpp::expected_blank<Error> condattr_init(pthread_condattr_t* const attr) const noexcept;

    ///
    /// https://man7.org/linux/man-pages/man3/pthread_condattr_setpshared.3p.html
    ///
    virtual score::cpp::expected_blank<Error> condattr_setpshared(pthread_condattr_t* const attr,
                                                                  const std::int32_t pshared) const noexcept;

    ///
    /// https://man7.org/linux/man-pages/man3/pthread_condattr_destroy.3p.html
    ///
    virtual score::cpp::expected_blank<Error> condattr_destroy(pthread_condattr_t* const attr) const noexcept;

    ///
    /// https://man7.org/linux/man-pages/man3/pthread_cond_init.3p.html
    ///
    virtual score::cpp::expected_blank<Error> cond_init(pthread_cond_t* const cond,
                                                        const pthread_condattr_t* const attr) const noexcept;

    ///
    /// https://man7.org/linux/man-pages/man3/pthread_cond_init.3p.html
    ///
    virtual score::cpp::expected_blank<Error> cond_destroy(pthread_cond_t* const cond) const noexcept;

    ///
    /// https://man7.org/linux/man-pages/man3/pthread_mutexattr_init.3.html
    ///
    virtual score::cpp::expected_blank<Error> mutexattr_init(pthread_mutexattr_t* const attr) const noexcept;

    ///
    /// https://man7.org/linux/man-pages/man3/pthread_mutexattr_setpshared.3p.html
    ///
    virtual score::cpp::expected_blank<Error> mutexattr_setpshared(pthread_mutexattr_t* const attr,
                                                                   const std::int32_t pshared) const noexcept;

    ///
    /// https://man7.org/linux/man-pages/man3/pthread_mutexattr_init.3.html
    ///
    virtual score::cpp::expected_blank<Error> mutexattr_destroy(pthread_mutexattr_t* const attr) const noexcept;

    ///
    /// https://man7.org/linux/man-pages/man3/pthread_mutex_init.3p.html
    ///
    virtual score::cpp::expected_blank<Error> mutex_init(pthread_mutex_t* const mutex,
                                                         const pthread_mutexattr_t* const attr) const noexcept;

    ///
    /// https://man7.org/linux/man-pages/man3/pthread_mutex_destroy.3p.html
    ///
    virtual score::cpp::expected_blank<Error> mutex_destroy(pthread_mutex_t* const mutex) const noexcept;

    ///
    /// https://man7.org/linux/man-pages/man3/pthread_setschedparam.3.html
    ///
    virtual score::cpp::expected_blank<score::os::Error> pthread_setschedparam(
        const pthread_t thread,
        const std::int32_t policy,
        const struct sched_param* const param) const noexcept;

    ///
    /// https://man7.org/linux/man-pages/man3/pthread_setschedparam.3.html
    ///
    virtual score::cpp::expected_blank<score::os::Error> pthread_getschedparam(
        const pthread_t thread,
        std::int32_t* const policy,
        struct sched_param* const param) const noexcept;

  protected:
    Pthread() = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_PTHREAD_H
