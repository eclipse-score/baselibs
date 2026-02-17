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
#ifndef SCORE_LIB_OS_MOCKLIB_QNX_MOCK_SECPOLEV_H
#define SCORE_LIB_OS_MOCKLIB_QNX_MOCK_SECPOLEV_H

#include "score/os/qnx/secpolev.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace os
{
namespace qnx
{

class MockSecpolEv : public SecpolEv
{
  public:
    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                init,
                (const unsigned flags, const unsigned buffer_count, const unsigned flush_interval),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                shutdown,
                (secpolev_shutdown_type_t type),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<secpolev_event_type_t, score::os::Error>),
                wait_event,
                (unsigned* flags),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>), flush, (), (const, noexcept, override));
    MOCK_METHOD(uint64_t, get_dropped_event_count, (), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<pid_t, score::os::Error>), get_pid, (), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<const char*, score::os::Error>), get_process_name, (), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<const char*, score::os::Error>), get_process_type, (), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                get_ability_info,
                (unsigned* id, const char** name, unsigned* flags, uint64_t* start, uint64_t* end),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                get_path_info,
                (const char** path, secpolev_path_event_status_t* status),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                get_custom_perm_info,
                (const char** class_name, const char** perm_name, const char** obj_type),
                (const, noexcept, override));
};

}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_QNX_MOCK_SECPOLEV_H
