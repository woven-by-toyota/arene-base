// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_POINTER_TRAITS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_POINTER_TRAITS_HPP_

#include "arene/base/constraints.hpp"
#include "stdlib/include/stdlib_detail/addressof.hpp"
#include "stdlib/include/stdlib_detail/conditional.hpp"
#include "stdlib/include/stdlib_detail/cstddef.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/integral_constant.hpp"
#include "stdlib/include/stdlib_detail/is_void.hpp"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <memory>
// IWYU pragma: friend "stdlib_detail/.*"

namespace std {

namespace pointer_traits_detail {

/// @brief Require that @c PointerLike has a type member named @c element_type
template <class PointerLike>
using require_element_type = typename PointerLike::element_type;

/// @brief @c true if @c PointerLike has a type member named @c element_type
template <class PointerLike>
constexpr extern bool has_element_type_v = arene::base::substitution_succeeds<require_element_type, PointerLike>;

/// @brief Determine the @c element_type to expose in @c pointer_traits
template <class PointerLike, class = arene::base::constraints<>>
struct pointer_traits_element_type {};

/// @brief Determine the @c element_type to expose in the @c pointer_traits
///
/// Use @c PointerLike::element_type if defined
template <class PointerLike>
struct pointer_traits_element_type<
    PointerLike,
    arene::base::constraints<enable_if_t<has_element_type_v<PointerLike>>>> {
  /// @brief The resulting type
  using type = typename PointerLike::element_type;
};

/// @brief Determine the @c element_type to expose in the @c pointer_traits
///
/// Otherwise, use the first template parameter if @c PointerLike is a class template and @c element_type is not defined
template <template <class, class...> class PointerLike, class T, class... Args>
struct pointer_traits_element_type<
    PointerLike<T, Args...>,
    arene::base::constraints<enable_if_t<!has_element_type_v<PointerLike<T, Args...>>>>> {
  /// @brief The resulting type
  using type = T;
};

/// @brief Require that @c PointerTraitsElementType has a type member named @c type
template <class PointerTraitsElementType>
using require_type = typename PointerTraitsElementType::type;

/// @brief @c true if an @c element_type was determined for @c PointerLike
///
/// If no @c element_type is determined, then the resulting @c pointer_traits will define no members
template <class PointerLike>
constexpr extern bool is_element_type_defined_v =
    arene::base::substitution_succeeds<require_type, pointer_traits_element_type<PointerLike>>;

/// @brief Require that @c PointerLike has a type member named @c difference_type
template <class PointerLike>
using require_difference_type = typename PointerLike::difference_type;

/// @brief Determine the @c difference_type to expose in the @c pointer_traits
///
/// Default to using @c std::ptrdiff_t
template <class PointerLike, class = arene::base::constraints<>>
struct pointer_traits_difference_type {
  /// @brief The resulting type
  using type = ptrdiff_t;
};

/// @brief Determine the @c difference_type to expose in the @c pointer_traits
///
/// Use @c PointerLike::difference_type if defined
template <class PointerLike>
struct pointer_traits_difference_type<
    PointerLike,
    arene::base::constraints<enable_if_t<arene::base::substitution_succeeds<require_difference_type, PointerLike>>>> {
  /// @brief The resulting type
  using type = typename PointerLike::difference_type;
};

/// @brief Require that @c PointerLike has an alias template member named @c rebind
template <class PointerLike, class U>
using require_rebind = typename PointerLike::template rebind<U>;

/// @brief @c true if @c PointerLike has an alias template member named @c rebind
template <class PointerLike, class U>
constexpr extern bool has_rebind_v = arene::base::substitution_succeeds<require_rebind, PointerLike, U>;

/// @brief Determine the @c rebind  to expose in the @c pointer_traits
template <class T, class U, class = arene::base::constraints<>>
struct pointer_traits_rebind {};

/// @brief Determine the @c rebind  to expose in the @c pointer_traits
///
/// Use <c> template<class U>PointerLike::rebind<U> </c> if defined
template <class T, class U>
struct pointer_traits_rebind<T, U, arene::base::constraints<enable_if_t<has_rebind_v<T, U>>>> {
  /// @brief The resulting type
  using type = typename T::template rebind<U>;
};

/// @brief Determine the @c rebind  to expose in the @c pointer_traits
///
/// Otherwise, use <c> template<class U>PointerLike<U, Args...> </c> if @c PointerLike is a class template
template <template <class, class...> class PointerLike, class T, class U, class... Args>
struct pointer_traits_rebind<
    PointerLike<T, Args...>,
    U,
    arene::base::constraints<enable_if_t<!has_rebind_v<PointerLike<T, Args...>, U>>>> {
  /// @brief The resulting type
  using type = PointerLike<U, Args...>;
};

/// @brief An unspecified type to use as the input type for @c pointer_to
class unspecified {
 public:
  /// @brief Deleted constructor to prevent any accidental use
  explicit unspecified() = delete;
};

/// @brief Determine the input argument type of @c pointer_traits<PointerLike>::pointer_to
///
/// If @c PointerLike::element_type is @c void, then the argument type is unspecified
/// Otherwise, the argument type is @c element_type&
template <class ElementType>
using pointer_to_arg_t = conditional_t<is_void_v<ElementType>, unspecified, ElementType>&;

/// @brief Define @c pointer_traits as empty when @c is_element_type_defined_v<PointerLike> is false
template <class PointerLike, class = arene::base::constraints<>>
class pointer_traits_impl {};

/// @brief Define @c pointer_traits for the base case when @c is_element_type_defined_v is true
template <class PointerLike>
class pointer_traits_impl<PointerLike, arene::base::constraints<enable_if_t<is_element_type_defined_v<PointerLike>>>> {
 public:
  /// @brief The type of the @c PointerLike
  using pointer = PointerLike;

  /// @brief The type of the element that is pointed to
  using element_type = typename pointer_traits_element_type<PointerLike>::type;

  /// @brief The type used to represent the difference between two @c PointerLike instances
  using difference_type = typename pointer_traits_difference_type<PointerLike>::type;

  /// @brief The @c pointer type rebound to hold an element of type @c U
  /// @tparam The new type of the element to bind
  template <class U>
  using rebind = typename pointer_traits_rebind<PointerLike, U>::type;

  /// @brief Return a @c pointer to the given element
  /// @param element The element to get a pointer to
  /// @return A @c pointer to the input element
  static constexpr auto pointer_to(pointer_to_arg_t<element_type> element
  ) noexcept(noexcept(PointerLike::pointer_to(element))) -> pointer {
    return PointerLike::pointer_to(element);
  }
};
}  // namespace pointer_traits_detail

/// @brief provides a uniform interface to pointer-like types
/// @tparam T pointer type to retrieve properties for
///
/// @c pointer_traits provides a uniform interface to retrieve properties of pointers and other types that behave like
/// pointers. This template can be specialized for user-defined pointer-like types so that the information about the
/// pointer can be retrieved even if type does not provide the usual typedefs.
///
/// @note this implementation includes the C++23 implementation change to make the traits SFINAE-friendly for the case
/// where @c element_type is not defined.
/// @see https://cplusplus.github.io/LWG/issue3545
template <class T>
class pointer_traits : public pointer_traits_detail::pointer_traits_impl<T> {};

/// @brief @c pointer_traits specialization to handle raw pointer types.
template <class T>
class pointer_traits<T*> {
 public:
  /// @brief The type of the pointer.
  using pointer = T*;

  /// @brief The type of the element that is pointed to
  using element_type = T;

  /// @brief The type used to represent the difference between two pointers
  using difference_type = ptrdiff_t;

  /// @brief A pointer to @c U
  /// @tparam U The new element type
  template <class U>
  using rebind = U*;

  // parasoft-begin-suppress AUTOSAR-A5_0_3-a "False positive: there is only 1 level of pointer indirection"
  /// @brief Return a @c pointer to the given element
  /// @param element The element to get a pointer to
  /// @return A @c pointer to the input element
  static constexpr auto pointer_to(pointer_traits_detail::pointer_to_arg_t<element_type> element) noexcept -> pointer {
    return addressof(element);
  }
  // parasoft-end-suppress AUTOSAR-A5_0_3-a
};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_POINTER_TRAITS_HPP_
