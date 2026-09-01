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
/// \brief A collection of serializers for range-based containers.
/// \details Provides serializers for arrays and tuples.

#ifndef SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_TYPES_ARRAY_TYPE_H
#define SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_TYPES_ARRAY_TYPE_H

#include <functional>
#include <string_view>
#include <type_traits>
#include <utility>

#include "score/json/internal/writer/vajson/writer/serializers/structures/serializer.h"
#include "score/json/internal/writer/vajson/writer/types/basic_types.h"

namespace score
{
namespace json
{
namespace vajson
{
inline namespace types
{
/// \brief A serializer type for a JSON array from a homogeneous C++ range
/// \tparam Range Type of range.
/// \tparam Fn The function type for this serializer.
template <typename Range, typename Fn>
class RangeSerializer final
{
  public:
    /// \brief Constructs a RangeSerializer
    /// \tparam Fn1 Type of function.
    /// \param[in] range to serialize.
    /// \param[in] fn Function used to serialize. Must not throw exceptions.
    template <typename Fn1 = Fn>
    RangeSerializer(const Range& range, Fn1&& fn) noexcept : container_{range}, function_{std::forward<Fn1>(fn)}
    {
    }

    /// \brief Call operator
    /// \details
    /// - Serialize every element of the array as a JSON value.
    /// \tparam AS Type of array serializer.
    /// \param[in] as Array serializer to write into.
    /// \pre The function contained in the class does not throw any exceptions
    template <typename AS = ArrayStart>
    void operator()(AS as) const noexcept
    {
        for (const auto& value : this->container_.get())
        {
            as = std::move(as) << this->function_(value);
        }
    }

  private:
    /// \brief Container instance to be serialized
    std::reference_wrapper<const Range> container_;

    /// \brief Function to serialize single items with
    Fn function_;
};

/// \brief Serialize an ad-hoc defined Tuple as heterogeneous array
/// \tparam Fn The function type that defines the serialization.
template <typename Fn>
struct JArrayType final
{
    /// \brief Wrapped function value
    Fn fn;
};

/// \brief Serializes an ad-hoc defined Tuple as a heterogeneous array
/// \details The function can be used to define a tuple by adding values.
/// \tparam Fn Type of serializer function. Must take an ArrayStart&& and return the follow-up serializer.
/// \param[in] fn Function used to serialize the tuple.
/// \return A serializable Tuple type.
/// \pre The passed function does not throw any exceptions
template <typename Fn, typename = std::enable_if_t<std::is_rvalue_reference<Fn&&>::value>>
auto JArray(Fn&& fn) noexcept -> JArrayType<Fn>
{  // coverity[autosar_cpp14_a13_3_1_violation]
    return {std::forward<Fn>(fn)};
}

/// \brief Serializes a homogeneous C++ range as a JSON array
/// \tparam Range Type of range.
/// \tparam Fn Type of value serializer function. Must take the range's value type and return a JSON type.
/// \param[in] range instance to be serialized.
/// \param[in] fn Function used to serialize single elements.
/// \return A serializable JSON array.
/// \pre The passed range & function do not throw any exceptions
template <typename Range, typename Fn = IdSerializer<Range>>
auto JArray(const Range& range, Fn&& fn = IdSerializer<Range>{}) noexcept -> JArrayType<RangeSerializer<Range, Fn>>
{
    return {RangeSerializer<Range, Fn>{range, std::forward<Fn>(fn)}};
}

// clang-format off
}  // inline namespace types
// clang-format off
}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_TYPES_ARRAY_TYPE_H
