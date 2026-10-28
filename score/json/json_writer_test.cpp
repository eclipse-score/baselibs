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
#include "score/json/json_writer.h"
#include "score/filesystem/filestream/file_factory_fake.h"
#include "score/filesystem/filestream/simple_string_stream_collection.h"
#include "score/quality/compiler_warnings/warnings.h"

#include <gtest/gtest.h>

#include <array>
#include <iterator>
#include <limits>
#include <memory>
#include <sstream>
#include <string_view>
#include <vector>

namespace score
{
namespace json
{
namespace
{

using ::testing::_;
using ::testing::ByMove;
using ::testing::Return;
using ::testing::StrEq;

constexpr auto kKeySeparator = "\": ";

class TestJsonList : public json::List
{
  public:
    TestJsonList()
    {
        emplace_back(1234);
        emplace_back(std::string{"string"});
        score::json::Object obj{};
        obj["key"] = score::json::Any{std::string{"value"}};
        emplace_back(std::move(obj));
    }

    static constexpr std::string_view kExpectedCompact{R"([1234,"string",{"key":"value"}])"};
    static constexpr std::string_view kExpectedPrettyPrint{R"([
    1234,
    "string",
    {
        "key": "value"
    }
])"};
};

class TestJsonObject : public json::Object
{
  public:
    TestJsonObject()
    {
        emplace("string", score::json::Any{std::string{"foo"}});
        emplace("num", score::json::Any{1});
    }

    static constexpr std::string_view kExpectedCompact{R"({"num":1,"string":"foo"})"};
    static constexpr std::string_view kExpectedPrettyPrint{R"({
    "num": 1,
    "string": "foo"
})"};
};

class TestJsonAny : public json::Any
{
  public:
    TestJsonAny() : json::Any(std::string{"any_foo"}) {}

    static constexpr std::string_view kExpectedCompact{R"("any_foo")"};
    static constexpr std::string_view kExpectedPrettyPrint{kExpectedCompact};
};

/// @brief Combines a sample JSON value with the formatting it is written with.
template <typename SampleType, Formatting formatting>
struct WriterTestCase
{
    using SampleJson = SampleType;
    static constexpr Formatting kFormatting{formatting};
};

using WriterTestCases = ::testing::Types<WriterTestCase<TestJsonList, Formatting::kCompact>,
                                         WriterTestCase<TestJsonList, Formatting::kPrettyPrint>,
                                         WriterTestCase<TestJsonObject, Formatting::kCompact>,
                                         WriterTestCase<TestJsonObject, Formatting::kPrettyPrint>,
                                         WriterTestCase<TestJsonAny, Formatting::kCompact>,
                                         WriterTestCase<TestJsonAny, Formatting::kPrettyPrint>>;

template <typename T>
class JsonWriterWriteToFileTest : public ::testing::Test
{
  protected:
    using SampleJson = typename T::SampleJson;
    static constexpr Formatting kFormatting{T::kFormatting};

    /// @brief Returns the representation emitted for SampleJson written with kFormatting.
    static std::string_view Expected() noexcept
    {
        return (kFormatting == Formatting::kCompact) ? SampleJson::kExpectedCompact : SampleJson::kExpectedPrettyPrint;
    }

    score::filesystem::SimpleStringStreamCollection stream{};
    score::filesystem::FileFactoryFake file_factory_fake{stream};

    template <typename Json>
    std::string WriteToFile(const Json& json, std::string_view path, FileSyncMode type)
    {
        score::json::JsonWriter writer{file_factory_fake,
                                       type,
                                       score::filesystem::kUseTargetFileUID | score::filesystem::kUseTargetFileGID,
                                       kFormatting};
        auto result = writer.ToFile(json, path);

        EXPECT_EQ(result.has_value(), true);

        auto& file = file_factory_fake.Get("/foo/foo.json");
        return file.str();
    }
};

TYPED_TEST_SUITE(JsonWriterWriteToFileTest, WriterTestCases, );

TYPED_TEST(JsonWriterWriteToFileTest, ToBuffer)
{
    this->RecordProperty("Verifies", "::score::json::JsonWriter::ToBuffer");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "writing json to string buffer, cf. RFC-8259 section 4, 5 and 9");
    this->RecordProperty("TestType", "interface-test");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");  // boundary values
    this->RecordProperty("Priority", "3");

    typename TestFixture::SampleJson json;
    score::json::JsonWriter writer{FileSyncMode::kUnsynced,
                                   score::filesystem::kUseTargetFileUID | score::filesystem::kUseTargetFileGID,
                                   TestFixture::kFormatting};
    std::string buffer = *writer.ToBuffer(json);

    EXPECT_EQ(buffer, TestFixture::Expected());
}

TYPED_TEST(JsonWriterWriteToFileTest, ToFile)
{
    this->RecordProperty("Verifies", "::score::json::JsonWriter::ToFile");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "writing json to string buffer, cf. RFC-8259 section 4, 5 and 9");
    this->RecordProperty("TestType", "interface-test");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");  // boundary values
    this->RecordProperty("Priority", "3");

    typename TestFixture::SampleJson json;

    EXPECT_CALL(this->file_factory_fake, AtomicUpdate(StrEq("/foo/foo.json"), std::ios::out | std::ios::trunc, _))
        .Times(1);

    auto file_content = this->WriteToFile(json, "/foo/foo.json", FileSyncMode::kSynced);

    EXPECT_EQ(file_content, TestFixture::Expected());
}

TYPED_TEST(JsonWriterWriteToFileTest, ToUnsyncedFile)
{
    this->RecordProperty("Verifies", "::score::json::JsonWriter::ToFile");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "writing json to string buffer, cf. RFC-8259 section 4, 5 and 9");
    this->RecordProperty("TestType", "interface-test");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");  // boundary values
    this->RecordProperty("Priority", "3");

    typename TestFixture::SampleJson json;

    EXPECT_CALL(this->file_factory_fake, Open(StrEq("/foo/foo.json"), std::ios::out | std::ios::trunc)).Times(1);

    auto file_content = this->WriteToFile(json, "/foo/foo.json", FileSyncMode::kUnsynced);

    EXPECT_EQ(file_content, TestFixture::Expected());
}

TYPED_TEST(JsonWriterWriteToFileTest, ToSyncedFile)
{
    this->RecordProperty("Verifies", "::score::json::JsonWriter::ToFile");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "writing json to string buffer, cf. RFC-8259 section 4, 5 and 9");
    this->RecordProperty("TestType", "interface-test");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");  // boundary values
    this->RecordProperty("Priority", "3");

    typename TestFixture::SampleJson json;

    EXPECT_CALL(this->file_factory_fake, AtomicUpdate(StrEq("/foo/foo.json"), std::ios::out | std::ios::trunc, _))
        .Times(1);

    auto file_content = this->WriteToFile(json, "/foo/foo.json", FileSyncMode::kSynced);

    EXPECT_EQ(file_content, TestFixture::Expected());
}

TYPED_TEST(JsonWriterWriteToFileTest, ToUnsyncedFileResultsInError)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "Invalid file path returns error when writing unsynced file");
    this->RecordProperty("TestType", "requirements-based");                // requirements test
    this->RecordProperty("DerivationTechnique", "requirements-analysis");  // requirements
    this->RecordProperty("Priority", "3");

    typename TestFixture::SampleJson json;

    EXPECT_CALL(this->file_factory_fake, Open(StrEq("/foo/foo.json"), std::ios::out | std::ios::trunc))
        .WillOnce(Return(ByMove(score::MakeUnexpected(score::json::Error::kInvalidFilePath))));

    score::json::JsonWriter writer{this->file_factory_fake, FileSyncMode::kUnsynced};
    auto result = writer.ToFile(json, "/foo/foo.json");

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::json::Error::kInvalidFilePath);
}

TYPED_TEST(JsonWriterWriteToFileTest, ToSyncedFileResultsInError)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "Invalid file path returns error when writing with atomic update");
    this->RecordProperty("TestType", "requirements-based");                // requirements test
    this->RecordProperty("DerivationTechnique", "requirements-analysis");  // requirements
    this->RecordProperty("Priority", "3");

    typename TestFixture::SampleJson json;

    EXPECT_CALL(this->file_factory_fake, AtomicUpdate(StrEq("/foo/foo.json"), std::ios::out | std::ios::trunc, _))
        .WillOnce(Return(ByMove(score::MakeUnexpected(score::json::Error::kInvalidFilePath))));

    score::json::JsonWriter writer{this->file_factory_fake, FileSyncMode::kSynced};
    auto result = writer.ToFile(json, "/foo/foo.json");

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::json::Error::kInvalidFilePath);
}

TYPED_TEST(JsonWriterWriteToFileTest, DeprecatedToFileWritesThroughGivenFileFactory)
{
    this->RecordProperty("Verifies", "::score::json::JsonWriter::ToFile");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "The deprecated overload writes through the file factory passed to it.");
    this->RecordProperty("TestType", "interface-test");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");
    this->RecordProperty("Priority", "3");

    typename TestFixture::SampleJson json;
    score::filesystem::SimpleStringStreamCollection other_stream{};
    auto other_file_factory = std::make_shared<score::filesystem::FileFactoryFake>(other_stream);

    EXPECT_CALL(this->file_factory_fake, Open(_, _)).Times(0);
    EXPECT_CALL(*other_file_factory, Open(StrEq("/foo/foo.json"), std::ios::out | std::ios::trunc)).Times(1);

    score::json::JsonWriter writer{this->file_factory_fake,
                                   FileSyncMode::kUnsynced,
                                   score::filesystem::kUseTargetFileUID | score::filesystem::kUseTargetFileGID,
                                   TestFixture::kFormatting};
    // clang-format off
    DISABLE_WARNING_PUSH
    DISABLE_WARNING(-Wdeprecated-declarations)
    auto result = writer.ToFile(json, "/foo/foo.json", other_file_factory);
    DISABLE_WARNING_POP
    // clang-format on

    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(other_file_factory->Get("/foo/foo.json").str(), TestFixture::Expected());
}

TYPED_TEST(JsonWriterWriteToFileTest, DeprecatedToFileWithNullFileFactoryResultsInError)
{
    this->RecordProperty("Verifies", "::score::json::JsonWriter::ToFile");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "The deprecated overload returns an error instead of dereferencing null.");
    this->RecordProperty("TestType", "interface-test");
    this->RecordProperty("DerivationTechnique", "error-guessing");
    this->RecordProperty("Priority", "3");

    typename TestFixture::SampleJson json;

    EXPECT_CALL(this->file_factory_fake, Open(_, _)).Times(0);

    score::json::JsonWriter writer{this->file_factory_fake};
    // clang-format off
    DISABLE_WARNING_PUSH
    DISABLE_WARNING(-Wdeprecated-declarations)
    auto result = writer.ToFile(json, "/foo/foo.json", nullptr);
    DISABLE_WARNING_POP
    // clang-format on

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::json::Error::kInvalidFilePath);
}

TEST(JsonWriterDefaultFormattingTest, PrettyPrintsWhenFormattingIsOmitted)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__serialization");
    RecordProperty("Description",
                   "Check that a JsonWriter constructed without a Formatting argument writes pretty-printed JSON.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");

    score::json::JsonWriter writer{};

    EXPECT_EQ(*writer.ToBuffer(TestJsonObject{}), TestJsonObject::kExpectedPrettyPrint);
    EXPECT_EQ(*writer.ToBuffer(TestJsonList{}), TestJsonList::kExpectedPrettyPrint);
    EXPECT_EQ(*writer.ToBuffer(TestJsonAny{}), TestJsonAny::kExpectedPrettyPrint);
}

template <typename T>
class JsonWriterIntegerTest : public ::testing::Test
{
};

// NOTE: Only types supported by score::json::Any/Number are included here.
// The Number variant currently supports std::int{8,16,32,64}_t and std::uint{8,16,32,64}_t.
// Types such as long long and unsigned long long can only be tested on platforms where the above typedefs resolve to
// them (usually <64-bit platforms).
// This test suite covers all integral types not causing a -Wsign-promo error accepted by the production JSON
// implementation.
using IntegralTypes = ::testing::Types<std::int32_t, std::int64_t, std::uint32_t, std::uint64_t>;
TYPED_TEST_SUITE(JsonWriterIntegerTest, IntegralTypes, );

TYPED_TEST(JsonWriterIntegerTest, FormatsIntegralValuesCorrectly)
{
    this->RecordProperty("Verifies", "::score::json::JsonWriter::ToBuffer");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "Test integer formatting with type-specific values");
    this->RecordProperty("TestType", "interface-test");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");  // boundary values
    this->RecordProperty("Priority", "3");

    using T = TypeParam;

    score::json::Object obj;

    // Always add zero, a positive sample, and small positive corner cases.
    obj["zero"] = score::json::Any{static_cast<T>(0)};
    obj["positive"] = score::json::Any{static_cast<T>(12345)};
    obj["p9"] = score::json::Any{static_cast<T>(9)};
    obj["p10"] = score::json::Any{static_cast<T>(10)};
    obj["p11"] = score::json::Any{static_cast<T>(11)};

    // Add negative values only for signed types
    if constexpr (std::numeric_limits<T>::is_signed)
    {
        obj["negative"] = score::json::Any{static_cast<T>(-12345)};
        obj["min"] = score::json::Any{std::numeric_limits<T>::min()};
        obj["m9"] = score::json::Any{static_cast<T>(-9)};
        obj["m10"] = score::json::Any{static_cast<T>(-10)};
        obj["m11"] = score::json::Any{static_cast<T>(-11)};
    }

    obj["max"] = score::json::Any{std::numeric_limits<T>::max()};

    // Use the JsonWriter to serialize
    score::json::JsonWriter writer{FileSyncMode::kUnsynced,
                                   score::filesystem::kUseTargetFileUID | score::filesystem::kUseTargetFileGID,
                                   Formatting::kPrettyPrint};
    auto result = writer.ToBuffer(obj);
    ASSERT_TRUE(result.has_value());
    const std::string json_str = *result;

    // Verify all values are formatted correctly
    EXPECT_NE(std::string::npos,
              json_str.find(std::string{"\"zero"} + kKeySeparator + std::to_string(static_cast<T>(0))));
    EXPECT_NE(std::string::npos,
              json_str.find(std::string{"\"positive"} + kKeySeparator + std::to_string(static_cast<T>(12345))));

    EXPECT_NE(std::string::npos,
              json_str.find(std::string{"\"p9"} + kKeySeparator + std::to_string(static_cast<T>(9))));
    EXPECT_NE(std::string::npos,
              json_str.find(std::string{"\"p10"} + kKeySeparator + std::to_string(static_cast<T>(10))));
    EXPECT_NE(std::string::npos,
              json_str.find(std::string{"\"p11"} + kKeySeparator + std::to_string(static_cast<T>(11))));

    if constexpr (std::numeric_limits<T>::is_signed)
    {
        EXPECT_NE(std::string::npos,
                  json_str.find(std::string{"\"negative"} + kKeySeparator + std::to_string(static_cast<T>(-12345))));
        EXPECT_NE(std::string::npos,
                  json_str.find(std::string{"\"min"} + kKeySeparator + std::to_string(std::numeric_limits<T>::min())));
        EXPECT_NE(std::string::npos,
                  json_str.find(std::string{"\"m9"} + kKeySeparator + std::to_string(static_cast<T>(-9))));
        EXPECT_NE(std::string::npos,
                  json_str.find(std::string{"\"m10"} + kKeySeparator + std::to_string(static_cast<T>(-10))));
        EXPECT_NE(std::string::npos,
                  json_str.find(std::string{"\"m11"} + kKeySeparator + std::to_string(static_cast<T>(-11))));
    }

    EXPECT_NE(std::string::npos,
              json_str.find(std::string{"\"max"} + kKeySeparator + std::to_string(std::numeric_limits<T>::max())));
}

}  // namespace
}  // namespace json
}  // namespace score
