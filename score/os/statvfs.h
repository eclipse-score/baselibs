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
#ifndef SCORE_LIB_OS_STATVFS_H
#define SCORE_LIB_OS_STATVFS_H

#include "score/bitmanipulation/bitmask_operators.h"
#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"

#include <sys/statvfs.h>
#include <sys/types.h>
#include <cstdint>

/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */

namespace score
{
namespace os
{

class Statvfs : public ObjectSeam<Statvfs>
{
  public:
    static Statvfs& instance() noexcept;
    /* KW_SUPPRESS_START: MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected_blank<Error> statvfs(const char* const path,
                                                      struct statvfs* const buf) const noexcept = 0;
    /* KW_SUPPRESS_END: MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
    virtual ~Statvfs() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Statvfs(const Statvfs&) = delete;
    Statvfs& operator=(const Statvfs&) = delete;
    Statvfs(Statvfs&& other) = delete;
    Statvfs& operator=(Statvfs&& other) = delete;

  protected:
    Statvfs() = default;
};

/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */

}  // namespace os
}  // namespace score
#endif  // SCORE_LIB_OS_STATVFS_H
