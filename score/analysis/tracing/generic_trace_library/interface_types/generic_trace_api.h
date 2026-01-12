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
#ifndef SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_GENERIC_TRACE_API_H
#define SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_GENERIC_TRACE_API_H

#include "meta_info_variants.h"
#include "score/analysis/tracing/common/interface_types/types.h"
#include "score/analysis/tracing/generic_trace_library/interface_types/chunk_list/local_data_chunk_list.h"
#include "score/analysis/tracing/generic_trace_library/interface_types/chunk_list/shm_data_chunk_list.h"
#include "score/analysis/tracing/generic_trace_library/interface_types/error_code/error_code.h"
#include "score/result/result.h"
#include <score/callback.hpp>
#include <score/expected.hpp>
#include <cstdint>

#include "i_trace_library.h"

namespace score
{
namespace analysis
{

namespace tracing
{

class TraceLibraryMock;
class TraceLibraryItfMock;

/// @brief GenericTraceAPI class
///
/// Static interface to use Generic Trace API functionalities.
// This is false positive. GenericTraceAPI is declared only once in the library side.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
class GenericTraceAPI
{

  public:
    /// @brief Constructor deleted. Only static methods can be used.
    GenericTraceAPI() = delete;

    /// @brief Register client to the API.
    /// This should be the first method called by an upper layer. First call to this method will start initialization of
    /// the library. This method can be called after the Daemon process is started. Clients are registered
    /// internally and no communication with the Daemon is necessary. TraceClientId that is returned is later used to
    /// identify the respective client which calls on other library methods. Configurations such as callbacks are also
    /// bounds to this id.
    ///
    /// @note If a client uses multiple bindings, it has to be registered for each binding type.
    ///
    /// @param type The binding used by the client.
    /// @param app_instance_identifier Description of the client
    ///
    /// @return A TraceClientID or error code (kInvalidAppInstanceIdFatal, kInvalidBindingTypeFatal,
    /// kDaemonNotConnectedFatal) if operation was not successful.
    static RegisterClientResult RegisterClient(const BindingType type, const std::string& app_instance_identifier);

    /// @brief Register shared-memory object.
    /// Method used to register shared-memory (SHM) object within library. It is needed to provide possibility to use
    /// same region of memory by both upper layer and Generic Trace library. After successful registration path to
    /// the SHM object will be later represented by ShmObjectHandle that allows some optimization in access.
    ///
    /// @param client Id of trace client
    /// @param path Path to the shared-memory object
    ///
    /// @return A handle to shared-memory object or error code
    /// (kInvalidArgumentFatal, kDaemonNotConnectedFatal, kBadFileDescriptorFatal,
    /// kSharedMemoryObjectNotInTypedMemoryFatal, kSharedMemoryObjectRegistrationFailedFatal,
    /// kMessageSendFailedRecoverable, kSharedMemoryObjectHandleCreationFailedFatal, kClientNotFoundRecoverable) if
    /// operation was not successful
    static RegisterSharedMemoryObjectResult RegisterShmObject(const TraceClientId client,
                                                              const std::string& shm_object_path) noexcept;

    /// @brief Register shared-memory object.
    /// Overloaded version of method used to register shared-memory (SHM) object within library. It is needed to provide
    /// possibility to use same region of memory by both upper layer and Generic Trace library. After successful
    /// registration,the file descriptor is represented by a ShmObjectHandle. This allows some optimization
    /// in access.
    ///
    /// @param client Id of trace client
    /// @param shm_object_fd Shared-memory object file descriptor
    ///
    /// @return A handle to shared-memory object or error code
    /// (kInvalidArgumentFatal, kDaemonNotConnectedFatal, kSharedMemoryObjectNotInTypedMemoryFatal,
    /// kSharedMemoryObjectRegistrationFailedFatal, kMessageSendFailedRecoverable,
    /// kSharedMemoryObjectHandleCreationFailedFatal, kClientNotFoundRecoverable) if operation was not successful
    static RegisterSharedMemoryObjectResult RegisterShmObject(const TraceClientId client,
                                                              const std::int32_t shm_object_fd) noexcept;

    /// @brief Unregister shared-memory object.
    /// Method used to unregister previously registered shared-memory object. Upon return from this function, the client
    /// may reuse or delete the shared memory object. The backend may still access the shared memory region for some
    /// time after this call returns, as there may be pending traces. To avoid any data loss the client needs to ensure
    /// that there is no pending trace before unregistering. In the worst case scenario the backend will access its own
    /// private mapping and send out garbage.
    ///
    /// @param handle Handle to previously registered shared-memory object
    /// @return a blank or error code (kDaemonNotConnectedFatal, kSharedMemoryObjectUnregisterFailedFatal,
    /// kMessageSendFailedRecoverable, kClientNotFoundRecoverable) if operation was not successful
    static ResultBlank UnregisterShmObject(const TraceClientId client, const ShmObjectHandle handle);

    /// @brief Register a callback to be invoked once a trace has completed.
    /// Method used to register callback method that will be called when the trace operation is done. Each client has
    /// its own callback method. Callback method should be of score::cpp::callback<void(TraceContextId)> type.
    /// The callback will be invoked from a different thread context and should be thread-safe.
    ///
    /// @param client Id of trace client
    /// @param trace_done_callback TraceDoneCallBackType method used as callback
    ///
    /// @note Subsequent callback invocations are handled sequentially.
    ///
    /// @return An error code (kDaemonNotConnectedFatal, kCallbackAlreadyRegisteredRecoverable,
    /// kClientNotFoundRecoverable) in case where register operation was not successful
    static RegisterTraceDoneCallBackResult RegisterTraceDoneCB(const TraceClientId client,
                                                               TraceDoneCallBackType trace_done_callback);

    /// @brief Trace the data placed in shared-memory region
    /// Method used to perform actual trace of the data. This version provides interface to trace data placed already
    /// in shared memory without doing any copies. Only the meta-info passed to this function by reference will be
    /// copied to another shared-memory region by library. The library does not access at any point the memory during
    /// its processing. The shared memory regions where the data to be traced is located must have been previously
    /// registered via a call to RegisterShmObject(). Once the backend has processed the trace data, the client will be
    /// notified via the callback registered via RegisterTraceDoneCB() using the context_id provided here as an
    /// argument. The callback may be invoked even if the memory is unregistered during an on going trace operation.
    ///
    /// @param meta_info Meta info data
    /// @param data List of data chunks placed in shared-memory region which should be traced
    /// @param context_id Context id of data used to distinguish it. It should be unique for the client.
    ///
    /// @return An error code (kDaemonNotConnectedFatal, kModuleNotInitializedRecoverable,
    /// kNotEnoughMemoryRecoverable, kRingBufferNotInitializedRecoverable, kRingBufferInvalidStateRecoverable,
    /// kRingBufferFullRecoverable, kRingBufferNoEmptyElementRecoverable, kRingBufferNoReadyElementRecoverable,
    /// kClientNotFoundRecoverable) in case where trace operation was not successful
    static TraceResult Trace(const TraceClientId client,
                             const MetaInfoVariants::Type& meta_info,
                             ShmDataChunkList& data,
                             TraceContextId context_id);

    /// @brief Trace data placed in local memory region
    /// Method used to perform actual trace of the data. This version provides interface to trace data placed in local
    /// memory so whole process of copying this data to shared-memory will be taken out by the library. Meta info passed
    /// to this function by reference will be copied to another shared-memory region by library and trace daemon will be
    /// fed this data which allows it to send it out. Context_id is later used to notify upper layer that the trace
    /// data was successfully sent out so it should be unique identifier.
    ///
    /// @param meta_info Meta info data
    /// @param data List of data chunks placed in local memory region which should be traced.
    ///             This data will be copied to shared-memory region be GenericTraceAPI
    ///
    /// @return An error code (kDaemonNotConnectedFatal, kModuleNotInitializedRecoverable,
    /// kNotEnoughMemoryRecoverable, kRingBufferNotInitializedRecoverable, kRingBufferInvalidStateRecoverable,
    /// kRingBufferFullRecoverable, kRingBufferNoEmptyElementRecoverable, kRingBufferNoReadyElementRecoverable,
    /// kClientNotFoundRecoverable) in case where trace operation was not successful
    static TraceResult Trace(const TraceClientId client,
                             const MetaInfoVariants::Type& meta_info,
                             LocalDataChunkList& data) noexcept;

  private:
    /// @brief Gives access to the currently used underlying trace library instance.
    static ITraceLibrary& GetInstance();

    /// @brief Allow the user to inject mock for testability
    /// @param The new test instance to be set.
    static void InjectMock(ITraceLibrary* mock) noexcept;

    /// @brief mocking the underlying trace library for the user testability
    static ITraceLibrary* gMock;
    // No harm from using friend keyword as it used here to let TraceLibraryMock access the private members of
    // GenericTraceAPI
    //  coverity[autosar_cpp14_a11_3_1_violation]
    friend class TraceLibraryMock;
    // Suppres "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used."
    // No harm from using friend keyword as it used here to let TraceLibraryMock access the private members of
    // GenericTraceAPI
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class TraceLibraryItfMock;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_GENERIC_TRACE_API_H
