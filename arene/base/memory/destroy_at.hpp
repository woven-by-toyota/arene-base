// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::destroy_at"

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MEMORY_DESTROY_AT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MEMORY_DESTROY_AT_HPP_

// IWYU pragma: private, include "arene/base/memory.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/addressof.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_array.hpp"

namespace arene {
namespace base {

namespace destroy_at_detail {

/// @brief Implementation helper for @c arene::base::destroy_at.
class do_destroy_at {
 public:
  // parasoft-begin-suppress AUTOSAR-M0_1_8-a "False positive: there is an external side effect"
  /// @brief Overload for non array types.
  /// @see arene::base::destroy_at
  /// @tparam T The type
  /// @param ptr A pointer to the value
  template <typename T, constraints<std::enable_if_t<!std::is_array<T>::value>> = nullptr>
  void operator()(T* ptr) const noexcept {
    ptr->~T();
  }
  // parasoft-end-suppress AUTOSAR-M0_1_8-a

  /// @brief Overload for array types.
  /// @see arene::base::destroy_at
  /// @tparam T The array type
  /// @param ptr A pointer to the array
  template <typename T, constraints<std::enable_if_t<std::is_array<T>::value>> = nullptr>
  // parasoft-begin-suppress AUTOSAR-A5_0_3-a-2 "False positive: ptr has only one level of indirection"
  void operator()(T* ptr) const noexcept {
    // parasoft-end-suppress AUTOSAR-A5_0_3-a-2
    // parasoft-begin-suppress AUTOSAR-A18_1_1-a-2 "Iteration through C array provided by caller"
    for (auto& element : *ptr) {
      // parasoft-end-suppress AUTOSAR-A18_1_1-a-2
      (*this)(std::addressof(element));
    }
  }
};

}  // namespace destroy_at_detail

/// @def arene::base::destroy_at
/// @brief Destroys the object(s) located at the given memory address @c std::destroy_at back ported from C++20.
/// @tparam T the type to destroy.
/// @param ptr a pointer to the object(s) to be destroyed.
/// @post If @c T is not an array type, equivalent to having called the destructor on the object pointed to by @c ptr.
///       If @c T is an array type, recursively destroys elements of @c *ptr in order.
/// @note Unlike @c std::destroy_at, constexpr evaluation is not supported as dynamic memory allocation (even using
///       placement new) is not supported until C++20.
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(destroy_at_detail::do_destroy_at, destroy_at);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MEMORY_DESTROY_AT_HPP_
