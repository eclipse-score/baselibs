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

#ifndef SCORE_LIB_JSON_VAJSON_JSON_WRITER_H
#define SCORE_LIB_JSON_VAJSON_JSON_WRITER_H

#include "score/json/i_json_writer.h"

#include <string_view>

namespace score
{
namespace json
{

/// @brief Implementation of IJsonWriter using the vaJSON serializer (VajsonSerialize).
class VajsonJsonWriter final : public IJsonWriter
{
  public:
    VajsonJsonWriter() noexcept = default;
    VajsonJsonWriter(const VajsonJsonWriter&) = delete;
    VajsonJsonWriter(VajsonJsonWriter&&) noexcept = delete;
    VajsonJsonWriter& operator=(const VajsonJsonWriter&) = delete;
    VajsonJsonWriter& operator=(VajsonJsonWriter&&) noexcept = delete;
    ~VajsonJsonWriter() noexcept override = default;

    score::Result<void> ToFile(const score::json::Object& json_data,
                               const std::string_view& file_path,
                               std::shared_ptr<score::filesystem::IFileFactory> file_factory) override;

    score::Result<void> ToFile(const score::json::List& json_data,
                               const std::string_view& file_path,
                               std::shared_ptr<score::filesystem::IFileFactory> file_factory) override;

    score::Result<void> ToFile(const score::json::Any& json_data,
                               const std::string_view& file_path,
                               std::shared_ptr<score::filesystem::IFileFactory> file_factory) override;

    score::Result<std::string> ToBuffer(const score::json::Object& json_data) override;
    score::Result<std::string> ToBuffer(const score::json::List& json_data) override;
    score::Result<std::string> ToBuffer(const score::json::Any& json_data) override;
};

}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_VAJSON_JSON_WRITER_H
