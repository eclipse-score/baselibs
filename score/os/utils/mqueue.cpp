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
#include "score/os/utils/mqueue.h"

#include "score/utility.hpp"
#include "score/datetime_converter/time_conversion.h"
#include "score/os/mqueue.h"
#include "score/os/stat.h"
#include <cassert>
#include <chrono>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <utility>
namespace score
{
namespace common
{
std::size_t get_string_as_hash(const std::string& name)
{
    std::hash<std::string> hash_fn;
    return hash_fn(name);
}
}  // namespace common
namespace os
{

/* KW_SUPPRESS_START:MISRA.LINKAGE.EXTERN:PIMPL implementation principle */
class MQueue::MQueuePrivate
{
    /* KW_SUPPRESS_START:AUTOSAR.CTOR.NSDMI_INIT_LIST:False positive - all values are set in constructor */
    struct mq_attr m_attr{};
    std::int32_t m_fd{};
    std::size_t m_id{};
    std::string m_name{};
    /* KW_SUPPRESS_END:AUTOSAR.CTOR.NSDMI_INIT_LIST:False positive - all values are set in constructor */

    score::cpp::expected_blank<score::os::Error> open_create(const size_t max_msg_size,
                                                    const size_t max_msgs,
                                                    const AccessMode mode) noexcept;
    score::cpp::expected_blank<score::os::Error>
    open() noexcept; /* KW_SUPPRESS:MISRA.VAR.HIDDEN:Class method doesn't hide a same-name function in fcntl.h */
    static std::string create_name(const std::string name);
    static std::string create_name(const size_t id) noexcept;

  public:
    explicit MQueuePrivate(const std::string& name,
                           const AccessMode mode,
                           const size_t max_msg_size,
                           const size_t max_msgs);
    explicit MQueuePrivate(const std::string& name) noexcept;
    explicit MQueuePrivate(const size_t id);
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    void send(const std::string& msg) const;
    /* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
    score::cpp::expected_blank<score::os::Error> send(const char* const msg, const size_t length) const;
    ssize_t timed_send(const char* const msg,
                       const size_t length,
                       const std::chrono::milliseconds timeout) const noexcept;
    score::cpp::expected<size_t, Error> receive(char* const msg) const noexcept;
    /* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
    std::int32_t get_msg_size() const;
    std::string receive() const noexcept;
    /* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
    std::pair<ssize_t, bool> timed_receive(char* const msg, const std::chrono::milliseconds timeout) const noexcept;
    /* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
    std::pair<std::string, bool> timed_receive(const std::chrono::milliseconds timeout) const noexcept;
    score::cpp::expected_blank<score::os::Error> unlink() const noexcept;
    size_t get_id() const;
    score::cpp::expected<std::uint32_t, Error> get_mq_st_mode() const noexcept;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    MQueuePrivate(MQueuePrivate&) = delete;
    MQueuePrivate& operator=(MQueuePrivate&) = delete;
    MQueuePrivate(MQueuePrivate&&) = default;
    MQueuePrivate& operator=(MQueuePrivate&&) = default;
    ~MQueuePrivate();
}; /* KW_SUPPRESS_END:MISRA.LINKAGE.EXTERN:PIMPL implementation principle */

/************ class definition****************/
/* KW_SUPPRESS_START:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: */
/* Methods MQueue::MQueuePrivate::timed_receive and MQueue::timed_receive belong to different classes and can easily be
 * differentiated */
std::pair<ssize_t, bool> MQueue::MQueuePrivate::timed_receive(char* const msg,
                                                              const std::chrono::milliseconds timeout) const noexcept
{
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Methods MQueue::MQueuePrivate::timed_receive and MQueue::timed_receive belong to
     * different classes and can easily be differentiated */
    /* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
    if (msg == nullptr)
    {
        return {-1, false};
    }

    auto result = score::cpp::expected<ssize_t, score::os::Error>{};
    timespec times = score::common::timeout_in_timespec(timeout, std::chrono::system_clock::now());

    do
    {
        result = score::os::Mqueue::instance().mq_timedreceive(
            m_fd, msg, static_cast<size_t>(m_attr.mq_msgsize), nullptr, &times);
    } while ((!result.has_value()) && (result.error() == score::os::Error::Code::kOperationWasInterruptedBySignal));

    bool empty_queue{false};
    if (!result.has_value())
    {
        const auto error_code = ::score::os::geterrno();
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: */
        /* Using library-defined macro to ensure correct operation */
        if ((error_code == ETIMEDOUT) || (error_code == EAGAIN))
        { /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: */
            empty_queue = true;
        }
        else
        {
            return {-1, false};
        }
    }

    return {result.has_value() ? result.value() - 1 : 0, empty_queue};
} /* KW_SUPPRESS_END:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */

/* KW_SUPPRESS_START:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: */
/* Methods MQueue::MQueuePrivate::get_msg_size and MQueue::get_msg_size belong to different classes and can easily be
 * differentiated */
std::int32_t MQueue::MQueuePrivate::get_msg_size() const
{
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Methods MQueue::MQueuePrivate::get_msg_size and MQueue::get_msg_size belong to
     * different classes and can easily be differentiated */
    return static_cast<std::int32_t>(m_attr.mq_msgsize);
} /* KW_SUPPRESS_END:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */

/* KW_SUPPRESS_START:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: */
/* Methods MQueue::MQueuePrivate::timed_receive and MQueue::timed_receive belong to different classes and can easily be
 * differentiated */
std::pair<std::string, bool> MQueue::MQueuePrivate::timed_receive(
    const std::chrono::milliseconds timeout) const noexcept
{
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Methods MQueue::MQueuePrivate::timed_receive and MQueue::timed_receive belong to
     * different classes and can easily be differentiated */
    std::vector<char> msg(static_cast<std::uint32_t>(m_attr.mq_msgsize), 0);
    const auto pair = timed_receive(msg.data(), timeout);
    const std::pair<std::string, bool> pair_return = {std::string(msg.data()), pair.second};
    return pair_return;
} /* KW_SUPPRESS_END:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */

std::string MQueue::MQueuePrivate::create_name(const std::string name)
{
    std::string retval{'/'};
    score::cpp::ignore = retval.append(name);
    return retval;
}

std::string MQueue::MQueuePrivate::create_name(const size_t id) noexcept
{
    std::string retval{'/'};
    score::cpp::ignore = retval.append(std::to_string(id));
    return retval;
}

/* KW_SUPPRESS_START:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: */
/* Methods MQueue::MQueuePrivate::get_id and MQueue::get_id belong to different classes and can easily be differentiated
 */
std::size_t MQueue::MQueuePrivate::get_id() const
{
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Methods MQueue::MQueuePrivate::get_id and MQueue::get_id belong to different
     * classes and can easily be differentiated */
    return m_id;
} /* KW_SUPPRESS_END:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */

MQueue::MQueuePrivate::MQueuePrivate(/* KW_SUPPRESS:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
                                     const size_t id)
    : m_id(id), m_name(create_name(m_id))
{
    score::cpp::ignore = open();
}

/* KW_SUPPRESS_START:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
MQueue::MQueuePrivate::~MQueuePrivate()
{ /* KW_SUPPRESS_END:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
    score::cpp::ignore = score::os::Mqueue::instance().mq_close(m_fd);
}

MQueue::MQueuePrivate::MQueuePrivate(/* KW_SUPPRESS:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
                                     const std::string& name,
                                     const AccessMode mode,
                                     const size_t max_msg_size,
                                     const size_t max_msgs)
    : m_id(common::get_string_as_hash(name)), m_name(create_name(m_id))
{
    if ((mode == AccessMode::kCreate) || (mode == AccessMode::kCreateNonBlocking) ||
        (mode == AccessMode::kIfExistUseOthCreate))
    {
        score::cpp::ignore = open_create(max_msg_size, max_msgs, mode);
    }
    else
    {
        score::cpp::ignore = open();
        /* LCOV_EXCL_BR_START: Unable to run the false assert case */
        /* KW_SUPPRESS_START:MISRA.CONV.INT.SIGN:Both operands are of type unsigned long */
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: */
        /* False positive, as UL is a suffix, not a macro */
        assert(max_msg_size == 100UL);  // cannot modify msg_size by using
        assert(max_msgs == 10UL);       // cannot modify max amount of messages by using
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: */
        /* KW_SUPPRESS_END:MISRA.CONV.INT.SIGN:Both operands are of type unsigned long */
        /* LCOV_EXCL_BR_STOP */
    }
}

/* KW_SUPPRESS_START:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
MQueue::MQueuePrivate::MQueuePrivate(
    /* KW_SUPPRESS_END:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
    const std::string& name) noexcept
    : m_id(common::get_string_as_hash(name)), m_name(create_name(name))
{
    score::cpp::ignore = open();
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: */
/* Methods MQueue::MQueuePrivate::unlink and MQueue::unlink belong to different classes and can easily be differentiated
 */
/* KW_SUPPRESS_START:MISRA.LINKAGE.EXTERN:PIMPL model hides the implementation */
score::cpp::expected_blank<score::os::Error> MQueue::MQueuePrivate::unlink() const noexcept
{ /* KW_SUPPRESS_END:MISRA.LINKAGE.EXTERN:PIMPL model hides the implementation */
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Methods MQueue::MQueuePrivate::unlink and MQueue::unlink belong to different
     * classes and can easily be differentiated */
    const auto ret = score::os::Mqueue::instance().mq_unlink(m_name.c_str());
    if (!ret.has_value())
    {
        // MQueuePrivate::open_create call unlink before creating new mqueue
        // If there is no mqueue with specified name just ignore, return otherwise
        if (ret.error() != ::score::os::Error::Code::kNoSuchFileOrDirectory)
        {
            return score::cpp::make_unexpected(score::os::Error::createFromErrno());
        }
    }
    return ret;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: */
/* Methods MQueue::MQueuePrivate::open and MQueue::open belong to different classes and can easily be differentiated */
/* KW_SUPPRESS_START:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
score::cpp::expected_blank<score::os::Error> MQueue::MQueuePrivate::open() noexcept
{
    /* KW_SUPPRESS_END:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: */
    /* Methods MQueue::MQueuePrivate::open and MQueue::open belong to different classes and can easily be differentiated
     */
    const score::os::Mqueue::OpenFlag flags = score::os::Mqueue::OpenFlag::kReadWrite;
    const auto result = score::os::Mqueue::instance().mq_open(m_name.c_str(), flags);
    m_fd = result.has_value() ? result.value() : -1;

    if (m_fd == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    const auto getAttrRet = score::os::Mqueue::instance().mq_getattr(m_fd, m_attr);
    if (!getAttrRet.has_value())
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }

    return score::cpp::expected_blank<score::os::Error>{};
}

score::cpp::expected_blank<score::os::Error>
MQueue::MQueuePrivate::open_create(/* KW_SUPPRESS:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
                                   const size_t max_msg_size,
                                   const size_t max_msgs,
                                   const AccessMode mode) noexcept
{
    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: */
    /* False positive, as UL is a suffix, not a macro */
    /* LCOV_EXCL_BR_START: Unable to run the false assert case */
    assert(max_msg_size > 1UL); /* KW_SUPPRESS:MISRA.CONV.INT.SIGN:Both operands are of type unsigned long */
    assert(max_msgs > 1UL);     /* KW_SUPPRESS:MISRA.CONV.INT.SIGN:Both operands are of type unsigned long */
    /* LCOV_EXCL_BR_STOP */
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION: */
    if (mode != AccessMode::kIfExistUseOthCreate)
    {
        // delete if it is already there, if not ignore
        score::cpp::ignore = unlink();
    }

    const score::os::Mqueue::ModeFlag perm{
        score::os::Mqueue::ModeFlag::kReadUser | score::os::Mqueue::ModeFlag::kWriteUser |
        score::os::Mqueue::ModeFlag::kReadGroup | score::os::Mqueue::ModeFlag::kWriteGroup |
        score::os::Mqueue::ModeFlag::kReadOthers}; /* KW_SUPPRESS:MISRA.BITS.*:Safe macro OR use */
    score::os::Mqueue::OpenFlag flags =
        score::os::Mqueue::OpenFlag::kCreate |
        score::os::Mqueue::OpenFlag::kReadWrite; /* KW_SUPPRESS:MISRA.BITS.*:Safe macro OR use */
    if (mode == AccessMode::kCreateNonBlocking)
    {
        flags = flags | score::os::Mqueue::OpenFlag::kNonBlocking; /* KW_SUPPRESS:MISRA.BITS.NOT_UNSIGNED:Operand has
                                                                    non-negative value */
    }
    m_attr.mq_maxmsg = static_cast<std::int64_t>(max_msgs);
    m_attr.mq_msgsize = static_cast<std::int64_t>(max_msg_size);

    const auto result = ::score::os::Mqueue::instance().mq_open(m_name.c_str(), flags, perm, &m_attr);
    m_fd = result.has_value() ? result.value() : -1;

    if (m_fd == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }

    const score::os::Stat::Mode statPerm{score::os::Stat::Mode::kReadUser | score::os::Stat::Mode::kWriteUser |
                                       score::os::Stat::Mode::kReadGroup | score::os::Stat::Mode::kWriteGroup |
                                       score::os::Stat::Mode::kReadOthers};
    // Set permissions explicitly in case mq_open does not set +w for the group
    const auto fchmodRet = score::os::Stat::instance().fchmod(m_fd, statPerm);
    if (!fchmodRet.has_value())
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return score::cpp::expected_blank<score::os::Error>{};
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: */
/* Methods MQueue::MQueuePrivate::send and MQueue::send belong to different classes and can easily be differentiated */
void MQueue::MQueuePrivate::send(/* KW_SUPPRESS:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
                                 const std::string& msg) const
{
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Methods MQueue::MQueuePrivate::send and MQueue::send belong to different classes
     * and can easily be differentiated */
    score::cpp::ignore = send(msg.c_str(), msg.length());
}

/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used as an argument of a wrapped function */
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: */
/* Methods MQueue::MQueuePrivate::send and MQueue::send belong to different classes and can easily be differentiated */
score::cpp::expected_blank<score::os::Error>
MQueue::MQueuePrivate::send(/* KW_SUPPRESS:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
                            const char* const msg,
                            const size_t length) const
{
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Methods MQueue::MQueuePrivate::send and MQueue::send belong to different classes
     * and can easily be differentiated */
    const auto ret = score::os::Mqueue::instance().mq_send(m_fd, msg, length + 1U, 0);
    if (!ret.has_value())
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }

    return ret;
} /* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used as an argument of a wrapped function */

/* KW_SUPPRESS_START:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used as an argument of a wrapped function */
ssize_t MQueue::MQueuePrivate::timed_send(/* KW_SUPPRESS:MISRA.VAR.HIDDEN:Wrapper function definition */
                                          const char* const msg,
                                          const size_t length,
                                          const std::chrono::milliseconds timeout) const noexcept
{
    /* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used as an argument of a wrapped function */
    timespec times = score::common::timeout_in_timespec(timeout, std::chrono::system_clock::now());
    auto result = score::cpp::expected_blank<score::os::Error>{};

    do
    {
        result = score::os::Mqueue::instance().mq_timedsend(m_fd, msg, length + 1U, 0, &times);
    } while ((!result.has_value()) && (result.error() == score::os::Error::Code::kOperationWasInterruptedBySignal));

    return result.has_value() ? 0 : -1;
} /* KW_SUPPRESS_END:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */

/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used as an argument of a wrapped function */
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: */
/* Methods MQueue::MQueuePrivate::receive and MQueue::receive belong to different classes and can easily be
 * differentiated */
score::cpp::expected<size_t, Error>
MQueue::MQueuePrivate::receive(/* KW_SUPPRESS:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
                               char* const msg) const noexcept
{
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: */
    /* Methods MQueue::MQueuePrivate::receive and MQueue::receive belong to different classes and can easily be
     * differentiated */
    const auto ret = score::os::Mqueue::instance().mq_receive(m_fd, msg, static_cast<size_t>(m_attr.mq_msgsize), nullptr);

    if (!ret.has_value())
    {
        if (ret.error() != score::os::Error::Code::kResourceTemporarilyUnavailable)
        {
            return score::cpp::make_unexpected(score::os::Error::createFromErrno());
        }
    }
    else
    {
        return ret.value();
    }
    return score::cpp::expected<size_t, score::os::Error>{};
} /* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used as an argument of a wrapped function */

/* KW_SUPPRESS_START:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: */
/* Methods MQueue::MQueuePrivate::receive and MQueue::receive belong to different classes and can easily be
 * differentiated */
std::string MQueue::MQueuePrivate::receive() const noexcept
{
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Methods MQueue::MQueuePrivate::receive and MQueue::receive belong to different
     * classes and can easily be differentiated */
    std::vector<char> msg(static_cast<std::uint32_t>(m_attr.mq_msgsize), 0);
    score::cpp::ignore = receive(msg.data());
    return std::string{msg.data()};
} /* KW_SUPPRESS_END:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */

/* KW_SUPPRESS_START:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: */
/* Methods MQueue::MQueuePrivate::get_mq_st_mode and MQueue::get_mq_st_mode belong to different classes and can easily
 * be differentiated */
score::cpp::expected<std::uint32_t, Error> MQueue::MQueuePrivate::get_mq_st_mode() const noexcept
{
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Methods MQueue::MQueuePrivate::get_mq_st_mode and MQueue::get_mq_st_mode belong
     * to different classes and can easily be differentiated */
    struct score::os::StatBuffer f_stat{};
    const auto fstatRet = score::os::Stat::instance().fstat(m_fd, f_stat);
    if (!fstatRet.has_value())
    {
        return score::cpp::make_unexpected(fstatRet.error());
    }
    return (f_stat.st_mode);
} /* KW_SUPPRESS_END:MISRA.LINKAGE.EXTERN:PIMPL model obscures the implementation */

std::int32_t MQueue::get_msg_size() const
{
    return m_pointer->get_msg_size();
}

/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used as an argument of called function */
void MQueue::receive(char* const msg) const
{
    score::cpp::ignore = m_pointer->receive(msg);
} /* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used as an argument of called function */

MQueue::MQueue(const std::string& name, const AccessMode mode, const size_t max_msg_size, const size_t max_msgs)
{
    m_pointer = std::make_unique<MQueuePrivate>(name, mode, max_msg_size, max_msgs);
}

MQueue::MQueue(const std::string& name)
{
    m_pointer = std::make_unique<MQueuePrivate>(name);
}

MQueue::MQueue(const size_t id)
{
    m_pointer = std::make_unique<MQueuePrivate>(id);
}

void MQueue::send(const std::string& msg) const
{
    m_pointer->send(msg);
}

score::cpp::expected_blank<score::os::Error> MQueue::send(const char* const msg, const size_t length) const
{
    return m_pointer->send(msg, length);
}

ssize_t MQueue::timed_send(const char* const msg, const size_t length, const std::chrono::milliseconds timeout) const
{
    return m_pointer->timed_send(msg, length, timeout);
}

std::string MQueue::receive() const
{
    return m_pointer->receive();
}

score::cpp::expected_blank<score::os::Error> MQueue::unlink() const
{
    return m_pointer->unlink();
}

std::size_t MQueue::get_id() const
{
    return m_pointer->get_id();
}

std::pair<ssize_t, bool> MQueue::timed_receive(char* const msg, const std::chrono::milliseconds timeout) const
{
    return m_pointer->timed_receive(msg, timeout);
}

std::pair<std::string, bool> MQueue::timed_receive(const std::chrono::milliseconds timeout) const
{
    return m_pointer->timed_receive(timeout);
}

score::cpp::expected<std::uint32_t, Error> MQueue::get_mq_st_mode() const noexcept
{
    auto result = m_pointer->get_mq_st_mode();
    if (!result.has_value())
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result.value();
}

MQueue::~MQueue() = default;
MQueue::MQueue(MQueue&&) noexcept = default;
MQueue& MQueue::operator=(MQueue&&) noexcept = default;

}  // namespace os
}  // namespace score
