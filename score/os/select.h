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
#ifndef SCORE_LIB_OS_SELECT_H
#define SCORE_LIB_OS_SELECT_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"

#include <sys/time.h>/* KW_SUPPRESS:MISRA.INCL.UNSAFE:time.h libarary needed for wrapper function */
#include <cstdint>

/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
/* KW_SUPPRESS_START:MISRA.PTR.TO_PTR_TO_PTR:Used parameters match the param requirements of wrapped function */

namespace score
{
namespace os
{

/// \brief OS-independent abstraction of https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_select.h.html
class Select : public ObjectSeam<Select>
{
  public:
    /// \brief thread-safe singleton accessor
    /// \return Either concrete OS-dependent instance or respective set mock instance
    static Select& instance() noexcept;
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, score::os::Error> select(
        const std::int32_t nfds,
        fd_set* const readfds,
        fd_set* const writefds,
        fd_set* const exceptfds,
        struct timeval* const timeout) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    virtual ~Select() = default;

  protected:
    Select() = default;
    Select(const Select&) = default;
    Select(Select&&) = default;
    Select& operator=(const Select&) = default;
    Select& operator=(Select&&) = default;
};

/* KW_SUPPRESS_END:MISRA.PTR.TO_PTR_TO_PTR:Used parameters match the param requirements of wrapped function */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_SELECT_H
