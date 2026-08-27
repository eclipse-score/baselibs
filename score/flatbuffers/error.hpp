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

#ifndef SCORE_LIB_FLATBUFFERS_ERROR_HPP
#define SCORE_LIB_FLATBUFFERS_ERROR_HPP

#include "score/result/result.h"

namespace score
{
namespace flatbuffers
{

/// @brief Error codes returned by the FlatBuffers utility functions.
enum class ErrorCode : score::result::ErrorCode
{
    /// A null data pointer was passed to a buffer function.
    kNullDataPointer,
    /// The FlatBuffer failed structural verification (too small, bad offset, etc.).
    kVerificationFailed,
    /// The version_info table is not present in the buffer.
    kVersionInfoNotPresent,
    /// The actual buffer version does not match the expected version.
    kVersionMismatch,
};

score::result::Error MakeError(ErrorCode code, std::string_view user_message = "") noexcept;

}  // namespace flatbuffers
}  // namespace score

#endif  // SCORE_LIB_FLATBUFFERS_ERROR_HPP
