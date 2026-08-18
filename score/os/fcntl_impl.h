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
#ifndef SCORE_LIB_OS_FCNTL_IMPL_H
#define SCORE_LIB_OS_FCNTL_IMPL_H

#include "score/os/fcntl.h"

namespace score::os
{

class FcntlImpl final : public Fcntl
{
  public:
    constexpr FcntlImpl() = default;
    score::cpp::expected_blank<Error> fcntl(const std::int32_t fd,
                                            const Fcntl::Command command,
                                            const Fcntl::Open flags) const noexcept override;

    score::cpp::expected<Fcntl::Open, Error> fcntl(const std::int32_t fd,
                                                   const Fcntl::Command command) const noexcept override;

    score::cpp::expected<std::int32_t, Error> open(const char* const pathname,
                                                   const Open flags) const noexcept override;
    score::cpp::expected<std::int32_t, Error> open(const char* const pathname,
                                                   const Open flags,
                                                   const Stat::Mode mode) const noexcept override;

    score::cpp::expected_blank<Error> posix_fallocate(const std::int32_t fd,
                                                      const off_t offset,
                                                      const off_t len) const noexcept override;

    score::cpp::expected_blank<Error> flock(const std::int32_t filedes, const Operation op) const noexcept override;
};

}  // namespace score::os

#endif  // SCORE_LIB_OS_FCNTL_IMPL_H
