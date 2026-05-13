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

/// @file load_buffer.hpp
/// @brief Utility for loading FlatBuffer files from the filesystem.

#ifndef SCORE_LIB_FLATBUFFERS_LOAD_BUFFER_HPP
#define SCORE_LIB_FLATBUFFERS_LOAD_BUFFER_HPP

#include "score/filesystem/path.h"
#include "score/os/errno.h"

#include <cstdint>
#include <memory_resource>
#include <vector>

namespace score
{

namespace flatbuffers
{

/// @brief Loads the entire contents of a binary file into a
/// `std::vector<uint8_t>`.
///
/// @param[in] path The filesystem path to the file to load.
///
/// @returns a `std::vector<uint8_t>` on success, a `score::os::Error` on failure
///
/// @note `std::bad_alloc` thrown by `resize()` is caught and returned
///       as `score::os::Error::Code::kNotEnoughSpace`.
///       Any other exception from `resize()` is returned as an unspecified error.
///
/// @see LoadBuffer(const score::filesystem::Path&, std::pmr::vector<uint8_t>&)
///      for a variant using polymorphic memory resources.
score::os::Result<std::vector<uint8_t>> LoadBuffer(const score::filesystem::Path& path) noexcept;

/// @brief Loads the entire contents of a binary file into a
/// `std::pmr::vector<uint8_t>`.
///
/// @param[in] path   The filesystem path to the file to load.
/// @param[out] data  Output container where the file contents will be placed.
///                   This vector will be resized to fit the file's contents.
///
/// @returns a `score::cpp::blank` on success, a `score::os::Error` on failure
///
/// @note `std::bad_alloc` thrown by `resize()` is caught and returned
///       as `score::os::Error::Code::kNotEnoughSpace`.
///       Any other exception from `resize()` is returned as an unspecified error.
///
/// @note `data` is not cleared before use. On error, its contents are
///       unspecified — it may have been resized and partially populated.
score::os::Result<score::cpp::blank> LoadBuffer(const score::filesystem::Path& path,
                                                std::pmr::vector<uint8_t>& data) noexcept;
}  // namespace flatbuffers
}  // namespace score

#endif  // SCORE_LIB_FLATBUFFERS_LOAD_BUFFER_HPP
