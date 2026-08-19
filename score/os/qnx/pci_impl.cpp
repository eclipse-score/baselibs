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
#include "score/os/qnx/pci_impl.h"

namespace score
{
namespace os
{
namespace qnx
{
pci_bdf_t PciImpl::pci_bdf(std::uint8_t bus, std::uint8_t dev, std::uint8_t func) const noexcept
{
    // Suppress "AUTOSAR C++14 M5-8-1", The rule states: "The right hand operand of a shift operator shall lie
    // between zero and one less than the width in bits of the underlying type of the left hand operand."
    // Rationale: violation is happening out of our code domain - PCI_BDF macro. No harm to our code.
    // coverity[autosar_cpp14_m5_8_1_violation]
    return PCI_BDF(bus, dev, func);
}
score::cpp::expected_blank<std::string> PciImpl::pci_device_cfg_rd32(pci_bdf_t bdf,
                                                                     std::uint16_t offset,
                                                                     std::uint32_t* val) const noexcept
{
    const pci_err_t res = ::pci_device_cfg_rd32(bdf, offset, val);
    if (res != PCI_ERR_OK)
    {
        return score::cpp::make_unexpected<std::string>(pci_strerror(res));
    }
    return {};
}
score::cpp::expected_blank<std::string> PciImpl::pci_device_read_did(pci_bdf_t bdf, pci_did_t* did) const noexcept
{
    const pci_err_t res = ::pci_device_read_did(bdf, did);
    if (res != PCI_ERR_OK)
    {
        return score::cpp::make_unexpected<std::string>(pci_strerror(res));
    }
    return {};
}
score::cpp::expected_blank<std::string> PciImpl::pci_device_read_vid(pci_bdf_t bdf, pci_vid_t* vid) const noexcept
{
    const pci_err_t res = ::pci_device_read_vid(bdf, vid);
    if (res != PCI_ERR_OK)
    {
        return score::cpp::make_unexpected<std::string>(pci_strerror(res));
    }
    return {};
}
score::cpp::expected_blank<std::string> PciImpl::pci_device_read_cmd(const pci_bdf_t bdf,
                                                                     pci_cmd_t* const cmd) const noexcept
{
    const pci_err_t res = ::pci_device_read_cmd(bdf, cmd);
    if (res != PCI_ERR_OK)
    {
        return score::cpp::make_unexpected<std::string>(pci_strerror(res));
    }
    return {};
}
score::cpp::expected<pci_devhdl_t, std::string> PciImpl::pci_device_attach(const pci_bdf_t pdf,
                                                                           const pci_attachFlags_t flags) const noexcept
{
    pci_err_t pci_status = PCI_ERR_OK;
    const pci_devhdl_t devhdl = ::pci_device_attach(pdf, flags, &pci_status);
    if (pci_status == PCI_ERR_OK)
    {
        return devhdl;
    }
    else
    {
        return score::cpp::make_unexpected<std::string>(pci_strerror(pci_status));
    }
}

score::cpp::expected_blank<std::string> PciImpl::pci_device_detach(pci_devhdl_t device_handle) const noexcept
{
    const auto res = ::pci_device_detach(device_handle);
    if (res != PCI_ERR_OK)
    {
        return score::cpp::make_unexpected<std::string>(pci_strerror(res));
    }
    return {};
}

score::cpp::expected_blank<std::string> PciImpl::pci_device_read_ba(const pci_devhdl_t hdl,
                                                                    int_t* const nba,
                                                                    pci_ba_t* const ba,
                                                                    const pci_reqType_t reg_type) const noexcept
{
    const pci_err_t res = ::pci_device_read_ba(hdl, nba, ba, reg_type);
    if (res != PCI_ERR_OK)
    {
        return score::cpp::make_unexpected<std::string>(pci_strerror(res));
    }
    return {};
}

score::cpp::expected<pci_bdf_t, std::string> PciImpl::pci_device_find(const uint_t idx,
                                                                      const pci_vid_t vid,
                                                                      const pci_did_t did,
                                                                      const pci_ccode_t classcode) const noexcept
{
    const pci_bdf_t res = ::pci_device_find(idx, vid, did, classcode);
    if (res == PCI_BDF_NONE)
    {
        return score::cpp::make_unexpected<std::string>("Device not found!");
    }
    return res;
}

}  // namespace qnx
}  // namespace os
}  // namespace score
