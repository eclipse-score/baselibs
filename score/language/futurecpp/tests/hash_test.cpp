/********************************************************************************
 * Copyright (c) 2020 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2020 Contributors to the Eclipse Foundation
///

#include <score/hash.hpp>
#include <score/hash.hpp> // test include guard

#include <array>
#include <cstddef>
#include <cstring>
#include <set>
#include <string>

#include <gtest/gtest.h>

namespace
{
// generated with
// sort --random-sort /usr/share/dict/american-english
// | head -n 102 | awk 'BEGIN { printf("{\n"); } { printf(" \"%s\",\n", $1); } END { print("};\n"); }'
static const std::vector<std::string> samples = {
    "Chernomyrdin's", "remounted",    "madwomen",       "pensions",     "uncooked",       "jasmines",
    "alley's",        "decorative",   "Orr's",          "vector",       "terrifies",      "Jennie's",
    "headsets",       "noticeboard",  "denials",        "reassert",     "joyride's",      "fluorite",
    "peacemakers",    "ration's",     "lingerer's",     "gather's",     "Katharine's",    "superintending",
    "Handel's",       "corms",        "justest",        "Arnulfo's",    "roomer",         "curving",
    "snippiest",      "Cross's",      "airier",         "malfeasance",  "seventeen",      "ribbons",
    "accusing",       "tauter",       "Southwest's",    "parley",       "dredging",       "five",
    "duty",           "newsletters",  "noncooperation", "agricultural", "trefoils",       "shopper's",
    "reefed",         "betterment's", "listener's",     "Aurora",       "cultivation",    "quadruped",
    "weasel's",       "scamps",       "transcendental", "photon's",     "Forrest",        "truce's",
    "Ozarks's",       "recover",      "tensor",         "Sui's",        "entrusting",     "Clemson",
    "whiling",        "diseases",     "cataclysm",      "crucifixions", "boneless",       "exclusively",
    "tampers",        "outputs",      "hazarding",      "incompetents", "resuscitated",   "packages",
    "alludes",        "dinnering",    "trader's",       "jazzy",        "cries",          "cabanas",
    "flash",          "kilowatt's",   "mullets",        "Novokuznetsk", "timelessness's", "silt's",
    "kumquats",       "pigment's",    "abhor",          "knight's",     "chamois's",      "Ephesus",
    "Tenochtitlan's", "Turkestan",    "indentures",     "firmware",     "Ruben's",        "Stolypin",
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17948197
TEST(hash_test, check_for_equal_hashes_for_same_keys)
{
    const char* const key = "abc12344321cba";
    EXPECT_EQ(score::cpp::hash_bytes(key, std::strlen(key)), score::cpp::hash_bytes(key, std::strlen(key)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17948197
TEST(hash_test, check_for_equal_hashes_for_at_once_and_continued_hashing)
{
    const std::array<std::uint8_t, 8> input{0, 1, 2, 3, 4, 5, 6, 7};
    constexpr std::size_t half_size{input.size() / 2};

    const std::size_t hash_at_once{score::cpp::hash_bytes_fnv1a(input.data(), input.size())};

    const std::size_t hash_first_part{score::cpp::hash_bytes_fnv1a(input.data(), half_size)};
    const std::size_t hash_continued{
        score::cpp::hash_bytes_fnv1a(input.data() + half_size, input.size() - half_size, hash_first_part)};

    EXPECT_EQ(hash_at_once, hash_continued);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17948197
TEST(hash_test, null_input_returns_offset_basis_modulo_32bit)
{
    EXPECT_EQ(score::cpp::hash_bytes_fnv1a<std::uint32_t>(nullptr, 0U), 2166136261U);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17948197
TEST(hash_test, one_zero_input_returns_offset_basis_multiplied_by_prime_modulo_32bit)
{
    const std::uint64_t expected64 = 2166136261UL * 16777619UL;
    const std::uint32_t expected = static_cast<std::uint32_t>(expected64);
    EXPECT_NE(expected, expected64) << "calculate with 32bit overflow";
    EXPECT_EQ(score::cpp::hash_bytes_fnv1a<std::uint32_t>("\0", 1U), expected);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17948197
TEST(hash_test, null_input_returns_offset_basis_modulo_64bit)
{
    EXPECT_EQ(score::cpp::hash_bytes_fnv1a<std::uint64_t>(nullptr, 0U), 14695981039346656037UL);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17948197
TEST(hash_test, one_zero_input_returns_offset_basis_multiplied_by_prime_modulo_64bit)
{
    const std::uint64_t expected = 14695981039346656037UL * 1099511628211UL;
    EXPECT_EQ(score::cpp::hash_bytes_fnv1a<std::uint64_t>("\0", 1U), expected);
}

// http://www.isthe.com/chongo/tech/comp/fnv/index.html#zero-hash
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17948197
TEST(hash_test, test_known_32bit_zero_hashes)
{
    {
        const std::array<std::uint8_t, 4> input{0xCC, 0x24, 0x31, 0xC4};
        EXPECT_EQ(score::cpp::hash_bytes_fnv1a<std::uint32_t>(input.data(), input.size()), 0);
    }
    {
        const std::array<std::uint8_t, 4> input{0xE0, 0x4D, 0x9F, 0xCB};
        EXPECT_EQ(score::cpp::hash_bytes_fnv1a<std::uint32_t>(input.data(), input.size()), 0);
    }
}

// http://www.isthe.com/chongo/tech/comp/fnv/index.html#zero-hash
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17948197
TEST(hash_test, test_known_64bit_zero_hashes)
{
    const std::array<std::uint8_t, 8> input{0xD5, 0x6B, 0xB9, 0x53, 0x42, 0x87, 0x08, 0x36};
    EXPECT_EQ(score::cpp::hash_bytes_fnv1a<std::uint64_t>(input.data(), input.size()), 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17948197
TEST(hash_test, collisions_32bit)
{
    std::set<std::uint32_t> hashes;
    for (const auto& word : samples)
    {
        hashes.insert(score::cpp::hash_bytes_fnv1a<std::uint32_t>(word.data(), word.size()));
    }
    EXPECT_EQ(hashes.size(), samples.size());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17948197
TEST(hash_test, collisions_64bit)
{
    std::set<std::uint64_t> hashes;
    for (const auto& word : samples)
    {
        hashes.insert(score::cpp::hash_bytes_fnv1a<std::uint64_t>(word.data(), word.size()));
    }
    EXPECT_EQ(hashes.size(), samples.size());
}

} // namespace
