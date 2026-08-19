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
#include "score/os/qnx/pcap.h"

namespace score
{
namespace os
{
namespace
{

class PcapImpl final : public Pcap
{
  public:
    score::cpp::expected<pcap_t*, Error> pcap_open_live(const char* device,
                                                        std::int32_t snaplen,
                                                        std::int32_t promisc,
                                                        std::int32_t to_ms,
                                                        char* errbuf) const noexcept override
    {
        pcap_t* handle{nullptr};
        handle = ::pcap_open_live(device, snaplen, promisc, to_ms, errbuf);

        if (handle == nullptr)
        {
            return score::cpp::make_unexpected(::score::os::Error::createFromErrno());
        }

        return handle;
    }

    score::cpp::expected<pcap_t*, Error> pcap_open_dead(std::int32_t linktype,
                                                        std::int32_t snaplen) const noexcept override
    {
        pcap_t* handle{nullptr};
        handle = ::pcap_open_dead(linktype, snaplen);

        if (handle == nullptr)
        {
            // NOTE_1: ::pcap_open_dead() returns nullptr only when malloc fails.
            // This takes too much effort to test in UT, so we will not test this case.
            return score::cpp::make_unexpected(::score::os::Error::createFromErrno());  // LCOV_EXCL_LINE see NOTE_1
        }

        return handle;
    }

    score::cpp::expected_blank<Error> pcap_loop(pcap_t* p,
                                                std::int32_t cnt,
                                                pcap_handler callback,
                                                u_char* user) const noexcept override
    {
        if (::pcap_loop(p, cnt, callback, user) == PCAP_ERROR)
        {
            return score::cpp::make_unexpected(::score::os::Error::createFromErrno());
        }

        return {};
    }

    score::cpp::expected_blank<Error> pcap_breakloop(pcap_t* p) const noexcept override
    {
        if (p == nullptr)
        {
            return score::cpp::make_unexpected(::score::os::Error::createFromErrno(EINVAL));
        }

        ::pcap_breakloop(p);
        return {};
    }

    score::cpp::expected_blank<Error> pcap_close(pcap_t* p) const noexcept override
    {
        if (p == nullptr)
        {
            return score::cpp::make_unexpected(::score::os::Error::createFromErrno(EINVAL));
        }

        ::pcap_close(p);
        return {};
    }

    score::cpp::expected<char*, Error> pcap_geterr(pcap_t* p) const noexcept override
    {
        if (p == nullptr)
        {
            return score::cpp::make_unexpected(::score::os::Error::createFromErrno(EINVAL));
        }

        char* result = ::pcap_geterr(p);

        if (result == nullptr)
        {
            // NOTE_2: from pcap source code (pcap.c), pcap_geterr() returns the internal errbuf[] array.
            // So it will never be nullptr.
            return score::cpp::make_unexpected(
                ::score::os::Error::createFromErrno(EBADMSG));  // LCOV_EXCL_LINE see NOTE_2
        }

        return result;
    }

    score::cpp::expected<int, Error> pcap_compile(pcap_t* p,
                                                  struct bpf_program* fp,
                                                  const char* str,
                                                  int optimize,
                                                  bpf_u_int32 netmask) const noexcept override
    {
        if (p == nullptr)
        {
            return score::cpp::make_unexpected(::score::os::Error::createFromErrno(EINVAL));
        }
        if (fp == nullptr)
        {
            return score::cpp::make_unexpected(::score::os::Error::createFromErrno(EINVAL));
        }
        if (str == nullptr)
        {
            return score::cpp::make_unexpected(::score::os::Error::createFromErrno(EINVAL));
        }
        return ::pcap_compile(p, fp, str, optimize, netmask);
    }
    score::cpp::expected<int, Error> pcap_setfilter(pcap_t* p, struct bpf_program* fp) const noexcept override
    {
        if (p == nullptr)
        {
            return score::cpp::make_unexpected(::score::os::Error::createFromErrno(EINVAL));
        }
        if (fp == nullptr)
        {
            return score::cpp::make_unexpected(::score::os::Error::createFromErrno(EINVAL));
        }
        return ::pcap_setfilter(p, fp);
    }
    score::cpp::expected_blank<Error> pcap_freecode(struct bpf_program* fp) const noexcept override
    {
        if (fp == nullptr)
        {
            return score::cpp::make_unexpected(::score::os::Error::createFromErrno(EINVAL));
        }
        ::pcap_freecode(fp);
        return {};
    }
    score::cpp::expected<pcap_dumper_t*, Error> pcap_dump_open(pcap_t* p, const char* filename) const noexcept override
    {
        pcap_dumper_t* dumper = nullptr;
        if (p == nullptr)
        {
            return score::cpp::make_unexpected(::score::os::Error::createFromErrno(EINVAL));
        }
        if (filename == nullptr)
        {
            return score::cpp::make_unexpected(::score::os::Error::createFromErrno(EINVAL));
        }
        dumper = ::pcap_dump_open(p, filename);

        if (dumper == nullptr)
        {
            return score::cpp::make_unexpected(::score::os::Error::createFromErrno(EBADMSG));
        }
        return dumper;
    }
    score::cpp::expected_blank<Error> pcap_dump(u_char* user,
                                                const struct pcap_pkthdr* h,
                                                const u_char* sp) const noexcept override
    {
        if (user == nullptr)
        {
            return score::cpp::make_unexpected(::score::os::Error::createFromErrno(EINVAL));
        }
        if (h == nullptr)
        {
            return score::cpp::make_unexpected(::score::os::Error::createFromErrno(EINVAL));
        }
        if (sp == nullptr)
        {
            return score::cpp::make_unexpected(::score::os::Error::createFromErrno(EINVAL));
        }
        ::pcap_dump(user, h, sp);
        return {};
    }

    score::cpp::expected_blank<Error> pcap_dump_close(pcap_dumper_t* p) const noexcept override
    {
        if (p == nullptr)
        {
            return score::cpp::make_unexpected(::score::os::Error::createFromErrno(EINVAL));
        }
        ::pcap_dump_close(p);
        return {};
    }
};

}  // namespace
}  // namespace os
}  // namespace score

score::os::Pcap& score::os::Pcap::instance() noexcept
{
    static score::os::PcapImpl instance{};
    return select_instance(instance);
}
