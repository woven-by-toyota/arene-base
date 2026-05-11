// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_FUNCTION_TRAITS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_FUNCTION_TRAITS_HPP_

// IWYU pragma: private, include "arene/base/functional.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/functional/is_noexcept_part_of_function_type.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/type_list/type_list.hpp"

namespace arene {
namespace base {

/// @brief Denotes the cv-qualification
/// Specficially for member function pointers
enum class cv_qualifier : std::uint8_t {
  unqualified,
  const_qualified,
  volatile_qualified,
  const_volatile_qualified,
};

/// @brief Denotes the reference-qualification
/// Specficially for member function pointers
enum class reference_qualifier : std::uint8_t {
  unqualified,
  lvalue_qualified,
  rvalue_qualified,
};

namespace function_traits_detail {

/// @brief Provides the return and argument types of a function as member types
/// @tparam R Function return type
/// @tparam Args Function argument types
template <class R, class... Args>
class mixin_function_traits {
 public:
  /// @brief Function return type
  using return_type = R;
  /// @brief List of function argument types
  using argument_types = type_list<Args...>;
};

// parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property"

/// @brief Determines the return and argument types of a function
/// @tparam Fn function type
/// @note This does not provide full support for ref-qualified member functions.
///
/// This primary template is empty.
template <class Fn, class = void>
class function_traits_impl {};

/// @brief Determines the return and argument types of a function
/// @tparam R function return type
/// @tparam Args function argument types
///
/// Specialization for member functions without no qualifiers.
template <class R, class... Args>
class function_traits_impl<R(Args...)> : public mixin_function_traits<R, Args...> {
 public:
  /// @brief Member function cv-qualification
  static constexpr auto member_function_cv_qualifier = cv_qualifier::unqualified;
  /// @brief Member function reference-qualification
  static constexpr auto member_function_reference_qualifier = reference_qualifier::unqualified;
};

/// @brief Determines the return and argument types of a function
/// @tparam R function return type
/// @tparam Args function argument types
///
/// Specialization for @c noexcept qualified member functions.
template <class R, class... Args>
class function_traits_impl<R(Args...) noexcept, std::enable_if_t<is_noexcept_part_of_function_type_v<R>>>
    : public function_traits_impl<R(Args...)> {};

/// @brief Determines the return and argument types of a function
/// @tparam R function return type
/// @tparam Args function argument types
///
/// Specialization for member functions without no qualifiers.
template <class R, class... Args>
class function_traits_impl<R(Args...)&> : public mixin_function_traits<R, Args...> {
 public:
  /// @brief Member function cv-qualification
  static constexpr auto member_function_cv_qualifier = cv_qualifier::unqualified;
  /// @brief Member function reference-qualification
  static constexpr auto member_function_reference_qualifier = reference_qualifier::lvalue_qualified;
};

/// @brief Determines the return and argument types of a function
/// @tparam R function return type
/// @tparam Args function argument types
///
/// Specialization for @c noexcept qualified member functions.
template <class R, class... Args>
class function_traits_impl<R(Args...) & noexcept, std::enable_if_t<is_noexcept_part_of_function_type_v<R>>>
    : public function_traits_impl<R(Args...)&> {};

/// @brief Determines the return and argument types of a function
/// @tparam R function return type
/// @tparam Args function argument types
///
/// Specialization for member functions without no qualifiers.
template <class R, class... Args>
class function_traits_impl<R(Args...) &&> : public mixin_function_traits<R, Args...> {
 public:
  /// @brief Member function cv-qualification
  static constexpr auto member_function_cv_qualifier = cv_qualifier::unqualified;
  /// @brief Member function reference-qualification
  static constexpr auto member_function_reference_qualifier = reference_qualifier::rvalue_qualified;
};

/// @brief Determines the return and argument types of a function
/// @tparam R function return type
/// @tparam Args function argument types
///
/// Specialization for @c noexcept qualified member functions.
template <class R, class... Args>
class function_traits_impl<R(Args...) && noexcept, std::enable_if_t<is_noexcept_part_of_function_type_v<R>>>
    : public function_traits_impl<R(Args...) &&> {};

/// @brief Determines the return and argument types of a function
/// @tparam R function return type
/// @tparam Args function argument types
///
/// Specialization for member functions without no qualifiers.
template <class R, class... Args>
class function_traits_impl<R(Args...) const> : public mixin_function_traits<R, Args...> {
 public:
  /// @brief Member function cv-qualification
  static constexpr auto member_function_cv_qualifier = cv_qualifier::const_qualified;
  /// @brief Member function reference-qualification
  static constexpr auto member_function_reference_qualifier = reference_qualifier::unqualified;
};

/// @brief Determines the return and argument types of a function
/// @tparam R function return type
/// @tparam Args function argument types
///
/// Specialization for @c noexcept qualified member functions.
template <class R, class... Args>
class function_traits_impl<R(Args...) const noexcept, std::enable_if_t<is_noexcept_part_of_function_type_v<R>>>
    : public function_traits_impl<R(Args...) const> {};

/// @brief Determines the return and argument types of a function
/// @tparam R function return type
/// @tparam Args function argument types
///
/// Specialization for member functions without no qualifiers.
template <class R, class... Args>
class function_traits_impl<R(Args...) const&> : public mixin_function_traits<R, Args...> {
 public:
  /// @brief Member function cv-qualification
  static constexpr auto member_function_cv_qualifier = cv_qualifier::const_qualified;
  /// @brief Member function reference-qualification
  static constexpr auto member_function_reference_qualifier = reference_qualifier::lvalue_qualified;
};

/// @brief Determines the return and argument types of a function
/// @tparam R function return type
/// @tparam Args function argument types
///
/// Specialization for @c noexcept qualified member functions.
template <class R, class... Args>
class function_traits_impl<R(Args...) const & noexcept, std::enable_if_t<is_noexcept_part_of_function_type_v<R>>>
    : public function_traits_impl<R(Args...) const&> {};

/// @brief Determines the return and argument types of a function
/// @tparam R function return type
/// @tparam Args function argument types
///
/// Specialization for member functions without no qualifiers.
template <class R, class... Args>
class function_traits_impl<R(Args...) const&&> : public mixin_function_traits<R, Args...> {
 public:
  /// @brief Member function cv-qualification
  static constexpr auto member_function_cv_qualifier = cv_qualifier::const_qualified;
  /// @brief Member function reference-qualification
  static constexpr auto member_function_reference_qualifier = reference_qualifier::rvalue_qualified;
};

/// @brief Determines the return and argument types of a function
/// @tparam R function return type
/// @tparam Args function argument types
///
/// Specialization for @c noexcept qualified member functions.
template <class R, class... Args>
class function_traits_impl<R(Args...) const && noexcept, std::enable_if_t<is_noexcept_part_of_function_type_v<R>>>
    : public function_traits_impl<R(Args...) const&&> {};

// parasoft-begin-suppress AUTOSAR-A2_11_1-a "volatile keywork necessary for a complete implementation"

/// @brief Determines the return and argument types of a function
/// @tparam R function return type
/// @tparam Args function argument types
///
/// Specialization for member functions without no qualifiers.
template <class R, class... Args>
class function_traits_impl<R(Args...) volatile> : public mixin_function_traits<R, Args...> {
 public:
  /// @brief Member function cv-qualification
  static constexpr auto member_function_cv_qualifier = cv_qualifier::volatile_qualified;
  /// @brief Member function reference-qualification
  static constexpr auto member_function_reference_qualifier = reference_qualifier::unqualified;
};

/// @brief Determines the return and argument types of a function
/// @tparam R function return type
/// @tparam Args function argument types
///
/// Specialization for @c noexcept qualified member functions.
template <class R, class... Args>
class function_traits_impl<R(Args...) volatile noexcept, std::enable_if_t<is_noexcept_part_of_function_type_v<R>>>
    : public function_traits_impl<R(Args...) volatile> {};

/// @brief Determines the return and argument types of a function
/// @tparam R function return type
/// @tparam Args function argument types
///
/// Specialization for member functions without no qualifiers.
template <class R, class... Args>
class function_traits_impl<R(Args...) volatile&> : public mixin_function_traits<R, Args...> {
 public:
  /// @brief Member function cv-qualification
  static constexpr auto member_function_cv_qualifier = cv_qualifier::volatile_qualified;
  /// @brief Member function reference-qualification
  static constexpr auto member_function_reference_qualifier = reference_qualifier::lvalue_qualified;
};

/// @brief Determines the return and argument types of a function
/// @tparam R function return type
/// @tparam Args function argument types
///
/// Specialization for @c noexcept qualified member functions.
template <class R, class... Args>
class function_traits_impl<R(Args...) volatile & noexcept, std::enable_if_t<is_noexcept_part_of_function_type_v<R>>>
    : public function_traits_impl<R(Args...) volatile&> {};

/// @brief Determines the return and argument types of a function
/// @tparam R function return type
/// @tparam Args function argument types
///
/// Specialization for member functions without no qualifiers.
template <class R, class... Args>
class function_traits_impl<R(Args...) volatile&&> : public mixin_function_traits<R, Args...> {
 public:
  /// @brief Member function cv-qualification
  static constexpr auto member_function_cv_qualifier = cv_qualifier::volatile_qualified;
  /// @brief Member function reference-qualification
  static constexpr auto member_function_reference_qualifier = reference_qualifier::rvalue_qualified;
};

/// @brief Determines the return and argument types of a function
/// @tparam R function return type
/// @tparam Args function argument types
///
/// Specialization for @c noexcept qualified member functions.
template <class R, class... Args>
class function_traits_impl<R(Args...) volatile && noexcept, std::enable_if_t<is_noexcept_part_of_function_type_v<R>>>
    : public function_traits_impl<R(Args...) volatile&&> {};

/// @brief Determines the return and argument types of a function
/// @tparam R function return type
/// @tparam Args function argument types
///
/// Specialization for member functions without no qualifiers.
template <class R, class... Args>
class function_traits_impl<R(Args...) const volatile> : public mixin_function_traits<R, Args...> {
 public:
  /// @brief Member function cv-qualification
  static constexpr auto member_function_cv_qualifier = cv_qualifier::const_volatile_qualified;
  /// @brief Member function reference-qualification
  static constexpr auto member_function_reference_qualifier = reference_qualifier::unqualified;
};

/// @brief Determines the return and argument types of a function
/// @tparam R function return type
/// @tparam Args function argument types
///
/// Specialization for @c noexcept qualified member functions.
template <class R, class... Args>
class function_traits_impl<R(Args...) const volatile noexcept, std::enable_if_t<is_noexcept_part_of_function_type_v<R>>>
    : public function_traits_impl<R(Args...) const volatile> {};

/// @brief Determines the return and argument types of a function
/// @tparam R function return type
/// @tparam Args function argument types
///
/// Specialization for member functions without no qualifiers.
template <class R, class... Args>
class function_traits_impl<R(Args...) const volatile&> : public mixin_function_traits<R, Args...> {
 public:
  /// @brief Member function cv-qualification
  static constexpr auto member_function_cv_qualifier = cv_qualifier::const_volatile_qualified;
  /// @brief Member function reference-qualification
  static constexpr auto member_function_reference_qualifier = reference_qualifier::lvalue_qualified;
};

/// @brief Determines the return and argument types of a function
/// @tparam R function return type
/// @tparam Args function argument types
///
/// Specialization for @c noexcept qualified member functions.
template <class R, class... Args>
class function_traits_impl<
    R(Args...) const volatile & noexcept,
    std::enable_if_t<is_noexcept_part_of_function_type_v<R>>>
    : public function_traits_impl<R(Args...) const volatile&> {};

/// @brief Determines the return and argument types of a function
/// @tparam R function return type
/// @tparam Args function argument types
///
/// Specialization for member functions without no qualifiers.
template <class R, class... Args>
class function_traits_impl<R(Args...) const volatile&&> : public mixin_function_traits<R, Args...> {
 public:
  /// @brief Member function cv-qualification
  static constexpr auto member_function_cv_qualifier = cv_qualifier::const_volatile_qualified;
  /// @brief Member function reference-qualification
  static constexpr auto member_function_reference_qualifier = reference_qualifier::rvalue_qualified;
};

/// @brief Determines the return and argument types of a function
/// @tparam R function return type
/// @tparam Args function argument types
///
/// Specialization for @c noexcept qualified member functions.
template <class R, class... Args>
class function_traits_impl<
    R(Args...) const volatile && noexcept,
    std::enable_if_t<is_noexcept_part_of_function_type_v<R>>>
    : public function_traits_impl<R(Args...) const volatile&&> {};

// parasoft-end-suppress AUTOSAR-A2_11_1-a
// parasoft-end-suppress AUTOSAR-M11_0_1-a-2

/// @brief Provides aliases to the class and pointer type of a member pointer
///
/// This primary template is empty.
template <class F>
class member_pointer_traits_impl {};

/// @brief Provides aliases to the class and pointer type of a member pointer
/// @tparam T class type of a member pointer
/// @tparam P pointer type of a member pointer
template <class T, class P>
class member_pointer_traits_impl<P T::*> {
 public:
  /// @brief Associated class type
  using class_type = T;
  /// @brief Type pointed to for a pointer to member object or pointer to member function
  using pointee_type = P;
};

/// @brief Modify a type by the cv qualifier given by an enum, accessible as @c type
/// Undefined base for specialization
/// @tparam T Type to modify
/// @tparam CVQual Enum representing the cv qualifier to add
template <class T, cv_qualifier CVQual>
struct add_cv_qualifier;

/// @brief Specialization for no cv qualifier
/// @tparam T Type to modify
template <class T>
struct add_cv_qualifier<T, cv_qualifier::unqualified> {
  /// @brief Unqualified type
  using type = T;
};

/// @brief Specialization for @c const cv qualifier
/// @tparam T Type to modify
template <class T>
struct add_cv_qualifier<T, cv_qualifier::const_qualified> {
  /// @brief @c const qualified type
  using type = T const;
};

// parasoft-begin-suppress AUTOSAR-A2_11_1-a "volatile keywork necessary for a complete implementation"

/// @brief Specialization for @c volatile cv qualifier
/// @tparam T Type to modify
template <class T>
struct add_cv_qualifier<T, cv_qualifier::volatile_qualified> {
  /// @brief @c volatile qualified type
  using type = T volatile;
};

/// @brief Specialization for @c const @c volatile cv qualifier
/// @tparam T Type to modify
template <class T>
struct add_cv_qualifier<T, cv_qualifier::const_volatile_qualified> {
  /// @brief @c const @c volatile qualified type
  using type = T const volatile;
};

// parasoft-end-suppress AUTOSAR-A2_11_1-a

/// @brief Modify a type by the cv qualifier given by an enum
/// @tparam T Type to modify
/// @tparam CVQual Enum representing the cv qualifier to add
template <class T, cv_qualifier CVQual>
using add_cv_qualifier_t = typename add_cv_qualifier<T, CVQual>::type;

/// @brief Modify a type by the reference qualifier given by an enum,
/// accessable as @c type
/// Undefined base for specialization
/// @tparam T Type to modify
/// @tparam RefQual Enum representing the reference qualifier to add
template <class T, reference_qualifier RefQual>
struct add_reference_qualifier;

/// @brief Specialization for no reference qualifier
/// @tparam T Type to modify
template <class T>
struct add_reference_qualifier<T, reference_qualifier::unqualified> {
  /// @brief Unqualified type
  using type = T;
};

/// @brief Specialization for lvalue reference qualifier
/// @tparam T Type to modify
template <class T>
struct add_reference_qualifier<T, reference_qualifier::lvalue_qualified> {
  /// @brief lvalue qualified type
  using type = T&;
};

/// @brief Specialization for rvalue reference qualifier
/// @tparam T Type to modify
template <class T>
struct add_reference_qualifier<T, reference_qualifier::rvalue_qualified> {
  /// @brief rvalue qualified type
  using type = T&&;
};

/// @brief Modify a type by the reference qualifier given by an enum
/// @tparam T Type to modify
/// @tparam RefQual Enum representing the reference qualifier to add
template <class T, reference_qualifier RefQual>
using add_reference_qualifier_t = typename add_reference_qualifier<T, RefQual>::type;

}  // namespace function_traits_detail

/// @brief Contains the @c return_type, @c argument_type, and @c cv_qualifier
/// of the given function type
/// @tparam Fn Function type to build traits from
template <class Fn>
using function_traits = typename function_traits_detail::function_traits_impl<Fn>;

/// @brief Determines the return and argument types of a function
/// @tparam Fn function type
template <class Fn>
using function_return_t = typename function_traits<Fn>::return_type;

/// @brief Determines the argument types of a function
/// @tparam Fn function type
template <class Fn>
using function_arguments_t = typename function_traits<Fn>::argument_types;

/// @brief Contains the @c class_type and @c pointee_type of the given member pointer
/// @tparam F member pointer type
template <class F>
using member_pointer_traits = typename function_traits_detail::member_pointer_traits_impl<std::remove_cv_t<F>>;

/// @brief Contains the types within @c function_traits and
/// @c member_pointer_traits for a member function pointer
/// @tparam F member function pointer type
template <class F>
class member_function_pointer_traits
    : public member_pointer_traits<F>
    , public function_traits<typename member_pointer_traits<F>::pointee_type> {
  /// @brief Parent alias for easy access
  using mem_traits = member_pointer_traits<F>;
  /// @brief Parent alias for easy access
  using fun_traits = function_traits<typename member_pointer_traits<F>::pointee_type>;

 public:
  /// @brief Implicit argument to the member function pointer which is the
  /// class with cv and reference qualifiers matching the member function
  using implicit_argument_type = function_traits_detail::add_reference_qualifier_t<
      function_traits_detail::
          add_cv_qualifier_t<typename mem_traits::class_type, fun_traits::member_function_cv_qualifier>,
      fun_traits::member_function_reference_qualifier>;
};

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: the typedef *is* preceded by a comment with @brief"
/// @brief Obtains the return type of a member function pointer type
/// @tparam F member function pointer type
template <class F>
using member_function_return_t = function_return_t<typename member_pointer_traits<F>::pointee_type>;
// parasoft-end-suppress AUTOSAR-A2_7_3-a

/// @brief Obtains the argument types of a member function pointer type
/// @tparam F member function pointer type
template <class F>
using member_function_arguments_t = function_arguments_t<typename member_pointer_traits<F>::pointee_type>;

/// @brief Obtains the cv-qualification of a member function pointer type
/// @tparam F member function pointer type
template <class F>
class member_function_cv
    : public std::integral_constant<
          cv_qualifier,
          function_traits<typename member_pointer_traits<F>::pointee_type>::member_function_cv_qualifier> {};

/// @brief Obtains the reference-qualification of a member function pointer type
/// @tparam F member function pointer type
template <class F>
class member_function_reference
    : public std::integral_constant<
          reference_qualifier,
          function_traits<typename member_pointer_traits<F>::pointee_type>::member_function_reference_qualifier> {};

/// @brief An enum representing the cv-qualification of a member function pointer type
/// @tparam F member function pointer type
template <class F>
static constexpr auto member_function_cv_v = member_function_cv<F>::value;

/// @brief An enum representing the reference-qualification of a member function pointer type
/// @tparam F member function pointer type
template <class F>
static constexpr auto member_function_reference_v = member_function_reference<F>::value;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_FUNCTION_TRAITS_HPP_
