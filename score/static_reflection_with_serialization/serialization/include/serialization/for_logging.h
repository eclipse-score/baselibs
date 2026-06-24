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
#ifndef SCORE_COMMON_SERIALIZATION_INCLUDE_SERIALIZATION_FOR_LOGGING_H
#define SCORE_COMMON_SERIALIZATION_INCLUDE_SERIALIZATION_FOR_LOGGING_H

#include "visit_serialize.h"
#include "visit_size.h"
#include <score/span.hpp>

namespace score
{
namespace common
{
namespace visitor
{

using Byte = char;

struct log_alloc_t
{
    using offset_t = uint32_t;
    using subsize_t = uint16_t;
};

constexpr log_alloc_t::offset_t get_offset_t_max()
{
    return std::numeric_limits<log_alloc_t::offset_t>::max();
}

class logging_serializer
{
  public:
    using offset_t = typename log_alloc_t::offset_t;
    using impl = serializer_t<log_alloc_t>;
    using impl_size = serialized_size_t<log_alloc_t>;
    template <typename T>
    static offset_t serialize(const T& t, std::uint8_t* const data, const size_t size)
    {
        if (get_offset_t_max() <= size)
        {
            return 0UL;
        }
        return impl::serialize(t, data, static_cast<offset_t>(size));
    }
    template <typename T>
    static offset_t serialize(const T& t, Byte* const data, const size_t size)
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT(size <= std::numeric_limits<offset_t>::max());
        // The previous assertion prevent the TRUE case of the below condition.
        if (get_offset_t_max() <= static_cast<offset_t>(size))  // LCOV_EXCL_BR_LINE
        {
            return static_cast<offset_t>(0);  // LCOV_EXCL_LINE
        }
        // coverity[autosar_cpp14_a5_2_4_violation]
        return impl::serialize(t, reinterpret_cast<std::uint8_t* const>(data), static_cast<offset_t>(size));
    }
    template <typename T>
    static deserialization_result_t deserialize(const std::uint8_t* const data, const size_t size, T& t)
    {
        if (get_offset_t_max() <= size)
        {
            return deserialization_result_t{
                true /* out of bounds */, false /* invalid format */, false /* zero offset */};
        }
        return impl::deserialize(data, static_cast<offset_t>(size), t);
    }
    template <typename T>
    static deserialization_result_t deserialize(const Byte* const data, const size_t size, T& t)
    {
        // Both True and False cases are already tested as shown in the coverage report screenshot there
        // broken_link_j/Ticket-175753?focusedId=17749331&page=com.atlassian.jira.plugin
        //     .system.issuetabpanels%3Acomment-tabpanel#comment-17749331
        // But the LCOV complains that the False case didn't covered. So, we are going to exclude the branch
        // from the coverage report.
        if (get_offset_t_max() <= size)  // LCOV_EXCL_BR_LINE
        {
            return deserialization_result_t{
                true /* out of bounds */, false /* invalid format */, false /* zero offset */};
        }
        // coverity[autosar_cpp14_a5_2_4_violation]
        return impl::deserialize(reinterpret_cast<const std::uint8_t* const>(data), static_cast<offset_t>(size), t);
    }
    template <typename T>
    static offset_t serialize_size(const T& t)
    {
        return impl_size::serialized_size<offset_t>(t);
    }
};

template <typename T>
using logging_serialized_descriptor = serialized_descriptor_t<log_alloc_t, T>;

// The type description format for logger_type_string() and logger_type_info()
// (1-byte aligned):
//   uint32_t name_size;
//   char fully_qualified_type_name[name_size];
//   [optional, TBD] char payload_format_description[];

template <typename T>
inline std::string logger_memcpy(const T& t)
{
    // coverity[autosar_cpp14_a5_2_4_violation]
    return std::string(reinterpret_cast<const Byte*>(&t), sizeof(T));
}

inline std::string logger_pack_string(const std::string& str)
{
    // Takes size of name as uint32_t; that's why the function casts the argument to uint32_t,
    // and in the copy function inside the Typeinfo class,
    // it copies the name starting after the uint32_t to copy the name of Type.
    // The format is "name_size(uint32_t)name", so the function only limits the name size.
    // coverity[autosar_cpp14_a4_7_1_violation]
    return logger_memcpy(static_cast<uint32_t>(str.size())) + str;
}

template <typename T>
inline std::string logger_type_string()
{
    using visitable = ::score::common::visitor::struct_visitable<T>;
    return logger_pack_string(visitable::name());
}

template <typename T>
inline auto logger_type_info()
{
    class Typeinfo
    {
      public:
        explicit Typeinfo() : payload_(struct_visitable<T>::template name<std::pair<const Byte*, const Byte*>>()) {}
        std::size_t size() const
        {
            const auto strsize = static_cast<std::size_t>(std::distance(payload_.first, payload_.second));
            return sizeof(uint32_t) + strsize;
        }
        void copy(Byte* const data, const std::size_t size) const
        {
            if (size >= sizeof(uint32_t))
            {
                const auto strsize = static_cast<std::size_t>(std::distance(payload_.first, payload_.second));
                if (size >= sizeof(uint32_t) + strsize)
                {
                    SCORE_LANGUAGE_FUTURECPP_ASSERT(strsize <= std::numeric_limits<uint32_t>::max());
                    auto intsize = static_cast<uint32_t>(strsize);
                    // needed to copy data to the target location
                    std::ignore = memcpy(data, &intsize, sizeof(uint32_t));
                    // needed to copy data to the target location and arithmetic used to get offset from this location
                    // tolerated per design
                    score::cpp::span<Byte> dataSpan{static_cast<Byte*>(data),
                                             static_cast<typename score::cpp::span<Byte*>::size_type>(size)};
                    std::ignore = memcpy(dataSpan.subspan(sizeof(uint32_t)).data(), payload_.first, strsize);
                }
                else
                {
                    // used to set 0 to std type
                    std::ignore = memset(data, 0, sizeof(uint32_t));
                }
            }
        }

      private:
        const std::pair<const Byte*, const Byte*> payload_;
    };

    return Typeinfo();
}

}  // namespace visitor
}  // namespace common
}  // namespace score

#endif  // SCORE_COMMON_SERIALIZATION_INCLUDE_SERIALIZATION_FOR_LOGGING_H
