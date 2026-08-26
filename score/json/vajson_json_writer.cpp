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
#include "score/json/vajson_json_writer.h"
#include "score/json/internal/model/error.h"
#include "score/json/internal/parser/vajson/vajson_impl/vajson_serialize.h"
namespace
{
template <typename T>
score::Result<void> ToFileInternal(const T& json_data,
                                   const std::string_view& file_path,
                                   score::filesystem::IFileFactory& file_factory)
{
    const std::string file_path_string{file_path.data(), file_path.size()};
    const auto file = file_factory.Open(file_path_string, std::ios::out | std::ios::trunc);
    if (!file.has_value())
    {
        return score::Result<void>{score::unexpect,
                                   score::json::MakeError(score::json::Error::kInvalidFilePath, "Failed to open file")};
    }
    score::json::VajsonSerialize serializer{**file};
    return serializer << json_data;
}
template <typename T>
score::Result<std::string> ToBufferInternal(const T& json_data)
{
    return score::json::VajsonToBuffer(json_data);
}
}  // namespace
namespace score
{
namespace json
{
score::Result<void> VajsonJsonWriter::ToFile(const score::json::Object& json_data,
                                             const std::string_view& file_path,
                                             std::shared_ptr<score::filesystem::IFileFactory> file_factory)
{
    return ToFileInternal(json_data, file_path, *file_factory);
}
score::Result<void> VajsonJsonWriter::ToFile(const score::json::List& json_data,
                                             const std::string_view& file_path,
                                             std::shared_ptr<score::filesystem::IFileFactory> file_factory)
{
    return ToFileInternal(json_data, file_path, *file_factory);
}
score::Result<void> VajsonJsonWriter::ToFile(const score::json::Any& json_data,
                                             const std::string_view& file_path,
                                             std::shared_ptr<score::filesystem::IFileFactory> file_factory)
{
    return ToFileInternal(json_data, file_path, *file_factory);
}
score::Result<std::string> VajsonJsonWriter::ToBuffer(const score::json::Object& json_data)
{
    return ToBufferInternal(json_data);
}
score::Result<std::string> VajsonJsonWriter::ToBuffer(const score::json::List& json_data)
{
    return ToBufferInternal(json_data);
}
score::Result<std::string> VajsonJsonWriter::ToBuffer(const score::json::Any& json_data)
{
    return ToBufferInternal(json_data);
}
}  // namespace json
}  // namespace score
