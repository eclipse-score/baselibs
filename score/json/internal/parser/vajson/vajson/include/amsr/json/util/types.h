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
 *        \brief  Type definitions for vaJson.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_UTIL_TYPES_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_UTIL_TYPES_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <string>
#include <string_view>

#include "score/language/safecpp/string_view/zstring_view.h"
#include "score/optional.hpp"
#include "score/span.hpp"

namespace amsr
{
namespace json
{
/*!
 * \brief           Unqualified access to Optional
 * \tparam          T
 *                  Type of value.
 */
template <typename T>
using Optional = score::cpp::optional<T>;

/// \brief           Unqualified access to CStringView
using CStringView = score::safecpp::zstring_view;

/// \brief           Unqualified access to StringView
using StringView = std::string_view;

/// \brief           Unqualified access to String
using String = std::string;

/// \brief           Unqualified access to binary data
using Bytes = score::cpp::span<const char>;

/// \brief           Short form for creating a StringView
using std::literals::string_view_literals::operator""sv;

/// \brief           The encoding of the document
enum class EncodingType : std::uint8_t
{
    kNone,
    kUtf8,
};

}  // namespace json
}  // namespace amsr

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_UTIL_TYPES_H_
