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
#include "score/os/sigevent_impl.h"
#include "score/os/sigevent_error.h"
#include "score/mw/log/logging.h"

#include <cstring>

namespace score::os
{

SigEventImpl::~SigEventImpl()
{
    mw::log::LogDebug() << __func__ << "called";
    raw_sigevent_ = sigevent{};
}

ResultBlank SigEventImpl::SetNotificationType(const SigEvent::NotificationType notification_type)
{
    mw::log::LogDebug() << __func__ << "called";
    ResultBlank result{};
    switch (notification_type)
    {
        case SigEvent::NotificationType::kNone:
        {
            raw_sigevent_.sigev_notify = SIGEV_NONE;
            break;
        }

        case SigEvent::NotificationType::kSignal:
        {
            // Suppress "AUTOSAR C++14 M5-0-21" rule findings. This rule declares: "Bitwise operators shall only be
            // applied to operands of unsigned underlying type."
            // SIGEV_THREAD is part of the QNX standard and can not be changed.
            // coverity[autosar_cpp14_m5_0_21_violation]
            raw_sigevent_.sigev_notify = SIGEV_SIGNAL;
            break;
        }

        case SigEvent::NotificationType::kThread:
        {
            // Suppress "AUTOSAR C++14 M5-0-21" rule findings. This rule declares: "Bitwise operators shall only be
            // applied to operands of unsigned underlying type."
            // SIGEV_THREAD is part of the QNX standard and can not be changed.
            // coverity[autosar_cpp14_m5_0_21_violation]
            raw_sigevent_.sigev_notify = SIGEV_THREAD;
            break;
        }

        default:
        {
            mw::log::LogError() << __func__ << "Unexpected notification type";
            result = MakeUnexpected(SigEventErrorCode::kInvalidNotificationType);
            break;
        }
    }
    // Suppress "AUTOSAR C++14 A8-5-0" rule findings. This rule declares: "All memory shall be initialized
    // before it is read."
    // False positive, result is initialized at the beginning of the method.
    // coverity[autosar_cpp14_a8_5_0_violation: FALSE]
    return result;
}

ResultBlank SigEventImpl::SetSignalNumber(const std::int32_t signal_number)
{
    mw::log::LogDebug() << __func__ << "called";
    if ((signal_number <= 0) || (signal_number >= NSIG))
    {
        mw::log::LogError() << __func__ << "Unexpected signal number";
        return MakeUnexpected(SigEventErrorCode::kInvalidSignalNumber);
    }
    raw_sigevent_.sigev_signo = signal_number;
    return {};
}

ResultBlank SigEventImpl::SetSignalEventValue(const std::variant<int32_t, void*> signal_event_value)
{
    mw::log::LogDebug() << __func__ << "called";
    if (raw_sigevent_.sigev_notify == SIGEV_NONE)
    {
        mw::log::LogError() << __func__ << "Invalid signal event notification type";
        return MakeUnexpected(SigEventErrorCode::kInvalidSignalEventNotificationType);
    }

    if (std::holds_alternative<int>(signal_event_value))
    {
        // Use of C API, from which the union comes. Union access is unavoidable.
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access) POSIX Standard union
        raw_sigevent_.sigev_value.sival_int = std::get<int>(signal_event_value);
    }
    else
    {
        auto ptr = std::get<void*>(signal_event_value);
        if (ptr == nullptr)
        {
            return MakeUnexpected(SigEventErrorCode::kInvalidSignalEventValue);
        }
        // Use of C API, from which the union comes. Union access is unavoidable.
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access) POSIX Standard union
        raw_sigevent_.sigev_value.sival_ptr = ptr;
    }
    return {};
}

ResultBlank SigEventImpl::SetThreadCallback(const SigValCallback callback)
{
    mw::log::LogDebug() << __func__ << "called";
    // Suppress "AUTOSAR C++14 M5-0-21" rule findings. This rule declares: "Bitwise operators shall only be
    // applied to operands of unsigned underlying type."
    // SIGEV_THREAD is part of the QNX standard and can not be changed.
    // coverity[autosar_cpp14_m5_0_21_violation]
    if (raw_sigevent_.sigev_notify != SIGEV_THREAD)
    {
        // notification type MUST be SIGEV_THREAD
        mw::log::LogError() << __func__ << "Signal event notification is not SIGEV_THREAD";
        return MakeUnexpected(SigEventErrorCode::kInvalidThreadCallbackNotificationType);
    }

    if (callback == nullptr)
    {
        return MakeUnexpected(SigEventErrorCode::kInvalidArgument);
    }

    raw_sigevent_.sigev_notify_function = callback;
    return {};
}

ResultBlank SigEventImpl::SetThreadAttributes(pthread_attr_t& attr)
{
    mw::log::LogDebug() << __func__ << "called";
    // Suppress "AUTOSAR C++14 M5-0-21" rule findings. This rule declares: "Bitwise operators shall only be
    // applied to operands of unsigned underlying type."
    // SIGEV_THREAD is part of the QNX standard and can not be changed.
    // coverity[autosar_cpp14_m5_0_21_violation]
    if (raw_sigevent_.sigev_notify != SIGEV_THREAD)
    {
        // notification type MUST be SIGEV_THREAD
        mw::log::LogError() << __func__ << "Signal event notification is not SIGEV_THREAD";
        return MakeUnexpected(SigEventErrorCode::kInvalidThreadAttributesNotificationType);
    }
    raw_sigevent_.sigev_notify_attributes = &attr;
    return {};
}

const sigevent& SigEventImpl::GetSigevent() const
{
    mw::log::LogDebug() << __func__ << "called";
    return raw_sigevent_;
}

void SigEventImpl::ModifySigevent(const SigeventModifier& modifier)
{
    mw::log::LogDebug() << __func__ << "called";
    modifier(raw_sigevent_);
}

void SigEventImpl::Reset()
{
    mw::log::LogDebug() << __func__ << "called";
    raw_sigevent_ = sigevent{};
}

}  // namespace score::os
