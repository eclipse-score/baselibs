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

#ifndef SCORE_LIB_JSON_JSON_WRITER_H
#define SCORE_LIB_JSON_JSON_WRITER_H

#include "score/filesystem/filestream/file_factory.h"
#include "score/json/i_json_writer.h"

#include <string_view>

namespace score
{
namespace json
{

enum class FileSyncMode
{
    kSynced,
    kUnsynced,
};

class JsonWriter final : public IJsonWriter
{
  public:
    /** @brief There are two usage modes: kUnsynced (default) updates the files directly,
     *         whereas kSynced will create a temporary file first, what guarantees
     *         that either the file will be completely replaced with the new contents
     *         or no changes will be seen if the operation fails or is interrupted
     *         (say, due to a power loss). Therefore, this mode is also referred to as
     *         `Atomic Update`.
     *
     *  When using the kSynced option, if the process creating the temporary file
     *  does not have the same UID/GID of the original file, one could end up with
     *  a different ownership for the resulting file.
     *  To avoid that, there's the extra parameter `ownership`,
     *  that can be set to specify what properties have to be kept.
     *
     *  Notice that changing the UID of the temporary file to reflect the one from
     *  the original file would require the process to have the privilege of executing
     *  chown operations. As that's usually not the case, it's advised that, if the
     *  user has this use-case where the ownership will differ, then all processes
     *  involved into updating the file shall share the same extra group, and the
     *  ownerhip flag should be set as `kUseCurrentProcessUID | kUseTargetFileGID`,
     *  such that even if there would be different UIDs, all processes
     *  could still access the file through the shared group.
     *
     *  The `ownership` parameter is ignored when kUnsynced mode is used.
     *
     *  @param file_sync_mode: Determines the synchronization mode (see above).
     *  @param ownership: When using kSynced mode, determines how to adjust the ownership
     *                    of the temporary file created.
     */
    explicit JsonWriter(FileSyncMode file_sync_mode = FileSyncMode::kUnsynced,
                        const score::filesystem::AtomicUpdateOwnershipFlags ownership =
                            score::filesystem::kUseTargetFileUID | score::filesystem::kUseTargetFileGID) noexcept;
    JsonWriter(const JsonWriter&) = delete;
    JsonWriter(JsonWriter&&) noexcept = delete;
    JsonWriter& operator=(const JsonWriter&) = delete;
    JsonWriter& operator=(JsonWriter&&) noexcept = delete;
    ~JsonWriter() noexcept override = default;
    score::ResultBlank ToFile(const score::json::Object& json_data,
                            const std::string_view& file_path,
                            std::shared_ptr<score::filesystem::IFileFactory> file_factory) override;

    score::ResultBlank ToFile(const score::json::List& json_data,
                            const std::string_view& file_path,
                            std::shared_ptr<score::filesystem::IFileFactory> file_factory) override;

    score::ResultBlank ToFile(const score::json::Any& json_data,
                            const std::string_view& file_path,
                            std::shared_ptr<score::filesystem::IFileFactory> file_factory) override;

    score::Result<std::string> ToBuffer(const score::json::Object& json_data) override;
    score::Result<std::string> ToBuffer(const score::json::List& json_data) override;
    score::Result<std::string> ToBuffer(const score::json::Any& json_data) override;

  private:
    FileSyncMode file_sync_mode_;
    const score::filesystem::AtomicUpdateOwnershipFlags atomic_ownership_;
};

}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_JSON_PARSER_H
