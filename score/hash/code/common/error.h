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

#ifndef SCORE_LIB_HASH_CODE_COMMON_ERROR_H
#define SCORE_LIB_HASH_CODE_COMMON_ERROR_H

#include "score/result/error.h"
#include "score/result/error_code.h"
#include "score/result/error_domain.h"

#include <score/string_view.hpp>

namespace score
{
namespace hash
{

/* KW_SUPPRESS_START:UNUSED.STYLE.SINGLE_STMT_PER_LINE: we do want to define an enum on multiple lines */
/* KW_SUPPRESS_START:MISRA.ONEDEFRULE.VAR: false positive, this is not a variable definition, so ODR doesn't apply */
enum class ErrorCode : score::result::ErrorCode
{
    kCouldNotCreateDigest,
    kCouldNotUpdateDigest,
    kCouldNotUpdateDigestFromStream,
    kCouldNotFinalizeDigest,
    kInvalidParameters,
    kNotFound,
    kStreamError,
    kUnknownError,
};
/* KW_SUPPRESS_END:MISRA.ONEDEFRULE.VAR */
/* KW_SUPPRESS_END:UNUSED.STYLE.SINGLE_STMT_PER_LINE */

/* KW_SUPPRESS_START:MISRA.ONEDEFRULE.VAR: FP: noexcept is a keyword and not an identifier, so ODR doesn't apply */
/* KW_SUPPRESS_START:UNUSED.STYLE.SINGLE_STMT_PER_LINE: False positive, this is exactly one statement */
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: False positive, noexcept is not an identifier */
score::result::Error MakeError(const score::hash::ErrorCode code, const std::string_view user_message = "") noexcept;
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN */
/* KW_SUPPRESS_END:UNUSED.STYLE.SINGLE_STMT_PER_LINE */
/* KW_SUPPRESS_END:MISRA.ONEDEFRULE.VAR */

}  // namespace hash
}  // namespace score
#endif  // SCORE_LIB_HASH_CODE_COMMON_ERROR_H
