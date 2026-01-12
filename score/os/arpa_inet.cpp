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
#include "score/os/arpa_inet.h"
#include <sys/socket.h>

namespace
{

class ArpaInetInstance final : public score::os::ArpaInet
{
  public:
    /* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
    score::os::InAddrT InetAddr(const std::string& ip_addr) const override
    {
        // JUSTIFICATION_BEGIN
        // \ID                score-qnx-banned-builtin
        // \RELATED_RULES     spp-quality-clang-tidy-qnx8-banned-builtins, posix-usage-check, deprecated-api-usage
        // \DESCRIPTION       inet_addr() is a legacy IPv4 address conversion API that remains in limited use for
        //                    backward compatibility with existing network stacks. Under QNX 7/8, the function
        //                    is internally implemented as __inet_addr, triggering a banned-builtin warning.
        //                    This is a known false positive because inet_addr() is implemented using standard
        //                    library routines, not disallowed compiler builtins.
        // \COUNTERMEASURE    The function is used only in non-ASIL, QM legacy paths for maintaining compatibility
        //                    with older modules. The input is validated before conversion and the return value
        //                    is properly checked. Safer alternatives like inet_pton() are preferred in new code.
        //                    Reviewed and accepted for controlled legacy use.
        // JUSTIFICATION_END

        // coverity[score-qnx-banned-builtin] see above justification
        // NOLINTNEXTLINE(score-qnx-banned-builtin): see above justification
        return ::inet_addr(ip_addr.c_str());
    }
    /* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */

    // @todo: use score::Result instead of score::cpp::expected after merging this ticket(Ticket-61178).
    /* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
    score::cpp::expected<score::os::InAddr, score::os::Error::Code> InetPton(const std::string& src) const override
    /* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
    {
        score::os::InAddr imr_multiaddr{};
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:OS library macros */
        // JUSTIFICATION_BEGIN
        // \ID                score-qnx-banned-builtin
        // \RELATED_RULES     spp-quality-clang-tidy-qnx8-banned-builtins, posix-usage-check
        // \DESCRIPTION       The POSIX-standard function inet_pton() is used to convert a text-based IP address
        //                    (IPv4 or IPv6) into its binary form. The QNX 7/8 implementation internally calls
        //                    __inet_pton, which triggers the BMW-QNX banned-builtin warning in the SPP pipeline.
        //                    This is a false positive since the function does not rely on unsafe compiler
        //                    builtins (MMX/SSE/CTZ/CLZ/Popcount) but on standard library routines.
        // \COUNTERMEASURE    The call is bounded and validated: input strings are parsed by higher-level
        //                    address utilities before conversion. The function behavior is deterministic,
        //                    reentrant, and safe for both QM and ASIL contexts.
        //                    Reviewed and approved for use within network initialization code paths.
        // JUSTIFICATION_END

        // coverity[score-qnx-banned-builtin] see above justification
        // NOLINTNEXTLINE(score-qnx-banned-builtin): see above justification
        std::int32_t conversion_state = ::inet_pton(AF_INET, src.c_str(), &imr_multiaddr);
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:OS library macros */
        if (conversion_state == 0)
        {
            // We are using "kInvalidArgument" error code because inet_pton return 0 when 'src' does not contain a
            // character string representing a valid network address in the specified address family, so, it is similar
            // to invalid argument.
            // @todo: use score::MakeUnexpected instead of score::cpp::make_unexpected after merging this ticket(Ticket-61178).
            return score::cpp::make_unexpected(score::os::Error::Code::kInvalidArgument);
        }
        else if (conversion_state == 1)
        {
            // Network address was successfully converted.
            return imr_multiaddr;
        }
        else
        {
            // @todo: use score::MakeUnexpected instead of score::cpp::make_unexpected after merging this ticket(Ticket-61178).
            // Not supported error code returned from 'inet_pton' system call.
            // NOTE_1: ::inet_pton() returns -1 when the domain is not supported, however, in our case this will never
            // happen as we pass in the fixed `AF_INET` parameter. So exclude this line from coverage tool.
            return score::cpp::make_unexpected(score::os::Error::Code::kUnexpected);  // LCOV_EXCL_LINE - see NOTE_1 above
        }
    }
};

}  // namespace

score::os::ArpaInet& score::os::ArpaInet::instance() noexcept
{
    static ArpaInetInstance instance;
    return select_instance(instance);
}
