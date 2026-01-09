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
#include "score/os/qnx/resmgr_impl.h"

#include <gtest/gtest.h>

namespace
{

TEST(ResMgrTest, ResMgrMsgWriteFails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Res Mgr Msg Write Fails");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    score::os::ResMgrImpl resmgr;
    resmgr_context_t ctp;
    memset(&ctp, 0, sizeof(resmgr_context_t));

    auto msgwrite_result = resmgr.resmgr_msgwrite(&ctp, nullptr, 0, 0);
    ASSERT_FALSE(msgwrite_result);
}

}  // namespace
