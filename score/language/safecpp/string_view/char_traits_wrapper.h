/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
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
#ifndef SCORE_LANGUAGE_SAFECPP_STRING_VIEW_CHAR_TRAITS_WRAPPER_H
#define SCORE_LANGUAGE_SAFECPP_STRING_VIEW_CHAR_TRAITS_WRAPPER_H

#include "score/quality/compiler_warnings/warnings.h"

#include <string>
#include <type_traits>

// NOLINTBEGIN(readability-identifier-naming): STL-style notation is intended here
// in order to facilitate interoperability with other STL-like containters/algorithms

namespace score::safecpp
{

template <typename CharType>
struct char_traits_wrapper
{
    static_assert(
        std::disjunction_v<std::is_same<CharType, char>,
                           // NOLINTNEXTLINE(score-banned-type) wchar_t is only used for static assertion check here
                           std::is_same<CharType, wchar_t>,
#ifdef __cpp_char8_t
                           std::is_same<CharType, char8_t>,
#endif
                           std::is_same<CharType, char16_t>,
                           std::is_same<CharType, char32_t>>,
        "Instantiating the class template `safecpp::char_traits_wrapper` with a character type other "
        "than char, wchar_t, char8_t, char16_t or char32_t is non-standard and therefore not permitted.");

// Suppress incorrect deprecation warnings which get emitted only by QNX's QCC. And there, only from version 8.0.0 on.
// Rationale: We already ensure via the above static assertion that `CharType` is one of the compliant ones
//            according to the C++ standard. Hence, the class template `std::char_traits` can never get
//            instantiated with a non-compliant character type here.
// This issue got already reported to Blackberry (see Ticket-240117).
#if defined(__QNX__) && __QNX__ >= 800 && __QNX__ < 900  // from QCC version 9.0.0 on, we expect that to be fixed again
    // clang-format off
    DISABLE_WARNING_PUSH
    DISABLE_WARNING(-Wdeprecated-declarations)
    using traits_type = std::char_traits<CharType>;
    DISABLE_WARNING_POP
    // clang-format on
#else
    using traits_type = std::char_traits<CharType>;
#endif
};

}  // namespace score::safecpp

// NOLINTEND(readability-identifier-naming)

#endif  // SCORE_LANGUAGE_SAFECPP_STRING_VIEW_CHAR_TRAITS_WRAPPER_H
