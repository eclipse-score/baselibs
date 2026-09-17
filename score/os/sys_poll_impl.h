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
#ifndef SCORE_LIB_OS_SYS_POLL_IMPL_H
#define SCORE_LIB_OS_SYS_POLL_IMPL_H

#include "score/os/sys_poll.h"

namespace score
{
namespace os
{

class SysPollImpl final : public SysPoll
{
  public:
    constexpr SysPollImpl() = default;
    // Justification: The global namespace shall only contain main, namespace declarations and extern ""C""
    // declarations. This is a wrapper function over system API inside local namespace
    // coverity[autosar_cpp14_m7_3_1_violation]
    score::cpp::expected<std::int32_t, Error> poll(struct pollfd* fds,
                                                   const nfds_t nfds,
                                                   const std::int32_t timeout) const noexcept override;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_SYS_POLL_IMPL_H
