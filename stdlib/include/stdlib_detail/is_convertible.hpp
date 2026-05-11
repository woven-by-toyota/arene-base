// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_CONVERTIBLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_CONVERTIBLE_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// parasoft-begin-suppress AUTOSAR-A2_11_1-a-2 "volatile usage required for correct implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "arene/base/constraints.hpp"
#include "stdlib/include/stdlib_detail/declval.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/integral_constant.hpp"
#include "stdlib/include/stdlib_detail/is_same.hpp"
#include "stdlib/include/stdlib_detail/remove_cv.hpp"

namespace std {

namespace is_convertible_detail {
// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: these is a comment with @brief"
// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "This must be the same declaration in all TUs to avoid ODR violations"
// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-A5_0_3-a "False positive: The argument is a template parameter"
/// @brief Implementation function declared but not defined that accepts an argument of the specified type
/// @tparam To The type of the argument
template <typename To>
void check(To) noexcept;
// parasoft-end-suppress AUTOSAR-A5_0_3-a
// parasoft-end-suppress CERT_C-EXP37-a-3
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2
// parasoft-end-suppress AUTOSAR-A2_7_3-a

/// @brief Type trait for checking if @c TypeToCheck has the same cv-qualification as @c Baseline, or is more
/// cv-qualified.
/// @tparam TypeToCheck The type to check
/// @tparam Baseline The type to check against
template <typename TypeToCheck, typename Baseline, typename = arene::base::constraints<>>
extern constexpr bool is_same_or_more_cv_qualified_v = false;

/// @brief Type trait for checking if @c TypeToCheck has the same cv-qualification as @c Baseline, or is more
/// cv-qualified.
///
/// This specialization is for the case that @c Baseline is not cv-qualified
/// @tparam TypeToCheck The type to check
/// @tparam Baseline The type to check against
template <typename TypeToCheck, typename Baseline>
extern constexpr bool is_same_or_more_cv_qualified_v<
    TypeToCheck,
    Baseline,
    arene::base::constraints<enable_if_t<is_same_v<remove_cv_t<Baseline>, Baseline>>>> = true;

/// @brief Type trait for checking if @c TypeToCheck has the same cv-qualification as @c Baseline, or is more
/// cv-qualified.
///
/// This specialization is for the case that @c Baseline is @c const and @c TypeToCheck is @c const
/// @tparam TypeToCheck The type to check
/// @tparam Baseline The type to check against
template <typename TypeToCheck, typename Baseline>
extern constexpr bool is_same_or_more_cv_qualified_v<
    TypeToCheck const,
    Baseline const,
    arene::base::constraints<
        enable_if_t<is_same_v<remove_cv_t<TypeToCheck>, TypeToCheck>>,
        enable_if_t<is_same_v<remove_cv_t<Baseline>, Baseline>>>> = true;

/// @brief Type trait for checking if @c TypeToCheck has the same cv-qualification as @c Baseline, or is more
/// cv-qualified.
///
/// This specialization is for the case that @c Baseline is @c const and @c TypeToCheck is @c const @c volatile
/// @tparam TypeToCheck The type to check
/// @tparam Baseline The type to check against
template <typename TypeToCheck, typename Baseline>
extern constexpr bool is_same_or_more_cv_qualified_v<
    TypeToCheck const volatile,
    Baseline const,
    arene::base::constraints<
        enable_if_t<is_same_v<remove_cv_t<TypeToCheck>, TypeToCheck>>,
        enable_if_t<is_same_v<remove_cv_t<Baseline>, Baseline>>>> = true;

/// @brief Type trait for checking if @c TypeToCheck has the same cv-qualification as @c Baseline, or is more
/// cv-qualified.
///
/// This specialization is for the case that @c Baseline is @c volatile and @c TypeToCheck is @c volatile
/// @tparam TypeToCheck The type to check
/// @tparam Baseline The type to check against
template <typename TypeToCheck, typename Baseline>
extern constexpr bool is_same_or_more_cv_qualified_v<
    TypeToCheck volatile,
    Baseline volatile,
    arene::base::constraints<
        enable_if_t<is_same_v<remove_cv_t<TypeToCheck>, TypeToCheck>>,
        enable_if_t<is_same_v<remove_cv_t<Baseline>, Baseline>>>> = true;

/// @brief Type trait for checking if @c TypeToCheck has the same cv-qualification as @c Baseline, or is more
/// cv-qualified.
///
/// This specialization is for the case that @c Baseline is @c volatile and @c TypeToCheck is @c const @c volatile
/// @tparam TypeToCheck The type to check
/// @tparam Baseline The type to check against
template <typename TypeToCheck, typename Baseline>
extern constexpr bool is_same_or_more_cv_qualified_v<
    TypeToCheck const volatile,
    Baseline volatile,
    arene::base::constraints<
        enable_if_t<is_same_v<remove_cv_t<TypeToCheck>, TypeToCheck>>,
        enable_if_t<is_same_v<remove_cv_t<Baseline>, Baseline>>>> = true;

/// @brief Type trait for checking if @c TypeToCheck has the same cv-qualification as @c Baseline, or is more
/// cv-qualified.
///
/// This specialization is for the case that @c Baseline is @c const @c volatile and @c TypeToCheck is @c const @c
/// volatile
/// @tparam TypeToCheck The type to check
/// @tparam Baseline The type to check against
template <typename TypeToCheck, typename Baseline>
extern constexpr bool is_same_or_more_cv_qualified_v<
    TypeToCheck const volatile,
    Baseline const volatile,
    arene::base::constraints<
        enable_if_t<is_same_v<remove_cv_t<TypeToCheck>, TypeToCheck>>,
        enable_if_t<is_same_v<remove_cv_t<Baseline>, Baseline>>>> = true;

/// @brief Type trait to detect if one type is convertible to another
/// @tparam From The type to (try to) convert from
/// @tparam To The type to (try to) convert to
template <typename From, typename To, typename = arene::base::constraints<>>
extern constexpr bool is_convertible_v = false;

/// @brief Type trait to detect if one type is convertible to another.
///
/// This specialization handles the case that they are
/// @tparam From The type to (try to) convert from
/// @tparam To The type to (try to) convert to
template <typename From, typename To>
extern constexpr bool
    is_convertible_v<From, To, arene::base::constraints<decltype(is_convertible_detail::check<To>(declval<From>()))>> =
        true;

/// @brief Type trait to detect if one type is convertible to another.
///
/// This specialization handles the case that the source and target are both @c void
/// @tparam From The type to (try to) convert from
/// @tparam To The type to (try to) convert to
template <typename From, typename To>
extern constexpr bool is_convertible_v<
    From,
    To,
    arene::base::constraints<  //
        enable_if_t<is_same_v<remove_cv_t<From>, void>>,
        enable_if_t<is_same_v<remove_cv_t<To>, void>>>> =
    is_convertible_detail::is_same_or_more_cv_qualified_v<To, From>;

}  // namespace is_convertible_detail

/// @brief Type trait to detect if one type is convertible to another
/// @tparam From The type to (try to) convert from
/// @tparam To The type to (try to) convert to
template <typename From, typename To>
extern constexpr bool is_convertible_v = is_convertible_detail::is_convertible_v<From, To>;

/// @brief Type trait to detect if one type is convertible to another
/// @tparam From The type to (try to) convert from
/// @tparam To The type to (try to) convert to
template <typename From, typename To>
class is_convertible : public bool_constant<is_convertible_v<From, To>> {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_CONVERTIBLE_HPP_
