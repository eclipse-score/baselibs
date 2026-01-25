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
#ifndef SCORE_LIB_OS_SYS_POLL_H
#define SCORE_LIB_OS_SYS_POLL_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"
#include "score/memory.hpp"

#include <cstdint>

#include <sys/poll.h>

namespace score
{
namespace os
{

class SysPoll : public ObjectSeam<SysPoll>
{
  public:
    /// \brief thread-safe singleton accessor
    /// \return Either concrete OS-dependent instance or respective set mock instance
    static SysPoll& instance() noexcept;

    static score::cpp::pmr::unique_ptr<SysPoll> Default(score::cpp::pmr::memory_resource* memory_resource) noexcept;

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    // Justification: The global namespace shall only contain main, namespace declarations and extern ""C""
    // declarations. This is a wrapper function over system API inside local namespace
    // coverity[autosar_cpp14_m7_3_1_violation]
    virtual score::cpp::expected<std::int32_t, Error> poll(struct pollfd* fds,
                                                    const nfds_t nfds,
                                                    const std::int32_t timeout) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    virtual ~SysPoll() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    SysPoll(const SysPoll&) = delete;
    SysPoll& operator=(const SysPoll&) = delete;
    SysPoll(SysPoll&& other) = delete;
    SysPoll& operator=(SysPoll&& other) = delete;

  protected:
    SysPoll() = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_SYS_POLL_H
