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

#include "score/os/mocklib/qnx/mock_secpolev.h"

#include <gtest/gtest.h>

namespace score
{
namespace os
{
namespace qnx
{

// Verify mock is not abstract and can be instantiated.
TEST(MockSecpolEvTest, CanBeInstantiated)
{
    MockSecpolEv mock;
    SUCCEED();
}

}  // namespace qnx
}  // namespace os
}  // namespace score
