// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_ITERATOR_CATEGORY_TRAITS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_ITERATOR_CATEGORY_TRAITS_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/is_base_of.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_copy_assignable.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/is_pointer.hpp"
#include "arene/base/stdlib_choice/is_reference.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/is_signed.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_manipulation/give_qualifiers_to.hpp"
#include "arene/base/type_manipulation/static_if.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"

namespace arene {
namespace base {

namespace iterator_category_detail {
/// Type trait to check if a type meets the basic syntactic requirements of an
/// iterator:
/// * There is a defined @c iterator_category for that type
/// * The type is copy-constructible
/// * The type is copy-assignable
/// * The expression @c ++x yields a @c T&
/// * The expression @c *x yields an object type
/// * The expression @c *x++ yields an object type
/// where @c x is an instance of @c T
/// The value is @c true if the type meets the requirements, @c false otherwise.
/// @tparam T The type being checked
template <typename T, typename = constraints<>>
constexpr bool is_basic_iterator_v = false;

/// Specialization for types that do meet the syntactic requirements.
/// @tparam T The type to check.
template <typename T>
constexpr bool is_basic_iterator_v<
    T,
    constraints<
        typename std::iterator_traits<T>::iterator_category,
        std::enable_if_t<std::is_copy_constructible<T>::value>,
        std::enable_if_t<std::is_copy_assignable<T>::value>,
        std::enable_if_t<std::is_same<T&, decltype(++std::declval<T&>())>::value>,
        decltype(*std::declval<T&>())&,
        decltype(*std::declval<T&>()++)&>> = true;

/// A variable holding @c true if the specified type has an iterator category
/// derived from @c std::output_iterator_tag or @c false otherwise.
/// @tparam T The type to check.
template <typename T, typename = constraints<>>
constexpr bool has_output_iterator_category_v = false;

/// Specialization for types that do have an output iterator category
/// @tparam T The type to check.
template <typename T>
constexpr bool has_output_iterator_category_v<
    T,
    constraints<std::enable_if_t<
        std::is_base_of<std::output_iterator_tag, typename std::iterator_traits<T>::iterator_category>::value>>> = true;

/// @brief alias to apply the cv union of @c T1 and @c T2
/// @tparam T1 first type with cv-qualifiers
/// @tparam T2 second type with cv-qualifiers
/// @tparam X type to apply the cv union qualifiers to
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <class T1, class T2, class X>
using add_cv_union_t = arene::base::give_qualifiers_to<T2, arene::base::give_qualifiers_to<T1, X>>;
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Helper trait to determine the simple common reference of two lvalue reference types
/// @tparam T1 first lvalue reference type
/// @tparam T2 second lvalue reference type
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename T1, typename T2, typename = constraints<>>
class simple_common_lvalue_reference {};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Specialization if both types are lvalue reference types
/// @tparam T1 first lvalue reference type
/// @tparam T2 second lvalue reference type
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename T1, typename T2>
class simple_common_lvalue_reference<
    T1&,
    T2&,
    constraints<std::enable_if_t<std::is_reference<
        decltype(false ? std::declval<add_cv_union_t<T1, T2, T1>&>() : std::declval<add_cv_union_t<T1, T2, T2>&>())>::
                                     value>>> {
 public:
  /// @brief simple common reference
  using type =
      decltype(false ? std::declval<add_cv_union_t<T1, T2, T1>&>() : std::declval<add_cv_union_t<T1, T2, T2>&>());
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Helper alias to determine the simple common reference of two lvalue references
/// @tparam T1 first lvalue reference type
/// @tparam T2 second lvalue reference type
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename T1, typename T2>
using simple_common_lvalue_reference_t = typename simple_common_lvalue_reference<T1, T2>::type;
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Trait to determine the simple common reference of two reference types
/// @tparam T1 first reference type
/// @tparam T2 second reference type
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename T1, typename T2, typename = constraints<>>
class simple_common_reference {};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Alias to determine the simple common reference of two reference types
/// @tparam T1 first reference type
/// @tparam T2 second reference type
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename T1, typename T2>
using simple_common_reference_t = typename simple_common_reference<T1, T2>::type;
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Specialization if both types are lvalue references
/// @tparam T1 first reference type
/// @tparam T2 second reference type
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename T1, typename T2>
class simple_common_reference<T1&, T2&, constraints<simple_common_lvalue_reference_t<T1&, T2&>>> {
 public:
  /// @brief simple common reference
  using type = simple_common_lvalue_reference_t<T1&, T2&>;
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Specialization if both types are rvalue references
/// @tparam T1 first reference type
/// @tparam T2 second reference type
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename T1, typename T2>
class simple_common_reference<
    T1&&,
    T2&&,
    constraints<
        std::enable_if_t<
            std::is_convertible<T1&&, std::remove_reference_t<simple_common_reference_t<T1&, T2&>>&&>::value>,
        std::enable_if_t<
            std::is_convertible<T2&&, std::remove_reference_t<simple_common_reference_t<T1&, T2&>>&&>::value>>> {
 public:
  /// @brief simple common reference
  using type = std::remove_reference_t<simple_common_reference_t<T1&, T2&>>&&;
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Specialization if one type is an lvalue reference and the other type is an rvalue reference
/// @tparam T1 first reference type
/// @tparam T2 second reference type
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename A, typename B>
class simple_common_reference<
    A&,
    B&&,
    constraints<std::enable_if_t<std::is_convertible<B&&, simple_common_reference_t<A&, B const&>>::value>>> {
 public:
  /// @brief simple common reference
  using type = simple_common_reference_t<A&, B const&>;
};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Specialization if one type is an lvalue reference and the other type is an rvalue reference
/// @tparam T1 first reference type
/// @tparam T2 second reference type
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename A, typename B>
class simple_common_reference<B&&, A&> : public simple_common_reference<A&, B&&> {};
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Trait to check if two types have a common type they are both convertible to.
/// Resolves to @c true if they do, @c false otherwise.
/// @tparam T The first type to check
/// @tparam U The second type to check
///
/// @note This uses a simplification of std::common_reference
///
template <typename T, typename U, typename = constraints<>>
constexpr bool have_common_type_v = false;

/// Specialization for the case that @c T and @c U are both reference types.
/// @tparam T The first type to check
/// @tparam U The second type to check
template <typename T, typename U>
constexpr bool have_common_type_v<
    T,
    U,
    constraints<
        std::enable_if_t<std::is_reference<T>::value && std::is_reference<U>::value>,
        simple_common_reference_t<T, U>>> = true;

/// Specialization for the case that @c T and @c U do have a common type.
/// @tparam T The first type to check
/// @tparam U The second type to check
template <typename T, typename U>
constexpr bool have_common_type_v<
    T,
    U,
    constraints<
        std::enable_if_t<!(std::is_reference<T>::value && std::is_reference<U>::value)>,
        decltype(false ? std::declval<T>() : std::declval<U>())>> = true;

/// A variable for checking if a type has a valid iterator pointer trait. It is
/// @c true if the type supports @c operator-> and @c
/// std::iterator_traits<T>::pointer is the return type of @c operator->, and @c
/// false otherwise.
/// @tparam T The type being checked
template <typename T, typename = constraints<>>
constexpr bool has_valid_pointer_trait_v =
    std::is_same<typename std::iterator_traits<T>::pointer, void>::value || std::is_pointer<T>::value;

/// Specialization for the case that @c T does have an @c operator->
/// @tparam T The type being checked
template <typename T>
constexpr bool has_valid_pointer_trait_v<T, constraints<decltype(std::declval<T&>().operator->())>> =
    std::is_same<typename std::iterator_traits<T>::pointer, decltype(std::declval<T&>().operator->())>::value;

}  // namespace iterator_category_detail

/// Type trait to check if a type is an output iterator
/// The value is @c true if the type meets the requirements, @c false otherwise.
/// @tparam T The type being checked
template <typename T, typename = constraints<>>
constexpr bool is_output_iterator_v = false;

/// Specialization for types that meet the basic iterator requirements
/// @tparam T The type being checked
template <typename T>
constexpr bool
    is_output_iterator_v<T, constraints<std::enable_if_t<iterator_category_detail::is_basic_iterator_v<T>>>> =
        iterator_category_detail::has_output_iterator_category_v<T> ||
        std::is_assignable<decltype(*std::declval<T&>()++), typename std::iterator_traits<T>::value_type>::value;

/// Type trait to check if a type is an input iterator
/// The value is @c true if the type meets the requirements, @c false otherwise.
/// @tparam T The type being checked
template <typename T, typename = constraints<>>
constexpr bool is_input_iterator_v = false;

/// Specialization for types that meet the syntactic input iterator requirements
/// @tparam T The type being checked
template <typename T>
constexpr bool is_input_iterator_v<
    T,
    constraints<
        std::enable_if_t<iterator_category_detail::is_basic_iterator_v<T>>,
        typename std::iterator_traits<T>::reference&&,
        typename std::iterator_traits<T>::value_type&,
        std::enable_if_t<
            std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<T>::iterator_category>::value>,
        std::enable_if_t<is_equality_comparable_v<T>>,
        std::enable_if_t<is_inequality_comparable_v<T>>,
        std::enable_if_t<std::is_integral<typename std::iterator_traits<T>::difference_type>::value>,
        std::enable_if_t<std::is_signed<typename std::iterator_traits<T>::difference_type>::value>,
        std::enable_if_t<
            std::is_same<decltype(*std::declval<T&>()), typename std::iterator_traits<T>::reference>::value>,
        std::enable_if_t<iterator_category_detail::have_common_type_v<
            typename std::iterator_traits<T>::reference&&,
            typename std::iterator_traits<T>::value_type&>>,
        std::enable_if_t<iterator_category_detail::have_common_type_v<
            decltype(*std::declval<T&>()++)&&,
            typename std::iterator_traits<T>::value_type&>>,
        std::enable_if_t<iterator_category_detail::has_valid_pointer_trait_v<T>>>> = true;

/// Type trait to check if a type is a forward iterator
/// The value is @c true if the type meets the requirements, @c false otherwise.
/// @tparam T The type being checked
template <typename T, typename = constraints<>>
constexpr bool is_forward_iterator_v = false;

/// Specialization for types that meet the input iterator requirements
/// @tparam T The type being checked
template <typename T>
constexpr bool is_forward_iterator_v<
    T,
    constraints<
        std::enable_if_t<is_input_iterator_v<T>>,
        std::enable_if_t<
            std::is_base_of<std::forward_iterator_tag, typename std::iterator_traits<T>::iterator_category>::value>,
        std::enable_if_t<std::is_default_constructible<T>::value>,
        std::enable_if_t<std::is_same<T, decltype(std::declval<T&>()++)>::value>,
        std::enable_if_t<std::is_reference<typename std::iterator_traits<T>::reference>::value>,
        std::enable_if_t<std::is_same<
            remove_cvref_t<typename std::iterator_traits<T>::value_type>,
            remove_cvref_t<typename std::iterator_traits<T>::reference>>::value>>> = true;

/// Type trait to check if a type is a bidirectional iterator
/// The value is @c true if the type meets the requirements, @c false otherwise.
/// @tparam T The type being checked
template <typename T, typename = constraints<>>
constexpr bool is_bidirectional_iterator_v = false;

/// Specialization for types that meet the syntactic requirements for a
/// bidirectional iterator
/// @tparam T The type being checked
template <typename T>
constexpr bool is_bidirectional_iterator_v<
    T,
    constraints<
        std::enable_if_t<is_forward_iterator_v<T>>,
        std::enable_if_t<std::is_base_of<
            std::bidirectional_iterator_tag,
            typename std::iterator_traits<T>::iterator_category>::value>,
        std::enable_if_t<std::is_same<T&, decltype(--std::declval<T&>())>::value>,
        std::enable_if_t<std::is_same<T, decltype(std::declval<T&>()--)>::value>>> = true;

/// Type trait to check if a type is a random-access iterator
/// The value is @c true if the type meets the requirements, @c false otherwise.
/// @tparam T The type being checked
template <typename T, typename = constraints<>>
constexpr bool is_random_access_iterator_v = false;

/// Specialization for types that meets the syntactic requirements of a random
/// access iterator
/// @tparam T The type being checked
template <typename T>
constexpr bool is_random_access_iterator_v<
    T,
    constraints<
        std::enable_if_t<is_bidirectional_iterator_v<T>>,
        std::enable_if_t<std::is_base_of<
            std::random_access_iterator_tag,
            typename std::iterator_traits<T>::iterator_category>::value>,
        std::enable_if_t<std::is_same<
            decltype(std::declval<T&>() += std::declval<typename std::iterator_traits<T>::difference_type>()),
            T&>::value>,
        std::enable_if_t<std::is_same<
            decltype(std::declval<T&>() + std::declval<typename std::iterator_traits<T>::difference_type>()),
            T>::value>,
        std::enable_if_t<std::is_same<
            decltype(std::declval<typename std::iterator_traits<T>::difference_type>() + std::declval<T&>()),
            T>::value>,
        std::enable_if_t<std::is_same<
            decltype(std::declval<T&>() -= std::declval<typename std::iterator_traits<T>::difference_type>()),
            T&>::value>,
        std::enable_if_t<std::is_same<
            decltype(std::declval<T&>() - std::declval<typename std::iterator_traits<T>::difference_type>()),
            T>::value>,
        std::enable_if_t<std::is_same<
            decltype(std::declval<T&>() - std::declval<T&>()),
            typename std::iterator_traits<T>::difference_type>::value>,
        std::enable_if_t<std::is_same<
            decltype(std::declval<T&>()[std::declval<typename std::iterator_traits<T>::difference_type>()]),
            typename std::iterator_traits<T>::reference>::value>,
        std::enable_if_t<is_less_than_comparable_v<T>>,
        std::enable_if_t<is_greater_than_comparable_v<T>>,
        std::enable_if_t<is_less_than_or_equal_comparable_v<T>>,
        std::enable_if_t<is_greater_than_or_equal_comparable_v<T>>>> = true;

/// Type trait for checking if a type is an iterator.
/// The value is @c true if the type meets the requirements, @c false otherwise.
/// @tparam T The type being checked
template <typename T>
constexpr bool is_iterator_v =
    is_input_iterator_v<T> || (iterator_category_detail::has_output_iterator_category_v<T> && is_output_iterator_v<T>);

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_ITERATOR_CATEGORY_TRAITS_HPP_
