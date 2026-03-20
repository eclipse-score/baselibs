/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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
#include "score/shm/container_error.h"

namespace score::shm
{

namespace
{

constexpr ContainerErrorDomain g_container_error_domain{};

}  // namespace

score::result::Error MakeError(const ContainerErrorCode code, const std::string_view user_message) noexcept
{
    return {static_cast<score::result::ErrorCode>(code), g_container_error_domain, user_message};
}

}  // namespace score::shm
