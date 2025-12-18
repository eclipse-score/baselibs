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
#ifndef SCORE_MW_LOG_CONSOLE_LOGGING_ENVIRONMENT_H
#define SCORE_MW_LOG_CONSOLE_LOGGING_ENVIRONMENT_H

#include "gtest/gtest.h"

#include "score/mw/log/recorder.h"

namespace score
{
namespace mw
{
namespace log
{

class ConsoleLoggingEnvironment : public ::testing::Environment
{
  public:
    void SetUp() override;

    void TearDown() override;

  private:
    std::unique_ptr<Recorder> recorder_{nullptr};
};

}  // namespace log
}  // namespace mw
}  // namespace score

#endif
