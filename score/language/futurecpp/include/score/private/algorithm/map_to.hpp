/********************************************************************************
 * Copyright (c) 2016 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2016 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.Algorithm component
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_ALGORITHM_MAP_TO_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_ALGORITHM_MAP_TO_HPP

#include <score/optional.hpp>
#include <score/type_traits.hpp>

#include <algorithm>
#include <iterator>
#include <tuple>

namespace score::cpp
{

///
/// \brief Maps key of type InputType to associated value of type OutputType according to given multidirectional map.
///
/// This function implements the find/map operation on a multidirectional map between heterogeneous values.
/// It accepts any container as a map that defines this mapping using the std::tuple, std::pair.
///
/// More precisely, if the value of InputType exist multiple times then the first matching entry is returned.
///
/// \tparam OutputType Requested mapping type.
/// \tparam InputType  Type of mapping key.
/// \param key         Value serving as key in the map.
/// \param map         A container defining the mapping.
/// \return            Value of type OutputType if mapping was successful, empty optional otherwise. If more than
///                    one container entry has an InputType equal to key, OutputType of the first entry is taken.
template <typename OutputType, typename InputType, typename Map>
score::cpp::optional<OutputType> map_to(const InputType& key, const Map& map)
{
    using EntryType = typename Map::value_type;
    static_assert(is_pair_v<EntryType> || is_tuple_v<EntryType>,
                  "multidirectional map shall contain entries of type pair or tuple");
    const auto is_equal = [&key](const auto& map_entry) { return std::get<InputType>(map_entry) == key; };

    const auto it = std::find_if(std::cbegin(map), std::cend(map), is_equal);
    if (it == std::end(map))
    {
        return {};
    }
    return std::get<OutputType>(*it);
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_ALGORITHM_MAP_TO_HPP
