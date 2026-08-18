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
/*KW_SUPPRESS_START: MISRA.INCL.INSIDE:extern "C" is needed to link C related code*/
#include <netdb.h>
/*KW_SUPPRESS_END: MISRA.INCL.INSIDE:extern "C" is needed to link C related code*/
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */

namespace score
{
namespace os
{

namespace
{

class NetdbImpl final : public Netdb
{
  public:
    /* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
    score::cpp::expected_blank<Error> getnameinfo(const struct sockaddr* sa,
                                                  const socklen_t salen,
                                                  char* host,
                                                  const socklen_t hostlen,
                                                  char* serv,
                                                  const socklen_t servlen,
                                                  const NameFlag flags) const noexcept override
    /* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
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
            /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the sign of the result */
            /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:OS library macros */
            // NOLINTNEXTLINE(hicpp-signed-bitwise): macro does not affect the sign of the result.
            native_flags |= NI_NAMEREQD;
            /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:OS library macros */
            /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the sign of the result */
        }
        if (flags & NameFlag::kDatagramService)
        {
            /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the sign of the result */
            /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:OS library macros */
            // NOLINTNEXTLINE(hicpp-signed-bitwise): macro does not affect the sign of the result.
            native_flags |= NI_DGRAM;
            /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:OS library macros */
            /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the sign of the result */
        }
        if (flags & NameFlag::kNodeNameOfDomain)
        {
            /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the sign of the result */
            /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:OS library macros */
            // NOLINTNEXTLINE(hicpp-signed-bitwise): macro does not affect the sign of the result.
            native_flags |= NI_NOFQDN;
            /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:OS library macros */
            /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the sign of the result */
        }
        if (flags & NameFlag::kNumericHost)
        {
            /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the sign of the result */
            /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:OS library macros */
            // NOLINTNEXTLINE(hicpp-signed-bitwise): macro does not affect the sign of the result.
            native_flags |= NI_NUMERICHOST;
            /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:OS library macros */
            /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the sign of the result */
        }
        if (flags & NameFlag::kNumericService)
        {
            /* KW_SUPPRESS_START:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the sign of the result */
            /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:OS library macros */
            // NOLINTNEXTLINE(hicpp-signed-bitwise): macro does not affect the sign of the result.
            native_flags |= NI_NUMERICSERV;
            /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:OS library macros */
            /* KW_SUPPRESS_END:MISRA.BITS.NOT_UNSIGNED: Macro does not affect the sign of the result */
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

/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
