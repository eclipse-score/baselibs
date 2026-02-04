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
#include "score/os/sigevent_error.h"
#include "score/os/sigevent_impl.h"

#include <gtest/gtest.h>

namespace score::os
{

class SigEventTest : public testing::Test
{
  public:
    void SetUp() override
    {
        signal_event_ = std::make_unique<SigEventImpl>();
    }

    std::unique_ptr<SigEvent> signal_event_;
};

TEST_F(SigEventTest, SetNotificationType)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventTest set notification types");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto none_notification = signal_event_->SetNotificationType(SigEvent::NotificationType::kNone);
    ASSERT_TRUE(none_notification.has_value());
    EXPECT_EQ(signal_event_->GetSigevent().sigev_notify, SIGEV_NONE);

    auto signal_notification = signal_event_->SetNotificationType(SigEvent::NotificationType::kSignal);
    ASSERT_TRUE(signal_notification.has_value());
    EXPECT_EQ(signal_event_->GetSigevent().sigev_notify, SIGEV_SIGNAL);

    auto thread_notification = signal_event_->SetNotificationType(SigEvent::NotificationType::kThread);
    ASSERT_TRUE(thread_notification.has_value());
    EXPECT_EQ(signal_event_->GetSigevent().sigev_notify, SIGEV_THREAD);

    constexpr auto invalid_arg = 42U;
    auto invalid_notification =
        signal_event_->SetNotificationType(static_cast<SigEvent::NotificationType>(invalid_arg));
    ASSERT_FALSE(invalid_notification.has_value());
    EXPECT_EQ(invalid_notification.error(), SigEventErrorCode::kInvalidNotificationType);
    SigEventErrorCodeDomain errorDomain;
    auto error_msg =
        errorDomain.MessageFor(static_cast<score::result::ErrorCode>(SigEventErrorCode::kInvalidNotificationType));
    EXPECT_EQ(invalid_notification.error().Message(), error_msg);
}

TEST_F(SigEventTest, SetSignalNumber)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventTest set signal number");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    SigEventErrorCodeDomain errorDomain;

    auto result = signal_event_->SetSignalNumber(0);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), SigEventErrorCode::kInvalidSignalNumber);
    auto error_msg =
        errorDomain.MessageFor(static_cast<score::result::ErrorCode>(SigEventErrorCode::kInvalidSignalNumber));
    EXPECT_EQ(result.error().Message(), error_msg);

    result = signal_event_->SetSignalNumber(NSIG);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), SigEventErrorCode::kInvalidSignalNumber);
    error_msg = errorDomain.MessageFor(static_cast<score::result::ErrorCode>(SigEventErrorCode::kInvalidSignalNumber));
    EXPECT_EQ(result.error().Message(), error_msg);

    result = signal_event_->SetSignalNumber(SIGUSR1);
    EXPECT_TRUE(result.has_value());
}

TEST_F(SigEventTest, SetSignalEventValue)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventTest set signal event value");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    SigEventErrorCodeDomain errorDomain;
    std::variant<int, void*> signal_event_value;
    bool value = false;
    signal_event_value = reinterpret_cast<void*>(&value);

    auto result = signal_event_->SetNotificationType(SigEvent::NotificationType::kNone);
    EXPECT_TRUE(result.has_value());
    result = signal_event_->SetSignalEventValue(signal_event_value);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), SigEventErrorCode::kInvalidSignalEventNotificationType);
    auto error_msg = errorDomain.MessageFor(
        static_cast<score::result::ErrorCode>(SigEventErrorCode::kInvalidSignalEventNotificationType));
    EXPECT_EQ(result.error().Message(), error_msg);

    result = signal_event_->SetNotificationType(SigEvent::NotificationType::kSignal);
    EXPECT_TRUE(result.has_value());
    result = signal_event_->SetSignalEventValue(signal_event_value);
    EXPECT_TRUE(result.has_value());

    signal_event_value = nullptr;
    result = signal_event_->SetSignalEventValue(signal_event_value);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), SigEventErrorCode::kInvalidSignalEventValue);
    error_msg =
        errorDomain.MessageFor(static_cast<score::result::ErrorCode>(SigEventErrorCode::kInvalidSignalEventValue));
    EXPECT_EQ(result.error().Message(), error_msg);

    signal_event_value = 42;
    result = signal_event_->SetSignalEventValue(signal_event_value);
    EXPECT_TRUE(result.has_value());
}

TEST_F(SigEventTest, SetThreadCallback)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventTest set thread callback");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    SigEventErrorCodeDomain errorDomain;

    auto result = signal_event_->SetNotificationType(SigEvent::NotificationType::kSignal);
    EXPECT_TRUE(result.has_value());

    result = signal_event_->SetThreadCallback(nullptr);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), SigEventErrorCode::kInvalidThreadCallbackNotificationType);
    auto error_msg = errorDomain.MessageFor(
        static_cast<score::result::ErrorCode>(SigEventErrorCode::kInvalidThreadCallbackNotificationType));
    EXPECT_EQ(result.error().Message(), error_msg);

    result = signal_event_->SetNotificationType(SigEvent::NotificationType::kThread);
    EXPECT_TRUE(result.has_value());

    result = signal_event_->SetThreadCallback(nullptr);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), SigEventErrorCode::kInvalidArgument);
    error_msg = errorDomain.MessageFor(static_cast<score::result::ErrorCode>(SigEventErrorCode::kInvalidArgument));
    EXPECT_EQ(result.error().Message(), error_msg);

    auto callback = [](sigval) {};
    result = signal_event_->SetThreadCallback(callback);
    EXPECT_TRUE(result.has_value());
}

TEST_F(SigEventTest, SetThreadAttributes)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventTest set thread attributes");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    SigEventErrorCodeDomain errorDomain;

    auto result = signal_event_->SetNotificationType(SigEvent::NotificationType::kSignal);
    EXPECT_TRUE(result.has_value());

    pthread_attr_t attributes{};
    result = signal_event_->SetThreadAttributes(attributes);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), SigEventErrorCode::kInvalidThreadAttributesNotificationType);
    auto error_msg = errorDomain.MessageFor(
        static_cast<score::result::ErrorCode>(SigEventErrorCode::kInvalidThreadAttributesNotificationType));
    EXPECT_EQ(result.error().Message(), error_msg);

    result = signal_event_->SetNotificationType(SigEvent::NotificationType::kThread);
    EXPECT_TRUE(result.has_value());
    result = signal_event_->SetThreadAttributes(attributes);
    EXPECT_TRUE(result.has_value());
}

TEST_F(SigEventTest, Reset)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventTest reset sigevent");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto result = signal_event_->SetNotificationType(SigEvent::NotificationType::kThread);
    EXPECT_TRUE(result.has_value());
    pthread_attr_t attributes{};
    result = signal_event_->SetThreadAttributes(attributes);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(signal_event_->GetSigevent().sigev_notify, SIGEV_THREAD);

    auto callback = [](sigval) {};
    signal_event_->SetThreadCallback(callback);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(signal_event_->GetSigevent().sigev_notify_function, callback);

    signal_event_->SetSignalNumber(SIGUSR1);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(signal_event_->GetSigevent().sigev_signo, SIGUSR1);

    signal_event_->Reset();
    const auto& signal_event = signal_event_->GetSigevent();
    EXPECT_NE(signal_event.sigev_signo, SIGUSR1);
    EXPECT_NE(signal_event.sigev_notify, SIGEV_THREAD);
    EXPECT_NE(signal_event.sigev_notify_function, callback);
}

TEST_F(SigEventTest, ModifySigevent)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventTest modify sigevent positive and negative scenarios");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr std::int32_t kTestSignalValue = 42;

    signal_event_->ModifySigevent([](sigevent& raw_sigevent) {
        raw_sigevent.sigev_notify = SIGEV_SIGNAL;
        raw_sigevent.sigev_signo = SIGUSR1;
        raw_sigevent.sigev_value.sival_int = kTestSignalValue;
    });

    const auto& signal_event = signal_event_->GetSigevent();
    EXPECT_EQ(signal_event.sigev_notify, SIGEV_SIGNAL);
    EXPECT_EQ(signal_event.sigev_signo, SIGUSR1);
    EXPECT_EQ(signal_event.sigev_value.sival_int, kTestSignalValue);
}

TEST_F(SigEventTest, Getter)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventTest getter sigevent");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    auto&& const_ref = signal_event_->GetSigevent();

    EXPECT_TRUE((std::is_const_v<std::remove_reference_t<decltype(const_ref)>>));
}

TEST_F(SigEventTest, DefaultError)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SigEventTest default error sigevent");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    SigEventErrorCodeDomain errorDomain;
    auto error_msg = errorDomain.MessageFor(9999);
    EXPECT_EQ(error_msg, "Unknown error");
}
}  // namespace score::os
