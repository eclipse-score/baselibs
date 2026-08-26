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
#ifndef SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_VAJSON_SERIALIZE_H
#define SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_VAJSON_SERIALIZE_H
#include "score/json/internal/model/any.h"
#include "score/json/internal/model/error.h"
#include "score/json/internal/writer/vajson/writer/serializers/structures/generic_value_serializer_impl.h"
#include "score/json/internal/writer/vajson/writer/serializers/structures/key_serializer.h"
#include "score/result/result.h"
#include <score/assert.hpp>
#include <score/utility.hpp>
#include <cstdint>
#include <optional>
#include <ostream>
#include <string>
#include <utility>
namespace score
{
namespace json
{
namespace internal
{
namespace writer
{
namespace vajson
{
class ObjectKeySerializer final
{
  public:
    auto operator()(const score::memory::StringComparisonAdaptor& key) const noexcept -> score::json::vajson::JKeyType
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
    // The serializer is consumed by whichever alternative matches, so the outcome is parked in an
    // optional to keep a single exit point. Number::As() re-parses the value on every call, hence the
    // chain stays an else-if: the alternatives must be probed lazily, in order.
    std::optional<typename score::json::vajson::GenericValueSerializer<Next>::Next> serialized{};

    if (const auto unsigned_value = value.As<std::uint64_t>(); unsigned_value.has_value())
    {
        serialized.emplace(std::move(serializer) << score::json::vajson::JNumber(*unsigned_value));
    }
    else if (const auto signed_value = value.As<std::int64_t>(); signed_value.has_value())
    {
        serialized.emplace(std::move(serializer) << score::json::vajson::JNumber(*signed_value));
    }
    else if (const auto float_value = value.As<float>(); float_value.has_value())
    {
        serialized.emplace(std::move(serializer) << score::json::vajson::JNumber(*float_value));
    }
    else if (const auto double_value = value.As<double>(); double_value.has_value())
    {
        serialized.emplace(std::move(serializer) << score::json::vajson::JNumber(*double_value));
    }
    else
    {
        serialized.emplace(std::move(serializer) << score::json::vajson::JNull());
    }

    return *std::move(serialized);
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
    // See SerializeNumber() for why the outcome is parked in an optional rather than returned directly.
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
    else
    {
        const auto boolean = value.As<bool>();
        serialized.emplace(std::move(serializer) << score::json::vajson::JBool(*boolean));
    }

    return *std::move(serialized);
}
}  // namespace vajson
}  // namespace writer
}  // namespace internal
class VajsonSerialize final
{
  public:
    explicit VajsonSerialize(std::ostream& out_stream) noexcept;
    ~VajsonSerialize() noexcept = default;
    VajsonSerialize(const VajsonSerialize&) = delete;
    VajsonSerialize(VajsonSerialize&&) noexcept = default;
    VajsonSerialize& operator=(const VajsonSerialize&) = delete;
    VajsonSerialize& operator=(VajsonSerialize&&) = delete;
    score::Result<void> operator<<(const score::json::Object& json_data);
    score::Result<void> operator<<(const score::json::List& json_data);
    score::Result<void> operator<<(const score::json::Any& json_data);

  private:
    std::ostream& out_stream_;
};
score::Result<std::string> VajsonToBuffer(const score::json::Object& json_data);
score::Result<std::string> VajsonToBuffer(const score::json::List& json_data);
score::Result<std::string> VajsonToBuffer(const score::json::Any& json_data);
}  // namespace json
}  // namespace score
namespace score
{
namespace json
{
namespace vajson
{
template <typename Next>
auto operator<<(GenericValueSerializer<Next>&& serializer, const score::json::Object& value) noexcept ->
    typename GenericValueSerializer<Next>::Next
{
    return score::json::internal::writer::vajson::SerializeObject(std::move(serializer), value);
}
template <typename Next>
auto operator<<(GenericValueSerializer<Next>&& serializer, const score::json::List& value) noexcept ->
    typename GenericValueSerializer<Next>::Next
{
    return score::json::internal::writer::vajson::SerializeList(std::move(serializer), value);
}
template <typename Next>
auto operator<<(GenericValueSerializer<Next>&& serializer, const score::json::Any& value) noexcept ->
    typename GenericValueSerializer<Next>::Next
{
    return score::json::internal::writer::vajson::SerializeValue(std::move(serializer), value);
}
}  // namespace vajson
}  // namespace json
}  // namespace score
#endif  // SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_VAJSON_SERIALIZE_H
