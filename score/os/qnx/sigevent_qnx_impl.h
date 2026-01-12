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
#ifndef SCORE_LIB_OS_SIGEVENT_QNX_IMPL_H
#define SCORE_LIB_OS_SIGEVENT_QNX_IMPL_H

#include "score/os/qnx/sigevent_qnx.h"

namespace score::os
{

class SigEventQnxImpl final : public SigEventQnx
{
  public:
    explicit SigEventQnxImpl(std::unique_ptr<SigEvent> signal_event = nullptr);
    ~SigEventQnxImpl() override = default;
    SigEventQnxImpl(const SigEventQnxImpl&) = delete;
    SigEventQnxImpl& operator=(const SigEventQnxImpl&) = delete;
    SigEventQnxImpl(SigEventQnxImpl&&) = delete;
    SigEventQnxImpl& operator=(SigEventQnxImpl&&) = delete;

    // override the common SigEvent functions
    ResultBlank SetNotificationType(const NotificationType notification_type) override;
    ResultBlank SetSignalNumber(const std::int32_t signal_number) override;
    ResultBlank SetSignalEventValue(const std::variant<int32_t, void*> signal_event_value) override;
    ResultBlank SetThreadCallback(const SigValCallback callback) override;
    ResultBlank SetThreadAttributes(pthread_attr_t& attr) override;
    const sigevent& GetSigevent() const override;
    void Reset() override;

    // override the QNX-specific SigEventQnx functions
    void SetUnblock() override;
    void SetPulse(const std::int32_t connection_id,
                  const std::int16_t priority,
                  const std::int16_t code,
                  const std::uintptr_t value) override;
    void SetSignalThread(const std::int32_t signal_number, const std::int32_t value, const std::int16_t tid) override;
    void SetSignalCode(const std::int32_t signal_number, const std::intptr_t value, const std::int16_t code) override;
    // Suppress "AUTOSAR C++14 A2-11-1", The rule states: "Volatile keyword shall not be used."
    // It is required for QNX API compliance as sigevent struct expects volatile unsigned* for memory monitoring
    // coverity[autosar_cpp14_a2_11_1_violation]
    void SetMemory(volatile std::uint32_t* addr, std::int32_t size, std::int32_t offset) override;
    void SetInterrupt() override;

  private:
    void ModifySigevent(const SigeventModifier& modifier) override;

    std::unique_ptr<SigEvent> signal_event_;
};

}  // namespace score::os

#endif  // SCORE_LIB_OS_SIGEVENT_QNX_IMPL_H
