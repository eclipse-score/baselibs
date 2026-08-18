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
#ifndef SCORE_LIB_OS_STATVFS_IMPL_H
#define SCORE_LIB_OS_STATVFS_IMPL_H

#include "score/os/statvfs.h"

/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */

namespace score
{
namespace os
{

class StatvfsImpl final : public Statvfs
{
  public:
    constexpr StatvfsImpl() = default;
    /* KW_SUPPRESS_START: MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
    score::cpp::expected_blank<Error> statvfs(const char* const path,
                                              struct statvfs* const buf) const noexcept override;
    /* KW_SUPPRESS_END: MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
};

/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */

}  // namespace os
}  // namespace score
#endif  // SCORE_LIB_OS_STATVFS_IMPL_H
