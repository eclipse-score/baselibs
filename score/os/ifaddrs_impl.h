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

#ifndef SCORE_LIB_OS_IFADDRS_IMPL_H
#define SCORE_LIB_OS_IFADDRS_IMPL_H

#include "score/os/ifaddrs.h"
#include <ifaddrs.h>

namespace score
{
namespace os
{

/// @brief Implementation class for the ifaddrs.h module wrapper
class IfaddrsImpl final : public Ifaddrs
{
  public:
    score::cpp::expected<ifaddrs*, Error> getifaddrs() const noexcept override;

    void freeifaddrs(ifaddrs* ifa) const noexcept override;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_IFADDRS_IMPL_H
