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
#ifndef SCORE_LIB_OS_SELECT_IMPL_H
#define SCORE_LIB_OS_SELECT_IMPL_H

#include "score/os/select.h"

namespace score
{
namespace os
{
class SelectImpl final : public Select
{
  public:
    constexpr SelectImpl() = default;
    /* KW_SUPPRESS_START:MISRA.PTR.TO_PTR_TO_PTR:Used parameters match the param requirements of wrapped function */
    /* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<std::int32_t, score::os::Error> select(const std::int32_t nfds,
                                                                fd_set* const readfds,
                                                                fd_set* const writefds,
                                                                fd_set* const exceptfds,
                                                                struct timeval* const timeout) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
    /* KW_SUPPRESS_END:MISRA.PTR.TO_PTR_TO_PTR:Used parameters match the param requirements of wrapped function */
};
}  // namespace os
}  // namespace score
#endif  // SCORE_LIB_OS_SELECT_IMPL_H
