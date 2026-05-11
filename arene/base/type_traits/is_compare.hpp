// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_COMPARE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_COMPARE_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/type_traits/is_binary_predicate.hpp"

namespace arene {
namespace base {
namespace is_compare_detail {

template <class F, class T1, class T2, class = void>
extern constexpr bool is_equiv_valid_v = false;

template <class F, class T1, class T2>
extern constexpr bool is_equiv_valid_v<  //
    F,
    T1,
    T2,
    std::enable_if_t<  //
        std::is_same<
            bool,
            decltype(  //
                !(std::declval<F>()(std::declval<T1>(), std::declval<T2>())) &&
                !(std::declval<F>()(std::declval<T2>(), std::declval<T1>()))
            )>::value>  //
    > = true;

}  // namespace is_compare_detail

/// @brief determines if a type satisfies the Compare named requirement
/// @tparam F the possible compare function object
/// @tparam T1 type of the first argument
/// @tparam T2 type of the second argument
///
/// Compare is a set of requirements expected by some of the standard library
/// facilities from the user-provided function object types.
///
/// The return value of the function call operation applied to an object of a
/// type satisfying Compare, when converted to bool, yields true if the first
/// argument of the call appears before the second in the strict weak ordering
/// relation induced by this type, and false otherwise.
///
/// As with any BinaryPredicate, evaluation of that expression is not allowed to
/// call non-const functions through the dereferenced iterators and,
/// syntactically, the function call operation must accept const object
/// arguments, with the same behavior regardless of whether the arguments are
/// const or non-const.
///
/// A type satisfies Compare if
/// * the type satisfies BinaryPredicate and
/// Given @c comp, an object of type @c F and <c> equiv(a, b) </c>, an
/// expression-equivalent to <c> !comp(a, b) && !comp(b, a) </c>,
/// the following expressions must be valid and have their specified effects:
/// * <c>comp(a, b)</c> establishes a strict weak ordering
/// * <c>equiv(a, b)</c> establishes an equivalence relationship
///
/// @{
template <class F, class T1, class T2 = T1>
extern constexpr bool is_compare_v =     //
    is_binary_predicate_v<F, T1, T2> &&  //
    is_binary_predicate_v<F, T2, T1> &&  //
    is_compare_detail::is_equiv_valid_v<F, T1, T2>;
/// @}

}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_COMPARE_HPP_
