// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_POINTER_DETAIL_TO_ADDRESS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_POINTER_DETAIL_TO_ADDRESS_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/pointer/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/pointer/detail/type_traits.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_function.hpp"
#include "arene/base/stdlib_choice/pointer_traits.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {
namespace pointer {
namespace detail {
namespace to_address_detail {

///
/// @brief Concept which tests if a type has a @c std::pointer_traits::to_address definition.
///
/// @{
template <typename P>
using use_ptraits_to_address = decltype(std::declval<std::pointer_traits<P>>().to_address(std::declval<P>()));

template <typename P>
constexpr bool has_ptraits_to_address_v = substitution_succeeds<use_ptraits_to_address, P>;
/// @}

/// @brief function-object for implementation of @c arene::base::pointer::detail::to_address()
class to_address {
 public:
  /// @brief implementation helper for @c arene::base::pointer::detail::to_address()
  /// for raw-pointers.
  /// @tparam P The type of the pointee
  /// @param ptr The pointer to return
  /// @return ptr
  template <typename P>
  ARENE_NODISCARD constexpr auto operator()(P* ptr) const noexcept -> P* {
    static_assert(!std::is_function<P>::value, "Cannot call to_address on a function pointer!");
    return ptr;
  }

  /// @brief implementation helper for @c arene::base::pointer::detail::to_address()
  /// for "fancy pointers" with a @c pointer_traits::to_address specialization.
  /// @tparam P The type of the pointer
  /// @param ptr The pointer to return the address of the pointee for
  /// @return The address of the object pointed to by @c ptr
  template <typename P, constraints<std::enable_if_t<has_ptraits_to_address_v<P>>> = nullptr>
  ARENE_NODISCARD constexpr auto operator()(P const& ptr) const -> decltype(auto) {
    return std::pointer_traits<P>::to_address(ptr);
  }

  // parasoft-begin-suppress AUTOSAR-A8_4_12-a "Passing unique_ptr by const-reference matches the standard library
  // specification."
  /// @brief implementation helper for @c arene::base::pointer::detail::to_address()
  /// for "fancy pointers" without a @c pointer_traits::to_address specialization.
  /// @tparam P The type of the pointer
  /// @param ptr The pointer to return the address of the pointee for
  /// @return The address of the object pointed to by @c ptr
  template <typename P, constraints<std::enable_if_t<has_arrow_operator<P> && !has_ptraits_to_address_v<P>>> = nullptr>
  ARENE_NODISCARD constexpr auto operator()(P const& ptr) const -> decltype(auto) {
    return (*this)(ptr.operator->());
  }
  // parasoft-end-suppress AUTOSAR-A8_4_12-a
};

}  // namespace to_address_detail

/// @def arene::base::pointer::detail::to_address
/// @brief A backport of C++20's @c to_address functionality.
///
/// @c to_address provides a uniform way to handle obtaining a raw pointer from a pointer type that may be a raw pointer
/// or may be a "fancy" pointer (@c std::shared_ptr etc), without the need to form a reference to the held element. It
/// does this conversion in the following manner:
/// -# If the pointer is already a raw pointer, it simply returns the input.
/// -# If the pointer is a "fancy pointer":
///   -# If @c std::pointer_traits<P>::to_address is well-formed, it is used
///   -# Otherwise, returns the result of applying @c to_address to @c P::operator->()
///
/// @tparam The type of the pointer to call @c to_address against.
/// @param ptr The pointer to call @c to_address against.
/// @return Raw pointer that represents the same address that @c ptr does.
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(to_address_detail::to_address, to_address);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace detail
}  // namespace pointer
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_POINTER_DETAIL_TO_ADDRESS_HPP_
