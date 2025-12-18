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
#include "score/os/errno.h"
#include "score/os/errno_logging.h"
#include "score/mw/log/logging.h"
#include "score/mw/log/recorder_mock.h"

#include <glob.h>
#include <sstream>

#include <gtest/gtest.h>

namespace score
{
namespace os
{
namespace
{

using ::testing::_;
using ::testing::Return;
const mw::log::SlotHandle HANDLE{42};

class OsErrorViaLogStreamFixture : public ::testing::Test
{
  public:
    void SetUp() override
    {
        mw::log::SetLogRecorder(&recorder_);

        EXPECT_CALL(recorder_, StartRecord(_, _)).WillOnce(Return(HANDLE));
        EXPECT_CALL(recorder_, StopRecord(_)).Times(1);
    }

    mw::log::RecorderMock recorder_{};
};

TEST_F(OsErrorViaLogStreamFixture, CanStreamViaRValueStream)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "OsErrorViaLogStreamFixture Can Stream Via RValue Stream");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    // Given a constructed error
    const Error error{Error::createFromErrno(EPERM)};

    // Expecting that this error message is logged
    ::testing::InSequence in_sequence{};
    EXPECT_CALL(recorder_, LogStringView(_, {"An OS error has occurred with error code: "}));
    EXPECT_CALL(recorder_, LogStringView(_, {"Operation not permitted"}));

    // When streaming it into an r-value LogStream
    mw::log::LogError() << error;
}

TEST_F(OsErrorViaLogStreamFixture, StreamIntoMwLogStream2)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "OsErrorViaLogStreamFixture Stream Into Mw Log Stream2");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    // Given a constructed error
    const Error error{Error::createFromErrno(EPERM)};

    // Expecting that this error message is logged
    ::testing::InSequence in_sequence{};
    EXPECT_CALL(recorder_, LogStringView(_, {"An OS error has occurred with error code: "}));
    EXPECT_CALL(recorder_, LogStringView(_, {"Operation not permitted"}));

    // When streaming it into an l-value LogStream
    auto stream = mw::log::LogError();
    stream << error;
}

TEST(Error, CreationFromErrno)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Error Creation From Errno");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    Error error{Error::createFromErrno(EPERM)};
    EXPECT_EQ(error, Error::Code::kOperationNotPermitted);
}

TEST(Error, EqualityCompare)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Error Equality Compare");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto error1{Error::createFromErrno(EPERM)};
    const auto error2{Error::createFromErrno(EPERM)};
    EXPECT_EQ(error1, error2);
}

TEST(Error, InequalityCompare)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Error Inequality Compare");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto error1{Error::createFromErrno(EPERM)};
    const auto error3{Error::createFromErrno(EOVERFLOW)};
    EXPECT_NE(error1, error3);
}

TEST(Error, InequalityCompareToErrorCode)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Error Inequality Compare To Error Code");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto error1{Error::createFromErrno(EPERM)};
    EXPECT_NE(error1, Error::Code::kNotEnoughSpace);
}

TEST(Error, CreateUnspecifiedError)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Error Create Unspecified Error");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto error1{Error::createUnspecifiedError()};
    EXPECT_EQ(error1, Error::Code::kUnexpected);
}

TEST(Error, StreamingOut)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Error Streaming Out");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::stringstream ss;
    ss << Error::createFromErrno(EPERM);
    EXPECT_EQ(ss.str(), "An OS error has occurred with error code: Operation not permitted");
}

TEST(Error, SetErrno)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Error Set Errno");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    seterrno(EPERM);
    EXPECT_EQ(geterrno(), EPERM);
}

TEST(Error, ToString)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Error To String");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto error1{Error::createFromErrno(EPERM)};
    EXPECT_EQ(error1.ToString(), "An OS error has occurred with error code: Operation not permitted");
}

TEST(Error, GetOsDependentErrorCode)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Error Get Os Dependent Error Code");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto error1{Error::createFromErrno(EPERM)};
    EXPECT_EQ(error1.GetOsDependentErrorCode(), EPERM);
}

TEST(Error, CreateFromGlobErrorNoSpace)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Error Create From Glob Error No Space");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto error1{Error::createFromGlobError(GLOB_NOSPACE)};
    EXPECT_EQ(error1, Error::Code::kGlobNoSpace);
}

TEST(Error, CreateFromGlobErrorNotImplemented)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Error Create From Glob Error Not Implemented");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto error1{Error::createFromGlobError(GLOB_NOSYS)};
    EXPECT_EQ(error1, Error::Code::kUnexpected);
}

TEST(Error, CreateFromErrnoFlockSpecificOperationNotSupported)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Error Create From Errno Flock Specific Operation Not Supported");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto error1{Error::createFromErrnoFlockSpecific(EOPNOTSUPP)};
    EXPECT_EQ(error1, Error::Code::kFdRefersToAnObject);
}

TEST(Error, ErrorCodeConversion)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Error Error Code Conversion");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::map<const std::int32_t, const Error::Code> error_code_map = {
        {EPERM, Error::Code::kOperationNotPermitted},
        {ENOENT, Error::Code::kNoSuchFileOrDirectory},
        {EINTR, Error::Code::kOperationWasInterruptedBySignal},
        {EIO, Error::Code::kInputOutput},
        {ENXIO, Error::Code::kNoSuchFileOrDirectory},
        {EBADF, Error::Code::kBadFileDescriptor},
        {EAGAIN, Error::Code::kResourceTemporarilyUnavailable},
        {ENOMEM, Error::Code::kNotEnoughSpace},
        {EACCES, Error::Code::kPermissionDenied},
        {EBUSY, Error::Code::kDeviceOrResourceBusy},
        {ENOTDIR, Error::Code::kNotADirectory},
        {EISDIR, Error::Code::kIsADirectory},
        {EINVAL, Error::Code::kInvalidArgument},
        {ENFILE, Error::Code::kTooManyOpenFilesInSystem},
        {EMFILE, Error::Code::kTooManyOpenFiles},
        {ENOSPC, Error::Code::kNoSpaceLeftOnDevice},
        {EROFS, Error::Code::kReadOnlyFileSystem},
        {ENAMETOOLONG, Error::Code::kFilenameTooLong},
        {ELOOP, Error::Code::kToManyLevelsOfSymbolicLinks},
        {EOVERFLOW, Error::Code::kValueTooLargeForDataType},
        {ENOTSUP, Error::Code::kOperationNotSupported},
        {EEXIST, Error::Code::kObjectExists},
        {ESRCH, Error::Code::kNoSuchProcess},
        {ENOSYS, Error::Code::kFileSystemDoesNotSupportTheOperation},
        {ENOLCK, Error::Code::kKernelOutOfMemoryForAllocatingLocks}};
    for (const auto& error : error_code_map)
    {
        EXPECT_EQ(Error::createFromErrno(error.first), error.second);
    }
}

}  // namespace
}  // namespace os
}  // namespace score
