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
#include "score/os/ObjectSeam.h"
#include "gtest/gtest.h"

namespace score
{
namespace os
{
namespace test
{
// Mock Object for Testing
class MockObject
{
  public:
    MockObject(const std::string& data) : data_(data) {}
    MockObject(const MockObject& other) : data_(other.data_) {}
    MockObject(MockObject&& other) noexcept : data_(std::move(other.data_)) {}

    std::string data_;
};

template <typename Object>
class TestableBase : public ObjectSeam<Object>
{
  public:
    using ObjectSeam<Object>::ObjectSeam;  // Inherit constructors

    // Expose the protected select_instance method for testing
    static Object& test_select_instance(Object& instance)
    {
        return ObjectSeam<Object>::select_instance(instance);
    }
};

TEST(ObjectSeamTest, CopyConstructor)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "ObjectSeamTest Copy Constructor");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    MockObject mock("Testing Copy");
    TestableBase<MockObject> b1;
    TestableBase<MockObject> b2 = b1;  // Copy constructor

    EXPECT_NO_THROW(TestableBase<MockObject>::set_testing_instance(mock));
    EXPECT_EQ(&TestableBase<MockObject>::test_select_instance(mock), &mock);
    TestableBase<MockObject>::restore_instance();
}

TEST(ObjectSeamTest, MoveConstructor)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "ObjectSeamTest Move Constructor");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    MockObject mock("Testing Move");
    TestableBase<MockObject> b1;
    TestableBase<MockObject> b2 = std::move(b1);  // Move constructor

    EXPECT_NO_THROW(TestableBase<MockObject>::set_testing_instance(mock));
    EXPECT_EQ(&TestableBase<MockObject>::test_select_instance(mock), &mock);
    TestableBase<MockObject>::restore_instance();
}

}  // namespace test
}  // namespace os
}  // namespace score
