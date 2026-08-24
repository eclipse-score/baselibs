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
#ifndef SCORE_LIB_OS_STDLIB_IMPL_H
#define SCORE_LIB_OS_STDLIB_IMPL_H

#include "score/os/stdlib.h"

/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

namespace score
{
namespace os
{

class StdlibImpl final : public Stdlib
{
  public:
    constexpr StdlibImpl() = default;
    score::cpp::expected_blank<Error> system_call(const std::string& cmd) const noexcept override;
    /* KW_SUPPRESS_START:MISRA.STDLIB.WRONGNAME: Function is wrapped */
    void exit(const int status) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.STDLIB.WRONGNAME: Function is wrapped */
    void quick_exit(const int status) const noexcept override;
    /* KW_SUPPRESS_START:MISRA.STDLIB.WRONGNAME: Function is wrapped */
    char* getenv(const char* const name) const noexcept override;
    int setenv(const char* const name, const char* const value, int overwrite) const noexcept override;
    int unsetenv(const char* const name) const noexcept override;
    Result<void*> calloc(size_t num_of_elements, size_t size) const noexcept override;
    void free(void* ptr) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.STDLIB.WRONGNAME: Function is wrapped */
    Result<char*> realpath(const char* const path, char* const resolved_path) const noexcept override;
    Result<int> mkstemp(char* const path) const noexcept override;
    Result<int> mkstemps(char* const path, const int len) const noexcept override;
};

}  // namespace os
}  // namespace score

/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */

#endif  // SCORE_LIB_OS_STDLIB_IMPL_H
