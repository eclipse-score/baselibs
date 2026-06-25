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
#ifndef SCORE_ANALYSIS_TRACING_COMMON_SHARED_MEMORY_LOCATION_H
#define SCORE_ANALYSIS_TRACING_COMMON_SHARED_MEMORY_LOCATION_H

#include "types.h"

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief SharedMemoryLocation class
///
/// Class used to store single address of data that needs to be traced.
/// It stores handle to the shared-memory object and offset that points to the region
/// where the data is stored.
struct SharedMemoryLocation
{
    ShmObjectHandle shm_object_handle_;  ///< Handle to the shared-memory object
    std::size_t offset_;                 ///< Offset in memory where the data that needs to be traced is stored
};
bool operator==(const SharedMemoryLocation& lhs, const SharedMemoryLocation& rhs) noexcept;
static_assert(std::is_trivial<SharedMemoryLocation>::value == true, "SharedMemoryLocation is not trivial");

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif
