// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::type_traits::is_predicate"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_PREDICATE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_PREDICATE_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace arene {
namespace base {
namespace is_predicate_detail {

/// @brief implementation trait for @c is_predicate
/// @tparam F the possible predicate
/// @tparam Args argument types
///
/// Primary template which is always @c false_type.
///
template <class F, class Args, class = void>
class is_predicate_impl : public std::false_type {};

/// @brief implementation trait for @c is_predicate
/// @tparam F the possible predicate
/// @tparam Args argument types
///
/// Template specialization selected if @c F is invocable with @c Args ... and
/// the result is convertible to @c bool.
///
template <class F, class... Args>
class is_predicate_impl<
    F,
    type_list<Args...>,
    std::enable_if_t<                                                     //
        is_invocable_v<F, Args...> &&                                     //
        std::is_convertible<invoke_result_t<F, Args...>, bool>::value &&  //
        std::is_convertible<decltype(!std::declval<invoke_result_t<F, Args...>>()), bool>::value>>
    : public std::true_type {};

}  // namespace is_predicate_detail

/// @brief backport for the C++20 predicate concept
/// @tparam F the possible predicate
/// @tparam Args the argument types that the predicate is invoked with
///
/// The concept @c predicate<F, Args...> specifies that @c F is a predicate that
/// accepts arguments whose types and value categories are encoded by Args...,
/// i.e., it can be invoked with these arguments to produce a boolean-testable
/// result.
///
/// @{
template <class F, class... Args>
extern constexpr bool is_predicate_v = is_predicate_detail::is_predicate_impl<F, type_list<Args...>>::value;
/// @}

}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_PREDICATE_HPP_
