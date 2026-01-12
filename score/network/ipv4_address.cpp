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
#include "score/network/ipv4_address.h"

#include <regex>

namespace
{
std::string uint8_to_string(std::uint8_t value)
{
    return std::to_string(static_cast<unsigned int>(value));
}

constexpr std::uint8_t SelectByteLE(std::uint32_t address, std::uint8_t byte_position)
{
    return static_cast<std::uint8_t>((address >> (byte_position * 8U)) & 0xFFU);
}

}  // namespace

namespace score::os
{

Ipv4Address::Ipv4Address() noexcept = default;

/* KW_SUPPRESS_START:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False Positive */
Ipv4Address::Ipv4Address(const std::string& address) noexcept
/* KW_SUPPRESS_END:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False Positive */
{
    std::smatch match;
    std::regex pattern(
        "^([[:digit:]]{1,3})\\.([[:digit:]]{1,3})\\.([[:digit:]]{1,3})\\.([[:digit:]]{1,3})(?:/"
        "([[:digit:]]{1,2}))?$");
    if (std::regex_search(address, match, pattern) && match.size() >= 5U)
    {
        if (!ParseByte(match[1], ipv4_address_bytes_[0]) || !ParseByte(match[2], ipv4_address_bytes_[1]) ||
            !ParseByte(match[3], ipv4_address_bytes_[2]) || !ParseByte(match[4], ipv4_address_bytes_[3]))
        {
            ipv4_address_bytes_.fill(0U);
        }
        if (match.size() == 6U && match[5U] != "")
        {
            if (!ParseByte(match[5U], ipv4_netmask_, kMaxNetMask))
            {
                SetNetmask(kDefaultNetMask);
            }
        }
        else
        {
            SetNetmask(kDefaultNetMask);
        }
    }
}

Ipv4Address::Ipv4Address(const Ipv4Address::AddressBytes& address, const std::uint8_t netmask)
{
    ipv4_address_bytes_ = address;
    SetNetmask(netmask);
}

Ipv4Address::Ipv4Address(const std::vector<std::uint8_t>& address, const std::uint8_t netmask)
{
    if (address.size() == kIpv4AddressLength)
    {
        ipv4_address_bytes_ = Ipv4Address::AddressBytes{address.at(0), address.at(1), address.at(2), address.at(3)};
        SetNetmask(netmask);
    }
}

Ipv4Address::Ipv4Address(const std::uint8_t byte1,
                         const std::uint8_t byte2,
                         const std::uint8_t byte3,
                         const std::uint8_t byte4,
                         const std::uint8_t prefix)
{
    ipv4_address_bytes_[0U] = byte1;
    ipv4_address_bytes_[1U] = byte2;
    ipv4_address_bytes_[2U] = byte3;
    ipv4_address_bytes_[3U] = byte4;
    SetNetmask(prefix);
}

std::uint8_t Ipv4Address::ConvertNetmaskUint32NetOrderToPrefix(const std::uint32_t netmask)
{
    // reinterpret_cast is used here to reinterpret the netmask value as an array of std::uint8_t bytes, allowing
    // access individual byte components. This is required for examining each bit of the netmask during the loop
    // iteration.
    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast): safe usage of reintrpret_cast
    // coverity[autosar_cpp14_a5_2_4_violation] : see justification above
    const auto netmask_bytes = reinterpret_cast<const std::uint8_t*>(&netmask);
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
    for (std::uint8_t prefix = 0; prefix < kMaxNetMask; ++prefix)
    {
        auto byte_pos = static_cast<std::uint8_t>(prefix / 8);
        auto bit_pos = static_cast<std::uint8_t>(7 - prefix % 8);
        // NOLINTBEGIN(*pointer-arithmetic) safe use of pointer arithmetic
        // coverity[autosar_cpp14_m5_0_15_violation] : safe use of pointer arithmetic
        if (static_cast<std::uint8_t>(netmask_bytes[byte_pos] & static_cast<std::uint8_t>(1U << bit_pos)) == 0)
        // NOLINTEND(*pointer-arithmetic)
        {
            return prefix;
        }
    }
    return kMaxNetMask;
}

Ipv4Address Ipv4Address::CreateFromUint32NetOrder(const std::uint32_t address, const std::uint8_t prefix)
{
    return CreateFromUint32BE(address, prefix);
}

Ipv4Address Ipv4Address::CreateFromUint32HostOrder(const std::uint32_t address, const std::uint8_t prefix)
{
    if (IsLittleEndian()) /* LCOV_EXCL_BR_LINE: Not possible to simulate bigenddian. */
    {
        return CreateFromUint32LE(address, prefix);
    }
    else
    {
        // Not possible to simulate bigenddian through unit tests.
        return CreateFromUint32BE(address, prefix);  // LCOV_EXCL_LINE
    }
}

std::string Ipv4Address::ToString() const noexcept
{
    std::string addr = uint8_to_string(ipv4_address_bytes_[0]) + "." + uint8_to_string(ipv4_address_bytes_[1]) + "." +
                       uint8_to_string(ipv4_address_bytes_[2]) + "." + uint8_to_string(ipv4_address_bytes_[3]);
    return addr;
}

std::string Ipv4Address::ToStringWithNetmask() const noexcept
{
    return ToString() + "/" + uint8_to_string(ipv4_netmask_);
}

std::vector<std::uint8_t> Ipv4Address::ToBytes() const noexcept
{
    return {ipv4_address_bytes_.begin(), ipv4_address_bytes_.end()};
}
Ipv4Address::AddressBytes Ipv4Address::ToIpv4Bytes() const noexcept
{
    return ipv4_address_bytes_;
}
std::uint8_t Ipv4Address::GetNetmask() const noexcept
{
    return ipv4_netmask_;
}

void Ipv4Address::SetNetmask(const std::uint8_t netmask) noexcept
{
    (IsValid() && (netmask <= kMaxNetMask)) ? ipv4_netmask_ = netmask : ipv4_netmask_ = kMaxNetMask;
}

bool Ipv4Address::IsValid() const noexcept
{
    return (ipv4_address_bytes_[0] != 0) || (ipv4_address_bytes_[1] != 0) || (ipv4_address_bytes_[2] != 0) ||
           (ipv4_address_bytes_[3] != 0);
}

bool Ipv4Address::IsIpv6() const noexcept
{
    return false;
}

bool Ipv4Address::operator==(const Ipv4Address& rhs) const noexcept
{
    return (ipv4_address_bytes_ == rhs.ipv4_address_bytes_) && (ipv4_netmask_ == rhs.ipv4_netmask_);
}

bool Ipv4Address::operator!=(const Ipv4Address& rhs) const noexcept
{
    return !operator==(rhs);
}

Ipv4Address Ipv4Address::CreateFromUint32BE(const std::uint32_t address, const std::uint8_t prefix)
{
    return Ipv4Address(SelectByteLE(address, 0U),
                       SelectByteLE(address, 1U),
                       SelectByteLE(address, 2U),
                       SelectByteLE(address, 3U),
                       prefix);
}

Ipv4Address Ipv4Address::CreateFromUint32LE(const std::uint32_t address, const std::uint8_t prefix)
{
    return Ipv4Address(SelectByteLE(address, 3U),
                       SelectByteLE(address, 2U),
                       SelectByteLE(address, 1U),
                       SelectByteLE(address, 0U),
                       prefix);
}

bool Ipv4Address::ParseByte(const std::string& byte_str,
                            std::uint8_t& byte,
                            const std::uint8_t max_allowed_value) noexcept
{
    char* endptr = nullptr;
    // Using strtol since stol can throw
    auto int_value = std::strtol(byte_str.c_str(), &endptr, 10);
    byte = static_cast<std::uint8_t>(int_value);
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic) safe use of pointer arithmetic
    // coverity[autosar_cpp14_m5_0_15_violation] : safe use of pointer arithmetic
    return (endptr == (byte_str.c_str() + byte_str.size())) && ((int_value >= 0) && (int_value <= max_allowed_value));
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

Ipv4Address::AddressBytes Ipv4Address::GetNetmaskBytes() const noexcept
{
    const std::uint32_t mask = 0xFFFFFFFFU << (kMaxNetMask - ipv4_netmask_);
    Ipv4Address::AddressBytes ret{};
    if (IsLittleEndian()) /* LCOV_EXCL_BR_LINE: Not possible to simulate bigenddian. */
    {
        ret[0] = SelectByteLE(mask, 3U);
        ret[1] = SelectByteLE(mask, 2U);
        ret[2] = SelectByteLE(mask, 1U);
        ret[3] = SelectByteLE(mask, 0U);
    }
    else
    {
        // LCOV_EXCL_START: Not possible to simulate bigenddian through unit tests. */
        ret[0] = SelectByteLE(mask, 0U);
        ret[1] = SelectByteLE(mask, 1U);
        ret[2] = SelectByteLE(mask, 2U);
        ret[3] = SelectByteLE(mask, 3U);
        // LCOV_EXCL_STOP
    }

    return ret;
}

}  // namespace score::os
