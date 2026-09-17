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
#ifndef SCORE_LIB_OS_POSIX_FTW_H
#define SCORE_LIB_OS_POSIX_FTW_H

#include "score/os/interface/ftw.h"

namespace score
{
namespace os
{

class FtwPosix : public Ftw
{
  public:
    score::cpp::expected<std::int32_t, score::os::Error> ftw(const char* const path,
                                                             std::int32_t (*const fn)(const char* const fname,
                                                                                      const struct stat* sbuf,
                                                                                      std::int32_t flag),
                                                             const std::int32_t ndirs) const noexcept override;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_POSIX_FTW_H
