// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTRUSIVE_SINGLY_LINKED_NODE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTRUSIVE_SINGLY_LINKED_NODE_HPP_

#include "arene/base/intrusive/detail/default_tag.hpp"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace intrusive {

// Forward declare, so it can be referenced in the friend declaration
// IWYU pragma: begin_keep
template <typename T, typename Tag>
class queue;
// IWYU pragma: end_keep

namespace detail {
/// @brief Pass-key type for accessing singly-linked nodes
class singly_linked_node_pass_key {
  /// @brief Private constructor, so only friends can construct instances
  constexpr explicit singly_linked_node_pass_key() = default;

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Passkey idiom permitted by by A11-3-1 Permit #1 v1.0.0"
  /// @brief Friend declaration to allow @c intrusive_queue to construct instances.
  /// @tparam T the type of element in the queue
  /// @tparam Tag The tag of the element. Used to support adding the same
  /// element to multiple intrusive containers.
  template <typename T, typename Tag>
  friend class intrusive::queue;
  // parasoft-end-suppress AUTOSAR-A11_3_1-a
};
}  // namespace detail

/// @brief Singly linked hook. User type can be derived from this
/// class to support intrusive containers.
/// @tparam T The type of the element.
/// @tparam Tag The tag of the element. Used to support adding the same
/// element to multiple intrusive containers.
template <class Tag = detail::default_tag>
class singly_linked_node {
  /// @brief The next node in the list
  singly_linked_node* next_{nullptr};

 public:
  // parasoft-begin-suppress AUTOSAR-A7_1_7-a "False positive: no expression statement and identifier declaration on
  // same line"
  /// @brief Check if the element is linked to a container.
  /// @return @c true if the element is linked to a container, @c false
  constexpr auto is_linked() const noexcept -> bool { return next_ != nullptr; }
  // parasoft-end-suppress AUTOSAR-A7_1_7-a

  // parasoft-begin-suppress AUTOSAR-A7_1_1-a	"Declaring 'element' as reference to const changes semantics"
  // parasoft-begin-suppress AUTOSAR-M7_1_2-c "Declaring 'element' as reference to const changes semantics"
  // parasoft-begin-suppress AUTOSAR-A8_4_9-a	"Declaring 'element' as reference to const changes semantics"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Hidden friends permitted by A11-3-1 Permit #2 v1.0.0"
  // parasoft-begin-suppress AUTOSAR-A0_1_3-a "False positive: Function is namespace scope and used in other
  // translation units"
  /// @brief Getter to the next_ member. singly_linked_node_pass_key idiom is
  /// used to grant access only to the intrusive queue.
  /// @param element The element to get the next_ member from
  /// @return Reference to the next_ member
  friend constexpr auto get_next_link(singly_linked_node& element, Tag, detail::singly_linked_node_pass_key) noexcept
      -> singly_linked_node*& {
    return element.singly_linked_node<Tag>::next_;
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a
  // parasoft-end-suppress AUTOSAR-A11_3_1-a
  // parasoft-end-suppress AUTOSAR-A8_4_9-a
  // parasoft-end-suppress AUTOSAR-M7_1_2-c
  // parasoft-end-suppress AUTOSAR-A7_1_1-a
};
}  // namespace intrusive
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTRUSIVE_SINGLY_LINKED_NODE_HPP_
