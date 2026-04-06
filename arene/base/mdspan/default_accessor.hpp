// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DEFAULT_ACCESSOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DEFAULT_ACCESSOR_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_abstract.hpp"
#include "arene/base/stdlib_choice/is_array.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_object.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

/// @brief accesses a single element from a pointer and an index
/// @tparam ElementType complete object type that is accessed by @c default_accessor
///
/// Backport of @c std::default_accessor from C++23. The @c default_accessor
/// class template is the default AccessorPolicy of @c mdspan if no
/// user-specified accessor policy is provided. It defines the @c
/// data_handle_type as a pointer to the @c ElementType and access will return a
/// reference to the @c ElementType.
///
template <class ElementType>
class default_accessor {
 public:
  static_assert(std::is_object<ElementType>::value, "'ElementType' must be an object");
  static_assert(!std::is_abstract<ElementType>::value, "'ElementType' must not be an abstract class");
  static_assert(!std::is_array<ElementType>::value, "'ElementType' must not be an array");

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: Identifiers do not hide anything"
  /// @brief offset policy type
  using offset_policy = default_accessor;
  /// @brief element type to access
  using element_type = ElementType;
  /// @brief reference type
  /// Returned on element to access. In this case, an lvalue reference.
  using reference = ElementType&;
  /// @brief data handle type
  /// Refers to a group of elements. In this case, a pointer.
  using data_handle_type = ElementType*;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  /// @brief default constructor
  /// @note This constructor has no visible effect
  constexpr default_accessor() noexcept = default;

  /// @brief converting constructor
  /// @tparam OtherElementType element type of the other @c default_accessor
  /// @note This constructor has no visible effect
  template <
      class OtherElementType,
      // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
      constraints<std::enable_if_t<std::is_convertible<OtherElementType (*)[], element_type (*)[]>::value>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr default_accessor(default_accessor<OtherElementType>) noexcept {}

  /// @brief obtain the element at the specified index
  /// @param ptr pointer to an element
  /// @param index offset from the first element
  /// @return reference to the element at the specified index
  constexpr auto access(data_handle_type ptr, std::size_t index) const noexcept -> reference { return ptr[index]; }

  /// @brief apply an offset to a pointer
  /// @param ptr pointer to an element
  /// @param index offset to apply
  /// @return data handle after applying the offset
  constexpr auto offset(data_handle_type ptr, std::size_t index) const noexcept -> data_handle_type {
    return ptr + index;
  }
};

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DEFAULT_ACCESSOR_HPP_
