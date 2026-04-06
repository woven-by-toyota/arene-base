// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_VECTOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_VECTOR_HPP_

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/equal.hpp"
#include "arene/base/algorithm/lexicographical_compare.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/compare/operators.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/detail/exceptions.hpp"
#include "arene/base/detail/wrapped_iterator.hpp"  // IWYU pragma: keep
#include "arene/base/inline_container/detail/unsafe_element_storage.hpp"
#include "arene/base/iterator/distance.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/iterator/reverse_iterator.hpp"
#include "arene/base/memory/construct_at.hpp"
#include "arene/base/optional/optional.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_copy_assignable.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_destructible.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/stdlib_choice/min_value_overload.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/move_iterator.hpp"
#include "arene/base/type_manipulation/non_constructible_dummy.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/denotes_range.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "arene/base/utility/in_place.hpp"
#include "arene/base/utility/make_subrange.hpp"
#include "arene/base/utility/swap.hpp"

// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress CERT_CPP-ERR55-a-2 "False positive: Exception specification is conditional"
// parasoft-begin-suppress CERT_CPP-ERR50-h-3 "False positive: Exception specification is conditional"
// parasoft-begin-suppress CERT_CPP-ERR50-g-3 "False positive: Exception specification is conditional"
// parasoft-begin-suppress CERT_CPP-ERR51-b-3 "False positive: Exception specification is conditional"
// parasoft-begin-suppress AUTOSAR-A15_3_2-a-2 "False positive: Exception specification is conditional"
// parasoft-begin-suppress AUTOSAR-A15_5_3-g-2 "False positive: Exception specification is conditional"
// parasoft-begin-suppress AUTOSAR-A15_5_3-h-2 "False positive: Exception specification is conditional"
// parasoft-begin-suppress AUTOSAR-M15_3_4-b-2 "False positive: Exception is caught"
// parasoft-begin-suppress AUTOSAR-A15_2_1-b-2 "False positive: Throwing constructor not invoked"
// parasoft-begin-suppress CERT_CPP-ERR50-b-3 "False positive: Called functions are noexcept"
// parasoft-begin-suppress AUTOSAR-A15_5_1-a-2 "False positive: Called functions are noexcept"
// parasoft-begin-suppress AUTOSAR-A15_5_3-b-2 "False positive: Called functions are noexcept"
// parasoft-begin-suppress CERT_CPP-DCL57-a-2 "False positive: Called functions are noexcept"

namespace arene {
namespace base {

/// @brief A container similar to @c std::vector<T> that has a fixed capacity. The
/// storage for the elements is held directly within the class. Any attempt to
/// store more than @c Capacity elements will throw @c std::length_error
/// @tparam T The type of each element
/// @tparam Capacity The maximum number of elements that can be stored.
template <typename T, std::size_t Capacity>
class inline_vector;

namespace inline_vector_detail {

/// @brief implementation details for @c inline_vector which depend on neither the capacity nor the storage mechanism.
template <typename T>
class vector_base {
 protected:
  /// @brief Tag type used to protect construction of iterators.
  class iterator_passkey {
   public:
    /// @brief The default constructor is explicit to disable construction with just @c {}
    constexpr explicit iterator_passkey() noexcept = default;
  };

 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False Positive: These identifiers do not hide anything"
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False Positive: These identifiers do not hide anything"
  /// @brief The type used for the size of the vector.
  using size_type = std::size_t;
  /// @brief The type of a value stored in the vector
  using value_type = T;
  /// @brief The type of a reference to a value stored in the vector
  using reference = value_type&;
  /// @brief The type of a const reference to a value stored in the vector
  using const_reference = value_type const&;
  /// @brief The type of a pointer to an element stored in the container
  using pointer = value_type*;
  /// @brief The type of a const pointer to an element stored in the container
  using const_pointer = value_type const*;
  /// @brief The type of an iterator to a position in the vector
  using iterator = ::arene::base::detail::wrapped_iterator<pointer, iterator_passkey>;
  /// @brief The type of a const iterator to a position in the vector
  using const_iterator = ::arene::base::detail::wrapped_iterator<const_pointer, iterator_passkey>;
  /// @brief The type of a reverse-iterator to a position in the vector
  using reverse_iterator = ::arene::base::reverse_iterator<iterator>;
  /// @brief The type of a const reverse-iterator to a position in the vector
  using const_reverse_iterator = ::arene::base::reverse_iterator<const_iterator>;
  /// @brief The type of the difference between two iterators
  using difference_type = typename iterator::difference_type;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e
  // parasoft-end-suppress AUTOSAR-A2_10_1-d
};

/// @brief Type trait to check if we can construct a vector of @c T in c constexpr
/// contexts. The result is @c true if we can, @c false otherwise.
/// @tparam T the type being checked.
template <typename T>
constexpr bool allow_constexpr_vector_v =
    std::is_trivially_default_constructible<T>::value && std::is_trivially_move_assignable<T>::value;

/// @brief Base class for @c inline_vector that holds the elements when the elements aren't usable in @c constexpr
/// contexts.
/// @tparam T The type of the elements
/// @tparam Capacity The capacity of the vector
template <typename T, std::size_t Capacity, bool = std::is_trivially_destructible<T>::value>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class inline_vector_internal_non_constexpr_storage {
 public:
  /// @brief Deleted copy constructor
  inline_vector_internal_non_constexpr_storage(inline_vector_internal_non_constexpr_storage const&) = delete;
  /// @brief Deleted move constructor
  inline_vector_internal_non_constexpr_storage(inline_vector_internal_non_constexpr_storage&&) = delete;

  /// @brief Deleted copy assignment
  auto operator=(inline_vector_internal_non_constexpr_storage const&)
      -> inline_vector_internal_non_constexpr_storage& = delete;
  /// @brief Deleted move assignment
  auto operator=(inline_vector_internal_non_constexpr_storage&&)
      -> inline_vector_internal_non_constexpr_storage& = delete;

 protected:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: 'size_type' does not hide anything"
  /// @brief The type used to hold the size
  using size_type = std::size_t;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: 'value_type' does not hide anything"
  /// @brief The type of the elements
  using value_type = T;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d

  /// @brief Construct the storage with no elements
  constexpr inline_vector_internal_non_constexpr_storage() = default;

  /// @brief Destroy all constructed elements.
  ~inline_vector_internal_non_constexpr_storage() {
    // parasoft-begin-suppress AUTOSAR-M0_1_3-b-2 "False positive: necessary for invoking the destructors"
    for (auto& value : values()) {
      value.~T();
    }
    // parasoft-end-suppress AUTOSAR-M0_1_3-b-2
  }

  /// @brief Get a span over the current elements
  /// @return The span
  constexpr auto values() noexcept -> span<value_type> {
    // We specify Capacity as a maximum valid value to eliminate the
    // precondition in base::span constructor.
    return {&values_.front().object, std::min(size_, Capacity)};
  }

  /// @brief Get a @c const span over the current elements
  /// @return The span
  constexpr auto values() const noexcept -> span<value_type const> {
    // We specify Capacity as a maximum valid value to eliminate the
    // precondition in base::span constructor.
    return {&values_.front().object, std::min(size_, Capacity)};
  }

  /// @brief If the current size is more than @c new_size, reduce the size to @c new_size.
  /// @param new_size The new size limit
  constexpr void shrink_to(size_type new_size) noexcept {
    if (new_size < this->size_) {
      // parasoft-begin-suppress AUTOSAR-M0_1_3-b-2 "False positive: necessary for invoking the destructors"
      for (auto& value : values().last(size_ - new_size)) {
        value.~T();
      }
      // parasoft-end-suppress AUTOSAR-M0_1_3-b-2
      size_ = new_size;
    }
  }

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "Protected member to allow access by derived class"
  /// @brief The current size
  // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
  size_type size_{0U};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a

  /// @brief underlying element type that allows a value to exist or not exist
  using element_storage = inline_container::detail::unsafe_element_storage<T>;

  // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
  // side-effects"
  static_assert(alignof(element_storage) == alignof(T), "Alignment must be the same");
  static_assert(sizeof(element_storage) == sizeof(T), "Size must be the same");
  // parasoft-end-suppress CERT_C-PRE31-c-3

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "Protected member to allow access by derived class"
  // parasoft-begin-suppress AUTOSAR-M8_5_2-b-2 "False Positive: all elements default-initialized"
  /// @brief The storage for the elements
  // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
  arene::base::array<element_storage, Capacity> values_{};
  // parasoft-end-suppress AUTOSAR-M8_5_2-b-2
  // parasoft-end-suppress AUTOSAR-M11_0_1-a
};

/// @brief Base class for @c inline_vector that holds the elements when the elements aren't usable in @c constexpr
/// contexts.
/// @tparam T The type of the elements
/// @tparam Capacity The capacity of the vector
///
/// This specialization handles the case that the elements are trivially destructible
template <typename T, std::size_t Capacity>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class inline_vector_internal_non_constexpr_storage<T, Capacity, true> {
 protected:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: 'size_type' does not hide anything"
  /// @brief The type used to hold the size
  using size_type = std::size_t;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: 'value_type' does not hide anything"
  /// @brief The type of the elements
  using value_type = T;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d

  /// @brief Get a span over the current elements
  /// @return The span
  constexpr auto values() noexcept -> span<value_type> {
    // We specify Capacity as a maximum valid value to eliminate the
    // precondition in base::span constructor.
    return {&values_.front().object, std::min(size_, Capacity)};
  }

  /// @brief Get a @c const span over the current elements
  /// @return The span
  constexpr auto values() const noexcept -> span<value_type const> {
    // We specify Capacity as a maximum valid value to eliminate the
    // precondition in base::span constructor.
    return {&values_.front().object, std::min(size_, Capacity)};
  }

  /// @brief If the current size is more than @c new_size, reduce the size to @c new_size.
  /// @param new_size The new size limit
  constexpr void shrink_to(size_type new_size) noexcept {
    if (new_size < this->size_) {
      size_ = new_size;
    }
  }

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "Protected member to allow access by derived class"
  /// @brief The current size
  // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
  size_type size_{0U};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a

  /// @brief underlying element type that allows a value to exist or not exist
  using element_storage = inline_container::detail::unsafe_element_storage<T>;

  // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
  // side-effects"
  static_assert(alignof(element_storage) == alignof(T), "Alignment must be the same");
  static_assert(sizeof(element_storage) == sizeof(T), "Size must be the same");
  // parasoft-end-suppress CERT_C-PRE31-c-3

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "Protected member to allow access by derived class"
  // parasoft-begin-suppress AUTOSAR-M8_5_2-b-2 "False Positive: all elements default-initialized"
  /// @brief The storage for the elements
  // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
  arene::base::array<element_storage, Capacity> values_{};
  // parasoft-end-suppress AUTOSAR-M8_5_2-b-2
  // parasoft-end-suppress AUTOSAR-M11_0_1-a
};

// parasoft-begin-suppress CERT_CPP-MEM51-c-1 "False Positive: Copy assignment is defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-M14_5_3-a-2 "False Positive: Copy assignment is defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-A14_5_1-a-2 "False Positive: Copy constructor is defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-A1_1_1-c-2 "False Positive: Copy operations are defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-A1_1_1-b-2 "False Positive: Copy operations are defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-A12_0_1-a-2 "False Positive: Copy operations are defined or deleted as appropriate"
/// @brief Base class for @c inline_vector that holds the elements.
/// @tparam T The type of the elements
/// @tparam Capacity The capacity of the vector
template <typename T, std::size_t Capacity, bool = Capacity != 0 && allow_constexpr_vector_v<T>>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class inline_vector_internal_storage : public inline_vector_internal_non_constexpr_storage<T, Capacity> {
  /// @brief The base class with the actual storage
  using non_constexpr_storage_base = inline_vector_internal_non_constexpr_storage<T, Capacity>;

  /// @brief The type of the argument to the "copy constructor": either @c
  /// inline_vector_internal_storage to provide a real copy constructor if @c T is
  /// copy-constructible, or @c non_constructible_dummy otherwise.
  using copy_constructor_arg =
      std::conditional_t<std::is_copy_constructible<T>::value, inline_vector_internal_storage, non_constructible_dummy>;

  /// @brief The type of the argument to the "copy assignment operator": either @c
  /// inline_vector_internal_storage to provide a real copy assignment operator if @c T is
  /// copy-assignable, or @c non_constructible_dummy otherwise.
  using copy_assignment_arg = std::conditional_t<
      std::is_copy_constructible<T>::value && std::is_copy_assignable<T>::value,
      inline_vector_internal_storage,
      non_constructible_dummy>;

  /// @brief The type of the argument to the "move constructor": either @c
  /// inline_vector_internal_storage to provide a real move constructor if @c T is
  /// move-constructible, or @c non_constructible_dummy otherwise.
  using move_constructor_arg =
      std::conditional_t<std::is_move_constructible<T>::value, inline_vector_internal_storage, non_constructible_dummy>;

  /// @brief The type of the argument to the "move assignment operator": either @c
  /// inline_vector_internal_storage to provide a real move assignment operator if @c T is
  /// move-assignable, or @c non_constructible_dummy otherwise.
  using move_assignment_arg = std::conditional_t<
      std::is_move_constructible<T>::value && std::is_move_assignable<T>::value,
      inline_vector_internal_storage,
      non_constructible_dummy>;

 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: 'size_type' does not hide anything"
  /// @brief The type used to hold the size
  using size_type = typename non_constexpr_storage_base::size_type;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: 'value_type' does not hide anything"
  /// @brief The type of the elements
  using value_type = typename non_constexpr_storage_base::value_type;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property,
  /// @brief Get the maximum number of elements in the vector, @c Capacity
  static constexpr std::integral_constant<std::size_t, Capacity> capacity{};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property,
  /// @brief Get the maximum number of elements in the vector, @c Capacity
  static constexpr std::integral_constant<std::size_t, Capacity> max_size{};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2

  /// @brief Construct the storage with no elements
  constexpr inline_vector_internal_storage() = default;

  // parasoft-begin-suppress AUTOSAR-A12_8_6-a-2 "Not guaranteed to be a base class"
  /// @brief Copy constructor if @c T is copy-constructible, useless conversion from @c
  /// non_constructible_dummy otherwise. If copying, copies all the elements
  /// from @c other into @c *this.
  /// @param other The source
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  constexpr inline_vector_internal_storage(copy_constructor_arg const& other
  ) noexcept(std::is_nothrow_copy_constructible<T>::value)
      : non_constexpr_storage_base{} {
    for (auto& value : other.values()) {
      internal_emplace_back(value);
    }
  }
  // parasoft-end-suppress AUTOSAR-A12_8_6-a-2

  // parasoft-begin-suppress AUTOSAR-A15_5_1-b-2 "False positive: Conditionally noxecept"
  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "False positive: Elements are moved"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "False positive: Elements are moved"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: Elements are moved"
  /// @brief Move constructor if @c T is move-constructible, useless conversion from @c
  /// non_constructible_dummy otherwise. If moving, move-constructs all the
  /// elements from @c other into @c *this.
  /// @param other The source
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  constexpr inline_vector_internal_storage(move_constructor_arg&& other
  ) noexcept(std::is_nothrow_move_constructible<T>::value)
      : non_constexpr_storage_base{} {
    for (auto& value : other.values()) {
      internal_emplace_back(std::move(value));
    }
    other.shrink_to(0U);
  }
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2
  // parasoft-end-suppress AUTOSAR-A15_5_1-b-2

  // parasoft-begin-suppress AUTOSAR-A12_8_6-a-2 "Not guaranteed to be a base class"
  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "False positive: Elements are moved"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "False positive: Elements are moved"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: Elements are moved"
  /// @brief Copy assignment operator if @c T is copy-constructible and copy
  /// assignable, useless conversion from @c non_constructible_dummy otherwise.
  /// If copying, copies all the elements from @c other into @c *this, assigning
  /// over existing elements, constructing new ones, and destroying an excess
  /// existing elements.
  /// @param other The source
  /// @return @c *this
  constexpr auto operator=(copy_assignment_arg const& other
  ) noexcept(std::is_nothrow_copy_constructible<T>::value && std::is_nothrow_copy_assignable<T>::value)
      -> inline_vector_internal_storage& {
    if (&other != this) {
      auto our_values = this->values();
      auto other_values = other.values();
      inline_vector_internal_storage::size_type idx{0U};
      for (; (idx < our_values.size()) && (idx < other_values.size()); ++idx) {
        our_values[idx] = other_values[idx];
      }
      for (; idx < other_values.size(); ++idx) {
        internal_emplace_back(other_values[idx]);
      }
      this->shrink_to(idx);
    }
    return *this;
  }
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_6-a-2

  // parasoft-begin-suppress AUTOSAR-A15_5_1-b-2 "False positive: Conditionally noxecept"
  // parasoft-begin-suppress AUTOSAR-A12_8_6-a-2 "Not guaranteed to be a base class"
  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "False positive: Elements are moved"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "False positive: Elements are moved"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: Elements are moved"
  /// @brief Move assignment operator if @c T is move-constructible and move
  /// assignable, useless conversion from @c non_constructible_dummy otherwise.
  /// If moving, copies all the elements from @c other into @c *this, assigning
  /// over existing elements, constructing new ones, and destroying an excess
  /// existing elements.
  /// @param other The source
  /// @return @c *this
  constexpr auto operator=(move_assignment_arg&& other
  ) noexcept(std::is_nothrow_move_constructible<T>::value && std::is_nothrow_move_assignable<T>::value)
      -> inline_vector_internal_storage& {
    if (&other != this) {
      auto our_values = this->values();
      auto other_values = other.values();
      inline_vector_internal_storage::size_type idx{0U};
      for (; (idx < our_values.size()) && (idx < other_values.size()); ++idx) {
        our_values[idx] = std::move(other_values[idx]);
      }
      for (; idx < other_values.size(); ++idx) {
        internal_emplace_back(std::move(other_values[idx]));
      }
      this->shrink_to(idx);
      other.shrink_to(0U);
    }
    return *this;
  }
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_6-a-2
  // parasoft-end-suppress AUTOSAR-A15_5_1-b-2
  /// @brief Destroy all constructed elements.
  ~inline_vector_internal_storage() = default;

  /// @brief Construct a new element at the end of the existing range, from the
  /// supplied arguments.
  /// @tparam Args The types of the arguments
  /// @param args The constructor arguments
  template <typename... Args>
  constexpr void internal_emplace_back(Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value) {
    construct_at(&this->values_[this->size_].object, std::forward<Args>(args)...);
    ++this->size_;
  }
};
// parasoft-end-suppress AUTOSAR-A12_0_1-a-2
// parasoft-end-suppress AUTOSAR-A1_1_1-c-2
// parasoft-end-suppress AUTOSAR-A1_1_1-b-2
// parasoft-end-suppress AUTOSAR-M14_5_3-a-2
// parasoft-end-suppress AUTOSAR-A14_5_1-a-2

/// @brief Specialization for the case that the element @c T is suitable for use in a
/// @c constexpr container. The elements are held as a default-constructed array
/// of @c T rather than an array of storage entries.
/// @tparam T The type of the elements
/// @tparam Capacity The number of elements that can be stored
template <typename T, std::size_t Capacity>
class inline_vector_internal_storage<T, Capacity, true> {
 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: 'size_type' does not hide anything"
  /// @brief The type used to hold the size
  using size_type = std::size_t;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: 'value_type' does not hide anything"
  /// @brief The type of the elements
  using value_type = T;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property,
  /// @brief Get the maximum number of elements in the vector, @c Capacity
  static constexpr std::integral_constant<size_type, Capacity> capacity{};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property,
  /// @brief Get the maximum number of elements in the vector, @c Capacity
  static constexpr std::integral_constant<size_type, Capacity> max_size{};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2

  /// @brief Get a span of the current "live" values
  /// @return The span
  constexpr auto values() noexcept -> span<value_type> {
    // We specify Capacity as a maximum valid value to eliminate the
    // precondition in base::span constructor.
    return {&values_.front(), std::min(size_, Capacity)};
  }
  /// @brief Get a @c const span of the current "live" values
  /// @return The span
  constexpr auto values() const noexcept -> span<value_type const> {
    // We specify Capacity as a maximum valid value to eliminate the
    // precondition in base::span constructor.
    return {&values_.front(), std::min(size_, Capacity)};
  }

  /// @brief Construct a new element at the end of the existing range, from the
  /// supplied arguments.
  /// @tparam Args The types of the arguments
  /// @param args The constructor arguments
  template <typename... Args>
  constexpr void internal_emplace_back(Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value) {
    // parasoft-begin-suppress AUTOSAR-A5_2_2-a-2 "False positive: constructor call, not C-style cast"
    values_[size_] = T(std::forward<Args>(args)...);
    // parasoft-end-suppress AUTOSAR-A5_2_2-a-2
    ++size_;
  }

  /// @brief If the current size is more than @c new_size, reduce the size to @c new_size.
  /// @param new_size The new size limit
  constexpr void shrink_to(size_type new_size) noexcept {
    if (new_size < size_) {
      size_ = new_size;
    }
  }

 private:
  /// @brief The current size
  size_type size_{0U};

  /// @brief The elements
  arene::base::array<value_type, Capacity> values_{};
};
// parasoft-end-suppress CERT_CPP-MEM51-c-1

/// @brief Specialization for the case that the @c Capacity is zero: such a vector can
/// never hold any elements, so has a size of zero and is an empty class.
/// @tparam T The element type
template <typename T>
class inline_vector_internal_storage<T, 0, false> {
 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: 'size_type' does not hide anything"
  /// @brief The type used to hold the size
  using size_type = std::size_t;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: 'value_type' does not hide anything"
  /// @brief The type of the elements
  using value_type = T;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property,
  /// @brief Get the maximum number of elements in the vector, @c Capacity
  static constexpr std::integral_constant<size_type, 0> capacity{};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property,
  /// @brief Get the maximum number of elements in the vector, @c Capacity
  static constexpr std::integral_constant<size_type, 0> max_size{};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2

  /// @brief Get an empty span for the current elements
  /// @return The span
  static constexpr auto values() noexcept -> span<value_type> { return {}; }
  /// @brief Do-nothing placeholder for constructing a new element
  /// @tparam Args The types of the initializers
  template <typename... Args>
  static constexpr void internal_emplace_back(Args&&...) noexcept {
    ARENE_INVARIANT_UNREACHABLE("Can never add to a zero-size container");
  }
  /// @brief Do-nothing placeholder for shrinking the size
  /// @param new_size The new size to shrink to
  static constexpr void shrink_to(size_type new_size) noexcept { ARENE_PRECONDITION(new_size == 0U); }
};

template <typename T, std::size_t Capacity, bool NonEmptyConstexpr>
constexpr std::integral_constant<std::size_t, Capacity>
    inline_vector_internal_storage<T, Capacity, NonEmptyConstexpr>::capacity;

template <typename T, std::size_t Capacity, bool NonEmptyConstexpr>
constexpr std::integral_constant<std::size_t, Capacity>
    inline_vector_internal_storage<T, Capacity, NonEmptyConstexpr>::max_size;

template <typename T, std::size_t Capacity>
constexpr std::integral_constant<std::size_t, Capacity> inline_vector_internal_storage<T, Capacity, true>::capacity;

template <typename T, std::size_t Capacity>
constexpr std::integral_constant<std::size_t, Capacity> inline_vector_internal_storage<T, Capacity, true>::max_size;

template <typename T>
constexpr std::integral_constant<std::size_t, 0> inline_vector_internal_storage<T, 0, false>::capacity;

template <typename T>
constexpr std::integral_constant<std::size_t, 0> inline_vector_internal_storage<T, 0, false>::max_size;

// parasoft-begin-suppress CERT_CPP-MEM51-c-1 "False Positive: Copy assignment is defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-M14_5_3-a-2 "False Positive: Copy assignment is defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-A14_5_1-a-2 "False Positive: Copy constructor is defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-A1_1_1-c-2 "False Positive: Copy operations are defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-A1_1_1-b-2 "False Positive: Copy operations are defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-A12_0_1-a-2 "False Positive: Copy operations are defined or deleted as appropriate"
// parasoft-begin-suppress AUTOSAR-A10_1_1-a-2 "False positive: 'vector_base' is an interface
/// @brief Interface to the storage for @c inline_vector
/// @tparam Storage The class that provides storage for holding the elements
///
/// Helper class to define functions necessary for defining a vector.
///
/// The @c Storage type is required to provide the following typedefs:
/// * @c size_type
/// * @c value_type
///
/// The @c Storage type is required to implement the following functions:
/// * <tt> auto values() const -> span<value_type const> </tt>
/// * <tt> auto values() -> span<value_type> </tt>
/// * <tt> void internal_emplace_back(args&&...) </tt>
/// * <tt> void shrink_to() </tt>
///
/// This interface can be constructed as:
/// * <tt> vector_interface(vector_interface) </tt> (protected)
/// * <tt> vector_interface() </tt>
/// * <tt> vector_interface(std::initializer_list) </tt>
/// * <tt> vector_interface(size_type) </tt>
/// * <tt> vector_interface(size_type, value_type) </tt>
/// * <tt> vector_interface(Iterator, Iterator) </tt>
/// * <tt> vector_interface(move_construction_tag, SourceContainer) </tt>
///
/// This interface will provide:
/// * <tt> auto empty() const -> bool </tt>
/// * <tt> auto size() const -> size_type </tt>
/// * <tt> auto back() -> reference </tt>
/// * <tt> auto back() const -> const_reference </tt>
/// * <tt> auto front() -> reference </tt>
/// * <tt> auto front() const -> const_reference </tt>
/// * <tt> void assign(Iterator, Iterator) </tt>
/// * <tt> void assign(std::initializer_list) </tt>
/// * <tt> void assign(size_type, value_type) </tt>
/// * <tt> void resize(size_type) </tt>
/// * <tt> void resize(size_type, value_type) </tt>
/// * <tt> void push_back(value_type) </tt>
/// * <tt> void emplace_back(Args...) </tt>
/// * <tt> void clear() </tt>
/// * <tt> auto data() -> pointer </tt>
/// * <tt> auto data() const -> const_pointer </tt>
/// * <tt> auto at(size_type) -> reference </tt>
/// * <tt> auto at(size_type) const -> const_reference </tt>
/// * <tt> auto operator[](size_type) -> reference </tt>
/// * <tt> auto operator[](size_type) const -> const_reference </tt>
/// * <tt> auto begin() -> iterator </tt>
/// * <tt> auto begin() const -> const_iterator </tt>
/// * <tt> auto cbegin() const -> const_iterator </tt>
/// * <tt> auto end() -> iterator </tt>
/// * <tt> auto end() const -> const_iterator </tt>
/// * <tt> auto cend() const -> const_iterator </tt>
/// * <tt> auto rbegin() -> reverse_iterator </tt>
/// * <tt> auto rbegin() const -> const_reverse_iterator </tt>
/// * <tt> auto crbegin() const -> const_reverse_iterator </tt>
/// * <tt> auto rend() -> reverse_iterator </tt>
/// * <tt> auto rend() const -> const_reverse_iterator </tt>
/// * <tt> auto crend() const -> const_iterator </tt>
/// * <tt> auto insert(const_iterator, value_type) -> iterator </tt>
/// * <tt> auto insert(const_iterator, size_type, value_type) -> iterator </tt>
/// * <tt> auto insert(const_iterator, InputIterator, InputIterator) -> iterator </tt>
/// * <tt> auto erase(const_iterator) -> iterator </tt>
/// * <tt> auto erase(const_iterator, const_iterator) -> iterator </tt>
/// * <tt> void pop_back() </tt>
template <typename Storage>
// NOLINTNEXTLINE(hicpp-special-member-functions)
class vector_interface
    : public vector_base<typename Storage::value_type>
    , public detail::mixin_at<vector_interface<Storage>, typename vector_base<typename Storage::value_type>::size_type>
    , protected Storage {
  /// @brief type alias for the base class with type aliases
  using vector_base_type = vector_base<typename Storage::value_type>;

 protected:
  // Alias in typedefs from base class
  using typename vector_base_type::const_iterator;
  using typename vector_base_type::const_pointer;
  using typename vector_base_type::const_reference;
  using typename vector_base_type::const_reverse_iterator;
  using typename vector_base_type::difference_type;
  using typename vector_base_type::iterator;
  using typename vector_base_type::pointer;
  using typename vector_base_type::reference;
  using typename vector_base_type::reverse_iterator;
  using typename vector_base_type::size_type;
  using typename vector_base_type::value_type;

  using Storage::capacity;

  /// @brief swaps all the elements between this vector and another.
  ///
  /// @param other the vector to swap with.
  /// @post The elements in @c this and @c other are exchanged 1:1 up to the @c size() of the smallest vector. Relative
  ///       order is maintained. The remaining elements from the originally larger vector are move-constructed into the
  ///       originally smaller vector maintaining relative order, and the elements in the originally larger vector are
  ///       destroyed.
  ///
  constexpr void do_swap(vector_interface& other
  ) noexcept(is_nothrow_swappable_v<value_type> && std::is_nothrow_move_constructible<value_type>::value) {
    auto our_values = this->values();
    auto other_values = other.values();
    auto const swap_count = std::min<>(size(), other.size());
    for (vector_interface::size_type i{0U}; i < swap_count; ++i) {
      ::arene::base::swap(our_values[i], other_values[i]);
    }
    if (size() < other.size()) {
      for (vector_interface::size_type i{swap_count}; i < other.size(); ++i) {
        this->internal_emplace_back(std::move(other_values[i]));
      }
      other.shrink_to(swap_count);
    } else {
      for (vector_interface::size_type i{swap_count}; i < size(); ++i) {
        other.internal_emplace_back(std::move(our_values[i]));
      }
      this->shrink_to(swap_count);
    }
  }

  /// @brief passkey type for constructor to indicate move construction from a source
  class move_construction_tag {
   public:
    /// @brief explicit default constructor to avoid initialization from empty braces
    explicit move_construction_tag() = default;
  };

  /// @brief Default copy constructor
  constexpr vector_interface(vector_interface const&) = default;
  /// @brief Default move constructor
  constexpr vector_interface(vector_interface&&) = default;

  /// @brief Default copy assignment operator
  constexpr auto operator=(vector_interface const&) -> vector_interface& = default;
  /// @brief Default move assignment operator
  constexpr auto operator=(vector_interface&&) -> vector_interface& = default;

 public:
  /// @brief Construct the storage with no elements
  constexpr vector_interface() noexcept = default;

  /// @brief Default destructor
  ~vector_interface() = default;

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Construct a vector populated with the supplied elements.
  /// @tparam U Dummy template parameter the same as @c value_type for constraints
  /// @param initial_values The initial initial_values
  /// @pre @c initial_values.size()<=capacity(), else @c ARENE_PRECONDITION violation
  template <
      typename U = value_type,
      constraints<
          std::enable_if_t<std::is_same<U, value_type>::value>,
          std::enable_if_t<std::is_copy_constructible<U>::value>> = nullptr>
  constexpr vector_interface(std::initializer_list<value_type> initial_values
  ) noexcept(std::is_nothrow_copy_constructible<value_type>::value)
      : vector_interface() {
    ARENE_PRECONDITION(initial_values.size() <= capacity());
    // parasoft-begin-suppress AUTOSAR-M14_6_1-a-2 "False Positive: unqualified identifier not used"
    for (auto& value : initial_values) {  // parasoft-suppress AUTOSAR-A7_1_5-a-2 "False Positive: variable is declared
                                          // to have same type as initializer"
      // parasoft-end-suppress AUTOSAR-M14_6_1-a-2
      this->internal_emplace_back(value);
    }
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Construct a vector populated with the specified number of default-constructed elements
  /// @tparam U Dummy template parameter the same as @c value_type for constraints
  /// @param count The initial size
  /// @pre @c count<=capacity(), else @c ARENE_PRECONDITION violation
  template <
      typename U = value_type,
      constraints<
          std::enable_if_t<std::is_same<U, value_type>::value>,
          std::enable_if_t<std::is_default_constructible<U>::value>> = nullptr>
  constexpr explicit vector_interface(vector_interface::size_type const count
  ) noexcept(std::is_nothrow_default_constructible<value_type>::value)
      : vector_interface() {
    ARENE_PRECONDITION(count <= capacity());
    for (vector_interface::size_type i{count}; i != 0U; --i) {
      this->internal_emplace_back();
    }
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Construct a vector populated with the specified number of
  /// elements copy-constructed from the supplied value
  /// @tparam U Dummy template parameter the same as @c value_type for constraints
  /// @param count The initial size
  /// @param source The initial value
  /// @pre @c count<=capacity(), else @c ARENE_PRECONDITION violation
  template <
      typename U = value_type,
      constraints<
          std::enable_if_t<std::is_same<U, value_type>::value>,
          std::enable_if_t<std::is_copy_constructible<U>::value>> = nullptr>
  constexpr explicit vector_interface(vector_interface::size_type const count, value_type const& source) noexcept(
      std::is_nothrow_copy_constructible<value_type>::value
  )
      : vector_interface() {
    ARENE_PRECONDITION(count <= capacity());
    for (vector_interface::size_type i{count}; i != 0U; --i) {
      this->internal_emplace_back(source);
    }
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Construct a vector from the elements of an input iterator range
  /// @tparam Iterator The type of the iterator
  /// @param first The start of the range
  /// @param last The end of the range
  /// @pre The number of elements in the range must be less than @c Capacity, else @c ARENE_PRECONDITION violation
  template <
      typename Iterator,
      constraints<
          std::enable_if_t<arene::base::is_input_iterator_v<Iterator>>,
          std::enable_if_t<!arene::base::is_bidirectional_iterator_v<Iterator>>> = nullptr>
  vector_interface(Iterator first, Iterator last) noexcept(
      denotes_nothrow_iterable_range_v<Iterator> &&
      std::is_nothrow_constructible<value_type, typename std::iterator_traits<Iterator>::reference>::value
  )
      : vector_interface() {
    for (auto&& elem : make_subrange(first, last)) {
      push_back(std::forward<decltype(elem)>(elem));
    }
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2
  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Construct a vector from the elements of a bidrectional iterator range
  /// @tparam Iterator The type of the iterator
  /// @param first The start of the range
  /// @param last The end of the range
  /// @pre The number of elements in the range must be less than @c Capacity, else @c ARENE_PRECONDITION violation
  template <
      typename Iterator,
      constraints<
          std::enable_if_t<arene::base::is_bidirectional_iterator_v<Iterator>>,
          std::enable_if_t<
              std::is_constructible<value_type, typename std::iterator_traits<Iterator>::reference>::value>> = nullptr>
  constexpr vector_interface(Iterator first, Iterator last) noexcept(
      denotes_nothrow_iterable_range_v<Iterator> &&
      std::is_nothrow_constructible<value_type, typename std::iterator_traits<Iterator>::reference>::value
  )
      : vector_interface() {
    auto const source_count = static_cast<size_type>(arene::base::distance(first, last));
    ARENE_PRECONDITION(source_count <= capacity());
    for (auto&& elem : make_subrange(first, last)) {
      this->internal_emplace_back(std::forward<decltype(elem)>(elem));
    }
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "False positive: Elements are moved"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "False positive: Elements are moved"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: Elements are moved"
  /// @brief Move construct from another vector
  /// @tparam SoureContainer The type of the other vector
  /// @param other The other vector
  /// @pre @c other.size()<=Capacity, else @c ARENE_PRECONDITION violation
  template <typename SourceContainer>
  constexpr explicit vector_interface(move_construction_tag, SourceContainer&& other) noexcept(
      std::is_nothrow_move_constructible<value_type>::value
  )
      : vector_interface() {
    ARENE_PRECONDITION(other.size() <= capacity());
    // parasoft-begin-suppress AUTOSAR-M14_6_1-a-2 "False Positive: unqualified identifier not used"
    for (auto& elem : other) {
      // parasoft-end-suppress AUTOSAR-M14_6_1-a-2
      this->internal_emplace_back(std::move(elem));
    }
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2

  /// @brief Check if the vector is empty.
  /// @return @c true if the vector is empty, @c false otherwise
  constexpr auto empty() const noexcept -> bool { return size() == 0U; }
  /// @brief Get the current number of live elements in the vector
  /// @return the size
  constexpr auto size() const noexcept -> size_type { return this->values().size(); }

  /// @brief Get a reference to the last element.
  /// @return A reference to the element
  /// @pre The vector must not be empty
  constexpr auto back() noexcept -> reference { return this->values().back(); }
  /// @brief Get a reference to the last element.
  /// @return A reference to the element
  /// @pre The vector must not be empty
  constexpr auto back() const noexcept -> const_reference { return this->values().back(); }

  /// @brief Get a reference to the first element.
  /// @return A reference to the element
  /// @pre The vector must not be empty
  constexpr auto front() noexcept -> reference { return this->values().front(); }
  /// @brief Get a reference to the first element.
  /// @return A reference to the element
  /// @pre The vector must not be empty
  constexpr auto front() const noexcept -> const_reference { return this->values().front(); }

  // parasoft-begin-suppress AUTOSAR-M0_1_8-b "False positive: function assign is not empty"
  // parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: parameters first and last are used"
  /// @brief Assign to the vector from the elements of an input iterator range. Assigns
  /// over existing elements, constructs additional elements, and destroys any
  /// excess.
  /// @tparam Iterator The type of the iterator
  /// @param first The start of the range
  /// @param last The end of the range
  /// @pre The number of elements in the range must be less than @c Capacity, else @c ARENE_PRECONDITION violation
  template <
      typename Iterator,
      constraints<
          std::enable_if_t<arene::base::is_input_iterator_v<Iterator>>,
          std::enable_if_t<!arene::base::is_forward_iterator_v<Iterator>>> = nullptr>
  constexpr void assign(Iterator first, Iterator last) noexcept(
      denotes_nothrow_iterable_range_v<Iterator> &&
      std::is_nothrow_constructible<value_type, typename std::iterator_traits<Iterator>::reference>::value &&
      std::is_nothrow_assignable<value_type&, typename std::iterator_traits<Iterator>::reference>::value
  ) {
    auto our_values = this->values();
    vector_interface::size_type idx{0U};
    while ((idx < our_values.size()) && (first != last)) {
      our_values[idx] = *first++;
      ++idx;
    }
    while (first != last) {
      push_back(*first++);
      ++idx;
    }
    this->shrink_to(idx);
  }
  // parasoft-end-suppress AUTOSAR-A0_1_4-a
  // parasoft-end-suppress AUTOSAR-M0_1_8-b

  /// @brief Assign to the vector from the elements of a bidrectional iterator range.
  /// Assigns over existing elements, constructs additional elements, and
  /// destroys any excess.
  /// @tparam Iterator The type of the iterator
  /// @param first The start of the range
  /// @param last The end of the range
  /// @pre The number of elements in the range must be less than @c Capacity, else @c ARENE_PRECONDITION violation
  template <typename Iterator, constraints<std::enable_if_t<arene::base::is_forward_iterator_v<Iterator>>> = nullptr>
  constexpr void assign(Iterator const first, Iterator const last) noexcept(
      denotes_nothrow_iterable_range_v<Iterator> &&
      std::is_nothrow_constructible<value_type, typename std::iterator_traits<Iterator>::reference>::value &&
      std::is_nothrow_assignable<value_type&, typename std::iterator_traits<Iterator>::reference>::value
  ) {
    auto const source_distance = arene::base::distance(first, last);
    auto const source_size = static_cast<size_type>(source_distance);
    ARENE_PRECONDITION(source_size <= this->capacity());

    // parasoft-begin-suppress AUTOSAR-M14_6_1-a-2 "False Positive: unqualified identifier not used"
    Iterator current_source{first};
    for (auto& elem_to_overwrite : this->values()) {
      // parasoft-end-suppress AUTOSAR-M14_6_1-a-2
      if (current_source == last) {
        break;
      }
      elem_to_overwrite = *current_source;
      // parasoft-begin-suppress AUTOSAR-M5_0_15-a-2 "False positive: current_source is an iterator, bounded by last"
      ++current_source;
      // parasoft-end-suppress AUTOSAR-M5_0_15-a-2
    }

    for (auto&& source_elem : make_subrange(current_source, last)) {
      this->internal_emplace_back(std::forward<decltype(source_elem)>(source_elem));
    }
    this->shrink_to(source_size);
  }

  /// @brief Assign to the vector from the elements of an initializer list. Assigns
  /// over existing elements, constructs additional elements, and destroys any
  /// excess.
  /// @param source_values The source values
  /// @pre @c source_values.size()<=Capacity, else @c ARENE_PRECONDITION violation
  void assign(std::initializer_list<value_type> source_values) noexcept(
      std::is_nothrow_copy_constructible<value_type>::value && std::is_nothrow_copy_assignable<value_type>::value
  ) {
    assign(source_values.begin(), source_values.end());
  }

  /// @brief Assign to the vector the specified number of copies of a given source
  /// element. Assigns over existing elements, constructs additional elements,
  /// and destroys any excess.
  /// @param count The number of copies to store
  /// @param value The source value to use
  /// @pre @c count<=Capacity, else @c ARENE_PRECONDITION violation
  void assign(size_type count, value_type const& value) noexcept(
      std::is_nothrow_copy_constructible<value_type>::value && std::is_nothrow_copy_assignable<value_type>::value
  ) {
    ARENE_PRECONDITION(count <= this->capacity());
    auto our_values = this->values();
    for (vector_interface::size_type i{0U}; (i < our_values.size()) && (i < count); ++i) {
      our_values[i] = value;
    }
    resize(count, value);
  }

  /// @brief Resize the vector to the specified size. Default-constructs any missing
  /// elements, and destroys any excess.
  /// @param count The desired size
  /// @pre @c count<=Capacity, else @c ARENE_PRECONDITION violation
  constexpr void resize(size_type count) noexcept(std::is_nothrow_default_constructible<value_type>::value) {
    ARENE_PRECONDITION(count <= this->capacity());
    while (size() < count) {
      this->internal_emplace_back();
    }
    this->shrink_to(count);
  }

  /// @brief Resize the vector to the specified size using a supplied value.
  /// Copy-constructs any missing elements from the provided value, and destroys
  /// any excess.
  /// @param count The desired size
  /// @param value The source value to use for the new elements
  /// @pre @c count<=Capacity, else @c ARENE_PRECONDITION violation
  constexpr void resize(size_type count, value_type const& value) noexcept(
      std::is_nothrow_copy_constructible<value_type>::value
  ) {
    ARENE_PRECONDITION(count <= this->capacity());
    while (size() < count) {
      this->internal_emplace_back(value);
    }
    this->shrink_to(count);
  }

  /// @brief Add a new element to the end of the vector as a copy of the supplied
  /// value.
  /// @param value The value to copy
  /// @pre @c size()<Capacity, else @c ARENE_PRECONDITION violation
  constexpr void push_back(value_type const& value) noexcept(std::is_nothrow_copy_constructible<value_type>::value) {
    ARENE_PRECONDITION(size() < this->capacity());
    this->internal_emplace_back(value);
  }

  /// @brief Add a new element to the end of the vector by move-constructing from the
  /// supplied value.
  /// @param value The value to move from
  /// @pre @c size()<Capacity, else @c ARENE_PRECONDITION violation
  constexpr void push_back(value_type&& value) noexcept(std::is_nothrow_move_constructible<value_type>::value) {
    ARENE_PRECONDITION(size() < this->capacity());
    this->internal_emplace_back(std::move(value));
  }

  /// @brief Add a new element to the end of the vector, constructed in place from the
  /// supplied arguments.
  /// @tparam Args The types of the arguments
  /// @param args The constructor arguments
  /// @return A reference to the new element
  /// @pre @c size()<Capacity, else @c ARENE_PRECONDITION violation
  template <typename... Args>
  constexpr auto emplace_back(Args&&... args) noexcept(std::is_nothrow_constructible<value_type, Args&&...>::value)
      -> reference {
    ARENE_PRECONDITION(size() < this->capacity());
    this->internal_emplace_back(std::forward<Args>(args)...);
    return back();
  }

  /// @brief Destroy all elements and set the size to zero
  constexpr void clear() noexcept { this->shrink_to(size_type{}); }

  /// @brief Obtain a pointer to the stored elements
  /// @return A raw pointer to the elements
  constexpr auto data() noexcept -> pointer { return this->values().data(); }

  /// @brief Obtain a pointer to the stored elements
  /// @return A raw pointer to the elements
  constexpr auto data() const noexcept -> const_pointer { return this->values().data(); }

  // parasoft-begin-suppress AUTOSAR-A2_10_1-a "False Positive: 'index' does not hide anything"
  /// @brief Obtain a reference to the element with the specified index
  /// @param index The index of the element
  /// @return A reference to the element
  /// @pre @c index must be less than @c size()
  constexpr auto operator[](size_type index) noexcept -> reference { return this->values()[index]; }

  /// @brief Obtain a const reference to the element with the specified index
  /// @param index The index of the element
  /// @return A reference to the element
  /// @pre @c index must be less than @c size()
  constexpr auto operator[](size_type index) const noexcept -> const_reference { return this->values()[index]; }
  // parasoft-end-suppress AUTOSAR-A2_10_1-a

  /// @brief Obtain an iterator to the start of the vector.
  /// @return The iterator
  constexpr auto begin() noexcept -> iterator {
    return {typename vector_base_type::iterator_passkey{}, this->values().begin().base()};
  }
  /// @brief Obtain an iterator to the end of the vector.
  /// @return The iterator
  constexpr auto end() noexcept -> iterator {
    return {typename vector_base_type::iterator_passkey{}, this->values().end().base()};
  }

  /// @brief Obtain an iterator to the start of the vector.
  /// @return The iterator
  constexpr auto begin() const noexcept -> const_iterator { return cbegin(); }
  /// @brief Obtain an iterator to the end of the vector.
  /// @return The iterator
  constexpr auto end() const noexcept -> const_iterator { return cend(); }

  /// @brief Obtain an iterator to the start of the vector.
  /// @return The iterator
  constexpr auto cbegin() const noexcept -> const_iterator {
    return {typename vector_base_type::iterator_passkey{}, this->values().cbegin().base()};
  }
  /// @brief Obtain an iterator to the end of the vector.
  /// @return The iterator
  constexpr auto cend() const noexcept -> const_iterator {
    return {typename vector_base_type::iterator_passkey{}, this->values().cend().base()};
  }

  /// @brief Obtain an iterator to the end of the vector for iterating in reverse.
  /// @return The iterator
  constexpr auto rbegin() noexcept -> reverse_iterator { return reverse_iterator(end()); }
  /// @brief Obtain an iterator to the start of the vector for iterating in reverse
  /// @return The iterator
  constexpr auto rend() noexcept -> reverse_iterator { return reverse_iterator(begin()); }

  /// @brief Obtain an iterator to the end of the vector for iterating in reverse.
  /// @return The iterator
  constexpr auto rbegin() const noexcept -> const_reverse_iterator { return crbegin(); }
  /// @brief Obtain an iterator to the start of the vector for iterating in reverse
  /// @return The iterator
  constexpr auto rend() const noexcept -> const_reverse_iterator { return crend(); }

  /// @brief Obtain an iterator to the end of the vector for iterating in reverse.
  /// @return The iterator
  constexpr auto crbegin() const noexcept -> const_reverse_iterator { return const_reverse_iterator(cend()); }
  /// @brief Obtain an iterator to the start of the vector for iterating in reverse
  /// @return The iterator
  constexpr auto crend() const noexcept -> const_reverse_iterator { return const_reverse_iterator(cbegin()); }

  /// @brief Insert a new element at the specified position in the vector by
  /// move-constructing or move-assigning from the supplied value. Any existing
  /// elements from the specified position to the end of the vector are moved to
  /// make room.
  /// @param pos The position to insert the new element
  /// @param value The value to move from
  /// @return An iterator referring to the new element
  /// @pre @c size()<Capacity, else @c ARENE_PRECONDITION violation
  constexpr auto insert(const_iterator pos, value_type&& value) noexcept(
      std::is_nothrow_move_constructible<value_type>::value && std::is_nothrow_move_assignable<value_type>::value
  ) -> iterator {
    return internal_insert(pos, 1U, std::move(value));
  }
  /// @brief Insert a new element at the specified position in the vector by
  /// copy-constructing or copy-assigning from the supplied value. Any existing
  /// elements from the specified position to the end of the vector are moved to
  /// make room.
  /// @param pos The position to insert the new element
  /// @param value The value to copy from
  /// @return An iterator referring to the new element
  /// @pre @c size()<Capacity, else @c ARENE_PRECONDITION violation
  constexpr auto insert(const_iterator pos, value_type const& value) noexcept(
      std::is_nothrow_move_constructible<value_type>::value && std::is_nothrow_move_assignable<value_type>::value &&
      std::is_nothrow_copy_constructible<value_type>::value
  ) -> iterator {
    return internal_insert(pos, 1U, value);
  }
  /// @brief Insert a specified number of copies of a new element at the specified
  /// position in the vector by copy-constructing or copy-assigning from the
  /// supplied value. Any existing elements from the specified position to the
  /// end of the vector are moved to make room.
  /// @param pos The position to insert the new element
  /// @param count The number of copies to insert
  /// @param value The value to copy from
  /// @return An iterator referring to the location of the new elements
  /// @pre @c count<=(Capacity-size()), else @c ARENE_PRECONDITION violation
  constexpr auto insert(const_iterator pos, size_type count, value_type const& value) noexcept(
      std::is_nothrow_move_constructible<value_type>::value && std::is_nothrow_move_assignable<value_type>::value &&
      std::is_nothrow_copy_constructible<value_type>::value
  ) -> iterator {
    return internal_insert(pos, count, value);
  }

  /// @brief Insert new element at the specified position in the vector by
  /// constructing or assigning from the elements in the supplied range. Any
  /// existing elements from the specified position to the end of the vector are
  /// moved to make room.
  /// @param pos The position to insert the new element
  /// @param first The start of the range to insert
  /// @param last The end of the range to insert
  /// @return An iterator referring to the new element
  /// @pre The number of elements in the range must be less than or equal to @c Capacity-size(), else @c
  /// ARENE_PRECONDITION violation
  template <
      typename InputIterator,
      constraints<std::enable_if_t<arene::base::is_input_iterator_v<InputIterator>>> = nullptr>
  constexpr auto insert(const_iterator pos, InputIterator first, InputIterator last) noexcept(
      denotes_nothrow_iterable_range_v<InputIterator> && std::is_nothrow_move_constructible<value_type>::value &&
      std::is_nothrow_move_assignable<value_type>::value &&
      std::is_nothrow_constructible<value_type, typename std::iterator_traits<InputIterator>::reference>::value
  ) -> iterator {
    return internal_insert(pos, first, last);
  }

  /// @brief Insert a new element at the specified position in the vector by
  /// move-constructing or move-assigning from a new element constructed from
  /// the supplied arguments. Any existing elements from the specified position
  /// to the end of the vector are moved to make room.
  /// @tparam Args The types of the constructor arguments
  /// @param pos The position to insert the new element
  /// @param args The constructor arguments
  /// @return An iterator referring to the new element
  /// @pre @c size()<Capacity, else @c ARENE_PRECONDITION violation
  template <typename... Args>
  constexpr auto emplace(const_iterator pos, Args&&... args) noexcept(
      std::is_nothrow_move_constructible<value_type>::value && std::is_nothrow_move_assignable<value_type>::value &&
      std::is_nothrow_constructible<value_type, Args&&...>::value
  ) -> iterator {
    // parasoft-begin-suppress AUTOSAR-A5_2_2-a-2 "False positive: constructor syntax not C-style cast"
    return internal_insert(pos, 1U, value_type(std::forward<Args>(args)...));
    // parasoft-end-suppress AUTOSAR-A5_2_2-a-2
  }

  /// @brief Erase the element at the specified position.
  /// @param pos An iterator referring to the element to erase
  /// @return An iterator referring to the erase location
  /// @pre @c pos Must be a valid iterator to a live element in @c this
  constexpr auto erase(const_iterator pos) noexcept(std::is_nothrow_move_assignable<value_type>::value) -> iterator {
    return erase(pos, pos + 1);
  }

  /// @brief Erase all the element in the specified range.
  /// @param first An iterator referring to the start of the range to erase
  /// @param last An iterator referring to the end of the range to erase
  /// @pre @c [first,last)  Must be a valid iterator range in @c this
  /// @return An iterator referring to the erase location
  constexpr auto erase(const_iterator first, const_iterator last) noexcept(
      std::is_nothrow_move_assignable<value_type>::value
  ) -> iterator {
    // parasoft-begin-suppress AUTOSAR-M14_6_1-a-2 "False Positive: unqualified identifier not used"
    auto count = last - first;
    auto ncpos = begin() + (first - begin());
    for (auto next = ncpos + count, endpos = end(); next != endpos; ++next) {
      *(next - count) = std::move(*next);
    }
    // parasoft-end-suppress AUTOSAR-M14_6_1-a-2
    this->shrink_to(size() - static_cast<size_type>(count));
    return ncpos;
  }

  /// @brief Destroy the last element in the vector and decrease the size
  /// @pre The vector must not be empty
  constexpr void pop_back() noexcept {
    ARENE_PRECONDITION(!empty());
    this->shrink_to(size() - size_type{1U});
  }

 private:
  /// @brief Obtain the available free space
  /// @return std::size_t holding the available space before the capacity is exhausted
  constexpr auto free_space() const noexcept -> std::size_t { return this->capacity() - size(); }

  /// @brief Actually do an insert at the specified position, of the specified number
  /// of elements from the given source. Existing elements are moved out of the
  /// way.
  /// @tparam Source The type of the source
  /// @param pos The position at which to insert elements
  /// @param count The number of elements to insert
  /// @param source A class representing the source of the elements
  /// @return An iterator to the insert position
  /// @pre @c (Capacity-size())<=count, else @c ARENE_PRECONDITION violation
  template <typename Source>
  constexpr auto internal_do_insert(const_iterator pos, size_type count, Source source) -> iterator {
    ARENE_PRECONDITION(free_space() >= count);
    auto const insert_offset = pos - begin();
    auto ncpos = begin() + insert_offset;
    auto const move_count = end() - pos;
    auto const insert_count = static_cast<difference_type>(count);
    auto const move_construct_count = std::min<>(move_count, insert_count);
    auto const insert_construct_count = (insert_count > move_count) ? insert_count - move_count : 0;
    // parasoft-begin-suppress AUTOSAR-M3_4_1-a-2 "False positive: Moving this later may affect the value"
    // NOLINTNEXTLINE(hicpp-use-auto)
    difference_type const original_end_offset{static_cast<difference_type>(size())};
    // parasoft-end-suppress AUTOSAR-M3_4_1-a-2

    source.reset_index(move_construct_count);
    for (vector_interface::difference_type i{0}; i < insert_construct_count; ++i) {
      this->internal_emplace_back(source.next());
    }
    if (move_count != 0) {
      auto const move_assign_count = (insert_count > move_count) ? 0 : move_count - insert_count;
      auto old_elem = original_end_offset - move_construct_count;
      for (vector_interface::difference_type i{0}; i < move_construct_count; ++i) {
        this->internal_emplace_back(std::move((*this)[static_cast<size_type>(old_elem)]));
        ++old_elem;
      }
      old_elem = original_end_offset - move_construct_count - 1;
      vector_interface::difference_type move_target{original_end_offset - 1};

      for (vector_interface::difference_type i{0}; i < move_assign_count; ++i) {
        (*this)[static_cast<size_type>(move_target)] = std::move((*this)[static_cast<size_type>(old_elem)]);
        --move_target;
        --old_elem;
      }
      auto insert_pos = ncpos;
      source.reset_index(0);
      for (vector_interface::difference_type i{0}; i < move_construct_count; ++i) {
        *insert_pos++ = source.next();
      }
    }
    return ncpos;
  }

  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "False positive: arg is moved"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "False positive: arg is moved"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: arg is moved"
  /// @brief Insert a number of copies of the specified value at the specified
  /// position. If @c Arg is an rvalue, moves rather than copies.
  /// @tparam Arg The type of the argument.
  /// @param pos The position at which to insert the elements
  /// @param count The number of elements to insert
  /// @param arg The value to insert
  /// @return An iterator referring to the insert location
  template <typename Arg>
  constexpr auto internal_insert(const_iterator pos, size_type count, Arg&& arg) -> iterator {
    /// @brief This type holds a reference to the argument, in order to avoid unnecessary copying.
    /// The stored reference is then used to construct the inserted value
    class fixed_source {
      // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
      Arg& arg_;

     public:
      constexpr explicit fixed_source(Arg& arg) noexcept
          : arg_(arg) {}
      constexpr void reset_index(difference_type) noexcept {}
      constexpr auto next() const noexcept -> Arg&& { return std::forward<Arg>(arg_); }
    };
    return internal_do_insert(pos, count, fixed_source{arg});
  }
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2

  /// @brief Insert elements from a bidirectional iterator range at the specified
  /// position.
  /// @tparam Iterator The type of the iterator
  /// @param pos The position at which to insert the elements
  /// @param first The start of the iterator range to insert
  /// @param last The end of the iterator range to insert
  /// @return An iterator referring to the insert location
  template <
      typename Iterator,
      constraints<std::enable_if_t<arene::base::is_bidirectional_iterator_v<Iterator>>> = nullptr>
  constexpr auto internal_insert(const_iterator pos, Iterator first, Iterator last) -> iterator {
    auto const insert_count = static_cast<size_type>(arene::base::distance(first, last));
    ARENE_PRECONDITION(free_space() >= insert_count);

    class iterator_source {
      Iterator base_;
      Iterator current_;

     public:
      constexpr explicit iterator_source(Iterator base)
          : base_(base),
            current_(base) {}

      constexpr void reset_index(difference_type index) { current_ = ::arene::base::next(base_, index); }
      constexpr auto next() -> decltype(*current_) { return *current_++; }
    };
    return internal_do_insert(pos, insert_count, iterator_source{first});
  }

  /// @brief Insert elements from an input iterator range at the specified
  /// position.
  /// @return An iterator referring to the insert location
  /// @tparam Iterator The type of the iterator
  /// @param pos The position at which to insert the elements
  /// @param first The start of the iterator range to insert
  /// @param last The end of the iterator range to insert
  template <
      typename Iterator,
      constraints<
          std::enable_if_t<arene::base::is_input_iterator_v<Iterator>>,
          std::enable_if_t<!arene::base::is_bidirectional_iterator_v<Iterator>>> = nullptr>
  constexpr auto internal_insert(const_iterator pos, Iterator first, Iterator last) -> iterator {
    auto ncpos = begin() + (pos - begin());
    for (; first != last; ++first) {
      internal_insert(pos++, 1, *first);
    }
    return ncpos;
  }
};
// parasoft-end-suppress AUTOSAR-A10_1_1-a-2
// parasoft-end-suppress AUTOSAR-A12_0_1-a-2
// parasoft-end-suppress AUTOSAR-A1_1_1-c-2
// parasoft-end-suppress AUTOSAR-A1_1_1-b-2
// parasoft-end-suppress AUTOSAR-M14_5_3-a-2
// parasoft-end-suppress AUTOSAR-A14_5_1-a-2

/// @brief Constant that is @c true if a container is an instantiation of @c inline_vector of @c T with any capacity,
/// @c false otherwise
/// @tparam T The element type
/// @tparam Container The container type to check
template <typename T, typename Container>
constexpr bool is_inline_vector_of_t_v = false;

/// @brief Constant that is @c true if a container is an instantiation of @c inline_vector of @c T with any capacity,
/// @c false otherwise
/// @tparam T The element type
/// @tparam Capacity The capacity of the @c inline_vector being checked
template <typename T, std::size_t Capacity>
constexpr bool is_inline_vector_of_t_v<T, inline_vector<T, Capacity>> = true;

}  // namespace inline_vector_detail

// parasoft-begin-suppress AUTOSAR-A10_1_1-a-2 "False positive: the vector_interface is the only non-empty base class"
// parasoft-begin-suppress AUTOSAR-A12_1_5-a "False positive: No work done in constructors to delegate"
/// @brief A container similar to @c std::vector<T> that has a fixed capacity. The
/// storage for the elements is held directly within the class. Any attempt to
/// store more than @c Capacity elements will throw @c std::length_error
/// @tparam T The type of each element
/// @tparam Capacity The maximum number of elements that can be stored.
template <typename T, std::size_t Capacity>
class inline_vector
    : public inline_vector_detail::vector_interface<inline_vector_detail::inline_vector_internal_storage<T, Capacity>>
    , full_ordering_operators_from_less_than_and_equals<inline_vector<T, Capacity>> {
  static_assert(std::is_nothrow_destructible<T>::value, "Destructors must not throw");

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False Positive: This identifier does not hide anything"
  /// @brief The type of the interface base class
  using interface_base =
      inline_vector_detail::vector_interface<inline_vector_detail::inline_vector_internal_storage<T, Capacity>>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

 public:
  // Alias in typedefs from base class
  using typename interface_base::const_iterator;
  using typename interface_base::const_pointer;
  using typename interface_base::const_reference;
  using typename interface_base::const_reverse_iterator;
  using typename interface_base::difference_type;
  using typename interface_base::iterator;
  using typename interface_base::pointer;
  using typename interface_base::reference;
  using typename interface_base::reverse_iterator;
  using typename interface_base::size_type;
  using typename interface_base::value_type;

  using interface_base::assign;
  using interface_base::back;
  using interface_base::begin;
  using interface_base::capacity;
  using interface_base::cbegin;
  using interface_base::cend;
  using interface_base::clear;
  using interface_base::crbegin;
  using interface_base::crend;
  using interface_base::data;
  using interface_base::emplace_back;
  using interface_base::empty;
  using interface_base::end;
  using interface_base::erase;
  using interface_base::front;
  using interface_base::insert;
  using interface_base::max_size;
  using interface_base::push_back;
  using interface_base::rbegin;
  using interface_base::rend;
  using interface_base::resize;
  using interface_base::size;
  using interface_base::operator[];
  using interface_base::interface_base;

  /// @brief Construct an empty vector
  constexpr inline_vector() = default;

  /// @brief Default destructor
  ~inline_vector() = default;

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Copy construct from a vector with a different capacity
  /// @tparam OtherCapacity The capacity of the other vector
  /// @tparam U Dummy template parameter the same as @c T for constraints
  /// @param other The other vector
  /// @pre @c other.size()<=capacity(), else @c ARENE_PRECONDITION violation
  template <
      size_type OtherCapacity,
      typename U = T,
      constraints<
          std::enable_if_t<std::is_same<U, T>::value>,
          std::enable_if_t<std::is_copy_constructible<U>::value>,
          std::enable_if_t<OtherCapacity != Capacity>> = nullptr>
  constexpr explicit inline_vector(inline_vector<T, OtherCapacity> const& other
  ) noexcept(std::is_nothrow_copy_constructible<T>::value)
      : inline_vector(other.begin(), other.end()) {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Move construct from a vector with a different capacity
  /// @tparam OtherCapacity The capacity of the other vector
  /// @param other The other vector
  /// @pre @c other.size()<=Capacity, else @c ARENE_PRECONDITION violation
  template <size_type OtherCapacity, constraints<std::enable_if_t<OtherCapacity != Capacity>> = nullptr>
  constexpr explicit inline_vector(inline_vector<T, OtherCapacity>&& other
  ) noexcept(std::is_nothrow_move_constructible<T>::value)
      : inline_vector(typename interface_base::move_construction_tag{}, std::move(other)) {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  /// @brief Default copy constructor
  constexpr inline_vector(inline_vector const&) = default;

  /// @brief Default move constructor
  constexpr inline_vector(inline_vector&&) = default;

  /// @brief Default copy assignment
  constexpr auto operator=(inline_vector const&) -> inline_vector& = default;

  /// @brief Default move assignment
  constexpr auto operator=(inline_vector&&) -> inline_vector& = default;

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False Positive: This identifier does not hide anything"
  /// @brief Default construct a vector
  /// @return optional<inline_vector> holding the default-constructed vector
  static constexpr auto try_construct() noexcept -> optional<inline_vector> { return {in_place}; }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False Positive: This identifier does not hide anything"
  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "Constrained via SFINAE, permitted by A13-3-1 Permit #1"
  /// @brief Construct a vector initialized with the specified parameters
  /// @tparam InitElement The type of the elements in the initializer list with which to initialize the vector
  /// @param init_list The initializer list with which to initialize the vector
  /// @return optional<inline_vector> holding the vector if the number of elements in the source container is less than
  /// or equal to the @c Capacity, otherwise returns an empty optional
  template <
      typename InitElement,
      constraints<std::enable_if_t<std::is_constructible<
          inline_vector,
          typename std::initializer_list<InitElement>::const_iterator,
          typename std::initializer_list<InitElement>::const_iterator>::value>> = nullptr>
  static constexpr auto try_construct(std::initializer_list<InitElement> const& init_list) noexcept(
      std::is_nothrow_constructible<
          inline_vector,
          typename std::initializer_list<InitElement>::const_iterator,
          typename std::initializer_list<InitElement>::const_iterator>::value
  ) -> optional<inline_vector> {
    if (init_list.size() > capacity()) {
      return {};
    }
    return {in_place, init_list.begin(), init_list.end()};
  }
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "Constrained via SFINAE, permitted by A13-3-1 Permit #1"
  /// @brief Construct a vector initialized with the specified paramters
  /// @tparam Container The type of the container with which to initialize the vector
  /// @param container The container with which to initialize the vector
  /// @return optional<inline_vector> holding the vector if the number of elements in the source container is less than
  /// or equal to the @c Capacity, otherwise returns an empty optional
  template <
      typename Container,
      constraints<
          std::enable_if_t<std::is_constructible<inline_vector, Container const&>::value>,
          std::enable_if_t<inline_vector_detail::is_inline_vector_of_t_v<T, Container>>> = nullptr>
  static constexpr auto try_construct(Container const& container
  ) noexcept(std::is_nothrow_constructible<inline_vector, Container const&>::value) -> optional<inline_vector> {
    if (container.size() > capacity()) {
      return {};
    }
    return {in_place, container};
  }
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2

  /// @brief Construct a vector initialized with the specified paramters
  /// @tparam Container The type of the container with which to initialize the vector
  /// @param container The container with which to initialize the vector
  /// @return optional<inline_vector> holding the vector if the number of elements in the source container is less than
  /// or equal to the @c Capacity, otherwise returns an empty optional
  template <
      typename Container,
      constraints<
          std::enable_if_t<std::is_constructible<inline_vector, Container&&>::value>,
          std::enable_if_t<inline_vector_detail::is_inline_vector_of_t_v<T, Container>>> = nullptr>
  static constexpr auto try_construct(Container&& container
  ) noexcept(std::is_nothrow_constructible<inline_vector, Container&&>::value) -> optional<inline_vector> {
    if (container.size() > capacity()) {
      return {};
    }
    return {in_place, std::forward<Container>(container)};
  }

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "Constrained via SFINAE, permitted by A13-3-1 Permit #1"
  /// @brief Construct a vector with the specified number of default-constructed elements
  /// @tparam U Dummy template parameter the same as @c T for constraints
  /// @param count The number of elements to construct in the vector
  /// @return optional<inline_vector> holding the vector if the @c count is less than or equal to the @c Capacity,
  /// otherwise returns an empty optional
  template <
      typename U = T,
      constraints<
          std::enable_if_t<std::is_same<U, T>::value>,
          std::enable_if_t<std::is_default_constructible<U>::value>> = nullptr>
  static constexpr auto try_construct(size_type count) noexcept(std::is_nothrow_default_constructible<T>::value)
      -> optional<inline_vector> {
    if (count > capacity()) {
      return {};
    }
    return {in_place, count};
  }
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2

  /// @brief Construct a vector with the specified number of elements copied from the provided source
  /// @tparam U Dummy template parameter the same as @c T for constraints
  /// @param count The number of elements to construct in the vector
  /// @param source The element to copy
  /// @return optional<inline_vector> holding the vector if the @c count is less than or equal to the @c Capacity,
  /// otherwise returns an empty optional
  template <
      typename U = T,
      constraints<std::enable_if_t<std::is_same<U, T>::value>, std::enable_if_t<std::is_copy_constructible<U>::value>> =
          nullptr>
  static constexpr auto try_construct(size_type count, T const& source) noexcept(
      std::is_nothrow_copy_constructible<T>::value
  ) -> optional<inline_vector> {
    if (count > capacity()) {
      return {};
    }
    return {in_place, count, source};
  }

  /// @brief Construct a vector from a bidrectional iterator range
  /// @tparam Iterator The type of the iterator
  /// @tparam U Dummy template parameter the same as @c T for constraints
  /// @param first The start of the range
  /// @param last The end of the range
  /// @return optional<inline_vector> holding the resulting vector if the number of elements in the range is less than
  /// or equal to @c Capacity, an empty optional otherwise
  template <
      typename Iterator,
      typename U = T,
      constraints<
          std::enable_if_t<is_bidirectional_iterator_v<Iterator>>,
          std::enable_if_t<std::is_same<U, T>::value>,
          std::enable_if_t<std::is_constructible<U, typename std::iterator_traits<Iterator>::reference>::value>> =
          nullptr>
  static constexpr auto try_construct(Iterator first, Iterator last) noexcept(
      denotes_nothrow_iterable_range_v<Iterator> &&
      std::is_nothrow_constructible<T, typename std::iterator_traits<Iterator>::reference>::value
  ) -> optional<inline_vector> {
    if (static_cast<size_type>(arene::base::distance(first, last)) > capacity()) {
      return {};
    }
    return {in_place, first, last};
  }

  /// @brief Construct a vector from an input iterator range
  /// @tparam Iterator The type of the iterator
  /// @tparam U Dummy template parameter the same as @c T for constraints
  /// @param first The start of the range
  /// @param last The end of the range
  /// @return optional<inline_vector> holding the resulting vector if the number of elements in the range is less than
  /// or equal to @c Capacity, an empty optional otherwise
  template <
      typename Iterator,
      typename U = T,
      constraints<
          std::enable_if_t<is_input_iterator_v<Iterator>>,
          std::enable_if_t<!is_bidirectional_iterator_v<Iterator>>,
          std::enable_if_t<std::is_same<U, T>::value>,
          std::enable_if_t<std::is_constructible<U, typename std::iterator_traits<Iterator>::reference>::value>> =
          nullptr>
  static constexpr auto try_construct(Iterator first, Iterator last) noexcept(
      denotes_nothrow_iterable_range_v<Iterator> &&
      std::is_nothrow_constructible<T, typename std::iterator_traits<Iterator>::reference>::value
  ) -> optional<inline_vector> {
    optional<inline_vector> res{in_place};
    for (auto& elem : make_subrange(first, last)) {
      if (res->size() == interface_base::capacity()) {
        return {};
      }
      res->internal_emplace_back(elem);
    }
    return res;
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  /// @brief Assign to the vector from the elements of an initializer list. Assigns
  /// over existing elements, constructs additional elements, and destroys any
  /// excess.
  /// @param source_values The source values
  /// @pre @c source_values.size()<=Capacity, else @c ARENE_PRECONDITION violation
  auto operator=(std::initializer_list<T> source_values
  ) noexcept(std::is_nothrow_copy_constructible<T>::value && std::is_nothrow_copy_assignable<T>::value)
      -> inline_vector& {
    this->assign(source_values);
    return *this;
  }

  /// @brief Copy-assign to the vector from the elements of a vector with a different
  /// capacity. Assigns over existing elements, constructs additional elements,
  /// and destroys any excess.
  /// @param other The source vector
  /// @pre @c other.size()<=Capacity, else @c ARENE_PRECONDITION violation
  template <
      typename U = T,
      size_type OtherCapacity,
      constraints<
          std::enable_if_t<OtherCapacity != Capacity>,
          std::enable_if_t<std::is_copy_constructible<U>::value>,
          std::enable_if_t<std::is_copy_assignable<U>::value>> = nullptr>
  auto operator=(inline_vector<U, OtherCapacity> const& other
  ) noexcept(std::is_nothrow_copy_constructible<U>::value && std::is_nothrow_copy_assignable<U>::value)
      -> inline_vector& {
    this->assign(other.begin(), other.end());
    return *this;
  }

  // parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "False positive: Elements are moved via move_iterator"
  // parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "False positive: Elements are moved via move_iterator"
  // parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: Elements are moved via move_iterator"
  /// @brief Move-assign to the vector from the elements of a vector with a different
  /// capacity. Assigns over existing elements, constructs additional elements,
  /// and destroys any excess.
  /// @param other The source vector
  /// @pre @c other.size()<=Capacity, else @c ARENE_PRECONDITION violation
  template <size_type OtherCapacity, constraints<std::enable_if_t<OtherCapacity != Capacity>> = nullptr>
  auto operator=(inline_vector<T, OtherCapacity>&& other
  ) noexcept(std::is_nothrow_move_constructible<T>::value && std::is_nothrow_move_assignable<T>::value)
      -> inline_vector& {
    this->assign(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
    return *this;
  }
  // parasoft-end-suppress AUTOSAR-A8_4_6-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_5-a-2
  // parasoft-end-suppress AUTOSAR-A12_8_4-a-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False Positive: 'swap' does not hide anything"
  /// @brief swaps all the elements between this vector and another.
  ///
  /// @tparam U the type of the elements in the array. Must satisfy @c is_swappable_v .
  /// @param other the vector to swap with.
  /// @post The elements in @c this and @c other are exchanged 1:1 up to the @c size() of the smallest vector. Relative
  ///       order is maintained. The remaining elements from the originally larger vector are move-constructed into the
  ///       originally smaller vector maintaining relative order, and the elements in the originally larger vector are
  ///       destroyed.
  ///
  template <typename U = value_type, constraints<std::enable_if_t<is_swappable_v<U>>> = nullptr>
  constexpr void swap(inline_vector& other
  ) noexcept(is_nothrow_swappable_v<value_type> && std::is_nothrow_move_constructible<value_type>::value) {
    this->do_swap(other);
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-d

  // parasoft-begin-suppress AUTOSAR-A2_7_2-a-2 "False positive: no commented-out code"
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  /// @brief swaps all the elements between two vectors.
  ///
  /// @tparam U the type of the elements in the vector. Must satisfy @c is_swappable_v .
  /// @param lhs the left hand vector to swap.
  /// @param rhs the right hand vector to swap.
  /// @post Equivalent to having called @c lhs.swap(rhs);
  /// @see inline_vector::swap.
  ///
  template <typename U = value_type, constraints<std::enable_if_t<is_swappable_v<U>>> = nullptr>
  friend constexpr void swap(inline_vector& lhs, inline_vector& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  // parasoft-end-suppress AUTOSAR-A2_7_2-a-2

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: It is allowed to declare comparison operators as
  // friend functions"
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Compare two vectors for equality.
  ///
  /// @tparam U dummy parameter to disable if @c value_type is three-way-comparable
  /// @param lhs The first vector
  /// @param rhs The second vector
  /// @return bool equivalent to @c arene::base::equal(lhs.begin(),lhs.end(),rhs.begin(),rhs.end()) .
  template <typename U = value_type, constraints<std::enable_if_t<is_equality_comparable_v<U>>> = nullptr>
  friend constexpr auto operator==(inline_vector const& lhs, inline_vector const& rhs) noexcept -> bool {
    return ::arene::base::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: It is allowed to declare comparison operators as
  // friend functions"
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  /// @brief Compare two vectors for lexicographical ordering.
  ///
  /// @tparam U dummy parameter to disable if @c value_type is not less-than-comparable
  /// @param lhs The first vector
  /// @param rhs The second vector
  /// @return bool equivalent to @c arene::base::lexicographical_compare(lhs.begin(),lhs.end(),rhs.begin(),rhs.end()) .
  template <typename U = value_type, constraints<std::enable_if_t<is_less_than_comparable_v<U>>> = nullptr>
  friend constexpr auto operator<(inline_vector const& lhs, inline_vector const& rhs) noexcept -> bool {
    return ::arene::base::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2
};
// parasoft-end-suppress AUTOSAR-A10_1_1-a-2
// parasoft-end-suppress AUTOSAR-A12_1_5-a

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M2_10_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_VECTOR_HPP_
