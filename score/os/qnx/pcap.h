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
#ifndef SCORE_LIB_OS_QNX_PCAP_H
#define SCORE_LIB_OS_QNX_PCAP_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include <score/expected.hpp>

extern "C" {
#include <pcap/pcap.h>
}

namespace score
{
namespace os
{

class Pcap : public ObjectSeam<Pcap>
{
  public:
    static Pcap& instance() noexcept;

    virtual score::cpp::expected<pcap_t*, Error> pcap_open_live(const char* device,
                                                                std::int32_t snaplen,
                                                                std::int32_t promisc,
                                                                std::int32_t to_ms,
                                                                char* errbuf) const noexcept = 0;

    virtual score::cpp::expected<pcap_t*, Error> pcap_open_dead(std::int32_t linktype,
                                                                std::int32_t snaplen) const noexcept = 0;

    virtual score::cpp::expected_blank<Error> pcap_loop(pcap_t* p,
                                                        std::int32_t cnt,
                                                        pcap_handler callback,
                                                        u_char* user) const noexcept = 0;

    virtual score::cpp::expected_blank<Error> pcap_breakloop(pcap_t* p) const noexcept = 0;

    virtual score::cpp::expected_blank<Error> pcap_close(pcap_t* p) const noexcept = 0;

    /* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
    virtual score::cpp::expected<char*, Error> pcap_geterr(pcap_t* p) const noexcept = 0;
    /* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC*/

    virtual score::cpp::expected<int, Error> pcap_compile(pcap_t* p,
                                                          struct bpf_program* fp,
                                                          const char* str,
                                                          int optimize,
                                                          bpf_u_int32 netmask) const noexcept = 0;

    virtual score::cpp::expected<int, Error> pcap_setfilter(pcap_t* p, struct bpf_program* fp) const noexcept = 0;

    virtual score::cpp::expected_blank<Error> pcap_freecode(struct bpf_program* fp) const noexcept = 0;

    virtual score::cpp::expected<pcap_dumper_t*, Error> pcap_dump_open(pcap_t* p,
                                                                       const char* filename) const noexcept = 0;

    virtual score::cpp::expected_blank<Error> pcap_dump(u_char* user,
                                                        const struct pcap_pkthdr* h,
                                                        const u_char* sp) const noexcept = 0;

    virtual score::cpp::expected_blank<Error> pcap_dump_close(pcap_dumper_t* p) const noexcept = 0;

    virtual ~Pcap() = default;
};

}  // namespace os
}  // namespace score
#endif  // SCORE_LIB_OS_QNX_PCAP_H
