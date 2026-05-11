// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::remove_nth"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_REMOVE_NTH_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_REMOVE_NTH_HPP_

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/type_list/at.hpp"
#include "arene/base/type_traits/type_identity.hpp"

namespace arene {
namespace base {
namespace type_lists {

/// @cond INTERNAL

namespace remove_nth_detail {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
/// @brief implementation function for selecting the specified elements from a type list
/// @tparam List Type list template
/// @tparam Ts The types in the list
/// @tparam Js The indices of the elements to return
/// @return type list with the selected elements
template <template <class...> class List, class... Ts, std::size_t... Js>
auto select_elements_from_impl(type_identity<List<Ts...>>, std::index_sequence<Js...>)
    -> List<type_lists::at_t<List<Ts...>, Js>...>;
// parasoft-end-suppress CERT_C-EXP37-a-3
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief A type alias for a type list with the selected elements from the source list
/// @tparam List The source type list
/// @tparam Indices An instance of @c std:index_sequence with the indices of the selected elements
template <typename List, typename Indices>
using select_elements_from_t = decltype(select_elements_from_impl(type_identity<List>{}, Indices{}));

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
/// @brief implementation function for getting an index sequence with the nth element removed
/// @tparam IndexToRemove The index to remove
/// @tparam Js The indices of the elements
/// @return index sequence with the specified index removed
template <std::size_t IndexToRemove, std::size_t... Js>
auto remove_nth_helper(std::index_sequence<Js...>) -> std::index_sequence<((Js < IndexToRemove) ? Js : (Js + 1))...>;
// parasoft-end-suppress CERT_C-EXP37-a-3

/// @brief implementation type for @c remove_nth
/// @tparam List type list
/// @tparam Index the index of the entry to remove
template <typename List, std::size_t Index>
class remove_nth_impl;

/// @brief implementation type for @c remove_nth
/// @tparam List type list
/// @tparam Ts types
/// @tparam Index the index of the entry to remove
template <template <class...> class List, class... Ts, std::size_t Index>
class remove_nth_impl<List<Ts...>, Index> {
 public:
  static_assert(Index < sizeof...(Ts), "The index must be less than the size of the list");

  /// @brief type list with nth removed
  using type = select_elements_from_t<
      List<Ts...>,
      decltype(remove_nth_helper<Index>(std::make_index_sequence<sizeof...(Ts) - 1>{}))>;
};

}  // namespace remove_nth_detail

/// @endcond

/// @brief Filter a type-list to remove the element at the specified index
///
/// @tparam List An instantiation of a type-list that holds the types to filter
/// @tparam Index the index of the element to remove
/// @pre @c List is a type-list
template <class List, std::size_t Index>
using remove_nth = arene::base::type_lists::remove_nth_detail::remove_nth_impl<List, Index>;

/// @brief Filter a type-list to remove the element at the specified index
///
/// @tparam List An instantiation of a type-list that holds the types to filter
/// @tparam Index the index of the element to remove
/// @pre @c List is a type-list
/// @pre <c>Index < size<List></c>
template <class List, std::size_t Index>
using remove_nth_t = typename remove_nth<List, Index>::type;

}  // namespace type_lists
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_REMOVE_NTH_HPP_
