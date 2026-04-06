// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_FLAT_MAP_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_FLAT_MAP_HPP_

// IWYU pragma: private, include "arene/base/type_list.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/type_list/apply_all.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_traits/type_identity.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace type_lists {

/// @cond INTERNAL

namespace flat_map_detail {

/// @brief Local alias for type_list.
/// @tparam Tn The elements of the list
template <class... Tn>
using tl = arene::base::type_list<Tn...>;

/// @brief applies Apply<T> to each T in L0 (aka TypeList0<T...>) to produce
/// TypeList<U...> by concatenating all the Apply<T> results
template <class L0, template <class> class Apply>
struct flat_map_impl;

/// @brief The mapping of an empty list is an empty list
/// @tparam TypeList0 The type list template
/// @tparam Apply The metafunction to apply
template <template <class...> class TypeList0, template <class> class Apply>
struct flat_map_impl<TypeList0<>, Apply> {
  /// @brief @p type is an empty list when the type list has no elements
  using type = TypeList0<>;
};

/// @brief applies @c Apply<T> to each @c T in @c L0 (aka @c TypeList0<T...>) to produce
/// @c TypeList<U...> by concatenating all the @c Apply<T> results
/// @tparam TypeList0 The type list template
/// @tparam T00 The first type in the type list
/// @tparam T0n The other types in the type list.
/// @tparam Apply The metafunction to apply
template <template <class...> class TypeList0, class T00, class... T0n, template <class> class Apply>
struct flat_map_impl<TypeList0<T00, T0n...>, Apply> {
 private:
  /// @brief invokes @c Apply with each element (start with tl<> so that the applied type-list is tl<..>)
  using applied = arene::base::type_lists::concat_t<tl<>, Apply<T00>, Apply<T0n>...>;
  /// @brief the result must use the @c TypeList0 template
  using final = arene::base::type_lists::apply_all_t<applied, TypeList0>;

 public:
  /// @brief @p type is a type list that contains the result of invoking
  /// @p Apply<> with each of the elements of the type list
  using type = final;
};

}  // namespace flat_map_detail

/// @endcond

/// @brief Produce TypeList<T...> by concatenating the type-list provided by
/// the supplied class template meta-function @c Apply when it is invoked
/// for each element in L0.
///
/// @tparam L0 An instantiation of a type-list that holds the types
/// @tparam Apply A class template meta-function that accepts a single type
/// parameter and produces a new type-list
/// @return TypeList<T...>
template <class L0, template <class> class Apply>
using flat_map = arene::base::type_lists::flat_map_detail::flat_map_impl<L0, Apply>;

/// @brief Produce TypeList<T...> by concatenating the type-list provided by
/// the supplied class template meta-function @c Apply when it is invoked
/// for each element in L0.
///
/// @tparam L0 An instantiation of a type-list that holds the types
/// @tparam Apply A class template meta-function that accepts a single type
/// parameter and produces a new type-list
/// @return TypeList<T...>
template <class L0, template <class> class Apply>
using flat_map_t = typename flat_map<L0, Apply>::type;

/// @brief collapse L0 aka TypeList0<TypeList1<T...>, .., TypeListN<U...>>
/// into TypeList<T..., .., U...>
///
/// @tparam L0 A type-list that holds the type-lists to flatten.
/// @return TypeList<T...>
template <class L0>
using flatten = flat_map<L0, arene::base::type_identity_t>;

/// @brief collapse L0 aka TypeList0<TypeList1<T...>, .., TypeListN<U...>>
/// into TypeList<T..., .., U...>
///
/// @tparam L0 A type-list that holds the type-lists to flatten.
/// @return TypeList<T...>
template <class L0>
using flatten_t = typename flatten<L0>::type;

}  // namespace type_lists
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_FLAT_MAP_HPP_
