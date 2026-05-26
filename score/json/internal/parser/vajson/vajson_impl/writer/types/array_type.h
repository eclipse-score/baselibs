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
 *        \brief  A collection of serializers for range-based containers.
 *
 *      \details  Provides serializers for arrays and tuples.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_TYPES_ARRAY_TYPE_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_TYPES_ARRAY_TYPE_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <functional>
#include <string_view>
#include <type_traits>
#include <utility>

#include "amsr/json/writer/serializers/stl/primitives.h"
#include "amsr/json/writer/serializers/structures/serializer.h"
#include "amsr/json/writer/serializers/vac/primitives.h"
#include "amsr/json/writer/types/basic_types.h"

namespace amsr {
namespace json {
inline namespace types {
/*!
 * \brief           A serializer type for a JSON array from a homogeneous C++ range
 * \vprivate        Vector component internal API
 * \tparam          Range
 *                  Type of range.
 * \tparam          Fn
 *                  The function type for this serializer.
 */
template <typename Range, typename Fn>
class RangeSerializer final {
 public:
  /*!
   * \brief           Constructs a RangeSerializer
   * \vprivate        Vector component internal API
   * \tparam          Fn1
   *                  Type of function.
   * \param[in]       range
   *                  to serialize.
   * \param[in]       fn
   *                  Function used to serialize. Must not throw exceptions.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \spec
   * requires true;
   * \endspec
   */
  template <typename Fn1 = Fn>
  RangeSerializer(Range const& range, Fn1&& fn) noexcept : container_{range}, function_{std::forward<Fn1>(fn)} {}

  /*!
   * \brief           Call operator
   * \vprivate        Vector component internal API
   *
   * \tparam          AS
   *                  Type of array serializer.
   * \param[in]       as
   *                  Array serializer to write into.
   *
   * \context         ANY
   * \pre             The function contained in the class does not throw any exceptions
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \internal
   * - Serialize every element of the array as a JSON value.
   * \endinternal
   */
  template <typename AS = ArrayStart>
  void operator()(AS as) const noexcept {
    // VCA_VAJSON_THIS_DEREF
    for (auto const& value : this->container_.get()) {
      as = std::move(as) << this->function_(value);
    }  // VCA_VAJSON_EXTERNAL_CALL
  }

 private:
  /*!
   * \brief           Container instance to be serialized
   */
  std::reference_wrapper<Range const> container_;

  /*!
   * \brief           Function to serialize single items with
   */
  Fn function_;
};

/*!
 * \brief           Serialize an ad-hoc defined Tuple as heterogeneous array
 * \vprivate        Vector component internal API
 * \tparam          Fn
 *                  The function type that defines the serialization.
 */
template <typename Fn>
struct JArrayType final {
  /*!
   * \brief           Wrapped function value
   */
  Fn fn;
};

/*!
 * \brief           Serializes an ad-hoc defined Tuple as a heterogeneous array
 * \details         The function can be used to define a tuple by adding values.
 * \vpublic
 * \tparam          Fn
 *                  Type of serializer function. Must take an ArrayStart&& and return the follow-up serializer.
 * \param[in]       fn
 *                  Function used to serialize the tuple.
 * \return          A serializable Tuple type.
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
auto JArray(Fn&& fn) noexcept -> JArrayType<Fn> {  // VECTOR SL AutosarC++17_10-A13.3.1: MD_JSON_rvalue_ref
  return {std::forward<Fn>(fn)};
}

/*!
 * \brief           Serializes a homogeneous C++ range as a JSON array
 * \vpublic
 * \tparam          Range
 *                  Type of range.
 * \tparam          Fn
 *                  Type of value serializer function. Must take the range's value type and return a JSON type.
 * \param[in]       range
 *                  instance to be serialized.
 * \param[in]       fn
 *                  Function used to serialize single elements.
 * \return          A serializable JSON array.
 *
 * \context         ANY
 * \pre             The passed range & function do not throw any exceptions
 * \threadsafe      FALSE
 * \reentrant       FALSE
 * \synchronous     -
 * \spec
 *    requires true;
 * \endspec
 */
template <typename Range, typename Fn = IdSerializer<Range>>
auto JArray(Range const& range, Fn&& fn = IdSerializer<Range>{}) noexcept -> JArrayType<RangeSerializer<Range, Fn>> {
  return {RangeSerializer<Range, Fn>{range, std::forward<Fn>(fn)}};
}

// clang-format off
}  // inline namespace types
// clang-format off
}  // namespace json
}  // namespace amsr

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_TYPES_ARRAY_TYPE_H_
