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

#ifndef SCORE_LIB_JSON_I_JSON_WRITER_H
#define SCORE_LIB_JSON_I_JSON_WRITER_H

#include "score/filesystem/filestream/i_file_factory.h"
#include "score/json/internal/model/any.h"
#include "score/result/result.h"

#include <memory>

namespace score
{
namespace json
{

class IJsonWriter
{
  public:
    virtual score::ResultBlank ToFile(const score::json::Object& json_data,
                                    const std::string_view& file_path,
                                    std::shared_ptr<score::filesystem::IFileFactory> file_factory) = 0;
    virtual score::ResultBlank ToFile(const score::json::List& json_data,
                                    const std::string_view& file_path,
                                    std::shared_ptr<score::filesystem::IFileFactory> file_factory) = 0;
    virtual score::ResultBlank ToFile(const score::json::Any& json_data,
                                    const std::string_view& file_path,
                                    std::shared_ptr<score::filesystem::IFileFactory> file_factory) = 0;
    virtual score::Result<std::string> ToBuffer(const score::json::Object& json_data) = 0;
    virtual score::Result<std::string> ToBuffer(const score::json::List& json_data) = 0;
    virtual score::Result<std::string> ToBuffer(const score::json::Any& json_data) = 0;
    IJsonWriter() noexcept = default;
    IJsonWriter(const IJsonWriter&) = delete;
    IJsonWriter(IJsonWriter&&) noexcept = delete;
    IJsonWriter& operator=(const IJsonWriter&) = delete;
    IJsonWriter& operator=(IJsonWriter&&) noexcept = delete;
    virtual ~IJsonWriter() noexcept;
};

}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_I_JSON_WRITER_H
