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
#ifndef SCORE_LIB_OS_SIGEVENT_IMPL_H
#define SCORE_LIB_OS_SIGEVENT_IMPL_H

#include "score/os/sigevent.h"

#include <csignal>

namespace score::os
{

class SigEventImpl final : public SigEvent
{
  public:
    SigEventImpl() = default;
    ~SigEventImpl() override;
    SigEventImpl(const SigEventImpl&) = delete;
    SigEventImpl& operator=(const SigEventImpl&) = delete;
    SigEventImpl(SigEventImpl&&) = delete;
    SigEventImpl& operator=(SigEventImpl&&) = delete;

    ResultBlank SetNotificationType(const SigEvent::NotificationType notification_type) override;
    ResultBlank SetSignalNumber(const std::int32_t signal_number) override;
    ResultBlank SetSignalEventValue(const std::variant<int32_t, void*> signal_event_value) override;
    ResultBlank SetThreadCallback(const SigValCallback callback) override;
    ResultBlank SetThreadAttributes(pthread_attr_t& attr) override;
    const sigevent& GetSigevent() const override;
    void Reset() override;

  protected:
    void ModifySigevent(const SigeventModifier& modifier) override;

  private:
    sigevent raw_sigevent_{};
};

}  // namespace score::os

#endif
