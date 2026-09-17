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
#ifndef SCORE_LIB_OS_LIBGEN_IMPL_H
#define SCORE_LIB_OS_LIBGEN_IMPL_H

#include "score/os/libgen.h"
#include <libgen.h>

namespace score
{
namespace os
{

class LibgenImpl final : public Libgen
{
  public:
    constexpr LibgenImpl() = default;

  private:
    char* base_name(char* const path) const noexcept override;

    char* dirname(char* const path) const noexcept override;
};

}  // namespace os
}  // namespace score

#endif
