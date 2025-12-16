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
#include "score/os/qnx/neutrino_impl.h"
#include "score/os/qnx/pci_impl.h"

#include <gtest/gtest.h>
#include <cstring>

namespace
{
struct PciTest : ::testing::Test
{
    void SetUp() override
    {
        ASSERT_TRUE(
            neutrino_.ThreadCtl(static_cast<std::int32_t>(score::os::qnx::Neutrino::TCtlCommands::kTCtlIo), nullptr)
                .has_value());
    }
    score::os::qnx::PciImpl pci_;
    score::os::qnx::NeutrinoImpl neutrino_;
    std::uint8_t bus_ = 0U;
    std::uint8_t dev_ = 4U;
    std::uint8_t func_ = 0U;
};

TEST_F(PciTest, pci_device_read_cmd_fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pci Device Read Cmd Fails");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pci_bdf_t bdf;
    pci_cmd_t cmd;
    std::memset(&bdf, 0xff, sizeof(pci_bdf_t));

    ASSERT_FALSE(pci_.pci_device_read_cmd(bdf, &cmd).has_value());
}

TEST_F(PciTest, pci_device_read_cmd_succeed)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pci Device Read Cmd Succeed");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pci_cmd_t cmd;
    pci_bdf_t bdf = pci_.pci_bdf(bus_, dev_, func_);

    ASSERT_TRUE(pci_.pci_device_read_cmd(bdf, &cmd).has_value());
}

TEST_F(PciTest, pci_device_attach_fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pci Device Attach Fails");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pci_bdf_t bdf;
    std::memset(&bdf, 0, sizeof(pci_bdf_t));

    ASSERT_FALSE(pci_.pci_device_attach(bdf, static_cast<pci_attachFlags_t>(0xff)).has_value());
}

TEST_F(PciTest, pci_device_read_ba_fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pci Device Read Ba Fails");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pci_devhdl_t hdl;
    std::memset(&hdl, 0, sizeof(pci_bdf_t));

    ASSERT_FALSE(pci_.pci_device_read_ba(hdl, nullptr, nullptr, static_cast<pci_reqType_t>(9)).has_value());
}

TEST_F(PciTest, pci_device_read_ba_succeed)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pci Device Read Ba Succeed");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pci_bdf_t bdf = pci_.pci_bdf(bus_, dev_, func_);
    auto attach_res = pci_.pci_device_attach(bdf, pci_attachFlags_MULTI_OWNER);
    ASSERT_TRUE(attach_res.has_value());

    pci_devhdl_t hdl = attach_res.value();

    pci_ba_t ba[1];
    int_t nba = NELEMENTS(ba);
    ba[0].bar_num = 0;

    ASSERT_TRUE(pci_.pci_device_read_ba(hdl, &nba, ba, pci_reqType_e_MANDATORY));
    ASSERT_EQ(pci_device_detach(hdl), PCI_ERR_OK);
}

TEST_F(PciTest, pci_device_cfg_rd32_fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pci Device Cfg Rd Fails");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pci_bdf_t bdf;
    std::uint16_t offset{64};
    std::uint32_t val = 0;
    std::memset(&bdf, 0xff, sizeof(pci_bdf_t));

    ASSERT_FALSE(pci_.pci_device_cfg_rd32(bdf, offset, &val).has_value());
}

TEST_F(PciTest, pci_device_cfg_rd32_succeed)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pci Device Cfg Rd Succeed");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::uint16_t offset{64U};
    std::uint32_t val = 0U;
    pci_bdf_t bdf = pci_.pci_bdf(bus_, dev_, func_);

    ASSERT_TRUE(pci_.pci_device_cfg_rd32(bdf, offset, &val).has_value());
    EXPECT_NO_THROW(score::os::qnx::Pci::instance());
}

TEST_F(PciTest, pci_device_read_did_fails)
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

TEST_F(PciTest, pci_device_read_did_succeed)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pci Device Read Did Succeed");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pci_did_t did;
    pci_bdf_t bdf = pci_.pci_bdf(bus_, dev_, func_);

    ASSERT_TRUE(pci_.pci_device_read_did(bdf, &did).has_value());
}

TEST_F(PciTest, pci_device_read_vid_fails)
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

TEST_F(PciTest, pci_device_read_vid_succeed)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pci Device Read Vid Succeed");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pci_vid_t vid;
    pci_bdf_t bdf = pci_.pci_bdf(bus_, dev_, func_);

    ASSERT_TRUE(pci_.pci_device_read_vid(bdf, &vid).has_value());
}

TEST_F(PciTest, pci_device_detach_succeed)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pci Device Detach Succeed");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pci_bdf_t bdf = pci_.pci_bdf(bus_, dev_, func_);
    auto attach_res = pci_.pci_device_attach(bdf, pci_attachFlags_MULTI_OWNER);
    ASSERT_TRUE(attach_res.has_value());

    pci_devhdl_t hdl = attach_res.value();
    ASSERT_TRUE(pci_.pci_device_detach(hdl).has_value());
}

TEST_F(PciTest, pci_device_detach_fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pci Device Detach Fails");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pci_devhdl_t hdl = static_cast<pci_devhdl_t>(0);  // Invalid handle
    EXPECT_FALSE(pci_.pci_device_detach(hdl).has_value());
}

TEST_F(PciTest, pci_device_find_succeed)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pci Device Find Succeed");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pci_vid_t vid = 0xFFFF;
    pci_did_t did = 0xFFFF;
    pci_ccode_t classcode = 0xFFFF;
    EXPECT_TRUE(pci_.pci_device_find(0, vid, did, classcode).has_value());
}

TEST_F(PciTest, pci_device_find_fails)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pci Device Find Fails");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pci_vid_t vid = 0x1234;
    pci_did_t did = 0x5678;
    pci_ccode_t classcode = 0x0001;
    ASSERT_FALSE(pci_.pci_device_find(0U, vid, did, classcode).has_value());
}

}  // namespace
