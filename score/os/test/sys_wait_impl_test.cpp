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
#include "score/os/sys_wait_impl.h"
#include "gtest/gtest.h"
#include "score/os/sys_wait.h"
#include <chrono>
#include <ctime>
#include <ratio>

constexpr uint32_t WNOTHREAD = 0x20000000;
constexpr uint32_t WALL = 0x40000000;
constexpr uint32_t WCLONE = 0x80000000;
constexpr int SLEEP_DURATION = 5;

using namespace std::chrono;

namespace score
{
namespace os
{
namespace test
{

pid_t spawnProcess()
{
    pid_t cpid = fork();
    if (cpid == -1)
    {
        abort();
    }
    if (cpid == 0)
    {
        sleep(SLEEP_DURATION);
        exit(EXIT_SUCCESS);
    }
    return cpid;
}

TEST(SysWaitImplTest, Wait)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SysWaitImplTest Wait");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    score::os::SysWaitImpl syswait;
    pid_t cpid = 0;
    int status = 0;
    steady_clock::time_point t1 = steady_clock::now();
    cpid = spawnProcess();
    score::cpp::expected<pid_t, Error> ret = syswait.wait(&status);
    steady_clock::time_point t2 = steady_clock::now();
    auto seconds = duration_cast<std::chrono::milliseconds>(t2 - t1);
    EXPECT_TRUE(seconds.count() > SLEEP_DURATION);
    EXPECT_EQ(cpid, ret.value());
    EXPECT_FALSE(WIFEXITED(status) && WEXITSTATUS(status));
}

TEST(SysWaitImplTest, WaitFail)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SysWaitImplTest Wait Fail");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    score::os::SysWait& syswait = score::os::SysWait::instance();
    int status;
    score::cpp::expected<pid_t, Error> err = syswait.wait(&status);
    EXPECT_EQ(err.error(), score::os::Error::createFromErrno(ECHILD));
}

TEST(SysWaitImplTest, Waitpid)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SysWaitImplTest Waitpid");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    score::os::SysWait& syswait = score::os::SysWait::instance();
    pid_t cpid = 0;
    int status = 0;
    steady_clock::time_point t1 = steady_clock::now();
    cpid = spawnProcess();
    score::cpp::expected<pid_t, Error> ret = syswait.waitpid(cpid, &status, WUNTRACED | WCONTINUED);
    steady_clock::time_point t2 = steady_clock::now();
    auto seconds = duration_cast<std::chrono::milliseconds>(t2 - t1);
    EXPECT_TRUE(seconds.count() > SLEEP_DURATION);
    EXPECT_EQ(cpid, ret.value());
    EXPECT_FALSE(WIFEXITED(status) && WEXITSTATUS(status));
}

TEST(SysWaitImplTest, WaitpidFail)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SysWaitImplTest Waitpid Fail");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    score::os::SysWaitImpl syswait;
    pid_t cpid = 0;
    int status = 0;
    cpid = spawnProcess();
    score::cpp::expected<pid_t, Error> retval =
        syswait.waitpid(cpid, &status, ~(WNOHANG | WUNTRACED | WCONTINUED | WNOTHREAD | WCLONE | WALL));
    EXPECT_EQ(retval.error(), score::os::Error::createFromErrno(EINVAL));
}
}  // namespace test
}  // namespace os
}  // namespace score
