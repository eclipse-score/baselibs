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
#include "score/os/qnx/sigevent_qnx_impl.h"
#include "score/os/sigevent_error.h"

#include <gtest/gtest.h>
#include <sys/siginfo.h>

namespace score::os
{

class SigEventQnxTest : public testing::Test
{
  public:
    void SetUp() override
    {
        signal_event_qnx_ = std::make_unique<SigEventQnxImpl>();
    }

    std::unique_ptr<SigEventQnx> signal_event_qnx_;

    static constexpr std::int32_t kConnectionId = 12;
    static constexpr std::int16_t kPriority = 10;
    static constexpr std::int16_t kCode = 42;
    static constexpr std::intptr_t kSignalEventValue = 1337;
    static constexpr std::int32_t kSignalNumber = 30;
    static constexpr std::int16_t kSignalCode = 50;
    static constexpr std::int16_t kThreadId = 20;
};

TEST_F(SigEventQnxTest, SetUnblock)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventQnxTest set unblock");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    signal_event_qnx_->SetUnblock();

    const auto raw_sigevent = signal_event_qnx_->GetSigevent();
    EXPECT_EQ(raw_sigevent.sigev_notify, SIGEV_UNBLOCK);
}

TEST_F(SigEventQnxTest, SetPulse)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventQnxTest set pulse");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    signal_event_qnx_->SetPulse(kConnectionId, kPriority, kCode, kSignalEventValue);

    const auto& raw_sigevent = signal_event_qnx_->GetSigevent();
    EXPECT_EQ(raw_sigevent.sigev_notify, SIGEV_PULSE);
    EXPECT_EQ(raw_sigevent.sigev_coid, kConnectionId);
    EXPECT_EQ(raw_sigevent.sigev_priority, kPriority);
    EXPECT_EQ(raw_sigevent.sigev_code, kCode);
    EXPECT_EQ(raw_sigevent.sigev_value.sival_int, kSignalEventValue);
}

TEST_F(SigEventQnxTest, SetSignalThread)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventQnxTest set signal thread");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    signal_event_qnx_->SetSignalThread(kSignalNumber, kSignalEventValue, kThreadId);

    auto& raw_sigevent = const_cast<sigevent&>(signal_event_qnx_->GetSigevent());
    EXPECT_EQ(raw_sigevent.sigev_notify, SIGEV_SIGNAL_THREAD);
    EXPECT_EQ(raw_sigevent.sigev_signo, kSignalNumber);
    EXPECT_EQ(raw_sigevent.sigev_value.sival_int, kSignalEventValue);
}

TEST_F(SigEventQnxTest, SetSignalCode)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventQnxTest set signal code");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    signal_event_qnx_->SetSignalCode(kSignalNumber, kSignalEventValue, kSignalCode);

    const auto& raw_sigevent = signal_event_qnx_->GetSigevent();
    EXPECT_EQ(raw_sigevent.sigev_notify, SIGEV_SIGNAL_CODE);
    EXPECT_EQ(raw_sigevent.sigev_signo, kSignalNumber);
    EXPECT_EQ(raw_sigevent.sigev_code, kSignalCode);
    EXPECT_EQ(raw_sigevent.sigev_value.sival_int, kSignalEventValue);
}

TEST_F(SigEventQnxTest, SetMemory)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventQnxTest set memory");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    volatile std::uint32_t dummy_mem = 0;
    constexpr std::int32_t kSize = 4;
    constexpr std::int32_t kOffset = 0;

    signal_event_qnx_->SetMemory(&dummy_mem, kSize, kOffset);

    auto& raw_sigevent = const_cast<sigevent&>(signal_event_qnx_->GetSigevent());
    EXPECT_EQ(raw_sigevent.sigev_notify, SIGEV_MEMORY);
}

TEST_F(SigEventQnxTest, SetInterrupt)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventQnxTest set interrupt");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    signal_event_qnx_->SetInterrupt();

    const auto& raw_sigevent = signal_event_qnx_->GetSigevent();
    EXPECT_EQ(raw_sigevent.sigev_notify, SIGEV_INTR);
}

TEST_F(SigEventQnxTest, SetSignalEventValue)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventQnxTest set signal event value");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    SigEventErrorCodeDomain errorDomain;
    std::variant<int, void*> signal_event_value;
    bool value = false;
    signal_event_value = reinterpret_cast<void*>(&value);

    auto result = signal_event_qnx_->SetNotificationType(SigEvent::NotificationType::kSignal);
    EXPECT_TRUE(result.has_value());
    result = signal_event_qnx_->SetSignalEventValue(signal_event_value);
    EXPECT_TRUE(result.has_value());

    signal_event_value = nullptr;
    result = signal_event_qnx_->SetSignalEventValue(signal_event_value);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), SigEventErrorCode::kInvalidSignalEventValue);
    auto error_msg =
        errorDomain.MessageFor(static_cast<score::result::ErrorCode>(SigEventErrorCode::kInvalidSignalEventValue));
    EXPECT_EQ(result.error().Message(), error_msg);

    signal_event_qnx_->SetUnblock();
    result = signal_event_qnx_->SetSignalEventValue(signal_event_value);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), SigEventErrorCode::kInvalidSignalEventNotificationType);
    error_msg = errorDomain.MessageFor(
        static_cast<score::result::ErrorCode>(SigEventErrorCode::kInvalidSignalEventNotificationType));
    EXPECT_EQ(result.error().Message(), error_msg);
}

TEST_F(SigEventQnxTest, Reset)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventQnxTest reset sigevent");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto result = signal_event_qnx_->SetNotificationType(SigEvent::NotificationType::kThread);
    EXPECT_TRUE(result.has_value());
    pthread_attr_t attributes{};
    result = signal_event_qnx_->SetThreadAttributes(attributes);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(signal_event_qnx_->GetSigevent().sigev_notify, SIGEV_THREAD);

    auto callback = [](sigval) {};
    signal_event_qnx_->SetThreadCallback(callback);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(signal_event_qnx_->GetSigevent().sigev_notify_function, callback);

    signal_event_qnx_->SetSignalNumber(SIGUSR1);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(signal_event_qnx_->GetSigevent().sigev_signo, SIGUSR1);

    signal_event_qnx_->Reset();
    const auto& signal_event = signal_event_qnx_->GetSigevent();
    EXPECT_NE(signal_event.sigev_signo, SIGUSR1);
    EXPECT_NE(signal_event.sigev_notify, SIGEV_THREAD);
    EXPECT_NE(signal_event.sigev_notify_function, callback);
}

TEST_F(SigEventQnxTest, Getter)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventQnxTest getters sigevent");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto&& const_ref = signal_event_qnx_->GetSigevent();

    EXPECT_TRUE((std::is_const_v<std::remove_reference_t<decltype(const_ref)>>));
}

TEST_F(SigEventQnxTest, ModifySigevent)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventQnxTest modify sigevent");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    signal_event_qnx_->ModifySigevent([](sigevent& raw_sigevent) {
        raw_sigevent.sigev_notify = SIGEV_SIGNAL;
        raw_sigevent.sigev_signo = SIGUSR1;
        raw_sigevent.sigev_value.sival_int = kCode;
    });

    const auto& signal_event = signal_event_qnx_->GetSigevent();
    EXPECT_EQ(signal_event.sigev_notify, SIGEV_SIGNAL);
    EXPECT_EQ(signal_event.sigev_signo, SIGUSR1);
    EXPECT_EQ(signal_event.sigev_value.sival_int, kCode);
}

}  // namespace score::os
