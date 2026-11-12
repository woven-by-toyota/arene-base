// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_INCREMENTABLE_TRAITS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_INCREMENTABLE_TRAITS_HPP_

// IWYU pragma: private, include "arene/base/iterator.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/is_object.hpp"
#include "arene/base/stdlib_choice/make_signed.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

namespace arene {
namespace base {

namespace incrementable_traits_detail {

/// @brief Helper for deducing the difference type of an iterator type
/// @tparam Itr The iterator type
/// @see ::arene::base::incrementable_traits<Itr>
template <typename Itr, typename = constraints<>>
class incrementable_traits_impl {};

/// @brief Deduces the difference type of an iterator type from its member typedef @c difference_type
/// @tparam Itr The iterator type
template <typename Itr>
using use_difference_type = typename Itr::difference_type;

/// @brief Helper to determine if an iterator type has a member typedef @c difference_type
/// @tparam Itr The iterator type to check
template <typename Itr>
constexpr bool has_difference_type_v = substitution_succeeds<use_difference_type, Itr>;

/// @brief Specialization for types that have a member typedef @c difference_type
/// @tparam Itr The iterator type. Must satisfy @c has_difference_type_v<Itr>
template <typename Itr>
class incrementable_traits_impl<Itr, constraints<std::enable_if_t<has_difference_type_v<Itr>>>> {
 public:
  /// @brief The difference type of the iterator type.
  using difference_type = typename Itr::difference_type;
};

/// @brief Deduces the difference type of an iterator type from the result of its subtraction operator.
/// @tparam Itr The iterator type
template <typename Itr>
using subtraction_result_t = decltype(std::declval<Itr const&>() - std::declval<Itr const&>());

/// @brief Helper to determine if an iterator type has a subtraction operator
/// @tparam Itr The iterator type to check
template <typename Itr>
constexpr bool has_subtraction_result_v = substitution_succeeds<subtraction_result_t, Itr>;

/// @brief Helper to determine if an iterator type has a subtraction operator that returns an integral type
/// @tparam Itr The iterator type to check
template <typename Itr, bool = has_subtraction_result_v<Itr>>
constexpr bool has_integral_subtraction_result_v = false;

/// @brief Helper to determine if an iterator type has a subtraction operator that returns an integral type
/// @tparam Itr The iterator type to check
template <typename Itr>
constexpr bool has_integral_subtraction_result_v<Itr, true> = std::is_integral<subtraction_result_t<Itr>>::value;

/// @brief Specialization for when the iterator doesn't have iterator_traits but does have subtraction operator
/// @tparam Itr The iterator type. Must satisfy @c has_integral_subtraction_result_v<Itr>
template <typename Itr>
class incrementable_traits_impl<
    Itr,
    constraints<
        std::enable_if_t<!has_difference_type_v<Itr>>,
        std::enable_if_t<has_integral_subtraction_result_v<Itr>>>> {
 public:
  /// @brief The difference type of the iterator type.
  using difference_type = std::make_signed_t<subtraction_result_t<Itr>>;
};

/// @brief Specialization of @c incrementable_traits for pointer types.
///
/// @tparam T the type pointed to by the pointer type.
template <typename T, bool = std::is_object<T>::value>
class incrementable_traits_pointer_specialization {
 public:
  /// @brief The difference type of the pointer type.
  using difference_type = std::ptrdiff_t;
};

/// @brief Specialization of @c incrementable_traits for pointer types where @c T is not an object type.
template <typename T>
class incrementable_traits_pointer_specialization<T, false> {};

}  // namespace incrementable_traits_detail

/// @brief Computes the difference type of an incrementable iterator type.
///
/// @tparam Itr The incrementable iterator type for which to compute the difference type.
///
/// The member typedef @c difference_type will be present and defined if either of the following are true, in order of
/// preference:
/// 1. A specialization of @c incrementable_traits is provided for @c Itr that defines a member typedef
///    @c difference_type.
/// 2. @c Itr has a member typedef @c difference_type.
/// 3. The expression @c std::declval<Itr>()-std::declval<Itr>() is well-formed and has an integral type.
///
/// Otherwise, the member typedef @c difference_type will not be present.
template <typename Itr>
class incrementable_traits : public incrementable_traits_detail::incrementable_traits_impl<Itr> {};

/// @brief Specialization of @c incrementable_traits for pointer types.
/// @tparam T the type pointed to by the pointer type.
///
/// If @c T satisfies @c std::is_object<T>, the member typedef @c difference_type will be defined as @c std::ptrdiff_t ,
/// otherwise it will be absent.
template <typename T>
class incrementable_traits<T*> : public incrementable_traits_detail::incrementable_traits_pointer_specialization<T> {};

/// @brief Specialization of @c incrementable_traits for const types, dispatches to the non-const specialization.
/// @tparam Itr the const type for which to compute the difference type.
/// @see ::arene::base::incrementable_traits<Itr>
template <typename Itr>
class incrementable_traits<Itr const> : public incrementable_traits<Itr> {};

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_INCREMENTABLE_TRAITS_HPP_
