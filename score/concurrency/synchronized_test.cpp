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
#include "score/concurrency/synchronized.h"
#include "score/concurrency/test_types.h"

#include "score/quality/compiler_warnings/deprecation.h"
#include "score/quality/compiler_warnings/warnings.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <functional>
#include <future>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using score::concurrency::Synchronized;

namespace test
{

namespace
{
template <typename Sync, typename Func>
auto GetWithLockMember()
{
    using SyncType = std::remove_cv_t<std::remove_reference_t<Sync>>;

    DISABLE_WARNING_PUSH
    DISABLE_WARNING_DEPRECATED
    if constexpr (std::is_const_v<std::remove_reference_t<Sync>> ||
                  !std::is_invocable_v<Func, typename SyncType::pointer>)
    {
        using ReturnType = std::decay_t<std::invoke_result_t<Func, typename SyncType::const_pointer>>;
        using WithLockMember = ReturnType (SyncType::*)(Func&&) const;
        return static_cast<WithLockMember>(&SyncType::template WithLock<Func>);
    }
    else
    {
        using ReturnType = std::decay_t<std::invoke_result_t<Func, typename SyncType::pointer>>;
        using WithLockMember = ReturnType (SyncType::*)(Func&&);
        return static_cast<WithLockMember>(&SyncType::template WithLock<Func>);
    }
    DISABLE_WARNING_POP
}

template <typename Sync, typename Func>
decltype(auto) InvokeDeprecatedWithLock(Sync& sync, Func func)
{
    const auto with_lock = GetWithLockMember<Sync, Func>();
    return score::InvokeDeprecated(with_lock, sync, std::move(func));
}

constexpr auto clone = [](auto ptr) noexcept(std::is_nothrow_copy_constructible_v<std::decay_t<decltype(*ptr)>>) {
    static_assert(std::is_copy_constructible_v<std::decay_t<decltype(*ptr)>>,
                  "clone requires a copy-constructible type");
    return *ptr;
};

template <template <typename> class Synced, typename T>
auto clone_synced(const Synced<T>& sync)
{
    return InvokeDeprecatedWithLock(sync, clone);
}
}  // namespace

class TestStruct
{
  public:
    int value{};

    explicit TestStruct(int v) : value(v) {}

    int plus_50() const
    {
        return value + 50;
    }

    [[nodiscard]] explicit operator int() const noexcept
    {
        return value;
    }

    [[nodiscard]] explicit operator std::string() const noexcept
    {
        return std::to_string(value);
    }

    [[nodiscard]] bool operator==(const TestStruct& ts) const noexcept
    {
        return value == ts.value;
    }
    [[nodiscard]] bool operator==(int val) const noexcept
    {
        return value == val;
    }
};

class MoveOnlyObject
{
  public:
    int value;

    explicit MoveOnlyObject(int v) : value(v) {}
    ~MoveOnlyObject() = default;
    MoveOnlyObject(const MoveOnlyObject&) = delete;
    MoveOnlyObject& operator=(const MoveOnlyObject&) = delete;
    MoveOnlyObject(MoveOnlyObject&&) = default;
    MoveOnlyObject& operator=(MoveOnlyObject&&) = default;
};

class NoCopyNoMoveObject
{
  public:
    int value;

    explicit NoCopyNoMoveObject(int v) : value(v) {}
    ~NoCopyNoMoveObject() = default;
    NoCopyNoMoveObject(const NoCopyNoMoveObject&) = delete;
    NoCopyNoMoveObject& operator=(const NoCopyNoMoveObject&) = delete;
    NoCopyNoMoveObject(NoCopyNoMoveObject&&) = delete;
    NoCopyNoMoveObject& operator=(NoCopyNoMoveObject&&) = delete;
};

class DefaultConstructibleObject
{
  public:
    int value;

    DefaultConstructibleObject() : value(42) {}
    explicit DefaultConstructibleObject(int v) : value(v) {}
};

class ParameterizedObject
{
  public:
    int x, y;
    std::string name;

    ParameterizedObject(int x_val, int y_val, const std::string& n) : x(x_val), y(y_val), name(n) {}

    int x_plus_y() const
    {
        return x + y;
    }
    const std::string& GetName() const
    {
        return name;
    }
};

using Syncint_t = Synchronized<int>;
using SyncTS_t = Synchronized<TestStruct>;
using SyncMoveOnly_t = Synchronized<MoveOnlyObject>;
using SyncNoCopyNoMove_t = Synchronized<NoCopyNoMoveObject>;
using SyncDefaultConstructible_t = Synchronized<DefaultConstructibleObject>;
using SyncParameterized_t = Synchronized<ParameterizedObject>;

void set_999(SyncTS_t::pointer p) noexcept
{
    p->value = 999;
}
int get_value(SyncTS_t::const_pointer p) noexcept
{
    return p->value;
}

template <typename T>
class ConstNonConstCallable
{
  public:
    int operator()(typename Synchronized<T>::pointer ptr) const noexcept
    {
        return static_cast<int>(*ptr);
    }
    std::string operator()(typename Synchronized<T>::const_pointer ptr) const noexcept
    {
        return static_cast<std::string>(*ptr);
    }
};

class NonConstCallable
{
  public:
    template <typename Ptr>
    std::string operator()(Ptr ptr) const noexcept
    {
        return static_cast<std::string>(*ptr);
    }
};

TEST(SynchronizedTest, TestSynchronizedWrapperTemplate)
{
    Syncint_t sync_value(42);

    EXPECT_EQ(InvokeDeprecatedWithLock(sync_value, clone), 42);

    InvokeDeprecatedWithLock(sync_value, [](Syncint_t::pointer ptr) noexcept {
        *ptr = 100;
    });

    EXPECT_EQ(InvokeDeprecatedWithLock(sync_value, clone), 100);
}

// Test for thread safety with multiple threads accessing a synchronized wrapper
TEST(SynchronizedTest, TestConcurrency)
{
    Syncint_t counter(0);
    const int num_tasks = 10;
    const int increments_per_task = 1000;

    auto launch_task = [&counter]() {
        return std::async(std::launch::async, [&counter]() noexcept {
            for (int i = 0; i < increments_per_task; ++i)
            {
                InvokeDeprecatedWithLock(counter, [](Syncint_t::pointer ptr) noexcept {
                    ++(*ptr);
                });
            }
        });
    };

    std::vector<std::future<void>> futures;
    futures.reserve(num_tasks);
    std::generate_n(std::back_inserter(futures), num_tasks, launch_task);

    for (auto& future : futures)
    {
        future.wait();
    }

    EXPECT_EQ(InvokeDeprecatedWithLock(counter, clone), num_tasks * increments_per_task);
}

TEST(SynchronizedTest, LockMethodBasic)
{
    Synchronized<int> sync_int(42);

    {
        auto locked_ptr = sync_int.lock();
        EXPECT_EQ(*locked_ptr, 42);
        *locked_ptr = 100;
        EXPECT_EQ(*locked_ptr, 100);
    }

    EXPECT_EQ(InvokeDeprecatedWithLock(sync_int, clone), 100);
}

TEST(SynchronizedTest, TestStructLockReturnValue)
{
    Synchronized<TestStruct> sync_struct(42);

    {
        auto locked_ptr = sync_struct.lock();
        ASSERT_TRUE(locked_ptr);
        EXPECT_EQ(locked_ptr->value, 42);

        locked_ptr->value = 100;
        EXPECT_EQ(locked_ptr->value, 100);

        EXPECT_EQ((*locked_ptr).value, 100);
        EXPECT_EQ((*locked_ptr).plus_50(), 150);
    }
}

TEST(SynchronizedTest, TestStructConstLockBehavior)
{
    const SyncTS_t const_sync_struct(42);

    {
        auto const_locked_ptr = const_sync_struct.lock();
        EXPECT_EQ(const_locked_ptr->value, 42);
        EXPECT_EQ((*const_locked_ptr).value, 42);
        EXPECT_EQ((*const_locked_ptr).plus_50(), 92);
    }

    EXPECT_EQ(InvokeDeprecatedWithLock(const_sync_struct, clone), 42);
}

TEST(SynchronizedTest, TestStructWithLockVariations)
{
    SyncTS_t sync_struct(TestStruct{42});

    InvokeDeprecatedWithLock(sync_struct, [](SyncTS_t::pointer s) noexcept {
        s->value = 100;
    });

    EXPECT_EQ(InvokeDeprecatedWithLock(sync_struct, clone), 100);

    InvokeDeprecatedWithLock(sync_struct, [](SyncTS_t::pointer s) noexcept {
        s->value = s->value * 2;
    });

    EXPECT_EQ(InvokeDeprecatedWithLock(sync_struct, clone), 200);
}

TEST(SynchronizedTest, TestStructMemberFunctionPointers)
{
    SyncTS_t sync_struct(TestStruct{42});
    EXPECT_EQ(InvokeDeprecatedWithLock(sync_struct, std::mem_fn(&TestStruct::plus_50)), 92);
    EXPECT_EQ(InvokeDeprecatedWithLock(sync_struct, &TestStruct::plus_50), 92);

    const SyncTS_t sync_const(42);
    EXPECT_EQ(InvokeDeprecatedWithLock(sync_const, std::mem_fn(&TestStruct::plus_50)), 92);
    EXPECT_EQ(InvokeDeprecatedWithLock(sync_const, &TestStruct::plus_50), 92);
}

TEST(SynchronizedTest, TestStructFreeFunctionPointers)
{
    SyncTS_t sync_struct(TestStruct{42});

    InvokeDeprecatedWithLock(sync_struct, &set_999);

    EXPECT_EQ(InvokeDeprecatedWithLock(sync_struct, &get_value), 999);
}

TEST(SynchronizedTest, TestStructLockNonConst)
{
    SyncTS_t sync_struct(TestStruct{42});

    {
        auto locked_ptr = sync_struct.lock();
        locked_ptr->value = 77;
        EXPECT_EQ(locked_ptr->value, 77);
    }

    EXPECT_EQ(InvokeDeprecatedWithLock(sync_struct, clone), 77);
}

TEST(SynchronizedTest, TestStructLockConst)
{
    const SyncTS_t const_sync_struct(TestStruct{42});

    {
        auto const_locked_ptr = const_sync_struct.lock();
        EXPECT_EQ(const_locked_ptr->value, 42);
    }

    EXPECT_EQ(InvokeDeprecatedWithLock(const_sync_struct, clone), 42);
}

TEST(SynchronizedTest, TestStructLambdaCaptureModes)
{
    SyncTS_t sync_struct(TestStruct{42});

    int external_value = 10;

    // Test lambda with value capture
    InvokeDeprecatedWithLock(sync_struct, [external_value](SyncTS_t::pointer s) noexcept {
        s->value = external_value * 5;
    });

    auto result = InvokeDeprecatedWithLock(sync_struct, [](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(result, 50);

    // Test lambda with reference capture
    int multiplier = 4;
    InvokeDeprecatedWithLock(sync_struct, [&multiplier](SyncTS_t::pointer s) noexcept {
        s->value = s->value * multiplier;
    });

    EXPECT_EQ(InvokeDeprecatedWithLock(sync_struct, clone), 200);
}

TEST(SynchronizedTest, TestStructVoidReturningLambdas)
{
    SyncTS_t sync_struct(TestStruct{42});

    InvokeDeprecatedWithLock(sync_struct, [](SyncTS_t::pointer s) noexcept -> void {
        s->value = 123;
    });

    EXPECT_EQ(InvokeDeprecatedWithLock(sync_struct, clone), 123);
}

TEST(SynchronizedTest, TestStructCopyVsMoveSemantics)
{
    // Test construction from temporary (move semantics)
    SyncTS_t sync_struct1(TestStruct{42});
    auto val = InvokeDeprecatedWithLock(sync_struct1, [](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(val, 42);

    // Test construction from lvalue (copy semantics)
    TestStruct temp_struct{55};
    SyncTS_t sync_struct2(temp_struct);
    auto val1 = InvokeDeprecatedWithLock(sync_struct2, [](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(val1, 55);

    // Test in-place construction
    SyncTS_t sync_struct3(TestStruct{77});
    auto val2 = InvokeDeprecatedWithLock(sync_struct3, [](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(val2, 77);
}

TEST(SynchronizedTest, TestMoveOnlyObject)
{
    SyncMoveOnly_t sync_obj(150);

    auto result = InvokeDeprecatedWithLock(sync_obj, [](SyncMoveOnly_t::const_pointer ptr) noexcept {
        return ptr->value;
    });
    EXPECT_EQ(result, 150);

    InvokeDeprecatedWithLock(sync_obj, [](SyncMoveOnly_t::pointer ptr) noexcept {
        ptr->value = 300;
    });

    auto val = InvokeDeprecatedWithLock(sync_obj, [](SyncMoveOnly_t::const_pointer ptr) noexcept {
        return ptr->value;
    });
    EXPECT_EQ(val, 300);
}

TEST(SynchronizedTest, TestNoCopyNoMoveObject)
{
    SyncNoCopyNoMove_t sync_obj(250);

    auto result = InvokeDeprecatedWithLock(sync_obj, [](SyncNoCopyNoMove_t::const_pointer ptr) noexcept {
        return ptr->value;
    });
    EXPECT_EQ(result, 250);

    InvokeDeprecatedWithLock(sync_obj, [](SyncNoCopyNoMove_t::pointer ptr) noexcept {
        ptr->value = 500;
    });

    auto val = InvokeDeprecatedWithLock(sync_obj, [](SyncNoCopyNoMove_t::const_pointer ptr) noexcept {
        return ptr->value;
    });
    EXPECT_EQ(val, 500);
}

TEST(SynchronizedTest, TestDefaultConstructedObject)
{
    SyncDefaultConstructible_t sync_obj{};

    auto result = InvokeDeprecatedWithLock(sync_obj, [](SyncDefaultConstructible_t::const_pointer ptr) noexcept {
        return ptr->value;
    });
    EXPECT_EQ(result, 42);

    InvokeDeprecatedWithLock(sync_obj, [](SyncDefaultConstructible_t::pointer ptr) noexcept {
        ptr->value = 100;
    });

    auto val = InvokeDeprecatedWithLock(sync_obj, [](SyncDefaultConstructible_t::const_pointer ptr) noexcept {
        return ptr->value;
    });
    EXPECT_EQ(val, 100);
}

TEST(SynchronizedTest, TestParameterizedConstruction)
{
    SyncParameterized_t sync_obj(10, 20, "test_object");

    auto result = InvokeDeprecatedWithLock(sync_obj, [](SyncParameterized_t::const_pointer ptr) noexcept {
        return ptr->x_plus_y();
    });
    EXPECT_EQ(result, 30);

    auto name = InvokeDeprecatedWithLock(sync_obj, [](SyncParameterized_t::const_pointer ptr) noexcept {
        return ptr->GetName();
    });
    EXPECT_EQ(name, "test_object");

    InvokeDeprecatedWithLock(sync_obj, [](SyncParameterized_t::pointer ptr) noexcept {
        ptr->x = 15;
        ptr->y = 25;
    });

    result = InvokeDeprecatedWithLock(sync_obj, [](SyncParameterized_t::const_pointer ptr) noexcept {
        return ptr->x_plus_y();
    });
    EXPECT_EQ(result, 40);
}

TEST(SynchronizedTest, TestCtadConstruction)
{
    std::vector reference_vec{"one", "two", "three"};
    Synchronized<std::vector<std::string>> sync_vec_ctad(begin(reference_vec), end(reference_vec));
    EXPECT_THAT(clone_synced(sync_vec_ctad), ::testing::ElementsAreArray(reference_vec));
}

TEST(SynchronizedTest, TestInitializerListConstruction)
{
    using namespace std::string_literals;

    Synchronized<std::vector<int>> sync_vi{1, 2, 3, 4, 5};
    EXPECT_THAT(clone_synced(sync_vi), ::testing::ElementsAre(1, 2, 3, 4, 5));

    Synchronized<std::vector<std::string>> sync_vs{"un"s, "deux"s, "trois"s};
    EXPECT_THAT(clone_synced(sync_vs), ::testing::ElementsAre("un"s, "deux"s, "trois"s));

    std::initializer_list<std::string> il_str = {"ten"s, "twenty"s, "thirty"s};
    Synchronized<std::vector<std::string>> sync_vs2(il_str);
    EXPECT_THAT(clone_synced(sync_vs2), ::testing::ElementsAreArray(il_str));

    Synchronized<std::string> sync_str{'H', 'e', 'l', 'l', 'o'};
    EXPECT_EQ(clone_synced(sync_str), "Hello");
}

TEST(SynchronizedTest, TestPiecewiseConstruction)
{
    using namespace std::string_literals;

    Synchronized<std::string, MockMutexParameterized> sync_piecewise(
        std::piecewise_construct,
        std::forward_as_tuple(5, 'Z'),                      // string args
        std::forward_as_tuple("Literal for mutex"s, 100));  // MockMutexParameterized args

    EXPECT_EQ(InvokeDeprecatedWithLock(sync_piecewise, clone), "ZZZZZ");
}

TEST(SynchronizedTest, TestConstOperations)
{
    const SyncTS_t const_sync_struct(TestStruct{42});

    auto result = InvokeDeprecatedWithLock(const_sync_struct, [](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(result, 42);

    {
        auto const_locked = const_sync_struct.lock();
        EXPECT_EQ(const_locked->value, 42);
    }
}

TEST(SynchronizedTest, LockAndWithLock)
{
    SyncTS_t sync_struct(TestStruct{42});

    {
        auto locked_ptr = sync_struct.lock();
        locked_ptr->value = 150;
        EXPECT_EQ(locked_ptr->value, 150);
    }

    InvokeDeprecatedWithLock(sync_struct, [](SyncTS_t::pointer s) noexcept {
        s->value = 200;
    });

    auto result = InvokeDeprecatedWithLock(sync_struct, [](SyncTS_t::const_pointer s) {
        return s->plus_50();
    });
    EXPECT_EQ(result, 250);
    auto val1 = InvokeDeprecatedWithLock(sync_struct, [](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(val1, 200);

    EXPECT_NO_THROW({
        InvokeDeprecatedWithLock(sync_struct, [](SyncTS_t::const_pointer s) noexcept {
            EXPECT_EQ(s->value, 200);
        });
    });

    auto val2 = InvokeDeprecatedWithLock(sync_struct, [](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(val2, 200);
}

TEST(SynchronizedTest, ConstNonConstCallableTest)
{
    {
        Synchronized<TestStruct> sync(TestStruct{42});
        EXPECT_EQ(InvokeDeprecatedWithLock(sync, ConstNonConstCallable<TestStruct>{}), 42);
        EXPECT_EQ(InvokeDeprecatedWithLock(sync, NonConstCallable{}), "42");

        ConstNonConstCallable<TestStruct> cnc_callable;
        EXPECT_EQ(InvokeDeprecatedWithLock(sync, cnc_callable), 42);

        NonConstCallable nc_callable;
        EXPECT_EQ(InvokeDeprecatedWithLock(sync, nc_callable), "42");
    }

    {
        const SyncTS_t const_sync(TestStruct{42});
        EXPECT_EQ(InvokeDeprecatedWithLock(const_sync, ConstNonConstCallable<TestStruct>{}), "42");
        EXPECT_EQ(InvokeDeprecatedWithLock(const_sync, NonConstCallable{}), "42");

        ConstNonConstCallable<TestStruct> cnc_callable;
        EXPECT_EQ(InvokeDeprecatedWithLock(const_sync, cnc_callable), "42");

        NonConstCallable nc_callable;
        EXPECT_EQ(InvokeDeprecatedWithLock(const_sync, nc_callable), "42");
    }
}

TEST(SynchronizedTest, TestStructExceptionSafetyDetailed)
{
    SyncTS_t sync_struct(TestStruct{42});

    InvokeDeprecatedWithLock(sync_struct, [](SyncTS_t::pointer s) noexcept {
        s->value = 100;
    });

    EXPECT_NO_THROW({
        InvokeDeprecatedWithLock(sync_struct, [](SyncTS_t::pointer s) noexcept {
            s->value = 999;
            EXPECT_EQ(s->value, 999);
        });
    });

    auto val = InvokeDeprecatedWithLock(sync_struct, [](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(val, 999);

    InvokeDeprecatedWithLock(sync_struct, [](SyncTS_t::pointer s) noexcept {
        s->value = 200;
    });
    auto val2 = InvokeDeprecatedWithLock(sync_struct, [](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(val2, 200);
}

}  // namespace test
