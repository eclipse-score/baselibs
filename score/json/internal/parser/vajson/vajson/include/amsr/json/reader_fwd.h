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
 *        \brief  Forward declarations of public JSON types.
 *
 *********************************************************************************************************************/
#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_READER_FWD_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_READER_FWD_H_

namespace score
{
namespace json
{
namespace vajson
{

/// \brief           Forward declaration for JsonData
class JsonData;

namespace internal
{
/// \brief           Forward declaration for JsonOps

class JsonOps;
}  // namespace internal

/// \brief           Forward declaration for JsonParser
class JsonParser;

namespace v2
{
/// \brief           Forward declaration for SingleArrayParser
class SingleArrayParser;
/// \brief           Forward declaration for SingleObjectParser
class SingleObjectParser;
/// \brief           Forward declaration for Parser
class Parser;
}  // namespace v2
}  // namespace vajson
}  // namespace json
}  // namespace score
#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_READER_FWD_H_
