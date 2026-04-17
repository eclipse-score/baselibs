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
#include "score/filesystem/filestream/file_factory_fake.h"
#include "score/filesystem/filestream/file_buf.h"

#include <score/assert.hpp>
#include <score/utility.hpp>
#include <sstream>

namespace score::filesystem
{

namespace
{

class FakeFileBuf : public details::StdioFileBuf, std::stringbuf
{
  public:
    // Needed since qcc sees them as ambiguous
    using int_type = details::StdioFileBuf::int_type;
    using char_type = details::StdioFileBuf::char_type;

    static_assert(std::is_same_v<details::StdioFileBuf::int_type, std::stringbuf::int_type>,
                  "int_type differs between StdioFileBuf and std::stringbuf");
    static_assert(std::is_same_v<details::StdioFileBuf::char_type, std::stringbuf::char_type>,
                  "char_type differs between StdioFileBuf and std::stringbuf");

    explicit FakeFileBuf(std::stringstream& sstream, std::ios::openmode mode)
        : StdioFileBuf{-1, mode}, std::stringbuf{sstream.str()}, sstream_{sstream}
    {
    }
    ~FakeFileBuf() override
    {
        sstream_.get().str(str());
    }

  protected:
    void imbue(const std::locale& loc) override
    {
        std::stringbuf::imbue(loc);
    }
    std::streambuf* setbuf(char_type* s, std::streamsize n) override
    {
        return std::stringbuf::setbuf(s, n);
    }
    std::filebuf::pos_type seekoff(std::filebuf::off_type off,
                                   std::ios_base::seekdir dir,
                                   std::ios_base::openmode which) override
    {
        return std::stringbuf::seekoff(off, dir, which);
    }
    std::filebuf::pos_type seekpos(std::filebuf::pos_type pos, std::ios_base::openmode which) override
    {
        return std::stringbuf::seekpos(pos, which);
    }
    int sync() override
    {
        return std::stringbuf::sync();
    }
    std::streamsize showmanyc() override
    {
        return std::stringbuf::showmanyc();
    }
    int_type underflow() override
    {
        return std::stringbuf::underflow();
    }
    int_type uflow() override
    {
        return std::stringbuf::uflow();
    }
    std::streamsize xsgetn(char_type* s, std::streamsize count) override
    {
        return std::stringbuf::xsgetn(s, count);
    }
    std::streamsize xsputn(const char_type* s, std::streamsize count) override
    {
        return std::stringbuf::xsputn(s, count);
    }
    int_type overflow(int_type ch) override
    {
        return std::stringbuf::overflow(ch);
    }
    int_type pbackfail(int_type c) override
    {
        return std::stringbuf::pbackfail(c);
    }

  private:
    std::reference_wrapper<std::stringstream> sstream_;
};

}  // namespace

FileFactoryFake::FileFactoryFake(IStringStreamCollection& collection) : FileFactoryMock{}, collection_{collection}
{
    using ::testing::_;
    ON_CALL(*this, Open(_, _)).WillByDefault(::testing::Invoke(this, &FileFactoryFake::FakeOpen));
    // coverity[autosar_cpp14_a0_1_2_violation] False positive
    ON_CALL(*this, AtomicUpdate(_, _, _)).WillByDefault(::testing::Invoke(this, &FileFactoryFake::FakeAtomicUpdate));
}

score::Result<std::unique_ptr<std::iostream>> FileFactoryFake::FakeOpenWithMode(const Path& path,
                                                                              const std::ios_base::openmode mode,
                                                                              const os::Stat::Mode create_mode) const
{
    score::cpp::ignore = mode;  // not supported, Ticket-36536
    score::cpp::ignore = create_mode;
    auto stream_result = collection_.get().OpenStringStream(path);
    if (!stream_result.has_value())
    {
        return MakeUnexpected<std::unique_ptr<std::iostream>>(stream_result.error());
    }
    return std::make_unique<std::iostream>(stream_result.value().get().rdbuf());
}

Result<std::unique_ptr<FileStream>> FileFactoryFake::FakeAtomicUpdate(const Path& path,
                                                                      const std::ios_base::openmode mode,
                                                                      const AtomicUpdateOwnershipFlags) const
{
    // Only accept writing and truncating
    if ((mode & ~(std::ios::out | std::ios::trunc | std::ios::binary)) != 0)
    {
        return MakeUnexpected(ErrorCode::kNotImplemented);
    }
    auto stream_result = collection_.get().OpenStringStream(path);
    if (!stream_result.has_value())
    {
        return MakeUnexpected<std::unique_ptr<FileStream>>(stream_result.error());
    }
    return std::make_unique<details::FileStreamImpl<FakeFileBuf>>(stream_result->get(), mode);
}

std::stringstream& FileFactoryFake::Get(const Path& path) const
{
    auto stream_result = collection_.get().OpenStringStream(path);
    SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE(stream_result.has_value(),
                       "The Get()-function should be exucuted for valid paths that exist in the collection.");
    return stream_result.value().get();
}

}  // namespace score::filesystem
