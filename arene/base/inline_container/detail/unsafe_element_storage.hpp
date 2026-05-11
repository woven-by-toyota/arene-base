// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_UNSAFE_ELEMENT_STORAGE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_UNSAFE_ELEMENT_STORAGE_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/inline_container/.*"

#include "arene/base/stdlib_choice/is_destructible.hpp"

namespace arene {
namespace base {
namespace inline_container {
namespace detail {

// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"

// parasoft-begin-suppress AUTOSAR-A9_5_1-a "False positive: this is covered by the discriminated union exception"
// parasoft-begin-suppress AUTOSAR-A9_5_1-b "False positive: this is covered by the discriminated union exception"
// parasoft-begin-suppress AUTOSAR-M11_0_1-a "False positive: this is a POD union"

/// @brief unsafe storage for an individual element
/// @tparam T element type to store
///
/// Provides storage for an element, allowing deferred construction by use of a
/// @c union. Unlike @c optional, this type does not track the active member
/// within the @c union; the active member must be tracked externally and the
/// destructor of the active member must be explicitly invoked.
///
template <class T, bool = std::is_trivially_destructible<T>::value>
union unsafe_element_storage {
  /// @brief The type of the dummy entry
  struct dummy_type {};
  /// @brief A dummy entry for when there is no element here
  dummy_type dummy;
  /// @brief The object when there is an element here
  T object;

  // parasoft-begin-suppress AUTOSAR-A12_7_1-a-2 "false positive: =default would have different semantics"
  /// @brief Construct without an element
  constexpr unsafe_element_storage() noexcept
      : dummy() {}

  /// @brief Destroy the storage; assumes the element has already been destroyed
  // NOLINTNEXTLINE(hicpp-use-equals-default) =default makes destructor deleted
  ~unsafe_element_storage() {}
  // parasoft-end-suppress AUTOSAR-A12_7_1-a-2

  /// @brief Copying is managed externally from this type; the storage is not
  /// copyable
  unsafe_element_storage(unsafe_element_storage const&) = delete;
  /// @brief Moving is managed externally from this type; the storage is not
  /// movable
  unsafe_element_storage(unsafe_element_storage&&) = delete;
  /// @brief Copying is managed externally from this type; the storage is not
  /// copyable
  auto operator=(unsafe_element_storage const&) -> unsafe_element_storage& = delete;
  /// @brief Moving is managed externally from this type; the storage is not
  /// movable
  auto operator=(unsafe_element_storage&&) -> unsafe_element_storage& = delete;
};

/// @brief unsafe storage for an individual element
/// @tparam T element type to store
///
/// Provides storage for an element, allowing deferred construction by use of a
/// @c union. Unlike @c optional, this type does not track the active member
/// within the @c union; the active member must be tracked externally and the
/// destructor of the active member must be explicitly invoked.
///
/// This specialization is for those cases where the stored type is trivially destructible
template <class T>
union unsafe_element_storage<T, true> {
  /// @brief The type of the dummy entry
  struct dummy_type {};
  /// @brief A dummy entry for when there is no element here
  dummy_type dummy;
  /// @brief The object when there is an element here
  T object;

  // parasoft-begin-suppress AUTOSAR-A12_7_1-a-2 "false positive: =default would have different semantics"
  /// @brief Construct without an element
  constexpr unsafe_element_storage() noexcept
      : dummy() {}

  /// @brief Destroy the object; both alternatives are trivially destructible, so this is too
  ~unsafe_element_storage() = default;

  /// @brief Copying is managed externally from this type; the storage is not
  /// copyable
  unsafe_element_storage(unsafe_element_storage const&) = delete;
  /// @brief Moving is managed externally from this type; the storage is not
  /// movable
  unsafe_element_storage(unsafe_element_storage&&) = delete;
  /// @brief Copying is managed externally from this type; the storage is not
  /// copyable
  auto operator=(unsafe_element_storage const&) -> unsafe_element_storage& = delete;
  /// @brief Moving is managed externally from this type; the storage is not
  /// movable
  auto operator=(unsafe_element_storage&&) -> unsafe_element_storage& = delete;
};

// parasoft-end-suppress AUTOSAR-A9_5_1-a
// parasoft-end-suppress AUTOSAR-A9_5_1-b
// parasoft-end-suppress AUTOSAR-M11_0_1-a

}  // namespace detail
}  // namespace inline_container
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_UNSAFE_ELEMENT_STORAGE_HPP_
