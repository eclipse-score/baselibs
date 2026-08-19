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
#ifndef SCORE_LIB_OS_MOCKLIB_PCI_MOCK_H
#define SCORE_LIB_OS_MOCKLIB_PCI_MOCK_H

#include "score/os/qnx/pci.h"

#include <gmock/gmock.h>

namespace score
{
namespace os
{
namespace qnx
{

class PciMock : public Pci
{
  public:
    MOCK_METHOD((uint32_t), pci_bdf, (uint8_t, uint8_t, uint8_t), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<std::string>),
                pci_device_cfg_rd32,
                (uint32_t bdf, uint16_t offset, uint32_t* val),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<std::string>),
                pci_device_read_vid,
                (uint32_t bdf, uint16_t* vid),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<std::string>),
                pci_device_read_did,
                (uint32_t bdf, uint16_t* did),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<std::string>),
                pci_device_read_cmd,
                (const pci_bdf_t bdf, pci_cmd_t* const cmd),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<pci_devhdl_t, std::string>),
                pci_device_attach,
                (const pci_bdf_t pdf, const pci_attachFlags_t flags),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<std::string>),
                pci_device_detach,
                (pci_devhdl_t),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<std::string>),
                pci_device_read_ba,
                (const pci_devhdl_t hdl, int_t* const nba, pci_ba_t* const ba, const pci_reqType_t reg_type),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<pci_bdf_t, std::string>),
                pci_device_find,
                (const uint_t, const pci_vid_t, const pci_did_t, const pci_ccode_t),
                (const, noexcept, override));
};

}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_PCI_MOCK_H
