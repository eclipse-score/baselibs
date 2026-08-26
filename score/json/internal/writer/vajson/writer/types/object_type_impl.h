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
/*!        \file
 *        \brief  Implementation of methods for object type.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_TYPES_OBJECT_TYPE_IMPL_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_TYPES_OBJECT_TYPE_IMPL_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <utility>

#include "score/json/internal/writer/vajson/writer/serializers/structures/generic_value_serializer.h"
#include "score/json/internal/writer/vajson/writer/serializers/structures/serializer.h"
#include "score/json/internal/writer/vajson/writer/types/object_type.h"

namespace amsr {
namespace json {

inline namespace types {
/*!
 * \brief           Serialize every key-value pair as a JSON key followed by a JSON value
 * \vprivate        Vector component internal API
 *
 * \spec
 * requires true;
 * \endspec
 */
template <typename Range, typename KeyFn, typename ValueFn>
template <typename KS>
auto PairRangeSerializer<Range, KeyFn, ValueFn>::operator()(KS os) const noexcept -> KS {
  // VCA_VAJSON_THIS_DEREF
  // VECTOR NCL MisraC++2023-11.6.1: MD_JSON_MISRAC++2023-11.6.1_range_based_initialization
  for (auto const& pair : this->map_.get()) {
    // VCA_VAJSON_THIS_DEREF
    ObjectSerializerValue osv{std::move(os) << this->key_function_(pair.first)};
    // VCA_VAJSON_THIS_DEREF
    os = std::move(osv) << this->value_function_(pair.second);
  }
  return os;
}

// clang-format off
}  // namespace types
// clang-format on
}  // namespace json
}  // namespace amsr
#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_TYPES_OBJECT_TYPE_IMPL_H_
