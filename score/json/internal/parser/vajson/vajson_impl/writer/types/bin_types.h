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
 *        \brief  Implementation of methods for Bin type.
 *
 *      \details  Provides serializers for arrays and tuples.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_TYPES_BIN_TYPES_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_TYPES_BIN_TYPES_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <limits>
#include <string>
#include "amsr/json/util/types.h"
#include "amsr/msra_always_assert.h"

namespace amsr {
namespace json {
namespace internal {
/*!
 * \brief           Safely convert a length
 * \param[in]       length
 *                  to convert.
 * \return          The converted length.
 *
 * \context         ANY
 * \pre             -
 * \threadsafe      TRUE, for different this pointer
 * \spec
 * requires true;
 * \endspec
 */
inline auto ConvertLength(std::size_t const length) noexcept -> std::uint32_t {
  msra_always_assert(length <= std::numeric_limits<std::uint32_t>::max());

  // VECTOR NCL AutosarC++17_10-A4.7.1: MD_JSON_AutosarC++17_10-A4.7.1_truncating_cast
  // VECTOR NL AutosarC++17_10-M0.3.1: MD_JSON_AutosarC++17_10-M0.3.1_OOB
  return static_cast<std::uint32_t>(length);
}

}  // namespace internal

inline namespace types {
/*!
 * \brief           A binary type
 *
 * \vprivate        Vector component internal API
 */
class JBinType final {
 public:
  /*!
   * \brief           Constructs a binary type
   * \vprivate        Vector component internal API
   * \param[in]       b
   *                  Bytes to serialize.
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
  constexpr explicit JBinType(score::cpp::span<char const> b) noexcept : value_(b) {}

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
  auto GetValue() const noexcept -> score::cpp::span<char const> { return this->value_; }

  /*!
   * \brief           Returns the length
   * \vprivate        Vector component internal API
   *
   * \return          The length.
   * \context         ANY
   * \pre             -
   * \threadsafe      TRUE, for different this pointer
   * \spec
   * requires true;
   * \endspec
   */
  auto GetLength() const noexcept -> std::uint32_t { return internal::ConvertLength(this->value_.size()); }

 private:
  /*!
   * \brief           Wrapped binary value
   */
  score::cpp::span<char const> value_;
};

/*!
 * \brief           Serializes a binary value
 * \vpublic
 *
 * \param[in]       b
 *                  Bytes to serialize.
 * \return          The serializable binary value.
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
constexpr auto JBin(score::cpp::span<char const> b) noexcept -> JBinType { return JBinType{b}; }

/*!
 * \brief           A binary string type
 *
 * \vprivate        Vector component internal API
 */
class JBinStringType final {
 public:
  /*!
   * \brief           Constructs a binary string
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
  constexpr explicit JBinStringType(StringView s) noexcept : value_(s) {}

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
  auto GetValue() const noexcept -> StringView { return this->value_; }

  /*!
   * \brief           Returns the length
   * \vprivate        Vector component internal API
   *
   * \return          The length.
   * \context         ANY
   * \pre             -
   * \threadsafe      TRUE, for different this pointer
   * \spec
   * requires true;
   * \endspec
   */
  auto GetLength() const noexcept -> std::uint32_t { return internal::ConvertLength(this->value_.size()); }

 private:
  /*!
   * \brief           Wrapped string value
   */
  StringView value_;
};

/*!
 * \brief           Serializes a string as binary
 * \vpublic
 *
 * \param[in]       s
 *                  The string to serialize.
 * \return          The serializable string value.
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
constexpr auto JBinString(StringView s) noexcept -> JBinStringType { return JBinStringType{s}; }

/*!
 * \brief           Serializes a string as binary
 * \vpublic
 *
 * \param[in]       s
 *                  The string to serialize.
 * \return          The serializable string value.
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
inline auto JBinString(std::string const& s) noexcept -> JBinStringType {
  return JBinString(std::string_view{s});
}

/*!
 * \brief           Serializes a string as binary
 * \vpublic
 *
 * \param[in]       s
 *                  The string to serialize.
 * \return          The serializable string value.
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
inline auto JBinString(std::string const& s) noexcept -> JBinStringType { return JBinString(std::string_view{s}); }

/*!
 * \brief           A binary key type
 *
 * \vprivate        Vector component internal API
 */
class JBinKeyType final {
 public:
  /*!
   * \brief           Constructs a binary key
   * \vprivate        Vector component internal API
   *
   * \param[in]       s
   *                  Key to serialize.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \spec
   * requires true;
   * \endspec
   */
  constexpr explicit JBinKeyType(StringView s) noexcept : value_(s) {}

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
  auto GetValue() const noexcept -> StringView { return this->value_; }

  /*!
   * \brief           Returns the length
   * \vprivate        Vector component internal API
   *
   * \return          The length.
   * \context         ANY
   * \pre             -
   * \threadsafe      TRUE, for different this pointer
   * \spec
   * requires true;
   * \endspec
   */
  auto GetLength() const noexcept -> std::uint32_t { return internal::ConvertLength(this->value_.size()); }

 private:
  /*!
   * \brief           Wrapped key value
   */
  StringView value_;
};

/*!
 * \brief           Serializes a key as binary
 * \vpublic
 *
 * \param[in]       s
 *                  The key to serialize.
 * \return          The serializable key value.
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
constexpr auto JBinKey(StringView s) noexcept -> JBinKeyType { return JBinKeyType{s}; }

/*!
 * \brief           Serializes a key as binary
 * \vpublic
 *
 * \param[in]       s
 *                  The key to serialize.
 * \return          The serializable key value.
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
inline auto JBinKey(std::string const& s) noexcept -> JBinKeyType { return JBinKey(std::string_view{s}); }

/*!
 * \brief           Serializes a key as binary
 * \vpublic
 *
 * \param[in]       s
 *                  The key to serialize.
 * \return          The serializable key value.
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
inline auto JBinKey(std::string const& s) noexcept -> JBinKeyType { return JBinKey(std::string_view{s}); }
// clang-format off
}  // inline namespace types
// clang-format off
}  // namespace json
}  // namespace amsr
#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_TYPES_BIN_TYPES_H_
