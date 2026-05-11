// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_APPLY_EACH_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_APPLY_EACH_HPP_

// IWYU pragma: private, include "arene/base/type_list.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

namespace arene {
namespace base {
namespace type_lists {

/// @brief Apply the given template individually to every element of the given type list
/// @tparam TypeList The type list to whose elements @c ToApply should be applied
/// @tparam ToApply The template to apply to each element of @c TypeList
template <class TypeList, template <class...> class ToApply>
struct apply_each;

/// @brief Apply the given template individually to every element of the given type list
/// @tparam TypeList The type list to whose elements @c ToApply should be applied
/// @tparam Types The individual types contained by this specialization of @c TypeList
/// @tparam ToApply The template to apply to each type in @c Types
template <template <class...> class TypeList, typename... Types, template <class...> class ToApply>
struct apply_each<TypeList<Types...>, ToApply> {
  /// @brief A new @c TypeList specialization whose parameters are @c ToApply<Types...> instead of @c Types...
  using type = TypeList<ToApply<Types>...>;
};

/// @brief Apply the given template individually to every element of the given type list
/// @tparam TypeList The type list to whose elements @c ToApply should be applied
/// @tparam ToApply The template to apply to each element of @c TypeList
template <class TypeList, template <class...> class ToApply>
using apply_each_t = typename apply_each<TypeList, ToApply>::type;

}  // namespace type_lists
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_LIST_APPLY_EACH_HPP_
