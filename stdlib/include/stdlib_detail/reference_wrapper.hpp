// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REFERENCE_WRAPPER_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REFERENCE_WRAPPER_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <functional>
// IWYU pragma: friend "stdlib_detail/.*"

#include "arene/base/constraints.hpp"
#include "arene/base/functional/function_traits.hpp"
#include "arene/base/functional/invoke.hpp"
#include "arene/base/functional/is_noexcept_part_of_function_type.hpp"
#include "arene/base/type_list/concat.hpp"
#include "arene/base/type_list/type_list.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "stdlib/include/stdlib_detail/addressof.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/forward.hpp"
#include "stdlib/include/stdlib_detail/integral_constant.hpp"
#include "stdlib/include/stdlib_detail/is_class.hpp"
#include "stdlib/include/stdlib_detail/is_function.hpp"
#include "stdlib/include/stdlib_detail/is_member_function_pointer.hpp"
#include "stdlib/include/stdlib_detail/remove_pointer.hpp"
#include "stdlib/include/stdlib_detail/remove_reference.hpp"

namespace std {

namespace reference_wrapper_detail {

/// @brief Add argument member types to reference wrapped functions and member
/// function pointers.
/// The types added depend on the number of arguments expected by the wrapped type.
/// For member function pointers the class pointer is considered as the first argument.
///
/// Default case, do not add any argument member types
///
/// @tparam Args List of args to the wrapped function
template <typename TypeList>
class mixin_argument_types {};

/// @brief One argument case, will add @c argument_type
/// @tparam Single argument to the wrapped function
template <typename Arg>
class mixin_argument_types<arene::base::type_list<Arg>> {
 public:
  /// @brief Type of the wrapped function's argument
  using argument_type = Arg;
};

/// @brief Two argument case, will add @c first_argument_type and @c second_argument_type
/// @tparam Arg1 First argument to the wrapped function
/// @tparam Arg2 Second argument to the wrapped function
template <typename Arg1, typename Arg2>
class mixin_argument_types<arene::base::type_list<Arg1, Arg2>> {
 public:
  /// @brief Type of the wrapped function's first argument
  using first_argument_type = Arg1;
  /// @brief Type of the wrapped function's second argument
  using second_argument_type = Arg2;
};

/// @brief Forward member type @c result_type from a reference wrapped class
/// containing that member type, if present
///
/// Case where @c reference_type is not present
///
/// @tparam T Wrapped class to forward type from
template <typename T, typename = arene::base::constraints<>>
class mixin_class_result_type {};

/// @brief Case where @c reference_type is present and forwarded
/// @tparam T Wrapped class to forward type from
template <typename T>
class mixin_class_result_type<T, arene::base::constraints<typename T::result_type>> {
 public:
  /// @brief Forwarded type from the wrapped class
  using result_type = typename T::result_type;
};

/// @brief Forward member type @c argument_type from a reference wrapped class
/// containing that member type, if present
///
/// Case where @c argument_type is not present
///
/// @tparam T Wrapped class to forward types from
template <typename T, typename = arene::base::constraints<>>
class mixin_class_unary_argument_type {};

/// @brief Case where @c argument_type is present and forwarded
/// @tparam T Wrapped class to forward type from
template <typename T>
class mixin_class_unary_argument_type<T, arene::base::constraints<typename T::argument_type>> {
 public:
  /// @brief Forwarded type from the wrapped class
  using argument_type = typename T::argument_type;
};

/// @brief Forward member types @c first_argument_type and @c second_argument_type
/// from a reference wrapped class containing those member types, if present
///
/// Case where @c first_argument_type and @c second_argument_type are not present
///
/// @tparam T Wrapped class to forward types from
template <typename T, typename = arene::base::constraints<>>
/// @brief Forwarded type from the wrapped class
class mixin_class_binary_argument_type {};

/// @brief Case where @c first_argument_type and @c second_argument_type are
/// present and forwarded
/// @tparam T Wrapped class to forward types from
template <typename T>
class mixin_class_binary_argument_type<
    T,
    arene::base::constraints<typename T::first_argument_type, typename T::second_argument_type>> {
 public:
  /// @brief Forwarded type from the wrapped class
  using first_argument_type = typename T::first_argument_type;
  /// @brief Forwarded type from the wrapped class
  using second_argument_type = typename T::second_argument_type;
};

/// @brief Add member types to @c reference_wrapper based on the underlying wrapped type
///
/// Base case where the wrapped type does not match requirements and no member
/// types are added
///
/// @tparam T Wrapped type to check
template <typename T, typename = arene::base::constraints<>>
class mixin_function_types {};

/// @brief Wrapped function case, sets @c result_type based on the return type
/// of the function and potentially types based on argument types
/// @tparam T Function to extract types from
template <typename T>
class mixin_function_types<T, arene::base::constraints<std::enable_if_t<std::is_function_v<std::remove_pointer_t<T>>>>>
    : public mixin_argument_types<arene::base::function_arguments_t<std::remove_pointer_t<T>>> {
 public:
  /// @brief Return type of the wrapped function
  using result_type = arene::base::function_return_t<std::remove_pointer_t<T>>;
};

/// @brief Wrapped member function pointer case, sets @c result_type based on
/// the return type of the function and potentially types based on member class
/// and argument types
/// @tparam T Member function pointer to extract types from
template <typename T>
class mixin_function_types<T, arene::base::constraints<std::enable_if_t<std::is_member_function_pointer_v<T>>>>
    : public mixin_argument_types<arene::base::type_lists::concat_t<
          arene::base::type_list<std::remove_reference_t<
              typename arene::base::member_function_pointer_traits<T>::implicit_argument_type>*>,
          arene::base::member_function_arguments_t<T>>> {
 public:
  /// @brief Return type of the wrapped member function pointer
  using result_type = arene::base::member_function_return_t<T>;
};

/// @brief Wrapped class case, potentially forwards member types if present
/// within the class
/// @tparam T Class type to forward types from
template <typename T>
class mixin_function_types<T, arene::base::constraints<std::enable_if_t<std::is_class_v<T>>>>
    : public mixin_class_result_type<T>
    , public mixin_class_unary_argument_type<T>
    , public mixin_class_binary_argument_type<T> {};

}  // namespace reference_wrapper_detail

// parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'reference_wrapper' does not hide an identifier in
// 'tuple'"
/// @brief wraps a reference in a copyable and assignable object
/// @tparam T Type of the reference
template <typename T>
class reference_wrapper : public reference_wrapper_detail::mixin_function_types<T> {
  // parasoft-begin-suppress AUTOSAR-A5_0_3-a "False positive: there is only 1 level of pointer indirection"
  /// @brief The wrapped reference
  T* pointer_;
  // parasoft-end-suppress AUTOSAR-A5_0_3-a

 public:
  /// @brief Wrapped type
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: this is in a different scope from other 'type's"
  using type = T;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  // TODO: Implement member types for functions

  /// @brief Construct a reference_wrapper around a given input. Wrapped object must
  /// have an lvalue reference address and not be the same type of
  /// @c reference_wrapper.
  /// @tparam U Type of passed object or function to wrap
  /// @param to_wrap or function to wrap
  // NOLINTNEXTLINE(hicpp-explicit-conversions) Not explicit per C++14 spec
  reference_wrapper(T& to_wrap) noexcept
      : pointer_{addressof(to_wrap)} {}

  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
  /// @brief Cannot be constructed from an rvalue reference
  reference_wrapper(T&&) = delete;
  // parasoft-end-suppress CERT_C-EXP37-a

  /// @brief Access the stored reference
  /// @return The reference wrapped by this instance
  auto get() const noexcept -> T& { return *pointer_; }

  // parasoft-begin-suppress AUTOSAR-A13_5_2-a "Not explicit per C++14 spec"
  // parasoft-begin-suppress AUTOSAR-A13_2_3-a "False positive: Not a relational operator"
  /// @brief Access the stored reference
  /// @return The reference wrapped by this instance
  // NOLINTNEXTLINE(hicpp-explicit-conversions) Not explicit per C++14 spec
  operator T&() const noexcept { return get(); }
  // parasoft-end-suppress AUTOSAR-A13_5_2-a
  // parasoft-end-suppress AUTOSAR-A13_2_3-a

  // parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"
  /// @brief Calls the stored function with given args. Only available if the stored
  /// type is callable.
  /// @param args Arguments to forward to the stored function
  /// @return The result of invoking the wrapped function
  template <class... Args, arene::base::constraints<enable_if_t<arene::base::is_invocable_v<T&, Args...>>> = nullptr>
  auto operator()(Args&&... args) const noexcept(arene::base::is_nothrow_invocable_v<T&, Args...>)
      -> arene::base::invoke_result_t<T&, Args...> {
    return arene::base::invoke(get(), std::forward<Args>(args)...);
  }
  // parasoft-end-suppress AUTOSAR-M2_10_1-a
};
// parasoft-end-suppress AUTOSAR-A2_10_1-e

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief Create a type-deduced @c reference_wrapper
/// @tparam T type of the reference
/// @param to_wrap Object or function to wrap
/// @return A @c reference_wrapper around the input
template <typename T>
auto ref(T& to_wrap) noexcept -> reference_wrapper<T> {
  return reference_wrapper<T>{to_wrap};
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief Create a type-deduced copy of a @c reference_wrapper
/// @tparam T Type wrapped by the input wrapper
/// @param other @c reference_wrapper to copy
/// @return A copy of the input @c reference_wrapper
template <typename T>
auto ref(reference_wrapper<T> other) noexcept -> reference_wrapper<T> {
  return other;
}

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
/// @brief Cannot create a @c reference_wrapper from an rvalue reference
/// @tparam T type of the reference
template <typename T>
auto ref(T&&) = delete;
// parasoft-end-suppress CERT_C-EXP37-a

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief Create a const type-deduced @c reference_wrapper
/// @tparam T type of the reference
/// @param to_wrap Object or function to wrap
/// @return A @c reference_wrapper around the input of const-qualified type
template <typename T>
auto cref(T& to_wrap) noexcept -> reference_wrapper<T const> {
  return reference_wrapper<T const>{to_wrap};
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief Create a const type-deduced copy of a @c reference_wrapper
/// @tparam T Type wrapped by the input wrapper
/// @param other @c reference_wrapper to copy
/// @return A copy of the input @c reference_wrapper
template <typename T>
auto cref(reference_wrapper<T> other) noexcept -> reference_wrapper<T const> {
  return other;
}

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
/// @brief Cannot create a @c reference_wrapper from an rvalue reference
/// @tparam T type of the reference
template <typename T>
auto cref(T&&) = delete;
// parasoft-end-suppress CERT_C-EXP37-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REFERENCE_WRAPPER_HPP_
