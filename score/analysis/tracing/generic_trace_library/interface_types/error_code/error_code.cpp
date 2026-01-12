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
#include "score/analysis/tracing/generic_trace_library/interface_types/error_code/error_code.h"
// SCORE_CCM_NO_LINT No harm from  using switch_case
std::string_view score::analysis::tracing::GenericTraceAPIErrorDomain::MessageFor(
    const score::result::ErrorCode& code) const noexcept
{
    std::string_view error_message;  // Variable to hold the error message
    // No harm from going outside the range as that will jump to the default state in switch_case(a7_2_1),No harm to
    // define the switch in that format (m6_4_3)
    //  coverity[autosar_cpp14_a7_2_1_violation]
    //  coverity[autosar_cpp14_m6_4_3_violation]
    switch (static_cast<score::analysis::tracing::ErrorCode>(code))
    {
        case ErrorCode::kNoErrorRecoverable:
            error_message = "No error occured";
            break;
        case ErrorCode::kDaemonNotConnectedFatal:
            error_message = "Daemon was unexpectedly disconnected";
            break;
        case ErrorCode::kInvalidAppInstanceIdFatal:
            error_message = "Invalid app instance ID";
            break;
        case ErrorCode::kInvalidArgumentFatal:
            error_message = "Invalid argument";
            break;
        case ErrorCode::kInvalidBindingTypeFatal:
            error_message = "Invalid binding type";
            break;
        case ErrorCode::kNotEnoughMemoryRecoverable:
            error_message = "Not enough memory for allocation";
            break;
        case ErrorCode::kNoMetaInfoProvidedRecoverable:
            error_message = "No meta info were provided";
            break;
        case ErrorCode::kNotEnoughMemoryInContainerRecoverable:
            error_message = "Not enough memory in trace job container";
            break;
        case ErrorCode::kOffsetCalculationFailedRecoverable:
            error_message = "Offset calculation failed";
            break;
        case ErrorCode::kAtomicRingBufferFullRecoverable:
            error_message = "Atomic ring buffer full";
            break;
        case ErrorCode::kAtomicRingBufferEmptyRecoverable:
            error_message = "Atomic ring buffer empty";
            break;
        case ErrorCode::kAtomicRingBufferMaxRetriesRecoverable:
            error_message = "Atomic ring buffer access failed max retry times";
            break;
        case ErrorCode::kModuleNotInitializedRecoverable:
            error_message = "Module not initialized";
            break;
        case ErrorCode::kModuleInitializedRecoverable:
            error_message = "Module already initialized";
            break;
        case ErrorCode::kDaemonNotConnectedRecoverable:
            error_message = "Daemon is not yet available";
            break;
        case ErrorCode::kRingBufferFullRecoverable:
            error_message = "Ring buffer full";
            break;
        case ErrorCode::kRingBufferEmptyRecoverable:
            error_message = "Ring buffer empty";
            break;
        case ErrorCode::kRingBufferNoEmptyElementRecoverable:
            error_message = "No empty element in the ring buffer could be acquired";
            break;
        case ErrorCode::kRingBufferNoReadyElementRecoverable:
            error_message = "No ready element in the ring buffer could be acquired";
            break;
        case ErrorCode::kRingBufferNotInitializedRecoverable:
            error_message = "Ring buffer not initialized";
            break;
        case ErrorCode::kRingBufferInitializedRecoverable:
            error_message = "Ring buffer already initialized";
            break;
        case ErrorCode::kRingBufferInvalidStateRecoverable:
            error_message = "Invalid state of ring buffer";
            break;
        case ErrorCode::kRingBufferInvalidMemoryResourceRecoverable:
            error_message = "Invalid memory resource passed to constructor";
            break;
        case ErrorCode::kRingBufferSharedMemoryCreationFatal:
            error_message = "Failed to create shared memory for shared memory ring buffer";
            break;
        case ErrorCode::kRingBufferSharedMemoryHandleCreationFatal:
            error_message = "Failed to create shared memory handle for shared memory ring buffer";
            break;
        case ErrorCode::kRingBufferSharedMemorySealFatal:
            error_message = "Failed to seal shared memory of shared memory ring buffer";
            break;
        case ErrorCode::kRingBufferSharedMemoryHandleOpenFatal:
            error_message = "Failed to open shared memory handle of shared memory ring buffer";
            break;
        case ErrorCode::kRingBufferSharedMemoryFstatFatal:
            error_message = "Failed to perform fstat on shared memory ring buffer file descriptor";
            break;
        case ErrorCode::kRingBufferSharedMemoryMapFatal:
            error_message = "Failed to to map memory region of shared memory ring buffer";
            break;
        case ErrorCode::kRingBufferSharedMemorySizeCalculationFatal:
            error_message =
                "Calculated shared memory size doesn't match the size of the original created shared memory";
            break;
        case ErrorCode::kNoSpaceLeftForAllocationRecoverable:
            error_message = "No space to allocate in TMD shared memory";
            break;
        case ErrorCode::kIndexOutOfBoundsInSharedListRecoverable:
            error_message = "Index is out of bounds";
            break;
        case ErrorCode::kMemoryCorruptionDetectedFatal:
            error_message = "Memory Corruption detected";
            break;
        case ErrorCode::kDaemonConnectionFailedFatal:
            error_message = "Daemon connection failed";
            break;
        case ErrorCode::kDaemonCommunicatorNotSupportedFatal:
            error_message = "Daemon communication is supported only with QNX";
            break;
        case ErrorCode::kServerConnectionNameOpenFailedFatal:
            error_message = "Server name open failed";
            break;
        case ErrorCode::kDaemonTerminationDetectionFailedFatal:
            error_message = "Daemon termination detection failed";
            break;
        case ErrorCode::kMessageSendFailedRecoverable:
            error_message = "Failed to send the message";
            break;
        case ErrorCode::kSharedMemoryObjectRegistrationFailedFatal:
            error_message = "Failed to register a shared-memory object";
            break;
        case ErrorCode::kSharedMemoryObjectAlreadyRegisteredRecoverable:
            error_message = "Shared-memory object is already registered with this file descriptor/path";
            break;
        case ErrorCode::kSharedMemoryObjectNotInTypedMemoryFatal:
            error_message = "Request to register non typed-memory";
            break;
        case ErrorCode::kSharedMemoryObjectUnregisterFailedFatal:
            error_message = "Failed to unregister a shared-memory object";
            break;
        case ErrorCode::kSharedMemoryObjectHandleCreationFailedFatal:
            error_message = "Failed to create a shared-memory object handle";
            break;
        case ErrorCode::kSharedMemoryObjectHandleDeletionFailedFatal:
            error_message = "Failed to delete a shared-memory object handle";
            break;
        case ErrorCode::kSharedMemoryObjectFlagsRetrievalFailedFatal:
            error_message = "Failed to retrieve the shared memory object's flags";
            break;
        case ErrorCode::kWrongMessageIdRecoverable:
            error_message = "Wrong message id";
            break;
        case ErrorCode::kWrongClientIdRecoverable:
            error_message = "Wrong client id";
            break;
        case ErrorCode::kClientNameAlreadyUsedRecoverable:
            error_message = "Client name is already used by different process";
            break;
        case ErrorCode::kBadFileDescriptorFatal:
            error_message = "Bad file descriptor";
            break;
        case ErrorCode::kChannelCreationFailedFatal:
            error_message = "Channel creation failed";
            break;
        case ErrorCode::kNameAttachFailedFatal:
            error_message = "Name attach failed";
            break;
        case ErrorCode::kNameDetachFailedFatal:
            error_message = "Name detach failed";
            break;
        case ErrorCode::kDispatchDestroyFailedRecoverable:
            error_message = "Dispatch destroy failed";
            break;
        case ErrorCode::kWrongHandleRecoverable:
            error_message = "Wrong memory handle of local trace job";
            break;
        case ErrorCode::kCallbackAlreadyRegisteredRecoverable:
            error_message = "Callback already registered for the specific client id";
            break;
        case ErrorCode::kNoFreeSlotToSaveTheCallbackRecoverable:
            error_message = "No free slot found to save the callback";
            break;
        case ErrorCode::kClientNotFoundRecoverable:
            error_message = "Client ID not found";
            break;
        case ErrorCode::kInvalidShmObjectHandleFatal:
            error_message = "Invalid SHM object handle";
            break;
        case ErrorCode::kNoDeallocatorCallbackRegisteredFatal:
            error_message = "No deallocator callback registered";
            break;
        case ErrorCode::kNoMoreSpaceForNewClientFatal:
            error_message = "No More Space for new client, Max Number of Clients allocated";
            break;
        case ErrorCode::kNoMoreSpaceForNewShmObjectFatal:
            error_message = "No More Space for new SHM object, Max Number of SHM objects allocated";
            break;
        case ErrorCode::kTerminalFatal:
            error_message = "Terminal Fatal";
            break;
        case ErrorCode::kDaemonNotAvailableFatal:
            error_message = "LTPM Daemon not available";
            break;
        case ErrorCode::kFailedRegisterCachedClientsFatal:
            error_message = "Failed to register the cached client registration requests";
            break;
        case ErrorCode::kFailedRegisterCachedShmObjectsFatal:
            error_message = "Failed to register the cached SHM object registration requests";
            break;
        case ErrorCode::kFailedToProcessJobsFatal:
            error_message = "Failed to process Jobs";
            break;
        case ErrorCode::kTraceJobAllocatorInitializationFailedFatal:
            error_message = "TraceJobAllocator initialization failed";
            break;
        case ErrorCode::kDaemonIsDisconnectedRecoverable:
            error_message = "Daemon is disconnected";
            break;
        case ErrorCode::kGenericErrorRecoverable:
        case ErrorCode::kLastRecoverable:
        default:
            error_message = "Unknown generic error";
            break;
    }
    return error_message;
}

namespace
{
constexpr score::analysis::tracing::GenericTraceAPIErrorDomain kGenericTraceApiErrorDomain;
}

score::result::Error score::analysis::tracing::MakeError(const score::analysis::tracing::ErrorCode code,
                                                     const std::string_view user_message) noexcept
{
    return {static_cast<score::result::ErrorCode>(code), kGenericTraceApiErrorDomain, user_message};
}

bool score::analysis::tracing::IsErrorRecoverable(const score::analysis::tracing::ErrorCode code) noexcept
{
    bool error = false;
    // No harm to define the switch in that format (m6_4_3)
    //  coverity[autosar_cpp14_m6_4_3_violation]
    switch (code)
    {
        case ErrorCode::kNoErrorRecoverable:
        case ErrorCode::kNotEnoughMemoryRecoverable:
        case ErrorCode::kNoMetaInfoProvidedRecoverable:
        case ErrorCode::kNotEnoughMemoryInContainerRecoverable:
        case ErrorCode::kOffsetCalculationFailedRecoverable:
        case ErrorCode::kAtomicRingBufferFullRecoverable:
        case ErrorCode::kAtomicRingBufferEmptyRecoverable:
        case ErrorCode::kAtomicRingBufferMaxRetriesRecoverable:
        case ErrorCode::kModuleNotInitializedRecoverable:
        case ErrorCode::kModuleInitializedRecoverable:
        case ErrorCode::kDaemonNotConnectedRecoverable:
        case ErrorCode::kRingBufferFullRecoverable:
        case ErrorCode::kRingBufferEmptyRecoverable:
        case ErrorCode::kRingBufferNoEmptyElementRecoverable:
        case ErrorCode::kRingBufferNoReadyElementRecoverable:
        case ErrorCode::kRingBufferNotInitializedRecoverable:
        case ErrorCode::kRingBufferInitializedRecoverable:
        case ErrorCode::kRingBufferInvalidStateRecoverable:
        case ErrorCode::kRingBufferInvalidMemoryResourceRecoverable:
        case ErrorCode::kCallbackAlreadyRegisteredRecoverable:
        case ErrorCode::kNoFreeSlotToSaveTheCallbackRecoverable:
        case ErrorCode::kMessageSendFailedRecoverable:
        case ErrorCode::kWrongMessageIdRecoverable:
        case ErrorCode::kWrongClientIdRecoverable:
        case ErrorCode::kClientNameAlreadyUsedRecoverable:
        case ErrorCode::kDispatchDestroyFailedRecoverable:
        case ErrorCode::kWrongHandleRecoverable:
        case ErrorCode::kLastRecoverable:
        case ErrorCode::kGenericErrorRecoverable:
        case ErrorCode::kClientNotFoundRecoverable:
        case ErrorCode::kSharedMemoryObjectAlreadyRegisteredRecoverable:
        case ErrorCode::kNoSpaceLeftForAllocationRecoverable:
        case ErrorCode::kIndexOutOfBoundsInSharedListRecoverable:
        case ErrorCode::kMemoryCorruptionDetectedFatal:
        case ErrorCode::kDaemonIsDisconnectedRecoverable:
            error = true;
            break;
        case ErrorCode::kDaemonNotConnectedFatal:
        case ErrorCode::kInvalidArgumentFatal:
        case ErrorCode::kDaemonConnectionFailedFatal:
        case ErrorCode::kDaemonCommunicatorNotSupportedFatal:
        case ErrorCode::kServerConnectionNameOpenFailedFatal:
        case ErrorCode::kDaemonTerminationDetectionFailedFatal:
        case ErrorCode::kNoDeallocatorCallbackRegisteredFatal:
        case ErrorCode::kSharedMemoryObjectRegistrationFailedFatal:
        case ErrorCode::kSharedMemoryObjectNotInTypedMemoryFatal:
        case ErrorCode::kSharedMemoryObjectUnregisterFailedFatal:
        case ErrorCode::kSharedMemoryObjectHandleCreationFailedFatal:
        case ErrorCode::kSharedMemoryObjectFlagsRetrievalFailedFatal:
        case ErrorCode::kSharedMemoryObjectHandleDeletionFailedFatal:
        case ErrorCode::kBadFileDescriptorFatal:
        case ErrorCode::kChannelCreationFailedFatal:
        case ErrorCode::kNameAttachFailedFatal:
        case ErrorCode::kNameDetachFailedFatal:
        case ErrorCode::kTerminalFatal:
        case ErrorCode::kInvalidAppInstanceIdFatal:
        case ErrorCode::kInvalidBindingTypeFatal:
        case ErrorCode::kNoMoreSpaceForNewClientFatal:
        case ErrorCode::kNoMoreSpaceForNewShmObjectFatal:
        case ErrorCode::kInvalidShmObjectHandleFatal:
        case ErrorCode::kDaemonNotAvailableFatal:
        case ErrorCode::kFailedRegisterCachedClientsFatal:
        case ErrorCode::kFailedRegisterCachedShmObjectsFatal:
        case ErrorCode::kTraceJobAllocatorInitializationFailedFatal:
        case ErrorCode::kFailedToProcessJobsFatal:
        case ErrorCode::kRingBufferSharedMemoryCreationFatal:
        case ErrorCode::kRingBufferSharedMemoryHandleCreationFatal:
        case ErrorCode::kRingBufferSharedMemorySealFatal:
        case ErrorCode::kRingBufferSharedMemoryHandleOpenFatal:
        case ErrorCode::kRingBufferSharedMemoryFstatFatal:
        case ErrorCode::kRingBufferSharedMemoryMapFatal:
        case ErrorCode::kRingBufferSharedMemorySizeCalculationFatal:
        default:
            error = false;
            break;
    }
    return error;
}
