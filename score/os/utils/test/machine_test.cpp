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
#include "score/os/utils/machine.h"
#include "score/os/utils/machine_seam.h"

#include <gtest/gtest.h>

namespace score
{
namespace os
{
namespace
{

TEST(MachineImpl, instance)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MachineImpl instance");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    score::os::Machine& obj = score::os::Machine::instance();
    ASSERT_TRUE(&obj);
}

TEST(MachineImpl, is_qemu)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MachineImpl is_qemu");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

#ifdef MACHINE_QEMU
    ASSERT_TRUE(score::os::Machine::instance().is_qemu());
#else
    ASSERT_FALSE(score::os::Machine::instance().is_qemu());
#endif
}

TEST(Machine, is_sctf)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Machine is_sctf");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    ASSERT_TRUE(setenv("SCTF", "TRUE", 1) == 0);
    ASSERT_TRUE(is_sctf());
    ASSERT_TRUE(unsetenv("SCTF") == 0);
}

TEST(Machine, is_sctf_false)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Machine is_sctf_false");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    ASSERT_FALSE(is_sctf());
}

}  // namespace
}  // namespace os
}  // namespace score
