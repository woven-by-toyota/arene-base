// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "Header defines a group of traits, so the name reflects that"
// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_COMMON_REFERENCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_COMMON_REFERENCE_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/stdlib_choice/add_pointer.hpp"
#include "arene/base/stdlib_choice/common_type.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_reference.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_manipulation/give_qualifiers_to.hpp"
#include "arene/base/type_traits/priority_tag.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"

// IWYU pragma: no_include "arene/base/type_manipulation/static_if.hpp"

namespace arene {
namespace base {

/// @brief A customization point to allow defining a common reference type for two types.
/// @tparam T The first type for which to define a common reference.
/// @tparam U The second type for which to define a common reference.
/// @tparam TQual A template that applies cv-qualifiers to T.
/// @tparam UQual A template that applies cv-qualifiers to U.
///
/// A specialization of this trait must be a partial specialization across @c T and @c U and must satisfy the following
/// constraints:
/// - @c std::is_same<T,decay_t<T>>::value must be true.
/// - @c std::is_same<U,decay_t<U>>::value must be true.
/// - At least one of @c T or @c U must be a user-defined type.
///
/// If there is a common reference type between the two types, the specialization must define a member type named @c
/// type that satisfies the following constraints:
/// - @c TQual<T>::type must be convertible to @c type.
/// - @c UQual<U>::type must be convertible to @c type.
///
/// In addition, @c basic_common_reference<T,U,TQual,UQual>::type and @c basic_common_reference<U,T,UQual,TQual>::type
/// must be the same type or both must not define a member type named @c type .
template <class T, class U, template <class> class TQual, template <class> class UQual>
class basic_common_reference {};

/// @brief A trait which determines the common reference type for a set of types.
/// @tparam Ts The types for which to determine the common reference.
///
/// The _common reference type_ is defined as follows:
/// - If @c sizeof...(Ts) is 0, @c type is not defined.
/// - If @c sizeof...(Ts) is 1, @c type names the only type in the pack.
/// - If @c sizeof...(Ts) is 2, the following alternatives are considered in order:
///   - Let @c R be @c COMMON-REF(T0,T1). If @c T0 and @c T1 are reference types, @c R is well-formed, and both
///     pointer-conversion checks to @c R succeed, @c type names @c R.
///   - Otherwise, if @c basic_common_reference<remove_cvref_t<T0>,remove_cvref_t<T1>,T0Qual,T1Qual>::type is defined,
///     @c type names that type.
///   - Otherwise, if @c COND-RES(T0,T1) is well-formed, @c type names that type.
///   - Otherwise, if @c std::common_type_t<T0,T1> is well-formed, @c type names that type.
///   - Otherwise, @c type is not defined.
/// - If @c sizeof...(Ts) is greater than 2, let @c C be @c common_reference_t<T0,T1>. If @c C is well-formed, @c type
///   names the type of @c common_reference<C,Ts...>; otherwise, @c type is not defined.
///
/// The exposition-only @c COMMON-REF(A,B) operation is defined as follows. Let @c X and @c Y be
/// the results of applying @c remove_reference_t to @c A and @c B, respectively. Let @c CREF(A) be a const lvalue
/// reference to @c X, @c COPYCV(FROM,TO) be @c TO with the top-level cv-qualifiers of @c FROM, and
/// @c COND-RES(X,Y) be <c>decltype(false ? std::declval<X(&)()>()() : std::declval<Y(&)()>()())</c>.
/// - If @c A and @c B are both lvalue reference types, @c COMMON-REF(A,B) is
///   @c COND-RES(COPYCV(X,Y)&,COPYCV(Y,X)&) if that type exists and is a reference type.
/// - If @c A and @c B are both rvalue reference types, let @c C be
///   @c remove_reference_t<COMMON-REF(X&,Y&)>&&. If @c C is well-formed and both types are convertible to @c C,
///   @c COMMON-REF(A,B) is @c C.
/// - If @c A is an rvalue reference and @c B is an lvalue reference, let @c D be @c COMMON-REF(CREF(X),Y&). If
///   @c D is well-formed and @c A is convertible to @c D, @c COMMON-REF(A,B) is @c D.
/// - If @c A is an lvalue reference and @c B is an rvalue reference, @c COMMON-REF(A,B) is @c COMMON-REF(B,A).
/// - Otherwise, @c COMMON-REF(A,B) is not defined.
template <typename... Ts>
class common_reference {};

/// @brief Specialization of @c common_reference for a single type.
/// @tparam T0 The single type for which to determine the common reference.
/// @see common_reference<Ts...>
template <typename T0>
class common_reference<T0> {
 public:
  /// @brief The common reference type of a single type, which is itself.
  using type = T0;
};

/// @brief Alias template to extract the common reference type for a set of types.
/// @tparam Ts The types for which to determine the common reference.
/// Equivalent to @c common_reference<Ts...>::type .
template <typename... Ts>
using common_reference_t = typename common_reference<Ts...>::type;

namespace common_reference_detail {

/// @brief Alias trait for @c basic_common_reference<...>::type
///
/// @tparam T The first type for which to define a common reference.
/// @tparam U The second type for which to define a common reference.
/// @tparam TQual A template that applies cv-qualifiers to T.
/// @tparam UQual A template that applies cv-qualifiers to U.
/// @see basic_common_reference<T,U,TQual,UQual>
template <class T, class U, template <class> class TQual, template <class> class UQual>
using basic_common_reference_t = typename basic_common_reference<T, U, TQual, UQual>::type;

// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
/// @brief Helper struct to deduce the common reference type of two types through overload resolution.
class common_ref_deducer {
 public:
  /// @brief Helper alias template to deduce the common reference type of two types.
  /// @tparam X The first type. Must be a non-reference
  /// @tparam Y The second type. Must be a non-reference
  /// @tparam CVUX The type X with the cv-qualifiers of Y applied.
  /// @tparam CVUY The type Y with the cv-qualifiers of X applied.
  /// @return The common reference type of X and Y if it is defined.
  template <typename X, typename Y, typename CVUX = give_qualifiers_to<Y, X>, typename CVUY = give_qualifiers_to<X, Y>>
  using common_ref_t = decltype(false ? std::declval<CVUX&>() : std::declval<CVUY&>());

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a "False positive: Constrained via SFINAE"
  /// @brief Helper function selected when both arguments are lvalue references.
  /// @tparam X The first non-reference type.
  /// @tparam Y The second non-reference type.
  /// @return The common reference type of X and Y if it is defined.
  /// @see common_reference
  template <typename X, typename Y>
  static constexpr auto type(X&, Y&) -> common_ref_deducer::common_ref_t<X, Y>;

  /// @brief Helper function selected when both arguments are rvalue references.
  /// @tparam X The first non-reference type.
  /// @tparam Y The second non-reference type.
  /// @tparam C The deduced common rvalue reference type.
  /// @return @c C if both arguments are convertible to it.
  /// @see common_reference
  template <
      typename X,
      typename Y,
      typename C = std::remove_reference_t<common_ref_deducer::common_ref_t<X, Y>>&&,
      constraints<
          std::enable_if_t<std::is_convertible<X&&, C>::value>,
          std::enable_if_t<std::is_convertible<Y&&, C>::value>> = nullptr>
  static constexpr auto type(X&&, Y&&) -> C;

  /// @brief Helper function selected when the first argument is an lvalue reference and the second is an rvalue
  /// reference.
  /// @tparam X The first non-reference type.
  /// @tparam Y The second non-reference type.
  /// @tparam D The deduced common reference type.
  /// @return @c D if the second argument is convertible to it.
  /// @see common_reference
  template <
      typename X,
      typename Y,
      typename D = common_ref_deducer::common_ref_t<X, Y const>,
      constraints<std::enable_if_t<std::is_convertible<Y&&, D>::value>> = nullptr>
  static constexpr auto type(X&, Y&&) -> D;

  /// @brief Helper function selected when the first argument is an rvalue reference and the second is an lvalue
  /// reference.
  /// @tparam X The first non-reference type.
  /// @tparam Y The second non-reference type.
  /// @tparam D The deduced common reference type.
  /// @return @c D if the first argument is convertible to it.
  /// @see common_reference
  template <
      typename X,
      typename Y,
      typename D = common_ref_deducer::common_ref_t<Y, X const>,
      constraints<std::enable_if_t<std::is_convertible<X&&, D>::value>> = nullptr>
  static constexpr auto type(X&&, Y&) -> D;
  // parasoft-end-suppress AUTOSAR-A13_3_1-a "False positive: Constrained via SFINAE"
};
// parasoft-end-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"

/// @brief Alias template to determine the common reference type for two types via @c common_ref_deducer .
/// @tparam T0 The first type. Must be a reference type.
/// @tparam T1 The second type. Must be a reference type.
/// @return The common reference type of T0 and T1 if it is defined, otherwise malformed.
template <typename T0, typename T1>
using common_ref_t = decltype(common_ref_deducer::type(std::declval<T0>(), std::declval<T1>()));

/// @brief Exposition-only helper that provides the COPYCV operation for a source type.
/// @tparam Source The source type from which to extract cv-qualifiers.
template <typename Source>
class copy_cv {
 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: identifier 'apply' does not hide anything"
  /// @brief Template alias to apply the cv-qualifiers of the source type to another type.
  /// @tparam T The type to which the cv-qualifiers of @c Source will be applied.
  template <typename T>
  using apply = give_qualifiers_to<Source, T>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d "False positive: identifier 'apply' does not hide anything"
};

// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
/// @brief Helper struct to deduce the common reference type of two types. It contains overloads of @c type<T0,T1> whose
/// return type is the common reference type if it is defined. If it is not defined, then @c type<T0,T1> does not
/// participate in overload resolution.

class common_reference_2deducer {
 public:
  /// @brief Overload of @c type for when both T0 and T1 are reference types and a common reference type can be deduced.
  /// @tparam T0 The first type.
  /// @tparam T1 The second type.
  /// @tparam CommonReferenceT The common reference type deduced for T0 and T1.
  /// @return The common reference type of T0 and T1
  template <
      typename T0,
      typename T1,
      typename CommonReferenceT = common_ref_t<T0, T1>,
      constraints<
          std::enable_if_t<std::is_reference<T0>::value && std::is_reference<T1>::value>,
          std::enable_if_t<std::is_convertible<std::add_pointer_t<T0>, std::add_pointer_t<CommonReferenceT>>::value>,
          std::enable_if_t<std::is_convertible<std::add_pointer_t<T1>, std::add_pointer_t<CommonReferenceT>>::value>> =
          nullptr>
  static auto type(priority_tag<3U>) -> CommonReferenceT;

  /// @brief Overload of @c type for when the type pairing has a specialization of @c basic_common_reference .
  /// @tparam T0 The first type.
  /// @tparam T1 The second type.
  /// @return The common reference type as defined by @c basic_common_reference for the given type pairing.
  template <typename T0, typename T1>
  static auto type(priority_tag<2U>) -> basic_common_reference_t<
      remove_cvref_t<T0>,
      remove_cvref_t<T1>,
      copy_cv<T0>::template apply,
      copy_cv<T1>::template apply>;

  /// @brief Overload of @c type for when the type pairing does not have a common reference type or a specialization of
  /// @c basic_common_reference but the common reference can be deduced by implicit conversion.
  /// @tparam T0 The first type.
  /// @tparam T1 The second type.
  /// @return The common reference type as deduced by the conditional operator.
  template <typename T0, typename T1>
  static auto type(priority_tag<1U>) -> decltype(false ? std::declval<T0 (&)()>()() : std::declval<T1 (&)()>()());

  /// @brief Overload of @c type for when the type pairing does not have a common reference type, a specialization of @c
  /// basic_common_reference, and cannot be deduced by implicit conversion.
  /// @tparam T0 The first type.
  /// @tparam T1 The second type.
  /// @return The common reference type as determined by @c std::common_type.
  template <typename T0, typename T1>
  static auto type(priority_tag<0U>) -> std::common_type_t<T0, T1>;
};
// parasoft-end-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"

/// @brief Alias template for deducing the common reference type of two types using @c common_reference_2deducer.
/// @tparam T0 The first type.
/// @tparam T1 The second type.
/// @return The common reference type of T0 and T1 if it is defined, otherwise malformed.
template <typename T0, typename T1>
using common_reference_2deducer_t = decltype(common_reference_2deducer::type<T0, T1>(priority_tag<3U>{}));

/// @brief Implementation detail for determining the common reference type for two types using @c
/// common_reference_2deducer_t.
/// @tparam T0 The first type.
/// @tparam T1 The second type.
/// If a common reference exists for T0 and T1, this class will have a member typedef @c type representing that common
/// reference, otherwise it will have no member typedef defined.
template <typename T0, typename T1, typename = constraints<>>
class common_reference_2impl {};

/// @brief Specialization of @c common_reference_2impl when a common reference exists for two types.
/// @tparam T0 The first type.
/// @tparam T1 The second type.
template <typename T0, typename T1>
class common_reference_2impl<T0, T1, constraints<common_reference_2deducer_t<T0, T1>>> {
 public:
  /// @brief The common reference type of T0 and T1 as determined by @c common_reference_2deducer_t.
  using type = common_reference_2deducer_t<T0, T1>;
};

/// @brief Implementation detail for determining the common reference type for three or more types.
/// @tparam CommonInitialReference Indicates whether a common reference exists for the initial two types.
/// @tparam T0 The first type.
/// @tparam T1 The second type.
/// @tparam Ts The remaining types.
/// The default implementation does not define a member typedef @c type
template <bool CommonInitialReference, typename T0, typename T1, typename... Ts>
class common_reference_3impl {};

/// @brief Specialization of @c common_reference_3impl when a common reference exists for the initial two types.
/// @tparam T0 The first type.
/// @tparam T1 The second type.
/// @tparam Ts The remaining types.
template <typename T0, typename T1, typename... Ts>
class common_reference_3impl<true, T0, T1, Ts...>
    : public common_reference<common_reference_2deducer_t<T0, T1>, Ts...> {};
}  // namespace common_reference_detail

/// @brief Specialization of @c common_reference for two types.
/// @tparam T0 The first type for which to determine the common reference.
/// @tparam T1 The second type for which to determine the common reference.
/// @see common_reference<Ts...>
template <typename T0, typename T1>
class common_reference<T0, T1> : public common_reference_detail::common_reference_2impl<T0, T1> {};

/// @brief Specialization of @c common_reference for more than two types.
/// @tparam T0 The first type for which to determine the common reference.
/// @tparam T1 The second type for which to determine the common reference.
/// @tparam Ts The remaining types for which to determine the common reference.
/// @see common_reference<Ts...>
template <typename T0, typename T1, typename... Ts>
class common_reference<T0, T1, Ts...>
    : public common_reference_detail::common_reference_3impl<
          substitution_succeeds<common_reference_detail::common_reference_2deducer_t, T0, T1>,
          T0,
          T1,
          Ts...> {};

namespace common_reference_with_detail {
/// @brief Trait to determine if two types have a common reference.
/// @tparam T The first type.
/// @tparam U The second type.
/// The default implementation is false
template <typename T, typename U, typename = constraints<>>
class has_common_reference_with : public std::false_type {};

/// @brief Specialization of @c has_common_reference_with for types that satisfy the common reference constraints.
/// @tparam T The first type.
/// @tparam U The second type.
template <typename T, typename U>
class has_common_reference_with<
    T,
    U,
    constraints<
        std::enable_if_t<std::is_same<common_reference_t<T, U>, common_reference_t<U, T>>::value>,
        std::enable_if_t<std::is_convertible<T, common_reference_t<T, U>>::value>,
        std::enable_if_t<std::is_convertible<U, common_reference_t<T, U>>::value>>> : public std::true_type {};
}  // namespace common_reference_with_detail

/// @brief Query trait to determine if two types have a common reference. A backport of the
/// @c [std::common_reference_with](https://en.cppreference.com/cpp/concepts/common_reference_with) concept.
/// @tparam T The first type.
/// @tparam U The second type.
/// @return bool @c true if all of the following are satisfied, else @c false :
/// - @c common_reference_t<T,U> is the same as @c common_reference_t<U,T>.
/// - @c T is convertible to @c common_reference_t<T,U>.
/// - @c U is convertible to @c common_reference_t<T,U>.
/// @note The semantic requirement of this trait is that conversion to the common reference type must be _equality
/// perserving_.
template <typename T, typename U>
extern constexpr bool has_common_reference_with_v =
    common_reference_with_detail::has_common_reference_with<T, U>::value;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_COMMON_REFERENCE_HPP_
