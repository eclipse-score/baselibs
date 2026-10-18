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

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <algorithm>
#include <future>
#include <string>
#include <utility>
#include <vector>

#include <score/optional.hpp>

using score::concurrency::Synchronized;

namespace test
{

namespace
{
constexpr auto clone_opt = [](auto ptr) noexcept(std::is_nothrow_copy_constructible_v<std::decay_t<decltype(*ptr)>>) {
    using ValueType = std::decay_t<decltype(*ptr)>;
    static_assert(std::is_copy_constructible_v<ValueType>, "clone_opt requires a copy-constructible type");
    if (ptr != nullptr)
    {
        return score::cpp::optional<ValueType>{*ptr};
    }
    return score::cpp::optional<ValueType>{};
};

template <typename T, typename Lockable>
auto clone_opt_synced(const score::concurrency::Synchronized<T, Lockable>& sync)
{
    return sync.lock().and_then(clone_opt);
}

// Precondition: sync.lock() returns a non-null pointer.
template <typename T, typename Lockable>
auto clone_synced(const score::concurrency::Synchronized<T, Lockable>& sync)
{
    return clone_opt_synced(sync).value();
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

    TestStruct& operator++() noexcept
    {
        ++value;
        return *this;
    }

    TestStruct operator++(int) noexcept
    {
        TestStruct previous{*this};
        ++value;
        return previous;
    }

    TestStruct& operator--() noexcept
    {
        --value;
        return *this;
    }

    TestStruct operator--(int) noexcept
    {
        TestStruct previous{*this};
        --value;
        return previous;
    }

    TestStruct& operator=(const TestStruct&) = default;

    TestStruct& operator=(int assigned_value) noexcept
    {
        value = assigned_value;
        return *this;
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

template <int value_to_set, typename Pointer>
int set_and_ret_val(Pointer p) noexcept
{
    if constexpr (std::is_assignable_v<decltype(*p), int>)
    {
        *p = value_to_set;
        return static_cast<int>(*p);
    }
    else
    {
        p->value = value_to_set;
        return static_cast<int>(p->value);
    }
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

    EXPECT_EQ(sync_value.lock().and_then(clone_opt).value(), 42);

    EXPECT_EQ(sync_value.lock().transform(set_and_ret_val<100, Syncint_t::pointer>).value(), 100);

    EXPECT_EQ(sync_value.lock().and_then(clone_opt).value(), 100);
}

// Test for thread safety with multiple threads accessing a synchronized wrapper
TEST(SynchronizedTest, TestConcurrency)
{
    Syncint_t counter(0);
    const int num_tasks = 10;
    const int increments_per_task = 1000;

    auto launch_task = [&counter]() {
        return std::async(std::launch::async, [&counter]() {
            const auto incr_val = [](Syncint_t::pointer ptr) noexcept -> score::cpp::optional<int> {
                return ++(*ptr);
            };

            for (int i = 0; i < increments_per_task; ++i)
            {
                EXPECT_TRUE(counter.lock().and_then(incr_val)) << "Locking and incrementing the value should not fail";
            }
            return increments_per_task;
        });
    };

    std::vector<std::future<int>> futures;
    futures.reserve(num_tasks);
    std::generate_n(std::back_inserter(futures), num_tasks, launch_task);

    for (auto& future : futures)
    {
        EXPECT_EQ(future.get(), increments_per_task);
    }

    EXPECT_EQ(counter.lock().and_then(clone_opt).value(), num_tasks * increments_per_task);
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

    EXPECT_EQ(sync_int.lock().and_then(clone_opt).value(), 100);
}

TEST(SynchronizedTest, TestStructLockReturnValue)
{
    Synchronized<TestStruct> sync_struct(42);

    {
        auto locked_ptr = sync_struct.lock();
        ASSERT_TRUE(locked_ptr);
        EXPECT_EQ(locked_ptr->value, 42);

        *locked_ptr = 100;
        EXPECT_EQ(locked_ptr->value, 100);

        *locked_ptr = TestStruct{42};
        EXPECT_EQ(locked_ptr->value, 42);

        const auto post_increment_value = (*locked_ptr)++;
        EXPECT_EQ(post_increment_value.value, 42);
        EXPECT_EQ(locked_ptr->value, 43);

        EXPECT_EQ((++(*locked_ptr)).value, 44);
        EXPECT_EQ(locked_ptr->value, 44);

        const auto post_decrement_value = (*locked_ptr)--;
        EXPECT_EQ(post_decrement_value.value, 44);
        EXPECT_EQ(locked_ptr->value, 43);

        EXPECT_EQ((--(*locked_ptr)).value, 42);
        EXPECT_EQ(locked_ptr->value, 42);
        EXPECT_EQ((*locked_ptr).plus_50(), 92);
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

    EXPECT_EQ(const_sync_struct.lock().and_then(clone_opt).value(), 42);
}

TEST(SynchronizedTest, TestStructTransformVariations)
{
    SyncTS_t sync_struct(TestStruct{42});

    EXPECT_EQ(sync_struct.lock().transform(set_and_ret_val<100, SyncTS_t::pointer>).value(), 100);

    EXPECT_EQ(sync_struct.lock().and_then(clone_opt).value(), 100);

    EXPECT_TRUE(sync_struct.lock()
                    .transform([](SyncTS_t::pointer s) noexcept {
                        s->value = s->value * 2;
                        return true;
                    })
                    .value());

    EXPECT_EQ(sync_struct.lock().and_then(clone_opt).value(), 200);
}

TEST(SynchronizedTest, TestStructMemberFunctionPointers)
{
    SyncTS_t sync_struct(TestStruct{42});
    EXPECT_EQ(sync_struct.lock().transform(std::mem_fn(&TestStruct::plus_50)).value(), 92);
    EXPECT_EQ(sync_struct.lock().transform(&TestStruct::plus_50).value(), 92);

    const SyncTS_t sync_const(42);
    EXPECT_EQ(sync_const.lock().transform(std::mem_fn(&TestStruct::plus_50)).value(), 92);
    EXPECT_EQ(sync_const.lock().transform(&TestStruct::plus_50).value(), 92);
}

TEST(SynchronizedTest, TestStructFreeFunctionPointers)
{
    SyncTS_t sync_struct(TestStruct{42});

    EXPECT_EQ(sync_struct.lock().transform(&set_and_ret_val<999, SyncTS_t::pointer>).value(), 999);

    EXPECT_EQ(std::as_const(sync_struct).lock().transform(&get_value).value(), 999);
}

TEST(SynchronizedTest, TestStructLockNonConst)
{
    SyncTS_t sync_struct(TestStruct{42});

    {
        auto locked_ptr = sync_struct.lock();
        locked_ptr->value = 77;
        EXPECT_EQ(locked_ptr->value, 77);
    }

    EXPECT_EQ(sync_struct.lock().and_then(clone_opt).value(), 77);
}

TEST(SynchronizedTest, TestStructLockConst)
{
    const SyncTS_t const_sync_struct(TestStruct{42});

    {
        auto const_locked_ptr = const_sync_struct.lock();
        EXPECT_EQ(const_locked_ptr->value, 42);
    }

    EXPECT_EQ(const_sync_struct.lock().and_then(clone_opt).value(), 42);
}

TEST(SynchronizedTest, TestStructLambdaCaptureModes)
{
    SyncTS_t sync_struct(TestStruct{42});

    // Test lambda with value capture
    EXPECT_EQ(sync_struct.lock().transform(set_and_ret_val<50, SyncTS_t::pointer>).value(), 50);

    EXPECT_EQ(std::as_const(sync_struct).lock().and_then(clone_opt).value().value, 50);

    // Test lambda with reference capture
    int multiplier = 4;
    EXPECT_EQ(sync_struct.lock()
                  .transform([&multiplier](SyncTS_t::pointer s) noexcept {
                      s->value = s->value * multiplier;
                      return s->value;
                  })
                  .value(),
              200);

    EXPECT_EQ(sync_struct.lock().and_then(clone_opt).value(), 200);
}

TEST(SynchronizedTest, TestStructVoidReturningLambdas)
{
    SyncTS_t sync_struct(TestStruct{42});

    EXPECT_EQ(sync_struct.lock().transform(set_and_ret_val<123, SyncTS_t::pointer>).value(), 123);

    EXPECT_EQ(sync_struct.lock().and_then(clone_opt).value(), 123);
}

TEST(SynchronizedTest, TestStructCopyVsMoveSemantics)
{
    // Test construction from temporary (move semantics)
    SyncTS_t sync_struct1(TestStruct{42});
    auto val = std::as_const(sync_struct1).lock().transform([](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(val.value(), 42);

    // Test construction from lvalue (copy semantics)
    TestStruct temp_struct{55};
    SyncTS_t sync_struct2(temp_struct);
    auto val1 = std::as_const(sync_struct2).lock().transform([](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(val1.value(), 55);

    // Test in-place construction
    SyncTS_t sync_struct3(TestStruct{77});
    auto val2 = std::as_const(sync_struct3).lock().transform([](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(val2.value(), 77);
}

TEST(SynchronizedTest, TestMoveOnlyObject)
{
    SyncMoveOnly_t sync_obj(150);

    auto result = std::as_const(sync_obj).lock().transform([](SyncMoveOnly_t::const_pointer ptr) noexcept {
        return ptr->value;
    });
    EXPECT_EQ(result.value(), 150);

    EXPECT_EQ(sync_obj.lock().transform(set_and_ret_val<300, SyncMoveOnly_t::pointer>).value(), 300);

    auto val = std::as_const(sync_obj).lock().transform([](SyncMoveOnly_t::const_pointer ptr) noexcept {
        return ptr->value;
    });
    EXPECT_EQ(val.value(), 300);
}

TEST(SynchronizedTest, TestNoCopyNoMoveObject)
{
    SyncNoCopyNoMove_t sync_obj(250);

    auto result = std::as_const(sync_obj).lock().transform([](SyncNoCopyNoMove_t::const_pointer ptr) noexcept {
        return ptr->value;
    });
    EXPECT_EQ(result.value(), 250);

    EXPECT_EQ(sync_obj.lock().transform(set_and_ret_val<500, SyncNoCopyNoMove_t::pointer>).value(), 500);

    auto val = std::as_const(sync_obj).lock().transform([](SyncNoCopyNoMove_t::const_pointer ptr) noexcept {
        return ptr->value;
    });
    EXPECT_EQ(val.value(), 500);
}

TEST(SynchronizedTest, TestDefaultConstructedObject)
{
    SyncDefaultConstructible_t sync_obj{};

    auto result = std::as_const(sync_obj).lock().transform([](SyncDefaultConstructible_t::const_pointer ptr) noexcept {
        return ptr->value;
    });
    EXPECT_EQ(result.value(), 42);

    EXPECT_EQ(sync_obj.lock().transform(set_and_ret_val<100, SyncDefaultConstructible_t::pointer>).value(), 100);

    auto val = std::as_const(sync_obj).lock().transform([](SyncDefaultConstructible_t::const_pointer ptr) noexcept {
        return ptr->value;
    });
    EXPECT_EQ(val.value(), 100);
}

TEST(SynchronizedTest, TestParameterizedConstruction)
{
    SyncParameterized_t sync_obj(10, 20, "test_object");

    auto result = std::as_const(sync_obj).lock().transform([](SyncParameterized_t::const_pointer ptr) noexcept {
        return ptr->x_plus_y();
    });
    EXPECT_EQ(result.value(), 30);

    auto name = std::as_const(sync_obj).lock().transform([](SyncParameterized_t::const_pointer ptr) noexcept {
        return std::string{ptr->GetName()};
    });
    EXPECT_EQ(name.value(), "test_object");

    EXPECT_TRUE(sync_obj.lock()
                    .transform([](SyncParameterized_t::pointer ptr) noexcept {
                        ptr->x = 15;
                        ptr->y = 25;
                        return true;
                    })
                    .value());

    result = std::as_const(sync_obj).lock().transform([](SyncParameterized_t::const_pointer ptr) noexcept {
        return ptr->x_plus_y();
    });
    EXPECT_EQ(result.value(), 40);
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

    EXPECT_EQ(sync_piecewise.lock().and_then(clone_opt).value(), "ZZZZZ");
}

TEST(SynchronizedTest, TestConstOperations)
{
    const SyncTS_t const_sync_struct(TestStruct{42});

    auto result = const_sync_struct.lock().transform([](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(result.value(), 42);

    {
        auto const_locked = const_sync_struct.lock();
        EXPECT_EQ(const_locked->value, 42);
    }
}

TEST(SynchronizedTest, LockAndTransform)
{
    SyncTS_t sync_struct(TestStruct{42});

    {
        auto locked_ptr = sync_struct.lock();
        locked_ptr->value = 150;
        EXPECT_EQ(locked_ptr->value, 150);
    }

    EXPECT_EQ(sync_struct.lock().transform(set_and_ret_val<200, SyncTS_t::pointer>).value(), 200);

    auto result = std::as_const(sync_struct).lock().transform([](SyncTS_t::const_pointer s) {
        return s->plus_50();
    });
    EXPECT_EQ(result.value(), 250);
    auto val1 = std::as_const(sync_struct).lock().transform([](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(val1.value(), 200);

    EXPECT_NO_THROW({
        auto callback_result = std::as_const(sync_struct).lock().transform([](SyncTS_t::const_pointer s) noexcept {
            EXPECT_EQ(s->value, 200);
            return true;
        });
        EXPECT_TRUE(callback_result.value());
    });

    auto val2 = std::as_const(sync_struct).lock().transform([](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(val2.value(), 200);
}

TEST(SynchronizedTest, ConstNonConstCallableTest)
{
    {
        Synchronized<TestStruct> sync(TestStruct{42});
        EXPECT_EQ(sync.lock().transform(ConstNonConstCallable<TestStruct>{}).value(), 42);
        EXPECT_EQ(sync.lock().transform(NonConstCallable{}).value(), "42");

        ConstNonConstCallable<TestStruct> cnc_callable;
        EXPECT_EQ(sync.lock().transform(cnc_callable).value(), 42);

        NonConstCallable nc_callable;
        EXPECT_EQ(sync.lock().transform(nc_callable).value(), "42");
    }

    {
        const SyncTS_t const_sync(TestStruct{42});
        EXPECT_EQ(const_sync.lock().transform(ConstNonConstCallable<TestStruct>{}).value(), "42");
        EXPECT_EQ(const_sync.lock().transform(NonConstCallable{}).value(), "42");

        ConstNonConstCallable<TestStruct> cnc_callable;
        EXPECT_EQ(const_sync.lock().transform(cnc_callable).value(), "42");

        NonConstCallable nc_callable;
        EXPECT_EQ(const_sync.lock().transform(nc_callable).value(), "42");
    }
}

TEST(SynchronizedTest, TestStructExceptionSafetyDetailed)
{
    SyncTS_t sync_struct(TestStruct{42});

    EXPECT_EQ(sync_struct.lock().transform(set_and_ret_val<100, SyncTS_t::pointer>).value(), 100);

    EXPECT_NO_THROW({
        auto callback_result = sync_struct.lock().transform(set_and_ret_val<999, SyncTS_t::pointer>);
        EXPECT_EQ(callback_result.value(), 999);
    });

    auto val = std::as_const(sync_struct).lock().transform([](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(val.value(), 999);

    EXPECT_EQ(sync_struct.lock().transform(set_and_ret_val<200, SyncTS_t::pointer>).value(), 200);
    auto val2 = std::as_const(sync_struct).lock().transform([](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(val2.value(), 200);
}

}  // namespace test
