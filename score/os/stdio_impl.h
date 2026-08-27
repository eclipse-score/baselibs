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
#ifndef SCORE_LIB_OS_STDIO_IMPL_H
#define SCORE_LIB_OS_STDIO_IMPL_H

#include "score/os/stdio.h"

namespace score::os
{

class StdioImpl final : public Stdio
{
  public:
    constexpr StdioImpl() = default;

    score::cpp::expected<FILE*, Error> fopen(const char* const filename,
                                             const char* const mode) const noexcept override;

    score::cpp::expected_blank<Error> fclose(FILE* const stream) const noexcept override;

    score::cpp::expected_blank<Error> remove(const char* const pathname) const noexcept override;

    score::cpp::expected_blank<Error> rename(const char* const oldname,
                                             const char* const newname) const noexcept override;

    score::cpp::expected<FILE*, Error> popen(const char* const filename,
                                             const char* const mode) const noexcept override;

    score::cpp::expected<std::int32_t, Error> pclose(FILE* const stream) const noexcept override;

    score::cpp::expected<std::int32_t, Error> fileno(FILE* const stream) const noexcept override;
};

}  // namespace score::os

#endif  // SCORE_LIB_OS_STDIO_IMPL_H
