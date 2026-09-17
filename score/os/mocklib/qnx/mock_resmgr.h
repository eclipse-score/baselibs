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
#ifndef SCORE_LIB_OS_MOCKLIB_QNX_MOCK_RES_MGR_MSG_WRITE_H
#define SCORE_LIB_OS_MOCKLIB_QNX_MOCK_RES_MGR_MSG_WRITE_H

#include "score/os/qnx/resmgr.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace os
{

class MockResMgr final : public ResMgr
{
  public:
    MOCK_METHOD((score::cpp::expected<size_t, score::os::Error>),
                resmgr_msgwrite,
                (resmgr_context_t * ctp, const void* msg, size_t size, size_t offset),
                (const, noexcept, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_QNX_MOCK_IOFUNC_H
