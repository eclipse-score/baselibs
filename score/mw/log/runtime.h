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
#ifndef SCORE_MW_LOG_RUNTIME_H
#define SCORE_MW_LOG_RUNTIME_H

#include "score/mw/log/logger_container.h"
#include "score/mw/log/recorder.h"

#include <score/memory.hpp>

namespace score
{
namespace singleton
{

// Suppress "AUTOSAR C++14 M3-2-3".
// Rationale: This is a forward declaration that does not vioalate this rule.
template <typename Object, template <class> class AtomicIndirectorType>
// coverity[autosar_cpp14_m3_2_3_violation]
class MeyerSingleton;

}  // namespace singleton

namespace mw
{
namespace log
{
namespace detail
{
class RecorderFactory;

/// \brief The runtime is a singleton that is responsible for providing the usage of the current Recorder. Due to the
/// distributed nature of the logging library, we need a central place to know which logging infrastructure to use. This
/// enables us logging distributed within our code base without the necessity to inject it everywhere.
///
/// \details We thought about not providing such a runtime and instead injecting the necessary logging recorder
/// everywhere. The main reason against this approach are:
/// 1) cluttering of dependencies. We need logging everywhere, so it would be a big dependency mess
/// 2) We have currently logging in a distributed manner, refactoring to an injecting pattern would require tremendous
/// efforts in the overall codebase
class Runtime final
{
    // Suppress "AUTOSAR C++14 A11-3-1"
    // The Runtime class is intended to only be constructed as a singleton object. Therefore, the constructor should be
    // private and the user should initialize the Runtime via GetRecorder(). The MeyerSingleton class is used to create
    // the Runtime singleton. Therefore, MeyerSingleton needs access to the private Runtime constructor.
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class singleton::MeyerSingleton<Runtime, memory::shared::AtomicIndirectorReal>;

  public:
    /// \brief Implements a singleton, so no copying or moving
    Runtime(const Runtime&) noexcept = delete;
    Runtime(Runtime&&) noexcept = delete;
    Runtime& operator=(const Runtime&) noexcept = delete;
    Runtime& operator=(Runtime&&) noexcept = delete;

    ~Runtime() = default;
    /// \brief Getter function for the currently configured recorder
    ///
    /// \details Be careful, you cannot use GetRecorder() and SetRecorder() in a thread-safe manner. We decided against
    /// a synchronisation primitive since in production code has no real use-case for usage of SetRecorder() and we want
    /// to avoid the cost of the synchronization for production code if race conditions only exists for testing. This
    /// also means that it is safe to invoke `GetRecorder()` concurrently.
    /// \return pointer to either the default recorder or the last set recorder
    static Recorder& GetRecorder() noexcept;

    /// \brief Inject a Recorder into the logging framework
    ///
    /// \param recorder The recorder that from now shall be used by any logging call
    ///
    /// \details Be careful, you cannot use GetRecorder() and SetRecorder() in a thread-safe manner. We decided against
    /// a synchronisation primitive since in production code no real use-case for SetRecorder() exists and we want to
    /// avoid the cost of the synchronization for production code if only exists for testing.
    ///
    /// Be advised that it is the responsibility of the user of this API, to ensure that the set recorder is alive as
    /// long as any other functionality might invoke logging statements!
    static void SetRecorder(Recorder* const recorder,
                            score::cpp::pmr::memory_resource* memory_resource = score::cpp::pmr::get_default_resource()) noexcept;

    static Recorder& GetFallbackRecorder() noexcept;

    static score::mw::log::LoggerContainer& GetLoggerContainer() noexcept;

  private:
    explicit Runtime(Recorder* const recorder, score::cpp::pmr::memory_resource* memory_resource) noexcept;

    static Runtime& Instance(Recorder* const recorder,
                             score::cpp::pmr::memory_resource* memory_resource = score::cpp::pmr::get_default_resource()) noexcept;
    LoggerContainer logger_container_instance_{};

    Recorder* recorder_instance_ = nullptr;
    std::unique_ptr<Recorder> default_recorder_ = nullptr;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_RUNTIME_H
