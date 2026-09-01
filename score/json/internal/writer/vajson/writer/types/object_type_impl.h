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
/// \file
/// \brief Implementation of methods for object type.

#ifndef SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_TYPES_OBJECT_TYPE_IMPL_H
#define SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_TYPES_OBJECT_TYPE_IMPL_H

#include <utility>

#include "score/json/internal/writer/vajson/writer/serializers/structures/generic_value_serializer.h"
#include "score/json/internal/writer/vajson/writer/serializers/structures/serializer.h"
#include "score/json/internal/writer/vajson/writer/types/object_type.h"

namespace score
{
namespace json
{
namespace vajson
{

inline namespace types
{
/// \brief Serialize every key-value pair as a JSON key followed by a JSON value
template <typename Range, typename KeyFn, typename ValueFn>
template <typename KS>
auto PairRangeSerializer<Range, KeyFn, ValueFn>::operator()(KS os) const noexcept -> KS
{
    for (const auto& pair : this->map_.get())
    {
        ObjectSerializerValue osv{std::move(os) << this->key_function_(pair.first)};
        os = std::move(osv) << this->value_function_(pair.second);
    }
    return os;
}

// clang-format off
}  // namespace types
// clang-format on
}  // namespace vajson
}  // namespace json
}  // namespace score
#endif  // SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_TYPES_OBJECT_TYPE_IMPL_H
