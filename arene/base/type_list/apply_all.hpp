// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_APPLY_ALL_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_APPLY_ALL_HPP_

// IWYU pragma: private, include "arene/base/type_list.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

namespace arene {
namespace base {

namespace type_lists {

/// @cond INTERNAL

namespace apply_all_detail {

/// @brief Evaluate a template meta-function, passing the elements of
///        the type-list L0 as the arguments.
/// @tparam L0 An instantiation of a type-list that holds the types
/// @pre @c L0 is a type-list
template <class L0, template <class...> class Apply>
struct apply_all_impl;

/// @brief Evaluate a template meta-function, passing the elements of
///        the type-list as the arguments.
/// @tparam TypeList0 A type list template
/// @tparam In The types in the type list
/// @tparam Apply The metafunction to apply to each element
template <template <class...> class TypeList0, class... In, template <class...> class Apply>
struct apply_all_impl<TypeList0<In...>, Apply> {
  /// @brief @p type is a type that is the result of invoking
  /// @p Apply<> once with all of the elements of the type list
  using type = Apply<In...>;
};

}  // namespace apply_all_detail

/// @endcond

/// @brief Evaluate a template meta-function, passing the elements of
///        the type-list L0 as the arguments.
/// @tparam L0 An instantiation of a type-list that holds the types
/// @pre @c L0 is a type-list
template <class L0, template <class...> class Func>
using apply_all = arene::base::type_lists::apply_all_detail::apply_all_impl<L0, Func>;

/// @brief Evaluate a template meta-function, passing the elements of
///        the type-list L0 as the arguments.
/// @tparam L0 An instantiation of a type-list that holds the types
/// @pre @c L0 is a type-list
template <class L0, template <class...> class Func>
using apply_all_t = typename apply_all<L0, Func>::type;

}  // namespace type_lists
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_APPLY_ALL_HPP_
