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
#include "score/analysis/tracing/generic_trace_library/interface_types/generic_trace_api.h"

namespace score
{
namespace analysis
{
namespace tracing
{

ITraceLibrary* GenericTraceAPI::gMock{nullptr};

RegisterClientResult GenericTraceAPI::RegisterClient(const BindingType type, const std::string& app_instance_identifier)
{
    if (gMock != nullptr)
    {
        return gMock->RegisterClient(type, app_instance_identifier);
    }
    return static_cast<TraceClientId>(0);
}

RegisterSharedMemoryObjectResult GenericTraceAPI::RegisterShmObject(const TraceClientId client,
                                                                    const std::string& shm_object_path) noexcept
{
    if (gMock != nullptr)
    {
        return gMock->RegisterShmObject(client, shm_object_path);
    }
    return 0;
}

RegisterSharedMemoryObjectResult GenericTraceAPI::RegisterShmObject(const TraceClientId client,
                                                                    const std::int32_t shm_object_fd) noexcept
{
    if (gMock != nullptr)
    {
        return gMock->RegisterShmObject(client, shm_object_fd);
    }
    //return static_cast<ShmObjectHandle>(0);
    return 0;
}

ResultBlank GenericTraceAPI::UnregisterShmObject(const TraceClientId client, const ShmObjectHandle handle)
{
    if (gMock != nullptr)
    {
        return gMock->UnregisterShmObject(client, handle);
    }
    return {};
}

RegisterTraceDoneCallBackResult GenericTraceAPI::RegisterTraceDoneCB(const TraceClientId client,
                                                                     TraceDoneCallBackType trace_done_callback)
{
    if (gMock != nullptr)
    {
        return gMock->RegisterTraceDoneCB(client, std::move(trace_done_callback));
    }
    return {};
}

TraceResult GenericTraceAPI::Trace(const TraceClientId client,
                                   const MetaInfoVariants::Type& meta_info,
                                   ShmDataChunkList& data,
                                   TraceContextId context_id)
{
    if (gMock != nullptr)
    {
        return gMock->Trace(client, meta_info, data, context_id);
    }
    return {};
}

TraceResult GenericTraceAPI::Trace(const TraceClientId client,
                                   const MetaInfoVariants::Type& meta_info,
                                   LocalDataChunkList& data) noexcept
{
    if (gMock != nullptr)
    {
        return gMock->Trace(client, meta_info, data);
    }
    return {};
}

void GenericTraceAPI::InjectMock(ITraceLibrary* mock) noexcept
{
    gMock = mock;
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
