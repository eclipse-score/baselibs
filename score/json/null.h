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

#ifndef SCORE_JSON_NULL_H
#define SCORE_JSON_NULL_H

namespace score
{
namespace json
{

class Null
{
  public:
    Null() = default;
    Null(const Null&) = default;
    Null(Null&&) = default;
    Null& operator=(const Null&) = default;
    Null& operator=(Null&&) noexcept = default;
    ~Null() = default;
};

bool operator==(const Null&, const Null&) noexcept;

}  // namespace json
}  // namespace score

#endif  // SCORE_JSON_NULL_H
