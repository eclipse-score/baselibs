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
#include "score/language/safecpp/safe_math/details/floating_point_environment.h"

#include "score/language/safecpp/safe_math/details/comparison/comparison.h"
#include "score/language/safecpp/safe_math/error.h"

#include <score/utility.hpp>

#include <cerrno>
#include <cfenv>
#include <cmath>
#include <limits>

score::safe_math::details::FloatingPointEnvironment::FloatingPointEnvironment()
{
    Clear();
}

score::safe_math::details::FloatingPointEnvironment::~FloatingPointEnvironment()
{
    Clear();
}

score::ResultBlank score::safe_math::details::FloatingPointEnvironment::Test() const noexcept
{
    static_assert((math_errhandling & (MATH_ERREXCEPT | MATH_ERRNO)) != 0,
                  "At least one error reporting scheme must be supported");

    bool error_happened{false};
    // According to the C++ standard, math_errhandling and MATH_ERREXCEPT are guaranteed to
    // contain only specific bitmask values that are safe to cast to unsigned,
    // (https://en.cppreference.com/w/cpp/numeric/math/math_errhandling) even if their
    // underlying type is signed.
    if ((static_cast<std::uint32_t>(math_errhandling) & static_cast<std::uint32_t>(MATH_ERREXCEPT)) != 0U)
    {
// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// Checks for signals from the FPU in a hardware-agnostic fashion.
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(FE_INEXACT)
        // If the FPU supports FE_INEXACT, we explicitly do not check it. This is because FE_INEXACT will be triggered
        // by any amount of inaccuracy. Since there is no way to tell sufficiently accurate operations from
        // insufficiently accurate operations, we ignore this check completely.
        // According to the C++ standard, FE_INEXACT is guaranteed to be a power of 2
        // (https://en.cppreference.com/w/cpp/numeric/math/math_errhandling), even if their
        // underlying type is signed.
        constexpr auto exceptions = FE_ALL_EXCEPT & static_cast<std::uint32_t>(~FE_INEXACT);
// coverity[autosar_cpp14_a16_0_1_violation]
#else
        constexpr auto exceptions = FE_ALL_EXCEPT;
// coverity[autosar_cpp14_a16_0_1_violation]
#endif
        // Before we can check if exceptions is bigger than int, we need to make sure that
        static_assert(CmpLessEqual(exceptions, std::numeric_limits<int>::max()), "exceptions exceeds max int value");
        // Depending on the implementation, "exceptions" may be promoted to an unsigned integer. Therefore, converting
        // from unsigned to signed will result in data loss if the value exceeds the maximum signed value.
        // This statement is checked at compile time.
        // NOLINTNEXTLINE(cppcoreguidelines-narrowing-conversions): See above
        error_happened = std::fetestexcept(static_cast<int>(exceptions)) != 0;
    }
    else if ((static_cast<std::uint32_t>(math_errhandling) & static_cast<std::uint32_t>(MATH_ERRNO)) != 0U)
    {
        // EDOM and ERANGE are defined in ISO/IEC 14882:2003. This is the fallback that catches
        // errors for operations where inputs could not be checked in advance.
        // Suppress "AUTOSAR C++14 M19-3-1", The rule states: "The error indicator errno shall not be used."
        // coverity[autosar_cpp14_m19_3_1_violation]
        error_happened = (errno == ERANGE) || (errno == EDOM);
    }

    return error_happened ? score::MakeUnexpected(ErrorCode::kUnknown) : score::ResultBlank{};
}

void score::safe_math::details::FloatingPointEnvironment::Clear() noexcept
{
    // In case that clearing exceptions fails, there is nothing that we can do. The user might get an error in a
    // followup floating point operation that succeded. This is better than terminating in the sense that still give the
    // posibility to the user to decide what to do. If it is a problem for the user that floating point operation fails,
    // they can still terminate or go to a safe state. Additionally the user could also check  and react on the
    // exception when the command fails. It is worth to mention that this is not an expected scenario, if as a user can
    // proof that clearing the exception failed, a bug can be reported.
    score::cpp::ignore = std::feclearexcept(FE_ALL_EXCEPT);

    // Interaction with errno unavoidable since this class is the fallback that catches errors for operations
    // where inputs could not be checked in advance. Setting errno to zero is required since some floating point
    // environments do not interact with errno. For these environments we must make sure to not read an error from
    // some previous user operation.
    // Suppress "AUTOSAR C++14 M19-3-1", The rule states: "The error indicator errno shall not be used."
    // coverity[autosar_cpp14_m19_3_1_violation]
    errno = 0;
}
