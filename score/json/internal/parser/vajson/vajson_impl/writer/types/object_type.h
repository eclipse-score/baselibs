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
 *        \brief  A collection of serializers for objects.
 *
 *      \details  Provides serializers for homogeneous C++ pair-ranges and Object types.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_TYPES_OBJECT_TYPE_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_TYPES_OBJECT_TYPE_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <string_view>
#include <type_traits>
#include <utility>

#include "amsr/json/util/types.h"
#include "amsr/json/writer/serializers/structures/serializer.h"
#include "amsr/json/writer/types/basic_types.h"

namespace amsr {
namespace json {
inline namespace types {
/*!
 * \brief           A serializer type for predefined keys
 *
 * \vprivate        Vector component internal API
 */
class DefaultKeySerializer {
 public:
  /*!
   * \brief           Call operator
   * \vprivate        Vector component internal API
   *
   * \tparam          T
   *                  Type of value.
   * \param[in]       value
   *                  to serialize as a key. Must be convertible to a StringView type.
   * \return          The serializable key type.
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   *
   * \spec
   * requires true;
   * \endspec
   */
  template <typename T>
  auto operator()(T const& value) const noexcept -> JKeyType {
    static_assert(std::is_constructible<std::string_view, T>::value, "Keys must be convertible to a StringView");
    return JKeyType{std::string_view{value}};
  }
};

/*!
 * \brief           A serializer type for a JSON array from a homogeneous C++ pair-range
 * \vprivate        Vector component internal API
 * \tparam          Range
 *                  Type of range to serialize.
 * \tparam          KeyFn
 *                  Type of key function.
 * \tparam          ValueFn
 *                  Type of value function.
 */
template <typename Range, typename KeyFn, typename ValueFn>
class PairRangeSerializer final {
 public:
  /*!
   * \brief           Constructs a PairRangeSerializer
   * \vprivate        Vector component internal API
   * \tparam          KeyFn1
   *                  Type of key function.
   * \tparam          ValueFn1
   *                  Type of value function.
   * \param[in]       range
   *                  to serialize.
   * \param[in]       key_fn
   *                  Function used to serialize single keys.
   * \param[in]       value_fn
   *                  Function used to serialize single values.
   *
   * \context         ANY
   * \pre             The passed functions & range do not throw any exceptions
   * \threadsafe      FALSE
   * \reentrant       FALSE
   */
  template <typename KeyFn1 = KeyFn, typename ValueFn1 = ValueFn>
  PairRangeSerializer(Range const& range, KeyFn1&& key_fn, ValueFn1&& value_fn) noexcept
      : map_{range}, key_function_{std::forward<KeyFn1>(key_fn)}, value_function_{std::forward<ValueFn1>(value_fn)} {}

  /*!
   * \brief           Call operator
   * \vprivate        Vector component internal API
   * \tparam          KS
   *                  Type of key serializer.
   * \param[in]       os
   *                  Object serializer to write into.
   * \return          The serializer state after serializing the range.
   *
   * \context         ANY
   * \pre             The functions contained in the class do not throw any exceptions
   * \threadsafe      FALSE
   * \reentrant       FALSE
   */
  template <typename KS = KeySerializer>
  auto operator()(KS os) const noexcept -> KS;

 private:
  /*!
   * \brief           Range instance to be serialized
   */
  std::reference_wrapper<Range const> map_;

  /*!
   * \brief           Function used to serialize single keys
   */
  KeyFn key_function_;

  /*!
   * \brief           Function used to serialize single values
   */
  ValueFn value_function_;
};

/*!
 * \brief           An Object type
 * \vprivate        Vector component internal API
 * \tparam          Fn
 *                  Type of serializer function.
 */
template <typename Fn>
// VCA_VAJSON_MOLE_1298
struct JObjectType final {
  /*!
   * \brief           Function used to serialize the object
   */
  Fn fn;
};

/*!
 * \brief           Serializes an object value
 * \vpublic
 * \tparam          Fn
 *                  Type of serializer function. Must take an ObjectStart&& and return the follow-up serializer.
 * \param[in]       fn
 *                  Function used to serialize the object.
 * \return          The serializable object type.
 *
 * \context         ANY
 * \pre             The passed function does not throw any exceptions
 * \threadsafe      FALSE
 * \reentrant       FALSE
 * \synchronous     -
 * \spec
 *    requires true;
 * \endspec
 */
template <typename Fn, typename = std::enable_if_t<std::is_rvalue_reference<Fn&&>::value>>
auto JObject(Fn&& fn) noexcept -> JObjectType<Fn> {  // VECTOR SL AutosarC++17_10-A13.3.1: MD_JSON_rvalue_ref
  return {std::forward<Fn>(fn)};
}

/*!
 * \brief           Serializes a homogeneous C++ pair-range (e.g. a map) as a JSON object
 * \vpublic
 * \tparam          Range
 *                  Type of range.
 * \tparam          KeyFn
 *                  Type of key serializer function. Must take the range's key type and return a JSON type.
 * \tparam          ValueFn
 *                  Type of value serializer function. Must take the range's value type and return a JSON type.
 * \param[in]       range
 *                  instance to be serialized.
 * \param[in]       key_fn
 *                  Function used to serialize single keys.
 * \param[in]       value_fn
 *                  Function used to serialize single values.
 * \return          The serializable JSON object.
 *
 * \context         ANY
 * \pre             The passed range & functions do not throw any exceptions
 * \threadsafe      FALSE
 * \reentrant       FALSE
 * \synchronous     -
 * \spec
 *    requires true;
 * \endspec
 */
template <typename Range, typename KeyFn = DefaultKeySerializer, typename ValueFn = IdSerializer<Range>>
auto JObject(Range const& range, KeyFn&& key_fn = DefaultKeySerializer{},
             ValueFn&& value_fn = IdSerializer<Range>{}) noexcept
    -> JObjectType<PairRangeSerializer<Range, KeyFn, ValueFn>> {
  return {
      PairRangeSerializer<Range, KeyFn, ValueFn>{range, std::forward<KeyFn>(key_fn), std::forward<ValueFn>(value_fn)}};
}

// clang-format off
}  // inline namespace types
// clang-format on
}  // namespace json
}  // namespace amsr

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_TYPES_OBJECT_TYPE_H_
