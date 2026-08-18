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
#ifndef SCORE_LIB_OS_STDIO_H
#define SCORE_LIB_OS_STDIO_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"

namespace score::os
{

class Stdio : public ObjectSeam<Stdio>
{
  public:
    static Stdio& instance() noexcept;

    virtual score::cpp::expected<FILE*, Error> fopen(const char* const filename,
                                                     const char* const mode) const noexcept = 0;

    virtual score::cpp::expected_blank<Error> fclose(FILE* const stream) const noexcept = 0;

    virtual score::cpp::expected_blank<Error> remove(const char* const pathname) const noexcept = 0;

    virtual score::cpp::expected_blank<Error> rename(const char* const oldname,
                                                     const char* const newname) const noexcept = 0;

    virtual score::cpp::expected<FILE*, Error> popen(const char* const filename,
                                                     const char* const mode) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, Error> pclose(FILE* const stream) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, Error> fileno(FILE* const stream) const noexcept = 0;

    virtual ~Stdio() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Stdio(const Stdio&) = delete;
    Stdio& operator=(const Stdio&) = delete;
    Stdio(Stdio&& other) = delete;
    Stdio& operator=(Stdio&& other) = delete;

  protected:
    Stdio() = default;
};

}  // namespace score::os

#endif  // SCORE_LIB_OS_STDIO_H
