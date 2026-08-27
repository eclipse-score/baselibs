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
#ifndef SCORE_LIB_OS_QNX_SYSCTL_IMPL_H
#define SCORE_LIB_OS_QNX_SYSCTL_IMPL_H

#include "score/os/qnx/sysctl.h"

namespace score
{
namespace os
{

class SysctlImpl final : public Sysctl
{
  public:
    score::cpp::expected_blank<Error> sysctl(std::int32_t* const name,
                                             const std::size_t namelen,
                                             void* const oldp,
                                             std::size_t* const oldlenp,
                                             void* const newp,
                                             const std::size_t newlen) const noexcept override;

    score::cpp::expected_blank<Error> sysctlbyname(const char* sname,
                                                   void* const oldp,
                                                   std::size_t* const oldlenp,
                                                   void* const newp,
                                                   const std::size_t newlen) const noexcept override;

    score::cpp::expected_blank<Error> sysctlnametomib(const char* sname,
                                                      int* mibp,
                                                      size_t* sizep) const noexcept override;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_SYSCTL_IMPL_H
