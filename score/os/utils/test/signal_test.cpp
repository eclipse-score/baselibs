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
#include "score/os/utils/signal_impl.h"

#include <gtest/gtest.h>
#include <unistd.h>
#include <string.h>
#include <csignal>
#include <future>
#include <thread>

namespace score
{
namespace os
{
namespace
{

using ::testing::Test;

class SignalTest : public ::testing::Test
{
  protected:
    void TearDown() override
    {
        // Reset signal handler to default
        struct sigaction dummy;
        auto val = unit_->SigAction(SIGTERM, old_sigaction_, dummy);
        EXPECT_TRUE(val.has_value());
        triggered_ = false;
    }

    void SetUp() override
    {
        sigset_t sig_set{};
        auto val = unit_->SigEmptySet(sig_set);
        EXPECT_TRUE(val.has_value());
        // Need to fully initialize otherwise memchecker complains
        sig_handler_.sa_flags = 0;
        sig_handler_.sa_mask = sig_set;
        sig_handler_.sa_handler = SIG_DFL;
        old_sigaction_.sa_flags = 0;
        old_sigaction_.sa_mask = sig_set;
        old_sigaction_.sa_handler = SIG_DFL;
    }

    void BlockSignal(int32_t signo)
    {
        sigset_t sigset{};
        auto val = unit_->SigAddSet(sigset, signo);
        EXPECT_TRUE(val.has_value());
        EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
        val = unit_->PthreadSigMask(SIG_BLOCK, sigset);
        EXPECT_TRUE(val.has_value());
        EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
    }

    void UnblockSignal(int32_t signo)
    {
        sigset_t sigset{};
        auto val = unit_->SigAddSet(sigset, signo);
        EXPECT_TRUE(val.has_value());
        EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
        val = unit_->PthreadSigMask(SIG_UNBLOCK, sigset);
        EXPECT_TRUE(val.has_value());
        EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
    }

    std::unique_ptr<Signal> unit_{std::make_unique<SignalImpl>()};

    static bool triggered_;
    struct sigaction sig_handler_;
    struct sigaction old_sigaction_;

    void MakeSignalTriggerBool(const int signal)
    {
        sig_handler_.sa_flags = SA_SIGINFO;
        sig_handler_.sa_handler = [](int) {
            triggered_ = true;
        };
        auto val = unit_->SigAction(signal, sig_handler_, old_sigaction_);
        EXPECT_TRUE(val.has_value());
    }
};

bool SignalTest::triggered_ = false;

TEST_F(SignalTest, IsNotAMemberWorks)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SignalTest is Not AMember Works");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    sigset_t sigset{};
    const auto val = unit_->SigIsMember(sigset, SIGUSR1);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
}

TEST_F(SignalTest, CheckIfSigSetIsEmptyWorks)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SignalTest Check If Sig Set Is Empty Works");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    sigset_t sigset{};
    auto val = unit_->SigAddSet(sigset, SIGUSR1);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);

    val = unit_->SigEmptySet(sigset);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);

    val = unit_->SigIsMember(sigset, SIGUSR1);
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
}

TEST_F(SignalTest, GetCurrentBlockedSignals)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SignalTest Get Current Blocked Signals");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    BlockSignal(SIGUSR1);

    sigset_t sigset{};
    auto val = unit_->GetCurrentBlockedSignals(sigset);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
    EXPECT_EQ(::sigismember(&sigset, SIGUSR1), 1) << "Error: " << strerror(errno);

    UnblockSignal(SIGUSR1);
}

TEST_F(SignalTest, IsSignalBlocked)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SignalTest Is Signal Blocked");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    BlockSignal(SIGUSR1);

    auto val = unit_->IsSignalBlocked(SIGUSR1);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 1) << "Error: " << strerror(errno);

    UnblockSignal(SIGUSR1);
}

TEST_F(SignalTest, PthreadSigMask)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SignalTest Pthread Sig Mask");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    MakeSignalTriggerBool(SIGUSR1);

    sigset_t sigset{};
    auto val = unit_->SigAddSet(sigset, SIGUSR1);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);

    val = unit_->PthreadSigMask(sigset);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
    raise(SIGUSR1);
    // As mask is set. triggered should not be updated
    EXPECT_FALSE(triggered_);

    // cleanup
    val = unit_->SigEmptySet(sigset);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);

    val = unit_->PthreadSigMask(sigset);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
}

TEST_F(SignalTest, PthreadSigMaskReturnsOldSet)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SignalTest Pthread Sig Mask Returns Old Set");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    MakeSignalTriggerBool(SIGUSR1);

    sigset_t sigset{};
    auto val = unit_->SigAddSet(sigset, SIGUSR1);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);

    val = unit_->PthreadSigMask(SIG_BLOCK, sigset);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
    raise(SIGUSR1);
    // As mask is set. triggered should not be updated
    EXPECT_FALSE(triggered_);

    // Unblock signal
    sigset_t oldsigset{};
    val = unit_->PthreadSigMask(SIG_UNBLOCK, sigset, oldsigset);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);

    // Check that the oldsigset contains the previously blocked signals
    val = unit_->SigIsMember(oldsigset, SIGUSR1);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 1) << "Error: " << strerror(errno);
}

TEST_F(SignalTest, SendSelfSigTerm)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SignalTest Send Self Sig Term");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    MakeSignalTriggerBool(SIGTERM);

    unit_->SendSelfSigterm();
    EXPECT_TRUE(triggered_);
}

TEST_F(SignalTest, SigAction)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SignalTest Sig Action");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    static bool triggered = false;
    struct sigaction sig_handler;
    struct sigaction old_sig_handler;
    memset(static_cast<void*>(&sig_handler), 0, sizeof(sig_handler));
    memset(static_cast<void*>(&old_sig_handler), 0, sizeof(old_sig_handler));

    sig_handler.sa_flags = SA_SIGINFO;
    sig_handler.sa_handler = [](int) {
        triggered = true;
    };

    auto val = unit_->SigAction(SIGUSR1, sig_handler, old_sig_handler);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
    raise(SIGUSR1);
    EXPECT_TRUE(triggered);

    // cleanup
    val = unit_->SigAction(SIGUSR1, old_sig_handler, sig_handler);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
}

TEST_F(SignalTest, Kill)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SignalTest Kill");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    MakeSignalTriggerBool(SIGTERM);

    unit_->Kill(getpid(), SIGTERM);
    EXPECT_TRUE(triggered_);
}

TEST_F(SignalTest, SigFillSetWorks)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SignalTest Sig Fill Set Works");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    sigset_t sigset{};
    auto val = unit_->SigFillSet(sigset);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 0) << "Error: " << strerror(errno);
    val = unit_->SigIsMember(sigset, SIGUSR1);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 1) << "Error: " << strerror(errno);
}

TEST_F(SignalTest, AddTerminationSignalWorks)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SignalTest Add Termination Signal Works");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    sigset_t sigset{};
    auto val = unit_->AddTerminationSignal(sigset);
    val = unit_->SigIsMember(sigset, SIGTERM);
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(val.value(), 1) << "Error: " << strerror(errno);
}

}  // namespace
}  // namespace os
}  // namespace score
