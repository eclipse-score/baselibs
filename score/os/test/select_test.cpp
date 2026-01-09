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
#include "score/os/select.h"
#include "score/os/select_impl.h"

#include <fcntl.h>
#include <gtest/gtest.h>

namespace score
{
namespace os
{

namespace
{

using namespace ::testing;

TEST(SelectTest, selectPass)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SelectTest select Pass");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    score::os::SelectImpl select;
    fd_set fds;
    struct timeval tv;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    tv.tv_sec = 0;
    tv.tv_usec = 500;
    score::cpp::expected<std::int32_t, score::os::Error> retval = select.select(1, &fds, NULL, NULL, &tv);
    ASSERT_NE(retval.value(), -1);
}

TEST(SelectTest, selectPassInstance)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SelectTest select Pass Instance");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    score::os::Select& select = score::os::Select::instance();
    fd_set fds;
    struct timeval tv;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    tv.tv_sec = 0;
    tv.tv_usec = 500;
    score::cpp::expected<std::int32_t, score::os::Error> retval = select.select(1, &fds, NULL, NULL, &tv);
    ASSERT_NE(retval.value(), -1);
}

}  // namespace
}  // namespace os
}  // namespace score
