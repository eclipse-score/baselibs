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
#include "score/os/qnx/slog2_impl.h"

namespace score
{
namespace os
{
namespace qnx
{

/* score::cpp::pmr::make_unique takes non-const memory_resource */
score::cpp::pmr::unique_ptr<Slog2> Slog2::Default(score::cpp::pmr::memory_resource* memory_resource) noexcept
{
    return score::cpp::pmr::make_unique<Slog2Impl>(memory_resource);
}

}  // namespace qnx
}  // namespace os
}  // namespace score
