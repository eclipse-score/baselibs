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

template <typename T>
score::Result<void> SerializeToStreamInternal(std::ostream& out_stream, const T& json_data)
{
    score::json::VajsonSerialize serializer{out_stream};
    return serializer << json_data;
}

}  // namespace

namespace score
{
namespace json
{
namespace internal
{
namespace writer
{

score::Result<void> SerializeToStream(std::ostream& out_stream, const score::json::Object& json_data)
{
    return SerializeToStreamInternal(out_stream, json_data);
}

score::Result<void> SerializeToStream(std::ostream& out_stream, const score::json::List& json_data)
{
    return SerializeToStreamInternal(out_stream, json_data);
}

score::Result<void> SerializeToStream(std::ostream& out_stream, const score::json::Any& json_data)
{
    return SerializeToStreamInternal(out_stream, json_data);
}

score::Result<std::string> SerializeToBuffer(const score::json::Object& json_data)
{
    return score::json::VajsonToBuffer(json_data);
}

score::Result<std::string> SerializeToBuffer(const score::json::List& json_data)
{
    return score::json::VajsonToBuffer(json_data);
}

score::Result<std::string> SerializeToBuffer(const score::json::Any& json_data)
{
    return score::json::VajsonToBuffer(json_data);
}

}  // namespace writer
}  // namespace internal
}  // namespace json
}  // namespace score
