// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTRUSIVE_BINARY_TREE_NODE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTRUSIVE_BINARY_TREE_NODE_HPP_

#include "arene/base/intrusive/detail/default_tag.hpp"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace intrusive {

// Forward declare
// IWYU pragma: begin_keep
template <typename T, typename Tag, typename Comp>
class priority_queue;
// IWYU pragma: end_keep

namespace detail {
/// @brief Pass-key type for controlling access to specific binary tree members
class binary_tree_node_pass_key {
  /// @brief Private constructor, so only friends can construct instances
  constexpr explicit binary_tree_node_pass_key() = default;

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Passkey idiom permitted by by A11-3-1 Permit #1 v1.0.0"
  template <typename T, typename Tag, typename Comp>
  friend class intrusive::priority_queue;
  // parasoft-end-suppress AUTOSAR-A11_3_1-a
};
}  // namespace detail

/// @brief The tree node of the intrusive container.
/// User type can be derived from this class to support intrusive containers.
/// The @c parent_ points to itself for the root of a tree an indicator.
/// @tparam Tag The tag of the element. Used to support adding the same
/// element to multiple intrusive containers.
template <typename Tag = detail::default_tag>
class binary_tree_node {
  /// @brief parent node
  binary_tree_node* parent_{nullptr};
  // parasoft-begin-suppress AUTOSAR-M0_1_3-c	"False positive: member 'left_' is used"
  /// @brief left child node
  binary_tree_node* left_{nullptr};
  // parasoft-end-suppress AUTOSAR-M0_1_3-c
  // parasoft-begin-suppress AUTOSAR-M0_1_3-c	"False positive: member 'right_' is used"
  /// @brief right child node
  binary_tree_node* right_{nullptr};
  // parasoft-end-suppress AUTOSAR-M0_1_3-c

 public:
  /// @brief Check if the element is linked to a container.
  /// @return @c true if the element is linked to a container, @c false
  constexpr auto is_linked() const noexcept -> bool { return parent_ != nullptr; }

  // parasoft-begin-suppress AUTOSAR-A7_1_1-a	"Declaring 'element' as reference to const changes semantics"
  // parasoft-begin-suppress AUTOSAR-M7_1_2-c "Declaring 'element' as reference to const changes semantics"
  // parasoft-begin-suppress AUTOSAR-A8_4_9-a	"Declaring 'element' as reference to const changes semantics"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Hidden friends permitted by A11-3-1 Permit #2 v1.0.0"
  // parasoft-begin-suppress AUTOSAR-A0_1_3-a "False positive: Function is namespace scope and used in other
  // translation units"
  /// @brief Getter to the parent_ member. pass_key idiom is used to grant
  /// access only to the intrusive priority queue.
  /// @param element The element to get the parent_ member from
  /// @return Reference to the parent_ member
  friend constexpr auto get_parent_link(binary_tree_node& element, Tag, detail::binary_tree_node_pass_key) noexcept
      -> binary_tree_node*& {
    return element.binary_tree_node<Tag>::parent_;
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a
  // parasoft-end-suppress AUTOSAR-A11_3_1-a
  // parasoft-end-suppress AUTOSAR-A8_4_9-a
  // parasoft-end-suppress AUTOSAR-M7_1_2-c
  // parasoft-end-suppress AUTOSAR-A7_1_1-a

  // parasoft-begin-suppress AUTOSAR-A7_1_1-a	"Declaring 'element' as reference to const changes semantics"
  // parasoft-begin-suppress AUTOSAR-M7_1_2-c "Declaring 'element' as reference to const changes semantics"
  // parasoft-begin-suppress AUTOSAR-A8_4_9-a	"Declaring 'element' as reference to const changes semantics"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Hidden friends permitted by A11-3-1 Permit #2 v1.0.0"
  // parasoft-begin-suppress AUTOSAR-A0_1_3-a "False positive: Function is namespace scope and used in other
  // translation units"
  /// @brief Getter to the left_ member. pass_key idiom is used to grant access
  /// only to the intrusive priority queue.
  /// @param element The element to get the left_ member from
  /// @return Reference to the left_ member
  friend constexpr auto get_left_link(binary_tree_node& element, Tag, detail::binary_tree_node_pass_key) noexcept
      -> binary_tree_node*& {
    return element.binary_tree_node<Tag>::left_;
  }
  // parasoft-end-suppress AUTOSAR-A0_1_3-a
  // parasoft-end-suppress AUTOSAR-A11_3_1-a
  // parasoft-end-suppress AUTOSAR-A8_4_9-a
  // parasoft-end-suppress AUTOSAR-M7_1_2-c
  // parasoft-end-suppress AUTOSAR-A7_1_1-a

  // parasoft-begin-suppress AUTOSAR-A7_1_1-a	"Declaring 'element' as reference to const changes semantics"
  // parasoft-begin-suppress AUTOSAR-M7_1_2-c "Declaring 'element' as reference to const changes semantics"
  // parasoft-begin-suppress AUTOSAR-A8_4_9-a	"Declaring 'element' as reference to const changes semantics"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Hidden friends permitted by A11-3-1 Permit #2 v1.0.0"
  // parasoft-begin-suppress AUTOSAR-A0_1_3-a "False positive: Function is namespace scope and used in other
  // translation units"
  /// @brief Getter to the right_ member. pass_key idiom is used to grant access
  /// only to the intrusive priority queue.
  /// @param element The element to get the right_ member from
  /// @return Reference to the right_ member
  friend constexpr auto get_right_link(binary_tree_node& element, Tag, detail::binary_tree_node_pass_key) noexcept
      -> binary_tree_node*& {
    return element.binary_tree_node<Tag>::right_;
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

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTRUSIVE_BINARY_TREE_NODE_HPP_
