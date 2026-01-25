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
/// \copyright Copyright (c) 2019 Contributors to the Eclipse Foundation
///

#include <score/assert.hpp>
#include <score/assert.hpp> // check on include guard

#include <gtest/gtest.h>

namespace score::cpp
{
namespace
{

class assert_test_class : public ::testing::Test
{
protected:
    assert_test_class() = default;
    void SetUp() override { default_handler_ = get_assertion_handler(); }
    void TearDown() override
    {
        set_assertion_handler(default_handler_); // Do not leak local handlers out of test suite
    }

private:
    handler default_handler_;
};

struct contract_violation
{
};

///
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#42723425
TEST_F(assert_test_class, get_assertion_handler)
{
    const auto handler = [](handler_parameters const& /*unused*/) {};
    set_assertion_handler(handler);
    EXPECT_EQ(get_assertion_handler(), handler);
}

///
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#42723425
TEST_F(assert_test_class, assertion_failed_invokes_active_handler)
{
    const auto handler = [](handler_parameters const& param) {
        ASSERT_EQ(param.file, std::string("file"));
        ASSERT_EQ(param.line, 42);
        ASSERT_EQ(param.function, std::string("func"));
        ASSERT_EQ(param.condition, std::string("cond"));
        ASSERT_EQ(param.message, std::string("msg"));
        ASSERT_EQ(param.user_data, nullptr);
        throw contract_violation{};
    };
    set_assertion_handler(handler);

    EXPECT_THROW(detail::assertion_failed(handler_parameters{"file", 42, "func", "cond", "msg", nullptr},
                                          detail::assert_system_handler_tag{}),
                 contract_violation);
}

///
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#42723425
TEST_F(assert_test_class, precondition_macro_behave_same_as_assert)
{
    set_assertion_handler([](handler_parameters const&) { throw contract_violation{}; });

    EXPECT_NO_THROW(SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(true));
    EXPECT_THROW(SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(false), contract_violation);
}

///
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#42723425
TEST_F(assert_test_class, unreachable_macro_triggers_failing_assertion)
{
    set_assertion_handler([](handler_parameters const&) { throw contract_violation{}; });

    EXPECT_THROW(SCORE_LANGUAGE_FUTURECPP_UNREACHABLE(), contract_violation);
}

///
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#42723425
TEST_F(assert_test_class, get_user_data)
{
    set_user_data(this); // Just a random, valid memory address is needed
    EXPECT_EQ(get_user_data(), this);
}

///
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#42723425
TEST_F(assert_test_class, abort)
{
    // In general we shouldn't use death test and instead the amp test support MACROs.
    // The recommended test pattern is to setup a special assertion handler which throws.
    // But we have to at least test once the implicit defaulted null handler and the final call to abort.
    EXPECT_DEATH(SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(false), "");
}

///
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#42723425
TEST_F(assert_test_class, set_nullptr_expect_abort)
{
    // In general we shouldn't use death test and instead the amp test support MACROs.
    // The recommended test pattern is to setup a special assertion handler which throws.
    // But we have to at least test once the explicit defaulted null handler and the final call to abort.
    set_assertion_handler(nullptr);
    EXPECT_DEATH(SCORE_LANGUAGE_FUTURECPP_PRECONDITION(false), "");
}

} // namespace
} // namespace score::cpp
