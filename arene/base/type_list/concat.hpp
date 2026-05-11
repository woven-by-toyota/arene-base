// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_CONCAT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_CONCAT_HPP_

// IWYU pragma: private, include "arene/base/type_list.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/type_list/type_list.hpp"

namespace arene {
namespace base {

namespace type_lists {

/// @cond INTERNAL

namespace concat_detail {

/// @brief Local alias for type_list.
/// @tparam Tn The elements of the list
template <class... Tn>
using tl = arene::base::type_list<Tn...>;

/// @brief collapse L0<T...>, .. Ln<U...> into TypeList<T...,
/// .., U...>
template <class... Ln>
struct concat_impl;

/// @brief Concatenating no lists yields an empty list
template <>
struct concat_impl<> {
  /// @brief @p type is an empty list when there are no type-lists to concat
  using type = tl<>;
};

/// @brief Concatenating a single list yields that list
/// @tparam TypeList0 The list template
/// @tparam T0n The list elements
template <template <class...> class TypeList0, class... T0n>
struct concat_impl<TypeList0<T0n...>> {
  /// @brief @p type is a list of the original elements when there is only
  /// one type-list and thus nothing to concat
  using type = TypeList0<T0n...>;
};

/// @brief Concatenating two lists yields a single list instantiated from the first list template, with the elements
/// from both of the lists
/// @tparam TypeList0 The first list template
/// @tparam T0n The first list elements
/// @tparam TypeList1 The second list template
/// @tparam T1n The second list elements
template <template <class...> class TypeList0, class... T0n, template <class...> class TypeList1, class... T1n>
struct concat_impl<TypeList0<T0n...>, TypeList1<T1n...>> {
  /// @brief @p type combines the two type lists into one type list
  using type = TypeList0<T0n..., T1n...>;
};

/// @brief Concatenating three or more lists yields a single list instantiated from the first list template, with the
/// elements from all of the lists
/// @tparam TypeList0 The first list template
/// @tparam T0n The first list elements
/// @tparam TypeList1 The second list template
/// @tparam T1n The second list elements
/// @tparam TypeList2 The third list template
/// @tparam T2n The third list elements
/// @tparam Ln Any subsequent lists
template <
    template <class...>
    class TypeList0,
    class... T0n,
    template <class...>
    class TypeList1,
    class... T1n,
    template <class...>
    class TypeList2,
    class... T2n,
    class... Ln>
struct concat_impl<TypeList0<T0n...>, TypeList1<T1n...>, TypeList2<T2n...>, Ln...> {
  /// @brief @p type combines the first two type lists into one type list and
  /// then recurses into concat when there are at least two type-lists to concat
  /// @details Keeps @c TypeList0 unchanged so that it is the template that is used for the final result.
  /// Uses @c tl for all the intermediate lists.
  using type = typename concat_impl<TypeList0<T0n...>, tl<T1n..., T2n...>, Ln...>::type;
};

}  // namespace concat_detail

/// @endcond

/// @brief collapse L0<T...>, .. Ln<U...> into TypeList<T...,
/// .., U...>
///
/// @tparam Ln A pack of type-lists that holds the types to concatenate.
/// @return TypeList<T...>
template <class... Ln>
using concat = arene::base::type_lists::concat_detail::concat_impl<Ln...>;

/// @brief collapse L0<T...>, .. Ln<U...> into TypeList<T...,
/// .., U...>
///
/// @tparam Ln A pack of type-lists that holds the types to concatenate.
/// @return TypeList<T...>
template <class... Ln>
using concat_t = typename concat<Ln...>::type;

}  // namespace type_lists
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_CONCAT_HPP_
