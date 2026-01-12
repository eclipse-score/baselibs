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
#ifndef SCORE_LIB_OS_SIGEVENT_H
#define SCORE_LIB_OS_SIGEVENT_H

#include "score/os/ObjectSeam.h"
#include "score/result/result.h"

#include <csignal>
#include <cstdint>
#include <functional>
#include <variant>

namespace score::os
{

using SigValCallback = void (*)(sigval);
using SigeventModifier = std::function<void(sigevent&)>;

class SigEvent : public ObjectSeam<SigEvent>
{
  public:
    enum class NotificationType
    {
        kNone,
        kSignal,
        kThread,
    };

    SigEvent() = default;
    virtual ~SigEvent() = default;
    SigEvent(const SigEvent&) = delete;
    SigEvent& operator=(const SigEvent&) = delete;
    SigEvent(SigEvent&&) = delete;
    SigEvent& operator=(SigEvent&&) = delete;

    virtual ResultBlank SetNotificationType(const NotificationType notification_type) = 0;
    virtual ResultBlank SetSignalNumber(const std::int32_t signal_number) = 0;
    virtual ResultBlank SetSignalEventValue(const std::variant<int32_t, void*> signal_event_value) = 0;
    virtual ResultBlank SetThreadCallback(const SigValCallback callback) = 0;
    virtual ResultBlank SetThreadAttributes(pthread_attr_t& attr) = 0;
    virtual const sigevent& GetSigevent() const = 0;
    virtual void ModifySigevent(const SigeventModifier& modifier) = 0;
    virtual void Reset() = 0;
};

}  // namespace score::os

#endif
