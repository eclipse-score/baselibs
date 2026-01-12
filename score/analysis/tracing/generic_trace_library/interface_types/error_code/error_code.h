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
#ifndef SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_ERROR_CODE_ERROR_CODE_H
#define SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_ERROR_CODE_ERROR_CODE_H

#include "score/result/error.h"
#include <score/string_view.hpp>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief Error code that method can return
enum class ErrorCode : score::result::ErrorCode
{
    kNoErrorRecoverable = 0x0U,              ///< No error occured (Recoverable)
    kDaemonNotConnectedFatal,                ///< Daemon was unexpectedly disconnected (Fatal)
    kInvalidAppInstanceIdFatal,              ///< Invalid app instance ID (Fatal)
    kInvalidArgumentFatal,                   ///< Invalid argument (Fatal)
    kInvalidBindingTypeFatal,                ///< Invalid binding type (Fatal)
    kModuleInitializedRecoverable,           ///< Module already initialized (Recoverable)
    kModuleNotInitializedRecoverable,        ///< Module not initialized (Recoverable)
    kNotEnoughMemoryRecoverable,             ///< Not enough memory for allocation (Recoverable)
    kNoMetaInfoProvidedRecoverable,          ///< No meta info were provided (Recoverable)
    kNotEnoughMemoryInContainerRecoverable,  ///< Not enough memory in trace job container (Recoverable)
    kOffsetCalculationFailedRecoverable,     ///< Offset calculation failed (Recoverable)

    kAtomicRingBufferFullRecoverable,        ///< Atomic ring buffer full (Recoverable)
    kAtomicRingBufferEmptyRecoverable,       ///< Atomic ring buffer empty (Recoverable)
    kAtomicRingBufferMaxRetriesRecoverable,  ///< Atomic ring buffer access failed max retry times (Recoverable)

    kDaemonNotConnectedRecoverable,        ///< Daemon is not yet available (Recoverable)
    kRingBufferFullRecoverable,            ///< Ring buffer full (Recoverable)
    kRingBufferEmptyRecoverable,           ///< Ring buffer empty (Recoverable)
    kRingBufferNoEmptyElementRecoverable,  ///< No empty element in the ring buffer could be acquired (recoverable)
    kRingBufferNoReadyElementRecoverable,  ///< No ready element in the ring buffer could be acquired (recoverable)
    kRingBufferNotInitializedRecoverable,  ///< Ring buffer not initialized (Recoverable)
    kRingBufferInitializedRecoverable,     ///< Ring buffer already initialized (Recoverable)
    kRingBufferInvalidStateRecoverable,    ///< Invalid buffer state (Recoverable)
    kRingBufferInvalidMemoryResourceRecoverable,  ///< Buffer size too small (Recoverable)
    kRingBufferSharedMemoryCreationFatal,         ///< Failed to create shm object for shared memory ring buffer
    kRingBufferSharedMemoryHandleCreationFatal,   ///< Failed to create handle for shared memory ring buffer
    kRingBufferSharedMemorySealFatal,             ///< Failed to seal shared memory for shared memory ring buffer
    kRingBufferSharedMemoryHandleOpenFatal,       ///< Failed to open handle for shared memory ring buffer
    kRingBufferSharedMemoryFstatFatal,            ///< Failed to perform fstat on shared memory ring buffer
    kRingBufferSharedMemoryMapFatal,              ///< Failed to map memory region of shared memory ring buffer
    kRingBufferSharedMemorySizeCalculationFatal,  ///< Calculated shared memory size doesn't match the size of the
                                                  ///< original created shared memory

    kDaemonConnectionFailedFatal,             ///< Daemon connection failed (Fatal)
    kDaemonCommunicatorNotSupportedFatal,     ///< Daemon connection is not supported
    kServerConnectionNameOpenFailedFatal,     ///< QNX name open failed (Fatal)
    kDaemonTerminationDetectionFailedFatal,   ///< Daemon Termination detection failed (Fatal)
    kClientNotFoundRecoverable,               ///< Client not registered (Recoverable)
    kInvalidShmObjectHandleFatal,             ///< SHM Object handle found (Recoverable)
    kCallbackAlreadyRegisteredRecoverable,    ///< Callback already registered the client id (Recoverable)
    kNoFreeSlotToSaveTheCallbackRecoverable,  ///< No free slot to save the callback (Recoverable)
    kNoDeallocatorCallbackRegisteredFatal,    ///< No deallocator callback registered (Fatal)

    kMessageSendFailedRecoverable,                    ///< Message sending failed (Recoverable)
    kSharedMemoryObjectRegistrationFailedFatal,       ///< Shared-memory object registration failed (Fatal)
    kSharedMemoryObjectAlreadyRegisteredRecoverable,  ///< Shared-memory object is already registered (Recoverable)
    kSharedMemoryObjectNotInTypedMemoryFatal,         ///< Tried to register non-typed memory. (Fatal)
    kSharedMemoryObjectUnregisterFailedFatal,         ///< Shared-memory object unregister failed (Fatal)
    kSharedMemoryObjectHandleCreationFailedFatal,     ///< Shared-memory object creation failed (Fatal)
    kSharedMemoryObjectFlagsRetrievalFailedFatal,     ///< Shared-memory object flags retrieval failed (Fatal)
    kSharedMemoryObjectHandleDeletionFailedFatal,     ///< Shared-memory object deletion failed (Fatal)
    kNoMoreSpaceForNewClientFatal,                    ///< Max Number of Clients allocated (Fatal)
    kNoMoreSpaceForNewShmObjectFatal,                 ///< Max Number of SHM objects allocated (Fatal)

    kWrongMessageIdRecoverable,         ///< Wrong message id in request (Recoverable)
    kWrongClientIdRecoverable,          ///< Wrong client id in response (Recoverable)
    kClientNameAlreadyUsedRecoverable,  // Client name is already used by different process

    kBadFileDescriptorFatal,
    kChannelCreationFailedFatal,        ///< QNX channel creation failed (Fatal)
    kNameAttachFailedFatal,             ///< QNX name_attach failed (Fatal)
    kNameDetachFailedFatal,             ///< QNX name_detach failed (Fatal)
    kDispatchDestroyFailedRecoverable,  ///< QNX dispatch_destroy failed (Recoverable)

    kWrongHandleRecoverable,  ///< Wrong handle of local trace job (Recoverable)

    kNoSpaceLeftForAllocationRecoverable,      ///< No space to allocate in TMD shared memory
    kIndexOutOfBoundsInSharedListRecoverable,  ///< Element os not found to retrieve or deallocate
    kMemoryCorruptionDetectedFatal,            ///< Memory corruption detected via canary values (Fatal)

    kDaemonNotAvailableFatal,              ///< LTPM Daemon not available (Fatal)
    kFailedRegisterCachedClientsFatal,     ///< Failed to register the cached client registration requests (Fatal)
    kFailedRegisterCachedShmObjectsFatal,  ///< Failed to register the cached SHM object registration requests (Fatal)
    kTraceJobAllocatorInitializationFailedFatal,  ///< TraceJobAllocator initialization failed (Fatal)
    kDaemonIsDisconnectedRecoverable,             ///< Daemon is disconnected after successful connection (Recoverable)
    kFailedToProcessJobsFatal,                    ///< Failed to process Jobs (Fatal)
    kGenericErrorRecoverable,                     ///< Generic error (Recoverable)
    kTerminalFatal,                               ///< Terminal (Fatal)
    kLastRecoverable  ///< to be used only for overflow check (Recoverable), no item below!!!
};

class GenericTraceAPIErrorDomain final : public score::result::ErrorDomain
{
  public:
    std::string_view MessageFor(const score::result::ErrorCode& code) const noexcept override;
};

score::result::Error MakeError(const score::analysis::tracing::ErrorCode code,
                             const std::string_view user_message = "") noexcept;

/// @brief helper function to check if error is recoverable
/// @param[in] code  error code to check
///
/// @return True if error is recoverable, otherwise false
bool IsErrorRecoverable(const score::analysis::tracing::ErrorCode code) noexcept;
}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_GENERIC_TRACE_LIBRARY_INTERFACE_TYPES_ERROR_CODE_ERROR_CODE_H
