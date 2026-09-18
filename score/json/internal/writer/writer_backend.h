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

#ifndef SCORE_LIB_JSON_INTERNAL_WRITER_WRITER_BACKEND_H
#define SCORE_LIB_JSON_INTERNAL_WRITER_WRITER_BACKEND_H

#include "score/json/internal/model/any.h"
#include "score/result/result.h"

#include <ostream>
#include <string>

namespace score
{
namespace json
{
namespace internal
{
namespace writer
{

/// \brief Backend seam for JSON serialization.
///
/// Exactly one backend provides definitions for the declarations below. Which one is linked is decided at build
/// time by the `//score/json:writer_library` flag, resolved through the `//score/json/internal/writer:writer`
/// alias -- the same mechanism the parser uses via `//score/json:base_library`.
///
/// Note that the emitted representation is backend specific: `json_serialize` pretty-prints with a four space
/// indentation, whereas `vajson` emits compact JSON without insignificant whitespace.

/// \brief Serializes json_data into out_stream
/// \param out_stream The stream to write the serialized representation to
/// \param json_data The data to serialize
/// \return empty result on success, error otherwise
score::Result<void> SerializeToStream(std::ostream& out_stream, const score::json::Object& json_data);
score::Result<void> SerializeToStream(std::ostream& out_stream, const score::json::List& json_data);
score::Result<void> SerializeToStream(std::ostream& out_stream, const score::json::Any& json_data);

/// \brief Serializes json_data into a string
/// \param json_data The data to serialize
/// \return the serialized representation on success, error otherwise
score::Result<std::string> SerializeToBuffer(const score::json::Object& json_data);
score::Result<std::string> SerializeToBuffer(const score::json::List& json_data);
score::Result<std::string> SerializeToBuffer(const score::json::Any& json_data);

}  // namespace writer
}  // namespace internal
}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_WRITER_WRITER_BACKEND_H
