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
#ifndef SCORE_MW_LOG_NVCONFIGFACTORY_H_
#define SCORE_MW_LOG_NVCONFIGFACTORY_H_

#include "nvconfig.h"

#include "score/json/json_parser.h"
#include "score/result/result.h"

#include <memory>
#include <optional>

namespace score
{
namespace mw
{
namespace log
{

// Error codes for NvConfig creation
enum class NvConfigErrorCode : std::uint8_t
{
    kParseError = 1,    // Failed to parse JSON file
    kContentError = 2,  // JSON content is invalid (missing required fields)
};

// Factory class for creating and initializing NvConfig instances
class NvConfigFactory
{
  public:
    using TypeMap = std::unordered_map<std::string, config::NvMsgDescriptor>;

    // Creates and initializes an NvConfig instance with the given file path
    // Returns Result containing NvConfig on success, or Error on failure
    static score::Result<NvConfig> CreateAndInit(
        const std::string& file_path = "/bmw/platform/opt/datarouter/etc/class-id.json");

    // Creates an empty NvConfig instance with no message descriptors
    // Always succeeds
    static NvConfig CreateEmpty() noexcept;

  private:
    static score::Result<TypeMap> ParseFromJson(const std::string& json_path) noexcept;
    static INvConfig::ReadResult HandleParseResult(const score::json::Object& parse_result,
                                                   NvConfigFactory::TypeMap& typemap) noexcept;
    static config::NvMsgDescriptor GetMsgDescriptor(
        const std::uint32_t id,
        const score::json::Object& object_array_value,
        const score::json::Object::const_iterator& object_appid_iterator,
        const score::json::Object::const_iterator& object_ctxid_iterator) noexcept;
    NvConfigFactory() = delete;  // Static class
};

}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_NVCONFIGFACTORY_H_
