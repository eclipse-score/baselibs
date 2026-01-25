/********************************************************************************
 * Copyright (c) 2024 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2024 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.ThisThread component
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_THREAD_THIS_THREAD_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_THREAD_THIS_THREAD_HPP

#include <score/private/thread/thread.hpp>
#include <score/private/utility/ignore.hpp>

// Header part of safety qualification
// broken_link_s/:b:/r/teams/SafeSoftwarePlatform/Freigegebene%20Dokumente/Ironman/safety/mpad/tuv/EZ_653_22_23_RL_2023_08_10_QOS_2_2_4.pdf?csf=1&web=1&e=FTMJRu
//{
#include <pthread.h>
#include <sched.h>
//}

namespace score::cpp
{
namespace this_thread
{

/// \brief Returns the id of the current thread.
///
/// \return id of the current thread.
inline score::cpp::thread::id get_id() noexcept { return ::pthread_self(); }

/// \brief Provides a hint to the implementation to reschedule the execution of threads, allowing other threads to run
inline void yield() noexcept { score::cpp::ignore = ::sched_yield(); }

} // namespace this_thread
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_THREAD_THIS_THREAD_HPP
