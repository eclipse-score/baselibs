/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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

#ifndef SCORE_SERIALIZATION_TEST_DOUBLES_FAKE_SERIALIZATION_STRATEGY_H
#define SCORE_SERIALIZATION_TEST_DOUBLES_FAKE_SERIALIZATION_STRATEGY_H

#include "score/serialization/meta_information.h"
#include "score/serialization/serialize_iterator.h"

#include <array>
#include <cstring>
#include <type_traits>

namespace score
{
namespace serialization
{
namespace test
{

class FakeSerializationStrategy
{
  public:
    FakeSerializationStrategy(bool swap_bytes) : swap_bytes_{swap_bytes} {}

    template <typename Visitor, typename Container, typename T, std::enable_if_t<std::is_scalar<T>::value, bool> = true>
    void Serialize(Visitor&, SerializeIterator<Container> iterator, const T& value) const
    {
        constexpr auto variable_size = sizeof(T);
        std::array<std::uint8_t, variable_size> bytes{};
        std::memcpy(&bytes, &value, variable_size);
        if (swap_bytes_)
        {
            std::copy(bytes.crbegin(), bytes.crend(), iterator);
        }
        else
        {
            std::copy(bytes.cbegin(), bytes.cend(), iterator);
        }
    }

    template <typename Visitor, typename Container>
    void Serialize(Visitor& visitor,
                   SerializeIterator<Container> iterator,
                   const std::vector<std::uint8_t>& value) const
    {
        Serialize(visitor, iterator, value.size());
        for (const auto& single : value)
        {
            iterator = single;
        }
    }

    template <typename Visitor, typename Container, typename T, std::enable_if_t<std::is_scalar<T>::value, bool> = true>
    Result<void> DeSerialize(Visitor&, DeSerializeIterator<Container>& iterator, T& value) const
    {
        constexpr auto size = sizeof(T);
        auto begin = iterator;
        std::advance(iterator, size);

        std::array<std::uint8_t, size> raw_bytes{};
        if (swap_bytes_)
        {
            std::copy(begin, iterator, raw_bytes.rbegin());
        }
        else
        {
            std::copy(begin, iterator, raw_bytes.begin());
        }

        std::memcpy(&value, raw_bytes.data(), size);

        return {};
    }

  private:
    const bool swap_bytes_;
};

class OtherFakeSerializationStrategy
{
  public:
    static constexpr SerializationError expected_error = SerializationError::kOutOfBoundsWrite;

    template <typename Visitor, typename Container>
    void Serialize(Visitor& visitor, SerializeIterator<Container> iterator, const std::uint8_t&) const
    {
        const auto meta = visitor.GetMetaInformation();
        if (meta.is_first_element_in_struct_)
        {
            iterator = 0x01;
        }
        else if (meta.is_last_element_in_struct_)
        {
            iterator = 0x03;
        }
        else
        {
            iterator = 0x02;
        }
    }

    template <typename Visitor, typename Container>
    Result<void> DeSerialize(Visitor& visitor, DeSerializeIterator<Container>&, std::uint8_t& value) const
    {
        const auto meta = visitor.GetMetaInformation();
        if (meta.is_first_element_in_struct_)
        {
            value = 0x01;
        }
        else if (meta.is_last_element_in_struct_)
        {
            value = 0x03;
        }
        else
        {
            value = 0x02;
        }

        return {};
    }

    template <typename Visitor, typename Container>
    Result<void> DeSerialize(Visitor&, DeSerializeIterator<Container>&, std::vector<std::uint8_t>&) const
    {
        return MakeUnexpected(expected_error);
    }
};

}  // namespace test
}  // namespace serialization
}  // namespace score

#endif  // SCORE_SERIALIZATION_TEST_DOUBLES_FAKE_SERIALIZATION_STRATEGY_H
