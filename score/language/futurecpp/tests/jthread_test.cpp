//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

///
/// \file
/// \copyright Copyright (c) 2021 Contributors to the Eclipse Foundation
///
/// The implementation is based on \c std::thread from https://github.com/llvm/llvm-project/tree/main/libcxx with
/// modifications listed in \c NOTICE.
///

#include <score/jthread.hpp>
#include <score/jthread.hpp> // test include guard

#include <score/vector.hpp>

#include <array>
#include <atomic>
#include <sstream>
#include <string>
#include <thread>

#include <gtest/gtest.h>

namespace
{

// Named requirements
static_assert(!std::is_copy_constructible<score::cpp::jthread>::value, "");
static_assert(!std::is_copy_assignable<score::cpp::jthread>::value, "");
static_assert(std::is_move_constructible<score::cpp::jthread>::value, "");
static_assert(std::is_move_assignable<score::cpp::jthread>::value, "");
static_assert(std::is_trivially_copyable<score::cpp::jthread::id>::value, "");

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/libcxx/thread/thread.threads/thread.thread.class/types.pass.cpp
static_assert(std::is_same<score::cpp::jthread::native_handle_type, pthread_t>::value, "");

class tracker
{
public:
    static int n_alive;
    static bool op_run;

    tracker() : alive_(1) { ++n_alive; }

    tracker(const tracker& other) : alive_(other.alive_) { ++n_alive; }

    ~tracker()
    {
        alive_ = 0;
        --n_alive;
    }

    void operator()() const
    {
        EXPECT_EQ(alive_, 1);
        EXPECT_GE(n_alive, 1);
        op_run = true;
    }

private:
    int alive_;
};

int tracker::n_alive = 0;
bool tracker::op_run = false;

std::string get_this_thread_name()
{
    std::array<char, score::cpp::detail::thread_name_hint::get_max_thread_name_length()> n{};
    EXPECT_EQ(::pthread_getname_np(::pthread_self(), n.data(), n.size()), 0);
    return std::string(n.data());
}

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/libcxx/thread/thread.threads/thread.thread.class/thread.thread.member/native_handle.pass.cpp
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, native_handle_represents_a_thread)
{
    tracker track;
    score::cpp::jthread t{track};
    EXPECT_NE(t.native_handle(), pthread_t{});
    t.join();
    EXPECT_TRUE(tracker::op_run);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, default_construct)
{
    score::cpp::jthread thread;
    EXPECT_FALSE(thread.joinable());
    EXPECT_EQ(thread.get_id(), score::cpp::jthread::id{});
    EXPECT_FALSE(thread.get_stop_source().stop_possible());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, construct)
{
    score::cpp::jthread thread([] {});
    EXPECT_TRUE(thread.joinable());
    EXPECT_NE(thread.get_id(), score::cpp::jthread::id{});
    EXPECT_TRUE(thread.get_stop_source().stop_possible());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, construct_with_lambda)
{
    bool called{false};
    score::cpp::jthread t{[&called] { called = true; }};
    t.join();
    EXPECT_TRUE(called);
}

// The stack size attribute determines the minimum size (in bytes) that will be allocated for threads created using the
// thread attributes object attr.
// https://man7.org/linux/man-pages/man3/pthread_attr_setstacksize.3.html#DESCRIPTION
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, construct_with_stack_size)
{
#if defined(__QNX__)
    GTEST_SKIP() << "test relies on non-available QNX functions";
#else
    constexpr score::cpp::jthread::stack_size_hint expected_stack_size{1'024 * 1'024};
    score::cpp::jthread t{expected_stack_size, [expected_stack_size] {
                       pthread_attr_t attr;
                       EXPECT_EQ(pthread_getattr_np(pthread_self(), &attr), 0); // not available in QNX
                       std::size_t actual_stack_size{0};
                       EXPECT_EQ(pthread_attr_getstacksize(&attr, &actual_stack_size), 0);
                       EXPECT_EQ(pthread_attr_destroy(&attr), 0);

                       EXPECT_GE(actual_stack_size, expected_stack_size.value());
                   }};
#endif
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, construct_with_stack_size_and_stop_token)
{
    constexpr score::cpp::jthread::stack_size_hint stack_size{1'024 * 1'024};
    score::cpp::jthread t{stack_size, [](const score::cpp::stop_token& stop_token) {
                       while (!stop_token.stop_requested())
                       {
                           std::this_thread::yield();
                       }
                       EXPECT_TRUE(stop_token.stop_requested());
                   }};
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, construct_with_max_stack_size_fails)
{
    constexpr score::cpp::jthread::stack_size_hint max_stack_size{std::numeric_limits<std::size_t>::max()};
    EXPECT_THROW(score::cpp::jthread(max_stack_size, [] {}), std::system_error);
    EXPECT_THROW(score::cpp::jthread(max_stack_size, [](const score::cpp::stop_token&) {}), std::system_error);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, construct_with_name)
{
    const std::string name(score::cpp::detail::thread_name_hint::get_max_thread_name_length(), 'a');
    score::cpp::jthread t{score::cpp::jthread::name_hint{name}, [] {
                       EXPECT_EQ(get_this_thread_name(),
                                 std::string(score::cpp::detail::thread_name_hint::get_max_thread_name_length() - 1U, 'a'));
                   }};
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, construct_with_name_and_stop_token)
{
    const std::string name(score::cpp::detail::thread_name_hint::get_max_thread_name_length(), 'a');
    score::cpp::jthread t{score::cpp::jthread::name_hint{name}, [](const score::cpp::stop_token&) {
                       EXPECT_EQ(get_this_thread_name(),
                                 std::string(score::cpp::detail::thread_name_hint::get_max_thread_name_length() - 1U, 'a'));
                   }};
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, construct_with_stack_size_and_name)
{
    constexpr score::cpp::jthread::stack_size_hint stack_size{1'024 * 1'024};
    const std::string name(score::cpp::detail::thread_name_hint::get_max_thread_name_length(), 'a');
    score::cpp::jthread t{stack_size, score::cpp::jthread::name_hint{name}, [] {
                       EXPECT_EQ(get_this_thread_name(),
                                 std::string(score::cpp::detail::thread_name_hint::get_max_thread_name_length() - 1U, 'a'));
                   }};
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, construct_with_stack_size_and_name_and_stop_token)
{
    constexpr score::cpp::jthread::stack_size_hint stack_size{1'024 * 1'024};
    const std::string name(score::cpp::detail::thread_name_hint::get_max_thread_name_length(), 'a');
    score::cpp::jthread t{stack_size, score::cpp::jthread::name_hint{name}, [](const score::cpp::stop_token&) {
                       EXPECT_EQ(get_this_thread_name(),
                                 std::string(score::cpp::detail::thread_name_hint::get_max_thread_name_length() - 1U, 'a'));
                   }};
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, move_constructor_transfers_id_and_native_handle)
{
    score::cpp::jthread t1{[]() {}};
    const score::cpp::jthread::id id1{t1.get_id()};
    const score::cpp::jthread::native_handle_type native_handle1{t1.native_handle()};

    score::cpp::jthread t2{std::move(t1)};

    EXPECT_EQ(t2.native_handle(), native_handle1);
    EXPECT_EQ(t1.native_handle(), score::cpp::jthread::native_handle_type{});
    EXPECT_EQ(t2.get_id(), id1);
    EXPECT_EQ(t1.get_id(), score::cpp::jthread::id());
    EXPECT_FALSE(t1.joinable());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, perfect_forward)
{
    struct move_only
    {
        move_only() = default;
        move_only(const move_only&) = delete;
        move_only(move_only&&) = default;
        move_only& operator=(const move_only&) = delete;
        move_only& operator=(move_only&&) = default;
        ~move_only() = default;

        void operator()(move_only&&) const& = delete;
        void operator()(move_only&&) const&& {}
    };

    score::cpp::pmr::vector<score::cpp::jthread> v;
    move_only f{};
    move_only arg{};
    v.emplace_back(score::cpp::jthread::stack_size_hint{1'024U * 1'024U}, std::move(f), std::move(arg));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, move_assignment_transfers_id_and_native_handle)
{
    score::cpp::jthread t1{[]() {}};
    const score::cpp::jthread::id id1{t1.get_id()};
    const score::cpp::jthread::native_handle_type native_handle1{t1.native_handle()};

    score::cpp::jthread t2{[]() {}};
    t2 = std::move(t1);

    EXPECT_EQ(t2.native_handle(), native_handle1);
    EXPECT_EQ(t1.native_handle(), score::cpp::jthread::native_handle_type{});
    EXPECT_EQ(t2.get_id(), id1);
    EXPECT_EQ(t1.get_id(), score::cpp::jthread::id());
    EXPECT_FALSE(t1.joinable());
}
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, move_assignment_of_non_joinable_type)
{
    score::cpp::jthread t1{};
    EXPECT_FALSE(t1.joinable());
    score::cpp::jthread t2{[]() {}};
    EXPECT_TRUE(t2.joinable());
    t2 = std::move(t1);
    EXPECT_FALSE(t2.joinable());
}
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, hardware_concurrency)
{
    EXPECT_EQ(score::cpp::jthread::hardware_concurrency(), std::thread::hardware_concurrency());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, detach)
{
    score::cpp::jthread thread([] {});
    EXPECT_TRUE(thread.joinable());
    thread.detach();
    EXPECT_FALSE(thread.joinable());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, join_error)
{
    score::cpp::jthread thread;
    EXPECT_FALSE(thread.joinable());
    EXPECT_THROW(thread.join(), std::system_error);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, detach_error)
{
    score::cpp::jthread thread;
    EXPECT_FALSE(thread.joinable());
    EXPECT_THROW(thread.detach(), std::system_error);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, join)
{
    score::cpp::jthread thread([] {});
    EXPECT_TRUE(thread.joinable());
    thread.join();
    EXPECT_FALSE(thread.joinable());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, auto_join)
{
    std::atomic<bool> has_joined{false};

    {
        score::cpp::jthread thread([&has_joined]() {
            std::this_thread::yield();
            has_joined.store(true);
        });
    }

    EXPECT_TRUE(has_joined.load());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, stop_via_stop_token)
{
    std::atomic<bool> has_stopped{false};

    score::cpp::jthread thread([&has_stopped](const score::cpp::stop_token& stop_token) {
        while (!stop_token.stop_requested())
        {
            std::this_thread::yield();
        }
        has_stopped.store(true);
    });

    EXPECT_TRUE(thread.joinable());
    EXPECT_FALSE(has_stopped.load());
    thread.request_stop();
    thread.join();
    EXPECT_TRUE(has_stopped.load());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, stop_source)
{
    score::cpp::jthread thread([]() {});
    score::cpp::stop_source ss{thread.get_stop_source()};
    const score::cpp::stop_token st{thread.get_stop_token()};
    EXPECT_FALSE(ss.stop_requested());
    EXPECT_FALSE(st.stop_requested());
    ss.request_stop();
    EXPECT_TRUE(st.stop_requested());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, same_thread_has_equal_hash)
{
    score::cpp::jthread t{[] {}};
    EXPECT_EQ(std::hash<score::cpp::jthread::id>{}(t.get_id()), std::hash<score::cpp::jthread::id>{}(t.get_id()));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, swap_member_func_exchanges_id_and_native_handle)
{
    score::cpp::jthread t1;
    score::cpp::jthread t2{[] {}};

    const score::cpp::jthread::id id1{t1.get_id()};
    const score::cpp::jthread::id id2{t2.get_id()};

    const score::cpp::jthread::native_handle_type native_handle1{t1.native_handle()};
    const score::cpp::jthread::native_handle_type native_handle2{t2.native_handle()};

    t1.swap(t2);

    EXPECT_EQ(t1.get_id(), id2);
    EXPECT_EQ(t2.get_id(), id1);
    EXPECT_EQ(t1.native_handle(), native_handle2);
    EXPECT_EQ(t2.native_handle(), native_handle1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, swap_exchanges_id_and_native_handle)
{
    score::cpp::jthread t1;
    score::cpp::jthread t2{[] {}};

    const score::cpp::jthread::id id1{t1.get_id()};
    const score::cpp::jthread::id id2{t2.get_id()};

    const score::cpp::jthread::native_handle_type native_handle1{t1.native_handle()};
    const score::cpp::jthread::native_handle_type native_handle2{t2.native_handle()};

    using std::swap;
    swap(t1, t2);

    EXPECT_EQ(t1.get_id(), id2);
    EXPECT_EQ(t2.get_id(), id1);
    EXPECT_EQ(t1.native_handle(), native_handle2);
    EXPECT_EQ(t2.native_handle(), native_handle1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, different_threads_are_not_equal)
{
    score::cpp::jthread t1{[] {}};
    score::cpp::jthread t2{[] {}};
    EXPECT_NE(t1.get_id(), t2.get_id());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, comparison_operators)
{
    score::cpp::jthread t{[] {}};
    const score::cpp::jthread::id id{t.get_id()};

    EXPECT_EQ(score::cpp::jthread::id{}, score::cpp::jthread::id{});
    EXPECT_NE(id, score::cpp::jthread::id{});
    EXPECT_GT(id, score::cpp::jthread::id{});
    EXPECT_GE(id, score::cpp::jthread::id{});
    EXPECT_LT(score::cpp::jthread::id{}, id);
    EXPECT_LE(score::cpp::jthread::id{}, id);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8173756
TEST(jthread_test, ostream_operator)
{
    score::cpp::jthread t{[] {}};
    std::stringstream ss;
    ss << t.get_id();
    EXPECT_GT(ss.str().length(), 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#20321807
TEST(jthread_test, this_thread_get_id)
{
    score::cpp::jthread::id within_created_thread_id{};
    score::cpp::jthread::id created_thread_id{};

    {
        score::cpp::jthread t{[&within_created_thread_id] { within_created_thread_id = score::cpp::this_thread::get_id(); }};
        created_thread_id = t.get_id();
    }

    EXPECT_EQ(within_created_thread_id, created_thread_id);
}

} // namespace
