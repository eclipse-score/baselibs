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
#ifndef SCORE_LIB_OS_INTERFACE_FTW_H
#define SCORE_LIB_OS_INTERFACE_FTW_H

#include "score/os/errno.h"

#include "score/expected.hpp"

#include <sys/stat.h>

/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */

namespace score
{
namespace os
{

class Ftw
{
  public:
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> ftw(const char* const path,
                                                                     std::int32_t (*const fn)(const char* const fname,
                                                                                              const struct stat* sbuf,
                                                                                              std::int32_t flag),
                                                                     const std::int32_t ndirs) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    virtual ~Ftw() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Ftw(const Ftw&) = delete;
    Ftw& operator=(const Ftw&) = delete;
    Ftw(Ftw&& other) = delete;
    Ftw& operator=(Ftw&& other) = delete;

  protected:
    Ftw() = default;
};

}  // namespace os
}  // namespace score

/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */

#endif  // SCORE_LIB_OS_INTERFACE_FTW_H
