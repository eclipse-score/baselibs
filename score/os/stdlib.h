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
#ifndef SCORE_LIB_OS_STDLIB_H
#define SCORE_LIB_OS_STDLIB_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include <memory>
#include <string>

#include "score/memory.hpp"

/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

namespace score
{
namespace os
{

class Stdlib : public ObjectSeam<Stdlib>
{
  public:
    static Stdlib& instance() noexcept;

    /// \brief Creates a new instance of the production implementation.
    /// \details This is to enable the usage of OSAL without the Singleton instance(). Especially library code
    /// should avoid the Singleton instance() method as this would interfere the unit tests of user code that also uses
    /// the instance().
    static std::unique_ptr<Stdlib> Default() noexcept;

    static score::cpp::pmr::unique_ptr<Stdlib> Default(score::cpp::pmr::memory_resource* memory_resource) noexcept;

    virtual score::cpp::expected_blank<Error> system_call(const std::string& cmd) const noexcept = 0;
    virtual void exit(const int status) const noexcept = 0;
    virtual void quick_exit(const int status) const noexcept = 0;
    virtual char* getenv(const char* const name) const noexcept = 0;
    virtual Result<int> setenv(const char* const name, const char* const value, int overwrite) const noexcept = 0;
    virtual Result<int> unsetenv(const char* const name) const noexcept = 0;
    virtual Result<char*> realpath(const char* const path, char* const resolved_path) const noexcept = 0;
    virtual Result<int> mkstemp(char* const path) const noexcept = 0;
    virtual Result<int> mkstemps(char* const path, const int len) const noexcept = 0;
    virtual Result<void*> calloc(size_t num_of_elements, size_t size) const noexcept = 0;
    virtual void free(void* ptr) const noexcept = 0;

    virtual ~Stdlib() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Stdlib(const Stdlib&) = delete;
    Stdlib& operator=(const Stdlib&) = delete;
    Stdlib(Stdlib&& other) = delete;
    Stdlib& operator=(Stdlib&& other) = delete;

  protected:
    Stdlib() = default;
};

/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_STDLIB_H
