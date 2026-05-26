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
 *        \brief  A collection of serializers for basic JSON types.
 *
 *      \details  Provides serializers for Null, Bool, Key, Number, and String types.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_TYPES_BASIC_TYPES_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_TYPES_BASIC_TYPES_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "amsr/json/util/types.h"

namespace amsr {
namespace json {
inline namespace types {
/*!
 * \brief           A Null type
 *
 * \vprivate        Vector component internal API
 */
struct JNullType final {};

/*!
 * \brief           Serializes a Null value
 * \vpublic
 * \return          The serializable null type.
 *
 * \context         ANY
 * \pre             -
 * \threadsafe      FALSE
 * \reentrant       FALSE
 *
 * \synchronous     -
 * \spec
 * requires true;
 * \endspec
 */
constexpr inline auto JNull() noexcept -> JNullType { return JNullType{}; }

/*!
 * \brief           A Bool type
 *
 * \vprivate        Vector component internal API
 */
struct JBoolType final {
  /*!
   * \brief           Wrapped bool value
   */
  bool value;
};

/*!
 * \brief           Serializes a Bool value
 * \vpublic
 * \param[in]       b
 *                  Bool value to serialize.
 * \return          The serializable bool type.
 *
 * \context         ANY
 * \pre             -
 * \threadsafe      FALSE
 * \reentrant       FALSE
 *
 * \synchronous     -
 * \spec
 * requires true;
 * \endspec
 */
constexpr inline auto JBool(bool b) noexcept -> JBoolType { return {b}; }

/*!
 * \brief           A Key type
 *
 * \vprivate        Vector component internal API
 */
class JKeyType final {
 public:
  /*!
   * \brief           Constructs a Key type
   * \param[in]       s
   *                  Key to serialize.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   *
   * \spec
   * requires true;
   * \endspec
   */
  explicit constexpr JKeyType(std::string_view s) noexcept : value_(s) {}

  /*!
   * \brief           Returns the contained value
   * \return          The value.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      TRUE, for different this pointer
   * \spec
   * requires true;
   * \endspec
   */
  auto GetValue() const noexcept -> std::string_view { return this->value_; }

 private:
  /*!
   * \brief           Wrapped string value
   */
  std::string_view value_;
};

/*!
 * \brief           Serializes a Key value
 * \vpublic
 * \param[in]       s
 *                  Key to serialize.
 * \return          The serializable key type.
 *
 * \context         ANY
 * \pre             -
 * \threadsafe      FALSE
 * \reentrant       FALSE
 *
 * \synchronous     -
 * \spec
 * requires true;
 * \endspec
 */
constexpr auto JKey(std::string_view s) noexcept -> JKeyType { return JKeyType{s}; }

/*!
 * \brief           Serializes a Key value
 * \vpublic
 * \param[in]       s
 *                  Key to serialize.
 * \return          The serializable key type.
 *
 * \context         ANY
 * \pre             -
 * \threadsafe      FALSE
 * \reentrant       FALSE
 *
 * \synchronous     -
 * \spec
 * requires true;
 * \endspec
 */
constexpr auto JKey(std::string const& s) noexcept -> JKeyType { return JKey(std::string_view{s}); }

/*!
 * \brief           Serializes a Key value
 * \vpublic
 * \param[in]       s
 *                  Key to serialize.
 * \return          The serializable key type.
 *
 * \context         ANY
 * \pre             -
 * \threadsafe      FALSE
 * \reentrant       FALSE
 *
 * \synchronous     -
 * \spec
 * requires true;
 * \endspec
 */
constexpr auto JKey(std::string const& s) noexcept -> JKeyType { return JKey(std::string_view(s)); }

inline namespace literals {
/*!
 * \brief           Serializes a Key value
 * \vpublic
 * \param[in]       s
 *                  String literal to serialize.
 * \param[in]       size
 *                  Size of the pointer.
 * \return          The serializable key type.
 *
 * \context         ANY
 * \pre             -
 * \threadsafe      FALSE
 * \reentrant       FALSE
 *
 * \synchronous     -
 * \spec
 * requires true;
 * \endspec
 */
// VECTOR NCL AutosarC++17_10-A13.1.3: MD_JSON_A13-1-3_false_positive
constexpr auto operator""_key(char const* s, std::size_t size) noexcept -> JKeyType {
  return JKey(score::safecpp::zstring_view{s, size});
}

// clang-format off
}  // namespace literals
// clang-format on

/*!
 * \brief           A Number type
 * \vprivate        Vector component internal API
 * \tparam          N
 *                  Type of number.
 */
template <typename N, typename = typename std::enable_if<std::is_arithmetic<N>::value>::type>
class JNumberType final {
 public:
  /*!
   * \brief           Constructs a Number type
   * \vprivate        Vector component internal API
   *
   * \param[in]       num
   *                  Number to serialize.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \spec
   * requires true;
   * \endspec
   */
  constexpr explicit JNumberType(N num) noexcept : value_(num) {}

  /*!
   * \brief           Returns the contained value
   * \return          The value.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      TRUE, for different this pointer
   * \spec
   * requires true;
   * \endspec
   */
  auto GetValue() const noexcept -> N { return this->value_; }

 private:
  /*!
   * \brief           Wrapped number value
   */
  N value_;
};

/*!
 * \brief           A char Number type
 *
 * \vprivate        Vector component internal API
 */
template <>
class JNumberType<char> final {
 public:
  /*!
   * \brief           Constructs a Number type
   * \vprivate        Vector component internal API
   *
   * \param[in]       num
   *                  Number to serialize.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \spec
   * requires true;
   * \endspec
   */
  constexpr explicit JNumberType(char num) noexcept : value_(std::char_traits<char>::to_int_type(num)) {}

  /*!
   * \brief           Returns the contained value
   * \vprivate        Vector component internal API
   *
   * \return          The value.
   * \context         ANY
   * \pre             -
   * \threadsafe      TRUE, for different this pointer
   * \spec
   * requires true;
   * \endspec
   */
  auto GetValue() const noexcept -> std::int32_t { return this->value_; }

 private:
  /*!
   * \brief           Wrapped number value
   */
  std::int32_t value_;
};

/*!
 * \brief           A std::uint8_t Number type
 * \vprivate        Vector component internal API
 */
template <>
class JNumberType<std::uint8_t> final {
 public:
  /*!
   * \brief           Constructs a Number type
   * \vprivate        Vector component internal API
   *
   * \param[in]       num
   *                  Number to serialize.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \spec
   * requires true;
   * \endspec
   */
  constexpr explicit JNumberType(std::uint8_t num) noexcept : value_(static_cast<std::uint32_t>(num)) {}

  /*!
   * \brief           Returns the contained value
   * \vprivate        Vector component internal API
   *
   * \return          The value.
   * \context         ANY
   * \pre             -
   * \threadsafe      TRUE, for different this pointer
   * \spec
   * requires true;
   * \endspec
   */
  auto GetValue() const noexcept -> std::uint32_t { return this->value_; }

 private:
  /*!
   * \brief           Wrapped number value
   */
  std::uint32_t value_;
};

/*!
 * \brief           A std::int8_t Number type
 *
 * \vprivate        Vector component internal API
 */
template <>
class JNumberType<std::int8_t> final {
 public:
  /*!
   * \brief           Constructs a Number type
   * \vprivate        Vector component internal API
   *
   * \param[in]       num
   *                  Number to serialize.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \spec
   * requires true;
   * \endspec
   */
  constexpr explicit JNumberType(std::int8_t num) noexcept : value_(static_cast<std::int32_t>(num)) {}

  /*!
   * \brief           Returns the contained value
   * \vprivate        Vector component internal API
   *
   * \return          The value.
   * \context         ANY
   * \pre             -
   * \threadsafe      TRUE, for different this pointer
   * \spec
   * requires true;
   * \endspec
   */
  auto GetValue() const noexcept -> std::int32_t { return this->value_; }

 private:
  /*!
   * \brief           Wrapped number value
   */
  std::int32_t value_;
};

/*!
 * \brief           Serializes a Number value
 * \vpublic
 * \tparam          N
 *                  Type of number.
 * \param[in]       n
 *                  The number to serialize.
 * \return          The serializable number type.
 *
 * \context         ANY
 * \pre             -
 * \threadsafe      FALSE
 * \reentrant       FALSE
 * \synchronous     -
 * \spec
 * requires true;
 * \endspec
 */
template <typename N, typename = typename std::enable_if<std::is_arithmetic<N>::value>::type>
constexpr auto JNumber(N n) noexcept -> JNumberType<N> {
  return JNumberType<N>{n};
}

/*!
 * \brief           A String type
 *
 * \vprivate        Vector component internal API
 */
class JStringType final {
 public:
  /*!
   * \brief           Constructs a String type
   * \vprivate        Vector component internal API
   *
   * \param[in]       s
   *                  String to serialize.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \spec
   * requires true;
   * \endspec
   */
  constexpr explicit JStringType(std::string_view s) noexcept : value_(s) {}

  /*!
   * \brief           Returns the contained value
   * \vprivate        Vector component internal API
   *
   * \return          The value.
   * \context         ANY
   * \pre             -
   * \threadsafe      TRUE, for different this pointer
   * \spec
   * requires true;
   * \endspec
   */
  auto GetValue() const noexcept -> std::string_view { return this->value_; }

 private:
  /*!
   * \brief           Wrapped string value
   */
  std::string_view value_;
};

/*!
 * \brief           Serializes a String value
 * \vpublic
 *
 * \param[in]       s
 *                  String to serialize.
 * \return          The serializable string type.
 *
 * \context         ANY
 * \pre             -
 * \threadsafe      FALSE
 * \reentrant       FALSE
 * \synchronous     -
 * \spec
 * requires true;
 * \endspec
 */
constexpr auto JString(std::string_view s) noexcept -> JStringType { return JStringType(s); }

/*!
 * \brief           Serializes a String value
 * \vpublic
 *
 * \param[in]       s
 *                  String to serialize.
 * \return          The serializable string type.
 *
 * \context         ANY
 * \pre             -
 * \threadsafe      FALSE
 * \reentrant       FALSE
 * \synchronous     -
 * \spec
 * requires true;
 * \endspec
 */
inline auto JString(std::string const& s) noexcept -> JStringType { return JString(std::string_view{s}); }

/*!
 * \brief           Serializes a String value
 * \vpublic
 *
 * \param[in]       s
 *                  String to serialize.
 * \return          The serializable string type.
 *
 * \context         ANY
 * \pre             -
 * \threadsafe      FALSE
 * \reentrant       FALSE
 * \synchronous     -
 * \spec
 * requires true;
 * \endspec
 */
inline auto JString(std::string const& s) noexcept -> JStringType { return JString(std::string_view{s}); }

/*!
 * \brief           A function object used to serialize predefined serializers
 * \vprivate        Vector component internal API
 * \tparam          Container
 *                  Type of container to serialize.
 */
template <typename Container>
class IdSerializer {
 public:
  /*!
   * \brief           Value Type of container
   * \vprivate        Vector component internal API
   */
  using value_type = typename Container::value_type;

  /*!
   * \brief           Returns the unchanged value
   * \vprivate        Vector component internal API
   *
   * \param[in]       v
   *                  Value to return.
   * \return          The unchanged value.
   * \context         ANY
   * \pre             -
   * \threadsafe      TRUE, for different this pointer
   * \spec
   * requires true;
   * \endspec
   */
  template <typename Value = value_type>
  auto operator()(Value&& v) const noexcept -> Value {
    return std::forward<Value>(v);
  }
};

// clang-format off
}  // namespace types
// // clang-format on
}  // namespace json
}  // namespace amsr
#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_TYPES_BASIC_TYPES_H_
