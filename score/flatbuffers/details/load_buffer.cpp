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

#include "score/flatbuffers/load_buffer.hpp"
#include "score/flatbuffers/details/load_buffer_internal.hpp"

namespace score
{

namespace flatbuffers
{

score::os::Result<std::vector<uint8_t>> LoadBuffer(const score::filesystem::Path& path) noexcept
{
    std::vector<uint8_t> data;
    const auto read_result = detail::LoadBufferImpl(detail::OS{}, path, data);
    if (read_result.has_value())
    {
        return std::move(data);
    }
    return score::cpp::make_unexpected(read_result.error());
}

score::os::Result<score::cpp::blank> LoadBuffer(const score::filesystem::Path& path,
                                                std::pmr::vector<uint8_t>& data) noexcept
{
    return detail::LoadBufferImpl(detail::OS{}, path, data);
}

}  // namespace flatbuffers
}  // namespace score
