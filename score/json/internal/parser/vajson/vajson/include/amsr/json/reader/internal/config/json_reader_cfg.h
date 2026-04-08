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
 *        \brief  Configuration data of vaJson.
 *
 *      \details  Contains the application specific configuration.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_READER_INTERNAL_CONFIG_JSON_READER_CFG_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_READER_INTERNAL_CONFIG_JSON_READER_CFG_H_

namespace amsr {
namespace json {
namespace internal {
namespace config {
/*!
 * \brief           Size of the key buffer
 */

constexpr static std::size_t kKeyBufferSize{1024};

/*!
 * \brief           Size of the string buffer
 */

constexpr static std::size_t kStringBufferSize{1024};
}  // namespace config
}  // namespace internal
}  // namespace json
}  // namespace amsr

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_READER_INTERNAL_CONFIG_JSON_READER_CFG_H_
