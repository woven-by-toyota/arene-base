// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_CONSTANT_ACCESSOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_CONSTANT_ACCESSOR_HPP_

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/is_abstract.hpp"
#include "arene/base/stdlib_choice/is_object.hpp"

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"

namespace arene {
namespace base {
/// @brief Accessor policy for @c mdspan which always returns the same value
/// @tparam ElementType the type of the element
template <typename ElementType>
class constant_accessor {
 public:
  static_assert(!std::is_abstract<ElementType>::value, "ElementType must not be abstract");
  static_assert(std::is_object<ElementType>::value, "ElementType must be an object type");

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: type aliases do not hide identifiers from class
  // 'constant_accessor'"
  /// @brief The type of the element
  using element_type = ElementType;
  /// @brief The type of the data handle --- this is the same as the element type, so no backing data is required, the
  /// fixed value is passed in as the handle
  using data_handle_type = ElementType;
  /// @brief The type of a "reference" to the element --- this is the same as the element type, so no backing data is
  /// required, the value is returned by value
  using reference = ElementType;
  /// @brief The type of the accessor policy for accessing data with an offset --- this is @c constant_accessor , since
  /// the value is constant, so the offset doesn't matter
  using offset_policy = constant_accessor;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'access' does not hide an identifier from class
  // 'constant_accessor'"
  /// @brief Return the value for the given data handle and index. The index is ignored as the value is always constant
  /// @param data_handle The fixed value to return
  /// @return The fixed value supplied as @c data_handle
  static constexpr auto access(data_handle_type data_handle, std::size_t) noexcept -> reference { return data_handle; }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'offset' does not hide an identifier from class
  // 'constant_accessor'"
  /// @brief Return the data handle value to use with the offset policy for the given data handle and index. The index
  /// is ignored as the value is always constant
  /// @param data_handle The fixed value to return
  /// @return The fixed value supplied as @c data_handle
  static constexpr auto offset(data_handle_type data_handle, std::size_t) noexcept -> reference { return data_handle; }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e
};
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_CONSTANT_ACCESSOR_HPP_
