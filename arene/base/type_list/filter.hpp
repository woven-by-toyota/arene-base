// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_FILTER_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_FILTER_HPP_

// IWYU pragma: private, include "arene/base/type_list.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/type_list/flat_map.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_traits/conditional.hpp"

namespace arene {
namespace base {

namespace type_lists {

/// @cond INTERNAL

namespace filter_detail {

/// @brief Local alias for type_list.
/// @tparam Tn The elements of the list
template <class... Tn>
using tl = arene::base::type_list<Tn...>;

/// @brief A class template meta-function that returns a type list that contains
/// only the elements in the type list for which Filter<>::value was true
///
/// @tparam TypeList The type list use to produce the result.
/// @tparam Filter The unary predicate to invoke.
/// @return TypeList<..>
template <template <class...> class Filter>
struct apply_filter {
  /// @brief @p filter is a type list that is empty when the result of invoking
  /// @p Filter<> with @p T is false and otherwise has a single element @p T
  template <class T>
  using filter = conditional_t<
      Filter<T>::value,
      // keep
      tl<T>,
      // discard
      tl<>>;
};

}  // namespace filter_detail

/// @endcond

/// @brief Filter a type-list to remove elements that don't match the supplied
/// @c Filter.
///
/// @tparam L0 An instantiation of type-list that holds the types to filter.
/// @tparam Filter A constexpr bool meta-function that accepts a single type
/// parameter and provides a boolean that is @c true if the type should be
/// included in the list, @c false otherwise.
/// @return TypeList<T...>
template <class L0, template <class> class Filter>
using filter = arene::base::type_lists::
    flat_map<L0, arene::base::type_lists::filter_detail::apply_filter<Filter>::template filter>;

/// @brief Filter a type-list to remove elements that don't match the supplied
/// @c Filter.
///
/// @tparam L0 An instantiation of type-list that holds the types to filter.
/// @tparam Filter A constexpr bool meta-function that accepts a single type
/// parameter and provides a boolean that is @c true if the type should be
/// included in the list, @c false otherwise.
/// @return TypeList<T...>
template <class L0, template <class> class Filter>
using filter_t = typename filter<L0, Filter>::type;

}  // namespace type_lists
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_FILTER_HPP_
