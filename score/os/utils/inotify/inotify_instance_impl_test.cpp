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
#include "score/os/utils/inotify/inotify_instance_impl.h"

#include "score/os/fcntl_impl.h"
#include "score/os/inotify.h"
#include "score/os/inotify_impl.h"
#include "score/os/mocklib/fcntl_mock.h"
#include "score/os/mocklib/inotify_mock.h"
#include "score/os/mocklib/sys_poll_mock.h"
#include "score/os/mocklib/unistdmock.h"
#include "score/os/sys_poll.h"
#include "score/os/sys_poll_impl.h"
#include "score/os/unistd.h"

#include <score/utility.hpp>

#include <gtest/gtest.h>

#include <fcntl.h>

#include <fstream>
#include <future>

namespace score
{
namespace os
{
namespace
{

using safecpp::literals::operator""_zsv;

class InotifyInstanceImplTest : public ::testing::Test
{
  protected:
#ifdef __QNX__
    // On QNX /tmp filesystem does not support inotify. (Ticket-114097)
    static constexpr auto test_directory = "/persistent/inotify_watch_test/"_zsv;
#else
    static constexpr auto test_directory = "/tmp/inotify_watch_test/"_zsv;
#endif
    static constexpr auto test_filename{"file"};
    static constexpr auto test_moved_filename{"moved_file"};

    void SetUp() override
    {
        internal::UnistdImpl{}.unlink(test_directory.c_str());
        ::mkdir(test_directory.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);

        fcntl_ = std::make_shared<FcntlImpl>();

        // To simultaneously introspect and execute the actual production code, we forward all mocked calls to the
        // implementation.

        inotify_mock_ = std::make_unique<::testing::NiceMock<InotifyMock>>();
        ON_CALL(*inotify_mock_, inotify_init()).WillByDefault(::testing::Return(InotifyImpl{}.inotify_init()));
        ON_CALL(*inotify_mock_, inotify_add_watch(testing::_, testing::_, testing::_))
            .WillByDefault([](auto fd, const auto name, auto mask) {
                return InotifyImpl{}.inotify_add_watch(fd, name, mask);
            });
        ON_CALL(*inotify_mock_, inotify_rm_watch(testing::_, testing::_)).WillByDefault([](auto fd, auto wd) {
            return InotifyImpl{}.inotify_rm_watch(fd, wd);
        });

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

        syspoll_mock_ = std::make_shared<::testing::NiceMock<SysPollMock>>();
        ON_CALL(*syspoll_mock_, poll(testing::_, testing::_, testing::_))
            .WillByDefault([](auto fds, auto nfds, auto timeout) {
                return SysPollImpl{}.poll(fds, nfds, timeout);
            });
    }

    void TearDown() override
    {
        Inotify::restore_instance();
        Unistd::restore_instance();

        // Remove all potentially created files and directories to make sure the tests get the expected events
        internal::UnistdImpl{}.unlink(test_filepath.c_str());
        internal::UnistdImpl{}.unlink(test_moved_filepath.c_str());
        internal::UnistdImpl{}.unlink(test_directory.c_str());
    }

    void CreateFile()
    {
        std::ofstream file{test_filepath};
        ASSERT_TRUE(file.is_open());
        file << "test";
        file.close();
    }

    void DeleteFile()
    {
        internal::UnistdImpl{}.unlink(test_filepath.c_str());
    }

    void MoveFile()
    {
        ::rename(test_filepath.c_str(), test_moved_filepath.c_str());
    }

    // To retrieve return values from the implementation when mocking a call, these functions can be used.

    static constexpr auto pipe_ = [](auto fds, auto& signaling_fd, auto& signaled_fd) {
        auto result = internal::UnistdImpl{}.pipe(fds);
        signaling_fd = fds[0];
        signaled_fd = fds[1];
        return result;
    };

    static constexpr auto inotify_init_ = [](auto& fd) {
        auto result = InotifyImpl{}.inotify_init();
        fd = result;
        return result;
    };

    static constexpr auto inotify_add_watch_ = [](const auto fd, const auto path, const auto mask, auto& wd) {
        auto result = InotifyImpl{}.inotify_add_watch(fd, path, mask);
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(result.has_value());
        wd = result.value();
        return result;
    };

    std::shared_ptr<FcntlImpl> fcntl_;
    std::shared_ptr<::testing::NiceMock<InotifyMock>> inotify_mock_;
    std::shared_ptr<::testing::NiceMock<UnistdMock>> unistd_mock_;
    std::shared_ptr<::testing::NiceMock<SysPollMock>> syspoll_mock_;

  private:
    std::string test_filepath{std::string{test_directory} + test_filename};
    std::string test_moved_filepath{std::string{test_directory} + test_moved_filename};
};

TEST_F(InotifyInstanceImplTest, ConstructsWithDefaultConstructor)
{
    InotifyInstanceImpl inotify_instance{};
    EXPECT_TRUE(inotify_instance.IsValid().has_value());
}

TEST_F(InotifyInstanceImplTest, CreatesNewInotifyInstanceWhenConstructed)
{
    EXPECT_CALL(*inotify_mock_, inotify_init());

    InotifyInstanceImpl inotify_instance{inotify_mock_, fcntl_, syspoll_mock_, unistd_mock_};
    EXPECT_TRUE(inotify_instance.IsValid().has_value());
}

TEST_F(InotifyInstanceImplTest, SetsErrorIfCreationOfNewInotifyInstanceFailsWhenConstructed)
{
    const auto error = Error::createFromErrno(EINVAL);
    EXPECT_CALL(*inotify_mock_, inotify_init()).WillOnce(::testing::Return(score::cpp::make_unexpected(error)));

    InotifyInstanceImpl inotify_instance{inotify_mock_, fcntl_, syspoll_mock_, unistd_mock_};
    ASSERT_FALSE(inotify_instance.IsValid().has_value());
    EXPECT_EQ(inotify_instance.IsValid().error(), error);
}

TEST_F(InotifyInstanceImplTest, SetsErrorIfMakingInotifyInstanceNonblockingFailsWhenConstructed)
{
    const auto error = Error::createFromErrno(EINVAL);
    auto fcntl = std::make_shared<::testing::NiceMock<FcntlMock>>();
    ON_CALL(*fcntl, fcntl(::testing::_, ::testing::_, ::testing::_))
        .WillByDefault(::testing::Return(score::cpp::make_unexpected(error)));

    InotifyInstanceImpl inotify_instance{inotify_mock_, fcntl, syspoll_mock_, unistd_mock_};
    ASSERT_FALSE(inotify_instance.IsValid().has_value());
    EXPECT_EQ(inotify_instance.IsValid().error(), error);
}

TEST_F(InotifyInstanceImplTest, SetsErrorIfCreationOfAbortableBlockingReaderFailsWhenConstructed)
{
    const auto error = Error::createFromErrno(EINVAL);
    EXPECT_CALL(*unistd_mock_, pipe(::testing::_)).WillOnce(::testing::Return(score::cpp::make_unexpected(error)));

    InotifyInstanceImpl inotify_instance{inotify_mock_, fcntl_, syspoll_mock_, unistd_mock_};
    ASSERT_FALSE(inotify_instance.IsValid().has_value());
    EXPECT_EQ(inotify_instance.IsValid().error(), error);
}

TEST_F(InotifyInstanceImplTest, ClosesInotifyInstanceWhenDestructed)
{
    score::cpp::expected<std::int32_t, Error> inotify_fd;
    EXPECT_CALL(*inotify_mock_, inotify_init()).WillOnce([&inotify_fd]() {
        return inotify_init_(inotify_fd);
    });

    std::int32_t signaling_fd;
    std::int32_t signaled_fd;
    EXPECT_CALL(*unistd_mock_, pipe(::testing::_)).WillOnce([&signaling_fd, &signaled_fd](auto fds) {
        return pipe_(fds, signaling_fd, signaled_fd);
    });

    InotifyInstanceImpl inotify_instance{inotify_mock_, fcntl_, syspoll_mock_, unistd_mock_};

    EXPECT_CALL(*unistd_mock_, close(signaling_fd));
    EXPECT_CALL(*unistd_mock_, close(signaled_fd));
    ASSERT_TRUE(inotify_fd.has_value());
    EXPECT_CALL(*unistd_mock_, close(inotify_fd.value()));

    EXPECT_TRUE(inotify_instance.IsValid().has_value());
}

TEST_F(InotifyInstanceImplTest, ClosesInotifyInstanceWhenCloseIsCalled)
{
    score::cpp::expected<std::int32_t, Error> inotify_fd;
    EXPECT_CALL(*inotify_mock_, inotify_init()).WillOnce([&inotify_fd]() {
        return inotify_init_(inotify_fd);
    });

    std::int32_t signaling_fd;
    std::int32_t signaled_fd;
    EXPECT_CALL(*unistd_mock_, pipe(::testing::_)).WillOnce([&signaling_fd, &signaled_fd](auto fds) {
        return pipe_(fds, signaling_fd, signaled_fd);
    });

    InotifyInstanceImpl inotify_instance{inotify_mock_, fcntl_, syspoll_mock_, unistd_mock_};

    EXPECT_CALL(*unistd_mock_, close(signaling_fd));
    EXPECT_CALL(*unistd_mock_, close(signaled_fd));
    ASSERT_TRUE(inotify_fd.has_value());
    EXPECT_CALL(*unistd_mock_, close(inotify_fd.value()));

    inotify_instance.Close();

    EXPECT_CALL(*unistd_mock_, close(signaling_fd)).Times(0);
    EXPECT_CALL(*unistd_mock_, close(signaled_fd)).Times(0);
    EXPECT_CALL(*unistd_mock_, close(inotify_fd.value())).Times(0);
}

TEST_F(InotifyInstanceImplTest, CreatesWatchWhenCallingAddWatch)
{
    score::cpp::expected<std::int32_t, Error> watch_descriptor;
    EXPECT_CALL(*inotify_mock_,
                inotify_add_watch(testing::_, ::testing::StrEq(test_directory), Inotify::EventMask::kInCreate))
        .WillOnce([&watch_descriptor](const auto fd, const auto path, const auto mask) {
            return inotify_add_watch_(fd, path, mask, watch_descriptor);
        });

    InotifyInstanceImpl inotify_instance{inotify_mock_, fcntl_, syspoll_mock_, unistd_mock_};
    ASSERT_TRUE(inotify_instance.IsValid().has_value());

    const auto result = inotify_instance.AddWatch(test_directory, Inotify::EventMask::kInCreate);
    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(watch_descriptor.has_value());
    EXPECT_EQ(result.value(), InotifyWatchDescriptor{watch_descriptor.value()});
}

TEST_F(InotifyInstanceImplTest, ChecksStateWhenCallingAddWatch)
{
    const auto error = Error::createFromErrno(EINVAL);
    EXPECT_CALL(*unistd_mock_, pipe(::testing::_)).WillOnce(::testing::Return(score::cpp::make_unexpected(error)));

    EXPECT_CALL(*inotify_mock_, inotify_add_watch(testing::_, ::testing::_, ::testing::_)).Times(0);

    InotifyInstanceImpl inotify_instance{inotify_mock_, fcntl_, syspoll_mock_, unistd_mock_};
    ASSERT_FALSE(inotify_instance.IsValid().has_value());

    const auto result = inotify_instance.AddWatch(test_directory, Inotify::EventMask::kInCreate);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), error);
}

TEST_F(InotifyInstanceImplTest, ReturnsErrorWhenCallingAddWatchFails)
{
    const auto error = Error::createFromErrno(EINVAL);
    EXPECT_CALL(*inotify_mock_,
                inotify_add_watch(testing::_, ::testing::StrEq(test_directory), Inotify::EventMask::kInCreate))
        .WillOnce(::testing::Return(score::cpp::make_unexpected(error)));

    InotifyInstanceImpl inotify_instance{inotify_mock_, fcntl_, syspoll_mock_, unistd_mock_};
    ASSERT_TRUE(inotify_instance.IsValid().has_value());

    const auto result = inotify_instance.AddWatch(test_directory, Inotify::EventMask::kInCreate);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), error);
}

TEST_F(InotifyInstanceImplTest, RemovesWatchWhenCallingRemoveWatch)
{
    score::cpp::expected<std::int32_t, Error> expected_underlying_watch_descriptor;
    EXPECT_CALL(*inotify_mock_,
                inotify_add_watch(testing::_, ::testing::StrEq(test_directory), Inotify::EventMask::kInCreate))
        .WillOnce([&expected_underlying_watch_descriptor](const auto fd, const auto path, const auto mask) {
            return inotify_add_watch_(fd, path, mask, expected_underlying_watch_descriptor);
        });

    std::int32_t underlying_watch_descriptor{};
    EXPECT_CALL(*inotify_mock_,
                inotify_rm_watch(testing::_, ::testing::Eq(::testing::ByRef(underlying_watch_descriptor))));

    InotifyInstanceImpl inotify_instance{inotify_mock_, fcntl_, syspoll_mock_, unistd_mock_};
    ASSERT_TRUE(inotify_instance.IsValid().has_value());

    const auto expected_watch_descriptor = inotify_instance.AddWatch(test_directory, Inotify::EventMask::kInCreate);
    ASSERT_TRUE(expected_watch_descriptor.has_value());
    ASSERT_TRUE(expected_underlying_watch_descriptor.has_value());
    underlying_watch_descriptor = expected_underlying_watch_descriptor.value();
    const auto watch_descriptor{expected_watch_descriptor.value()};

    const auto result = inotify_instance.RemoveWatch(watch_descriptor);
    EXPECT_TRUE(result.has_value());
}

TEST_F(InotifyInstanceImplTest, ChecksStateWhenCallingRemoveWatch)
{
    const auto error = Error::createFromErrno(EINVAL);
    EXPECT_CALL(*unistd_mock_, pipe(::testing::_)).WillOnce(::testing::Return(score::cpp::make_unexpected(error)));
    EXPECT_CALL(*inotify_mock_, inotify_rm_watch(testing::_, ::testing::_)).Times(0);

    InotifyInstanceImpl inotify_instance{inotify_mock_, fcntl_, syspoll_mock_, unistd_mock_};
    ASSERT_FALSE(inotify_instance.IsValid().has_value());

    InotifyWatchDescriptor watch_descriptor{1};
    const auto result = inotify_instance.RemoveWatch(watch_descriptor);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), error);
}

TEST_F(InotifyInstanceImplTest, ReturnsErrorWhenCallingRemoveWatchAndFails)
{
    const auto error = Error::createFromErrno(EINVAL);
    EXPECT_CALL(*inotify_mock_, inotify_rm_watch(testing::_, ::testing::_))
        .WillOnce(::testing::Return(score::cpp::make_unexpected(error)));

    InotifyInstanceImpl inotify_instance{inotify_mock_, fcntl_, syspoll_mock_, unistd_mock_};
    ASSERT_TRUE(inotify_instance.IsValid().has_value());

    const auto expected_watch_descriptor = inotify_instance.AddWatch(test_directory, Inotify::EventMask::kInCreate);
    ASSERT_TRUE(expected_watch_descriptor.has_value());
    const auto watch_descriptor{expected_watch_descriptor.value()};

    const auto result = inotify_instance.RemoveWatch(watch_descriptor);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), error);
}

TEST_F(InotifyInstanceImplTest, ChecksStateWhenCallingRead)
{
    const auto error = Error::createFromErrno(EINVAL);
    EXPECT_CALL(*unistd_mock_, pipe(::testing::_)).WillOnce(::testing::Return(score::cpp::make_unexpected(error)));

    InotifyInstanceImpl inotify_instance{inotify_mock_, fcntl_, syspoll_mock_, unistd_mock_};
    ASSERT_FALSE(inotify_instance.IsValid().has_value());

    auto result = inotify_instance.Read();
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), error);
}

TEST_F(InotifyInstanceImplTest, ReadReturnsWhenDestructingInotifyInstance)
{
    std::promise<void> blocking_promise{};
    ON_CALL(*syspoll_mock_, poll(testing::_, testing::_, testing::_))
        .WillByDefault([&blocking_promise](auto fds, auto nfds, auto timeout) {
            blocking_promise.set_value();
            return SysPollImpl{}.poll(fds, nfds, timeout);
        });

    std::future<void> future{};

    {
        InotifyInstanceImpl inotify_instance{inotify_mock_, fcntl_, syspoll_mock_, unistd_mock_};
        ASSERT_TRUE(inotify_instance.IsValid().has_value());

        auto inotify_read_call = [](InotifyInstanceImpl& ptr) noexcept {
            ptr.Read();
        };

        future = std::async(std::launch::async, inotify_read_call, std::ref(inotify_instance));
        blocking_promise.get_future().wait();
        const auto future_status = future.wait_for(std::chrono::milliseconds{10});
        ASSERT_EQ(future_status, std::future_status::timeout);
    }
    future.wait();
}

TEST_F(InotifyInstanceImplTest, ReadReturnsWhenCallingClose)
{
    std::promise<void> blocking_promise{};
    ON_CALL(*syspoll_mock_, poll(testing::_, testing::_, testing::_))
        .WillByDefault([&blocking_promise](auto fds, auto nfds, auto timeout) {
            blocking_promise.set_value();
            return SysPollImpl{}.poll(fds, nfds, timeout);
        });

    InotifyInstanceImpl inotify_instance{inotify_mock_, fcntl_, syspoll_mock_, unistd_mock_};
    ASSERT_TRUE(inotify_instance.IsValid().has_value());

    auto inotify_read_call = [](InotifyInstanceImpl& ptr) noexcept {
        ptr.Read();
    };

    auto future = std::async(std::launch::async, inotify_read_call, std::ref(inotify_instance));
    blocking_promise.get_future().wait();
    const auto future_status = future.wait_for(std::chrono::milliseconds{10});
    ASSERT_EQ(future_status, std::future_status::timeout);

    inotify_instance.Close();

    future.wait();
}

TEST_F(InotifyInstanceImplTest, EventNameContainsOnlyFileName)
{
    InotifyInstanceImpl inotify_instance{};
    ASSERT_TRUE(inotify_instance.IsValid().has_value());

    const auto expected_watch{inotify_instance.AddWatch(test_directory, Inotify::EventMask::kInCreate)};
    ASSERT_TRUE(expected_watch.has_value());

    CreateFile();

    auto expected_events = inotify_instance.Read();
    ASSERT_TRUE(expected_events.has_value());
    const auto events{std::move(expected_events.value())};

    EXPECT_EQ(events.size(), 1U);
    const auto& event = events[0];
    EXPECT_EQ(event.GetName(), std::string_view{test_filename});
}

TEST_F(InotifyInstanceImplTest, ReadReturnsEventWhenWatchTriggersForInCreate)
{
    InotifyInstanceImpl inotify_instance{};
    ASSERT_TRUE(inotify_instance.IsValid().has_value());

    const auto expected_watch{inotify_instance.AddWatch(test_directory, Inotify::EventMask::kInCreate)};
    ASSERT_TRUE(expected_watch.has_value());
    const auto watch{expected_watch.value()};

    CreateFile();

    auto expected_events = inotify_instance.Read();
    ASSERT_TRUE(expected_events.has_value());
    const auto events{std::move(expected_events.value())};

    EXPECT_EQ(events.size(), 1U);
    const auto& event = events[0];
    EXPECT_EQ(event.GetWatchDescriptor(), watch);
    EXPECT_EQ(event.GetMask(), InotifyEvent::ReadMask::kInCreate);
    EXPECT_EQ(event.GetName(), std::string_view{test_filename});
}

TEST_F(InotifyInstanceImplTest, ReadReturnsEventWhenWatchTriggersForInDelete)
{
    CreateFile();

    InotifyInstanceImpl inotify_instance{};
    ASSERT_TRUE(inotify_instance.IsValid().has_value());

    auto expected_watch{inotify_instance.AddWatch(test_directory, Inotify::EventMask::kInDelete)};
    ASSERT_TRUE(expected_watch.has_value());
    const auto watch{expected_watch.value()};

    DeleteFile();

    auto expected_events = inotify_instance.Read();
    ASSERT_TRUE(expected_events.has_value());
    const auto events{std::move(expected_events.value())};

    EXPECT_EQ(events.size(), 1U);
    const auto& event = events[0];
    EXPECT_EQ(event.GetWatchDescriptor(), watch);
    EXPECT_EQ(event.GetMask(), InotifyEvent::ReadMask::kInDelete);
    EXPECT_EQ(event.GetName(), std::string_view{test_filename});
}

TEST_F(InotifyInstanceImplTest, ReadReturnsEventsWhenWatchTriggersForMultipleEvents)
{
    InotifyInstanceImpl inotify_instance{};
    ASSERT_TRUE(inotify_instance.IsValid().has_value());

    auto masks{Inotify::EventMask::kInCreate | Inotify::EventMask::kInMovedTo};
    auto expected_watch{inotify_instance.AddWatch(test_directory, masks)};
    ASSERT_TRUE(expected_watch.has_value());
    const auto watch{expected_watch.value()};

    CreateFile();
    MoveFile();

    auto expected_events = inotify_instance.Read();
    ASSERT_TRUE(expected_events.has_value());
    const auto events{std::move(expected_events.value())};

    EXPECT_EQ(events.size(), 2U);
    const auto& event1 = events[0];
    EXPECT_EQ(event1.GetWatchDescriptor(), watch);
    EXPECT_EQ(event1.GetMask(), InotifyEvent::ReadMask::kInCreate);
    EXPECT_EQ(event1.GetName(), std::string_view{test_filename});

    const auto& event2 = events[1];
    EXPECT_EQ(event2.GetWatchDescriptor(), watch);
    EXPECT_EQ(event2.GetMask(), InotifyEvent::ReadMask::kInMovedTo);
    EXPECT_EQ(event2.GetName(), std::string_view{test_moved_filename});
}

}  // namespace
}  // namespace os
}  // namespace score
