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
#ifndef LIB_OS_MOCKLIB_QNX_SIGEVENT_QNX_MOCK_H
#define LIB_OS_MOCKLIB_QNX_SIGEVENT_QNX_MOCK_H

#include "score/os/qnx/sigevent_qnx.h"

#include <gmock/gmock.h>

namespace score::os
{

class SigEventQnxMock : public SigEventQnx
{
    ~SigEventQnxMock() override = default;

    // Mock generic SigEvent functionality
    MOCK_METHOD(ResultBlank, SetNotificationType, (const NotificationType), (override));
    MOCK_METHOD(ResultBlank, SetSignalNumber, (const std::int32_t), (override));
    MOCK_METHOD(ResultBlank, SetSignalEventValue, ((const std::variant<int, void*>)), (override));
    MOCK_METHOD(ResultBlank, SetThreadCallback, (const SigValCallback), (override));
    MOCK_METHOD(ResultBlank, SetThreadAttributes, (pthread_attr_t&), (override));
    MOCK_METHOD((const sigevent&), GetSigevent, (), (const, override));
    MOCK_METHOD(void, ModifySigevent, (const SigeventModifier&), (override));
    MOCK_METHOD(void, Reset, (), (override));

    // Mock QNX-specific functionality
    MOCK_METHOD(void, SetUnblock, (), (override));
    MOCK_METHOD(void,
                SetPulse,
                (const std::int32_t, const std::int16_t, const std::int16_t, const std::uintptr_t),
                (override));
    MOCK_METHOD(void, SetSignalThread, (const std::int32_t, const std::int32_t, const std::int16_t), (override));
    MOCK_METHOD(void, SetSignalCode, (const std::int32_t, const std::intptr_t, const std::int16_t), (override));
    MOCK_METHOD(void, SetMemory, (volatile std::uint32_t*, std::int32_t, std::int32_t), (override));
    MOCK_METHOD(void, SetInterrupt, (), (override));
};

}  // namespace score::os

#endif  // LIB_OS_MOCKLIB_QNX_SIGEVENT_QNX_MOCK_H
