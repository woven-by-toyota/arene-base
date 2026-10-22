// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::iter_const_reference_t"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_ITER_CONST_REFERENCE_T_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_ITER_CONST_REFERENCE_T_HPP_

// IWYU pragma: private, include "arene/base/iterator.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/iterator/indirectly_readable_traits.hpp"
#include "arene/base/iterator/is_indirectly_readable.hpp"
#include "arene/base/iterator/iter_reference_t.hpp"
#include "arene/base/iterator/iter_value_t.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_traits/common_reference.hpp"

namespace arene {
namespace base {

namespace iter_const_reference_detail {

/// @brief Helper class for computing the constant reference type of an indirectly readable type.
/// @tparam Type The indirectly readable type for which to compute the constant reference type.
/// @see iter_const_reference_t
/// The primary template is malformed.
template <typename Type, typename = constraints<>>
class iter_const_reference_t;

/// @brief Specialization of iter_const_reference_t for indirectly readable types.
/// @tparam Type The indirectly readable type for which to compute the constant reference type.
/// @see iter_const_reference_t
template <typename Type>
class iter_const_reference_t<Type, constraints<std::enable_if_t<is_indirectly_readable_v<Type>>>> {
 public:
  /// @brief The constant reference type for the indirectly readable type @c Type.
  using type = common_reference_t<iter_value_t<Type> const&&, iter_reference_t<Type>>;
};

}  // namespace iter_const_reference_detail

/// @brief A backport of @c std::iter_const_reference_t. Computes the
/// constant reference type for an indirectly readable type.
/// @tparam Type The indirectly readable type for which to compute the constant reference type.
///
/// If @c Type satisfies @c is_indirectly_readable , then this will be <c>common_reference_t<iter_value_t<Type> const&&,
/// iter_reference_t<Type>></c>. Otherwise, it will be ill-formed such that it would result in a specialization failure
/// during template instantiation.
template <typename Type>
using iter_const_reference_t = typename iter_const_reference_detail::iter_const_reference_t<Type>::type;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_ITER_CONST_REFERENCE_T_HPP_
