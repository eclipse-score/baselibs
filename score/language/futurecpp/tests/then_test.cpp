/********************************************************************************
 * Copyright (c) 2024 Contributors to the Eclipse Foundation
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

///
/// @file
/// @copyright Copyright (c) 2024 Contributors to the Eclipse Foundation
///

#include <score/execution.hpp>
#include <score/execution.hpp> // test include guard

#include <score/type_traits.hpp>

#include <gtest/gtest.h>

namespace score::cpp
{
namespace execution
{
namespace
{

enum class category
{
    init,
    lvalue,
    rvalue,
    const_lvalue,
    const_rvalue,
};

struct void_invocable
{
    void operator()() & { *c = category::lvalue; }
    void operator()() const& { *c = category::const_lvalue; }
    void operator()() && { *c = category::rvalue; }
    void operator()() const&& { *c = category::const_rvalue; }

    category* c;
};

struct non_void_invocable
{
    category operator()() & { return category::lvalue; }
    category operator()() const& { return category::const_lvalue; }
    category operator()() && { return category::rvalue; }
    category operator()() const&& { return category::const_rvalue; }

    category operator()(const category c) &
    {
        EXPECT_EQ(c, category::lvalue);
        return category::lvalue;
    }
    category operator()(const category c) const&
    {
        EXPECT_EQ(c, category::const_lvalue);
        return category::const_lvalue;
    }
    category operator()(const category c) &&
    {
        EXPECT_EQ(c, category::rvalue);
        return category::rvalue;
    }
    category operator()(const category c) const&&
    {
        EXPECT_EQ(c, category::const_rvalue);
        return category::const_rvalue;
    }
};

struct category_receiver
{
    using receiver_concept = receiver_t;

    void set_value() & { *value = category::lvalue; }
    void set_value() && { *value = category::rvalue; }

    void set_value(const category c) &
    {
        EXPECT_EQ(c, category::lvalue);
        *value = c;
    }
    void set_value(const category c) &&
    {
        EXPECT_EQ(c, category::rvalue);
        *value = c;
    }

    void set_stopped() & { *stopped = category::lvalue; }
    void set_stopped() && { *stopped = category::rvalue; }

    category* value;
    category* stopped;
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(then_receiver, set_value_GivenVoidInvocable_ExpectCorrectValueCategory)
{
    category invocable_action{category::init};
    category set_value_action{category::init};
    category set_stopped_action{category::init};

    using receiver_t = score::cpp::execution::detail::then_receiver<category_receiver, void_invocable>;
    receiver_t recv{category_receiver{&set_value_action, &set_stopped_action}, void_invocable{&invocable_action}};

    set_value(std::move(recv));

    EXPECT_EQ(invocable_action, category::rvalue);
    EXPECT_EQ(set_value_action, category::rvalue);
    EXPECT_EQ(set_stopped_action, category::init);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(then_receiver, set_value_GivenNonVoidInvocable_ExpectCorrectValueCategory)
{
    category set_value_action{category::init};
    category set_stopped_action{category::init};

    using receiver_t = score::cpp::execution::detail::then_receiver<category_receiver, non_void_invocable>;
    receiver_t recv{category_receiver{&set_value_action, &set_stopped_action}, non_void_invocable{}};

    set_value(std::move(recv));

    EXPECT_EQ(set_value_action, category::rvalue);
    EXPECT_EQ(set_stopped_action, category::init);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(then_receiver, set_stopped_GivenReceiverIsStopped_ExpectInvocableNotCalled)
{
    category invocable_action{category::init};
    category set_value_action{category::init};
    category set_stopped_action{category::init};

    using receiver_t = score::cpp::execution::detail::then_receiver<category_receiver, void_invocable>;
    receiver_t recv{category_receiver{&set_value_action, &set_stopped_action}, void_invocable{&invocable_action}};

    set_stopped(std::move(recv));

    EXPECT_EQ(invocable_action, category::init);
    EXPECT_EQ(set_value_action, category::init);
    EXPECT_EQ(set_stopped_action, category::rvalue);
}

template <typename Receiver>
struct inline_scheduler_op_state
{
    static_assert(std::is_object<Receiver>::value, "receiver is not an object type");
    static_assert(is_receiver<Receiver>::value, "not a receiver");

    using operation_state_concept = operation_state_t;

    void start() & { set_value(std::move(r)); }
    void start() && = delete;

    Receiver r;
};

struct inline_scheduler_sender
{
    using sender_concept = sender_t;

    template <typename Receiver>
    inline_scheduler_op_state<score::cpp::remove_cvref_t<Receiver>> connect(Receiver&& r) const
    {
        static_assert(is_receiver<Receiver>::value, "not a receiver");
        return {std::forward<Receiver>(r)};
    }
};

template <typename T>
struct get_result_receiver
{
    using receiver_concept = receiver_t;

    void set_value(const T v) & = delete;
    void set_value(const T v) && { *value = v; }

    T* value;
};

template <typename R, typename Sender>
R get_result(Sender&& sender)
{
    static_assert(is_sender<Sender>::value, "not a sender");
    static_assert(!std::is_same<void, R>::value, "must not be void");

    R result{};
    auto op = connect(std::forward<Sender>(sender), get_result_receiver<R>{&result});
    start(op);
    return result;
}

template <typename Invocable>
struct non_copyable
{
public:
    non_copyable() = default;
    non_copyable(const non_copyable&) = delete;
    non_copyable& operator=(const non_copyable&) = delete;
    non_copyable(non_copyable&&) = default;
    non_copyable& operator=(non_copyable&&) = default;
    ~non_copyable() = default;

    template <typename... U>
    auto operator()(U&&... args) &
    {
        return i_(std::forward<U>(args)...);
    }
    template <typename... U>
    auto operator()(U&&... args) &&
    {
        return std::move(i_)(std::forward<U>(args)...);
    }
    template <typename... U>
    auto operator()(U&&... args) const&
    {
        return i_(std::forward<U>(args)...);
    }
    template <typename... U>
    auto operator()(U&&... args) const&&
    {
        return std::move(i_)(std::forward<U>(args)...);
    }

private:
    Invocable i_{};
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(then, then_GivenCompositionWithNamedTemporaries_ExpectChainedResult)
{
    auto s1 = then(inline_scheduler_sender{}, non_void_invocable{});
    auto s2 = then(s1, non_void_invocable{});
    EXPECT_EQ(category::rvalue, get_result<category>(s2));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(then, then_GivenCompositionWithNestedFunctionCalls_ExpectChainedResult)
{
    auto s = then(then(inline_scheduler_sender{}, non_void_invocable{}), non_void_invocable{});
    EXPECT_EQ(category::rvalue, get_result<category>(s));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(then, then_GivenCompositionWithNestedFunctionCallsAndNonCopyableInvocable_ExpectChainedResult)
{
    auto s =
        then(then(inline_scheduler_sender{}, non_copyable<non_void_invocable>{}), non_copyable<non_void_invocable>{});
    EXPECT_EQ(category::rvalue, get_result<category>(std::move(s)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(then, then_GivenCompositionWithPipedNamedTemporaries_ExpectChainedResult)
{
    auto s1 = then(non_void_invocable{});
    auto s2 = then(non_void_invocable{});
    auto s3 = inline_scheduler_sender{} | s1 | s2;
    EXPECT_EQ(category::rvalue, get_result<category>(s3));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(then, then_GivenCompositionWithPipe_ExpectChainedResult)
{
    auto s = inline_scheduler_sender{} | then(non_void_invocable{}) | then(non_void_invocable{});
    EXPECT_EQ(category::rvalue, get_result<category>(s));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(then, then_GivenCompositionWithPipeAndNonCopyableInvocable_ExpectChainedResult)
{
    auto s =
        inline_scheduler_sender{} | then(non_copyable<non_void_invocable>{}) | then(non_copyable<non_void_invocable>{});
    EXPECT_EQ(category::rvalue, get_result<category>(std::move(s)));
}

struct argument_category
{
    category& operator()(category& c)
    {
        EXPECT_EQ(c, category::lvalue);
        return c;
    }
    const category& operator()(const category& c)
    {
        EXPECT_EQ(c, category::const_lvalue);
        return c;
    }
    category&& operator()(category&& c)
    {
        EXPECT_EQ(c, category::rvalue);
        return std::move(c);
    }
    const category&& operator()(const category&& c)
    {
        EXPECT_EQ(c, category::const_rvalue);
        return std::move(c);
    }
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(then, then_GivenInvocableReturnsLValue_ExpectArgumentIsPerfectlyForwarded)
{
    auto s1 = then(inline_scheduler_sender{}, [c = category::lvalue]() mutable -> category& { return c; });
    auto s2 = then(s1, argument_category{});
    EXPECT_EQ(category::lvalue, get_result<category>(s2));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(then, then_GivenInvocableReturnsConstLValue_ExpectArgumentIsPerfectlyForwarded)
{
    auto s1 = then(inline_scheduler_sender{}, [c = category::const_lvalue]() -> const category& { return c; });
    auto s2 = then(s1, argument_category{});
    EXPECT_EQ(category::const_lvalue, get_result<category>(s2));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(then, then_GivenInvocableReturnsRValue_ExpectArgumentIsPerfectlyForwarded)
{
    auto s1 = then(inline_scheduler_sender{}, [c = category::rvalue]() mutable -> category&& { return std::move(c); });
    auto s2 = then(s1, argument_category{});
    EXPECT_EQ(category::rvalue, get_result<category>(s2));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(then, then_GivenInvocableReturnsConstRValue_ExpectArgumentIsPerfectlyForwarded)
{
    auto s1 =
        then(inline_scheduler_sender{}, [c = category::const_rvalue]() -> const category&& { return std::move(c); });
    auto s2 = then(s1, argument_category{});
    EXPECT_EQ(category::const_rvalue, get_result<category>(s2));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(then, then_GivenMultipleInvocationsOnSameSender_ExpectSameResultBecauseSenderIsCopied)
{
    auto invocable = [v = 0]() mutable {
        v += 1;
        return v;
    };

    EXPECT_EQ(1, invocable());
    EXPECT_EQ(2, invocable());

    auto s = then(inline_scheduler_sender{}, invocable);
    EXPECT_EQ(3, get_result<std::int32_t>(s));
    EXPECT_EQ(3, get_result<std::int32_t>(s));
    EXPECT_EQ(3, get_result<std::int32_t>(s));
}

} // namespace
} // namespace execution
} // namespace score::cpp
