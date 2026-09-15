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
#include "score/os/netdb.h"

extern "C" {
#include <netdb.h>
}

namespace score
{
namespace os
{

namespace
{

class NetdbImpl final : public Netdb
{
  public:
    score::cpp::expected_blank<Error> getnameinfo(const struct sockaddr* sa,
                                                  const socklen_t salen,
                                                  char* host,
                                                  const socklen_t hostlen,
                                                  char* serv,
                                                  const socklen_t servlen,
                                                  const NameFlag flags) const noexcept override
    {
        if (::getnameinfo(sa, salen, host, hostlen, serv, servlen, nameflag_to_nativeflag(flags)) != 0)
        {
            return score::cpp::make_unexpected(score::os::Error::createFromErrno());
        }
        return {};
    }

  private:
    std::int32_t nameflag_to_nativeflag(const NameFlag flags) const noexcept
    {
        std::int32_t native_flags{};
        if (flags & NameFlag::kNameReq)
        {
            // NOLINTNEXTLINE(hicpp-signed-bitwise): macro does not affect the sign of the result.
            native_flags |= NI_NAMEREQD;
        }
        if (flags & NameFlag::kDatagramService)
        {
            // NOLINTNEXTLINE(hicpp-signed-bitwise): macro does not affect the sign of the result.
            native_flags |= NI_DGRAM;
        }
        if (flags & NameFlag::kNodeNameOfDomain)
        {
            // NOLINTNEXTLINE(hicpp-signed-bitwise): macro does not affect the sign of the result.
            native_flags |= NI_NOFQDN;
        }
        if (flags & NameFlag::kNumericHost)
        {
            // NOLINTNEXTLINE(hicpp-signed-bitwise): macro does not affect the sign of the result.
            native_flags |= NI_NUMERICHOST;
        }
        if (flags & NameFlag::kNumericService)
        {
            // NOLINTNEXTLINE(hicpp-signed-bitwise): macro does not affect the sign of the result.
            native_flags |= NI_NUMERICSERV;
        }
        return native_flags;
    }
};

}  // namespace
}  // namespace os
}  // namespace score

score::os::Netdb& score::os::Netdb::instance() noexcept
{
    static score::os::NetdbImpl instance;
    return select_instance(instance);
}
