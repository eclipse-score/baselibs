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
#include "score/os/qnx/sigevent_qnx_impl.h"
#include "score/os/sigevent_error.h"
#include "score/os/sigevent_impl.h"
#include "score/mw/log/logging.h"

#include <sys/siginfo.h>

namespace score::os
{

SigEventQnxImpl::SigEventQnxImpl(std::unique_ptr<SigEvent> signal_event)
    : SigEventQnx{}, signal_event_{signal_event != nullptr ? std::move(signal_event) : std::make_unique<SigEventImpl>()}
{
}

ResultBlank SigEventQnxImpl::SetNotificationType(const NotificationType notification_type)
{
    return signal_event_->SetNotificationType(notification_type);
}

ResultBlank SigEventQnxImpl::SetSignalNumber(const std::int32_t signal_number)
{
    return signal_event_->SetSignalNumber(signal_number);
}

ResultBlank SigEventQnxImpl::SetSignalEventValue(const std::variant<std::int32_t, void*> signal_event_value)
{
    const auto& raw_sigevent = signal_event_->GetSigevent();
    // Suppress "AUTOSAR C++14 M5-0-21" rule findings. This rule declares: "Bitwise operators shall only be
    // applied to operands of unsigned underlying type."
    // SIGEV_UNBLOCK is part of the QNX standard and can not be changed.
    // coverity[autosar_cpp14_m5_0_21_violation]
    if (raw_sigevent.sigev_notify == SIGEV_UNBLOCK)
    {
        mw::log::LogError() << __func__ << "Invalid signal event notification type";
        return MakeUnexpected(SigEventErrorCode::kInvalidSignalEventNotificationType);
    }
    return signal_event_->SetSignalEventValue(signal_event_value);
}

ResultBlank SigEventQnxImpl::SetThreadCallback(const SigValCallback callback)
{
    return signal_event_->SetThreadCallback(callback);
}

ResultBlank SigEventQnxImpl::SetThreadAttributes(pthread_attr_t& attr)
{
    return signal_event_->SetThreadAttributes(attr);
}

const sigevent& SigEventQnxImpl::GetSigevent() const
{
    return signal_event_->GetSigevent();
}

void SigEventQnxImpl::ModifySigevent(const SigeventModifier& modifier)
{
    signal_event_->ModifySigevent(modifier);
}

void SigEventQnxImpl::Reset()
{
    signal_event_->Reset();
}

void SigEventQnxImpl::SetUnblock()
{
    mw::log::LogDebug() << __func__ << "called";
    signal_event_->ModifySigevent([](sigevent& raw_signal_event) {
        // Suppress "AUTOSAR C++14 M5-0-21" rule findings. This rule declares: "Bitwise operators shall only be
        // applied to operands of unsigned underlying type."
        // These macros are part of the QNX standard and can not be changed.
        // coverity[autosar_cpp14_m5_0_21_violation]
        SIGEV_UNBLOCK_INIT(&raw_signal_event);
    });
}

void SigEventQnxImpl::SetPulse(const std::int32_t connection_id,
                               const std::int16_t priority,
                               const std::int16_t code,
                               const std::uintptr_t value)
{
    mw::log::LogDebug() << __func__ << "called";
    signal_event_->ModifySigevent([connection_id, priority, code, value](sigevent& raw_signal_event) {
        // Suppress "autosar_cpp14_m5_2_9_violation" rule finding. This rule states: "A cast shall not convert a pointer
        // type to an integral type."
        // These macros are part of the QNX standard and can not be changed.
        // Suppress "AUTOSAR C++14 A5-2-2": "Traditional C-style casts shall not be used."
        // same justification as above.
        // Suppress "AUTOSAR C++14 M5-0-21" rule findings. This rule declares: "Bitwise operators shall only be
        // applied to operands of unsigned underlying type."
        // same justification as above.
        // Suppress "AUTOSAR C++14 M6-2-1" rule finding: Assignment operators shall not be used in sub-expressions.
        // same justification as above.
        // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall
        // not lead to data loss.".
        // same justification as above.
        // Suppress "AUTOSAR C++14 M5-0-6" rule finding.
        // An implicit integral or floating-point conversion shall not reduce the size of the underlying type.
        // same justification as above.
        // Suppress "AUTOSAR C++14 M5-0-3" rule findings. This rule states: "A cvalue expression shall
        // not be implicitly converted to a different underlying type"
        // same justification as above.
        // coverity[autosar_cpp14_m5_2_9_violation]
        // coverity[autosar_cpp14_a5_2_2_violation]
        // coverity[autosar_cpp14_m5_0_21_violation]
        // coverity[autosar_cpp14_m6_2_1_violation]
        // coverity[autosar_cpp14_a4_7_1_violation]
        // coverity[autosar_cpp14_m5_0_6_violation]
        // coverity[autosar_cpp14_m5_0_3_violation]
        SIGEV_PULSE_INIT(&raw_signal_event, connection_id, priority, code, value);
    });
}

void SigEventQnxImpl::SetSignalThread(const std::int32_t signal_number,
                                      const std::int32_t value,
                                      const std::int16_t tid)
{
    mw::log::LogDebug() << __func__ << " called";
    signal_event_->ModifySigevent([signal_number, value, tid](sigevent& raw_signal_event) {
        // Suppress "autosar_cpp14_m5_2_9_violation" rule finding. This rule states: "A cast shall not convert a pointer
        // type to an integral type."
        // These macros are part of the QNX standard and and can not be changed.
        // Suppress "AUTOSAR C++14 A5-2-2": "Traditional C-style casts shall not be used."
        // same justification as above.
        // Suppress "AUTOSAR C++14 M5-0-21" rule findings. This rule declares: "Bitwise operators shall only be
        // applied to operands of unsigned underlying type."
        // same justification as above.
        // Suppress "AUTOSAR C++14 M6-2-1" rule finding:Assignment operators shall not be used in sub-expressions.
        // same justification as above.
        // coverity[autosar_cpp14_m5_2_9_violation]
        // coverity[autosar_cpp14_a5_2_2_violation]
        // coverity[autosar_cpp14_m5_0_21_violation]
        // coverity[autosar_cpp14_m6_2_1_violation]
        SIGEV_SIGNAL_THREAD_INIT(&raw_signal_event, signal_number, value, tid);
    });
}

void SigEventQnxImpl::SetSignalCode(const std::int32_t signal_number,
                                    const std::intptr_t value,
                                    const std::int16_t code)
{
    mw::log::LogDebug() << __func__ << " called";
    signal_event_->ModifySigevent([signal_number, value, code](sigevent& raw_signal_event) {
        // Suppress "autosar_cpp14_m5_2_9_violation" rule finding. This rule states: "A cast shall not convert a pointer
        // type to an integral type."
        // These macros are part of the QNX standard and and can not be changed.
        // Suppress "AUTOSAR C++14 A5-2-2": "Traditional C-style casts shall not be used."
        // same justification as above.
        // Suppress "AUTOSAR C++14 M5-0-21" rule findings. This rule declares: "Bitwise operators shall only be
        // applied to operands of unsigned underlying type."
        // same justification as above.
        // Suppress "AUTOSAR C++14 M6-2-1" rule finding:Assignment operators shall not be used in sub-expressions.
        // same justification as above.
        // coverity[autosar_cpp14_m5_2_9_violation]
        // coverity[autosar_cpp14_a5_2_2_violation]
        // coverity[autosar_cpp14_m5_0_21_violation]
        // coverity[autosar_cpp14_m6_2_1_violation]
        SIGEV_SIGNAL_CODE_INIT(&raw_signal_event, signal_number, value, code);
    });
}

// Suppress "AUTOSAR C++14 A2-11-1", The rule states: "Volatile keyword shall not be used."
// It is required for QNX API compliance as sigevent struct expects volatile unsigned* for memory monitoring
// coverity[autosar_cpp14_a2_11_1_violation]
void SigEventQnxImpl::SetMemory(volatile std::uint32_t* addr, std::int32_t size, std::int32_t offset)
{
    mw::log::LogDebug() << __func__ << " called";
    signal_event_->ModifySigevent([addr, size, offset](sigevent& raw_signal_event) {
        // Suppress "AUTOSAR C++14 A5-2-2": "Traditional C-style casts shall not be used."
        // same justification as above.
        // Suppress "AUTOSAR C++14 M5-0-21" rule findings. This rule declares: "Bitwise operators shall only be
        // applied to operands of unsigned underlying type."
        // same justification as above.
        // Suppress "AUTOSAR C++14 M6-2-1" rule finding:Assignment operators shall not be used in sub-expressions.
        // same justification as above.
        // coverity[autosar_cpp14_a5_2_2_violation]
        // coverity[autosar_cpp14_m5_0_21_violation]
        // coverity[autosar_cpp14_m6_2_1_violation]
        SIGEV_MEMORY_INIT(&raw_signal_event, addr, size, offset);
    });
}

void SigEventQnxImpl::SetInterrupt()
{
    mw::log::LogDebug() << __func__ << " called";
    signal_event_->ModifySigevent([](sigevent& raw_signal_event) {
        // Suppress "AUTOSAR C++14 M5-0-21" rule findings. This rule declares: "Bitwise operators shall only be
        // applied to operands of unsigned underlying type."
        // same justification as above.
        // coverity[autosar_cpp14_m5_0_21_violation]
        SIGEV_INTR_INIT(&raw_signal_event);
    });
}

}  // namespace score::os
