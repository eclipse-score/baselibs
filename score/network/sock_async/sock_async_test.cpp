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
#include "score/network/sock_async/sock_async.h"
#include "score/network/sock_async/sock_factory.h"
#include "score/os/mocklib/socketmock.h"
#include "score/os/mocklib/sys_poll_mock.h"
#include <functional>

#include <score/expected.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <thread>
namespace score
{

namespace os
{
namespace
{

using namespace testing;

std::condition_variable cv_;
std::mutex mtx_;
constexpr std::uint8_t kTestExecOnce{1};
constexpr std::uint8_t kTestExecAmount{3};
constexpr std::uint8_t kTestExecMaxTime{2};
constexpr std::uint8_t kTestExecShortMaxTime{2};

constexpr std::int32_t SOCKET_ID = 10;

class SocketAsyncTest : public testing::Test
{
  public:
    void CallbackFn(std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> buffer, ssize_t size)
    {
        stored_result = size;
        std::cout << "CallbackFn Buffer Size:" << buffer->size() << " operation result: " << size << std::endl;
        std::cout << "CallbackFn Buffer data size:" << buffer->at(0).size() << std::endl;
        counter.fetch_add(1);
    }
    void CallbackFn1(std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> buffer, ssize_t size)
    {
        stored_result = size;
        std::cout << "CallbackFn1 Buffer Size:" << buffer->size() << " operation result: " << size << std::endl;
        std::cout << "CallbackFn1 Buffer data size:" << buffer->at(0).size() << std::endl;
        std::cout << "CallbackFn1 Buffer data1 size:" << buffer->at(1).size() << std::endl;
        counter.fetch_add(1);
    }
    void CallbackFn2(std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> buffer, ssize_t size)
    {
        stored_result = size;
        std::cout << "CallbackFn2 Buffer Size:" << buffer->size() << " operation result: " << size << std::endl;
        std::cout << "CallbackFn2 Buffer data size:" << buffer->at(0).size() << std::endl;
        counter.fetch_add(1);
    }

    void SetUp() override
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        score::os::Socket::set_testing_instance(sock_mock_);
        score::os::SysPoll::set_testing_instance(sysPollMock);
        stored_result = 0;
        counter.store(0);
        cnt = 0;
    }

    void TearDown() override
    {
        delete factory;
    }

    score::os::SocketMock sock_mock_;
    score::os::SysPollMock sysPollMock;
    ssize_t stored_result;
    std::atomic<std::int32_t> counter;
    SocketFactory* factory;
    std::int32_t cnt;
};

TEST_F(SocketAsyncTest, ReadAsyncWithDataGreaterThanZero)
{
    RecordProperty("Verifies", "SCR-21202526, SCR-21202553");
    RecordProperty("ASIL", "B");
    RecordProperty("Priority", "3");
    RecordProperty("Description", "Verifies that async read operation is successful");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    EXPECT_CALL(sock_mock_, socket(_, _, _)).Times(Exactly(1)).WillOnce(Return(SOCKET_ID));
    EXPECT_CALL(sock_mock_, recvmsg(_, _, _))
        .Times(Exactly(1))
        .WillOnce([&](const std::int32_t, msghdr* const message, const Socket::MessageFlag) {
            return static_cast<ssize_t>(message->msg_iov->iov_len);
        });
    EXPECT_CALL(sysPollMock, poll(_, _, _)).WillRepeatedly([this](struct pollfd* in_pollfd, nfds_t, int) {
        switch (this->cnt)
        {
            case 0:
                in_pollfd[0].revents = POLLIN;
                this->cnt++;
                return 1;
            case 1:
                in_pollfd[0].revents = 0;
                in_pollfd[1].revents = POLLIN;
                this->cnt++;
                return 2;
            default:
                in_pollfd[0].revents = POLLIN;
                return 1;
        }
    });

    factory = new SocketFactory();
    std::shared_ptr<SocketAsync> socketAsync = factory->CreateSocket(SockType::UDP, Endpoint{});
    static std::uint8_t test_data[] = {1, 2, 3, 4, 5, 6, 7, 8};
    score::cpp::span<std::uint8_t> test_span(test_data);
    std::vector<score::cpp::span<std::uint8_t>> test_vector;
    test_vector.push_back(test_span);

    auto lambda = [&](std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> data, ssize_t size) {
        CallbackFn(data, size);
    };
    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> span_ptr =
        std::make_shared<std::vector<score::cpp::span<std::uint8_t>>>(test_vector);
    auto result = socketAsync->ReadAsync(span_ptr, std::move(lambda));

    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait_for(lock, std::chrono::seconds(kTestExecMaxTime), [this]() {
        return (this->counter.load() == kTestExecOnce);
    });
    lock.unlock();
    ASSERT_EQ(result, kExitSuccess);
    ASSERT_EQ(stored_result, test_span.size());
}

TEST_F(SocketAsyncTest, ReadAsyncWithDataGreaterThanZeroRaw)
{
    RecordProperty("Verifies", "SCR-21202526, SCR-21202553");
    RecordProperty("ASIL", "B");
    RecordProperty("Priority", "3");
    RecordProperty("Description", "Verifies that async read operation is successful on RAW socket type");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    EXPECT_CALL(sock_mock_, socket(_, _, _)).Times(Exactly(1)).WillOnce(Return(SOCKET_ID));
    EXPECT_CALL(sock_mock_, recvmsg(_, _, _))
        .Times(Exactly(1))
        .WillOnce([&](const std::int32_t, msghdr* const message, const Socket::MessageFlag) {
            return static_cast<ssize_t>(message->msg_iov->iov_len);
        });
    EXPECT_CALL(sysPollMock, poll(_, _, _)).WillRepeatedly([this](struct pollfd* in_pollfd, nfds_t, int) {
        switch (this->cnt)
        {
            case 0:
                in_pollfd[0].revents = POLLIN;
                this->cnt++;
                return 1;
            case 1:
                in_pollfd[0].revents = 0;
                in_pollfd[1].revents = POLLIN;
                this->cnt++;
                return 2;
            default:
                in_pollfd[0].revents = POLLIN;
                return 1;
        }
    });

    factory = new SocketFactory();
    std::shared_ptr<SocketAsync> socketAsync = factory->CreateSocket(SockType::RAW, Endpoint{});
    static std::uint8_t test_data[] = {1, 2, 3, 4, 5, 6, 7, 8};
    score::cpp::span<std::uint8_t> test_span(test_data);
    std::vector<score::cpp::span<std::uint8_t>> test_vector;
    test_vector.push_back(test_span);

    auto lambda = [&](std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> data, ssize_t size) {
        CallbackFn(data, size);
    };
    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> span_ptr =
        std::make_shared<std::vector<score::cpp::span<std::uint8_t>>>(test_vector);
    auto result = socketAsync->ReadAsync(span_ptr, std::move(lambda));

    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait_for(lock, std::chrono::seconds(kTestExecMaxTime), [this]() {
        return (this->counter.load() == kTestExecOnce);
    });
    lock.unlock();
    ASSERT_EQ(result, kExitSuccess);
    ASSERT_EQ(stored_result, test_span.size());
}

TEST_F(SocketAsyncTest, ReadAsyncWithDataGreaterThanZeroTCP)
{
    RecordProperty("Verifies", "SCR-21202526, SCR-21202553");
    RecordProperty("ASIL", "B");
    RecordProperty("Priority", "3");
    RecordProperty("Description", "Verifies that async read operation is successful on TCP socket type");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    EXPECT_CALL(sock_mock_, socket(_, _, _)).Times(Exactly(1)).WillOnce(Return(SOCKET_ID));
    EXPECT_CALL(sock_mock_, recvmsg(_, _, _))
        .Times(Exactly(1))
        .WillOnce([&](const std::int32_t, msghdr* const message, const Socket::MessageFlag) {
            return static_cast<ssize_t>(message->msg_iov->iov_len);
        });
    EXPECT_CALL(sysPollMock, poll(_, _, _)).WillRepeatedly([this](struct pollfd* in_pollfd, nfds_t, int) {
        switch (this->cnt)
        {
            case 0:
                in_pollfd[0].revents = POLLIN;
                this->cnt++;
                return 1;
            case 1:
                in_pollfd[0].revents = 0;
                in_pollfd[1].revents = POLLIN;
                this->cnt++;
                return 2;
            default:
                in_pollfd[0].revents = POLLIN;
                return 1;
        }
    });

    factory = new SocketFactory();
    std::shared_ptr<SocketAsync> socketAsync = factory->CreateSocket(SockType::TCP, Endpoint{});
    static std::uint8_t test_data[] = {1, 2, 3, 4, 5, 6, 7, 8};
    score::cpp::span<std::uint8_t> test_span(test_data);
    std::vector<score::cpp::span<std::uint8_t>> test_vector;
    test_vector.push_back(test_span);

    auto lambda = [&](std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> data, ssize_t size) {
        CallbackFn(data, size);
    };
    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> span_ptr =
        std::make_shared<std::vector<score::cpp::span<std::uint8_t>>>(test_vector);
    auto result = socketAsync->ReadAsync(span_ptr, std::move(lambda));

    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait_for(lock, std::chrono::seconds(kTestExecMaxTime), [this]() {
        return (this->counter.load() == kTestExecOnce);
    });
    lock.unlock();
    ASSERT_EQ(result, kExitSuccess);
    ASSERT_EQ(stored_result, test_span.size());
}

TEST_F(SocketAsyncTest, ReadAsyncWithDataGreaterThanZeroDefaultSocket)
{
    RecordProperty("Verifies", "SCR-21202526, SCR-21202553");
    RecordProperty("ASIL", "B");
    RecordProperty("Priority", "3");
    RecordProperty("Description", "Verifies that async read operation is successful on UDP socket type");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    EXPECT_CALL(sock_mock_, socket(_, _, _)).Times(Exactly(1)).WillOnce(Return(SOCKET_ID));
    EXPECT_CALL(sock_mock_, recvmsg(_, _, _))
        .Times(Exactly(1))
        .WillOnce([&](const std::int32_t, msghdr* const message, const Socket::MessageFlag) {
            return static_cast<ssize_t>(message->msg_iov->iov_len);
        });
    EXPECT_CALL(sysPollMock, poll(_, _, _)).WillRepeatedly([this](struct pollfd* in_pollfd, nfds_t, int) {
        switch (this->cnt)
        {
            case 0:
                in_pollfd[0].revents = POLLIN;
                this->cnt++;
                return 1;
            case 1:
                in_pollfd[0].revents = 0;
                in_pollfd[1].revents = POLLIN;
                this->cnt++;
                return 2;
            default:
                in_pollfd[0].revents = POLLIN;
                return 1;
        }
    });

    factory = new SocketFactory();
    std::shared_ptr<SocketAsync> socketAsync = factory->CreateSocket(static_cast<SockType>(5), Endpoint{});
    static std::uint8_t test_data[] = {1, 2, 3, 4, 5, 6, 7, 8};
    score::cpp::span<std::uint8_t> test_span(test_data);
    std::vector<score::cpp::span<std::uint8_t>> test_vector;
    test_vector.push_back(test_span);

    auto lambda = [&](std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> data, ssize_t size) {
        CallbackFn(data, size);
    };
    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> span_ptr =
        std::make_shared<std::vector<score::cpp::span<std::uint8_t>>>(test_vector);
    auto result = socketAsync->ReadAsync(span_ptr, std::move(lambda));

    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait_for(lock, std::chrono::seconds(kTestExecMaxTime), [this]() {
        return (this->counter.load() == kTestExecOnce);
    });
    lock.unlock();
    ASSERT_EQ(result, kExitSuccess);
    ASSERT_EQ(stored_result, test_span.size());
}

TEST_F(SocketAsyncTest, ReadAsyncWithDataGreaterThanZeroMMsg)
{
    RecordProperty("Verifies", "SCR-21202526, SCR-21202553");
    RecordProperty("ASIL", "B");
    RecordProperty("Priority", "3");
    RecordProperty("Description",
                   "Verifies that async read of multiple messages operation is successful on UDP socket type");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    EXPECT_CALL(sock_mock_, socket(_, _, _)).Times(Exactly(1)).WillOnce(Return(SOCKET_ID));
    EXPECT_CALL(sock_mock_, recvmmsg(_, _, _, _, _))
        .Times(Exactly(1))
        .WillOnce(
            [&](const std::int32_t, mmsghdr* const, const unsigned int, const Socket::MessageFlag, struct timespec*) {
                return static_cast<ssize_t>(16);
            });
    EXPECT_CALL(sysPollMock, poll(_, _, _)).WillRepeatedly([this](struct pollfd* in_pollfd, nfds_t, int) {
        switch (this->cnt)
        {
            case 0:
                in_pollfd[0].revents = POLLIN;
                this->cnt++;
                return 1;
            case 1:
                in_pollfd[0].revents = 0;
                in_pollfd[1].revents = POLLIN;
                this->cnt++;
                return 2;
            default:
                in_pollfd[0].revents = POLLIN;
                return 1;
        }
    });

    factory = new SocketFactory();
    std::shared_ptr<SocketAsync> socketAsync = factory->CreateSocket(SockType::UDP, Endpoint{});
    static std::uint8_t test_data[] = {1, 2, 3, 4, 5, 6, 7, 8};
    score::cpp::span<std::uint8_t> test_span(test_data);
    static std::uint8_t test_data1[] = {9, 0, 1, 2, 9, 0, 3, 4};
    score::cpp::span<std::uint8_t> test_span1(test_data1);
    std::vector<score::cpp::span<std::uint8_t>> test_vector;
    test_vector.push_back(test_span);
    test_vector.push_back(test_span1);

    auto lambda = [&](std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> data, ssize_t size) {
        CallbackFn1(data, size);
    };
    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> span_ptr =
        std::make_shared<std::vector<score::cpp::span<std::uint8_t>>>(test_vector);
    auto result = socketAsync->ReadAsync(span_ptr, std::move(lambda));

    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait_for(lock, std::chrono::seconds(kTestExecMaxTime), [this]() {
        return (this->counter.load() == kTestExecOnce);
    });
    lock.unlock();
    ASSERT_EQ(result, kExitSuccess);
    ASSERT_EQ(stored_result, (test_span.size() + test_span1.size()));
}

TEST_F(SocketAsyncTest, ReadAsyncWithDataGreaterThanZeroMMsgWithEndPoint)
{
    RecordProperty("Verifies", "SCR-21202526, SCR-21202553");
    RecordProperty("ASIL", "B");
    RecordProperty("Priority", "3");
    RecordProperty("Description",
                   "Verifies that async read of multiple messages operation is successful on UDP socket type "
                   "when endpoint is provided");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    EXPECT_CALL(sock_mock_, socket(_, _, _)).Times(Exactly(1)).WillOnce(Return(SOCKET_ID));
    EXPECT_CALL(sock_mock_, recvmmsg(_, _, _, _, _))
        .Times(Exactly(1))
        .WillOnce(
            [&](const std::int32_t, mmsghdr* const, const unsigned int, const Socket::MessageFlag, struct timespec*) {
                return static_cast<ssize_t>(16);
            });
    EXPECT_CALL(sysPollMock, poll(_, _, _)).WillRepeatedly([this](struct pollfd* in_pollfd, nfds_t, int) {
        switch (this->cnt)
        {
            case 0:
                in_pollfd[0].revents = POLLIN;
                this->cnt++;
                return 1;
            case 1:
                in_pollfd[0].revents = 0;
                in_pollfd[1].revents = POLLIN;
                this->cnt++;
                return 2;
            default:
                in_pollfd[0].revents = POLLIN;
                return 1;
        }
    });

    factory = new SocketFactory();
    Ipv4Address addr_1(1, 2, 0, 4);
    std::uint16_t port_1{32321};
    std::shared_ptr<SocketAsync> socketAsync = factory->CreateSocket(SockType::UDP, Endpoint{addr_1, port_1});
    static std::uint8_t test_data[] = {1, 2, 3, 4, 5, 6, 7, 8};
    score::cpp::span<std::uint8_t> test_span(test_data);
    static std::uint8_t test_data1[] = {9, 0, 1, 2, 9, 0, 3, 4};
    score::cpp::span<std::uint8_t> test_span1(test_data1);
    std::vector<score::cpp::span<std::uint8_t>> test_vector;
    test_vector.push_back(test_span);
    test_vector.push_back(test_span1);

    auto lambda = [&](std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> data, ssize_t size) {
        CallbackFn1(data, size);
    };
    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> span_ptr =
        std::make_shared<std::vector<score::cpp::span<std::uint8_t>>>(test_vector);
    auto result = socketAsync->ReadAsync(span_ptr, std::move(lambda));

    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait_for(lock, std::chrono::seconds(kTestExecMaxTime), [this]() {
        return (this->counter.load() == kTestExecOnce);
    });
    lock.unlock();
    ASSERT_EQ(result, kExitSuccess);
    ASSERT_EQ(stored_result, (test_span.size() + test_span1.size()));
}

TEST_F(SocketAsyncTest, ReadAsyncWithDataEqualZero)
{
    RecordProperty("Verifies", "SCR-21202526, SCR-21202553");
    RecordProperty("ASIL", "B");
    RecordProperty("Priority", "3");
    RecordProperty("Description",
                   "Verifies that async read of multiple messages operation is successful on UDP socket type "
                   "empty data is provided ");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    EXPECT_CALL(sock_mock_, socket(_, _, _)).Times(Exactly(1)).WillOnce(Return(SOCKET_ID));
    EXPECT_CALL(sysPollMock, poll(_, _, _)).WillRepeatedly([](struct pollfd* in_pollfd, nfds_t, int) {
        in_pollfd[0].revents = POLLIN;
        return 1;
    });

    factory = new SocketFactory();
    std::shared_ptr<SocketAsync> socketAsync = factory->CreateSocket(SockType::UDP, Endpoint{});
    score::cpp::span<std::uint8_t> test_span;
    std::vector<score::cpp::span<std::uint8_t>> test_vector;
    test_vector.push_back(test_span);

    auto lambda = [&](std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> data, ssize_t size) {
        CallbackFn(data, size);
    };
    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> span_ptr =
        std::make_shared<std::vector<score::cpp::span<std::uint8_t>>>(test_vector);
    auto result = socketAsync->ReadAsync(span_ptr, std::move(lambda));
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait_for(lock, std::chrono::seconds(kTestExecShortMaxTime), [this]() {
        return (this->counter.load() == kTestExecAmount);
    });
    lock.unlock();
    ASSERT_EQ(result, kExitNumOfSocketsExceeded);
    ASSERT_EQ(stored_result, test_span.size());
}

TEST_F(SocketAsyncTest, ReadAsyncWithDataGreaterThanZeroReadFailed)
{
    RecordProperty("Verifies", "SCR-21202526, SCR-21202553");
    RecordProperty("ASIL", "B");
    RecordProperty("Priority", "3");
    RecordProperty("Description", "Verifies that async read will fail on UDP socket type");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    EXPECT_CALL(sock_mock_, socket(_, _, _)).Times(Exactly(1)).WillOnce(Return(SOCKET_ID));
    EXPECT_CALL(sysPollMock, poll(_, _, _)).WillRepeatedly([this](struct pollfd* in_pollfd, nfds_t, int) {
        switch (this->cnt)
        {
            case 0:
                in_pollfd[0].revents = POLLIN;
                this->cnt++;
                return 1;
            case 1:
                in_pollfd[0].revents = 0;
                in_pollfd[1].revents = POLLIN;
                this->cnt++;
                return 2;
            default:
                in_pollfd[0].revents = POLLIN;
                return 1;
        }
    });
    factory = new SocketFactory();
    std::shared_ptr<SocketAsync> socketAsync = factory->CreateSocket(SockType::UDP, Endpoint{});
    std::uint8_t test_data[] = {1, 2, 3, 4, 5, 6, 7, 8};
    score::cpp::span<std::uint8_t> test_span(test_data);
    std::vector<score::cpp::span<std::uint8_t>> test_vector;
    test_vector.push_back(test_span);

    EXPECT_CALL(sock_mock_, recvmsg(_, _, _))
        .Times(Exactly(1))
        .WillOnce([&](const std::int32_t, msghdr* const, const Socket::MessageFlag) {
            return static_cast<ssize_t>(kExitFailure);
        });

    auto lambda = [&](std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> data, ssize_t size) {
        CallbackFn(data, size);
    };
    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> span_ptr =
        std::make_shared<std::vector<score::cpp::span<std::uint8_t>>>(test_vector);
    auto result = socketAsync->ReadAsync(span_ptr, std::move(lambda));

    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait_for(lock, std::chrono::seconds(kTestExecShortMaxTime), [this]() {
        return (this->counter.load() == kTestExecAmount);
    });
    lock.unlock();
    ASSERT_EQ(result, kExitSuccess);
    ASSERT_EQ(stored_result, kExitFailure);
}

TEST_F(SocketAsyncTest, ReadAsyncWithDataGreaterThanZeroWithEndpoint)
{
    RecordProperty("Verifies", "SCR-21202526, SCR-21202553");
    RecordProperty("ASIL", "B");
    RecordProperty("Priority", "3");
    RecordProperty("Description", "Verifies that async read will succeed on UDP socket type");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    EXPECT_CALL(sock_mock_, socket(_, _, _)).Times(Exactly(1)).WillOnce(Return(SOCKET_ID));
    EXPECT_CALL(sysPollMock, poll(_, _, _)).WillRepeatedly([this](struct pollfd* in_pollfd, nfds_t, int) {
        switch (this->cnt)
        {
            case 0:
                in_pollfd[0].revents = POLLIN;
                this->cnt++;
                return 1;
            case 1:
                in_pollfd[0].revents = 0;
                in_pollfd[1].revents = POLLIN;
                this->cnt++;
                return 2;
            default:
                in_pollfd[0].revents = POLLIN;
                return 1;
        }
    });
    factory = new SocketFactory();
    Ipv4Address addr_1(1, 2, 0, 4);
    std::uint16_t port_1{32321};
    std::shared_ptr<SocketAsync> socketAsync = factory->CreateSocket(SockType::UDP, Endpoint{addr_1, port_1});
    std::uint8_t test_data[] = {1, 2, 3, 4, 5, 6, 7, 8};
    score::cpp::span<std::uint8_t> test_span(test_data);
    std::vector<score::cpp::span<std::uint8_t>> test_vector;
    test_vector.push_back(test_span);

    EXPECT_CALL(sock_mock_, recvmsg(_, _, _))
        .Times(Exactly(1))
        .WillOnce([&](const std::int32_t, msghdr* const message, const Socket::MessageFlag) {
            return static_cast<ssize_t>(message->msg_iov->iov_len);
        });

    auto lambda = [&](std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> data, ssize_t size) {
        CallbackFn(data, size);
    };
    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> span_ptr =
        std::make_shared<std::vector<score::cpp::span<std::uint8_t>>>(test_vector);
    auto result = socketAsync->ReadAsync(span_ptr, std::move(lambda));

    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait_for(lock, std::chrono::seconds(kTestExecMaxTime), [this]() {
        return (this->counter.load() == kTestExecOnce);
    });
    lock.unlock();
    ASSERT_EQ(result, kExitSuccess);
    ASSERT_EQ(stored_result, test_span.size());
}

TEST_F(SocketAsyncTest, WriteAsyncBytes)
{
    RecordProperty("Verifies", "SCR-21202526, SCR-21202553");
    RecordProperty("ASIL", "B");
    RecordProperty("Priority", "3");
    RecordProperty("Description", "Verifies that async write will succeed on UDP socket type");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    EXPECT_CALL(sock_mock_, socket(_, _, _)).Times(Exactly(1)).WillOnce(Return(SOCKET_ID));
    EXPECT_CALL(sysPollMock, poll(_, _, _)).WillRepeatedly([](struct pollfd* in_pollfd, nfds_t, int) {
        in_pollfd[0].revents = POLLIN;
        return 1;
    });
    std::uint8_t test_data[] = {1, 2, 3, 4, 5, 6, 7, 8};
    factory = new SocketFactory();
    std::shared_ptr<SocketAsync> socketAsync = factory->CreateSocket(SockType::UDP, Endpoint{});

    score::cpp::span<std::uint8_t> test_span(test_data);
    std::vector<score::cpp::span<std::uint8_t>> test_vector;
    test_vector.push_back(test_span);

    EXPECT_CALL(sock_mock_, sendmsg(_, _, _))
        .Times(Exactly(1))
        .WillOnce([&](const std::int32_t, const msghdr* const message, const Socket::MessageFlag) {
            return static_cast<ssize_t>(message->msg_iov->iov_len);
        });

    auto lambda = [&](std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> data, ssize_t size) {
        CallbackFn(data, size);
    };
    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> span_ptr =
        std::make_shared<std::vector<score::cpp::span<std::uint8_t>>>(test_vector);
    auto result = socketAsync->WriteAsync(span_ptr, std::move(lambda));

    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait_for(lock, std::chrono::seconds(kTestExecMaxTime), [this]() {
        return (this->counter.load() == kTestExecAmount);
    });
    ASSERT_EQ(result, kExitSuccess);
    ASSERT_EQ(stored_result, test_span.size());
}

TEST_F(SocketAsyncTest, WriteAsyncBytesRaw)
{
    RecordProperty("Verifies", "SCR-21202526, SCR-21202553");
    RecordProperty("ASIL", "B");
    RecordProperty("Priority", "3");
    RecordProperty("Description", "Verifies that async write will succeed on RAW socket type");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    EXPECT_CALL(sock_mock_, socket(_, _, _)).Times(Exactly(1)).WillOnce(Return(SOCKET_ID));
    EXPECT_CALL(sysPollMock, poll(_, _, _)).WillRepeatedly([](struct pollfd* in_pollfd, nfds_t, int) {
        in_pollfd[0].revents = POLLIN;
        return 1;
    });
    std::uint8_t test_data[] = {1, 2, 3, 4, 5, 6, 7, 8};
    factory = new SocketFactory();
    std::shared_ptr<SocketAsync> socketAsync = factory->CreateSocket(SockType::RAW, Endpoint{});

    score::cpp::span<std::uint8_t> test_span(test_data);
    std::vector<score::cpp::span<std::uint8_t>> test_vector;
    test_vector.push_back(test_span);

    EXPECT_CALL(sock_mock_, sendmsg(_, _, _))
        .Times(Exactly(1))
        .WillOnce([&](const std::int32_t, const msghdr* const message, const Socket::MessageFlag) {
            return static_cast<ssize_t>(message->msg_iov->iov_len);
        });

    auto lambda = [&](std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> data, ssize_t size) {
        CallbackFn(data, size);
    };
    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> span_ptr =
        std::make_shared<std::vector<score::cpp::span<std::uint8_t>>>(test_vector);
    auto result = socketAsync->WriteAsync(span_ptr, std::move(lambda));

    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait_for(lock, std::chrono::seconds(kTestExecMaxTime), [this]() {
        return (this->counter.load() == kTestExecAmount);
    });
    ASSERT_EQ(result, kExitSuccess);
    ASSERT_EQ(stored_result, test_span.size());
}

TEST_F(SocketAsyncTest, WriteAsyncBytesTCP)
{
    RecordProperty("Verifies", "SCR-21202526, SCR-21202553");
    RecordProperty("ASIL", "B");
    RecordProperty("Priority", "3");
    RecordProperty("Description", "Verifies that async write will succeed on TCP socket type");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    EXPECT_CALL(sock_mock_, socket(_, _, _)).Times(Exactly(1)).WillOnce(Return(SOCKET_ID));
    EXPECT_CALL(sysPollMock, poll(_, _, _)).WillRepeatedly([](struct pollfd* in_pollfd, nfds_t, int) {
        in_pollfd[0].revents = POLLIN;
        return 1;
    });
    std::uint8_t test_data[] = {1, 2, 3, 4, 5, 6, 7, 8};
    factory = new SocketFactory();
    std::shared_ptr<SocketAsync> socketAsync = factory->CreateSocket(SockType::TCP, Endpoint{});

    score::cpp::span<std::uint8_t> test_span(test_data);
    std::vector<score::cpp::span<std::uint8_t>> test_vector;
    test_vector.push_back(test_span);

    EXPECT_CALL(sock_mock_, sendmsg(_, _, _))
        .Times(Exactly(1))
        .WillOnce([&](const std::int32_t, const msghdr* const message, const Socket::MessageFlag) {
            return static_cast<ssize_t>(message->msg_iov->iov_len);
        });

    auto lambda = [&](std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> data, ssize_t size) {
        CallbackFn(data, size);
    };
    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> span_ptr =
        std::make_shared<std::vector<score::cpp::span<std::uint8_t>>>(test_vector);
    auto result = socketAsync->WriteAsync(span_ptr, std::move(lambda));

    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait_for(lock, std::chrono::seconds(kTestExecMaxTime), [this]() {
        return (this->counter.load() == kTestExecAmount);
    });
    ASSERT_EQ(result, kExitSuccess);
    ASSERT_EQ(stored_result, test_span.size());
}

TEST_F(SocketAsyncTest, WriteAsyncBytesWithEndPoint)
{
    RecordProperty("Verifies", "SCR-21202526, SCR-21202553");
    RecordProperty("ASIL", "B");
    RecordProperty("Priority", "3");
    RecordProperty("Description", "Verifies that async write will succeed on UDP socket type with endpoint");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    EXPECT_CALL(sock_mock_, socket(_, _, _)).Times(Exactly(1)).WillOnce(Return(SOCKET_ID));
    EXPECT_CALL(sysPollMock, poll(_, _, _)).WillRepeatedly([](struct pollfd* in_pollfd, nfds_t, int) {
        in_pollfd[0].revents = POLLIN;
        return 1;
    });
    std::uint8_t test_data[] = {1, 2, 3, 4, 5, 6, 7, 8};
    factory = new SocketFactory();
    Ipv4Address addr_1(1, 2, 0, 4);
    std::uint16_t port_1{32321};
    std::shared_ptr<SocketAsync> socketAsync = factory->CreateSocket(SockType::UDP, Endpoint{addr_1, port_1});

    score::cpp::span<std::uint8_t> test_span(test_data);
    std::vector<score::cpp::span<std::uint8_t>> test_vector;
    test_vector.push_back(test_span);

    EXPECT_CALL(sock_mock_, sendmsg(_, _, _))
        .Times(Exactly(1))
        .WillOnce([&](const std::int32_t, const msghdr* const message, const Socket::MessageFlag) {
            return static_cast<ssize_t>(message->msg_iov->iov_len);
        });

    auto lambda = [&](std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> data, ssize_t size) {
        CallbackFn(data, size);
    };
    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> span_ptr =
        std::make_shared<std::vector<score::cpp::span<std::uint8_t>>>(test_vector);
    auto result = socketAsync->WriteAsync(span_ptr, std::move(lambda));

    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait_for(lock, std::chrono::seconds(kTestExecMaxTime), [this]() {
        return (this->counter.load() == kTestExecAmount);
    });
    ASSERT_EQ(result, kExitSuccess);
    ASSERT_EQ(stored_result, test_span.size());
}

TEST_F(SocketAsyncTest, WriteAsyncWithDataGreaterThanZeroWriteFailed)
{
    RecordProperty("Verifies", "SCR-21202526, SCR-21202553");
    RecordProperty("ASIL", "B");
    RecordProperty("Priority", "3");
    RecordProperty("Description", "Verifies that async write will fail on UDP socket type");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    EXPECT_CALL(sock_mock_, socket(_, _, _)).Times(Exactly(1)).WillOnce(Return(SOCKET_ID));
    EXPECT_CALL(sysPollMock, poll(_, _, _)).WillRepeatedly([](struct pollfd* in_pollfd, nfds_t, int) {
        in_pollfd[0].revents = POLLIN;
        return 1;
    });
    std::uint8_t test_data[] = {1, 2, 3, 4, 5, 6, 7, 8};
    factory = new SocketFactory();
    std::shared_ptr<SocketAsync> socketAsync = factory->CreateSocket(SockType::UDP, Endpoint{});

    score::cpp::span<std::uint8_t> test_span(test_data);
    std::vector<score::cpp::span<std::uint8_t>> test_vector;
    test_vector.push_back(test_span);

    EXPECT_CALL(sock_mock_, sendmsg(_, _, _))
        .Times(Exactly(1))
        .WillOnce([&](const std::int32_t, const msghdr* const, const Socket::MessageFlag) {
            return static_cast<ssize_t>(kExitFailure);
        });

    auto lambda = [&](std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> data, ssize_t size) {
        CallbackFn(data, size);
    };
    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> span_ptr =
        std::make_shared<std::vector<score::cpp::span<std::uint8_t>>>(test_vector);
    auto result = socketAsync->WriteAsync(span_ptr, std::move(lambda));

    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait_for(lock, std::chrono::seconds(kTestExecMaxTime), [this]() {
        return (this->counter.load() == kTestExecAmount);
    });
    ASSERT_EQ(result, kExitSuccess);
    ASSERT_EQ(stored_result, kExitFailure);
}

TEST_F(SocketAsyncTest, WriteAsyncBytesNoData)
{
    RecordProperty("Verifies", "SCR-21202526, SCR-21202553");
    RecordProperty("ASIL", "B");
    RecordProperty("Priority", "3");
    RecordProperty("Description", "Verifies that async write with empty data on UDP socket type");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    EXPECT_CALL(sock_mock_, socket(_, _, _)).Times(Exactly(1)).WillOnce(Return(SOCKET_ID));
    EXPECT_CALL(sysPollMock, poll(_, _, _)).WillRepeatedly([](struct pollfd* in_pollfd, nfds_t, int) {
        in_pollfd[0].revents = POLLIN;
        return 1;
    });
    score::cpp::span<std::uint8_t> test_span;
    std::vector<score::cpp::span<std::uint8_t>> test_vector;
    factory = new SocketFactory();
    std::shared_ptr<SocketAsync> socketAsync = factory->CreateSocket(SockType::UDP, Endpoint{});

    auto lambda = [&](std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> data, ssize_t size) {
        CallbackFn(data, size);
    };
    std::shared_ptr<std::vector<score::cpp::span<std::uint8_t>>> span_ptr =
        std::make_shared<std::vector<score::cpp::span<std::uint8_t>>>(test_vector);
    auto result = socketAsync->WriteAsync(span_ptr, std::move(lambda));

    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait_for(lock, std::chrono::seconds(kTestExecMaxTime), [this]() {
        return (this->counter.load() == kTestExecOnce);
    });
    ASSERT_EQ(result, kExitNumOfSocketsExceeded);
    ASSERT_EQ(stored_result, test_span.size());
}
}  // namespace
}  // namespace os
}  // namespace score
