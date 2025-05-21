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
#include "score/os/utils/abortable_blocking_reader.h"

#include "score/os/fcntl.h"
#include "score/os/fcntl_impl.h"
#include "score/os/mocklib/fcntl_mock.h"
#include "score/os/mocklib/sys_poll_mock.h"
#include "score/os/mocklib/unistdmock.h"
#include "score/os/sys_poll.h"
#include "score/os/sys_poll_impl.h"
#include "score/os/unistd.h"

#include <thread>
#include <score/expected.hpp>
#include <score/utility.hpp>

#include <include/gtest/gtest.h>

#include <unistd.h>

#include <array>
#include <cerrno>
#include <future>

namespace score
{
namespace os
{
namespace
{

class NonBlockingFileDescriptorTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        ON_CALL(fcntl_mock_, fcntl(::testing::_, Fcntl::Command::kFileGetStatusFlags))
            .WillByDefault(::testing::Return(existing_flags_));
        ON_CALL(fcntl_mock_, fcntl(::testing::_, ::testing::_, ::testing::_))
            .WillByDefault(::testing::Return(score::cpp::expected_blank<Error>{}));

        unistd_mock_ = std::make_unique<::testing::NiceMock<UnistdMock>>();
        ON_CALL(*unistd_mock_, close(testing::_)).WillByDefault([](auto fd) {
            return internal::UnistdImpl{}.close(fd);
        });

        auto expected_file_descriptor = FcntlImpl{}.open(filepath_, open_flags_, mode_);
        ASSERT_TRUE(expected_file_descriptor.has_value());
        file_descriptor_ = expected_file_descriptor.value();
    }

    void TearDown() override
    {
        score::cpp::ignore = internal::UnistdImpl{}.unlink(filepath_);
    }

    ::testing::NiceMock<FcntlMock> fcntl_mock_;
    std::shared_ptr<::testing::NiceMock<UnistdMock>> unistd_mock_;

    Fcntl::Open existing_flags_{Fcntl::Open::kExclusive};
    Fcntl::Open open_flags_{Fcntl::Open::kCreate | Fcntl::Open::kReadWrite};
    Stat::Mode mode_{Stat::Mode::kReadUser | Stat::Mode::kWriteUser};
    constexpr static auto filepath_{"/tmp/non_blocking_file_descriptor_test"};
    std::int32_t file_descriptor_{-1};
};

TEST_F(NonBlockingFileDescriptorTest, DefaultConstructionSetsUnderlyingFileDescriptorToInvalidValue)
{
    NonBlockingFileDescriptor non_blocking_file_descriptor{};
    EXPECT_EQ(non_blocking_file_descriptor.GetUnderlying(), -1);
}

TEST_F(NonBlockingFileDescriptorTest, ConstructionViaFactoryWhenNonBlockingFlagIsPresentInFileDescriptorFlags)
{
    const auto expected_flags{existing_flags_ | Fcntl::Open::kNonBlocking};
    EXPECT_CALL(fcntl_mock_, fcntl(file_descriptor_, Fcntl::Command::kFileGetStatusFlags))
        .WillOnce(::testing::Return(expected_flags));

    auto non_blocking_file_descriptor = NonBlockingFileDescriptor::Make(file_descriptor_, fcntl_mock_, unistd_mock_);
    ASSERT_TRUE(non_blocking_file_descriptor.has_value());
    EXPECT_EQ(non_blocking_file_descriptor.value().GetUnderlying(), file_descriptor_);
}

TEST_F(NonBlockingFileDescriptorTest, ConstructionViaFactoryAddsNonBlockingFlagToFileDescriptorFlags)
{
    ::testing::InSequence sequence{};

    const auto existing_flags{Fcntl::Open::kExclusive};
    EXPECT_CALL(fcntl_mock_, fcntl(file_descriptor_, Fcntl::Command::kFileGetStatusFlags));

    const auto expected_flags{existing_flags | Fcntl::Open::kNonBlocking};
    EXPECT_CALL(fcntl_mock_, fcntl(file_descriptor_, Fcntl::Command::kFileSetStatusFlags, expected_flags));

    auto non_blocking_file_descriptor = NonBlockingFileDescriptor::Make(file_descriptor_, fcntl_mock_, unistd_mock_);
    ASSERT_TRUE(non_blocking_file_descriptor.has_value());
    EXPECT_EQ(non_blocking_file_descriptor.value().GetUnderlying(), file_descriptor_);
}

TEST_F(NonBlockingFileDescriptorTest, ConstructionViaFactoryFailsIfCannotGetFlagsOfFileDescriptor)
{
    ::testing::InSequence sequence{};

    ON_CALL(fcntl_mock_, fcntl(file_descriptor_, Fcntl::Command::kFileGetStatusFlags))
        .WillByDefault(::testing::Return(score::cpp::make_unexpected(Error::createFromErrno(EBADF))));

    EXPECT_CALL(fcntl_mock_, fcntl(file_descriptor_, Fcntl::Command::kFileSetStatusFlags, ::testing::_)).Times(0);

    auto non_blocking_file_descriptor = NonBlockingFileDescriptor::Make(file_descriptor_, fcntl_mock_, unistd_mock_);
    ASSERT_FALSE(non_blocking_file_descriptor.has_value());
    EXPECT_EQ(non_blocking_file_descriptor.error(), Error::Code::kBadFileDescriptor);
}

TEST_F(NonBlockingFileDescriptorTest, ConstructionViaFactoryFailsIfCannotSetFlagsOfFileDescriptor)
{
    const auto expected_flags{existing_flags_ | Fcntl::Open::kNonBlocking};
    ON_CALL(fcntl_mock_, fcntl(file_descriptor_, Fcntl::Command::kFileSetStatusFlags, expected_flags))
        .WillByDefault(::testing::Return(score::cpp::make_unexpected(Error::createFromErrno(EINVAL))));

    auto non_blocking_file_descriptor = NonBlockingFileDescriptor::Make(file_descriptor_, fcntl_mock_, unistd_mock_);
    ASSERT_FALSE(non_blocking_file_descriptor.has_value());
    EXPECT_EQ(non_blocking_file_descriptor.error(), Error::Code::kInvalidArgument);
}

TEST_F(NonBlockingFileDescriptorTest, DestructionClosesUnderlyingFileDescriptor)
{

    auto non_blocking_file_descriptor = NonBlockingFileDescriptor::Make(file_descriptor_, fcntl_mock_, unistd_mock_);
    ASSERT_TRUE(non_blocking_file_descriptor.has_value());
    EXPECT_CALL(*unistd_mock_, close(file_descriptor_));
}

TEST_F(NonBlockingFileDescriptorTest, DestructionTerminatesIfItFailsToCloseUnderlyingFileDescriptor)
{
    ASSERT_DEATH(
        {
            auto non_blocking_file_descriptor =
                NonBlockingFileDescriptor::Make(file_descriptor_, fcntl_mock_, unistd_mock_);
            ASSERT_TRUE(non_blocking_file_descriptor.has_value());
            EXPECT_CALL(*unistd_mock_, close(file_descriptor_))
                .WillOnce(::testing::Return(score::cpp::make_unexpected(Error::createFromErrno(EPERM))));
        },
        ".*");
}

TEST_F(NonBlockingFileDescriptorTest, MoveAssignmentTerminatesIfItFailsToCloseUnderlyingFileDescriptor)
{
    EXPECT_DEATH(
        [this] {
            EXPECT_CALL(*unistd_mock_, close(file_descriptor_))
                .WillOnce(::testing::Return(score::cpp::make_unexpected(Error::createFromErrno(EPERM))));
            auto non_blocking_file_descriptor =
                NonBlockingFileDescriptor::Make(file_descriptor_, fcntl_mock_, unistd_mock_);
            ASSERT_TRUE(non_blocking_file_descriptor.has_value());

            NonBlockingFileDescriptor non_blocking_file_descriptor_new{};
            non_blocking_file_descriptor_new = std::move(non_blocking_file_descriptor.value());
        }(),
        ".*");
}

TEST_F(NonBlockingFileDescriptorTest, DestructionDoesNotTryToCloseInvalidUnderlyingFileDescriptor)
{
    const auto invalid_file_descriptor{-1};
    auto non_blocking_file_descriptor =
        NonBlockingFileDescriptor::Make(invalid_file_descriptor, fcntl_mock_, unistd_mock_);
    ASSERT_TRUE(non_blocking_file_descriptor.has_value());

    EXPECT_CALL(*unistd_mock_, close(::testing::_)).Times(0);
}

TEST_F(NonBlockingFileDescriptorTest, MoveConstructedFromInstanceDoesNotCloseMovedUnderlyingFileDescriptor)
{
    auto non_blocking_file_descriptor = NonBlockingFileDescriptor::Make(file_descriptor_, fcntl_mock_, unistd_mock_);
    ASSERT_TRUE(non_blocking_file_descriptor.has_value());
    auto moved_to{std::move(non_blocking_file_descriptor)};
    score::cpp::ignore = moved_to;

    // Expect only one invocation for moved_to
    EXPECT_CALL(*unistd_mock_, close(file_descriptor_)).Times(1);
}

TEST_F(NonBlockingFileDescriptorTest, MoveAssignedFromInstanceDoesNotCloseMovedUnderylingFileDescriptor)
{
    constexpr auto other_filepath{"/tmp/non_blocking_file_descriptor_test_other"};
    auto expected_other_file_descriptor = FcntlImpl{}.open(other_filepath, open_flags_, mode_);
    ASSERT_TRUE(expected_other_file_descriptor.has_value());
    const auto other_file_descriptor = expected_other_file_descriptor.value();

    auto non_blocking_file_descriptor = NonBlockingFileDescriptor::Make(file_descriptor_, fcntl_mock_, unistd_mock_);
    ASSERT_TRUE(non_blocking_file_descriptor.has_value());
    auto other_non_blocking_file_descriptor =
        NonBlockingFileDescriptor::Make(other_file_descriptor, fcntl_mock_, unistd_mock_);
    ASSERT_TRUE(other_non_blocking_file_descriptor.has_value());

    EXPECT_CALL(*unistd_mock_, close(file_descriptor_)).Times(1);
    EXPECT_CALL(*unistd_mock_, close(other_file_descriptor)).Times(0);

    non_blocking_file_descriptor = std::move(other_non_blocking_file_descriptor);

    EXPECT_CALL(*unistd_mock_, close(other_file_descriptor)).Times(1);
    EXPECT_EQ(non_blocking_file_descriptor.value(), other_file_descriptor);

    internal::UnistdImpl{}.unlink(other_filepath);
}

TEST_F(NonBlockingFileDescriptorTest, GetUnderlyingReturnsUnderlyingFileDescriptor)
{
    auto non_blocking_file_descriptor = NonBlockingFileDescriptor::Make(file_descriptor_, fcntl_mock_, unistd_mock_);
    ASSERT_TRUE(non_blocking_file_descriptor.has_value());
    EXPECT_EQ(non_blocking_file_descriptor.value().GetUnderlying(), file_descriptor_);
}

TEST_F(NonBlockingFileDescriptorTest, CanImplicitlyCastToInt32T)
{
    auto non_blocking_file_descriptor = NonBlockingFileDescriptor::Make(file_descriptor_, fcntl_mock_, unistd_mock_);
    ASSERT_TRUE(non_blocking_file_descriptor.has_value());
    const std::int32_t actual_non_blocking_file_descriptor{non_blocking_file_descriptor.value()};
    EXPECT_EQ(actual_non_blocking_file_descriptor, file_descriptor_);
}

class AbortableBlockingReaderTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        // To simultaneously introspect and execute the actual production code, we forward all mocked calls to the
        // implementation.
        fcntl_mock_ = std::make_shared<::testing::NiceMock<FcntlMock>>();
        ON_CALL(*fcntl_mock_, fcntl(::testing::_, ::testing::_)).WillByDefault([](auto fd, auto command) {
            return FcntlImpl{}.fcntl(fd, command);
        });
        ON_CALL(*fcntl_mock_, fcntl(::testing::_, ::testing::_, ::testing::_))
            .WillByDefault([](auto fd, auto command, auto flags) {
                return FcntlImpl{}.fcntl(fd, command, flags);
            });

        syspoll_mock_ = std::make_shared<::testing::NiceMock<SysPollMock>>();
        ON_CALL(*syspoll_mock_, poll(testing::_, testing::_, testing::_)).WillByDefault(::testing::Invoke(poll_));

        unistd_mock_ = std::make_shared<::testing::NiceMock<UnistdMock>>();
        ON_CALL(*unistd_mock_, pipe(::testing::_)).WillByDefault([](auto fds) {
            return internal::UnistdImpl{}.pipe(fds);
        });
        ON_CALL(*unistd_mock_, read(testing::_, testing::_, testing::_)).WillByDefault([](auto fd, auto buf, auto len) {
            return internal::UnistdImpl{}.read(fd, buf, len);
        });
        ON_CALL(*unistd_mock_, write(testing::_, testing::_, testing::_))
            .WillByDefault([](auto fd, auto buf, auto len) {
                return internal::UnistdImpl{}.write(fd, buf, len);
            });
        ON_CALL(*unistd_mock_, close(testing::_)).WillByDefault([](auto fd) {
            return internal::UnistdImpl{}.close(fd);
        });

        CreatePipe(file_descriptor_1_, writing_file_descriptor_1_);
        CreatePipe(file_descriptor_2_, writing_file_descriptor_2_);
    }

    void CreatePipe(NonBlockingFileDescriptor& file_descriptor, NonBlockingFileDescriptor& writing_file_descriptor)
    {
        std::int32_t pipe_fds[2];
        auto pipe_result = internal::UnistdImpl{}.pipe(pipe_fds);
        ASSERT_TRUE(pipe_result.has_value());

        auto expected_non_blocking_file_descriptor = NonBlockingFileDescriptor::Make(pipe_fds[0]);
        if (!expected_non_blocking_file_descriptor.has_value())
        {
            GTEST_FAIL();
        }
        file_descriptor = std::move(expected_non_blocking_file_descriptor.value());

        auto expected_non_blocking_writing_file_descriptor = NonBlockingFileDescriptor::Make(pipe_fds[1]);
        if (!expected_non_blocking_writing_file_descriptor.has_value())
        {
            GTEST_FAIL();
        }
        writing_file_descriptor = std::move(expected_non_blocking_writing_file_descriptor.value());
    }

    // To retrieve return values from the implementation when mocking a call, these functions can be used.

    static constexpr auto pipe_ = [](auto fds, auto& signaled_fd, auto& signaling_fd) {
        auto result = internal::UnistdImpl{}.pipe(fds);
        signaled_fd = fds[0];
        signaling_fd = fds[1];
        return result;
    };

    static constexpr auto poll_ = [](auto fds, auto nfds, auto timeout) {
        return SysPollImpl{}.poll(fds, nfds, timeout);
    };

    std::shared_ptr<::testing::NiceMock<FcntlMock>> fcntl_mock_;
    std::shared_ptr<::testing::NiceMock<UnistdMock>> unistd_mock_;
    std::shared_ptr<::testing::NiceMock<SysPollMock>> syspoll_mock_;

    NonBlockingFileDescriptor file_descriptor_1_;
    NonBlockingFileDescriptor writing_file_descriptor_1_;

    NonBlockingFileDescriptor file_descriptor_2_;
    NonBlockingFileDescriptor writing_file_descriptor_2_;
};

TEST(AbortableBlockingReaderTestDefaultConstructor, CreatesNewPipeWhenConstructed)
{
    AbortableBlockingReader reader{};
    ASSERT_TRUE(reader.IsValid());
}

TEST_F(AbortableBlockingReaderTest, CreatesNewPipeWhenConstructed)
{
    EXPECT_CALL(*unistd_mock_, pipe(::testing::_));

    AbortableBlockingReader reader{fcntl_mock_, syspoll_mock_, unistd_mock_};
    ASSERT_TRUE(reader.IsValid());
}

TEST_F(AbortableBlockingReaderTest, MarkedInvalidIfPipeCreationFailedDuringConstruction)
{
    EXPECT_CALL(*unistd_mock_, pipe(::testing::_))
        .WillOnce(::testing::Return(score::cpp::make_unexpected(Error::createFromErrno(EPERM))));

    AbortableBlockingReader reader{fcntl_mock_, syspoll_mock_, unistd_mock_};
    ASSERT_FALSE(reader.IsValid());
    EXPECT_EQ(reader.IsValid().error(), Error::Code::kOperationNotPermitted);
}

TEST_F(AbortableBlockingReaderTest, MarkedInvalidIfFirstPipeFileDescriptorCanNotBeMadeNonBlocking)
{
    EXPECT_CALL(*fcntl_mock_, fcntl(::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(score::cpp::make_unexpected(Error::createFromErrno(EPERM))));

    AbortableBlockingReader reader{fcntl_mock_, syspoll_mock_, unistd_mock_};
    ASSERT_FALSE(reader.IsValid());
    EXPECT_EQ(reader.IsValid().error(), Error::Code::kOperationNotPermitted);
}

TEST_F(AbortableBlockingReaderTest, MarkedInvalidIfSecondPipeFileDescriptorCanNotBeMadeNonBlocking)
{
    EXPECT_CALL(*fcntl_mock_, fcntl(::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::DoDefault())
        .WillOnce(::testing::Return(score::cpp::make_unexpected(Error::createFromErrno(EPERM))));

    AbortableBlockingReader reader{fcntl_mock_, syspoll_mock_, unistd_mock_};
    ASSERT_FALSE(reader.IsValid());
    EXPECT_EQ(reader.IsValid().error(), Error::Code::kOperationNotPermitted);
}

TEST_F(AbortableBlockingReaderTest, ClosesPipeWhenDestructed)
{
    std::int32_t signaled_fd;
    std::int32_t signaling_fd;
    ON_CALL(*unistd_mock_, pipe(::testing::_)).WillByDefault([&signaled_fd, &signaling_fd](auto fds) {
        return pipe_(fds, signaled_fd, signaling_fd);
    });

    EXPECT_CALL(*unistd_mock_, close(::testing::Eq(::testing::ByRef(signaled_fd))));
    EXPECT_CALL(*unistd_mock_, close(::testing::Eq(::testing::ByRef(signaling_fd))));

    AbortableBlockingReader reader{fcntl_mock_, syspoll_mock_, unistd_mock_};
    ASSERT_TRUE(reader.IsValid());
}

TEST_F(AbortableBlockingReaderTest, CanOnlyCallReadIfMarkedValid)
{
    EXPECT_CALL(*unistd_mock_, pipe(::testing::_))
        .WillOnce(::testing::Return(score::cpp::make_unexpected(Error::createFromErrno(EPERM))));

    AbortableBlockingReader reader{fcntl_mock_, syspoll_mock_, unistd_mock_};
    ASSERT_FALSE(reader.IsValid());

    const auto result = reader.Read(file_descriptor_1_, score::cpp::span<std::uint8_t>{});
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Error::Code::kOperationNotPermitted);
}

TEST_F(AbortableBlockingReaderTest, ReadReturnsOnceDataBecomesAvailable)
{
    std::promise<void> unblock_promise{};
    EXPECT_CALL(*syspoll_mock_, poll(::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::DoAll(::testing::InvokeWithoutArgs([&unblock_promise]() {
                                       unblock_promise.set_value();
                                   }),
                                   ::testing::Invoke(poll_)));

    AbortableBlockingReader reader{fcntl_mock_, syspoll_mock_, unistd_mock_};
    ASSERT_TRUE(reader.IsValid());

    constexpr std::array<std::uint8_t, 2> write_buffer{'1', '3'};

    auto read = [this, &reader, &write_buffer]() {
        std::array<std::uint8_t, 10> buffer{};
        const auto result = reader.Read(file_descriptor_1_, buffer);
        ASSERT_TRUE(result.has_value());
        EXPECT_THAT(result.value(), ::testing::ElementsAreArray(write_buffer));
    };

    auto read_future = std::async(read);

    unblock_promise.get_future().wait();
    internal::UnistdImpl{}.write(writing_file_descriptor_1_, write_buffer.data(), write_buffer.size());
    read_future.wait();
}

TEST_F(AbortableBlockingReaderTest, ReadReturnsErrorIfSelectFails)
{
    EXPECT_CALL(*syspoll_mock_, poll(::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(score::cpp::make_unexpected(Error::createFromErrno(EPERM))));

    AbortableBlockingReader reader{fcntl_mock_, syspoll_mock_, unistd_mock_};
    ASSERT_TRUE(reader.IsValid());

    std::array<std::uint8_t, 10> buffer{};
    const auto result = reader.Read(file_descriptor_1_, buffer);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Error::Code::kOperationNotPermitted);
}

TEST_F(AbortableBlockingReaderTest, ReadReturnsErrorIfFileDescriptorIsInvalid)
{
    AbortableBlockingReader reader{fcntl_mock_, syspoll_mock_, unistd_mock_};
    ASSERT_TRUE(reader.IsValid());

    std::array<std::uint8_t, 10> buffer{};
    NonBlockingFileDescriptor invalid_file_descriptor{};
    const auto result = reader.Read(invalid_file_descriptor, buffer);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Error::Code::kInvalidArgument);
}

TEST_F(AbortableBlockingReaderTest, ReadReturnsWhenReaderIsDestructed)
{
    std::future<void> read_future{};

    {
        std::promise<void> unblock_promise{};
        EXPECT_CALL(*syspoll_mock_, poll(::testing::_, ::testing::_, ::testing::_))
            .WillOnce(::testing::DoAll(::testing::InvokeWithoutArgs([&unblock_promise]() {
                                           unblock_promise.set_value();
                                       }),
                                       ::testing::Invoke(poll_)));

        AbortableBlockingReader reader{fcntl_mock_, syspoll_mock_, unistd_mock_};
        ASSERT_TRUE(reader.IsValid());

        auto read = [this, &reader]() {
            std::array<std::uint8_t, 10> buffer{};
            const auto result = reader.Read(file_descriptor_1_, buffer);
            ASSERT_FALSE(result.has_value());
            EXPECT_EQ(result.error(), Error::Code::kOperationWasInterruptedBySignal);
        };

        read_future = std::async(read);

        unblock_promise.get_future().wait();
    }

    read_future.wait();
}

TEST_F(AbortableBlockingReaderTest, ReadReturnsWhenStopCalled)
{
    std::promise<void> unblock_promise{};
    EXPECT_CALL(*syspoll_mock_, poll(::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::DoAll(::testing::InvokeWithoutArgs([&unblock_promise]() {
                                       unblock_promise.set_value();
                                   }),
                                   ::testing::Invoke(poll_)));

    AbortableBlockingReader reader{fcntl_mock_, syspoll_mock_, unistd_mock_};
    ASSERT_TRUE(reader.IsValid());

    auto read = [this, &reader]() {
        std::array<std::uint8_t, 10> buffer{};
        const auto result = reader.Read(file_descriptor_1_, buffer);
        ASSERT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), Error::Code::kOperationWasInterruptedBySignal);
    };

    auto read_future = std::async(read);

    unblock_promise.get_future().wait();

    std::this_thread::sleep_for(std::chrono::milliseconds{10});
    reader.Stop();

    read_future.wait();
}

TEST_F(AbortableBlockingReaderTest, StopIsInvokedUntilReaderReleasesTheMutex)
{
    std::promise<void> unblock_promise{};
    EXPECT_CALL(*syspoll_mock_, poll(::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::DoAll(::testing::InvokeWithoutArgs([&unblock_promise]() {
                                       unblock_promise.set_value();
                                       std::this_thread::sleep_for(std::chrono::milliseconds{10});
                                   }),
                                   ::testing::Invoke(poll_)));

    AbortableBlockingReader reader{fcntl_mock_, syspoll_mock_, unistd_mock_};
    ASSERT_TRUE(reader.IsValid());

    auto read = [this, &reader]() {
        std::array<std::uint8_t, 10> buffer{};
        const auto result = reader.Read(file_descriptor_1_, buffer);
        ASSERT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), Error::Code::kOperationWasInterruptedBySignal);
    };

    auto read_future = std::async(read);

    unblock_promise.get_future().wait();

    reader.Stop();
    read_future.wait();
}

TEST_F(AbortableBlockingReaderTest, DestructorTerminatesOnUnexpectedError)
{
    auto stop_reader = [this] {
        EXPECT_CALL(*unistd_mock_, write(testing::_, testing::_, testing::_))
            .WillRepeatedly(::testing::Return(score::cpp::make_unexpected(Error::createFromErrno(EPERM))));
        AbortableBlockingReader reader{fcntl_mock_, syspoll_mock_, unistd_mock_};
        ASSERT_TRUE(reader.IsValid());
    };
    EXPECT_DEATH(stop_reader(), ".*");
}

TEST_F(AbortableBlockingReaderTest, ReadReturnsErrorIfAlreadyStopped)
{
    AbortableBlockingReader reader{fcntl_mock_, syspoll_mock_, unistd_mock_};
    ASSERT_TRUE(reader.IsValid());
    reader.Stop();

    std::array<std::uint8_t, 10> buffer{};
    NonBlockingFileDescriptor invalid_file_descriptor{};
    const auto result = reader.Read(invalid_file_descriptor, buffer);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Error::Code::kInvalidArgument);
}

TEST_F(AbortableBlockingReaderTest, ReadReturnsErrorIfReadFails)
{
    EXPECT_CALL(*unistd_mock_, read(file_descriptor_1_.GetUnderlying(), ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(score::cpp::make_unexpected(Error::createFromErrno(EPERM))));

    AbortableBlockingReader reader{fcntl_mock_, syspoll_mock_, unistd_mock_};
    ASSERT_TRUE(reader.IsValid());

    constexpr std::array<std::uint8_t, 2> write_buffer{'1', '3'};
    internal::UnistdImpl{}.write(writing_file_descriptor_1_, write_buffer.data(), write_buffer.size());

    std::array<std::uint8_t, 10> buffer{};
    const auto result = reader.Read(file_descriptor_1_, buffer);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Error::Code::kOperationNotPermitted);
}

TEST_F(AbortableBlockingReaderTest, ReadReturnsDataForMultipleFileDescriptorsSimultaneously)
{
    std::promise<void> unblock_promise_1{};
    std::promise<void> unblock_promise_2{};
    EXPECT_CALL(*syspoll_mock_, poll(::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::DoAll(::testing::InvokeWithoutArgs([&unblock_promise_1]() {
                                       unblock_promise_1.set_value();
                                   }),
                                   ::testing::Invoke(poll_)))
        .WillOnce(::testing::DoAll(::testing::InvokeWithoutArgs([&unblock_promise_2]() {
                                       unblock_promise_2.set_value();
                                   }),
                                   ::testing::Invoke(poll_)));

    AbortableBlockingReader reader{fcntl_mock_, syspoll_mock_, unistd_mock_};
    ASSERT_TRUE(reader.IsValid());

    constexpr std::array<std::uint8_t, 2> write_buffer{'1', '3'};

    auto read = [&reader, &write_buffer](auto& file_descriptor) {
        std::array<std::uint8_t, 10> buffer{};
        const auto result = reader.Read(file_descriptor, buffer);
        ASSERT_TRUE(result.has_value());
        EXPECT_THAT(result.value(), ::testing::ElementsAreArray(write_buffer));
    };

    auto read_future_1 = std::async([this, &read]() {
        read(file_descriptor_1_);
    });
    auto read_future_2 = std::async([this, &read]() {
        read(file_descriptor_2_);
    });

    unblock_promise_1.get_future().wait();
    unblock_promise_2.get_future().wait();

    internal::UnistdImpl{}.write(writing_file_descriptor_1_, write_buffer.data(), write_buffer.size());
    internal::UnistdImpl{}.write(writing_file_descriptor_2_, write_buffer.data(), write_buffer.size());

    read_future_1.wait();
    read_future_2.wait();
}

TEST_F(AbortableBlockingReaderTest, WillUnblockReadsForMultipleFileDescriptorsSimultaneouslyOnDestruction)
{
    std::future<void> read_future_1{};
    std::future<void> read_future_2{};

    {
        std::promise<void> unblock_promise_1{};
        std::promise<void> unblock_promise_2{};
        EXPECT_CALL(*syspoll_mock_, poll(::testing::_, ::testing::_, ::testing::_))
            .WillOnce(::testing::DoAll(::testing::InvokeWithoutArgs([&unblock_promise_1]() {
                                           unblock_promise_1.set_value();
                                       }),
                                       ::testing::Invoke(poll_)))
            .WillOnce(::testing::DoAll(::testing::InvokeWithoutArgs([&unblock_promise_2]() {
                                           unblock_promise_2.set_value();
                                       }),
                                       ::testing::Invoke(poll_)));

        AbortableBlockingReader reader{fcntl_mock_, syspoll_mock_, unistd_mock_};
        ASSERT_TRUE(reader.IsValid());

        auto read = [&reader](auto& file_descriptor) {
            std::array<std::uint8_t, 10> buffer{};
            const auto result = reader.Read(file_descriptor, buffer);
            ASSERT_FALSE(result.has_value());
            EXPECT_THAT(result.error(), Error::Code::kOperationWasInterruptedBySignal);
        };

        read_future_1 = std::async([this, &read]() {
            read(file_descriptor_1_);
        });
        read_future_2 = std::async([this, &read]() {
            read(file_descriptor_2_);
        });

        unblock_promise_1.get_future().wait();
        unblock_promise_2.get_future().wait();
    }

    read_future_1.wait();
    read_future_2.wait();
}

}  // namespace
}  // namespace os
}  // namespace score
