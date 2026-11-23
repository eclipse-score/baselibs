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
#include "score/os/linux/unistd.h"
#include "score/quality/compiler_warnings/warnings.h"

#include "gtest/gtest.h"

namespace
{

TEST(Daemon, RunDaemonInChildProcess_NoError)
{
    const auto pid = ::fork();
    if (pid == -1)
    {
        GTEST_FAIL() << "Error when forking process. Could not run test.";
    }

    if (pid == 0)  // child process
    {
        // call daemon() in child process created by fork(),
        // then child process succeeds with exit(0) once the daemon call was successful
        constexpr int nochdir = 0;  // changes current dir to "/"
        constexpr int noclose = 1;  // not redirects output to /dev/null
        const auto res = score::os::daemon(nochdir, noclose);
        if (res == -1)
        {
            ::exit(-1);  // here still be running child process, notify main (test) about failure
        }
        // at this point no running child process, that was created by fork()
        // running daemonized process ...
        exit(0);
    }
    // main process..

    int status{};
    ::waitpid(pid, &status, 0);
    ASSERT_TRUE(WIFEXITED(status));
    EXPECT_EQ(WEXITSTATUS(status), 0);
}

TEST(Pipe2, PassInvalidFlagGetFailure)
{
    constexpr int invalid_flag{-1};
    int32_t fds[2] = {};

    // The deprecated function is called on purpose, to test it until it is removed.
    // clang-format off
    DISABLE_WARNING_PUSH
    DISABLE_WARNING_DEPRECATED
    const int val = score::os::pipe2(fds, invalid_flag);
    DISABLE_WARNING_POP
    // clang-format on

    ASSERT_EQ(val, -1);
}

}  // namespace
