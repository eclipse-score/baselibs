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
#include "score/mw/log/detail/file_recorder/file_recorder_factory.h"
#include "score/mw/log/detail/empty_recorder.h"

#include "score/os/mocklib/fcntl_mock.h"
#include "score/mw/log/detail/file_recorder/file_recorder.h"

#include "gtest/gtest.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

using ::testing::_;
using ::testing::Return;

template <typename ConcreteRecorder>
bool IsRecorderOfType(const std::unique_ptr<Recorder>& recorder) noexcept
{
    static_assert(std::is_base_of<Recorder, ConcreteRecorder>::value,
                  "Concrete recorder shall be derived from Recorder base class");

    return dynamic_cast<const ConcreteRecorder*>(recorder.get()) != nullptr;
}

class FileRecorderFactoryConfigFixture : public ::testing::Test
{
  public:
    void SetUp() override
    {
        memory_resource_ = score::cpp::pmr::get_default_resource();
    }
    void TearDown() override {}

  protected:
    score::cpp::pmr::memory_resource* memory_resource_ = nullptr;
};

TEST_F(FileRecorderFactoryConfigFixture, FileConfigurionShallCallFileCreationAndReturnFileRecorder)
{
    RecordProperty("Requirement", "SCR-861534, SCR-26319707");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "RecorderFactory can create FileRecorder and fileCreation will be called if file is configured. The "
                   "component shall set the FD_CLOEXEC (or O_CLOEXEC) flag on all the file descriptor it owns");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto fcntl_mock = score::cpp::pmr::make_unique<score::os::FcntlMock>(memory_resource_);
    auto fcntl_mock_raw_ptr = fcntl_mock.get();
    std::int32_t file_descriptor = 3;

    const score::os::Fcntl::Open open_flags =
        score::os::Fcntl::Open::kWriteOnly | score::os::Fcntl::Open::kCreate | score::os::Fcntl::Open::kCloseOnExec;
    const score::os::Stat::Mode access_flags = score::os::Stat::Mode::kReadUser | score::os::Stat::Mode::kWriteUser |
                                             score::os::Stat::Mode::kReadGroup | score::os::Stat::Mode::kReadOthers;

    EXPECT_CALL(*fcntl_mock_raw_ptr, open(_, open_flags, access_flags)).Times(1).WillOnce(Return(file_descriptor));

    Configuration config;

    auto recorder = FileRecorderFactory{std::move(fcntl_mock)}.CreateConcreteLogRecorder(config, memory_resource_);

    EXPECT_TRUE(IsRecorderOfType<FileRecorder>(recorder));
}

TEST_F(FileRecorderFactoryConfigFixture, CreateFileLoggingBackendFalied)
{
    auto fcntl_mock = score::cpp::pmr::make_unique<score::os::FcntlMock>(memory_resource_);
    auto fcntl_mock_raw_ptr = fcntl_mock.get();

    EXPECT_CALL(*fcntl_mock_raw_ptr, open(_, _, _))
        .Times(1)
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno(EINVAL))));

    Configuration config;

    auto recorder = FileRecorderFactory{std::move(fcntl_mock)}.CreateConcreteLogRecorder(config, memory_resource_);

    EXPECT_TRUE(IsRecorderOfType<EmptyRecorder>(recorder));
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
