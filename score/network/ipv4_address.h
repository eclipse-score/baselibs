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
///
/// @file ipv4_address.h
/// @copyright Copyright (C) 2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Header for IPv4Address class
///

#ifndef SCORE_LIB_OS_UTILS_IPV4_ADDRESS_H
#define SCORE_LIB_OS_UTILS_IPV4_ADDRESS_H

#include <array>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

namespace score::os
{

class Ipv4Address
{
  public:
    static constexpr std::uint8_t kMaxNetMask = 32U;
    static constexpr std::uint8_t kDefaultNetMask = 24U;
    static constexpr std::uint8_t kIpv4AddressLength = 4U;

    using AddressBytes = std::array<std::uint8_t, kIpv4AddressLength>;

    Ipv4Address() noexcept;

    explicit Ipv4Address(const std::string& address) noexcept;

    explicit Ipv4Address(const AddressBytes& address, const std::uint8_t netmask = kDefaultNetMask);

    explicit Ipv4Address(const std::vector<std::uint8_t>& address, const std::uint8_t netmask = kDefaultNetMask);

    static std::uint8_t ConvertNetmaskUint32NetOrderToPrefix(const std::uint32_t netmask);

    static Ipv4Address CreateFromUint32NetOrder(const std::uint32_t address,
                                                const std::uint8_t prefix = kDefaultNetMask);

    static Ipv4Address CreateFromUint32HostOrder(const std::uint32_t address,
                                                 const std::uint8_t prefix = kDefaultNetMask);

    Ipv4Address(const std::uint8_t byte1,
                const std::uint8_t byte2,
                const std::uint8_t byte3,
                const std::uint8_t byte4,
                const std::uint8_t prefix = kDefaultNetMask);

    std::string ToString() const noexcept;

    std::string ToStringWithNetmask() const noexcept;

    static Ipv4Address Broadcast()
    {
        return Ipv4Address(0xFF, 0xFF, 0xFF, 0xFF);
    }

    std::vector<std::uint8_t> ToBytes() const noexcept;
    AddressBytes ToIpv4Bytes() const noexcept;
    std::uint8_t GetNetmask() const noexcept;
    AddressBytes GetNetmaskBytes() const noexcept;
    void SetNetmask(const std::uint8_t netmask) noexcept;

    bool IsValid() const noexcept;

    bool IsIpv6() const noexcept;

    bool operator==(const Ipv4Address& rhs) const noexcept;

    bool operator!=(const Ipv4Address& rhs) const noexcept;

    bool operator<(const Ipv4Address& rhs) const noexcept
    {
        if ((ipv4_address_bytes_ < rhs.ipv4_address_bytes_) ||
            ((ipv4_address_bytes_ == rhs.ipv4_address_bytes_) && (ipv4_netmask_ < rhs.ipv4_netmask_)))
        {
            return true;
        }
        return false;
    }

  private:
    static bool IsLittleEndian() noexcept
    {
        // LE: 0x01 0x00 0x00 0x00
        // BE: 0x00 0x00 0x00 0x01
        std::int32_t one = 1;
        // Despite AUTOSAR A5-2-4 advising against reinterpret_cast, in this specific case, no memory safety violations
        // would occur.
        // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast): safe usage of reintrpret_cast
        // coverity[autosar_cpp14_a5_2_4_violation] : see justification above
        return (*reinterpret_cast<std::uint8_t*>(&one) != 0U);
        // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
    }

    static Ipv4Address CreateFromUint32BE(const std::uint32_t address, const std::uint8_t prefix);

    static Ipv4Address CreateFromUint32LE(const std::uint32_t address, const std::uint8_t prefix);

    static bool ParseByte(const std::string& byte_str,
                          std::uint8_t& byte,
                          const std::uint8_t max_allowed_value = std::numeric_limits<std::uint8_t>::max()) noexcept;

    // This is in network order
    AddressBytes ipv4_address_bytes_{};
    std::uint8_t ipv4_netmask_{kDefaultNetMask};
};

}  // namespace score::os

#endif  // XNM_SRC_IPV4_ADDRESS_H
