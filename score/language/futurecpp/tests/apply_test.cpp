/********************************************************************************
 * Copyright (c) 2019 Contributors to the Eclipse Foundation
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
/// \file
/// \copyright Copyright (c) 2019 Contributors to the Eclipse Foundation
///

#include <score/apply.hpp>
#include <score/apply.hpp> // test include guard

#include <array>
#include <cstdint>
#include <tuple>
#include <utility>

#include <gtest/gtest.h>

namespace
{

void simple_test_0() {}
void simple_test_1(std::int32_t) {}
void simple_test_2(std::int32_t, double) {}
void simple_test_3(std::int32_t, std::int32_t, std::int32_t) {}

struct simple_functor_type
{
    void operator()() {}
    void operator()(double) {}
    void operator()(std::int32_t, double) {}
    void operator()(double, double, double) {}
};

std::int8_t complex_test_0() { return 0; }
std::int16_t complex_test_1(std::int32_t) { return 1; }
std::int32_t complex_test_2(std::int32_t, double) { return 2; }
std::int64_t complex_test_3(std::int32_t, std::int32_t, std::int32_t) { return 3; }

struct complex_functor_type
{
    std::int8_t operator()() { return 0; }
    std::int16_t operator()(double) { return 1; }
    std::int32_t operator()(std::int32_t, double) { return 2; }
    std::int64_t operator()(double, double, double) { return 3; }
};

struct template_functor_type
{
    template <typename... Ts>
    std::size_t operator()(Ts&&...)
    {
        return sizeof...(Ts);
    }
};

struct forwarding_struct_type
{
    std::uint8_t operator()() const& { return {}; }
    double operator()() & { return {}; }
    float operator()() && { return {}; }

    std::int16_t operator()(std::int32_t&) { return {}; }
    std::int32_t operator()(const std::int32_t&) { return {}; }
    std::int64_t operator()(std::int32_t&&) { return {}; }
};

struct reference_return_value_struct_type
{
    double local{};
    const double& operator()() const { return local; }
    double& operator()() { return local; }
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8058382
TEST(score_future_cpp_apply_test, simple_function_test)
{
    using result_type_0_params = decltype(score::cpp::apply(simple_test_0, std::tuple<>{}));
    using result_type_1_params = decltype(score::cpp::apply(simple_test_1, std::tuple<std::int32_t>{}));
    using result_type_2_params = decltype(score::cpp::apply(simple_test_2, std::pair<std::int32_t, double>{}));
    using result_type_3_params = decltype(score::cpp::apply(simple_test_3, std::array<std::int32_t, 3>{}));

    EXPECT_TRUE((std::is_same<void, result_type_0_params>::value));
    EXPECT_TRUE((std::is_same<void, result_type_1_params>::value));
    EXPECT_TRUE((std::is_same<void, result_type_2_params>::value));
    EXPECT_TRUE((std::is_same<void, result_type_3_params>::value));

    EXPECT_NO_THROW(score::cpp::apply(simple_test_0, std::tuple<>{}));
    EXPECT_NO_THROW(score::cpp::apply(simple_test_1, std::tuple<std::int32_t>{}));
    EXPECT_NO_THROW(score::cpp::apply(simple_test_2, std::pair<std::int32_t, double>{}));
    EXPECT_NO_THROW(score::cpp::apply(simple_test_3, std::array<std::int32_t, 3>{}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8058382
TEST(score_future_cpp_apply_test, simple_functor_test)
{
    using result_type_0_params = decltype(score::cpp::apply(simple_functor_type{}, std::tuple<>{}));
    using result_type_1_params = decltype(score::cpp::apply(simple_functor_type{}, std::tuple<double>{}));
    using result_type_2_params = decltype(score::cpp::apply(simple_functor_type{}, std::pair<std::int32_t, double>{}));
    using result_type_3_params = decltype(score::cpp::apply(simple_functor_type{}, std::array<double, 3>{}));

    EXPECT_TRUE((std::is_same<void, result_type_0_params>::value));
    EXPECT_TRUE((std::is_same<void, result_type_1_params>::value));
    EXPECT_TRUE((std::is_same<void, result_type_2_params>::value));
    EXPECT_TRUE((std::is_same<void, result_type_3_params>::value));

    EXPECT_NO_THROW(score::cpp::apply(simple_functor_type{}, std::tuple<>{}));
    EXPECT_NO_THROW(score::cpp::apply(simple_functor_type{}, std::tuple<double>{}));
    EXPECT_NO_THROW(score::cpp::apply(simple_functor_type{}, std::pair<std::int32_t, double>{}));
    EXPECT_NO_THROW(score::cpp::apply(simple_functor_type{}, std::array<double, 3>{}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8058382
TEST(score_future_cpp_apply_test, complex_function_test)
{
    const auto result_0_params = score::cpp::apply(complex_test_0, std::tuple<>{});
    const auto result_1_params = score::cpp::apply(complex_test_1, std::tuple<std::int32_t>{});
    const auto result_2_params = score::cpp::apply(complex_test_2, std::pair<std::int32_t, double>{});
    const auto result_3_params = score::cpp::apply(complex_test_3, std::array<std::int32_t, 3>{});

    EXPECT_TRUE((std::is_same<std::int8_t, std::decay_t<decltype(result_0_params)>>::value));
    EXPECT_TRUE((std::is_same<std::int16_t, std::decay_t<decltype(result_1_params)>>::value));
    EXPECT_TRUE((std::is_same<std::int32_t, std::decay_t<decltype(result_2_params)>>::value));
    EXPECT_TRUE((std::is_same<std::int64_t, std::decay_t<decltype(result_3_params)>>::value));

    ASSERT_EQ(result_0_params, 0);
    ASSERT_EQ(result_1_params, 1);
    ASSERT_EQ(result_2_params, 2);
    ASSERT_EQ(result_3_params, 3);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8058382
TEST(score_future_cpp_apply_test, complex_functor_test)
{
    const auto result_0_params = score::cpp::apply(complex_functor_type{}, std::tuple<>{});
    const auto result_1_params = score::cpp::apply(complex_functor_type{}, std::tuple<double>{});
    const auto result_2_params = score::cpp::apply(complex_functor_type{}, std::pair<std::int32_t, double>{});
    const auto result_3_params = score::cpp::apply(complex_functor_type{}, std::array<double, 3>{});

    EXPECT_TRUE((std::is_same<std::int8_t, std::decay_t<decltype(result_0_params)>>::value));
    EXPECT_TRUE((std::is_same<std::int16_t, std::decay_t<decltype(result_1_params)>>::value));
    EXPECT_TRUE((std::is_same<std::int32_t, std::decay_t<decltype(result_2_params)>>::value));
    EXPECT_TRUE((std::is_same<std::int64_t, std::decay_t<decltype(result_3_params)>>::value));

    ASSERT_EQ(result_0_params, 0);
    ASSERT_EQ(result_1_params, 1);
    ASSERT_EQ(result_2_params, 2);
    ASSERT_EQ(result_3_params, 3);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8058382
TEST(score_future_cpp_apply_test, template_functor_test)
{
    const auto result_0_params = score::cpp::apply(template_functor_type{}, std::tuple<>{});
    const auto result_1_params = score::cpp::apply(template_functor_type{}, std::tuple<double>{});
    const auto result_2_params = score::cpp::apply(template_functor_type{}, std::pair<std::int32_t, double>{});
    const auto result_3_params = score::cpp::apply(template_functor_type{}, std::array<double, 3>{});

    EXPECT_TRUE((std::is_same<std::size_t, std::decay_t<decltype(result_0_params)>>::value));
    EXPECT_TRUE((std::is_same<std::size_t, std::decay_t<decltype(result_1_params)>>::value));
    EXPECT_TRUE((std::is_same<std::size_t, std::decay_t<decltype(result_2_params)>>::value));
    EXPECT_TRUE((std::is_same<std::size_t, std::decay_t<decltype(result_3_params)>>::value));

    ASSERT_EQ(result_0_params, 0);
    ASSERT_EQ(result_1_params, 1);
    ASSERT_EQ(result_2_params, 2);
    ASSERT_EQ(result_3_params, 3);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8058382
TEST(score_future_cpp_apply_test, forwarding_functor_test)
{
    forwarding_struct_type functor_ref;
    const auto& const_functor_ref = functor_ref;

    using result_type_lref = decltype(score::cpp::apply(functor_ref, std::tuple<>{}));
    using result_type_const_ref = decltype(score::cpp::apply(const_functor_ref, std::tuple<>{}));
    using result_type_rref = decltype(score::cpp::apply(forwarding_struct_type{}, std::tuple<>{}));

    EXPECT_TRUE((std::is_same<double, result_type_lref>::value));
    EXPECT_TRUE((std::is_same<std::uint8_t, result_type_const_ref>::value));
    EXPECT_TRUE((std::is_same<float, result_type_rref>::value));

    std::int32_t val_param{};
    std::int32_t& lref_param = val_param;
    const std::int32_t& const_lref_param = lref_param;
    using result_type_lref_param = decltype(score::cpp::apply(functor_ref, std::tuple<std::int32_t&>{lref_param}));
    using result_type_rref_param = decltype(score::cpp::apply(functor_ref, std::tuple<std::int32_t>{val_param}));
    using result_type_const_lref_param =
        decltype(score::cpp::apply(functor_ref, std::tuple<const std::int32_t&>{const_lref_param}));

    EXPECT_TRUE((std::is_same<std::int16_t, result_type_lref_param>::value));
    EXPECT_TRUE((std::is_same<std::int32_t, result_type_const_lref_param>::value));
    EXPECT_TRUE((std::is_same<std::int64_t, result_type_rref_param>::value));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8058382
TEST(score_future_cpp_apply_test, reference_return_value_test)
{
    reference_return_value_struct_type functor;
    const auto& const_functor_ref = functor;

    using result_type_lref = decltype(score::cpp::apply(functor, std::tuple<>{}));
    using result_type_const_ref = decltype(score::cpp::apply(const_functor_ref, std::tuple<>{}));

    EXPECT_TRUE((std::is_same<double&, result_type_lref>::value));
    EXPECT_TRUE((std::is_same<const double&, result_type_const_ref>::value));
}

} // namespace
