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
#ifndef SCORE_LIB_OS_ARPA_INET_H
#define SCORE_LIB_OS_ARPA_INET_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"
#include "score/os/netinet_in.h"

#include <arpa/inet.h>
#include <score/expected.hpp>

#include <cstdint>

namespace score
{
namespace os
{

class ArpaInet : public ObjectSeam<ArpaInet>
{
  public:
    /// \brief thread-safe singleton accessor
    /// \return Either concrete OS-dependent instance or respective set mock instance
    static ArpaInet& instance() noexcept;

    virtual InAddrT InetAddr(const std::string& ip_addr) const = 0;
    // @todo: use score::Result instead of score::cpp::expected after merging this ticket(Ticket-61178).
    virtual score::cpp::expected<InAddr, Error::Code> InetPton(const std::string& src) const = 0;

    virtual ~ArpaInet() = default;

  protected:
    ArpaInet() = default;
    ArpaInet(const ArpaInet&) = default;
    ArpaInet(ArpaInet&&) = default;
    ArpaInet& operator=(const ArpaInet&) = default;
    ArpaInet& operator=(ArpaInet&&) = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_ARPA_INET_H
