/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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
#ifndef SCORE_LIB_OS_QNX_SLOG_IMPL_H
#define SCORE_LIB_OS_QNX_SLOG_IMPL_H

#include "score/os/qnx/slog.h"

namespace score
{
namespace os
{
namespace qnx
{

class SlogImpl : public Slog
{
  public:
    // coverity[autosar_cpp14_a8_4_1_violation]: see above
    score::cpp::expected<std::int32_t, score::os::Error> slogf(const SlogCode code,
                                                               const SlogSeverity severity,
                                                               const char* const format,
                                                               ...) const noexcept override
        __attribute__((format(printf, 4, 5)));
};

}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_SLOG_IMPL_H
