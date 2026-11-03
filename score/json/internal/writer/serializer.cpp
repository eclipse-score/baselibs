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

#include "score/json/internal/writer/serializer.h"

#include "score/json/internal/model/error.h"
#include "score/json/internal/writer/vajson/any_serializer.h"

#include <functional>
#include <sstream>
#include <type_traits>
#include <utility>

namespace
{

template <typename T>
score::Result<void> SerializeToStreamInternal(std::ostream& out_stream,
                                              const T& json_data,
                                              const score::json::Formatting formatting)
{
    score::json::vajson::FormattingWriter writer{out_stream, formatting};
    auto serializer = score::json::vajson::DocumentSerializer{std::ref(writer)};
    if constexpr (std::is_same_v<T, score::json::Object>)
    {
        static_cast<void>(score::json::internal::writer::vajson::SerializeObject(std::move(serializer), json_data));
    }
    else if constexpr (std::is_same_v<T, score::json::List>)
    {
        static_cast<void>(score::json::internal::writer::vajson::SerializeList(std::move(serializer), json_data));
    }
    else
    {
        static_cast<void>(score::json::internal::writer::vajson::SerializeValue(std::move(serializer), json_data));
    }

    // A default constructed Result<void> already carries the success state.
    score::Result<void> result{};
    if (out_stream.fail())
    {
        result =
            score::MakeUnexpected(score::json::Error::kUnknownError, "vaJSON serializer failed to write to stream");
    }

    return result;
}

template <typename T>
score::Result<std::string> SerializeToBufferInternal(const T& json_data, const score::json::Formatting formatting)
{
    std::ostringstream out_stream{};

    // and_then keeps a single exit point: the buffer is only extracted once serialization succeeded,
    // and any error is forwarded unchanged.
    return SerializeToStreamInternal(out_stream, json_data, formatting)
        .and_then([&out_stream](auto&&...) -> score::Result<std::string> {
            return out_stream.str();
        });
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
    return SerializeToBufferInternal(json_data, formatting);
}

score::Result<std::string> SerializeToBuffer(const score::json::List& json_data,
                                             const score::json::Formatting formatting)
{
    return SerializeToBufferInternal(json_data, formatting);
}

score::Result<std::string> SerializeToBuffer(const score::json::Any& json_data,
                                             const score::json::Formatting formatting)
{
    return SerializeToBufferInternal(json_data, formatting);
}

}  // namespace score::json::internal::writer
