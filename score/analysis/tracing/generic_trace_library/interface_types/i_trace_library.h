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
#ifndef SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_I_TRACE_LIBRARY_H
#define SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_I_TRACE_LIBRARY_H

#include "meta_info_variants.h"
#include "score/analysis/tracing/common/interface_types/types.h"
#include "score/analysis/tracing/generic_trace_library/interface_types/chunk_list/local_data_chunk_list.h"
#include "score/analysis/tracing/generic_trace_library/interface_types/chunk_list/shm_data_chunk_list.h"

namespace score
{
namespace analysis
{
namespace tracing
{
// No need to define the rule of five for that class
// coverity[autosar_cpp14_a12_8_6_violation]
// coverity[autosar_cpp14_a12_0_1_violation]
class ITraceLibrary
{
  public:
    virtual ~ITraceLibrary() = default;
    virtual RegisterClientResult RegisterClient(const BindingType binding_type,
                                                const std::string& client_description) = 0;
    virtual RegisterSharedMemoryObjectResult RegisterShmObject(const TraceClientId trace_client_id,
                                                               const std::string& shm_object_path) = 0;
    virtual RegisterSharedMemoryObjectResult RegisterShmObject(const TraceClientId trace_client_id,
                                                               int shm_object_fd) = 0;
    virtual ResultBlank UnregisterShmObject(const TraceClientId trace_client_id, const ShmObjectHandle handle) = 0;
    virtual RegisterTraceDoneCallBackResult RegisterTraceDoneCB(const TraceClientId trace_client_id,
                                                                TraceDoneCallBackType trace_done_callback) = 0;
    virtual TraceResult Trace(const TraceClientId trace_client_id,
                              const MetaInfoVariants::Type& meta_info,
                              ShmDataChunkList& data,
                              TraceContextId context_id) = 0;
    virtual TraceResult Trace(const TraceClientId trace_client_id,
                              const MetaInfoVariants::Type& meta_info,
                              LocalDataChunkList& data) noexcept = 0;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score
#endif  // SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_I_TRACE_LIBRARY_H
