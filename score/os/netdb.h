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
#ifndef SCORE_LIB_OS_NETDB_H
#define SCORE_LIB_OS_NETDB_H

#include "score/bitmanipulation/bitmask_operators.h"
#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"

#include <sys/socket.h>

namespace score
{
namespace os
{

class Netdb : public ObjectSeam<Netdb>
{
  public:
    static Netdb& instance() noexcept;

    enum class NameFlag : std::int32_t
    {
        kNameReq = 0x01,
        kDatagramService = 0x02,
        kNodeNameOfDomain = 0x04,
        kNumericHost = 0x08,
        kNumericService = 0x10,
    };

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected_blank<Error> getnameinfo(const struct sockaddr* sa,
                                                   const socklen_t salen,
                                                   char* host,
                                                   const socklen_t hostlen,
                                                   char* serv,
                                                   const socklen_t servlen,
                                                   const NameFlag flags) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    virtual ~Netdb() = default;

  protected:
    Netdb() = default;
    Netdb(const Netdb&) = default;
    Netdb(Netdb&&) = default;
    Netdb& operator=(const Netdb&) = default;
    Netdb& operator=(Netdb&&) = default;
};

}  // namespace os
}  // namespace score

namespace score
{
template <>
struct enable_bitmask_operators<::score::os::Netdb::NameFlag> : public std::true_type
{
};
}  // namespace score

#endif  // SCORE_LIB_OS_NETDB_H
