// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DEQUE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DEQUE_HPP_

#include "arene/base/algorithm/copy.hpp"
#include "arene/base/algorithm/rotate.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/inline_container/detail/iterator_interface.hpp"
#include "arene/base/inline_container/detail/range_interface.hpp"
#include "arene/base/inline_container/detail/try_construct_interface.hpp"
#include "arene/base/inline_container/detail/unsafe_element_storage.hpp"
#include "arene/base/iterator/distance.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/memory/construct_at.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_copy_assignable.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/is_trivially_copyable.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/stdlib_choice/min_value_overload.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/type_manipulation/non_constructible_dummy.hpp"
#include "arene/base/type_traits/conditional.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "arene/base/utility/safe_comparisons.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {

namespace inline_deque_detail {

/// @brief Performs an optimized modulo operation when @c value is constrained to be in <c>[0, 2*modulus)</c>
/// @tparam T The type of the arguments; must be the same for both arguments to prevent unwanted promotions
/// @param value Value to be used in the modulo operation
/// @param modulus Modulus to be used in the modulo operation
/// @return @c value mod @c modulus (note this is always positive, unlike the @c % remainder operator)
/// @pre <c>0 <= value < 2*modulus</c>, otherwise the result will be incorrect
template <typename T>
constexpr auto constrained_modulo(T value, T modulus) noexcept -> T {
  return value < modulus ? value : value - modulus;
}

// parasoft-begin-suppress AUTOSAR-A7_1_5-a "False positive: Function uses trailing return type, not auto"
/// @brief Get the first index after @c current_front which is *not* in the deque (so it can be added)
/// @param current_front The current front index of a deque
/// @param capacity The capacity of the deque which @c current_front is an index of
/// @return The physical index one toward the front of <c>current_front</c>, wrapped to stay in bounds
/// @note This impl function takes @c capacity as a runtime parameter in order to improve test coverage.
constexpr auto next_at_front_impl(std::size_t const current_front, std::size_t const capacity) noexcept -> std::size_t {
  return current_front > 0U ? current_front - 1U : capacity - 1U;
}
// parasoft-end-suppress AUTOSAR-A7_1_5-a

/// @brief Aggregate object for holding a bundle of physical indices encountered when iterating over a deque
struct physical_indices {
  /// @brief Sentinel value for the logical end of the deque; does not correspond to a physical index
  static constexpr std::size_t logical_end{std::numeric_limits<std::size_t>::max()};

  /// @brief Current physical index being iterated over
  std::size_t current;
  /// @brief Physical index of the logical front of the deque
  std::size_t front;
  /// @brief Physical index of the logical back of the deque
  std::size_t back;
  /// @brief Capacity of the deque
  std::size_t capacity;
};

/// @brief ring-buffer indices independent of the contained value type
/// @tparam Capacity The capacity of the deque
/// @tparam AllowWrapping Whether index wraparound is allowed (true) or treated as a precondition violation (false)
template <std::size_t Capacity, bool AllowWrapping>
class ring_buffer_indices {
 public:
  /// @brief The type used to hold the size
  using size_type = std::size_t;

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "False positive: this is not 'member data', it is a public property"
  /// @brief Alias to @c Capacity with type @c size_type to reduce the required conversions
  static constexpr std::integral_constant<size_type, Capacity> capacity{};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a

  /// @brief Get the index to the back of the deque
  /// @return the back index
  constexpr auto back_index() const noexcept -> size_type { return n_from_front(size() - 1U); }

  /// @brief Get the index @c offset elements from the back of the deque
  /// @param offset The offset from the back
  /// @return The index @c offset elements from the back, wrapped to stay in bounds
  /// @pre <c>offset < size()</c> or the returned index will be invalid
  constexpr auto n_from_back(size_type offset) const noexcept -> size_type {
    return n_from_front(size() - 1U - offset);
  }

  /// @brief Get the index of the front of the deque
  /// @return the front index
  constexpr auto front_index() const noexcept -> size_type { return front_; }

  /// @brief Get the index @c offset elements back from the front of the queue
  /// @param offset The offset from the front
  /// @return The index @c offset elements from the front, wrapped to stay in bounds
  /// @pre <c>offset < capacity()</c> or the returned index will be invalid
  constexpr auto n_from_front(size_type offset) const noexcept -> size_type {
    return constrained_modulo(front_index() + offset, capacity());
  }

  /// @brief Get the first index after @c front_index() which is *not* in the deque (so it can be added)
  /// @return The physical index one toward the front of <c>front</c>, wrapped to stay in bounds
  constexpr auto next_at_front() const noexcept -> size_type { return next_at_front_impl(front_index(), capacity()); }

  /// @brief Get the first index after @c back_index() which is *not* in the deque (so it can be added)
  /// @return The physical index one toward the back of <c>back</c>, wrapped to stay in bounds
  constexpr auto next_at_back() const noexcept -> size_type { return n_from_front(size()); }

  /// @brief Get the number of elements
  /// @return the size
  constexpr auto size() const noexcept -> size_type { return size_; }

  /// @brief Check if the number of elements is zero
  /// @return @c true if the number of elements is zero, @c false otherwise
  constexpr auto empty() const noexcept -> bool { return size() == 0U; }

  /// @brief Shrink the buffer by removing space for one or more elements at the front
  /// @param amount The amount to grow by
  constexpr void shrink_at_front(size_type amount = 1U) noexcept {
    front_ = n_from_front(amount);
    size_ -= amount;
  }

  /// @brief Shrink the buffer by removing space for one element at the back
  constexpr void shrink_at_back() noexcept { --size_; }

  /// @brief Grow the buffer by adding space for one element at the front
  constexpr void grow_at_front() noexcept {
    front_ = next_at_front();
    ++size_;
  }

  /// @brief Grow the buffer by adding space for one or more elements at the back
  /// @param amount The amount to grow by
  constexpr void grow_at_back(size_type amount = 1U) noexcept { size_ += amount; }

  /// @brief Get a bundle of all of the physical indices relevant to iteration over a deque
  /// @param current_index Physical index currently being iterated over
  /// @return A bundle of @c current_index along with the front, back, and capacity of the deque
  /// @note This is provided as a runtime value so that functions using it are not capacity-dependent templates
  constexpr auto get_physical_indices(std::size_t current_index) const noexcept -> physical_indices {
    return {current_index, front_index(), back_index(), capacity()};
  }

 private:
  /// @brief Index of the front of the deque (the lowest/leftmost populated index)
  size_type front_{0U};
  /// @brief Number of elements
  size_type size_{0U};
};

template <std::size_t Capacity, bool AllowWrapping>
constexpr std::integral_constant<typename ring_buffer_indices<Capacity, AllowWrapping>::size_type, Capacity>
    ring_buffer_indices<Capacity, AllowWrapping>::capacity;

/// @brief An interface for @c inline_deque_internal_storage to conditionally allow storage to wrap around
/// @tparam Derived The derived class using this interface
/// @tparam AllowWrapping Whether to allow wrapping (if true) or treat it as a precondition violation (if false)
/// @note This is the version where wrapping is allowed, in which space will be cleared out from the opposite side
template <typename Derived, bool AllowWrapping>
class storage_wrapping_interface {
  /// @brief Get a reference to the current instance cast down to the derived class
  /// @return @c *this as a <c>Derived&</c>
  constexpr auto derived() noexcept -> Derived& { return static_cast<Derived&>(*this); }

 protected:
  /// @brief Ensure that space is available at the back, either as a precondition check or by clearing space when full
  /// @pre If @c AllowWrapping is <c>false</c> and <c>size() >= capacity()</c>, crashes with a precondition violation
  /// @post If this function returns, there is at least one free space at the back of the deque
  constexpr auto ensure_space_at_back() noexcept -> void {
    if (derived().size() == Derived::capacity()) {
      using value_type = typename Derived::value_type;

      // To make space at the *back*, we clear an entry off of the *front*
      // parasoft-begin-suppress CERT_C-ERR33-a "False positive: at_buffer_idx's return value is used"
      // parasoft-begin-suppress CERT_C-POS54-a "False positive: at_buffer_idx's return value is used"
      // parasoft-begin-suppress AUTOSAR-A0_1_2-a "False positive: at_buffer_idx's return value is used"
      derived().at_buffer_idx(derived().front_index()).~value_type();
      derived().shrink_at_front();
      // parasoft-end-suppress CERT_C-ERR33-a
      // parasoft-end-suppress CERT_C-POS54-a
      // parasoft-end-suppress AUTOSAR-A0_1_2-a
    }
  }

  /// @brief Ensure that space is available at the front, either as a precondition check or by clearing space when full
  /// @pre If @c AllowWrapping is <c>false</c> and <c>size() >= capacity()</c>, crashes with a precondition violation
  /// @post If this function returns, there is at least one free space at the front of the deque
  constexpr auto ensure_space_at_front() noexcept -> void {
    if (derived().size() == Derived::capacity()) {
      using value_type = typename Derived::value_type;

      // To make space at the *front*, we clear an entry off of the *back*
      // parasoft-begin-suppress CERT_C-ERR33-a "False positive: at_buffer_idx's return value is used"
      // parasoft-begin-suppress CERT_C-POS54-a "False positive: at_buffer_idx's return value is used"
      // parasoft-begin-suppress AUTOSAR-A0_1_2-a "False positive: at_buffer_idx's return value is used"
      derived().at_buffer_idx(derived().back_index()).~value_type();
      derived().shrink_at_back();
      // parasoft-end-suppress CERT_C-ERR33-a
      // parasoft-end-suppress CERT_C-POS54-a
      // parasoft-end-suppress AUTOSAR-A0_1_2-a
    }
  }
};

/// @brief An interface for @c inline_deque_internal_storage to conditionally allow storage to wrap around
/// @tparam Derived The derived class using this interface
/// @note This is the version where wrapping is not allowed, with a precondition check that the deque is not full
template <typename Derived>
class storage_wrapping_interface<Derived, false> {
  /// @brief Get an immutable reference to the current instance cast down to the derived class
  /// @return @c *this as a <c>Derived const&</c>
  constexpr auto derived() const noexcept -> Derived const& { return static_cast<Derived const&>(*this); }

 protected:
  /// @brief Ensure that space is available at the back, either as a precondition check or by clearing space when full
  /// @pre If @c AllowWrapping is <c>false</c> and <c>size() >= capacity()</c>, crashes with a precondition violation
  /// @post If this function returns, there is at least one free space at the back of the deque
  constexpr auto ensure_space_at_back() const noexcept -> void {
    ARENE_PRECONDITION(derived().size() < Derived::capacity());
  }

  /// @brief Ensure that space is available at the front, either as a precondition check or by clearing space when full
  /// @pre If @c AllowWrapping is <c>false</c> and <c>size() >= capacity()</c>, crashes with a precondition violation
  /// @post If this function returns, there is at least one free space at the front of the deque
  constexpr auto ensure_space_at_front() const noexcept -> void {
    ARENE_PRECONDITION(derived().size() < Derived::capacity());
  }
};

// parasoft-begin-suppress AUTOSAR-A1_1_1-c "False positive: this class defines all special member functions"
// parasoft-begin-suppress CERT_CPP-MEM51-c "False positive: this class defines all special member functions"
// parasoft-begin-suppress AUTOSAR-M14_5_3-a "False positive: this does have a copy assignment operator"
// parasoft-begin-suppress AUTOSAR-A14_5_1-a "False positive: this does have a copy constructor"
// parasoft-begin-suppress AUTOSAR-A12_0_1-a "False positive: this does have a copy constructor and assignment operator"
// parasoft-begin-suppress AUTOSAR-A10_1_1-a "Multiple inheritance to provide mixin member function implementation"
/// @brief Base class for @c inline_deque that holds the elements
/// @note This specialization applies when @c T is not trivially constructible or not trivially copyable.
/// @tparam T The type of the elements
/// @tparam Capacity The capacity of the deque
/// @tparam AllowWrapping Whether index wraparound is allowed (true) or treated as a precondition violation (false)
template <
    typename T,
    std::size_t Capacity,
    bool AllowWrapping,
    bool = std::is_trivially_default_constructible<T>::value && std::is_trivially_copyable<T>::value>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class inline_deque_internal_storage
    : private array<inline_container::detail::unsafe_element_storage<T>, Capacity>
    , private ring_buffer_indices<Capacity, AllowWrapping>
    , protected storage_wrapping_interface<inline_deque_internal_storage<T, Capacity, AllowWrapping>, AllowWrapping> {
 protected:
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Friend mechanism allows mixin to access protected member functions"
  /// @brief This mixin needs access to the class's protected and private members; it's only used inside this class
  friend storage_wrapping_interface<inline_deque_internal_storage, AllowWrapping>;
  // parasoft-end-suppress AUTOSAR-A11_3_1-a

  /// @brief The type used to hold the size
  using size_type = typename ring_buffer_indices<Capacity, AllowWrapping>::size_type;
  /// @brief The type of the values stored
  using value_type = T;
  /// @brief The type of a reference to a value stored
  using reference = value_type&;
  /// @brief The type of a const reference to a value stored
  using const_reference = value_type const&;

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "False positive: this is not 'member data', it is a public property"
  /// @brief Get the maximum number of elements in the storage, @c Capacity
  static constexpr std::integral_constant<size_type, Capacity> capacity{};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a

  /// @brief Check the current _logical size_ of the container
  /// @note Specifying this causes it to override @c array::size which is the _buffer size_ (i.e. the capacity)
  using ring_buffer_indices<Capacity, AllowWrapping>::size;

  /// @brief Check if the current _logical size_ of the container is 0
  /// @note Specifying this causes it to override @c array::empty which uses the _buffer size_ (i.e. the capacity)
  using ring_buffer_indices<Capacity, AllowWrapping>::empty;

  /// @copydoc ring_buffer_indices<Capacity, AllowWrapping>::front_index
  using ring_buffer_indices<Capacity, AllowWrapping>::front_index;

  /// @copydoc ring_buffer_indices<Capacity, AllowWrapping>::back_index
  using ring_buffer_indices<Capacity, AllowWrapping>::back_index;

  /// @copydoc ring_buffer_indices<Capacity, AllowWrapping>::n_from_front
  using ring_buffer_indices<Capacity, AllowWrapping>::n_from_front;

  /// @copydoc ring_buffer_indices<Capacity, AllowWrapping>::n_from_back
  using ring_buffer_indices<Capacity, AllowWrapping>::n_from_back;

  /// @copydoc ring_buffer_indices<Capacity, AllowWrapping>::get_physical_indices
  using ring_buffer_indices<Capacity, AllowWrapping>::get_physical_indices;

 private:
  /// @brief underlying element type that allows a value to exist or not exist
  using element_storage = inline_container::detail::unsafe_element_storage<T>;

  // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
  // side-effects"
  static_assert(alignof(element_storage) == alignof(T), "Alignment must be the same");
  // parasoft-end-suppress CERT_C-PRE31-c-3
  static_assert(sizeof(element_storage) == sizeof(T), "Size must be the same");

  /// @brief The type of the argument to the "copy constructor": either @c
  /// inline_deque_internal_storage to provide a real copy constructor if @c T is
  /// copy-constructible, or @c non_constructible_dummy otherwise.
  using copy_constructor_arg =
      conditional_t<std::is_copy_constructible<T>::value, inline_deque_internal_storage, non_constructible_dummy>;

  /// @brief The type of the argument to the "copy assignment operator": either @c
  /// inline_deque_internal_storage to provide a real copy assignment operator if @c T is
  /// copy-constructible and copy-assignable, or @c non_constructible_dummy otherwise.
  using copy_assignment_arg = conditional_t<
      std::is_copy_constructible<T>::value && std::is_copy_assignable<T>::value,
      inline_deque_internal_storage,
      non_constructible_dummy>;

  /// @brief The type of the argument to the "move constructor": either @c
  /// inline_deque_internal_storage to provide a real move constructor if @c T is
  /// move-constructible, or @c non_constructible_dummy otherwise.
  using move_constructor_arg =
      conditional_t<std::is_move_constructible<T>::value, inline_deque_internal_storage, non_constructible_dummy>;

  /// @brief The type of the argument to the "move assignment operator": either @c
  /// inline_deque_internal_storage to provide a real move assignment operator if @c T is
  /// move-constructible and move-assignable, or @c non_constructible_dummy otherwise.
  using move_assignment_arg = conditional_t<
      std::is_move_constructible<T>::value && std::is_move_assignable<T>::value,
      inline_deque_internal_storage,
      non_constructible_dummy>;

 protected:
  /// @brief Constructs the storage with no elements
  constexpr inline_deque_internal_storage() = default;

  /// @brief Construct storage from a range of elements, taken to be in front -> back order
  /// @tparam Iterator The type of the iterators to construct from
  /// @param in_begin An iterator to the beginning (front) of the input range
  /// @param in_end An iterator to the end (back+1) of the input range
  /// @pre <c>distance(in_begin, in_end) >= 0</c> and <c><= capacity()</c>, otherwise @c ARENE_PRECONDITION violation
  template <typename Iterator>
  constexpr inline_deque_internal_storage(Iterator in_begin, Iterator in_end) noexcept(
      noexcept(::arene::base::distance(in_begin, in_end)) &&
      std::is_nothrow_constructible<T, typename std::iterator_traits<Iterator>::reference>::value
  )
      : ::arene::base::array<element_storage, Capacity>{} {
    typename std::iterator_traits<Iterator>::difference_type const distance{::arene::base::distance(in_begin, in_end)};
    ARENE_PRECONDITION(distance >= 0);
    ARENE_PRECONDITION(static_cast<size_type>(distance) <= capacity);

    while (in_begin != in_end) {
      std::ignore = emplace_back(*in_begin);
      // parasoft-begin-suppress AUTOSAR-M5_0_15-a "False positive: this is iterator arithmetic, not pointer-specific"
      ++in_begin;
      // parasoft-end-suppress AUTOSAR-M5_0_15-a
    }
  }

  /// @brief Copy constructor if @c T is copy-constructible, otherwise
  /// conversion from @c non_constructible_dummy. If copying, copies
  /// alive elements from @c other into @c *this.
  /// @param other The source
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions) This is a copy constructor
  constexpr inline_deque_internal_storage(copy_constructor_arg const& other
  ) noexcept(std::is_nothrow_copy_constructible<T>::value) {
    for (inline_deque_internal_storage::size_type i{}; i < other.size(); i++) {
      std::ignore = emplace_front(other[other.n_from_back(i)].object);
    }
  }

  // parasoft-begin-suppress AUTOSAR-A15_5_1-b "noexcept specification is set based on that of the element type"
  /// @brief Move constructor if @c T is move-constructible, useless conversion from @c
  /// non_constructible_dummy otherwise.
  /// @param other The source
  /// @post @c other is empty
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions) This is a move constructor
  constexpr inline_deque_internal_storage(move_constructor_arg&& other
  ) noexcept(std::is_nothrow_move_constructible<T>::value) {
    for (inline_deque_internal_storage::size_type i{}; i < other.size(); i++) {
      std::ignore = emplace_front(std::move(other[other.n_from_back(i)].object));
    }
    other.clear();
  }
  // parasoft-end-suppress AUTOSAR-A15_5_1-b

  // parasoft-begin-suppress AUTOSAR-A15_5_3-h "False positive: this is noexcept(false) when it can throw"
  // parasoft-begin-suppress CERT_CPP-ERR50-h "False positive: this is noexcept(false) when it can throw"
  // parasoft-begin-suppress CERT_CPP-ERR55-a "False positive: this is noexcept(false) when it can throw"
  /// @brief Copy assignment operator if @c T is copy-constructible, useless
  /// assignment from @c non_constructible_dummy otherwise.
  /// @param other The source
  /// @return @c *this
  constexpr auto operator=(copy_assignment_arg const& other
  ) noexcept(std::is_nothrow_copy_constructible<T>::value && std::is_nothrow_copy_assignable<T>::value)
      -> inline_deque_internal_storage& {
    if (&other != this) {
      // If *this is bigger than other, first shrink so that they match
      while (this->size() > other.size()) {
        pop_back();
      }

      // Assign as many elements as possible (however many already exist)
      inline_deque_internal_storage::size_type assignments{std::min(this->size(), other.size())};
      for (inline_deque_internal_storage::size_type i{}; i < assignments; ++i) {
        (*this)[this->n_from_back(i)].object = other[other.n_from_back(i)].object;
      }

      // If other is bigger than *this, push in additional elements after the assignments
      if (this->size() < other.size()) {
        size_type const pushes{other.size() - this->size()};
        for (inline_deque_internal_storage::size_type i{}; i < pushes; ++i) {
          std::ignore = emplace_front(other[other.n_from_back(assignments + i)].object);
        }
      }
    }
    return *this;
  }

  // parasoft-begin-suppress AUTOSAR-A15_5_1-b "noexcept specification is set based on that of the element type"
  /// @brief Move assignment operator if @c T is move-constructible, useless
  /// assignment from @c non_constructible_dummy otherwise.
  /// Move assigns over all of the elements of @c *this that are already alive, then move constructs remaining elements
  /// if @c other.size() was greater than @c this->size()
  /// @param other The source
  /// @post @c other is empty
  /// @return @c *this
  constexpr auto operator=(move_assignment_arg&& other
  ) noexcept(std::is_nothrow_move_constructible<T>::value && std::is_nothrow_move_assignable<T>::value)
      -> inline_deque_internal_storage& {
    if (&other != this) {
      size_type const target_size{other.size()};

      // If *this is bigger than other, first shrink so that they match
      while (this->size() > target_size) {
        pop_back();
      }

      // Assign as many elements as possible (however many already exist)
      for (inline_deque_internal_storage::size_type i{}; i < this->size(); ++i) {
        (*this)[this->n_from_back(i)].object = std::move(other[other.back_index()].object);
        other.pop_back();
      }

      // If other is bigger than *this, push in additional elements after the assignments
      while (this->size() < target_size) {
        std::ignore = emplace_front(std::move(other[other.back_index()].object));
        other.pop_back();
      }

      other.clear();
    }
    return *this;
  }
  // parasoft-end-suppress AUTOSAR-A15_5_3-h
  // parasoft-end-suppress CERT_CPP-ERR50-h
  // parasoft-end-suppress CERT_CPP-ERR55-a
  // parasoft-end-suppress AUTOSAR-A15_5_1-b

  /// @brief Destroy all alive elements
  ~inline_deque_internal_storage() { clear(); }

  // parasoft-begin-suppress AUTOSAR-A15_5_3-h "False positive: this is noexcept(false) when it can throw"
  // parasoft-begin-suppress CERT_CPP-ERR50-h "False positive: this is noexcept(false) when it can throw"
  // parasoft-begin-suppress CERT_CPP-ERR55-a "False positive: this is noexcept(false) when it can throw"
  /// @brief Construct a new element at the front
  /// @tparam Args The types of the arguments
  /// @param args The constructor arguments
  /// @return A reference to the emplaced element
  /// @pre If wrapping is not allowed, <c>size() < capacity()</c>, otherwise @c ARENE_PRECONDITION violation
  /// @note If wrapping is allowed and <c>size() == capacity()</c>, the element at @c back() is removed and iterators
  /// pointing to it are invalidated.
  template <typename... Args>
  constexpr auto emplace_front(Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value) -> T& {
    this->ensure_space_at_front();
    T& new_ref{*construct_at(&(*this)[this->next_at_front()].object, std::forward<Args>(args)...)};
    this->grow_at_front();
    return new_ref;
  }

  /// @brief Construct a new element at the front
  /// @tparam Args The types of the arguments
  /// @param args The constructor arguments
  /// @return A reference to the emplaced element
  /// @pre If wrapping is not allowed, <c>size() < capacity()</c>, otherwise @c ARENE_PRECONDITION violation
  /// @note If wrapping is allowed and <c>size() == capacity()</c>, the element at @c front() is removed and iterators
  /// pointing to it are invalidated.
  template <typename... Args>
  constexpr auto emplace_back(Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value) -> T& {
    this->ensure_space_at_back();
    T& new_ref{*construct_at(&(*this)[this->next_at_back()].object, std::forward<Args>(args)...)};
    this->grow_at_back();
    return new_ref;
  }
  // parasoft-end-suppress AUTOSAR-A15_5_3-h
  // parasoft-end-suppress CERT_CPP-ERR50-h
  // parasoft-end-suppress CERT_CPP-ERR55-a

  /// @brief Destroy the element at the front and decrease the size
  /// @pre <c>size() > 0</c>, otherwise @c ARENE_PRECONDITION violation
  constexpr void pop_front() noexcept {
    ARENE_PRECONDITION(this->size() > size_type{});
    (*this)[this->front_index()].object.~T();
    this->shrink_at_front();
  }

  /// @brief Destroy the element at the back and decrease the size
  /// @pre <c>size() > 0</c>, otherwise @c ARENE_PRECONDITION violation
  constexpr void pop_back() noexcept {
    ARENE_PRECONDITION(this->size() > size_type{});
    (*this)[this->back_index()].object.~T();
    this->shrink_at_back();
  }

  /// @brief Perform an unchecked dereference of the element at the given *physical buffer* index
  /// @param buffer_idx The *physical* index to dereference; assumed to refer to a currently-valid element
  /// @return A const reference to the element at *physical* index @c idx
  constexpr auto at_buffer_idx(std::size_t buffer_idx) const noexcept -> const_reference {
    return (*this)[buffer_idx].object;
  }

  /// @brief Perform an unchecked dereference of the element at the given *physical buffer* index
  /// @param buffer_idx The *physical* index to dereference; assumed to refer to a currently-valid element
  /// @return A mutable reference to the element at *physical* index @c idx
  constexpr auto at_buffer_idx(std::size_t buffer_idx) noexcept -> reference { return (*this)[buffer_idx].object; }

 private:
  /// @brief Empties the storage
  constexpr void clear() noexcept {
    while (!this->empty()) {
      pop_back();
    }
  }
};
// parasoft-end-suppress AUTOSAR-A1_1_1-c
// parasoft-end-suppress CERT_CPP-MEM51-c
// parasoft-end-suppress AUTOSAR-M14_5_3-a
// parasoft-end-suppress AUTOSAR-A14_5_1-a
// parasoft-end-suppress AUTOSAR-A12_0_1-a
// parasoft-end-suppress AUTOSAR-A10_1_1-a

// parasoft-begin-suppress AUTOSAR-A10_1_1-a "Multiple inheritance to provide mixin member function implementation"
/// @brief Base class for @c inline_deque that holds the elements
/// @note This specialization applies when @c T is both trivially constructible and trivially copyable.
/// @tparam T The type of the elements
/// @tparam Capacity The capacity of the deque
/// @tparam AllowWrapping Whether index wraparound is allowed (true) or treated as a precondition violation (false)
template <typename T, std::size_t Capacity, bool AllowWrapping>
class inline_deque_internal_storage<T, Capacity, AllowWrapping, true>
    : private array<T, Capacity>
    , private ring_buffer_indices<Capacity, AllowWrapping>
    , protected storage_wrapping_interface<inline_deque_internal_storage<T, Capacity, AllowWrapping>, AllowWrapping> {
 protected:
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Friend mechanism allows mixin to access protected member functions"
  /// @brief This mixin needs access to the class's protected and private members; it's only used inside this class
  friend storage_wrapping_interface<inline_deque_internal_storage, AllowWrapping>;
  // parasoft-end-suppress AUTOSAR-A11_3_1-a

  /// @brief The type used to hold the size
  using size_type = typename ring_buffer_indices<Capacity, AllowWrapping>::size_type;
  /// @brief The type of a value stored
  using value_type = T;
  /// @brief The type of a reference to a value stored
  using reference = value_type&;
  /// @brief The type of a const reference to a value stored
  using const_reference = value_type const&;

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "False positive: this is not 'member data', it is a public property"
  /// @brief Get the maximum number of elements in the deque, @c Capacity
  static constexpr std::integral_constant<size_type, Capacity> capacity{};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a

  /// @brief Check the current _logical size_ of the container
  /// @note Specifying this causes it to override @c array::size which is the _buffer size_ (i.e. the capacity)
  using ring_buffer_indices<Capacity, AllowWrapping>::size;

  /// @brief Check if the current _logical size_ of the container is 0
  /// @note Specifying this causes it to override @c array::empty which uses the _buffer size_ (i.e. the capacity)
  using ring_buffer_indices<Capacity, AllowWrapping>::empty;

  /// @copydoc ring_buffer_indices<Capacity, AllowWrapping>::front_index
  using ring_buffer_indices<Capacity, AllowWrapping>::front_index;

  /// @copydoc ring_buffer_indices<Capacity, AllowWrapping>::back_index
  using ring_buffer_indices<Capacity, AllowWrapping>::back_index;

  /// @copydoc ring_buffer_indices<Capacity, AllowWrapping>::n_from_front
  using ring_buffer_indices<Capacity, AllowWrapping>::n_from_front;

  /// @copydoc ring_buffer_indices<Capacity, AllowWrapping>::n_from_back
  using ring_buffer_indices<Capacity, AllowWrapping>::n_from_back;

  /// @copydoc ring_buffer_indices<Capacity, AllowWrapping>::get_physical_indices
  using ring_buffer_indices<Capacity, AllowWrapping>::get_physical_indices;

  // parasoft-begin-suppress AUTOSAR-A12_7_1-a "False positive: defining with =default would not be constexpr"
  /// @brief Constructs the storage with no elements
  constexpr inline_deque_internal_storage() noexcept
      : array<T, Capacity>{},
        ring_buffer_indices<Capacity, AllowWrapping>{},
        storage_wrapping_interface<inline_deque_internal_storage, AllowWrapping>{} {}
  // parasoft-end-suppress AUTOSAR-A12_7_1-a

  /// @brief Construct storage from a range of elements, taken to be in front -> back order
  /// @tparam Iterator The type of the iterators to construct from
  /// @param in_begin An iterator to the beginning (front) of the input range
  /// @param in_end An iterator to the end (back+1) of the input range
  /// @pre <c>distance(in_begin, in_end) >= 0</c> and <c><= capacity()</c>, otherwise @c ARENE_PRECONDITION violation
  template <typename Iterator>
  constexpr inline_deque_internal_storage(Iterator in_begin, Iterator in_end) noexcept(noexcept(
      ::arene::base::distance(in_begin, in_end)
  ) && noexcept(::arene::base::copy(in_begin, in_end, std::declval<array<T, Capacity>&>().begin())))
      : ::arene::base::array<T, Capacity>{} {
    auto const count = ::arene::base::distance(in_begin, in_end);
    ARENE_PRECONDITION((count >= 0) && (static_cast<std::size_t>(count) <= capacity()));

    typename ::arene::base::array<T, Capacity>::iterator const new_end{
        ::arene::base::copy(in_begin, in_end, this->begin())
    };

    // Cast here is OK because the returned value of copy will always be equal to or after this->begin()
    this->grow_at_back(static_cast<std::size_t>(::arene::base::distance(this->begin(), new_end)));
  }

  /// @brief Construct a new element at the front
  /// @tparam Args The types of the arguments
  /// @param args The constructor arguments
  /// @return A reference to the emplaced element
  /// @pre If wrapping is not allowed, <c>size() < capacity()</c>, otherwise @c ARENE_PRECONDITION violation
  /// @note If wrapping is allowed and <c>size() == capacity()</c>, the element at @c back() is removed and iterators
  /// pointing to it are invalidated.
  template <typename... Args>
  constexpr auto emplace_front(Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value) -> T& {
    this->ensure_space_at_front();

    // parasoft-begin-suppress AUTOSAR-A5_2_2-a "False positive: not a C-cast, generic syntax needed for constructors"
    T& new_ref{((*this)[this->next_at_front()] = T(std::forward<Args>(args)...))};
    // parasoft-end-suppress AUTOSAR-A5_2_2-a

    this->grow_at_front();
    return new_ref;
  }

  /// @brief Construct a new element at the front
  /// @tparam Args The types of the arguments
  /// @param args The constructor arguments
  /// @return A reference to the emplaced element
  /// @pre If wrapping is not allowed, <c>size() < capacity()</c>, otherwise @c ARENE_PRECONDITION violation
  /// @note If wrapping is allowed and <c>size() == capacity()</c>, the element at @c front() is removed and iterators
  /// pointing to it are invalidated.
  template <typename... Args>
  constexpr auto emplace_back(Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value) -> T& {
    this->ensure_space_at_back();

    // parasoft-begin-suppress AUTOSAR-A5_2_2-a "False positive: not a C-cast, generic syntax needed for constructors"
    T& new_ref{((*this)[this->next_at_back()] = T(std::forward<Args>(args)...))};
    // parasoft-end-suppress AUTOSAR-A5_2_2-a

    this->grow_at_back();
    return new_ref;
  }

  /// @brief Decrease the size by one element from the front
  /// @pre <c>size() > 0</c>, otherwise @c ARENE_PRECONDITION violation
  constexpr void pop_front() noexcept {
    ARENE_PRECONDITION(this->size() > size_type{});
    this->shrink_at_front();
  }

  /// @brief Decrease the size by one element from the back
  /// @pre <c>size() > 0</c>, otherwise @c ARENE_PRECONDITION violation
  constexpr void pop_back() noexcept {
    ARENE_PRECONDITION(this->size() > size_type{});
    this->shrink_at_back();
  }

  /// @brief Perform an unchecked dereference of the element at the given *physical buffer* index
  /// @param buffer_idx The *physical* index to dereference; assumed to refer to a currently-valid element
  /// @return A const reference to the element at *physical* index @c idx
  constexpr auto at_buffer_idx(std::size_t buffer_idx) const noexcept -> const_reference { return (*this)[buffer_idx]; }

  /// @brief Perform an unchecked dereference of the element at the given *physical buffer* index
  /// @param buffer_idx The *physical* index to dereference; assumed to refer to a currently-valid element
  /// @return A mutable reference to the element at *physical* index @c idx
  constexpr auto at_buffer_idx(std::size_t buffer_idx) noexcept -> reference { return (*this)[buffer_idx]; }
};
// parasoft-end-suppress AUTOSAR-A10_1_1-a

template <typename T, std::size_t Capacity, bool AllowWrapping, bool IsTrivial>
constexpr std::integral_constant<
    typename inline_deque_internal_storage<T, Capacity, AllowWrapping, IsTrivial>::size_type,
    Capacity>
    inline_deque_internal_storage<T, Capacity, AllowWrapping, IsTrivial>::capacity;

template <typename T, std::size_t Capacity, bool AllowWrapping>
constexpr std::
    integral_constant<typename inline_deque_internal_storage<T, Capacity, AllowWrapping, true>::size_type, Capacity>
        inline_deque_internal_storage<T, Capacity, AllowWrapping, true>::capacity;

// parasoft-begin-suppress AUTOSAR-A7_1_5-a "False positive: Functions use trailing return type, not auto"

/// @brief Return the physical index of the logical element one step forward in the container (i.e. toward the back)
/// @param indices Current physical indices of iteration over a deque
/// @return The physical index of the element which is one logical step after @c indices.current
/// @note This function takes all parameters as runtime arguments so that all branches can be covered
constexpr auto step_logical_index_forward(physical_indices const& indices) noexcept -> std::size_t {
  // Stepping forward from the logical back falls off the edge of the logical indices, going to the special end index.
  if (indices.current == indices.back) {
    return indices.logical_end;
  }

  // Stepping forward from the physical back requires wrapping to the physical front.
  if (indices.current == indices.capacity - 1U) {
    return std::size_t{};
  }

  // If the current index is neither the physical nor logical back, then just increment it.
  return indices.current + 1U;
}

/// @brief Return the physical index of the logical element one step backward in the container (i.e. toward the front)
/// @param indices Current physical indices of iteration over a deque
/// @return The physical index of the element which is one logical step before @c indices.current
/// @note This function takes all parameters as runtime arguments so that all branches can be covered
constexpr auto step_logical_index_backward(physical_indices const& indices) noexcept -> std::size_t {
  // Stepping backward from the special end index enters the deque at the current logical back.
  if (indices.current == indices.logical_end) {
    return indices.back;
  }

  // Stepping backward from the physical front requires wrapping to the physical back.
  if (indices.current == std::size_t{}) {
    return indices.capacity - 1U;
  }

  // If the current index is neither the physical nor logical front, then just decrement it.
  return indices.current - 1U;
}

/// @brief Return the current logical index corresponding to the given physical index
/// @param indices Current physical indices of iteration over a deque
/// @return The logical index corresponding to <c>indices.current</c>, where @c indices.front is logical 0
/// @note This function takes all parameters as runtime arguments so that all branches can be covered
constexpr auto get_logical_index_impl(physical_indices const& indices) noexcept -> std::ptrdiff_t {
  std::size_t const shifted_idx{
      indices.front <= indices.current ? indices.current : indices.current + indices.capacity
  };
  return static_cast<std::ptrdiff_t>(shifted_idx) - static_cast<std::ptrdiff_t>(indices.front);
}

/// @brief Policy struct required by @c inline_container::detail::try_construct_interface to fill in basic information
/// @tparam T Value type parameter of an @c inline_deque
/// @tparam Capacity Capacity parameter of an @c inline_deque
template <class T, std::size_t Capacity>
class inline_deque_try_construct_policy {
 public:
  /// @brief The @c size_type of the corresponding @c inline_deque
  using size_type = std::size_t;
  /// @brief The @c value_type of the corresponding @c inline_deque
  using value_type = T;
  /// @brief A function to get the capacity of the corresponding @c inline_deque
  /// @return The static capacity of the corresponding @c inline_deque
  static constexpr auto capacity_of() noexcept -> size_type { return Capacity; }
};

// parasoft-end-suppress AUTOSAR-A7_1_5-a

// parasoft-begin-suppress AUTOSAR-A10_1_1-a "Multiple inheritance to provide mixin member function implementation"
/// @brief A container with fixed capacity featuring constant time insertion and
/// removal at opposite ends; i.e., with an interface similar to std::queue. The
/// storage for the elements is held directly within the class.
/// @tparam T The type of each element
/// @tparam Capacity The maximum number of elements that can be stored
/// @tparam AllowWrapping Whether index wraparound is allowed (true) or treated as a precondition violation (false)
template <typename T, std::size_t Capacity, bool AllowWrapping>
class inline_deque_impl
    : private inline_deque_detail::inline_deque_internal_storage<T, Capacity, AllowWrapping>
    , public inline_container::detail::range_interface<inline_deque_impl<T, Capacity, AllowWrapping>> {
 private:
  /// @brief Shorthand to the internal type containing the element storage
  using storage_type = inline_deque_detail::inline_deque_internal_storage<T, Capacity, AllowWrapping>;

  /// @brief Shorthand to the mixin type that synthesizes range functions based on the iterator defined below
  using range_interface = inline_container::detail::range_interface<inline_deque_impl>;

  // parasoft-begin-suppress AUTOSAR-A12_1_5-a "False positive: none of these constructors can be delegated"
  /// @brief An iterator for an @c inline_deque_impl which automatically wraps at @c Capacity so that it stays in bounds
  /// @tparam IsConst @c true if this is a <c>const_iterator</c>, @c false if it's a normal @c iterator
  /// @note Iterators are stored referring to particular physical elements so that they remain valid even when other
  /// elements are removed from the deque. This matches the invalidation characteristics of <c>std::deque</c>.
  template <bool IsConst>
  class cyclical_iterator : public inline_container_detail::iterator_interface<cyclical_iterator<IsConst>> {
   private:
    /// @brief Shorthand to the mixin type that synthesizes iterator functions based on the manually-defined ones below
    using iterator_interface = inline_container_detail::iterator_interface<cyclical_iterator<IsConst>>;

    // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Friendship allows conversion from non-const iterator to const iterator
    // without exposing implementation details."
    friend cyclical_iterator<true>;
    // parasoft-end-suppress AUTOSAR-A11_3_1-a

   public:
    // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False Positive: identifiers at class/namespace scope are exempt"
    /// @brief The value type of the iterator; same as the @c inline_deque_impl itself
    using value_type = T;
    /// @brief The pointer type of the iterator; given as a template parameter
    using pointer = conditional_t<IsConst, T const*, T*>;
    /// @brief The reference type of the iterator; determined by the pointer type
    using reference = decltype(*std::declval<pointer>());
    /// @brief The difference type of the iterator; determined by the pointer type
    using difference_type = decltype(std::declval<pointer>() - std::declval<pointer>());
    /// @brief The iterator category; random access because the logical space is effectively a flat array
    using iterator_category = std::random_access_iterator_tag;
    // parasoft-end-suppress AUTOSAR-A2_10_1-e

    // The size of the logical space spanned by these iterators is one larger than the size of the deque, so the type
    // used to represent capacity must have room to be incremented once, and this must be representable as a difference.
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert can have no side-effects"
    static_assert(
        Capacity <= std::numeric_limits<std::size_t>::max(),
        "inline_deque capacity() + 1U must be representable using the type of capacity()"
    );
    static_assert(
        cmp_less_equal(Capacity + 1U, std::numeric_limits<difference_type>::max()),
        "inline_deque capacity must be expressable as its iterator's difference_type"
    );
    // parasoft-end-suppress CERT_C-PRE31-c-3

    // parasoft-begin-suppress AUTOSAR-M11_0_1-a "False positive: this is not 'member data', it is a public property"
    /// @brief The sentinel index used to distinguish the "end" iterator from any valid iterator pointing to an element
    /// @note This special value is necessary because otherwise there would be no way to distinguish between @c begin()
    /// and @c end() for a full @c inline_deque_impl
    static constexpr std::integral_constant<std::size_t, std::numeric_limits<std::size_t>::max()> end_index{};
    // parasoft-end-suppress AUTOSAR-M11_0_1-a

    /// @brief Default-construct an iterator pointing to no queue in particular
    /// @note Using this iterator will never result in undefined behaviour, but it's also not very useful.
    /// It will crash when dereferenced or incremented, and will never compare equal to non-default iterators.
    /// The existence of a default constructor is required in order to satisfy the requirements of a forward (or higher)
    /// iterator.
    constexpr cyclical_iterator() noexcept
        : iterator_interface(),
          deque_(nullptr),
          idx_(end_index) {}

    /// @brief Construct a @c cyclical_iterator pointing to a given element of a given deque
    /// @param deque A reference to the @c inline_deque_impl instance which is creating this iterator; will be saved
    /// @param idx The starting index of this iterator *within the physical buffer, not within the deque*
    constexpr cyclical_iterator(
        conditional_t<IsConst, inline_deque_impl const&, inline_deque_impl&> deque,
        std::size_t idx
    ) noexcept
        : iterator_interface(),
          deque_(&deque),
          idx_(idx) {}

    /// @brief Implicit converting constructor for turning a non-const iterator into a const one
    /// @tparam Enable Dummy template parameter to enable SFINAE; this constructor only exists for mutable -> const
    /// @param non_const A non-const iterator for the same underlying deque type
    template <bool Enable = IsConst, constraints<std::enable_if_t<Enable>> = nullptr>
    // NOLINTNEXTLINE(hicpp-explicit-conversions) This must be implicit to comply with [container.reqmts]
    constexpr cyclical_iterator(cyclical_iterator<!Enable> non_const) noexcept
        : iterator_interface(),
          deque_(non_const.deque_),
          idx_(non_const.idx_) {}

    // parasoft-begin-suppress AUTOSAR-M9_3_1-a "False positive: returned reference is not to member data of this class"
    /// @brief Dereference the iterator
    /// @return A reference to the currently pointed-to element
    /// @pre @c *this is not the end iterator, otherwise @c ARENE_PRECONDITION violation
    /// @pre @c *this has not been invalidated (e.g. by popping its element), otherwise the behaviour is undefined
    constexpr auto operator*() const noexcept -> reference {
      // Like in wrapped_iterator, we enforce the relatively cheap precondition that this isn't the end iterator,
      // but do not enforce the expensive-to-check precondition that the iterator is currently in bounds of the deque.
      ARENE_PRECONDITION(idx_ != end_index);
      return deque_->at_buffer_idx(idx_);
    }
    // parasoft-end-suppress AUTOSAR-M9_3_1-a

    /// @brief Shift the iterator by a given signed distance
    /// @param diff Signed distance by which to shift the current iterator
    /// @return A reference to the current iterator
    /// @pre Applying @c step_forward @c diff times (if <c>diff > 0</c>) or applying @c step_backward @c -diff times
    /// (if <c>diff < 0</c>) results in a valid iterator, otherwise the behaviour is undefined
    constexpr auto operator+=(difference_type diff) noexcept -> cyclical_iterator& {
      // Increments are computed in the logical index space of size deque_->size() + 1.

      set_logical_index(get_logical_index() + diff);

      return *this;
    }

    // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Hidden friends permitted by A11-3-1 Permit #2 v1.0.0"
    // parasoft-begin-suppress AUTOSAR-A0_1_3-a "False positive: Function is public and used in other translation units"
    // parasoft-begin-suppress AUTOSAR-A7_1_5-a "False positive: Function uses trailing return type, not auto"
    /// @brief Compute the difference between two iterators
    /// @param lhs The left of the two iterators
    /// @param rhs The right of the two iterators
    /// @return The difference between the iterators, i.e. a signed number @c n such that <c>lhs == rhs + n</c>
    /// @pre The two iterators must refer to the same <c>inline_deque_impl</c>, else precondition violation
    friend constexpr auto operator-(cyclical_iterator const& lhs, cyclical_iterator const& rhs) noexcept
        -> difference_type {
      ARENE_PRECONDITION(lhs.deque_ == rhs.deque_);

      // This operation needs to take place in the logical index space of size deque_->size() + 1.

      difference_type const logical_lhs_idx{lhs.get_logical_index()};
      difference_type const logical_rhs_idx{rhs.get_logical_index()};

      return logical_lhs_idx - logical_rhs_idx;
    }

    /// @brief Compare two iterators for equality
    /// @param lhs The left iterator to compare
    /// @param rhs The right iterator to compare
    /// @return @c true if both iterators refer to the same element of the same deque, otherwise @c false
    /// @note We use this manually-defined equality operator instead of the one synthesized by the mixin so that two
    /// iterators from different deques compare unequal, rather than causing a precondition violation. The ordered
    /// comparisons use the synthesized <c> operator< </c> and therefore crash if the deque doesn't match.
    friend constexpr auto operator==(cyclical_iterator const& lhs, cyclical_iterator const& rhs) noexcept -> bool {
      return (lhs.deque_ == rhs.deque_) && (lhs.idx_ == rhs.idx_);
    }
    // parasoft-end-suppress AUTOSAR-A11_3_1-a
    // parasoft-end-suppress AUTOSAR-A0_1_3-a
    // parasoft-end-suppress AUTOSAR-A7_1_5-a

    /// @brief Get the current physical index of this iterator
    /// @return The current physical index within the underlying buffer of the element pointed to by this iterator
    constexpr auto physical_index() const noexcept -> std::size_t { return idx_; }

   private:
    /// @brief Get the current size of the logical index space for the pointed-to @c deque_
    /// @return The size of the pointed-to @c deque_ plus one for the end index
    constexpr auto logical_space_size() const noexcept -> difference_type {
      return static_cast<difference_type>(deque_->size() + 1U);
    }

    /// @brief Get the current logical index of this iterator
    /// @return The logical index, i.e. the distance from the current @c front() to the pointed-to element
    constexpr auto get_logical_index() const noexcept -> difference_type {
      if (idx_ == end_index()) {
        ARENE_PRECONDITION(deque_ != nullptr);
        return static_cast<difference_type>(deque_->size());
      }

      // It's impossible to form a non-end iterator to a null deque_, so we don't need to check for null here.
      // The logical index is the distance from the front to the current idx_, counted cyclically in physical space.
      return inline_deque_detail::get_logical_index_impl(deque_->get_physical_indices(idx_));
    }

    /// @brief Set the current logical index of this iterator to a given value
    /// @param logical_idx The new logical index, i.e. the distance from the current @c front() to the element
    /// @pre <c>logical_idx >= 0 && logical_idx <= logical_space_size()</c> (note: less than *or equal to*), otherwise
    /// the behaviour is undefined
    constexpr auto set_logical_index(difference_type logical_idx) noexcept -> void {
      if (logical_idx + 1 == logical_space_size()) {
        idx_ = end_index();
      } else {
        idx_ = inline_deque_detail::constrained_modulo(
            deque_->front_index() + static_cast<std::size_t>(logical_idx),
            capacity()
        );
      }
    }

    /// @brief Customization point used by @c iterator_interface to increment the iterator by 1 step
    /// @pre @c *this points to a valid, non-empty deque, otherwise the behaviour is undefined
    /// @pre @c *this is not an end iterator, otherwise the behaviour is undefined
    /// @post @c *this is shifted forward to the next valid element, or becomes the end iterator if there are no more
    /// valid elements.
    constexpr auto do_step_forward() noexcept -> void {
      idx_ = inline_deque_detail::step_logical_index_forward(deque_->get_physical_indices(idx_));
    }

    /// @brief Customization point used by @c iterator_interface to decrement the iterator by 1 step
    /// @pre @c *this points to a valid, non-empty deque, otherwise the behaviour is undefined
    /// @pre @c *this does not point to the front element, otherwise the behaviour is undefined
    /// @post @c *this is shifted backward to the next valid element. If @c this is an end iterator before stepping, it
    /// now points to the last (back) element.
    constexpr auto do_step_backward() noexcept -> void {
      idx_ = inline_deque_detail::step_logical_index_backward(deque_->get_physical_indices(idx_));
    }

    // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Use of a friend function with passkey improves encapsulation. It
    // allows data members to remain private and does not unnecessarily expose a public member function."
    // parasoft-begin-suppress AUTOSAR-A0_1_3-a "False positive: Function is public and used in other translation units"
    // parasoft-begin-suppress AUTOSAR-A7_1_1-a	"Declaring 'iter' as reference to const changes semantics"
    // parasoft-begin-suppress AUTOSAR-M7_1_2-c "Declaring 'iter' as reference to const changes semantics"
    // parasoft-begin-suppress AUTOSAR-A8_4_9-a	"Declaring 'iter' as reference to const changes semantics"
    // parasoft-begin-suppress AUTOSAR-A7_1_5-a "False positive: Function uses trailing return type, not auto"

    /// @brief Customization point used by @c iterator_interface to increment the iterator by 1 step
    /// @param self The iterator to be incremented
    /// @pre @c *this is not an end iterator, otherwise the behaviour is undefined
    /// @post @c *this is shifted forward to the next valid element, or becomes the end iterator if there are no more
    /// valid elements.
    /// @note The logic can't be in this function because GCC thinks it can't access private members of @c inline_deque
    friend constexpr auto
    step_forward(inline_container_detail::iterator_interface_tag, cyclical_iterator& self) noexcept -> void {
      self.do_step_forward();
    }

    /// @brief Customization point used by @c iterator_interface to decrement the iterator by 1 step
    /// @param self The iterator to be decremented
    /// @pre @c *this does not point to the front element, otherwise the behaviour is undefined
    /// @post @c *this is shifted backward to the next valid element, or becomes the end iterator if there are no more
    /// valid elements. If @c this is an end iterator, it becomes the last element.
    /// @note The logic can't be in this function because GCC thinks it can't access private members of @c inline_deque
    friend constexpr auto
    step_backward(inline_container_detail::iterator_interface_tag, cyclical_iterator& self) noexcept -> void {
      self.do_step_backward();
    }

    // parasoft-end-suppress AUTOSAR-A11_3_1-a
    // parasoft-end-suppress AUTOSAR-A0_1_3-a
    // parasoft-end-suppress AUTOSAR-A7_1_1-a
    // parasoft-end-suppress AUTOSAR-M7_1_2-c
    // parasoft-end-suppress AUTOSAR-A8_4_9-a
    // parasoft-end-suppress AUTOSAR-A7_1_5-a

    /// @brief A pointer to the inline_deque that the iterator came from so that it can query the current bounds
    conditional_t<IsConst, inline_deque_impl const*, inline_deque_impl*> deque_;

    /// @brief The current index of the iterator *within the physical buffer backing <c>deque_</c>*
    std::size_t idx_;
  };
  // parasoft-end-suppress AUTOSAR-A12_1_5-a

 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False Positive: identifiers at class/namespace scope are exempt"
  /// @brief The type used to hold the size
  using size_type = typename storage_type::size_type;
  /// @brief The type of a value stored
  using value_type = T;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e
  /// @brief The type of a reference to a value stored
  using reference = value_type&;
  /// @brief The type of a const reference to a value stored
  using const_reference = value_type const&;
  /// @brief The type of a mutable iterator
  using iterator = cyclical_iterator<false>;
  /// @brief The type of a const iterator
  using const_iterator = cyclical_iterator<true>;

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "False positive: this is not 'member data', it is a public property"
  /// @brief Get the maximum number of elements in the deque, @c Capacity
  static constexpr std::integral_constant<size_type, Capacity> capacity{};

  /// @brief Whether wrapping is allowed with the current instantiation; if false, wrapping is a precondition violation
  static constexpr std::integral_constant<bool, AllowWrapping> wrapping_allowed{};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a

  // parasoft-begin-suppress AUTOSAR-A12_7_1-a "False positive: range_interface is not default-constructed"
  /// @brief Construct an empty deque
  constexpr inline_deque_impl() noexcept
      : storage_type(),
        range_interface(this) {}
  // parasoft-end-suppress AUTOSAR-A12_7_1-a

  /// @brief Construct a deque from a range of elements, taken to be in front -> back order
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
  constexpr inline_deque_impl(Iterator in_begin, Iterator in_end) noexcept(
      noexcept(::arene::base::distance(in_begin, in_end)) &&
      std::is_nothrow_constructible<value_type, typename std::iterator_traits<Iterator>::reference>::value
  )
      : storage_type(in_begin, in_end),
        range_interface(this) {}

  /// @copydoc storage_type::empty
  using storage_type::empty;

  // parasoft-begin-suppress AUTOSAR-A10_2_1-a "False positive: redefines member of a privately inherited class"
  /// @brief Get an iterator pointing to the front element
  /// @return An iterator which, when dereferenced, yields @c front()
  constexpr auto begin() const noexcept -> const_iterator {
    if (this->empty()) {
      return this->end();
    }
    return {*this, this->front_index()};
  }

  /// @brief Get an iterator pointing to the front element
  /// @return An iterator which, when dereferenced, yields @c front()
  constexpr auto begin() noexcept -> iterator {
    if (this->empty()) {
      return this->end();
    }
    return {*this, this->front_index()};
  }

  /// @brief Get an iterator signifying the end of forward iteration
  /// @return An iterator which will compare equal to an element iterator which has finished its iteration
  constexpr auto end() const noexcept -> const_iterator { return {*this, const_iterator::end_index}; }

  /// @brief Get an iterator signifying the end of forward iteration
  /// @return An iterator which will compare equal to an element iterator which has finished its iteration
  constexpr auto end() noexcept -> iterator { return {*this, iterator::end_index}; }
  // parasoft-end-suppress AUTOSAR-A10_2_1-a

  // All of the functions provided by range_interface conflict with ones provided by storage_type, so we need to
  // explicitly expose the ones that we want from each base class.

  /// @copydoc range_interface::cbegin
  using range_interface::cbegin;
  /// @copydoc range_interface::cend
  using range_interface::cend;

  /// @copydoc range_interface::rbegin
  using range_interface::rbegin;
  /// @copydoc range_interface::rend
  using range_interface::rend;

  /// @copydoc range_interface::crbegin
  using range_interface::crbegin;
  /// @copydoc range_interface::crend
  using range_interface::crend;

  /// @copydoc range_interface::front
  using range_interface::front;
  /// @copydoc range_interface::back
  using range_interface::back;
  /// @copydoc range_interface::operator[]
  using range_interface::operator[];

  /// @brief Get the current number of alive elements
  /// @return the size
  using storage_type::size;

  /// @brief Emplace a new element at the front of the deque
  /// @param value The value to copy or to move from
  /// @pre <c>size() < Capacity</c>, otherwise @c ARENE_PRECONDITION violation
  using storage_type::emplace_front;

  /// @brief Emplace a new element at the back of the deque
  /// @param value The value to copy or to move from
  /// @pre <c>size() < Capacity</c>, otherwise @c ARENE_PRECONDITION violation
  using storage_type::emplace_back;

  /// @brief Copy a new element on to the front of the deque
  /// @param arg The argument to copy from
  /// @pre If wrapping is not allowed, <c>size() < capacity()</c>, otherwise @c ARENE_PRECONDITION violation
  /// @note If wrapping is allowed and <c>size() == capacity()</c>, the element at @c back() is removed and iterators
  /// pointing to it are invalidated.
  constexpr auto push_front(value_type const& arg) noexcept(std::is_nothrow_copy_constructible<value_type>::value)
      -> void {
    std::ignore = this->emplace_front(arg);
  }

  /// @brief Move a new element on to the front of the deque
  /// @param arg The argument to move from
  /// @pre If wrapping is not allowed, <c>size() < capacity()</c>, otherwise @c ARENE_PRECONDITION violation
  /// @note If wrapping is allowed and <c>size() == capacity()</c>, the element at @c back() is removed and iterators
  /// pointing to it are invalidated.
  constexpr auto push_front(value_type&& arg) noexcept(std::is_nothrow_move_constructible<value_type>::value) -> void {
    std::ignore = this->emplace_front(std::move(arg));
  }

  /// @brief Copy a new element on to the back of the deque
  /// @param arg The argument to copy from
  /// @pre If wrapping is not allowed, <c>size() < capacity()</c>, otherwise @c ARENE_PRECONDITION violation
  /// @note If wrapping is allowed and <c>size() == capacity()</c>, the element at @c front() is removed and iterators
  /// pointing to it are invalidated.
  constexpr auto push_back(value_type const& arg) noexcept(std::is_nothrow_copy_constructible<value_type>::value)
      -> void {
    std::ignore = this->emplace_back(arg);
  }

  /// @brief Move a new element on to the back of the deque
  /// @param arg The argument to move from
  /// @pre If wrapping is not allowed, <c>size() < capacity()</c>, otherwise @c ARENE_PRECONDITION violation
  /// @note If wrapping is allowed and <c>size() == capacity()</c>, the element at @c front() is removed and iterators
  /// pointing to it are invalidated.
  constexpr auto push_back(value_type&& arg) noexcept(std::is_nothrow_move_constructible<value_type>::value) -> void {
    std::ignore = this->emplace_back(std::move(arg));
  }

 private:
  /// @brief Perform an insertion of a single value at the front of the deque as part of an @c insert call
  /// @tparam U Type of the value to insert
  /// @tparam Wrap Whether or not wrapping is allowed; redefined here to delay evaluation for SFINAE
  /// @param value Value to insert
  /// @return A new iterator pointing to the inserted value if it was inserted; otherwise, @c begin()
  /// @pre The deque is not currently full or wrapping is allowed, otherwise @c ARENE_PRECONDITION violation
  /// @post @c value is inserted before the beginning, unless the queue is full and wrapping is allowed; in that case,
  /// @c value is not inserted. Logically, it is inserted at the front and then immediately popped for space.
  template <typename U, bool Wrap = wrapping_allowed, constraints<std::enable_if_t<Wrap>> = nullptr>
  constexpr auto internal_insert_at_front(U&& value) noexcept(std::is_nothrow_constructible<value_type, U&&>::value)
      -> inline_deque_impl::iterator {
    if (this->size() < this->capacity()) {
      std::ignore = this->emplace_front(std::forward<U>(value));
    }

    return this->begin();
  }

  /// @brief Perform an insertion of a single value at the front of the deque as part of an @c insert call
  /// @tparam U Type of the value to insert
  /// @tparam Wrap Whether or not wrapping is allowed; redefined here to delay evaluation for SFINAE
  /// @param value Value to insert
  /// @return A new iterator pointing to the inserted value if it was inserted; otherwise, @c begin()
  /// @pre The deque is not currently full or wrapping is allowed, otherwise @c ARENE_PRECONDITION violation
  /// @post @c value is inserted before the beginning, unless the queue is full and wrapping is allowed; in that case,
  /// @c value is not inserted. Logically, it is inserted at the front and then immediately popped for space.
  template <typename U, bool Wrap = wrapping_allowed, constraints<std::enable_if_t<!Wrap>> = nullptr>
  constexpr auto internal_insert_at_front(U&& value) noexcept(std::is_nothrow_constructible<value_type, U&&>::value)
      -> inline_deque_impl::iterator {
    std::ignore = this->emplace_front(std::forward<U>(value));
    return this->begin();
  }

  /// @brief Perform an insertion of a single value at the back of the deque as part of an @c insert call
  /// @tparam U Type of the value to insert
  /// @param value Value to insert
  /// @return A new iterator pointing to the inserted value
  /// @pre Either the deque is not currently full or wrapping is allowed, otherwise @c ARENE_PRECONDITION violation
  template <typename U>
  constexpr auto internal_insert_at_back(U&& value) noexcept(std::is_nothrow_constructible<value_type, U&&>::value)
      -> inline_deque_impl::iterator {
    // If wrapping is allowed, this will automatically pop the front element to make space; otherwise it crashes.
    std::ignore = this->emplace_back(std::forward<U>(value));
    return inline_deque_impl::iterator(*this, this->back_index());
  }

  /// @brief Perform an insertion of a single value in the middle of the deque as part of an @c insert call
  /// @tparam U Type of the value to insert
  /// @param pos Position at which to insert
  /// @param value Value to insert
  /// @return A new iterator pointing to the inserted value
  /// @pre Either the deque is not currently full or wrapping is allowed, otherwise @c ARENE_PRECONDITION violation
  /// @pre @c pos is an iterator pointing to a valid position in this deque, otherwise the behaviour is undefined
  /// @post If the return value is <c>r</c>, then @c *(++r) refers to the element previously referred to by @c pos
  template <typename U>
  constexpr auto internal_insert_in_middle(inline_deque_impl::const_iterator pos, U&& value) noexcept(
      std::is_nothrow_constructible<value_type, U&&>::value && std::is_nothrow_move_assignable<value_type>::value &&
      std::is_nothrow_move_constructible<value_type>::value
  ) -> inline_deque_impl::iterator {
    // Even if elements are not actually inserted at the back, this has the right behaviour for making space/crashing.
    this->ensure_space_at_back();

    // parasoft-begin-suppress AUTOSAR-M14_6_1-a "False positive: inline_deque_impl::iterator is a qualified-id"
    auto mutable_pos = inline_deque_impl::iterator(*this, pos.physical_index());
    inline_deque_impl::iterator insertion_target;
    // parasoft-end-suppress AUTOSAR-M14_6_1-a

    // parasoft-begin-suppress AUTOSAR-A5_1_1-a "Simple arithmetic constants permitted by A5-1-1 Permit #1"

    // Shift the physical storage toward whichever side is closer; the final logical order is the same either way.
    if (static_cast<size_type>(distance(this->cbegin(), pos)) < (this->capacity() / 2U)) {
      // The front is closer, so add an element at the front and shift the physical storage toward it.
      std::ignore = this->emplace_front(std::forward<U>(value));

      insertion_target = rotate(this->begin(), next(this->begin()), mutable_pos);
    } else {
      // The back is closer, so add an element at the back and shift the physical storage toward it.
      std::ignore = this->emplace_back(std::forward<U>(value));

      std::ignore = rotate(mutable_pos, inline_deque_impl::iterator(*this, this->back_index()), this->end());
      insertion_target = mutable_pos;
    }

    // parasoft-end-suppress AUTOSAR-A5_1_1-a

    return insertion_target;
  }

  /// @brief Perform an insertion of a single value at the indicated position
  /// @tparam U Type of the value to insert
  /// @param pos Position at which to insert
  /// @param value Value to insert
  /// @return A new iterator pointing to the inserted value if it was inserted; otherwise, @c begin()
  /// @pre If wrapping is disabled: the queue is not full, otherwise @c ARENE_PRECONDITION violation
  /// @pre @c pos is an iterator pointing to a valid position in this deque, otherwise the behaviour is undefined
  /// @post If wrapping is enabled, the queue is full, and @c pos is <c>begin</c>, then no insertion is performed.
  /// Otherwise, let the return value be <c>r</c>; then @c *r holds @c value and @c *(++r) refers to the element
  /// previously referred to by @c pos
  /// @note All existing iterators are invalidated if an insertion is performed.
  template <typename U>
  constexpr auto internal_insert(inline_deque_impl::const_iterator pos, U&& value) noexcept(noexcept(
      std::declval<inline_deque_impl>().internal_insert_at_front(std::forward<U>(value))
  ) && noexcept(std::declval<inline_deque_impl>().internal_insert_at_back(std::forward<U>(value))
  ) && noexcept(std::declval<inline_deque_impl>().internal_insert_in_middle(pos, std::forward<U>(value))))
      -> inline_deque_impl::iterator {
    if (pos == this->cbegin()) {
      return internal_insert_at_front(std::forward<U>(value));
    }
    if (pos == this->cend()) {
      return internal_insert_at_back(std::forward<U>(value));
    }

    return internal_insert_in_middle(pos, std::forward<U>(value));
  }

 public:
  /// @brief Copy-insert a single value at the indicated position
  /// @param pos Position at which to insert
  /// @param value Value to insert
  /// @return A new iterator pointing to the inserted value if it was inserted; otherwise, @c begin()
  /// @pre If wrapping is disabled: the queue is not full, otherwise @c ARENE_PRECONDITION violation
  /// @pre @c pos is an iterator pointing to a valid position in this deque, otherwise the behaviour is undefined
  /// @post If wrapping is enabled, the queue was full prior to this call, and @c pos is <c>begin()</c>, then no
  /// insertion is performed. Otherwise, let the return value be <c>r</c>; then @c *r holds @c value and @c *(++r)
  /// refers to the element previously referred to by <c>pos</c>.
  /// @note All existing iterators are invalidated if an insertion is performed.
  /// @note Complexity: O(x), where x is the distance from @c pos to the nearer of @c begin() or <c>end()</c>. This
  /// means that inserting at the beginning or end is constant complexity regardless of current size.
  template <
      typename U = value_type,
      constraints<
          std::enable_if_t<std::is_same<U, value_type>::value>,
          std::enable_if_t<std::is_copy_assignable<U>::value>,
          std::enable_if_t<std::is_move_assignable<U>::value>,
          std::enable_if_t<std::is_move_constructible<U>::value>> = nullptr>
  constexpr auto insert(inline_deque_impl::const_iterator pos, value_type const& value) noexcept(
      noexcept(std::declval<inline_deque_impl>().internal_insert(pos, value))
  ) -> inline_deque_impl::iterator {
    return this->internal_insert(pos, value);
  }

  /// @brief Move-insert a single value at the indicated position
  /// @param pos Position at which to insert
  /// @param value Value to insert
  /// @return A new iterator pointing to the inserted value if it was inserted; otherwise, @c begin()
  /// @pre If wrapping is disabled: the queue is not full, otherwise @c ARENE_PRECONDITION violation
  /// @pre @c pos is an iterator pointing to a valid position in this deque, otherwise the behaviour is undefined
  /// @post If wrapping is enabled, the queue was full prior to this call, and @c pos is <c>begin()</c>, then no
  /// insertion is performed. Otherwise, let the return value be <c>r</c>; then @c *r holds @c value and @c *(++r)
  /// refers to the element previously referred to by <c>pos</c>.
  /// @note All existing iterators are invalidated if an insertion is performed.
  /// @note Complexity: O(x), where x is the distance from @c pos to the nearer of @c begin() or <c>end()</c>. This
  /// means that inserting at the beginning or end is constant complexity regardless of current size.
  template <
      typename U = value_type,
      constraints<
          std::enable_if_t<std::is_same<U, value_type>::value>,
          std::enable_if_t<std::is_move_assignable<U>::value>,
          std::enable_if_t<std::is_move_constructible<U>::value>> = nullptr>
  constexpr auto insert(inline_deque_impl::const_iterator pos, value_type&& value) noexcept(
      noexcept(std::declval<inline_deque_impl>().internal_insert(pos, std::move(value)))
  ) -> inline_deque_impl::iterator {
    return this->internal_insert(pos, std::move(value));
  }

  /// @brief Remove the element at the front
  /// @pre <c>empty() == false</c>, otherwise @c ARENE_PRECONDITION violation
  using storage_type::pop_front;

  /// @brief Remove the element at the back
  /// @pre <c>empty() == false</c>, otherwise @c ARENE_PRECONDITION violation
  using storage_type::pop_back;
};
// parasoft-end-suppress AUTOSAR-A10_1_1-a

template <typename T, std::size_t Capacity, bool AllowWrapping>
constexpr std::integral_constant<typename inline_deque_impl<T, Capacity, AllowWrapping>::size_type, Capacity>
    inline_deque_impl<T, Capacity, AllowWrapping>::capacity;

template <typename T, std::size_t Capacity, bool AllowWrapping>
constexpr std::integral_constant<bool, AllowWrapping> inline_deque_impl<T, Capacity, AllowWrapping>::wrapping_allowed;

template <typename T, std::size_t Capacity, bool AllowWrapping>
template <bool IsConst>
constexpr std::integral_constant<std::size_t, std::numeric_limits<std::size_t>::max()>
    inline_deque_impl<T, Capacity, AllowWrapping>::cyclical_iterator<IsConst>::end_index;

}  // namespace inline_deque_detail

/// @brief A container with fixed capacity featuring constant time insertion and
/// removal at opposite ends; i.e., with an interface similar to std::deque. The
/// storage for the elements is held directly within the class. Any attempt to
/// store more than @c Capacity elements is an @c ARENE_PRECONDITION violation
/// @tparam T The type of each element
/// @tparam Capacity The maximum number of elements that can be stored
// parasoft-begin-suppress AUTOSAR-A10_1_1-a "False positive: try_construct_interface is an interface"
template <typename T, std::size_t Capacity>
class inline_deque
    : private inline_deque_detail::inline_deque_impl<T, Capacity, false>
    , public inline_container_detail::try_construct_interface<
          inline_deque<T, Capacity>,
          typename inline_deque_detail::inline_deque_try_construct_policy<T, Capacity>> {
  /// @brief The implementation class, separate to share implementations with other similar queues
  using impl = inline_deque_detail::inline_deque_impl<T, Capacity, false>;

  /// @brief Shorthand to the mixin type that synthesizes @c try_construct functions based on the normal constructors
  using try_construct_interface = inline_container_detail::try_construct_interface<
      inline_deque,
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

  // parasoft-begin-suppress AUTOSAR-A12_7_1-a "False positive: can not be defaulted by it needs to pass this pointer"
  /// @brief Construct an empty queue
  constexpr inline_deque() noexcept
      : impl(),
        try_construct_interface(this) {}
  // parasoft-end-suppress AUTOSAR-A12_7_1-a

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
  constexpr inline_deque(Iterator in_begin, Iterator in_end) noexcept(
      noexcept(::arene::base::distance(in_begin, in_end)) &&
      std::is_nothrow_constructible<value_type, typename std::iterator_traits<Iterator>::reference>::value
  )
      : impl(in_begin, in_end),
        try_construct_interface(this) {}

  /// @brief Construct a deque by copying from an initializer list of elements, taken to be in front -> back order
  /// @param init An initializer list of the value type
  /// @pre <c>init.size() <= capacity()</c>, otherwise @c ARENE_PRECONDITION violation
  template <typename U = value_type, constraints<std::enable_if_t<std::is_copy_constructible<U>::value>> = nullptr>
  constexpr inline_deque(std::initializer_list<value_type> init
  ) noexcept(noexcept(inline_deque(init.begin(), init.end())))
      : inline_deque(init.begin(), init.end()) {}

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
  /// @copydoc impl::emplace_front
  using impl::emplace_front;
  /// @copydoc impl::pop_back
  using impl::pop_back;
  /// @copydoc impl::pop_front
  using impl::pop_front;
  /// @copydoc impl::push_back
  using impl::push_back;
  /// @copydoc impl::push_front
  using impl::push_front;
  /// @copydoc impl::insert
  using impl::insert;
};
// parasoft-end-suppress AUTOSAR-A10_1_1-a

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M2_10_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DEQUE_HPP_
