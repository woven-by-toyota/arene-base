// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_INITIALIZER_LIST_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_INITIALIZER_LIST_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <initializer_list>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/cstddef.hpp"

namespace std {
/// @brief Implementation of @c std::initializer_list for brace initialization
/// @tparam T The type of the elements
/// @note This is an interface for an internal compiler type
template <typename T>
class initializer_list {
 public:
  /// @brief The type of each element
  using value_type = T;
  /// @brief The type of the size
  using size_type = size_t;
  /// @brief The type of the iterator
  using iterator = T const*;
  /// @brief The type of the iterator
  using const_iterator = iterator;
  /// @brief The type of a reference to the stored element
  using reference = T const&;
  /// @brief The type of a reference to the stored element
  using const_reference = reference;

  /// @brief Construct an empty list
  constexpr initializer_list() noexcept  // NOLINT(hicpp-member-init) Fields are initialized by delegated constructor
      : initializer_list(nullptr, 0U) {}

  /// @brief Get a pointer to the first element
  /// @return A pointer to the first element
  constexpr auto begin() const noexcept -> iterator { return element_ptr_; }

  // parasoft-begin-suppress AUTOSAR-M5_0_15-a-2 "The compiler guarantees that element_ptr_ refers to an array"
  /// @brief Get a pointer to one-past-the-last element
  /// @return A pointer to one-past-the-last element
  constexpr auto end() const noexcept -> iterator { return element_ptr_ + element_count_; }
  // parasoft-end-suppress AUTOSAR-M5_0_15-a-2

  /// @brief Get the number of elements in the list
  /// @return The number of elements
  constexpr auto size() const noexcept -> size_type { return element_count_; }

 private:
  /// @brief A pointer to the elements
  iterator element_ptr_;
  /// @brief The number of elements
  size_type element_count_;

  // parasoft-begin-suppress AUTOSAR-A2_10_1-a "False positive: 'count' does not hide anything"

  /// @brief Internal constructor used by the compiler
  /// @param ptr The pointer to the elements
  /// @param count The number of elements
  constexpr initializer_list(iterator ptr, size_type count) noexcept
      : element_ptr_{ptr},
        element_count_{count} {}
};

// parasoft-end-suppress AUTOSAR-A2_10_1-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_INITIALIZER_LIST_HPP_
