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

#ifndef SCORE_SERIALIZATION_DETAILS_SERIALIZATION_VISITOR_H
#define SCORE_SERIALIZATION_DETAILS_SERIALIZATION_VISITOR_H

#include "score/serialization/meta_information.h"
#include "score/serialization/serialize_iterator.h"

#include "static_reflection_with_serialization/visitor/visit.h"

#include <score/assert.hpp>
#include <score/type_traits.hpp>

#include <array>
#include <cstddef>

namespace score
{
namespace serialization
{

namespace helper
{

/// @brief Small helper to deduce a template type within decltype function calls
template <typename T>
auto AsReference() -> T&;

}  // namespace helper

class CommonVisitorLogic
{
  public:
    CommonVisitorLogic() = default;
    ~CommonVisitorLogic() noexcept = default;
    CommonVisitorLogic(const CommonVisitorLogic&) = delete;
    CommonVisitorLogic& operator=(const CommonVisitorLogic&) = delete;
    CommonVisitorLogic(CommonVisitorLogic&&) noexcept = delete;
    CommonVisitorLogic& operator=(CommonVisitorLogic&&) noexcept = delete;

    void MarkAsFirstElementInStruct()
    {
        meta_.is_first_element_in_struct_ = true;
        meta_.is_last_element_in_struct_ = false;
    }

    void MarkAsElementInStruct()
    {
        meta_.is_first_element_in_struct_ = false;
        meta_.is_last_element_in_struct_ = false;
    }

    void MarkAsLastElementInStruct()
    {
        meta_.is_first_element_in_struct_ = false;
        meta_.is_last_element_in_struct_ = true;
    }

    const MetaInformation& GetMetaInformation() const
    {
        return meta_;
    }

    void SetFieldIndex(std::size_t field_index)
    {
        meta_.field_index_ = field_index;
    }

    /// @brief Maximum supported struct/array nesting depth for the field path below.
    static constexpr std::size_t kMaxNestingDepth{16U};

    /// @brief Records that traversal descended into the field at the given index of the current struct. Together with
    /// PopField this maintains the full index path from the root to the element currently visited, letting a format
    /// resolve per-level configuration for arbitrarily nested types. is_last records whether this field is the last one
    /// of its struct, so a format can tell whether anything follows the element across all nesting levels.
    void PushField(const std::size_t field_index, const bool is_last)
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(field_depth_ < kMaxNestingDepth);
        field_path_[field_depth_] = field_index;
        last_path_[field_depth_] = is_last;
        ++field_depth_;
    }

    void PopField()
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(field_depth_ > 0U);
        --field_depth_;
    }

    const std::size_t* FieldPathData() const
    {
        return field_path_.data();
    }

    const bool* LastPathData() const
    {
        return last_path_.data();
    }

    std::size_t FieldDepth() const
    {
        return field_depth_;
    }

  private:
    MetaInformation meta_{};
    std::array<std::size_t, kMaxNestingDepth> field_path_{};
    std::array<bool, kMaxNestingDepth> last_path_{};
    std::size_t field_depth_{0U};
};

/// @brief Provides context for serialization but is not user
/// facing.
template <typename SequenceContainer, typename SerializationStrategy>
class SerializationVisitor : public CommonVisitorLogic
{
  public:
    explicit SerializationVisitor(SequenceContainer& data, const SerializationStrategy& strategy)
        : CommonVisitorLogic{}, strategy_{strategy}, data_{data}, iterator_{SerializeInto(data_)}
    {
    }

    template <typename T>
    auto Serialize(const T& value)
        -> std::enable_if_t<std::is_void<decltype(std::declval<SerializationStrategy>().Serialize(
                                helper::AsReference<SerializationVisitor<SequenceContainer, SerializationStrategy>>(),
                                std::declval<SerializeIterator<SequenceContainer>>(),
                                std::declval<T>()))>::value,
                            void>
    {
        strategy_.Serialize(*this, iterator_, value);
    }

    Result<std::size_t> GetResult() const
    {
        return iterator_.GetResult();
    }

    /// @brief enables to visit the next type for serialization (needed for complex types)
    template <typename T>
    void Next(T&& value)
    {
        common::visitor::visit(*this, std::forward<T>(value));
    }

  private:
    const SerializationStrategy& strategy_{};
    SequenceContainer& data_;
    SerializeIterator<SequenceContainer> iterator_;
};

/// @brief Provides context for deserialization but is not user
/// facing.
template <typename SequenceContainer, typename SerializationStrategy>
class DeSerializationVisitor : public CommonVisitorLogic
{
  public:
    explicit DeSerializationVisitor(const SequenceContainer& data, const SerializationStrategy& strategy)
        : CommonVisitorLogic{},
          strategy_{strategy},
          data_{data},
          iterator_{DeSerializeIterator<SequenceContainer>{data_}},
          result_{}
    {
    }

    template <typename T>
    auto DeSerialize(T& value) -> std::enable_if_t<
        std::is_same<decltype(std::declval<SerializationStrategy>().DeSerialize(
                         helper::AsReference<DeSerializationVisitor<SequenceContainer, SerializationStrategy>>(),
                         helper::AsReference<DeSerializeIterator<SequenceContainer>>(),
                         helper::AsReference<T>())),
                     Result<void>>::value,
        void>
    {
        const auto result =
            strategy_.template DeSerialize<DeSerializationVisitor, SequenceContainer>(*this, iterator_, value);
        if (not result.has_value())
        {
            result_ = result;
        }
    }

    Result<std::size_t> GetResult() const
    {
        if (not result_.has_value())
        {
            return Unexpected{result_.error()};
        }

        return iterator_.GetResult();
    }

    [[nodiscard]] bool HasError() const noexcept
    {
        // The iterator latches errors (e.g. too few bytes left) that are not mirrored in result_.
        return (not result_.has_value()) || iterator_.HasError();
    }

    [[nodiscard]] bool IsSerializedDataRemaining() const
    {
        const auto end = iterator_.End();
        return iterator_ != end;
    }

    /// @brief enables to visit next type for deserialization
    /// @details The underlying visitor API returns void, so traversal cannot be stopped from within.
    /// We therefore skip remaining fields once an error has been latched to avoid deserializing into a
    /// partially-valid object. The result object is only valid when GetResult() succeeds.
    template <typename T>
    void Next(T&& value)
    {
        if (not result_.has_value())
        {
            return;
        }
        common::visitor::visit(*this, std::forward<T>(value));
    }

  private:
    const SerializationStrategy& strategy_;
    const SequenceContainer& data_;
    DeSerializeIterator<SequenceContainer> iterator_;
    Result<void> result_;
};

}  // namespace serialization
}  // namespace score

#endif  // SCORE_SERIALIZATION_DETAILS_SERIALIZATION_VISITOR_H
