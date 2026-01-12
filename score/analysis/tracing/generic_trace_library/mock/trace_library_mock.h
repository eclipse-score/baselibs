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
#ifndef SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_MOCK_TRACE_LIBRARY_MOCK_H
#define SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_MOCK_TRACE_LIBRARY_MOCK_H

#include "gmock/gmock.h"
#include "score/analysis/tracing/generic_trace_library/interface_types/generic_trace_api.h"
#include "score/analysis/tracing/generic_trace_library/interface_types/i_trace_library.h"

namespace score
{
namespace analysis
{
namespace tracing
{

class TraceLibraryMock : public ITraceLibrary
{
  public:
    TraceLibraryMock()
    {
        GenericTraceAPI::InjectMock(this);
    }
    void Uninject()
    {
        GenericTraceAPI::InjectMock(nullptr);
    }
    MOCK_METHOD(RegisterClientResult,
                RegisterClient,
                (const BindingType type, const std::string& client_description),
                (override));
    MOCK_METHOD(RegisterSharedMemoryObjectResult,
                RegisterShmObject,
                (const TraceClientId client, const std::string& shm_object_path),
                (override));
    MOCK_METHOD(RegisterSharedMemoryObjectResult,
                RegisterShmObject,
                (const TraceClientId client, int shm_object_fd),
                (override));
    MOCK_METHOD(ResultBlank, UnregisterShmObject, (const TraceClientId client, ShmObjectHandle handle), (override));
    MOCK_METHOD(RegisterTraceDoneCallBackResult,
                RegisterTraceDoneCB,
                (const TraceClientId client, TraceDoneCallBackType trace_done_callback),
                (override));
    MOCK_METHOD(TraceResult,
                Trace,
                (const TraceClientId client,
                 const MetaInfoVariants::Type& meta_info,
                 ShmDataChunkList& data,
                 TraceContextId context_id),
                (override));
    MOCK_METHOD(TraceResult,
                Trace,
                (const TraceClientId client, const MetaInfoVariants::Type& meta_info, LocalDataChunkList& data),
                (noexcept, override));
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score
#endif  // SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_MOCK_TRACE_LIBRARY_MOCK_H
