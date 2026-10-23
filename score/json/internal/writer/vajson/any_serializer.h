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
#ifndef SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_ANY_SERIALIZER_H
#define SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_ANY_SERIALIZER_H

#include "score/json/internal/model/any.h"
#include "score/json/internal/writer/vajson/writer/serializers/structures/generic_value_serializer_impl.h"
#include "score/json/internal/writer/vajson/writer/serializers/structures/key_serializer.h"
#include "score/string_manipulation/string_comparison_adaptor.h"

#include <score/utility.hpp>

#include <cstdint>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>

/// @file
/// @brief Translation of the JSON data model (Any, Object, List) into calls on the vaJson serializers.

namespace score::json::internal::writer::vajson
{
class ObjectKeySerializer final
{
  public:
    auto operator()(const score::string_manipulation::StringComparisonAdaptor& key) const noexcept
        -> score::json::vajson::JKeyType
    {
        return score::json::vajson::JKey(key.GetAsStringView());
    }
};
template <typename Next>
auto SerializeValue(score::json::vajson::GenericValueSerializer<Next>&& serializer,
                    const score::json::Any& value) noexcept ->
    typename score::json::vajson::GenericValueSerializer<Next>::Next;
template <typename Next>
auto SerializeNumber(score::json::vajson::GenericValueSerializer<Next>&& serializer,
                     const score::json::Number& value) noexcept ->
    typename score::json::vajson::GenericValueSerializer<Next>::Next
{
    using Serialized = typename score::json::vajson::GenericValueSerializer<Next>::Next;

    // Dispatching on the stored type
    return value.Visit([&serializer](const auto stored) noexcept -> Serialized {
        using Stored = std::decay_t<decltype(stored)>;

        if constexpr (std::is_floating_point_v<Stored>)
        {
            return std::move(serializer) << score::json::vajson::JNumber(stored);
        }
        else if constexpr (std::is_unsigned_v<Stored>)
        {
            return std::move(serializer) << score::json::vajson::JNumber(static_cast<std::uint64_t>(stored));
        }
        else
        {
            return std::move(serializer) << score::json::vajson::JNumber(static_cast<std::int64_t>(stored));
        }
    });
}
template <typename Next>
auto SerializeList(score::json::vajson::GenericValueSerializer<Next>&& serializer,
                   const score::json::List& list) noexcept ->
    typename score::json::vajson::GenericValueSerializer<Next>::Next
{
    return std::move(serializer) << score::json::vajson::JArray(
               [&list](score::json::vajson::ArrayStart array_serializer) noexcept {
                   auto next = std::move(array_serializer);
                   for (const auto& value : list)
                   {
                       next = SerializeValue(std::move(next), value);
                   }
               });
}
template <typename Next>
auto SerializeObject(score::json::vajson::GenericValueSerializer<Next>&& serializer,
                     const score::json::Object& object) noexcept ->
    typename score::json::vajson::GenericValueSerializer<Next>::Next
{
    return std::move(serializer) << score::json::vajson::JObject(
               [&object](score::json::vajson::ObjectStart object_serializer) noexcept {
                   auto next = std::move(object_serializer);
                   for (const auto& element : object)
                   {
                       auto value_serializer = std::move(next) << ObjectKeySerializer{}(element.first);
                       next = SerializeValue(std::move(value_serializer), element.second);
                   }
                   return next;
               });
}
template <typename Next>
auto SerializeValue(score::json::vajson::GenericValueSerializer<Next>&& serializer,
                    const score::json::Any& value) noexcept ->
    typename score::json::vajson::GenericValueSerializer<Next>::Next
{
    // The outcome is parked in an optional to keep a single exit point.
    std::optional<typename score::json::vajson::GenericValueSerializer<Next>::Next> serialized{};

    if (const auto object = value.As<score::json::Object>(); object.has_value())
    {
        serialized.emplace(SerializeObject(std::move(serializer), object->get()));
    }
    else if (const auto list = value.As<score::json::List>(); list.has_value())
    {
        serialized.emplace(SerializeList(std::move(serializer), list->get()));
    }
    else if (const auto string_value = value.As<std::string>(); string_value.has_value())
    {
        serialized.emplace(std::move(serializer) << score::json::vajson::JString(string_value->get()));
    }
    else if (const auto null_value = value.As<score::json::Null>(); null_value.has_value())
    {
        score::cpp::ignore = null_value;
        serialized.emplace(std::move(serializer) << score::json::vajson::JNull());
    }
    else if (const auto number = value.As<score::json::Number>(); number.has_value())
    {
        serialized.emplace(SerializeNumber(std::move(serializer), number->get()));
    }
    else if (const auto boolean = value.As<bool>(); boolean.has_value())
    {
        serialized.emplace(std::move(serializer) << score::json::vajson::JBool(*boolean));
    }
    else
    {
        // Any holds a bool, a Number, a std::string, a Null, an Object or a List, so every alternative has been
        // probed by now and this branch cannot be reached. Writing null keeps the output valid JSON in case an
        // alternative is added to Any without being handled here.
        serialized.emplace(std::move(serializer) << score::json::vajson::JNull()); /* LCOV_EXCL_LINE */
    }

    return *std::move(serialized);
}
}  // namespace score::json::internal::writer::vajson

#endif  // SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_ANY_SERIALIZER_H
