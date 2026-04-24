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
    std::string_view error_message{"Unknown generic error"};

    switch (code)
    {
        case static_cast<score::result::ErrorCode>(ErrorCode::kNoErrorRecoverable):
            error_message = "No error occured";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kDaemonNotConnectedFatal):
            error_message = "Daemon was unexpectedly disconnected";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kInvalidAppInstanceIdFatal):
            error_message = "Invalid app instance ID";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kInvalidArgumentFatal):
            error_message = "Invalid argument";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kInvalidBindingTypeFatal):
            error_message = "Invalid binding type";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kModuleInitializedRecoverable):
            error_message = "Module already initialized";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kModuleNotInitializedRecoverable):
            error_message = "Module not initialized";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kNotEnoughMemoryRecoverable):
            error_message = "Not enough memory for allocation";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kNoMetaInfoProvidedRecoverable):
            error_message = "No meta info were provided";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kNotEnoughMemoryInContainerRecoverable):
            error_message = "Not enough memory in trace job container";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kOffsetCalculationFailedRecoverable):
            error_message = "Offset calculation failed";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kAtomicRingBufferFullRecoverable):
            error_message = "Atomic ring buffer full";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kAtomicRingBufferEmptyRecoverable):
            error_message = "Atomic ring buffer empty";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kAtomicRingBufferMaxRetriesRecoverable):
            error_message = "Atomic ring buffer access failed max retry times";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kFailedToCacheEarlyTraceJobRecoverable):
            error_message = "Failed to cache early trace job during daemon disconnection";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kDaemonNotConnectedRecoverable):
            error_message = "Daemon is not yet available";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kRingBufferFullRecoverable):
            error_message = "Ring buffer full";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kRingBufferEmptyRecoverable):
            error_message = "Ring buffer empty";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kRingBufferNoEmptyElementRecoverable):
            error_message = "No empty element in the ring buffer could be acquired";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kRingBufferNoReadyElementRecoverable):
            error_message = "No ready element in the ring buffer could be acquired";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kRingBufferNotInitializedRecoverable):
            error_message = "Ring buffer not initialized";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kRingBufferInitializedRecoverable):
            error_message = "Ring buffer already initialized";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kRingBufferInvalidStateRecoverable):
            error_message = "Invalid state of ring buffer";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kRingBufferInvalidMemoryResourceRecoverable):
            error_message = "Invalid memory resource passed to constructor";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kRingBufferSharedMemoryCreationFatal):
            error_message = "Failed to create shared memory for shared memory ring buffer";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kRingBufferSharedMemoryHandleCreationFatal):
            error_message = "Failed to create shared memory handle for shared memory ring buffer";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kRingBufferSharedMemorySealFatal):
            error_message = "Failed to seal shared memory of shared memory ring buffer";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kRingBufferSharedMemoryHandleOpenFatal):
            error_message = "Failed to open shared memory handle of shared memory ring buffer";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kRingBufferSharedMemoryFstatFatal):
            error_message = "Failed to perform fstat on shared memory ring buffer file descriptor";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kRingBufferSharedMemoryMapFatal):
            error_message = "Failed to to map memory region of shared memory ring buffer";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kRingBufferSharedMemorySizeCalculationFatal):
            error_message =
                "Calculated shared memory size doesn't match the size of the original created shared memory";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kNoSpaceLeftForAllocationRecoverable):
            error_message = "No space to allocate in TMD shared memory";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kIndexOutOfBoundsInSharedListRecoverable):
            error_message = "Index is out of bounds";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kMemoryCorruptionDetectedFatal):
            error_message = "Memory Corruption detected";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kDaemonConnectionFailedFatal):
            error_message = "Daemon connection failed";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kDaemonCommunicatorNotSupportedFatal):
            error_message = "Daemon communication is supported only with QNX";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kMessageSendFailedRecoverable):
            error_message = "Failed to send the message";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kSharedMemoryObjectRegistrationFailedFatal):
            error_message = "Failed to register a shared-memory object";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kSharedMemoryObjectAlreadyRegisteredRecoverable):
            error_message = "Shared-memory object is already registered with this file descriptor/path";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kSharedMemoryObjectNotInTypedMemoryFatal):
            error_message = "Request to register non typed-memory";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kSharedMemoryObjectUnregisterFailedFatal):
            error_message = "Failed to unregister a shared-memory object";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kSharedMemoryObjectHandleCreationFailedFatal):
            error_message = "Failed to create a shared-memory object handle";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kSharedMemoryObjectHandleDeletionFailedFatal):
            error_message = "Failed to delete a shared-memory object handle";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kSharedMemoryObjectFlagsRetrievalFailedFatal):
            error_message = "Failed to retrieve the shared memory object's flags";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kWrongMessageIdRecoverable):
            error_message = "Wrong message id";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kBadFileDescriptorFatal):
            error_message = "Bad file descriptor";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kWrongHandleRecoverable):
            error_message = "Wrong memory handle of local trace job";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kCallbackAlreadyRegisteredRecoverable):
            error_message = "Callback already registered for the specific client id";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kNoFreeSlotToSaveTheCallbackRecoverable):
            error_message = "No free slot found to save the callback";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kClientNotFoundRecoverable):
            error_message = "Client ID not found";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kInvalidShmObjectHandleFatal):
            error_message = "Invalid SHM object handle";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kNoDeallocatorCallbackRegisteredFatal):
            error_message = "No deallocator callback registered";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kNoMoreSpaceForNewClientFatal):
            error_message = "No More Space for new client, Max Number of Clients allocated";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kNoMoreSpaceForNewShmObjectFatal):
            error_message = "No More Space for new SHM object, Max Number of SHM objects allocated";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kTerminalFatal):
            error_message = "Terminal Fatal";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kDaemonNotAvailableFatal):
            error_message = "LTPM Daemon not available";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kFailedRegisterCachedClientsFatal):
            error_message = "Failed to register the cached client registration requests";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kFailedRegisterCachedShmObjectsFatal):
            error_message = "Failed to register the cached SHM object registration requests";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kFailedToProcessJobsFatal):
            error_message = "Failed to process Jobs";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kTraceJobAllocatorInitializationFailedFatal):
            error_message = "TraceJobAllocator initialization failed";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kDaemonIsDisconnectedRecoverable):
            error_message = "Daemon is disconnected";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kGenericErrorRecoverable):
            error_message = "Unknown generic error";
            break;
        case static_cast<score::result::ErrorCode>(ErrorCode::kLastRecoverable):
            error_message = "Unknown generic error";
            break;
        default:
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
    bool is_recoverable{false};

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
        case ErrorCode::kFailedToCacheEarlyTraceJobRecoverable:
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
        case ErrorCode::kWrongHandleRecoverable:
        case ErrorCode::kLastRecoverable:
        case ErrorCode::kGenericErrorRecoverable:
        case ErrorCode::kClientNotFoundRecoverable:
        case ErrorCode::kSharedMemoryObjectAlreadyRegisteredRecoverable:
        case ErrorCode::kNoSpaceLeftForAllocationRecoverable:
        case ErrorCode::kIndexOutOfBoundsInSharedListRecoverable:
        case ErrorCode::kMemoryCorruptionDetectedFatal:
        case ErrorCode::kDaemonIsDisconnectedRecoverable:
            is_recoverable = true;
            break;
        case ErrorCode::kDaemonNotConnectedFatal:
        case ErrorCode::kInvalidArgumentFatal:
        case ErrorCode::kDaemonConnectionFailedFatal:
        case ErrorCode::kDaemonCommunicatorNotSupportedFatal:
        case ErrorCode::kNoDeallocatorCallbackRegisteredFatal:
        case ErrorCode::kSharedMemoryObjectRegistrationFailedFatal:
        case ErrorCode::kSharedMemoryObjectNotInTypedMemoryFatal:
        case ErrorCode::kSharedMemoryObjectUnregisterFailedFatal:
        case ErrorCode::kSharedMemoryObjectHandleCreationFailedFatal:
        case ErrorCode::kSharedMemoryObjectFlagsRetrievalFailedFatal:
        case ErrorCode::kSharedMemoryObjectHandleDeletionFailedFatal:
        case ErrorCode::kBadFileDescriptorFatal:
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
            is_recoverable = false;
            break;
        default:
            break;
    }

    return is_recoverable;
}
