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

#include "score/json/internal/writer/writer_backend.h"

#include "score/json/internal/writer/vajson/vajson_serialize.h"

namespace
{

score::json::vajson::VajsonFormatting ToVajsonFormatting(const score::json::Formatting formatting) noexcept
{
    return (formatting == score::json::Formatting::kPrettyPrint) ? score::json::vajson::VajsonFormatting::kPrettyPrint
                                                                 : score::json::vajson::VajsonFormatting::kCompact;
}

template <typename T>
score::Result<void> SerializeToStreamInternal(std::ostream& out_stream,
                                              const T& json_data,
                                              const score::json::Formatting formatting)
{
    score::json::VajsonSerialize serializer{out_stream, ToVajsonFormatting(formatting)};
    return serializer << json_data;
}

}  // namespace

namespace score::json::internal::writer
{

score::Result<void> SerializeToStream(std::ostream& out_stream,
                                      const score::json::Object& json_data,
                                      const score::json::Formatting formatting)
{
    return SerializeToStreamInternal(out_stream, json_data, formatting);
}

score::Result<void> SerializeToStream(std::ostream& out_stream,
                                      const score::json::List& json_data,
                                      const score::json::Formatting formatting)
{
    return SerializeToStreamInternal(out_stream, json_data, formatting);
}

score::Result<void> SerializeToStream(std::ostream& out_stream,
                                      const score::json::Any& json_data,
                                      const score::json::Formatting formatting)
{
    return SerializeToStreamInternal(out_stream, json_data, formatting);
}

score::Result<std::string> SerializeToBuffer(const score::json::Object& json_data,
                                             const score::json::Formatting formatting)
{
    return score::json::VajsonToBuffer(json_data, ToVajsonFormatting(formatting));
}

score::Result<std::string> SerializeToBuffer(const score::json::List& json_data,
                                             const score::json::Formatting formatting)
{
    return score::json::VajsonToBuffer(json_data, ToVajsonFormatting(formatting));
}

score::Result<std::string> SerializeToBuffer(const score::json::Any& json_data,
                                             const score::json::Formatting formatting)
{
    return score::json::VajsonToBuffer(json_data, ToVajsonFormatting(formatting));
}

}  // namespace score::json::internal::writer
