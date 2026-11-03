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

#ifndef SCORE_SERIALIZATION_ERROR_SERIALIZATION_ERROR_H
#define SCORE_SERIALIZATION_ERROR_SERIALIZATION_ERROR_H

#include "score/result/error.h"
#include "score/result/error_code.h"

namespace score
{
namespace serialization
{

/// @brief Represents all errors that can be returned by our serialization library
// Suppress "AUTOSAR C++14 M3-2-3", The rule states: "A type, object or function that
// is used in multiple translation units shall be declared in one and only one file."
// This is a false positive. SerializationError is not defined more than once.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
enum class SerializationError : score::result::ErrorCode
{
    kOutOfBoundsWrite,
    kStringTooBig,
    kNotAllDataDeserialized
};

/// @brief ADL overload to fulfill design requirements from lib/result
// Suppress "AUTOSAR C++14 M3-2-3", The rule states: "A type, object or function that
// is used in multiple translation units shall be declared in one and only one file."
// This is a false positive. MakeError is not defined more than once.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
score::result::Error MakeError(const SerializationError code, const std::string_view user_message = "") noexcept;

}  // namespace serialization
}  // namespace score
#endif  // SCORE_SERIALIZATION_ERROR_SERIALIZATION_ERROR_H
