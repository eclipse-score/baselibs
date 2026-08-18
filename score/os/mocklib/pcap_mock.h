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
#ifndef SCORE_LIB_OS_MOCKLIB_PCAPMOCK_H
#define SCORE_LIB_OS_MOCKLIB_PCAPMOCK_H

#include "score/os/qnx/pcap.h"

#include <gmock/gmock.h>

namespace score
{
namespace os
{

class PcapMock : public Pcap
{
  public:
    MOCK_METHOD((score::cpp::expected<pcap_t*, Error>),
                pcap_open_live,
                (const char* device, std::int32_t snaplen, std::int32_t promisc, std::int32_t to_ms, char* errbuf),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<pcap_t*, Error>),
                pcap_open_dead,
                (std::int32_t linktype, std::int32_t snaplen),
                (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<Error>,
                pcap_loop,
                (pcap_t * p, std::int32_t cnt, pcap_handler callback, u_char* user),
                (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<Error>, pcap_breakloop, (pcap_t * p), (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<Error>, pcap_close, (pcap_t * p), (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<char*, Error>), pcap_geterr, (pcap_t * p), (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<int, Error>),
                pcap_compile,
                (pcap_t * p, struct bpf_program* fp, const char* str, int optimize, bpf_u_int32 netmask),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<int, Error>),
                pcap_setfilter,
                (pcap_t * p, struct bpf_program* fp),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<Error>),
                pcap_freecode,
                (struct bpf_program * fp),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected<pcap_dumper_t*, Error>),
                pcap_dump_open,
                (pcap_t * p, const char* filename),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<Error>),
                pcap_dump,
                (u_char * user, const struct pcap_pkthdr* h, const u_char* sp),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<Error>), pcap_dump_close, (pcap_dumper_t * p), (const, noexcept, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_PCAPMOCK_H
