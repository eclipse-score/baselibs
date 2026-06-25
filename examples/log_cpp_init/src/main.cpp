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

#include "score/mw/log/rust/stdout_logger_init.h"

extern "C" {
void show_logs();
}

int main()
{
    using namespace score::mw::log::rust;

    StdoutLoggerBuilder builder;
    builder.Context("ABCD").ShowModule(true).ShowFile(true).ShowLine(true).SetAsDefaultLogger();

    show_logs();

    return 0;
}
