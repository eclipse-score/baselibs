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
#include "score/json/internal/writer/vajson/vajson_serialize.h"

#include <functional>
#include <sstream>
#include <utility>
namespace
{
template <typename T>
score::Result<void> SerializeToStream(std::ostream& out_stream, const T& json_data)
{
    auto serializer = score::json::vajson::DocumentSerializer{std::ref(out_stream)};
    static_cast<void>(std::move(serializer) << json_data);

    // A default constructed Result<void> already carries the success state.
    score::Result<void> result{};
    if (out_stream.fail())
    {
        result = score::Result<void>{
            score::unexpect,
            score::json::MakeError(score::json::Error::kUnknownError, "vaJSON serializer failed to write to stream")};
    }

    return result;
}
template <typename T>
score::Result<std::string> SerializeToBuffer(const T& json_data)
{
    std::ostringstream out_stream{};

    // and_then keeps a single exit point: the buffer is only extracted once serialization succeeded,
    // and any error is forwarded unchanged.
    return SerializeToStream(out_stream, json_data).and_then([&out_stream](auto&&...) -> score::Result<std::string> {
        return out_stream.str();
    });
}
}  // namespace
namespace score
{
namespace json
{
VajsonSerialize::VajsonSerialize(std::ostream& out_stream) noexcept : out_stream_{out_stream} {}
score::Result<void> VajsonSerialize::operator<<(const score::json::Object& json_data)
{
    return SerializeToStream(out_stream_, json_data);
}
score::Result<void> VajsonSerialize::operator<<(const score::json::List& json_data)
{
    return SerializeToStream(out_stream_, json_data);
}
score::Result<void> VajsonSerialize::operator<<(const score::json::Any& json_data)
{
    return SerializeToStream(out_stream_, json_data);
}
score::Result<std::string> VajsonToBuffer(const score::json::Object& json_data)
{
    return SerializeToBuffer(json_data);
}
score::Result<std::string> VajsonToBuffer(const score::json::List& json_data)
{
    return SerializeToBuffer(json_data);
}
score::Result<std::string> VajsonToBuffer(const score::json::Any& json_data)
{
    return SerializeToBuffer(json_data);
}
}  // namespace json
}  // namespace score
