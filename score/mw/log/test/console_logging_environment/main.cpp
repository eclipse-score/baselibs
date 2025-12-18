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

#include "gtest/gtest.h"

#include "score/mw/log/test/console_logging_environment/console_logging_environment.h"

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    // Googletest will delete all registered global environments and thus we must use new() here.
    auto console_log_environment = new score::mw::log::ConsoleLoggingEnvironment();
    AddGlobalTestEnvironment(console_log_environment);

    return RUN_ALL_TESTS();
}
