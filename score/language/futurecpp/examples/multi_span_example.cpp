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

#include <algorithm>
#include <score/multi_span.hpp>

int main()
{
    /// [multi_span-ctor]
    std::size_t const x = 12;
    std::size_t const y = 8;
    std::size_t const z = 6;

    int fixed_size_array[z][y][x] = {0};

    score::cpp::bounds<3> bnds = score::cpp::make_bounds(z, y, x); // same as `{Z, Y, X}`
    score::cpp::multi_span<int, 3> view(bnds, &fixed_size_array[0][0][0]);
    // same as `score::cpp::as_multi_span(bnds, fixed_size_array)` [C++11]
    // score::cpp::multi_span<int> view1(fixed_size_array); - not yet supported
    /// [multi_span-ctor]

    /// [multi_span-access]
    score::cpp::offset<3> idx = score::cpp::make_offset(5, 3, 2); // or just `{5,3,2}` [C++11]
    view[idx] = 28;                                 // also fine `view[{5, 3, 2}] = 28` [C++11]
    /// [multi_span-access]

    /// [multi_span-range-loop]
    for (auto it = view.bounds().begin(); it != view.bounds().end(); ++it)
    {
        score::cpp::offset<3> const& idx_r = *it;
        auto z = idx_r[0];
        auto y = idx_r[1];
        auto x = idx_r[2]; // least significant dimension, incremented first
        view[idx_r] = static_cast<int>(x * 2 + y - z);
    }
    /// [multi_span-range-loop]

    /// [multi_span-iterator]
    score::cpp::bounds<3>::const_iterator first = std::begin(view.bounds()); // or view.bounds().begin()
    score::cpp::bounds<3>::const_iterator last = std::end(view.bounds());    // or view.bounds().end()

    // Dereferencing an iterator returns a `const score::cpp::offset<Rank>`. Indices are always immutable.
    std::for_each(first, last, [&view](const score::cpp::offset<3>& off) {
        auto z = off[0];
        auto y = off[1];
        auto x = off[2];
        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(view[off] == x * 2 + y - z);
    });
    /// [multi_span-iterator]

    /// [multi_span-slice]
    int const z0 = 5, y0 = 3;
    score::cpp::multi_span<int, 2> slice2d = view[z0];     // a 2d slice in the x-y plane
    score::cpp::multi_span<int, 1> slice1d = view[z0][y0]; // a row in x, also the contiguous dimension

    SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(slice2d[score::cpp::make_offset(3, 2)] == 2);
    /// [multi_span-slice]

    /// [multi_span-slice1D]
    SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(slice1d[score::cpp::make_offset(2)] == 2);
    SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(slice1d[2] == 2);
    /// [multi_span-slice1D]

#if 0
    /// [multi_span-section]
    score::cpp::offset<3> origin = {6, 3, 2};
    score::cpp::bounds<3> window = {3, 3, 2};
    auto section = av.section(origin, window);

    // Work with just this section of the data int sum =
    std::accumulate(std::begin(section.bounds()), std::end(section.bounds()), 0, [&](int a, score::cpp::offset<3> idx) {
        return a + section[idx];
    });
    /// [multi_span-section]

    /// [multi_span-strided-data]
    score::cpp::multi_span<int,3> view = .. // as before
    The strides in Z and Y are as calculated as for `view`, but the stride in X is no longer 1
    score::cpp::bounds<3> new_extents = { Z, Y, X / 3 };
    score::cpp::offset<3> new_stride = {view.stride()[0], view.stride()[1], 3};
    score::cpp::strided_multi_span<int, 3> sav(vec.data(), newExtents, newStride);

    for (auto& idx : sav.bounds())
    {
        // do something with sav[idx]
    }
    /// [multi_span-strided-data]
#endif

    return 0;
}
