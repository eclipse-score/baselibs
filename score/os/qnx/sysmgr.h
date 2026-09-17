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
#ifndef SCORE_LIB_OS_QNX_SYSMGR_H
#define SCORE_LIB_OS_QNX_SYSMGR_H

#include "score/os/errno.h"

#include "score/expected.hpp"

#include <sys/sysmgr.h>

namespace score
{
namespace os
{
namespace qnx
{

class SysMgr
{
  public:
    virtual score::cpp::expected<std::int32_t, score::os::Error> sysmgr_reboot() const noexcept = 0;

    virtual ~SysMgr() = default;
};

}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_SYSMGR_H
