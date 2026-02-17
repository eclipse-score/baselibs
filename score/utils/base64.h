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
#ifndef SCORE_LIB_UTILS_BASE64_H
#define SCORE_LIB_UTILS_BASE64_H

#include <cstdint>
#include <string>
#include <vector>

namespace score::utils
{

std::string EncodeBase64(const std::vector<std::uint8_t>& buffer);
std::vector<std::uint8_t> DecodeBase64(const std::string& encoded_string);

}  // namespace score::utils

#endif  // SCORE_LIB_UTILS_BASE64_H
