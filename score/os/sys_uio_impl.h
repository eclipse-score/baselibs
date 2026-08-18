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
#ifndef SCORE_LIB_OS_SYS_UIO_IMPL_H
#define SCORE_LIB_OS_SYS_UIO_IMPL_H

#include "score/os/sys_uio.h"

namespace score
{
namespace os
{

class SysUioImpl final : public SysUio
{
  public:
    score::cpp::expected<std::int64_t, Error> writev(const std::int32_t fd,
                                                     const struct iovec* iovec_ptr,
                                                     const std::int32_t count) const noexcept override;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_SYS_UIO_IMPL_H
