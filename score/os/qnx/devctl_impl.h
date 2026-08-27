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
#ifndef SCORE_LIB_OS_QNX_DEVCTL_IMPL_H
#define SCORE_LIB_OS_QNX_DEVCTL_IMPL_H

#include "score/os/qnx/devctl.h"

namespace score
{
namespace os
{

class DevctlImpl final : public Devctl
{
  public:
    score::cpp::expected_blank<Error> devctl(const std::int32_t fd,
                                             const std::int32_t dev_cmd,
                                             void* const dev_data_ptr,
                                             const std::size_t n_bytes,
                                             std::int32_t* const dev_info_ptr) const noexcept override;

    score::cpp::expected_blank<Error> devctlv(const std::int32_t fd,
                                              const std::int32_t dev_cmd,
                                              const std::int32_t sparts,
                                              const std::int32_t rparts,
                                              const iovec* const sv,
                                              const iovec* const rv,
                                              std::int32_t* const dev_info_ptr) const noexcept override;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_DEVCTL_IMPL_H
