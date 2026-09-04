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
#ifndef SCORE_LIB_OS_SYSLOG_IMPL_H
#define SCORE_LIB_OS_SYSLOG_IMPL_H

#include "score/os/syslog.h"

namespace score
{
namespace os
{

class SyslogImpl : public Syslog
{
  public:
    void openlog(const char* const ident, const std::int32_t option, const std::int32_t facility) const
        noexcept override;

    // coverity[autosar_cpp14_a8_4_1_violation]: see above
    void syslog(const std::int32_t priority, const char* const format, ...) const noexcept override
        __attribute__((format(printf, 3, 4)));

    void closelog() const noexcept override;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_SYSLOG_IMPL_H
