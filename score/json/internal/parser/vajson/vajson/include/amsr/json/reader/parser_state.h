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
 *        \brief  Contains the definition of the parser state.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_READER_PARSER_STATE_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_READER_PARSER_STATE_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "score/result/result.h"

namespace amsr {
namespace json {
/*!
 * \brief           State that is used for every parser event
 *
 * \vpublic
 */
enum class ParserState : bool { kRunning, kFinished };

/*!
 * \brief           Return type of most parser functions
 *
 * \vpublic
 */
using ParserResult = score::Result<ParserState>;
}  // namespace json
}  // namespace amsr

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_READER_PARSER_STATE_H_
