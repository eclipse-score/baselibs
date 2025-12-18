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
#include "score/os/pthread.h"
#include "score/os/mocklib/mock_pthread.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <future>
#include <thread>

#if defined(__QNX__)
#include <sys/neutrino.h>
#endif

namespace score
{
namespace os
{
namespace
{

using ::testing::Test;

#if defined(__linux__)
// The thread name is a meaningful C language string, whose length is restricted to 16 characters, including the
// terminating null byte ('\0').
constexpr std::size_t length = 16U;
#elif defined(__QNX__)
constexpr std::size_t length = _NTO_THREAD_NAME_MAX;
#else
#error "Unsupported OS"
#endif

class PthreadNameTest : public Test
{
  public:
    const char expected[length] = "foobar";
    char actual[length];
};

// Function that will be run by the thread
void* thread_func(void*)
{
    return nullptr;
}

TEST_F(PthreadNameTest, SetNameSucceeds)
{
    RecordProperty("Description", "PthreadNameTest Set Name Succeeds");
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("TestType", "T-REQ");

    Pthread::instance().self();

    EXPECT_TRUE(Pthread::instance().setname_np(Pthread::instance().self(), expected).has_value());
    EXPECT_EQ(::pthread_getname_np(pthread_self(), actual, length), 0);
    EXPECT_STREQ(expected, actual);
}

TEST_F(PthreadNameTest, GetNameSucceeds)
{
    RecordProperty("Description", "PthreadNameTest Get Name Succeeds");
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("TestType", "T-REQ");

    EXPECT_EQ(::pthread_setname_np(pthread_self(), expected), 0);
    EXPECT_TRUE(Pthread::instance().getname_np(Pthread::instance().self(), actual, length).has_value());
    EXPECT_STREQ(expected, actual);
}

TEST_F(PthreadNameTest, GetCpuClockIdSucceeds)
{
    RecordProperty("Description", "PthreadNameTest Get Cpu Clock Id Succeeds");
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("TestType", "T-REQ");

    clockid_t clock_id{};
    auto result = Pthread::instance().getcpuclockid(Pthread::instance().self(), &clock_id);
    EXPECT_TRUE(result.has_value());

    struct timespec tspec;
    EXPECT_EQ(::clock_gettime(clock_id, &tspec), 0);
    EXPECT_GT(tspec.tv_nsec, 0);
}

TEST_F(PthreadNameTest, GetCpuClockIdFails)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PthreadNameTest Get Cpu Clock Id Fails");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pthread_t thread;

    // Step 1: Create a thread
    int ret = pthread_create(&thread, nullptr, thread_func, nullptr);
    ASSERT_EQ(ret, 0) << "Failed to create thread";

    // Step 2: Join the thread to ensure it has exited
    ret = pthread_join(thread, nullptr);
    ASSERT_EQ(ret, 0) << "Failed to join thread";

    // Step 3: Try to get the CPU clock ID of the thread that has already exited
    clockid_t clock_id{};
    auto result = Pthread::instance().getcpuclockid(thread, &clock_id);

    ASSERT_FALSE(result.has_value());
}

TEST_F(PthreadNameTest, SetNameFailsWhenNameTooLong)
{
    RecordProperty("Description", "PthreadNameTest Set Name Fails When Name Too Long");
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("TestType", "T-REQ");

    std::array<char, length + 1U> name_too_long{};
    name_too_long.fill('1');
    (*name_too_long.begin()) = '2';
    (*name_too_long.rbegin()) = '\0';
    const auto result = Pthread::instance().setname_np(Pthread::instance().self(), name_too_long.data());

#if defined(__linux__)
    ASSERT_FALSE(result.has_value());
#elif defined(__QNX__)
    // QNX does not follow their documentation. In case of a too long name, the name is just cut off.
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(Pthread::instance().getname_np(Pthread::instance().self(), actual, length).has_value());
    std::array<char, length> name_cut_off{};
    name_cut_off.fill('1');
    (*name_cut_off.begin()) = '2';
    (*name_cut_off.rbegin()) = '\0';
    EXPECT_STREQ(actual, name_cut_off.data());
#endif
}

TEST_F(PthreadNameTest, GetNameFailsWhenBufferTooSmall)
{
    RecordProperty("Description", "PthreadNameTest Get Name Fails When Buffer Too Small");
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("TestType", "T-REQ");

    const auto write_result = Pthread::instance().setname_np(Pthread::instance().self(), expected);
    ASSERT_TRUE(write_result.has_value());

    const std::size_t size_too_small = strlen(expected);
    const auto read_result = Pthread::instance().getname_np(Pthread::instance().self(), actual, size_too_small);

#if defined(__linux__)
    ASSERT_FALSE(read_result.has_value());
#elif defined(__QNX__)
    // QNX does not follow their documentation. In case of a too long name, the name is just cut off.
    ASSERT_TRUE(read_result.has_value());
    char cut_off[length];
    std::memcpy(cut_off, expected, strlen(expected));
    cut_off[size_too_small - 1U] = '\0';
    EXPECT_STREQ(actual, cut_off);
#endif
}

TEST(PthreadTestingInstanceTest, RestoresOriginalAfterSettingTestingInstance)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PthreadTestingInstanceTest Restores Original After Setting Testing Instance");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const Pthread* instance = &Pthread::instance();

    MockPthread mock_pthread;
    Pthread::set_testing_instance(mock_pthread);
    const Pthread* testing_instance = &Pthread::instance();
    EXPECT_NE(testing_instance, instance);

    Pthread::restore_instance();
    const Pthread* original_instance = &Pthread::instance();
    EXPECT_EQ(original_instance, instance);
}

class PthreadMockTest : public Test
{
  public:
    PthreadMockTest()
    {
        Pthread::set_testing_instance(mock_pthread);
    }

    ~PthreadMockTest()
    {
        Pthread::restore_instance();
    }

    MockPthread mock_pthread;
    const char expected[length]{"foobar"};
    char actual[length];
};

TEST_F(PthreadMockTest, SetNameCallsPOSIXAPI)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PthreadMockTest Set Name Calls POSIXAPI");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    EXPECT_CALL(mock_pthread, setname_np);
    Pthread::instance().setname_np(Pthread::instance().self(), expected);
}

TEST_F(PthreadMockTest, GetNameCallsPOSIXAPI)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PthreadMockTest Get Name Calls POSIXAPI");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    EXPECT_CALL(mock_pthread, getname_np);
    Pthread::instance().getname_np(Pthread::instance().self(), actual, length);
}

TEST(PthreadCondAttrTest, Init)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PthreadCondAttrTest Init");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pthread_condattr_t attr{};
    const auto result = Pthread::instance().condattr_init(&attr);
    EXPECT_TRUE(result.has_value());
}

TEST(PthreadCondAttrTest, SetPSharedSetsNewAttribute)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PthreadCondAttrTest Set PShared Sets New Attribute");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pthread_condattr_t attr{};
    const std::int32_t pshared{PTHREAD_PROCESS_SHARED};
    const auto set_result = Pthread::instance().condattr_setpshared(&attr, pshared);
    ASSERT_TRUE(set_result.has_value());
    std::int32_t new_pshared{PTHREAD_PROCESS_PRIVATE};
    const auto get_result = ::pthread_condattr_getpshared(&attr, &new_pshared);
    ASSERT_EQ(get_result, 0);
    EXPECT_EQ(new_pshared, pshared);
}

TEST(PthreadCondAttrTest, SetPSharedFailsWhenNewAttributeUnknown)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PthreadCondAttrTest Set PShared Fails When New Attribute Unknown");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pthread_condattr_t attr{};
    const std::int32_t invalid_pshared{5};
    const auto result = Pthread::instance().condattr_setpshared(&attr, invalid_pshared);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Error::Code::kInvalidArgument);
}

TEST(PthreadCondAttrTest, Destroy)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PthreadCondAttrTest Destroy");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pthread_condattr_t attr{};
    const auto result = Pthread::instance().condattr_destroy(&attr);
    EXPECT_TRUE(result.has_value());
}

TEST(PthreadCondTest, Init)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PthreadCondTest Init");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pthread_cond_t cond{};
    pthread_condattr_t attr{};
    const auto result = Pthread::instance().cond_init(&cond, &attr);
    EXPECT_TRUE(result.has_value());
}

TEST(PthreadCondTest, Destroy)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PthreadCondTest Destroy");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pthread_cond_t cond{};
    const auto result = Pthread::instance().cond_destroy(&cond);
    EXPECT_TRUE(result.has_value());
}

TEST(PthreadMutexAttrTest, Init)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PthreadMutexAttrTest Init");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pthread_mutexattr_t attr{};
    const auto result = Pthread::instance().mutexattr_init(&attr);
    EXPECT_TRUE(result.has_value());
}

TEST(PthreadMutexAttrTest, SetPSharedSetsNewAttribute)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PthreadMutexAttrTest Set PShared Sets New Attribute");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pthread_mutexattr_t attr{};
    const std::int32_t pshared{PTHREAD_PROCESS_SHARED};
    const auto set_result = Pthread::instance().mutexattr_setpshared(&attr, pshared);
    ASSERT_TRUE(set_result.has_value());
    std::int32_t new_pshared{PTHREAD_PROCESS_PRIVATE};
    const auto get_result = ::pthread_mutexattr_getpshared(&attr, &new_pshared);
    ASSERT_EQ(get_result, 0);
    EXPECT_EQ(new_pshared, pshared);
}

TEST(PthreadMutexAttrTest, SetPSharedFailsWhenNewAttributeUnknown)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PthreadMutexAttrTest Set PShared Fails When New Attribute Unknown");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pthread_mutexattr_t attr{};
    const std::int32_t invalid_pshared{5};
    const auto result = Pthread::instance().mutexattr_setpshared(&attr, invalid_pshared);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Error::Code::kInvalidArgument);
}

TEST(PthreadMutexAttrTest, Destroy)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PthreadMutexAttrTest Destroy");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pthread_mutexattr_t attr{};
    const auto result = Pthread::instance().mutexattr_destroy(&attr);
    EXPECT_TRUE(result.has_value());
}

TEST(PthreadMutexTest, Init)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PthreadMutexTest Init");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pthread_mutex_t mutex{};
    pthread_mutexattr_t attr{};
    const auto result = Pthread::instance().mutex_init(&mutex, &attr);
    EXPECT_TRUE(result.has_value());
}

TEST(PthreadMutexTest, Destroy)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PthreadMutexTest Destroy");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    pthread_mutex_t mutex{};
    const auto result = Pthread::instance().mutex_destroy(&mutex);
    EXPECT_TRUE(result.has_value());
}

TEST(PthreadMutexTest, DestroyFailsIfMutexLockedByOtherThread)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PthreadMutexTest Destroy Fails If Mutex Locked By Other Thread");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    // TSAN is rightly complaining that we try to destroy a locked mutex. But this behavior is the core of the test.
    // If a user tries to destroy a mutex that is still locked, he should get an error as intended by POSIX. This test
    // is to make sure that this actually happens. Therefore, we only run the test outside of TSAN.
#if !defined(__SANITIZE_THREAD__)
    pthread_mutex_t mutex{};
    pthread_mutexattr_t attr{};
    const auto result_attr_init = Pthread::instance().mutexattr_init(&attr);
    ASSERT_TRUE(result_attr_init.has_value());
    const auto result_init = Pthread::instance().mutex_init(&mutex, &attr);
    ASSERT_TRUE(result_init.has_value());

    std::promise<void> lock_promise{};
    std::promise<void> unlock_promise{};
    std::thread lock_thread{[&mutex, &lock_promise, &unlock_promise]() {
        pthread_mutex_lock(&mutex);
        lock_promise.set_value();
        unlock_promise.get_future().wait();
        pthread_mutex_unlock(&mutex);
    }};

    lock_promise.get_future().wait();
    const auto result_destroy = Pthread::instance().mutex_destroy(&mutex);
    ASSERT_FALSE(result_destroy.has_value());
    EXPECT_EQ(result_destroy.error(), Error::Code::kDeviceOrResourceBusy);
    unlock_promise.set_value();
    lock_thread.join();
    Pthread::instance().mutex_destroy(&mutex);
#else
    GTEST_SKIP() << "Not supported in TSAN since we verify that destruction of a locked mutex returns an error";
#endif
}

TEST(PthreadSchedParamTest, SetSchedParam)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PthreadSchedParamTest Set Sched Param");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::promise<pthread_t> thread_id_promise{};
    std::promise<void> destroy_promise{};
    std::thread lock_thread{[&thread_id_promise, &destroy_promise]() {
        thread_id_promise.set_value(pthread_self());
        destroy_promise.get_future().wait();
    }};

    pthread_t thread{thread_id_promise.get_future().get()};
    int policy{SCHED_OTHER};
    sched_param sched_p{};
#if defined(__linux__)
    sched_p.sched_priority = 0;
#elif defined(__QNX__)
    sched_p.sched_priority = 1;
#else
#error "Unsupported OS"
#endif
    const auto result = Pthread::instance().pthread_setschedparam(thread, policy, &sched_p);
    EXPECT_TRUE(result.has_value());

    destroy_promise.set_value();
    lock_thread.join();
}

TEST(PthreadSchedParamTest, SetSchedParamFailsWithInvalidThread)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PthreadSchedParamTest Set Sched Param Fails With Invalid Thread");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::promise<pthread_t> thread_id_promise{};
    std::promise<void> destroy_promise{};
    std::thread lock_thread{[&thread_id_promise, &destroy_promise]() {
        thread_id_promise.set_value(pthread_self());
        destroy_promise.get_future().wait();
    }};

    pthread_t thread{thread_id_promise.get_future().get()};
    destroy_promise.set_value();
    lock_thread.join();

    int policy{SCHED_FIFO};
    sched_param sched_p{};
    sched_p.sched_priority = 1;
    const auto result = Pthread::instance().pthread_setschedparam(thread, policy, &sched_p);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Error::Code::kNoSuchProcess);
}

TEST(PthreadSchedParamTest, GetSchedParam)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PthreadSchedParamTest Get Sched Param");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::promise<pthread_t> thread_id_promise{};
    std::promise<void> destroy_promise{};
    std::thread lock_thread{[&thread_id_promise, &destroy_promise]() {
        thread_id_promise.set_value(pthread_self());
        destroy_promise.get_future().wait();
    }};

    pthread_t thread{thread_id_promise.get_future().get()};

    int policy{SCHED_OTHER};
    sched_param sched_p{};
#if defined(__linux__)
    sched_p.sched_priority = 0;
#elif defined(__QNX__)
    sched_p.sched_priority = 1;
#else
#error "Unsupported OS"
#endif
    const auto result = Pthread::instance().pthread_setschedparam(thread, policy, &sched_p);
    ASSERT_TRUE(result.has_value());

    int read_policy{};
    sched_param read_sched_p{};
    const auto read_result = Pthread::instance().pthread_getschedparam(thread, &read_policy, &read_sched_p);
    ASSERT_TRUE(read_result.has_value());
    EXPECT_EQ(read_policy, policy);
    EXPECT_EQ(read_sched_p.sched_priority, sched_p.sched_priority);

    destroy_promise.set_value();
    lock_thread.join();
}

TEST(PthreadSchedParamTest, GetSchedParamFailsWithJoinedThread)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PthreadSchedParamTest Get Sched Param Fails With Joined Thread");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::promise<pthread_t> thread_id_promise{};
    std::promise<void> destroy_promise{};
    std::thread lock_thread{[&thread_id_promise, &destroy_promise]() {
        thread_id_promise.set_value(pthread_self());
        destroy_promise.get_future().wait();
    }};

    pthread_t thread{thread_id_promise.get_future().get()};
    destroy_promise.set_value();
    lock_thread.join();

    int read_policy{};
    sched_param read_sched_p{};
    const auto read_result = Pthread::instance().pthread_getschedparam(thread, &read_policy, &read_sched_p);
    ASSERT_FALSE(read_result.has_value());
    EXPECT_EQ(read_result.error(), Error::Code::kNoSuchProcess);
}

}  // namespace
}  // namespace os
}  // namespace score
