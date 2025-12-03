///
/// \file
/// \copyright Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// Score.Futurecpp.ForEachTupleElement component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_FOR_EACH_TUPLE_ELEMENT_HPP
#define SCORE_LANGUAGE_FUTURECPP_FOR_EACH_TUPLE_ELEMENT_HPP

#include <tuple>
#include <type_traits>
#include <utility>

namespace score::cpp
{
namespace detail
{

/// \brief Implements the variadic function call expansion for apply by expanding a index sequence and calling the
/// functor on each tuple element.
template <typename Function, typename Tuple, std::size_t... Is>
void for_each_tuple_element_impl(Function&& f, Tuple&& tuple, std::index_sequence<Is...>)
{
    (..., f(std::get<Is>(std::forward<Tuple>(tuple))));
}

} // namespace detail

/// \brief Maps a functor to tuple elements and executes the call operator on each element, disregarding the return
/// value.
///
/// \tparam Function The type of the functor that should be applied to the elements of the tuple.
/// \tparam Tuple Types of the arguments. The tuple need not be std::tuple, and instead may be anything that supports
/// std::get and std::tuple_size; in particular, std::array and std::pair may be used.
template <typename Function, typename Tuple>
void for_each_tuple_element(Tuple&& tuple, Function&& f)
{
    score::cpp::detail::for_each_tuple_element_impl(
        std::forward<Function>(f),
        std::forward<Tuple>(tuple),
        std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{});
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_FOR_EACH_TUPLE_ELEMENT_HPP
