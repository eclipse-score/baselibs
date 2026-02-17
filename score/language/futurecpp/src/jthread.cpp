/********************************************************************************
 * Copyright (c) 2024 Contributors to the Eclipse Foundation
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

///
/// \file
/// \copyright Copyright (c) 2024 Contributors to the Eclipse Foundation
///

#include <score/jthread.hpp>

#include <unistd.h>

namespace score::cpp
{

unsigned int jthread::hardware_concurrency() noexcept
{
    const long result{::sysconf(_SC_NPROCESSORS_ONLN)};
    if (result < 0)
    {
        return 0U;
    }
    return static_cast<unsigned int>(result);
}

} // namespace score::cpp
