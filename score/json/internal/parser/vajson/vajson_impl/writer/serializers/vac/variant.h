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
 *        \brief  Provides a serializer for the score::cpp::variant type.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_VAC_VARIANT_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_VAC_VARIANT_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <utility>

#include "score/variant.hpp"
namespace amsr {
namespace json {
/*!
 * \brief           A generic variant visitor
 * \details         Serializes the children according to their implemented serializers.
 * \vprivate        component private
 *
 * \tparam          Serializer
 *                  Type of serializer. Must be noexcept move constructible and noexcept move assignable.
 *
 * \trace           DSGN-JSON-Writer-Serializable-Data-Structures
 */
template <typename Serializer>
class VariantVisitor {
  static_assert(std::is_nothrow_move_constructible<Serializer>::value, "Serializer must be noexcept movable");
  static_assert(std::is_nothrow_move_assignable<Serializer>::value, "Serializer must be noexcept movable");

 public:
  /*!
   * \brief           Constructs the visitor
   * \details         Visitor must only be used once.
   * \vprivate        Vector component internal API
   *
   * \param[in]       serializer
   *                  to use.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \spec
   * requires true;
   * \endspec
   */
  explicit VariantVisitor(Serializer&& serializer) noexcept : serializer_(std::move(serializer)) {}

  /*!
   * \brief           Call operator
   * \vprivate        Vector component internal API
   *
   * \tparam          T
   *                  Type of the variant.
   * \param[in]       value
   *                  to serialize.
   * \return          The succeeding serializer.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \spec
   * requires true;
   * \endspec
   */
  template <typename T>
  auto operator()(T const& value) noexcept -> typename Serializer::Next {
    return std::move(this->serializer_) << value;
  }

 private:
  /*!
   * \brief           Serializer of the variant
   */
  Serializer serializer_;
};

/*!
 * \brief           Serializes a variant of serializable elements
 * \vpublic
 *
 * \tparam          Serializer
 *                  Type of serializer.
 * \tparam          Types
 *                  of values in the variant.
 * \param[in]       s
 *                  Serializer instance to write into.
 * \param[in]       variant
 *                  to serialize.
 * \return          The succeeding serializer.
 *
 * \context         ANY
 * \pre             All alternatives contained in the variant are noexcept movable
 * \threadsafe      FALSE
 * \reentrant       FALSE
 * \synchronous     -
 * \trace           DSGN-JSON-Writer-Serializable-Data-Structures
 */
template <typename Serializer, typename... Types>
auto operator<<(Serializer s, score::cpp::variant<Types...>& variant) noexcept -> typename Serializer::Next {
  VariantVisitor<Serializer> ser{std::move(s)};
  // VCA_VAJSON_EXTERNAL_CALL
  return score::cpp::visit(std::move(ser), variant);
}

}  // namespace json
}  // namespace amsr

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_VAC_VARIANT_H_
