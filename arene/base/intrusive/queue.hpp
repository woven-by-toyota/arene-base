// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTRUSIVE_QUEUE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTRUSIVE_QUEUE_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

#include "arene/base/contracts.hpp"
#include "arene/base/intrusive/detail/default_tag.hpp"
#include "arene/base/intrusive/singly_linked_node.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/exchange.hpp"
#include "arene/base/stdlib_choice/is_base_of.hpp"
#include "arene/base/stdlib_choice/is_const.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/utility/swap.hpp"

// parasoft-end-suppress AUTOSAR-A16_2_2-a

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace intrusive {

/// @brief Intrusive queue implementation.
/// @tparam T The type of the elements in the queue.
/// @tparam Tag The tag of the priority queue to work on. Default to @c
/// default_tag.
template <typename T, typename Tag = detail::default_tag>
class queue {
  /// @brief linked node type
  using linked_node = singly_linked_node<Tag>;

  static_assert(std::is_base_of<linked_node, T>::value, "T must be derived from singly_linked_node<Tag>");
  static_assert(!std::is_const<T>::value, "T must not be const");

 public:
  /// @brief value type
  using value_type = T;
  /// @brief size type
  using size_type = std::size_t;
  /// @brief reference type
  using reference = T&;
  /// @brief const reference type
  using const_reference = T const&;

  /// @brief Default constructor.
  explicit constexpr queue() noexcept = default;

  /// @brief Move constructor.
  /// @param other The queue to move from.
  constexpr queue(queue&& other) noexcept
      : size_(std::exchange(other.size_, size_type{})),
        head_(std::exchange(other.head_, nullptr)),
        tail_(std::exchange(other.tail_, nullptr)) {}

  /// @brief Move assignment operator.
  /// @param other The queue to move assign from.
  /// @return Reference to the modified queue.
  constexpr auto operator=(queue&& other) noexcept -> queue& {
    if (this == &other) {
      return *this;
    }

    queue temp{std::move(other)};
    swap(temp);
    return *this;
  }

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Deleted copy constructor
  ///
  /// Intrusive queue does not support copy
  ///
  queue(queue const&) = delete;
  // parasoft-end-suppress CERT_C-EXP37-a-3

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Deleted copy assignment operator
  ///
  /// Intrusive queue does not support copy
  ///
  auto operator=(queue const&) -> queue& = delete;
  // parasoft-end-suppress CERT_C-EXP37-a-3

  /// @brief Trivial destructor.
  /// This is a tradeoff. C++14 doesn't allow user-provided
  /// destructor to be constexpr.
  ~queue() = default;

  /// @brief Get the reference to the front element of the queue.
  /// @pre The queue must not be empty.
  /// @return Reference to the front element.
  constexpr auto front() noexcept -> reference {
    ARENE_PRECONDITION(!empty());
    return static_cast<reference>(*head_);
  }

  /// @brief Get the reference to the back element of the queue.
  /// @pre The queue must not be empty.
  /// @return Reference to the back element.
  constexpr auto back() noexcept -> reference {
    ARENE_PRECONDITION(!empty());
    return static_cast<reference>(*tail_);
  }

  /// @brief Get the size of the queue.
  /// @return The number of elements in the queue.
  constexpr auto size() const noexcept -> size_type { return size_; }

  /// @brief Check if the queue is empty.
  /// @return @c true if the queue is empty, @c false otherwise.
  constexpr auto empty() const noexcept -> bool { return size() == size_type{}; }

  /// @brief Push an element into the queue.
  /// @pre The element must not be in any queue.
  /// @param value The element to push.
  constexpr void push(reference value) noexcept {
    // The value must not be in any queue
    ARENE_PRECONDITION(!value.linked_node::is_linked());
    if (empty()) {
      head_ = static_cast<linked_node*>(&value);
      tail_ = head_;
    } else {
      get_next_link(*tail_, Tag{}, detail::singly_linked_node_pass_key{}) = &value;
      tail_ = static_cast<linked_node*>(&value);
    }
    // The last element is always self-pointing
    // to indicate it's in a queue
    get_next_link(value, Tag{}, detail::singly_linked_node_pass_key{}) = static_cast<linked_node*>(&value);

    ++size_;
  }

  /// @brief Remove the front element from the queue.
  /// @pre The queue must not be empty.
  constexpr void pop() noexcept {
    ARENE_PRECONDITION(!empty());
    if (size() == 1U) {
      get_next_link(*head_, Tag{}, detail::singly_linked_node_pass_key{}) = nullptr;
      head_ = nullptr;
      tail_ = nullptr;
    } else {
      auto prev_head = head_;
      head_ = get_next_link(*head_, Tag{}, detail::singly_linked_node_pass_key{});
      get_next_link(*prev_head, Tag{}, detail::singly_linked_node_pass_key{}) = nullptr;
    }
    --size_;
  }

  /// @brief Swap the contents of two queues.
  /// @param other The queue to swap with.
  constexpr void swap(queue& other) noexcept {
    using ::arene::base::swap;  // constexpr eligible swap
    swap(size_, other.size_);
    swap(head_, other.head_);
    swap(tail_, other.tail_);
  }

  /// @brief deleted equality operator
  ///
  /// Queue does not support equality comparison as the definition
  /// of equality is not clear for intrusive containers.
  constexpr auto operator==(queue const& other) const noexcept -> bool = delete;

  /// @brief deleted equality operator
  ///
  /// Queue does not support inequality comparison as the definition
  /// of equality is not clear for intrusive containers.
  constexpr auto operator!=(queue const& other) const noexcept -> bool = delete;

 private:
  // parasoft-begin-suppress AUTOSAR-A12_1_2-a "False positive: move constructors are exempt from this rule"
  /// @brief number of nodes in the queue
  std::size_t size_{};
  /// @brief head node
  linked_node* head_{nullptr};
  /// @brief tail node
  linked_node* tail_{nullptr};
  // parasoft-end-suppress AUTOSAR-A12_1_2-a
};
}  // namespace intrusive
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTRUSIVE_QUEUE_HPP_
