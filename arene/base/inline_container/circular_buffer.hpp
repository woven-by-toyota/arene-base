// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_CIRCULAR_BUFFER_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_CIRCULAR_BUFFER_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/inline_container/deque.hpp"
#include "arene/base/inline_container/detail/try_construct_interface.hpp"
#include "arene/base/iterator/distance.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {

/// @brief A container with fixed capacity featuring constant time insertion and
/// removal at opposite ends; i.e., with an interface similar to std::queue. The
/// storage for the elements is held directly within the class. Storing more
/// than @c Capacity elements results in overwriting the oldest element.
/// @tparam T The type of each element
/// @tparam Capacity The maximum number of elements that can be stored
// parasoft-begin-suppress AUTOSAR-A10_1_1-a "False positive: try_construct_interface is an interface"
template <typename T, std::size_t Capacity>
class circular_buffer
    : private inline_deque_detail::inline_deque_impl<T, Capacity, true>
    , public inline_container_detail::try_construct_interface<
          circular_buffer<T, Capacity>,
          typename inline_deque_detail::inline_deque_try_construct_policy<T, Capacity>> {
  /// @brief The implementation class, separate to share implementations with other similar queues
  using impl = inline_deque_detail::inline_deque_impl<T, Capacity, true>;

  /// @brief Shorthand to the mixin type that synthesizes @c try_construct functions based on the normal constructors
  using try_construct_interface = inline_container_detail::try_construct_interface<
      circular_buffer,
      typename inline_deque_detail::inline_deque_try_construct_policy<T, Capacity>>;

 public:
  /// @copydoc impl::const_iterator
  using typename impl::const_iterator;
  /// @copydoc impl::const_reference
  using typename impl::const_reference;
  /// @copydoc impl::iterator
  using typename impl::iterator;
  /// @copydoc impl::reference
  using typename impl::reference;
  /// @copydoc impl::size_type
  using typename impl::size_type;
  /// @copydoc impl::value_type
  using typename impl::value_type;

  /// @copydoc impl::wrapping_allowed
  using impl::wrapping_allowed;

  // parasoft-begin-suppress AUTOSAR-A12_7_1-a "False positive: this is not the default implementation"
  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: all base class constructors are explicitly called"
  /// @brief Construct an empty queue
  constexpr circular_buffer() noexcept
      : impl(),
        try_construct_interface(this) {}
  // parasoft-end-suppress AUTOSAR-A12_7_1-a
  // parasoft-end-suppress AUTOSAR-A12_1_1-a

  /// @brief Construct a queue from a range of elements, taken to be in front -> back order
  /// @tparam Iterator The type of the iterators to construct from
  /// @param in_begin An iterator to the beginning (front) of the input range
  /// @param in_end An iterator to the end (back+1) of the input range
  /// @pre <c>distance(in_begin, in_end) >= 0</c> and <c><= capacity()</c>, otherwise @c ARENE_PRECONDITION violation
  /// @pre If the behaviour of <c>distance(in_begin, in_end)</c> is undefined, then this function's behaviour is too
  template <
      typename Iterator,
      constraints<
          std::enable_if_t<arene::base::is_forward_iterator_v<Iterator>>,
          std::enable_if_t<
              std::is_constructible<value_type, typename std::iterator_traits<Iterator>::reference>::value>> = nullptr>
  constexpr circular_buffer(Iterator in_begin, Iterator in_end) noexcept(
      noexcept(::arene::base::distance(in_begin, in_end)) &&
      std::is_nothrow_constructible<value_type, typename std::iterator_traits<Iterator>::reference>::value
  )
      : impl(in_begin, in_end),
        try_construct_interface(this) {}

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: this is a delegating constructor"
  /// @brief Construct a queue by copying from an initializer list of elements, taken to be in front -> back order
  /// @param init An initializer list of the value type
  /// @pre <c>init.size() <= capacity()</c>, otherwise @c ARENE_PRECONDITION violation
  template <typename U = value_type, constraints<std::enable_if_t<std::is_copy_constructible<U>::value>> = nullptr>
  constexpr circular_buffer(std::initializer_list<value_type> init
  ) noexcept(noexcept(circular_buffer(init.begin(), init.end())))
      : circular_buffer(init.begin(), init.end()) {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a

  /// @copydoc impl::capacity
  using impl::capacity;
  /// @copydoc impl::empty
  using impl::empty;
  /// @copydoc impl::size
  using impl::size;

  /// @copydoc impl::begin
  using impl::begin;
  /// @copydoc impl::cbegin
  using impl::cbegin;
  /// @copydoc impl::cend
  using impl::cend;
  /// @copydoc impl::crbegin
  using impl::crbegin;
  /// @copydoc impl::crend
  using impl::crend;
  /// @copydoc impl::end
  using impl::end;
  /// @copydoc impl::rbegin
  using impl::rbegin;
  /// @copydoc impl::rend
  using impl::rend;

  /// @copydoc impl::back
  using impl::back;
  /// @copydoc impl::front
  using impl::front;
  /// @copydoc impl::operator[]
  using impl::operator[];

  /// @copydoc impl::emplace_back
  using impl::emplace_back;
  /// @copydoc impl::pop_front
  using impl::pop_front;
  /// @copydoc impl::push_back
  using impl::push_back;
};
// parasoft-end-suppress AUTOSAR-A10_1_1-a

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M2_10_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_CIRCULAR_BUFFER_HPP_
