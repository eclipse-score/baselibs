/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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

#include "score/result/error_msg_mapping.h"
#include "score/result/dummy_error_code.h"

#include <gtest/gtest.h>

namespace score::result
{

TEST(RustBridgeTests, GetMessageForErrorCodeFFI)
{
    std::string_view error_message;
    LibResultErrorDomainGetMessageForErrorCode(
        dummy_error_domain, static_cast<ErrorCode>(DummyErrorCode::kFirstError), error_message);
    ASSERT_EQ("First Error!", error_message);
}

}  // namespace score::result
