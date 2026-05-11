// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_BINARY_PREDICATE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_BINARY_PREDICATE_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/type_traits/is_predicate.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/remove_cv.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/remove_reference.hpp"

namespace arene {
namespace base {

/// @brief determines if a type satisfies the BinaryPredicate named requirement
/// @tparam F the possible binary predicate function object
/// @tparam T1 type of the first argument
/// @tparam T2 type of the second argument
///
/// BinaryPredicate is a set of requirements expected by some of the standard
/// library facilities from the user-provided function object types.
///
/// Given a BinaryPredicate @c pred and a pair of iterators @c iter1 and
/// @c iter2 or an iterator @c iter and a (possibly const) value @c value, the
/// type and value category of the expression <c> pred(*iter1, *iter2) </c> or,
/// respectively, <c> pred(*iter, value) </c>, must meet the BooleanTestable
/// requirements.
///
/// In addition, evaluation of that expression is not allowed to call non-const
/// member functions of the dereferenced iterators; syntactically, the predicate
/// must accept const object arguments, with the same behavior regardless of
/// whether its arguments are const or non-const.
///
/// A type satisfies BinaryPredicate if
/// * the type satisfies Predicate
/// * the type satisfies CopyConstructible
///
/// @{
template <class F, class T1, class T2 = T1>
extern constexpr bool is_binary_predicate_v =  //
    is_predicate_v<F, T1, T2> &&
    is_predicate_v<
        F,
        remove_cvref_t<T1> const&,
        remove_cvref_t<T2> const&> &&
    std::is_move_constructible<remove_cvref_t<F>>::value &&  //
    std::is_copy_constructible<remove_cvref_t<F>>::value;
/// @}

}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_BINARY_PREDICATE_HPP_
