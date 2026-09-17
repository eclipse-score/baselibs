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

#ifndef SCORE_LIB_JSON_INTERNAL_MODEL_OBJECT_H
#define SCORE_LIB_JSON_INTERNAL_MODEL_OBJECT_H

#include "score/json/internal/model/any.h"

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace score
{
namespace json
{

namespace detail
{
template <typename T>
using IsJsonObject = std::enable_if_t<std::is_same<T, std::reference_wrapper<const Object>>::value ||
                                          std::is_same<T, Result<std::reference_wrapper<const Object>>>::value,
                                      bool>;

template <typename V, typename RV = V>
inline Result<RV> GetAttribute(const json::Object& object, const std::string_view& key) noexcept
{
    auto iterator = object.find(key);
    if (iterator == object.end())
    {
        return score::MakeUnexpected(score::json::Error::kKeyNotFound, "Key was not found on the object");
    }
    return iterator->second.As<V>();
}

template <typename V, typename RV = V>
inline Result<RV> GetAttribute(const std::reference_wrapper<const json::Object>& object_result,
                               const std::string_view& key) noexcept
{
    return GetAttribute<V, RV>(object_result.get(), key);
}

template <typename V, typename RV = V>
inline Result<RV> GetAttribute(const Result<std::reference_wrapper<const json::Object>>& object_result,
                               const std::string_view& key) noexcept
{
    if (!object_result.has_value())
    {
        return Unexpected{object_result.error()};
    }

    return GetAttribute<V, RV>(*object_result, key);
}

}  // namespace detail

/// \brief Convenience method to get attributes from json::Object
///
/// The object must be passed as std::cref(object) (or as the Result of a previous GetAttribute /
/// Any::As<Object>() call, which is forwarded together with its error). Taking a reference wrapper instead of a
/// const json::Object& prevents binding a temporary, since the returned Result may reference the object.
template <typename V,
          std::enable_if_t<(!std::is_same<V, List>::value) && (!std::is_same<V, Object>::value), bool> = true,
          typename T,
          detail::IsJsonObject<T> = true>
auto GetAttribute(T object, const std::string_view& key) noexcept
{
    return detail::GetAttribute<V>(object, key);
}

/// \brief Convenience method to get attributes from json::Object (variant for Objects and Lists)
template <typename V,
          std::enable_if_t<std::is_same<V, List>::value || std::is_same<V, Object>::value, bool> = true,
          typename T,
          detail::IsJsonObject<T> = true>
auto GetAttribute(T object, const std::string_view& key) noexcept
{
    return detail::GetAttribute<V, std::reference_wrapper<const V>>(object, key);
}

}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_MODEL_OBJECT_H
