// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTRUSIVE_PRIORITY_QUEUE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTRUSIVE_PRIORITY_QUEUE_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

#include "arene/base/compiler_support/expect.hpp"  // for check
#include "arene/base/contracts.hpp"
#include "arene/base/intrusive/binary_tree_node.hpp"
#include "arene/base/intrusive/detail/default_tag.hpp"
#include "arene/base/math.hpp"
#include "arene/base/stdlib_choice/climits.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/exchange.hpp"
#include "arene/base/stdlib_choice/is_base_of.hpp"
#include "arene/base/stdlib_choice/is_const.hpp"
#include "arene/base/stdlib_choice/less.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/type_traits.hpp"
#include "arene/base/utility/swap.hpp"

// parasoft-end-suppress AUTOSAR-A16_2_2-a

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace intrusive {

/// @brief The intrusive priority_queue. By default, the @c priority_queue is
/// building a max heap where the element at the top is always the largest.
/// To build a min heap, pass a @c std::greater<> as the comparator.
/// @tparam T The type of the element.
/// @tparam Compare The type of the comparator. Default to @c std::less<>
/// @tparam Tag The tag of the priority queue to work on. Default to @c
/// default_tag.
template <typename T, typename Compare = std::less<>, typename Tag = detail::default_tag>
class priority_queue {
  /// @brief tree node type
  using tree_node = binary_tree_node<Tag>;

  static_assert(std::is_base_of<tree_node, T>::value, "Type T must be derived from binary_tree_node<Tag>");
  static_assert(is_invocable_v<Compare, T const&, T const&>, "Compare must be invocable with two const T&");
  static_assert(!std::is_const<T>::value, "T must not be const");

 public:
  /// @brief value comparison type
  using value_compare = Compare;
  /// @brief value type
  using value_type = T;
  /// @brief size type
  using size_type = std::size_t;
  /// @brief reference type
  using reference = T&;
  /// @brief const reference type
  using const_reference = T const&;
  /// @brief tag type
  using tag = Tag;

  /// @brief Construct an empty priority_queue
  constexpr explicit priority_queue() noexcept = default;

  // parasoft-begin-suppress AUTOSAR-A8_4_5-a "values are logically moved without use of std::move"
  // parasoft-begin-suppress AUTOSAR-A8_4_6-a "False positive: no forwarding reference"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a "False positive: in move constructor, copy initialization for data
  // members of scalar types does not violate this rule"
  /// @brief Move construct a @c priority_queue from another @c priority_queue.
  /// The other @c priority_queue will be empty after the move.
  /// @param other The other @c priority_queue
  constexpr priority_queue(priority_queue&& other) noexcept
      : size_(std::exchange(other.size_, size_type{})),
        root_(std::exchange(other.root_, nullptr)) {}
  // parasoft-end-suppress AUTOSAR-A12_8_4-a
  // parasoft-end-suppress AUTOSAR-A8_4_6-a
  // parasoft-end-suppress AUTOSAR-A8_4_5-a

  /// @brief Move assign a @c priority_queue from another @c priority_queue.
  ///        The other @c priority_queue will be empty after the move.
  /// @param other The other priority_queue
  constexpr auto operator=(priority_queue&& other) noexcept -> priority_queue& {
    if (this == &other) {
      return *this;
    }
    priority_queue temp{std::move(other)};
    swap(temp);
    return *this;
  }

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Deleted copy constructor
  ///
  /// Intrusive priority queue does not support copy
  ///
  priority_queue(priority_queue const&) = delete;
  // parasoft-end-suppress CERT_C-EXP37-a-3

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Deleted copy constructor
  ///
  /// Intrusive priority queue does not support copy
  ///
  auto operator=(priority_queue const&) -> priority_queue& = delete;
  // parasoft-end-suppress CERT_C-EXP37-a-3

  /// @brief Destructor
  ~priority_queue() = default;

  /// @brief Return a reference to the top element of the @c priority_queue.
  /// @pre The queue must not be empty.
  /// @return Reference to the top element.
  constexpr auto top() const noexcept -> T& {
    ARENE_PRECONDITION(!empty());
    return static_cast<T&>(*root_);
  }

  /// @brief Return the number of elements in the @c priority_queue.
  /// @return The number of elements in the @c priority_queue.
  constexpr auto size() const noexcept -> size_type { return size_; }

  /// @brief Check whether the @c priority_queue is empty.
  /// @return True if the @c priority_queue is empty, false otherwise.
  constexpr auto empty() const noexcept -> bool { return size() == size_type{}; }

  /// @brief Push an element to the @c priority_queue.
  /// @pre The element must not already be in a @c priority_queue.
  /// @param value The element to be pushed.
  constexpr void push(reference value) noexcept {
    ARENE_PRECONDITION(!value.tree_node::is_linked());

    if (empty()) {
      root_ = static_cast<tree_node*>(&value);
      parent(*root_) = root_;  // parent_ points to itself for the root
      size_ = 1U;
      return;
    }

    // Push the node to the last position
    // Find node to insert the new node, (size_ + 1) / 2
    auto& parent_of_new_node = find_node_by_index((size_ + 1U) >> 1U);
    if (!is_even(size_)) {
      // Left
      left(parent_of_new_node) = static_cast<tree_node*>(&value);
    } else {
      // Right
      right(parent_of_new_node) = static_cast<tree_node*>(&value);
    }
    parent(value) = &parent_of_new_node;
    ++size_;

    // Now the node has been added to the last position,
    // reheapify the priority_queue.
    re_heapify(value);
  }

  /// @brief Pop the top element from the @c priority_queue.
  /// @pre The @c priority_queue must not be empty.
  constexpr void pop() noexcept {
    ARENE_PRECONDITION(!empty());
    erase_impl(*root_);
  }

  /// @brief Erase a specific node from the @c priority_queue.
  /// @pre The node must be in the @c priority_queue.
  /// @pre The @c priority_queue must not be empty.
  /// @param node The node to be erased.
  constexpr void erase(reference node) noexcept {
    ARENE_PRECONDITION(!empty());
    ARENE_PRECONDITION(contains(node));
    erase_impl(node);
  }

  /// @brief Swap the contents of two priority queues.
  /// @param other The queue to swap with.
  constexpr void swap(priority_queue& other) noexcept {
    using ::arene::base::swap;  // constexpr eligible swap
    swap(size_, other.size_);
    swap(root_, other.root_);
  }

  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
  /// @brief deleted equality operator
  ///
  /// Intrusive priority queue does not support equality comparison as the
  /// definition of equality is not clear for intrusive containers.
  ///
  constexpr auto operator==(priority_queue const&) const -> bool = delete;
  // parasoft-end-suppress CERT_C-EXP37-a

  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
  /// @brief deleted equality operator
  ///
  /// Intrusive priority queue does not support equality comparison as the
  /// definition of equality is not clear for intrusive containers.
  ///
  constexpr auto operator!=(priority_queue const&) const -> bool = delete;
  // parasoft-end-suppress CERT_C-EXP37-a

 private:
  /// @brief determine if an unsigned integer value is even
  /// @param value value to check
  /// @return @c true is value is even, otherwise @c false
  static constexpr auto is_even(std::size_t value) noexcept -> bool { return (value & std::size_t{1U}) == 0U; }

  /// @brief erase a node from the priority queue
  /// @param node node to erase
  /// @post nodes form a valid max/min heap
  constexpr void erase_impl(tree_node& node) noexcept {
    // If the queue only has one node, simply remove the root
    if (size_ == 1U) {
      parent(*root_) = nullptr;
      root_ = nullptr;
      --size_;
      return;
    }

    auto& last_node = find_node_by_index(size_);
    bool reheapify_needed{false};

    if (static_cast<tree_node*>(&node) != &last_node) {
      // Swap the node to be removed with the last node
      swap_nodes(node, last_node);  // last node become the node to be removed
      reheapify_needed = true;
    }

    // Erase the last node
    if (left(*parent(node)) == static_cast<tree_node*>(&node)) {
      left(*parent(node)) = nullptr;
    } else {
      right(*parent(node)) = nullptr;
    }
    parent(node) = nullptr;
    --size_;

    if (reheapify_needed) {
      re_heapify(last_node);
    }
  }

  /// @brief Compare two nodes
  /// @param node1 node to compare
  /// @param node2 node to compare
  /// @return @c comp_ of the value type of both nodes
  ///
  /// Compares two nodes using the stored comparison function object.
  ///
  constexpr auto compare_value(tree_node const& node1, tree_node const& node2) const noexcept -> bool {
    return comp_(static_cast<T const&>(node1), static_cast<T const&>(node2));
  }

  /// @brief Move the node up until it reaches the correct position
  /// @param node node to move
  constexpr void move_node_up(tree_node& node) noexcept {
    while ((&node != root_) &&  // Not root
           compare_value(*parent(node), node)) {
      if (left(*parent(node)) == &node) {
        swap_with_left(*parent(node));
      } else {
        swap_with_right(*parent(node));
      }
    }
  }

  /// @brief Move the node down until it reaches the correct position
  /// @param node node to move
  constexpr void move_node_down(tree_node& node) noexcept {
    while ((left(node) != nullptr) || (right(node) != nullptr)) {
      bool should_swap_with_left{(left(node) != nullptr) && compare_value(node, *left(node))};
      bool should_swap_with_right{(right(node) != nullptr) && compare_value(node, *right(node))};

      if (should_swap_with_left && should_swap_with_right) {
        should_swap_with_left = compare_value(*right(node), *left(node));
      }

      if (should_swap_with_left) {
        swap_with_left(node);
      } else if (should_swap_with_right) {
        swap_with_right(node);
      } else {
        break;
      }
    }
  }

  /// @brief Re-heapify from a given node to form a valid max/min heap
  /// by moving the node up and down till it reaches the correct position.
  /// @param node The node to re-heapify.
  constexpr void re_heapify(tree_node& node) noexcept {
    move_node_up(node);
    move_node_down(node);
  }

  /// @brief Find a node in the @c priority_queue based on its index.
  /// @param n The index of the node to find. Index starts from 1, i.e.,
  /// the root node has index 1. find_node_by_index(size_) returns the last
  /// node.
  /// @return Pointer to the found node.
  constexpr auto find_node_by_index(std::size_t n) const noexcept -> tree_node& {
    std::size_t const layers_num{log2(n)};  // Get the number of layers
    auto* ptr = root_;
    for (std::size_t i{1U}; i < layers_num + 1U; ++i) {
      // parasoft-begin-suppress AUTOSAR-M5_8_1-a "The return value of 'log2' stored in
      // 'layers_num' never exceeds 'sizeof(std::size_t)*CHAR_BIT - 1'.
      // Loop counter 'i' is subtracted from that value.
      // By construction of the loop, 'i' can never be larger than 'layers_num'.
      // As a result, 'layers_num - i' is always bounded to the
      // interval [0, sizeof(std::size_t)*CHAR_BIT - 1]"
      ARENE_INVARIANT((layers_num - i) <= (sizeof(std::size_t) * std::size_t{CHAR_BIT} - 1U));

      // Traverse down the tree, select the left or right
      // child based on the bit of the index
      if (is_even(n >> (layers_num - i))) {
        ptr = left(*ptr);
      } else {
        ptr = right(*ptr);
      }
      // parasoft-end-suppress AUTOSAR-M5_8_1-a
    }
    ARENE_INVARIANT(ptr != nullptr);
    return *ptr;
  }

  /// @brief Swap the given node with its left child.
  /// @param node The node to swap.
  constexpr void swap_with_left(tree_node& node) noexcept { swap_nodes(node, *left(node)); }

  /// @brief Swap the given node with its right child.
  /// @param node The node to swap.
  constexpr void swap_with_right(tree_node& node) noexcept { swap_nodes(node, *right(node)); }

  /// @brief Check if the @c priority_queue contains a specific node.
  /// @param node The node to check.
  /// @return True if the @c priority_queue contains the node, false otherwise
  /// or if the node is @c nullptr.
  constexpr auto contains(tree_node& node) const noexcept -> bool {
    auto* traversed_root = &node;
    while ((parent(*traversed_root) != nullptr) && (parent(*traversed_root) != traversed_root)) {
      traversed_root = parent(*traversed_root);
    }

    return traversed_root == root_;
  }

  /// @brief Swap two nodes in the @c priority_queue. Given the current
  /// algorithm, only the first node could possibly be root.
  /// @param node_1 The first node to swap.
  /// @param node_2 The second node to swap.
  constexpr void swap_nodes(tree_node& node_1, tree_node& node_2) noexcept {
    if ((root_ == &node_1) || (root_ == &node_2)) {
      parent(*root_) = nullptr;  // Temporarily change to simplify the algorithm
    }

    if (parent(node_1) != parent(node_2)) {
      ::arene::base::swap(parent(node_1), parent(node_2));
      // Update node_1's old parent to have node_2 as a child instead
      // Justification: Clang-tidy complains update_child_of_parent seems to swap the
      // two arguments, but it is not the case.
      // NOLINTNEXTLINE(readability-suspicious-call-argument)
      update_child_of_parent(node_2, node_1);
      // Update node_2's old parent to have node_1 as a child instead
      // NOLINTNEXTLINE(readability-suspicious-call-argument)
      update_child_of_parent(node_1, node_2);
    } else {
      auto* parent_node_ptr = parent(node_1);
      ARENE_INVARIANT(parent_node_ptr != nullptr);
      auto& common_parent_node = *parent_node_ptr;
      // Swap left/right child pointers of the common parent node
      ::arene::base::swap(left(common_parent_node), right(common_parent_node));
    }

    // Swap child pointers
    ::arene::base::swap(left(node_1), left(node_2));
    ::arene::base::swap(right(node_1), right(node_2));

    // Update children's parent pointers
    update_parent_of_child(node_1);
    update_parent_of_child(node_2);

    // Update root if necessary
    if (root_ == &node_1) {
      root_ = &node_2;
      parent(*root_) = root_;
    }
  }

  /// @brief Update the child pointer of the parent of a @c node_1 to become
  /// @c node_2
  /// @param node_1 The previous child node.
  /// @param node_2 The new child node.
  static constexpr void update_child_of_parent(tree_node& node_1, tree_node& node_2) noexcept {
    if (parent(node_1) != nullptr) {
      if (left(*parent(node_1)) == &node_2) {
        left(*parent(node_1)) = &node_1;
      } else {
        right(*parent(node_1)) = &node_1;
      }
    }
  }

  /// @brief Update the parent pointer of the children of a @c node_1 to
  /// become
  /// @c node
  /// @param node The new parent node.
  static constexpr void update_parent_of_child(tree_node& node) noexcept {
    if (left(node) != nullptr) {
      parent(*left(node)) = &node;
    }
    if (right(node) != nullptr) {
      parent(*right(node)) = &node;
    }
  }

  /// @brief Obtain the parent node
  /// @param node node to obtain the parent of
  /// @return reference to a pointer to the parent node
  static constexpr auto parent(tree_node& node) noexcept -> tree_node*& {
    return get_parent_link(node, Tag{}, detail::binary_tree_node_pass_key{});
  }
  /// @brief Obtain the left child node
  /// @param node node to obtain the left child of
  /// @return reference to a pointer to the left child node
  static constexpr auto left(tree_node& node) noexcept -> tree_node*& {
    return get_left_link(node, Tag{}, detail::binary_tree_node_pass_key{});
  }
  /// @brief Obtain the right child node
  /// @param node node to obtain the right child of
  /// @return reference to a pointer to the right child node
  static constexpr auto right(tree_node& node) noexcept -> tree_node*& {
    return get_right_link(node, Tag{}, detail::binary_tree_node_pass_key{});
  }

  // parasoft-begin-suppress AUTOSAR-A12_1_2-a "False positive: move constructors are exempt from this rule"
  /// @brief node comparison function object
  Compare comp_;
  /// @brief number of node in the priority queue
  std::size_t size_{};
  /// @brief first node in the queue
  tree_node* root_{nullptr};
  // parasoft-end-suppress AUTOSAR-A12_1_2-a
};
}  // namespace intrusive
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTRUSIVE_PRIORITY_QUEUE_HPP_
