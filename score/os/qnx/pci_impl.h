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
#ifndef SCORE_LIB_OS_QNX_PCI_IMPL_H
#define SCORE_LIB_OS_QNX_PCI_IMPL_H

#include "score/os/qnx/pci.h"

namespace score
{
namespace os
{
namespace qnx
{

class PciImpl final : public Pci
{
  public:
    pci_bdf_t pci_bdf(std::uint8_t bus, std::uint8_t dev, std::uint8_t func) const noexcept override;
    score::cpp::expected_blank<std::string> pci_device_cfg_rd32(pci_bdf_t bdf,
                                                                std::uint16_t offset,
                                                                std::uint32_t* val) const noexcept override;
    score::cpp::expected_blank<std::string> pci_device_read_did(pci_bdf_t bdf, pci_did_t* did) const noexcept override;
    score::cpp::expected_blank<std::string> pci_device_read_vid(pci_bdf_t bdf, pci_vid_t* vid) const noexcept override;
    score::cpp::expected_blank<std::string> pci_device_read_cmd(const pci_bdf_t bdf,
                                                                pci_cmd_t* const cmd) const noexcept override;
    score::cpp::expected<pci_devhdl_t, std::string> pci_device_attach(
        const pci_bdf_t pdf,
        const pci_attachFlags_t flags) const noexcept override;
    score::cpp::expected_blank<std::string> pci_device_detach(pci_devhdl_t device_handle) const noexcept override;
    score::cpp::expected_blank<std::string> pci_device_read_ba(const pci_devhdl_t hdl,
                                                               int_t* const nba,
                                                               pci_ba_t* const ba,
                                                               const pci_reqType_t reg_type) const noexcept override;

    score::cpp::expected<pci_bdf_t, std::string> pci_device_find(const uint_t idx,
                                                                 const pci_vid_t vid,
                                                                 const pci_did_t did,
                                                                 const pci_ccode_t classcode) const noexcept override;
};

}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_PCI_IMPL_H
