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
#include "score/os/qnx/pci_safety.h"
#include "score/os/qnx/pci_safety_impl.h"

#include <gtest/gtest.h>

namespace
{
struct PciSafetylTest : ::testing::Test
{
    void SetUp() override {}
    void TearDown() override {}

    score::os::qnx::PciSafetyImpl pci_;
    std::uint8_t bus = 0;
    std::uint8_t dev = 4;
    std::uint8_t func = 0;
};

TEST_F(PciSafetylTest, pci_device_cfg_rd32_fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pci Device Cfg Rd Fails");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pci_bdf_t bdf;
    std::uint16_t offset{64};
    std::uint32_t val{0};
    std::memset(&bdf, 0xff, sizeof(pci_bdf_t));

    ASSERT_FALSE(pci_.pci_device_cfg_rd32(bdf, offset, &val).has_value());
}

TEST_F(PciSafetylTest, pci_device_read_did_fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pci Device Read Did Fails");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pci_bdf_t bdf;
    pci_did_t did;
    std::memset(&bdf, 0xff, sizeof(pci_bdf_t));

    ASSERT_FALSE(pci_.pci_device_read_did(bdf, &did).has_value());
}

TEST_F(PciSafetylTest, pci_device_read_vid_fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pci Device Read Vid Fails");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pci_bdf_t bdf;
    pci_vid_t vid;
    std::memset(&bdf, 0xff, sizeof(pci_bdf_t));

    ASSERT_FALSE(pci_.pci_device_read_vid(bdf, &vid).has_value());
}

}  // namespace
