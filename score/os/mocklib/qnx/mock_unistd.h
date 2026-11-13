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
#ifndef SCORE_LIB_OS_MOCKLIB_QNX_MOCK_UNISTD_H
#define SCORE_LIB_OS_MOCKLIB_QNX_MOCK_UNISTD_H

#include "score/os/qnx/unistd.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace os
{
namespace qnx
{
class MockUnistd : public QnxUnistd
{
  public:
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                setgroupspid,
                (score::cpp::span<const GroupId> grouplist, pid_t pid),
                (const, noexcept, override));
};
}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_QNX_MOCK_UNISTD_H
