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
#ifndef SCORE_LIB_JSON_I_JSON_WRITER_MOCK_H
#define SCORE_LIB_JSON_I_JSON_WRITER_MOCK_H

#include "score/json/i_json_writer.h"

#include <gmock/gmock.h>

namespace score
{
namespace json
{

class IJsonWriterMock : public IJsonWriter
{
  public:
    MOCK_METHOD(score::ResultBlank,
                ToFile,
                (const score::json::Object& json_data,
                 const std::string_view& file_path,
                 std::shared_ptr<score::filesystem::IFileFactory> file_factory),
                (override));
    MOCK_METHOD(score::ResultBlank,
                ToFile,
                (const score::json::List& json_data,
                 const std::string_view& file_path,
                 std::shared_ptr<score::filesystem::IFileFactory> file_factory),
                (override));
    MOCK_METHOD(score::ResultBlank,
                ToFile,
                (const score::json::Any& json_data,
                 const std::string_view& file_path,
                 std::shared_ptr<score::filesystem::IFileFactory> file_factory),
                (override));
    MOCK_METHOD(score::Result<std::string>, ToBuffer, (const score::json::Object& json_data), (override));
    MOCK_METHOD(score::Result<std::string>, ToBuffer, (const score::json::List& json_data), (override));
    MOCK_METHOD(score::Result<std::string>, ToBuffer, (const score::json::Any& json_data), (override));
};

}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_I_JSON_WRITER_MOCK_H
