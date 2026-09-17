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
#ifndef SCORE_LIB_OS_GETOPT_H
#define SCORE_LIB_OS_GETOPT_H

#include "score/os/ObjectSeam.h"

#include <getopt.h>
#include <unistd.h>
#include <cstdint>

namespace score
{
namespace os
{

class Getopt : public ObjectSeam<Getopt>
{
  public:
    static Getopt& instance() noexcept;
    // Wrapped function's signature requires C-style array
    virtual std::int32_t getopt(const std::int32_t argc,
                                // NOLINTNEXTLINE(modernize-avoid-c-arrays) see comment above
                                char* const argv[],
                                const char* const optstring) const noexcept = 0;
    virtual std::int32_t getoptopt() const noexcept = 0;
    virtual std::int32_t getoptind() const noexcept = 0;
    virtual std::int32_t getopterr() const noexcept = 0;
    // Wrapped function's signature requires C-style array
    virtual std::int32_t getopt_long(const std::int32_t argc,
                                     char* const argv[],  // NOLINT(modernize-avoid-c-arrays) see comment above
                                     const char* const optstring,
                                     const option* const option,
                                     std::int32_t* const index) const noexcept = 0;

    virtual ~Getopt() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Getopt(const Getopt&) = delete;
    Getopt& operator=(const Getopt&) = delete;
    Getopt(Getopt&& other) = delete;
    Getopt& operator=(Getopt&& other) = delete;

  protected:
    Getopt() = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_GETOPT_H
