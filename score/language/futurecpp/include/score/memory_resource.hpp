/********************************************************************************
 * Copyright (c) 2019 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2019 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.Pmr.MemoryResource component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_MEMORY_RESOURCE_HPP
#define SCORE_LANGUAGE_FUTURECPP_MEMORY_RESOURCE_HPP

#include <score/private/memory_resource/memory_resource.hpp>              // IWYU pragma: export
#include <score/private/memory_resource/monotonic_buffer_resource.hpp>    // IWYU pragma: export
#include <score/private/memory_resource/polymorphic_allocator.hpp>        // IWYU pragma: export
#include <score/private/memory_resource/pool_options.hpp>                 // IWYU pragma: export
#include <score/private/memory_resource/resource_adaptor.hpp>             // IWYU pragma: export
#include <score/private/memory_resource/unsynchronized_pool_resource.hpp> // IWYU pragma: export

#include <score/algorithm.hpp> // to be removed but removing them may break downstream

#endif // SCORE_LANGUAGE_FUTURECPP_MEMORY_RESOURCE_HPP
