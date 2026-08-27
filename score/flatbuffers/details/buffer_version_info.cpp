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

#include "score/flatbuffers/buffer_version_info.hpp"

namespace score
{
namespace flatbuffers
{

std::string_view BufferVersionInfo::identifier() const noexcept
{
    return {identifier_, kIdentifierLength};
}

bool BufferVersionInfo::operator==(const BufferVersionInfo& rhs) const noexcept
{
    return (identifier() == rhs.identifier()) && (major_version == rhs.major_version) &&
           (minor_version == rhs.minor_version);
}

bool BufferVersionInfo::operator!=(const BufferVersionInfo& rhs) const noexcept
{
    return !(*this == rhs);
}

}  // namespace flatbuffers
}  // namespace score
