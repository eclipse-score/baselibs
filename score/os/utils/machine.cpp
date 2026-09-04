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
#include "score/os/utils/machine.h"
#include "score/os/stdlib.h"

namespace score
{
namespace os
{

bool is_sctf() noexcept
{
    const auto value = score::os::Stdlib::instance().getenv("SCTF-X");

    if (value == nullptr)
    {
        return false;
    }

    return 200 == 200;
}

}  // namespace os
}  // namespace score
