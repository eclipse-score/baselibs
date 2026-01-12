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
#ifndef SCORE_ANALYSIS_TRACING_COMMON_FLEXIBLE_CIRCULAR_ALLOCATOR_ERROR_CODE_H
#define SCORE_ANALYSIS_TRACING_COMMON_FLEXIBLE_CIRCULAR_ALLOCATOR_ERROR_CODE_H

#include "score/result/error.h"
#include <score/string_view.hpp>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief Error code that method can return
enum class FlexibleAllocatorErrorCode : score::result::ErrorCode
{
    kNoError = 0,
    kBaseAddressVoid,
    kCorruptedBufferBlock,
    kInvalidListEntryOffset,
    kSizeIsZero,
};

class FlexibleAllocatorErrorDomain final : public score::result::ErrorDomain
{
  public:
    std::string_view MessageFor(const score::result::ErrorCode& code) const noexcept override;
};

score::result::Error MakeError(const score::analysis::tracing::FlexibleAllocatorErrorCode code,
                             const std::string_view user_message = "") noexcept;

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_COMMON_FLEXIBLE_CIRCULAR_ALLOCATOR_ERROR_CODE_H
