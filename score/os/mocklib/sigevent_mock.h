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
#ifndef SCORE_LIB_OS_MOCKLIB_SIGEVENT_MOCK_H
#define SCORE_LIB_OS_MOCKLIB_SIGEVENT_MOCK_H

#include "score/os/sigevent.h"

#include <gmock/gmock.h>

namespace score::os
{

class SigEventMock : public SigEvent
{
    ~SigEventMock() override = default;
    MOCK_METHOD(ResultBlank, SetNotificationType, (const NotificationType));
    MOCK_METHOD(ResultBlank, SetSignalNumber, (const std::int32_t));
    MOCK_METHOD(ResultBlank, SetSignalEventValue, ((const std::variant<int, void*>)), (override));
    MOCK_METHOD(ResultBlank, SetThreadCallback, (const SigValCallback), (override));
    MOCK_METHOD(ResultBlank, SetThreadAttributes, (pthread_attr_t&), (override));
    MOCK_METHOD((const sigevent&), GetSigevent, (), (const, override));
    MOCK_METHOD(void, ModifySigevent, (const SigeventModifier&), (override));
    MOCK_METHOD(void, Reset, (), (override));
};

}  // namespace score::os

#endif  // SCORE_LIB_OS_MOCKLIB_SIGEVENT_MOCK_H
