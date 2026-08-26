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
/// \brief Provides a serializer for the score::cpp::variant type.

#ifndef SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_VAC_VARIANT_H
#define SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_VAC_VARIANT_H

#include <utility>

#include "score/variant.hpp"
namespace score
{
namespace json
{
namespace vajson
{
/// \brief A generic variant visitor
/// \details Serializes the children according to their implemented serializers.
/// \tparam Serializer Type of serializer. Must be noexcept move constructible and noexcept move assignable.
template <typename Serializer>
class VariantVisitor
{
    static_assert(std::is_nothrow_move_constructible<Serializer>::value, "Serializer must be noexcept movable");
    static_assert(std::is_nothrow_move_assignable<Serializer>::value, "Serializer must be noexcept movable");

  public:
    /// \brief Constructs the visitor
    /// \details Visitor must only be used once.
    /// \param[in] serializer to use.
    explicit VariantVisitor(Serializer&& serializer) noexcept : serializer_(std::move(serializer)) {}

    /// \brief Call operator
    /// \tparam T Type of the variant.
    /// \param[in] value to serialize.
    /// \return The succeeding serializer.
    template <typename T>
    auto operator()(const T& value) noexcept -> typename Serializer::Next
    {
        return std::move(this->serializer_) << value;
    }

  private:
    /// \brief Serializer of the variant
    Serializer serializer_;
};

/// \brief Serializes a variant of serializable elements
/// \tparam Serializer Type of serializer.
/// \tparam Types of values in the variant.
/// \param[in] s Serializer instance to write into.
/// \param[in] variant to serialize.
/// \return The succeeding serializer.
/// \pre All alternatives contained in the variant are noexcept movable
template <typename Serializer, typename... Types>
auto operator<<(Serializer s, score::cpp::variant<Types...>& variant) noexcept -> typename Serializer::Next
{
    VariantVisitor<Serializer> ser{std::move(s)};
    return score::cpp::visit(std::move(ser), variant);
}

}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_VAC_VARIANT_H
