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
#ifndef SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_META_INFO_VARIANTS_H
#define SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_META_INFO_VARIANTS_H

#include "ara_com_meta_info.h"
#include "dlt_meta_info.h"
#include <score/variant.hpp>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief MetaInfoVariants class
class MetaInfoVariants
{
  public:
    /// @brief Datatype used to store meta info data in std::variant
    using Type = score::cpp::variant<AraComMetaInfo, DltMetaInfo>;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_META_INFO_VARIANTS_H
