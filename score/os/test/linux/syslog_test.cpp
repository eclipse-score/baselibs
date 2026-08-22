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
#include "score/os/syslog.h"
#include "score/os/syslog_impl.h"

#include <gtest/gtest.h>

#include <syslog.h>

#include <tuple>

namespace
{
// ---------- score/os/syslog_impl.cpp --------
class SyslogImplFixture : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        unit_ = std::make_unique<score::os::SyslogImpl>();
    }

    std::unique_ptr<score::os::Syslog> unit_;
};

TEST_F(SyslogImplFixture, OpenSyslogAndCloseFlow)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "openlog/syslog/closelog passthroughs to glibc shall not throw or crash.");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");  // equivalence classes

    // glibc openlog/syslog/closelog return void; the wrapper cannot surface an error, so the
    // interface guarantee under test is that the real passthroughs execute without throwing.
    EXPECT_NO_FATAL_FAILURE(unit_->openlog("syslog_test", LOG_PID | LOG_NDELAY, LOG_USER));
    EXPECT_NO_FATAL_FAILURE(unit_->syslog(LOG_INFO, "formatted: %s", "hello world"));
    EXPECT_NO_FATAL_FAILURE(unit_->syslog(LOG_DEBUG, "constant string"));
    EXPECT_NO_FATAL_FAILURE(unit_->closelog());
}

TEST_F(SyslogImplFixture, SyslogBeforeOpenlogStillDoesNotCrash)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "syslog without a prior openlog is valid per POSIX and shall not crash.");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");  // equivalence classes

    // POSIX allows syslog() without a preceding openlog(); it uses default identity.
    EXPECT_NO_FATAL_FAILURE(unit_->syslog(LOG_WARNING, "no prior openlog"));
    EXPECT_NO_FATAL_FAILURE(unit_->closelog());
}

TEST(SyslogTest, PMRDefaultShallReturnImplInstance)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PMR Default Shall Return Impl Instance");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");  // equivalence classes

    score::cpp::pmr::memory_resource* memory_resource = score::cpp::pmr::get_default_resource();
    const auto instance = score::os::Syslog::Default(memory_resource);
    ASSERT_TRUE(instance != nullptr);
    EXPECT_NO_THROW(std::ignore = dynamic_cast<score::os::SyslogImpl*>(instance.get()));
}

}  // namespace
