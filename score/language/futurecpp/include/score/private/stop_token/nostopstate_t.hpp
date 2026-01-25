/********************************************************************************
 * Copyright (c) 2021 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2021 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.StopToken component
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_STOP_TOKEN_NOSTOPSTATE_T_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_STOP_TOKEN_NOSTOPSTATE_T_HPP

namespace score::cpp
{

/// \brief Unit type intended for use as a placeholder in score::cpp::stop_source
/// non-default constructor, that makes the constructed score::cpp::stop_source empty with no associated stop-state.
struct nostopstate_t
{
    explicit nostopstate_t() = default;
};

/// \brief This is a constant object instance of score::cpp::nostopstate_t
/// for use in constructing an empty score::cpp::stop_source, as a placeholder value in the non-default constructor.
extern const nostopstate_t nostopstate;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_STOP_TOKEN_NOSTOPSTATE_T_HPP
