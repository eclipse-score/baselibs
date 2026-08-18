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
#include "score/os/qnx/secpolev.h"

// see get_process_name()
#include <cerrno>

namespace score
{
namespace os
{
namespace qnx
{

// LCOV_EXCL_START
// Exclude this file from code coverage checks, because of compilation error introduced by Ticket-161297.
// Ticket-161297 use a newer version of xpad_qnx_sdp, but the qemu set-up is using an old mPAD QNX SDP.
// After the qemu set-up has been adapted, remove the macro, and start to write unit test.
class SecpolEvImpl final : public SecpolEv
{
  public:
    score::cpp::expected_blank<score::os::Error> init(const unsigned flags,
                                                      const unsigned buffer_count,
                                                      const unsigned flush_interval) const noexcept override
    {
        if (::secpolev_init(flags, buffer_count, flush_interval) != 0)
        {
            return score::cpp::make_unexpected(score::os::Error::createFromErrno());
        }
        return {};
    }

    score::cpp::expected_blank<score::os::Error> shutdown(secpolev_shutdown_type_t type) const noexcept override
    {
        // secpolev_shutdown interface changes in QOS227 (ES15 22.0.1.0)
        // Adapting to the new interface is not straightforward as the
        // CI job "xpad-test-unit-qnx-1" was failing after the change.
        //
        // Further investigation showed that the qemu setup is using old
        // mPAD QNX SDP, causing a lot of compatibility issues and failures in
        // the ddad check job
        //
        // As an interm solution, this chunck of code will be commented out to
        // continue with checks and fixing of other errors, till the responsible
        // team for the unit-test environment provide a proper solution.
        //
        // Latest commit with the interface change as a reference:
        // "888c26bac3c849ce21c05e1f29811714deaa5085"
        //
        if (::secpolev_shutdown(type) != 0)
        {
            return score::cpp::make_unexpected(score::os::Error::createFromErrno());
        }
        return {};
    }

    score::cpp::expected<secpolev_event_type_t, score::os::Error> wait_event(unsigned* flags) const noexcept override
    {
        secpolev_event_type_t result = secpolev_wait_event(flags);
        if (result == SECPOLEV_EVENT_ERROR)
        {
            return score::cpp::make_unexpected(score::os::Error::createFromErrno());
        }
        return result;
    }

    score::cpp::expected_blank<score::os::Error> flush(void) const noexcept override
    {
        if (::secpolev_flush() != 0)
        {
            return score::cpp::make_unexpected(score::os::Error::createFromErrno());
        }
        return {};
    }

    score::cpp::expected<const char*, score::os::Error> get_process_name() const noexcept override
    {
        // Documentation on secpolev_get_process_name() says:
        // "Note that in the initial stage of an exec*() call, the process has
        // no name but this is not considered an error and errno will not have
        // been set."
        // In that case NULL is returned, so we have to check if errno was set
        // to distinguish errors from legitimate NULLs. Lovely.
        // This is the fallback that catches errors for operations where inputs could not be checked in advance.
        // Suppress "AUTOSAR C++14 M19-3-1", The rule states: "The error indicator errno shall not be used."
        // coverity[autosar_cpp14_m19_3_1_violation]
        errno = 0;
        const char* result = secpolev_get_process_name();
        // coverity[autosar_cpp14_m19_3_1_violation] see above
        if ((NULL == result) && (errno != 0))
        {
            return score::cpp::make_unexpected(score::os::Error::createFromErrno());
        }
        return result;
    }

    uint64_t get_dropped_event_count() const noexcept override
    {
        return secpolev_get_dropped_count();
    }

    score::cpp::expected<const char*, score::os::Error> get_process_type() const noexcept override
    {
        const char* result = secpolev_get_process_type();
        if (NULL == result)
        {
            return score::cpp::make_unexpected(score::os::Error::createFromErrno());
        }
        return result;
    }

    score::cpp::expected<pid_t, score::os::Error> get_pid() const noexcept override
    {
        pid_t result = secpolev_get_pid();
        if (0 == result)
        {
            return score::cpp::make_unexpected(score::os::Error::createFromErrno());
        }
        return result;
    }

    score::cpp::expected_blank<score::os::Error> get_ability_info(unsigned* id,
                                                                  const char** name,
                                                                  unsigned* flags,
                                                                  uint64_t* start,
                                                                  uint64_t* end) const noexcept override
    {
        if (secpolev_get_ability_info(id, name, flags, start, end) != 0)
        {
            return score::cpp::make_unexpected(score::os::Error::createFromErrno());
        }
        return {};
    }

    score::cpp::expected_blank<score::os::Error> get_path_info(const char** path, secpolev_path_event_status_t* status)
        const noexcept override
    {
        if (secpolev_get_path_info(path, status) != 0)
        {
            return score::cpp::make_unexpected(score::os::Error::createFromErrno());
        }
        return {};
    }

    score::cpp::expected_blank<score::os::Error> get_custom_perm_info(const char** class_name,
                                                                      const char** perm_name,
                                                                      const char** obj_type) const noexcept override
    {
        if (secpolev_get_custom_perm_info(class_name, perm_name, obj_type) != 0)
        {
            return score::cpp::make_unexpected(score::os::Error::createFromErrno());
        }
        return {};
    }
};
// LCOV_EXCL_STOP

}  // namespace qnx
}  // namespace os
}  // namespace score

// LCOV_EXCL_START
// Exclude this file from code coverage checks, because of compilation error introduced by Ticket-161297.
// Ticket-161297 use a newer version of xpad_qnx_sdp, but the qemu set-up is using an old mPAD QNX SDP.
// After the qemu set-up has been adapted, remove the macro, and start to write unit test.
score::os::qnx::SecpolEv& score::os::qnx::SecpolEv::instance() noexcept
{
    static score::os::qnx::SecpolEvImpl instance;
    return select_instance(instance);
}
// LCOV_EXCL_STOP
