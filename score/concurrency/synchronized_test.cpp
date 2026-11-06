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

#include <future>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using score::concurrency::Synchronized;

namespace test
{

namespace
{
template <typename T, typename = void>
struct HasTryLock : std::false_type
{
};

template <typename T>
struct HasTryLock<T, std::void_t<decltype(std::declval<T&>().try_lock())>> : std::true_type
{
};

constexpr auto clone = [](auto ptr) noexcept(std::is_nothrow_copy_constructible_v<std::decay_t<decltype(*ptr)>>) {
    static_assert(std::is_copy_constructible_v<std::decay_t<decltype(*ptr)>>,
                  "clone requires a copy-constructible type");
    return *ptr;
};

template <template <typename> class Synced, typename T>
auto clone_synced(const Synced<T>& sync)
{
    return sync.WithLock(clone);
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

    EXPECT_EQ(sync_value.WithLock(clone), 42);

    sync_value.WithLock([](Syncint_t::pointer ptr) noexcept {
        *ptr = 100;
    });

    EXPECT_EQ(sync_value.WithLock(clone), 100);
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
                counter.WithLock([](Syncint_t::pointer ptr) noexcept {
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

    EXPECT_EQ(counter.WithLock(clone), num_tasks * increments_per_task);
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

    EXPECT_EQ(sync_int.WithLock(clone), 100);
}

TEST(SynchronizedTest, TryLockSupportsMutableConstAndUnavailableLockables)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("FullyVerifies", "comp_req__concurrency__synchronized_try_lock");
    RecordProperty("Description",
                   "Check that try_lock provides mutable or const access when available and returns nullptr on "
                   "contention, while remaining unavailable for Lockable types without try_lock.");

    static_assert(HasTryLock<Synchronized<int>>::value);
    static_assert(HasTryLock<const Synchronized<int>>::value);
    static_assert(HasTryLock<Synchronized<int, std::mutex>>::value);
    static_assert(HasTryLock<Synchronized<int, std::recursive_mutex>>::value);
    static_assert(HasTryLock<Synchronized<int, std::timed_mutex>>::value);
    static_assert(HasTryLock<Synchronized<int, std::recursive_timed_mutex>>::value);
    static_assert(HasTryLock<Synchronized<int, std::shared_mutex>>::value);
    static_assert(HasTryLock<Synchronized<int, std::shared_timed_mutex>>::value);
    static_assert(!HasTryLock<Synchronized<int, MockMutex>>::value);

    Synchronized<int> sync_value(42);
    const int* sync_value_ptr = sync_value.lock().get();

    {
        auto locked_ptr = sync_value.try_lock();
        ASSERT_EQ(locked_ptr.get(), sync_value_ptr);
        EXPECT_EQ(*locked_ptr, 42);
        *locked_ptr = 100;
    }

    {
        auto locked_ptr = std::as_const(sync_value).try_lock();
        ASSERT_EQ(locked_ptr.get(), sync_value_ptr);
        EXPECT_EQ(*locked_ptr, 100);
    }

    {
        auto held_lock = sync_value.lock();
        ASSERT_EQ(held_lock.get(), sync_value_ptr);
        auto try_lock_result = std::async(std::launch::async, [&sync_value]() {
            return sync_value.try_lock() == nullptr;
        });
        EXPECT_TRUE(try_lock_result.get());
    }

    {
        auto held_lock = sync_value.lock();
        ASSERT_EQ(held_lock.get(), sync_value_ptr);
        auto try_lock_result = std::async(std::launch::async, [&sync_value]() {
            return std::as_const(sync_value).try_lock() == nullptr;
        });
        EXPECT_TRUE(try_lock_result.get());
    }
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

    EXPECT_EQ(const_sync_struct.WithLock(clone), 42);
}

TEST(SynchronizedTest, TestStructWithLockVariations)
{
    SyncTS_t sync_struct(TestStruct{42});

    sync_struct.WithLock([](SyncTS_t::pointer s) noexcept {
        s->value = 100;
    });

    EXPECT_EQ(sync_struct.WithLock(clone), 100);

    sync_struct.WithLock([](SyncTS_t::pointer s) noexcept {
        s->value = s->value * 2;
    });

    EXPECT_EQ(sync_struct.WithLock(clone), 200);
}

TEST(SynchronizedTest, TestStructMemberFunctionPointers)
{
    SyncTS_t sync_struct(TestStruct{42});
    EXPECT_EQ(sync_struct.WithLock(std::mem_fn(&TestStruct::plus_50)), 92);
    EXPECT_EQ(sync_struct.WithLock(&TestStruct::plus_50), 92);

    const SyncTS_t sync_const(42);
    EXPECT_EQ(sync_const.WithLock(std::mem_fn(&TestStruct::plus_50)), 92);
    EXPECT_EQ(sync_const.WithLock(&TestStruct::plus_50), 92);
}

TEST(SynchronizedTest, TestStructFreeFunctionPointers)
{
    SyncTS_t sync_struct(TestStruct{42});

    sync_struct.WithLock(&set_999);

    EXPECT_EQ(sync_struct.WithLock(&get_value), 999);
}

TEST(SynchronizedTest, TestStructLockNonConst)
{
    SyncTS_t sync_struct(TestStruct{42});

    {
        auto locked_ptr = sync_struct.lock();
        locked_ptr->value = 77;
        EXPECT_EQ(locked_ptr->value, 77);
    }

    EXPECT_EQ(sync_struct.WithLock(clone), 77);
}

TEST(SynchronizedTest, TestStructLockConst)
{
    const SyncTS_t const_sync_struct(TestStruct{42});

    {
        auto const_locked_ptr = const_sync_struct.lock();
        EXPECT_EQ(const_locked_ptr->value, 42);
    }

    EXPECT_EQ(const_sync_struct.WithLock(clone), 42);
}

TEST(SynchronizedTest, TestStructLambdaCaptureModes)
{
    SyncTS_t sync_struct(TestStruct{42});

    int external_value = 10;

    // Test lambda with value capture
    sync_struct.WithLock([external_value](SyncTS_t::pointer s) noexcept {
        s->value = external_value * 5;
    });

    auto result = sync_struct.WithLock([](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(result, 50);

    // Test lambda with reference capture
    int multiplier = 4;
    sync_struct.WithLock([&multiplier](SyncTS_t::pointer s) noexcept {
        s->value = s->value * multiplier;
    });

    EXPECT_EQ(sync_struct.WithLock(clone), 200);
}

TEST(SynchronizedTest, TestStructVoidReturningLambdas)
{
    SyncTS_t sync_struct(TestStruct{42});

    sync_struct.WithLock([](SyncTS_t::pointer s) noexcept -> void {
        s->value = 123;
    });

    EXPECT_EQ(sync_struct.WithLock(clone), 123);
}

TEST(SynchronizedTest, TestStructCopyVsMoveSemantics)
{
    // Test construction from temporary (move semantics)
    SyncTS_t sync_struct1(TestStruct{42});
    auto val = sync_struct1.WithLock([](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(val, 42);

    // Test construction from lvalue (copy semantics)
    TestStruct temp_struct{55};
    SyncTS_t sync_struct2(temp_struct);
    auto val1 = sync_struct2.WithLock([](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(val1, 55);

    // Test in-place construction
    SyncTS_t sync_struct3(TestStruct{77});
    auto val2 = sync_struct3.WithLock([](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(val2, 77);
}

TEST(SynchronizedTest, TestMoveOnlyObject)
{
    SyncMoveOnly_t sync_obj(150);

    auto result = sync_obj.WithLock([](SyncMoveOnly_t::const_pointer ptr) noexcept {
        return ptr->value;
    });
    EXPECT_EQ(result, 150);

    sync_obj.WithLock([](SyncMoveOnly_t::pointer ptr) noexcept {
        ptr->value = 300;
    });

    auto val = sync_obj.WithLock([](SyncMoveOnly_t::const_pointer ptr) noexcept {
        return ptr->value;
    });
    EXPECT_EQ(val, 300);
}

TEST(SynchronizedTest, TestNoCopyNoMoveObject)
{
    SyncNoCopyNoMove_t sync_obj(250);

    auto result = sync_obj.WithLock([](SyncNoCopyNoMove_t::const_pointer ptr) noexcept {
        return ptr->value;
    });
    EXPECT_EQ(result, 250);

    sync_obj.WithLock([](SyncNoCopyNoMove_t::pointer ptr) noexcept {
        ptr->value = 500;
    });

    auto val = sync_obj.WithLock([](SyncNoCopyNoMove_t::const_pointer ptr) noexcept {
        return ptr->value;
    });
    EXPECT_EQ(val, 500);
}

TEST(SynchronizedTest, TestDefaultConstructedObject)
{
    SyncDefaultConstructible_t sync_obj{};

    auto result = sync_obj.WithLock([](SyncDefaultConstructible_t::const_pointer ptr) noexcept {
        return ptr->value;
    });
    EXPECT_EQ(result, 42);

    sync_obj.WithLock([](SyncDefaultConstructible_t::pointer ptr) noexcept {
        ptr->value = 100;
    });

    auto val = sync_obj.WithLock([](SyncDefaultConstructible_t::const_pointer ptr) noexcept {
        return ptr->value;
    });
    EXPECT_EQ(val, 100);
}

TEST(SynchronizedTest, TestParameterizedConstruction)
{
    SyncParameterized_t sync_obj(10, 20, "test_object");

    auto result = sync_obj.WithLock([](SyncParameterized_t::const_pointer ptr) noexcept {
        return ptr->x_plus_y();
    });
    EXPECT_EQ(result, 30);

    auto name = sync_obj.WithLock([](SyncParameterized_t::const_pointer ptr) noexcept {
        return ptr->GetName();
    });
    EXPECT_EQ(name, "test_object");

    sync_obj.WithLock([](SyncParameterized_t::pointer ptr) noexcept {
        ptr->x = 15;
        ptr->y = 25;
    });

    result = sync_obj.WithLock([](SyncParameterized_t::const_pointer ptr) noexcept {
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

    EXPECT_EQ(sync_piecewise.WithLock(clone), "ZZZZZ");
}

TEST(SynchronizedTest, TestConstOperations)
{
    const SyncTS_t const_sync_struct(TestStruct{42});

    auto result = const_sync_struct.WithLock([](SyncTS_t::const_pointer s) noexcept {
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

    sync_struct.WithLock([](SyncTS_t::pointer s) noexcept {
        s->value = 200;
    });

    auto result = sync_struct.WithLock([](SyncTS_t::const_pointer s) {
        return s->plus_50();
    });
    EXPECT_EQ(result, 250);
    auto val1 = sync_struct.WithLock([](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(val1, 200);

    EXPECT_NO_THROW({
        sync_struct.WithLock([](SyncTS_t::const_pointer s) noexcept {
            EXPECT_EQ(s->value, 200);
        });
    });

    auto val2 = sync_struct.WithLock([](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(val2, 200);
}

TEST(SynchronizedTest, ConstNonConstCallableTest)
{
    {
        Synchronized<TestStruct> sync(TestStruct{42});
        EXPECT_EQ(sync.WithLock(ConstNonConstCallable<TestStruct>{}), 42);
        EXPECT_EQ(sync.WithLock(NonConstCallable{}), "42");

        ConstNonConstCallable<TestStruct> cnc_callable;
        EXPECT_EQ(sync.WithLock(cnc_callable), 42);

        NonConstCallable nc_callable;
        EXPECT_EQ(sync.WithLock(nc_callable), "42");
    }

    {
        const SyncTS_t const_sync(TestStruct{42});
        EXPECT_EQ(const_sync.WithLock(ConstNonConstCallable<TestStruct>{}), "42");
        EXPECT_EQ(const_sync.WithLock(NonConstCallable{}), "42");

        ConstNonConstCallable<TestStruct> cnc_callable;
        EXPECT_EQ(const_sync.WithLock(cnc_callable), "42");

        NonConstCallable nc_callable;
        EXPECT_EQ(const_sync.WithLock(nc_callable), "42");
    }
}

TEST(SynchronizedTest, TestStructExceptionSafetyDetailed)
{
    SyncTS_t sync_struct(TestStruct{42});

    sync_struct.WithLock([](SyncTS_t::pointer s) noexcept {
        s->value = 100;
    });

    EXPECT_NO_THROW({
        sync_struct.WithLock([](SyncTS_t::pointer s) noexcept {
            s->value = 999;
            EXPECT_EQ(s->value, 999);
        });
    });

    auto val = sync_struct.WithLock([](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(val, 999);

    sync_struct.WithLock([](SyncTS_t::pointer s) noexcept {
        s->value = 200;
    });
    auto val2 = sync_struct.WithLock([](SyncTS_t::const_pointer s) noexcept {
        return s->value;
    });
    EXPECT_EQ(val2, 200);
}

}  // namespace test
