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
#ifndef SCORE_LIB_OS_QNX_SYSCTL_H
#define SCORE_LIB_OS_QNX_SYSCTL_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"

#include <sys/sysctl.h>
#include <cstdint>

namespace score
{
namespace os
{

///
/// @brief Socket manager control functions OSAL class
/// [QNX sysctl
/// documentation](https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/s/sysctl.html)
///
class Sysctl : public ObjectSeam<Sysctl>
{
  public:
    static Sysctl& instance() noexcept;

    virtual score::cpp::expected_blank<Error> sysctl(std::int32_t* const name,
                                              const std::size_t namelen,
                                              void* const oldp,
                                              std::size_t* const oldlenp,
                                              void* const newp,
                                              const std::size_t newlen) const noexcept = 0;

    virtual score::cpp::expected_blank<Error> sysctlbyname(const char* sname,
                                                    void* const oldp,
                                                    std::size_t* const oldlenp,
                                                    void* const newp,
                                                    const std::size_t newlen) const noexcept = 0;

    virtual score::cpp::expected_blank<Error> sysctlnametomib(const char* sname, int* mibp, size_t* sizep) const noexcept = 0;

    virtual ~Sysctl() = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_SYSCTL_H
