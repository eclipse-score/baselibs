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
#include "score/os/sys_poll_impl.h"

namespace score
{
namespace os
{

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
// Justification: The global namespace shall only contain main, namespace declarations and extern ""C"" declarations.
// This is a wrapper function over system API inside local namespace
// coverity[autosar_cpp14_m7_3_1_violation]
score::cpp::expected<std::int32_t, Error> SysPollImpl::poll(struct pollfd* fds,
                                                     const nfds_t nfds,
                                                     const std::int32_t timeout) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    // This is a wrapper over C banned function, thus the suppression is justified.
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const std::int32_t ret{::poll(fds, nfds, timeout)};
    // Manual analysis:
    // As per QNX documentation, the fu function returns error when the allocation of internal data structures fails,
    // a signal is caught during the execution of poll() the fds argument points to a portion of memory that does not
    // exist within the calling process's address space or there is not enough memory available. Which are not possible
    // to cover through unit test. In case of an error ::poll return a value of -1 and set errno to indicate the error.
    if (ret < 0)  // LCOV_EXCL_BR_LINE: Justification provided above
    {
        return score::cpp::make_unexpected(Error::createFromErrno());  // LCOV_EXCL_LINE: Justification provided above
    }
    return ret;
}

}  // namespace os
}  // namespace score
