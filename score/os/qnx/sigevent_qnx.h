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
#ifndef SCORE_LIB_OS_QNX_SIGEVENT_QNX_H
#define SCORE_LIB_OS_QNX_SIGEVENT_QNX_H

#include "score/os/sigevent.h"

namespace score::os
{

class SigEventQnx : public SigEvent
{
  public:
    SigEventQnx() = default;
    virtual ~SigEventQnx() = default;
    SigEventQnx(const SigEventQnx&) = delete;
    SigEventQnx& operator=(const SigEventQnx&) = delete;
    SigEventQnx(SigEventQnx&&) = delete;
    SigEventQnx& operator=(SigEventQnx&&) = delete;
    // extend the SigEvent abstract class
    ResultBlank SetNotificationType(const NotificationType notification_type) override = 0;
    ResultBlank SetSignalNumber(const std::int32_t signal_number) override = 0;
    ResultBlank SetSignalEventValue(const std::variant<int, void*> signal_event_value) override = 0;
    ResultBlank SetThreadCallback(const SigValCallback callback) override = 0;
    ResultBlank SetThreadAttributes(pthread_attr_t& attr) override = 0;
    const sigevent& GetSigevent() const override = 0;
    void ModifySigevent(const SigeventModifier& modifier) override = 0;
    void Reset() override = 0;

    // add QNX-specific functionality
    virtual void SetUnblock() = 0;
    virtual void SetPulse(const std::int32_t connection_id,
                          const std::int16_t priority,
                          const std::int16_t code,
                          const std::uintptr_t value) = 0;
    virtual void SetSignalThread(const std::int32_t signal_number,
                                 const std::int32_t value,
                                 const std::int16_t tid) = 0;
    virtual void SetSignalCode(const std::int32_t signal_number,
                               const std::intptr_t value,
                               const std::int16_t code) = 0;
    // Suppress "AUTOSAR C++14 A2-11-1", The rule states: "Volatile keyword shall not be used."
    // It is required for QNX API compliance as sigevent struct expects volatile unsigned* for memory monitoring
    // coverity[autosar_cpp14_a2_11_1_violation]
    virtual void SetMemory(volatile std::uint32_t* addr, std::int32_t size, std::int32_t offset) = 0;
    virtual void SetInterrupt() = 0;
};

}  // namespace score::os

#endif  // SCORE_LIB_OS_QNX_SIGEVENT_QNX_H
