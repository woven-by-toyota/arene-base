// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_AT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_AT_HPP_

// IWYU pragma: private, include "arene/base/type_list.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/tuple_element.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_traits/type_identity.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace type_lists {

/// @cond INTERNAL

namespace at_detail {

/// @brief Local alias for type_list.
/// @tparam Tn The elements of the list
template <class... Tn>
using tl = arene::base::type_list<Tn...>;

/// @brief An implementation class that derives from all the specified base classes.
/// @tparam Bases The list of base classes to derive from; must not contain
/// duplicates
template <typename... Bases>
class idx : public Bases... {};

/// @brief A tag type specifying the index of an element and its type
/// @tparam Index The index of this element in the type list
/// @tparam T The type of this element
template <std::size_t Index, typename T>
class idx_el {};

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
/// @brief Unimplemented function, the return type of which is the type of the
/// specified element in a type list
/// @tparam Index The index of the element to get the type of
/// @tparam T The type of that element
/// @return An instance of @c T
template <std::size_t Index, typename T>
inline auto get_element_type(idx_el<Index, T> const&) -> arene::base::type_identity<T>;
// parasoft-end-suppress CERT_C-EXP37-a-3
// parasoft-end-suppress AUTOSAR-M3_3_2-a

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
/// @brief Unimplemented function, the return type of which is a class that derives
/// from an instance of @c idx_el for each element in the
/// type list.
/// @tparam Indices The indices of the elements
/// @tparam Types The types of the elements
/// @return An instance of @c idx
template <std::size_t... Indices, typename... Types>
inline auto make_idx(std::index_sequence<Indices...>, tl<Types...>) -> idx<idx_el<Indices, Types>...> const&;
// parasoft-end-suppress CERT_C-EXP37-a-3
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief Get the n-th element from a type list
///
/// @tparam Index the index of the element to get
/// @tparam TL the type list for which to get the element
/// @pre @c TL is an instantiation of a type-list
/// @pre <c>Index < TL::Size()</c>
template <std::size_t Index, typename L0>
class at_impl;

/// @brief Get the n-th element from a type list.
///
/// @tparam Index the index of the element to get
/// @tparam Types the elements in the type list
template <std::size_t Index, template <typename...> class TypeList0, typename... Tn0>
class at_impl<Index, TypeList0<Tn0...>> {
 public:
  static_assert(Index < sizeof...(Tn0), "Index must be less than the number of types in the list");
  /// @brief The element type for Index
  using type = typename decltype(::arene::base::type_lists::at_detail::get_element_type<Index>(
      ::arene::base::type_lists::at_detail::make_idx(std::make_index_sequence<sizeof...(Tn0)>(), tl<Tn0...>{})
  ))::type;
};

}  // namespace at_detail

/// @endcond

/// @brief Get the n-th element from a type list
///
/// @tparam L0 An instantiation of a type-list that holds the types to index
/// @tparam Index the index of the element to get
/// @pre @c L0 is a type-list
/// @pre <c>Index < size<L0></c>
template <class L0, std::size_t Index>
using at = arene::base::type_lists::at_detail::at_impl<Index, L0>;

/// @brief Get the n-th element from a type list
///
/// @tparam L0 An instantiation of a type-list that holds the types to index
/// @tparam Index the index of the element to get
/// @pre @c L0 is a type-list
/// @pre <c>Index < size<L0></c>
template <class L0, std::size_t Index>
using at_t = typename at<L0, Index>::type;

}  // namespace type_lists
}  // namespace base
}  // namespace arene

// parasoft-begin-suppress AUTOSAR-A14_7_2-a-2 "False positive: 'type_list' is defined in type_list.hpp included above"
/// @brief A meta function to retrieve the specified element from a type list.
///
/// The @c type member is an alias of the resultant type
/// @tparam I The index of the element to retrieve
/// @tparam Tn The elements of the type list
template <std::size_t I, class... Tn>
class std::tuple_element<I, arene::base::type_list<Tn...>>
    : public arene::base::type_lists::at_detail::at_impl<I, arene::base::type_list<Tn...>> {};
// parasoft-end-suppress AUTOSAR-A14_7_2-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_AT_HPP_
