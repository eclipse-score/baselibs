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
#include "score/os/utils/path.h"
#include "score/os/mocklib/unistdmock.h"
#include "score/os/utils/path_impl.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace os
{
namespace
{

// https://www.man7.org/linux/man-pages/man3/basename.3p.html#EXAMPLES
TEST(get_base_name, man_page_examples)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "get_base_name man_page_examples");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_EQ(Path::instance().get_base_name("usr"), "usr");
    EXPECT_EQ(Path::instance().get_base_name("usr/"), "usr");
    EXPECT_EQ(Path::instance().get_base_name(""), ".");
    EXPECT_EQ(Path::instance().get_base_name("/"), "/");
    EXPECT_EQ(Path::instance().get_base_name("//"), "/");
    EXPECT_EQ(Path::instance().get_base_name("///"), "/");
    EXPECT_EQ(Path::instance().get_base_name("/usr/"), "usr");
    EXPECT_EQ(Path::instance().get_base_name("/usr/lib"), "lib");
    EXPECT_EQ(Path::instance().get_base_name("//usr//lib"), "lib");
    EXPECT_EQ(Path::instance().get_base_name("/home//dwc//test"), "test");
}

TEST(get_exec_path, returns_non_empty)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "get_exec_path returns_non_empty");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const score::cpp::expected<std::string, score::os::Error> result{Path::Default()->get_exec_path()};
    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value().empty());
}

#ifdef __linux__
TEST(get_exec_path, returns_length_lessthan_zero)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "get_exec_path returns_length_lessthan_zero");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    UnistdMock mock_instance;
    Unistd::set_testing_instance(mock_instance);
    EXPECT_CALL(mock_instance, readlink(testing::_, testing::_, testing::_))
        .WillOnce([](const char*, char*, const size_t) {
            return -1;  // Indicate failure
        });

    const score::cpp::expected<std::string, score::os::Error> result{Path::Default()->get_exec_path()};
    ASSERT_FALSE(result.has_value());
}

TEST(get_exec_path, returns_length_equalto_pathmax)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "get_exec_path returns_length_equalto_pathmax");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    UnistdMock mock_instance;
    Unistd::set_testing_instance(mock_instance);
    EXPECT_CALL(mock_instance, readlink(testing::_, testing::_, testing::_))
        .WillOnce([](const char*, char*, const size_t) {
            return PATH_MAX;  // Indicate failure
        });

    const score::cpp::expected<std::string, score::os::Error> result{Path::Default()->get_exec_path()};
    ASSERT_FALSE(result.has_value());
}

TEST(get_exec_path, returns_length_greaterthan_pathmax)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "get_exec_path returns_length_greaterthan_pathmax");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    UnistdMock mock_instance;
    Unistd::set_testing_instance(mock_instance);
    EXPECT_CALL(mock_instance, readlink(testing::_, testing::_, testing::_))
        .WillOnce([](const char*, char*, const size_t) {
            return (PATH_MAX + 1);  // Indicate failure
        });

    const score::cpp::expected<std::string, score::os::Error> result{Path::Default()->get_exec_path()};
    ASSERT_FALSE(result.has_value());
}
#endif

// https://www.man7.org/linux/man-pages/man3/dirname.3p.htm
TEST(get_parent_dir, ManPageExamples)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "get_parent_dir Man Page Examples");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_EQ(Path::instance().get_parent_dir("/foo/bar"), "/foo");
    EXPECT_EQ(Path::instance().get_parent_dir("foo"), ".");
    EXPECT_EQ(Path::instance().get_parent_dir(""), ".");

    // Leading "//" is implementation-defined. May return either "//" or '/' (but nothing else).
    EXPECT_THAT(Path::instance().get_parent_dir("//foo"), ::testing::MatchesRegex("\\/|\\/\\/"))
        << "may return either '//' or '/', but nothing else";
}

TEST(PathTest, PMRDefaultShallReturnImplInstance)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "PathTest PMRDefault Shall Return Impl Instance");
    RecordProperty("TestingTechnique", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    score::cpp::pmr::memory_resource* memory_resource = score::cpp::pmr::get_default_resource();
    const auto instance = score::os::Path::Default(memory_resource);
    ASSERT_TRUE(instance != nullptr);
    EXPECT_NO_THROW(std::ignore = dynamic_cast<score::os::PathImpl*>(instance.get()));
}

}  // namespace
}  // namespace os
}  // namespace score
