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

#ifndef SCORE_RESULT_RUST_RESULT_CXX_H
#define SCORE_RESULT_RUST_RESULT_CXX_H

#include "score/result/result.h"

#include <rust/cxx.h>

#include <memory>
#include <type_traits>

template <typename T>
struct rust::IsRelocatable<::score::Result<std::unique_ptr<T>>> : std::true_type
{
};

template <typename T>
struct rust::IsRelocatable<::score::Result<std::shared_ptr<T>>> : std::true_type
{
};

#endif  // SCORE_RESULT_RUST_RESULT_CXX_H
