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

#include "score/json/json_writer.h"
#include "score/json/i_json_writer.h"
#include "score/json/internal/model/error.h"
#include "score/json/internal/writer/writer_backend.h"

#include <ios>
#include <string>
#include <string_view>

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
        auto error = score::json::MakeError(score::json::Error::kInvalidFilePath, "Failed to open file");
        return score::Result<void>{score::unexpect, error};
    }

    return score::json::internal::writer::SerializeToStream(**file, json_data);
}

template <typename T>
score::Result<void> ToFileInternalAtomic(const T& json_data,
                                         const std::string_view& file_path,
                                         score::filesystem::IFileFactory& file_factory,
                                         const score::filesystem::AtomicUpdateOwnershipFlags atomic_ownership)

{
    return file_factory.AtomicUpdate(std::string{file_path}, std::ios::out | std::ios::trunc, atomic_ownership)
        .transform_error([](auto err) noexcept {
            return score::json::MakeError(score::json::Error::kInvalidFilePath, err.UserMessage());
        })
        .and_then([&json_data](auto filestream) -> score::Result<void> {
            auto serializer_result = score::json::internal::writer::SerializeToStream(*filestream, json_data);
            return filestream->Close().and_then([serializer_result](auto&&...) noexcept {
                return serializer_result;
            });
        });
}

template <typename T>
score::Result<std::string> ToBufferInternal(const T& json_data)
{
    return score::json::internal::writer::SerializeToBuffer(json_data);
}

}  // namespace

score::json::JsonWriter::JsonWriter(FileSyncMode file_sync_mode,
                                    const score::filesystem::AtomicUpdateOwnershipFlags ownership) noexcept
    : IJsonWriter{}, file_sync_mode_{file_sync_mode}, atomic_ownership_{ownership}
{
}

score::Result<void> score::json::JsonWriter::ToFile(const score::json::Object& json_data,
                                                    const std::string_view& file_path,
                                                    std::shared_ptr<score::filesystem::IFileFactory> file_factory)
{
    return (file_sync_mode_ == FileSyncMode::kSynced)
               ? ToFileInternalAtomic(
                     json_data, std::string_view{file_path.begin(), file_path.size()}, *file_factory, atomic_ownership_)
               : ToFileInternal(json_data, file_path, *file_factory);
}

score::Result<void> score::json::JsonWriter::ToFile(const score::json::List& json_data,
                                                    const std::string_view& file_path,
                                                    std::shared_ptr<score::filesystem::IFileFactory> file_factory)
{
    return (file_sync_mode_ == FileSyncMode::kSynced)
               ? ToFileInternalAtomic(
                     json_data, std::string_view{file_path.begin(), file_path.size()}, *file_factory, atomic_ownership_)
               : ToFileInternal(json_data, file_path, *file_factory);
}

score::Result<void> score::json::JsonWriter::ToFile(const score::json::Any& json_data,
                                                    const std::string_view& file_path,
                                                    std::shared_ptr<score::filesystem::IFileFactory> file_factory)
{
    return (file_sync_mode_ == FileSyncMode::kSynced)
               ? ToFileInternalAtomic(
                     json_data, std::string_view{file_path.begin(), file_path.size()}, *file_factory, atomic_ownership_)
               : ToFileInternal(json_data, file_path, *file_factory);
}

score::Result<std::string> score::json::JsonWriter::ToBuffer(const score::json::Object& json_data)
{
    return ToBufferInternal(json_data);
}

score::Result<std::string> score::json::JsonWriter::ToBuffer(const score::json::List& json_data)
{
    return ToBufferInternal(json_data);
}

score::Result<std::string> score::json::JsonWriter::ToBuffer(const score::json::Any& json_data)
{
    return ToBufferInternal(json_data);
}
