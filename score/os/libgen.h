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
#ifndef SCORE_LIB_OS_LIBGEN_H
#define SCORE_LIB_OS_LIBGEN_H

#include "score/os/ObjectSeam.h"

#include "score/memory.hpp"

namespace score
{
namespace os
{

class Libgen : public ObjectSeam<Libgen>
{
  public:
    static Libgen& instance() noexcept;

    static score::cpp::pmr::unique_ptr<Libgen> Default(score::cpp::pmr::memory_resource* memory_resource) noexcept;

    virtual char* base_name(char* const path) const noexcept = 0;
    virtual char* dirname(char* const path) const noexcept = 0;

    virtual ~Libgen() = default;
    // Below five member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Libgen(const Libgen&) = delete;
    Libgen& operator=(const Libgen&) = delete;
    Libgen(Libgen&& other) = delete;
    Libgen& operator=(Libgen&& other) = delete;

  protected:
    Libgen() = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_LIBGEN_H
