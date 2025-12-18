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
#include <gtest/gtest.h>
#include <sys/types.h>

extern "C" {
#include <pcap/pcap.h>
}

namespace score::os
{
namespace
{
constexpr int kSnapLenSmall{262144};
constexpr int kSnapLenTooLarge{128 * 1024 * 1024};
constexpr int kPacketBufferDelay{100};

void pcap_handler_func(u_char* user, const struct pcap_pkthdr* pkthdr, const u_char* packet)
{
    // do nothing
    (void)user;
    (void)pkthdr;
    (void)packet;
}

struct PcapFixture : ::testing::Test
{
    void SetUp() override
    {
        pcap_ = &Pcap::instance();
        pcap_buf_.resize(256);
    }

    void TearDown() override
    {
        if (good_pcap_ != nullptr)
        {
            pcap_->pcap_close(good_pcap_);
            good_pcap_ = nullptr;
        }
    }

    void GetMeADevice()
    {
        pcap_if_t* adddevsp{nullptr};
        int result = ::pcap_findalldevs(&adddevsp, errbuf_.data());
        ASSERT_EQ(result, 0);
        ASSERT_NE(adddevsp, nullptr);
        std::string device_name{adddevsp->name};
        pcap_freealldevs(adddevsp);

        auto open_result = pcap_->pcap_open_live(device_name.data(), kSnapLenSmall, 1, 100, errbuf_.data());
        ASSERT_TRUE(open_result.has_value())
            << "Failed to call pcap_open_live, error: " << open_result.error().ToString();
        good_pcap_ = open_result.value();
    }

    Pcap* pcap_;
    std::array<char, PCAP_ERRBUF_SIZE> errbuf_{};
    std::vector<char> pcap_buf_{};
    pcap_t* good_pcap_{nullptr};
};

TEST_F(PcapFixture, PcapOpenLiveSuccess)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Open Live Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
}

TEST_F(PcapFixture, PcapOpenLiveFailure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Open Live Failure");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto result = pcap_->pcap_open_live("invalid_device", kSnapLenTooLarge, 1, kPacketBufferDelay, errbuf_.data());
    EXPECT_FALSE(result.has_value());
}

TEST_F(PcapFixture, PcapOpenDeadSuccess)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Open Dead Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto result = pcap_->pcap_open_dead(DLT_EN10MB, kSnapLenSmall);
    ASSERT_TRUE(result.has_value());
    pcap_->pcap_close(result.value());
}

TEST_F(PcapFixture, PcapLoopSuccess)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Loop Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
    auto result = pcap_->pcap_loop(good_pcap_, 1, &pcap_handler_func, nullptr);
    EXPECT_TRUE(result.has_value());
}

TEST_F(PcapFixture, PcapBreakloopSuccess)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Breakloop Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
    auto result = pcap_->pcap_breakloop(good_pcap_);
    EXPECT_TRUE(result.has_value());
}

TEST_F(PcapFixture, PcapCloseSuccess)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Close Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
    auto result = pcap_->pcap_close(good_pcap_);
    EXPECT_TRUE(result.has_value());
    good_pcap_ = nullptr;
}

TEST_F(PcapFixture, PcapGeterrSuccess)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Geterr Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
    auto result = pcap_->pcap_geterr(good_pcap_);
    EXPECT_TRUE(result.has_value());
}

TEST_F(PcapFixture, PcapBreakloopFailure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Breakloop Failure");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto result = pcap_->pcap_breakloop(nullptr);
    EXPECT_FALSE(result.has_value());
}

TEST_F(PcapFixture, PcapCloseFailure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Close Failure");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto result = pcap_->pcap_close(nullptr);
    EXPECT_FALSE(result.has_value());
}

TEST_F(PcapFixture, PcapGeterrWithNullPcap)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Geterr With Null Pcap");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto result = pcap_->pcap_geterr(nullptr);
    EXPECT_FALSE(result.has_value());
}

TEST_F(PcapFixture, PcapLoopFailure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Loop Failure");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
    pcap_->pcap_close(good_pcap_);
    auto result = pcap_->pcap_loop(good_pcap_, 0, &pcap_handler_func, nullptr);
    EXPECT_FALSE(result.has_value());
    good_pcap_ = nullptr;
}

TEST_F(PcapFixture, PcapCompileSuccess)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Compile Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
    struct bpf_program fp;
    bpf_u_int32 net = 0;
    std::string filter_{"tcp"};
    auto result = pcap_->pcap_compile(good_pcap_, &fp, filter_.c_str(), 0, net);
    EXPECT_TRUE(result.has_value());
}

TEST_F(PcapFixture, PcapCompileFailure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Compile Failure");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
    struct bpf_program fp;
    bpf_u_int32 net = 0;
    std::string filter_{"tcp"};
    auto result = pcap_->pcap_compile(nullptr, &fp, filter_.c_str(), 0, net);
    EXPECT_FALSE(result.has_value());
}

TEST_F(PcapFixture, PcapCompileFailure2)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Compile Failure");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
    bpf_u_int32 net = 0;
    std::string filter_{"tcp"};
    auto result = pcap_->pcap_compile(good_pcap_, nullptr, filter_.c_str(), 0, net);
    EXPECT_FALSE(result.has_value());
}

TEST_F(PcapFixture, PcapCompileFailure3)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Compile Failure");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
    struct bpf_program fp;
    bpf_u_int32 net = 0;
    auto result = pcap_->pcap_compile(good_pcap_, &fp, nullptr, 0, net);
    EXPECT_FALSE(result.has_value());
}

TEST_F(PcapFixture, PcapSetFilterSuccess)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Set Filter Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
    struct bpf_program fp;
    bpf_u_int32 net = 0;
    std::string filter_{"tcp"};
    pcap_->pcap_compile(good_pcap_, &fp, filter_.c_str(), 0, net);

    auto result = pcap_->pcap_setfilter(good_pcap_, &fp);
    EXPECT_TRUE(result.has_value());
}

TEST_F(PcapFixture, PcapSetFilterFailure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Set Filter Failure");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
    struct bpf_program fp;
    auto result = pcap_->pcap_setfilter(nullptr, &fp);
    EXPECT_FALSE(result.has_value());
}

TEST_F(PcapFixture, PcapSetFilterFailure2)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Set Filter Failure");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
    auto result = pcap_->pcap_setfilter(good_pcap_, nullptr);
    EXPECT_FALSE(result.has_value());
}

TEST_F(PcapFixture, PcapFreeCodeSuccess)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Free Code Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
    struct bpf_program fp;
    bpf_u_int32 net = 0;
    std::string filter_{"tcp"};
    pcap_->pcap_compile(good_pcap_, &fp, filter_.c_str(), 0, net);
    pcap_->pcap_setfilter(good_pcap_, &fp);

    auto result = pcap_->pcap_freecode(&fp);
    EXPECT_TRUE(result.has_value());
}

TEST_F(PcapFixture, PcapFreeCodeFailure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Free Code Failure");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
    auto result = pcap_->pcap_freecode(nullptr);
    EXPECT_FALSE(result.has_value());
}

TEST_F(PcapFixture, PcapDumpOpenSuccess)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Dump Open Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
    std::string fname{"/tmp/vlan73.pcap"};
    auto result = pcap_->pcap_dump_open(good_pcap_, fname.c_str());
    EXPECT_TRUE(result.has_value());
}

TEST_F(PcapFixture, PcapDumpOpenFailure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Dump Open Failure");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
    std::string fname{"/tmp/vlan73.pcap"};
    auto result = pcap_->pcap_dump_open(nullptr, fname.c_str());
    EXPECT_FALSE(result.has_value());
}

TEST_F(PcapFixture, PcapDumpOpenFailure2)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Dump Open Failure");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
    auto result = pcap_->pcap_dump_open(good_pcap_, nullptr);
    EXPECT_FALSE(result.has_value());
}

TEST_F(PcapFixture, PcapDumpOpenFailure3)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Dump Open Failure");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
    std::string fname{"root/vlan73.pcap"};
    auto result = pcap_->pcap_dump_open(good_pcap_, fname.c_str());
    EXPECT_FALSE(result.has_value());
}

TEST_F(PcapFixture, PcapDumpSuccess)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Dump Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
    std::string fname{"/tmp/vlan73.pcap"};
    auto result = pcap_->pcap_dump_open(good_pcap_, fname.c_str());
    EXPECT_TRUE(result.has_value());
    pcap_dumper_t* dumper;
    dumper = result.value();
    pcap_pkthdr dummyHdr = {};
    dummyHdr.ts.tv_sec = 1618033988;  // arbitrary timestamp (epoch seconds)
    dummyHdr.ts.tv_usec = 123456;     // microseconds
    dummyHdr.caplen = 72;             // captured length
    dummyHdr.len = 128;               // actual packet length

    u_char dummyPktData[72] = {
        0x11, 0x22, 0x33, 0x44, 0x55,  // pfloghdr
        0x11, 0x22, 0x33, 0x55, 0x55, 0x11, 0x22, 0x33, 0x32, 0x55, 0x11, 0x22, 0x33, 0x33, 0x55, 0x11, 0x22,
        0x33, 0x34, 0x55, 0x11, 0x22, 0x33, 0x35, 0x55, 0x11, 0x22, 0x33, 0x36, 0x55, 0x11, 0x22, 0x33, 0x37,
        0x55, 0x11, 0x22, 0x33, 0x38, 0x55, 0x11, 0x22, 0x33, 0x44, 0x55, 0x11, 0x22, 0x36, 0x44, 0x55,         // iph
        0x11, 0x22, 0x06, 0x44, 0x55, 0x11, 0x22, 0x36, 0x44, 0x55, 0x11, 0x22, 0x36, 0x86, 0xC4, 0x86, 0xC5};  // ports
    auto result1 = pcap_->pcap_dump((u_char*)dumper, &dummyHdr, dummyPktData);
    EXPECT_TRUE(result1.has_value());
}

TEST_F(PcapFixture, PcapDumpFailure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Dump Failure");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
    pcap_pkthdr dummyHdr = {};
    dummyHdr.ts.tv_sec = 1618033988;  // arbitrary timestamp (epoch seconds)
    dummyHdr.ts.tv_usec = 123456;     // microseconds
    dummyHdr.caplen = 72;             // captured length
    dummyHdr.len = 128;               // actual packet length

    u_char dummyPktData[72] = {
        0x11, 0x22, 0x33, 0x44, 0x55,  // pfloghdr
        0x11, 0x22, 0x33, 0x55, 0x55, 0x11, 0x22, 0x33, 0x32, 0x55, 0x11, 0x22, 0x33, 0x33, 0x55, 0x11, 0x22,
        0x33, 0x34, 0x55, 0x11, 0x22, 0x33, 0x35, 0x55, 0x11, 0x22, 0x33, 0x36, 0x55, 0x11, 0x22, 0x33, 0x37,
        0x55, 0x11, 0x22, 0x33, 0x38, 0x55, 0x11, 0x22, 0x33, 0x44, 0x55, 0x11, 0x22, 0x36, 0x44, 0x55,         // iph
        0x11, 0x22, 0x06, 0x44, 0x55, 0x11, 0x22, 0x36, 0x44, 0x55, 0x11, 0x22, 0x36, 0x86, 0xC4, 0x86, 0xC5};  // ports
    auto result = pcap_->pcap_dump(nullptr, &dummyHdr, dummyPktData);
    EXPECT_FALSE(result.has_value());
}

TEST_F(PcapFixture, PcapDumpFailure2)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Dump Failure");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
    u_char user[5] = {0x11, 0x22, 0x33, 0x44, 0x55};

    u_char dummyPktData[72] = {
        0x11, 0x22, 0x33, 0x44, 0x55,  // pfloghdr
        0x11, 0x22, 0x33, 0x55, 0x55, 0x11, 0x22, 0x33, 0x32, 0x55, 0x11, 0x22, 0x33, 0x33, 0x55, 0x11, 0x22,
        0x33, 0x34, 0x55, 0x11, 0x22, 0x33, 0x35, 0x55, 0x11, 0x22, 0x33, 0x36, 0x55, 0x11, 0x22, 0x33, 0x37,
        0x55, 0x11, 0x22, 0x33, 0x38, 0x55, 0x11, 0x22, 0x33, 0x44, 0x55, 0x11, 0x22, 0x36, 0x44, 0x55,         // iph
        0x11, 0x22, 0x06, 0x44, 0x55, 0x11, 0x22, 0x36, 0x44, 0x55, 0x11, 0x22, 0x36, 0x86, 0xC4, 0x86, 0xC5};  // ports
    auto result = pcap_->pcap_dump(user, nullptr, dummyPktData);
    EXPECT_FALSE(result.has_value());
}

TEST_F(PcapFixture, PcapDumpFailure3)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Dump Failure");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
    u_char user[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
    pcap_pkthdr dummyHdr = {};
    dummyHdr.ts.tv_sec = 1618033988;  // arbitrary timestamp (epoch seconds)
    dummyHdr.ts.tv_usec = 123456;     // microseconds
    dummyHdr.caplen = 72;             // captured length
    dummyHdr.len = 128;               // actual packet length

    auto result = pcap_->pcap_dump(user, &dummyHdr, nullptr);
    EXPECT_FALSE(result.has_value());
}

TEST_F(PcapFixture, PcapDumpCloseSuccess)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Dump Close Success");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
    std::string fname{"/tmp/vlan73.pcap"};
    auto dumper = pcap_->pcap_dump_open(good_pcap_, fname.c_str());
    auto result = pcap_->pcap_dump_close(dumper.value());
    EXPECT_TRUE(result.has_value());
}

TEST_F(PcapFixture, PcapDumpCloseFailure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Pcap Dump Close Failure");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    GetMeADevice();
    auto result = pcap_->pcap_dump_close(nullptr);
    EXPECT_FALSE(result.has_value());
}

}  // namespace
}  // namespace score::os
