// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_INDIRECTLY_READABLE_TRAITS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_INDIRECTLY_READABLE_TRAITS_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/iterator/detail/cond_value_type.hpp"
#include "arene/base/iterator/detail/has_member_element_type.hpp"
#include "arene/base/iterator/detail/has_member_value_type.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_array.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_extent.hpp"

// IWYU pragma: private, include "arene/base/iterator.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

namespace arene {
namespace base {

namespace indirectly_readable_traits_detail {

/// @brief Helper trait for implementing @c indirectly_readable_traits .
/// @tparam Itr The input type for which to determine the @c value_type of.
/// @see arene::base::indirectly_readable_traits<Itr>
template <typename Itr, typename = constraints<>>
class indirectly_readable_traits {};

/// @brief Specialization of @c indirectly_readable_traits for array types.
/// @tparam Itr The array type for which to determine the @c value_type of.
/// @see arene::base::indirectly_readable_traits<Itr>
template <typename Itr>
class indirectly_readable_traits<Itr, constraints<std::enable_if_t<std::is_array<Itr>::value>>> {
 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: identifier 'value_type' does not hide anything"
  /// @brief The value type corresponding to the array element type.
  using value_type = std::remove_cv_t<std::remove_extent_t<Itr>>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d
};

/// @brief Specialization of @c indirectly_readable_traits for types with a @c value_type but no @c element_type.
/// @tparam Itr The input type for which to determine the @c value_type of.
/// @see arene::base::indirectly_readable_traits<Itr>
template <typename Itr>
class indirectly_readable_traits<
    Itr,
    constraints<
        std::enable_if_t<iterator_detail::has_member_value_type_v<Itr>>,
        std::enable_if_t<!iterator_detail::has_member_element_type_v<Itr>>>>
    : public iterator_detail::cond_value_type<typename Itr::value_type> {};

/// @brief Specialization of @c indirectly_readable_traits for types with an @c element_type but no @c value_type.
/// @tparam Itr The input type for which to determine the @c value_type of.
/// @see arene::base::indirectly_readable_traits<Itr>
template <typename Itr>
class indirectly_readable_traits<
    Itr,
    constraints<
        std::enable_if_t<!iterator_detail::has_member_value_type_v<Itr>>,
        std::enable_if_t<iterator_detail::has_member_element_type_v<Itr>>>>
    : public iterator_detail::cond_value_type<typename Itr::element_type> {};

/// @brief Specialization of @c indirectly_readable_traits for types with both a @c value_type and an @c element_type
/// where the two types are the same (ignoring cv-qualifiers).
/// @tparam Itr The input type for which to determine the @c value_type of.
/// @see arene::base::indirectly_readable_traits<Itr>
template <typename Itr>
class indirectly_readable_traits<
    Itr,
    constraints<
        std::enable_if_t<iterator_detail::has_member_value_type_v<Itr>>,
        std::enable_if_t<iterator_detail::has_member_element_type_v<Itr>>,
        std::enable_if_t<
            std::is_same<std::remove_cv_t<typename Itr::value_type>, std::remove_cv_t<typename Itr::element_type>>::
                value>>> : public iterator_detail::cond_value_type<typename Itr::value_type> {};

/// @brief Specialization of @c indirectly_readable_traits for types with both a @c value_type and an @c element_type
/// where the two types are different (ignoring cv-qualifiers).
/// @tparam Itr The input type for which to determine the @c value_type of.
/// @see arene::base::indirectly_readable_traits<Itr>
template <typename Itr>
class indirectly_readable_traits<
    Itr,
    constraints<
        std::enable_if_t<iterator_detail::has_member_value_type_v<Itr>>,
        std::enable_if_t<iterator_detail::has_member_element_type_v<Itr>>,
        std::enable_if_t<!std::is_same<
            std::remove_cv_t<typename Itr::value_type>,
            std::remove_cv_t<typename Itr::element_type>>::value>>> {};

}  // namespace indirectly_readable_traits_detail

/// @brief Determines the @c value_type of an input indirectly readable type, which is a type whose value is "readable"
/// via the dereference operator such as iterators, pointers, etc.
/// @tparam Itr The input type for which to determine the @c value_type of.
///
/// Let @c cond_value_type<T> be an exposition only trait which will have a member typedef @c value_type defined as
/// @c std::remove_cv_t<T> if @c std::is_object<T> is satisfied, otherwise @c value_type will not be defined .
///
/// The presence/type of the member typedef @c value_type is determined based on the following selecting rules:
/// 1. If the input type is an array, @c std::remove_cv_t<std::remove_extent_t<T>> where @c T is the pointed-to type .
/// 2. If the input type has a member typedef @c value_type but not @c element_type ,
///    <c>cond_value_type<typename Itr::value_type></c>
/// 3. If the input type has a member typedef @c element_type but not @c value_type ,
///    <c>cond_value_type<typename Itr::element_type></c>
/// 4. If the input type has both member typedefs @c value_type and @c element_type and they are the same (ignoring
///    cv-qualifiers), <c>cond_value_type<typename Itr::value_type></c>
/// 5. If the input type has both member typedefs @c value_type and @c element_type and they are different (ignoring
///    cv-qualifiers), then @c value_type will not be defined.
/// Otherwise, @c value_type will not be defined.
///
/// Users are free to specialize @c indirectly_readable_traits for their own types.
template <typename Itr>
class indirectly_readable_traits : public indirectly_readable_traits_detail::indirectly_readable_traits<Itr> {};

/// @brief Specialization of @c indirectly_readable_traits for const types.
/// @tparam Itr The input type for which to determine the @c value_type of.
/// @see arene::base::indirectly_readable_traits<Itr>
template <typename Itr>
class indirectly_readable_traits<Itr const> : public indirectly_readable_traits<Itr> {};

/// @brief Specialization of @c indirectly_readable_traits for pointer types.
/// @tparam T the pointed-to object type
///
/// Let @c cond_value_type<T> be an exposition only trait which will have a member typedef @c value_type defined as
/// @c std::remove_cv_t<T> if @c std::is_object<T> is satisfied, otherwise @c value_type will not be defined .
///
/// Then @c indirectly_readable_traits<T*> is equivalent to @c cond_value_type<T> .
template <typename T>
class indirectly_readable_traits<T*> : public iterator_detail::cond_value_type<T> {};

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_INDIRECTLY_READABLE_TRAITS_HPP_
