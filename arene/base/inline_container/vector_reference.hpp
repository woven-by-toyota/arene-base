// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_VECTOR_REFERENCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_VECTOR_REFERENCE_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/detail/exceptions.hpp"
#include "arene/base/inline_container/external_vector.hpp"
#include "arene/base/inline_container/vector.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/is_base_of.hpp"
#include "arene/base/utility/make_subrange.hpp"

// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

// forward declaration
// IWYU pragma: begin_keep
template <typename T>
class const_vector_reference;
// IWYU pragma: end_keep

namespace vector_reference_detail {

/// @brief function pointers to be used inside const member functions
/// @tparam T The type of each element
template <typename T>
struct const_operations {
  /// @brief The type of the base class
  using vector_base = inline_vector_detail::vector_base<T>;
  /// @brief The type of a const iterator to a position in the vector
  using const_iterator = typename vector_base::const_iterator;
  /// @brief The type of a const pointer to the base class
  using vector_ptr = vector_base const*;
  // parasoft-begin-suppress CERT_C-EXP37-b-3 "False positive: The rule does not mention naming all parameters"
  /// @brief The function pointer type of the data operation
  using data_op_type = T const* (*)(vector_ptr);
  /// @brief The function pointer type of the size operation
  using size_op_type = std::size_t (*)(vector_ptr);
  /// @brief The function pointer type of the capacity operation
  using capacity_op_type = std::size_t (*)(vector_ptr);
  /// @brief The function pointer type of the empty operation
  using empty_op_type = bool (*)(vector_ptr);
  /// @brief The function pointer type of the element accessor
  using element_op_type = T const& (*)(vector_ptr, std::size_t);
  /// @brief The function pointer type of the at operation
  using at_op_type = T const& (*)(vector_ptr, std::size_t);
  /// @brief The function pointer type of the front operation
  using front_op_type = T const& (*)(vector_ptr);
  /// @brief The function pointer type of the back operation
  using back_op_type = T const& (*)(vector_ptr);
  /// @brief The function pointer type of the begin operation
  using begin_op_type = const_iterator (*)(vector_ptr);
  /// @brief The function pointer type of the end operation
  using end_op_type = const_iterator (*)(vector_ptr);
  // parasoft-end-suppress CERT_C-EXP37-b-3

  /// @brief The function pointer of the data operation
  data_op_type data;
  /// @brief The function pointer of the size operation
  size_op_type size;
  /// @brief The function pointer of the capacity operation
  capacity_op_type capacity;
  /// @brief The function pointer of the empty operation
  empty_op_type empty;
  /// @brief The function pointer of the element accessor
  element_op_type element;
  /// @brief The function pointer of the at operation
  at_op_type at;
  /// @brief The function pointer of the front operation
  front_op_type front;
  /// @brief The function pointer of the back operation
  back_op_type back;
  /// @brief The function pointer of the begin iterator
  begin_op_type begin;
  /// @brief The function pointer of the end iterator
  end_op_type end;
};

/// @brief function pointers to be used inside non-const member functions
/// @tparam T The type of each element
template <typename T>
struct operations {
  /// @brief The type of the base class
  using vector_base = inline_vector_detail::vector_base<T>;
  /// @brief The type of an iterator to a position in the vector
  using iterator = typename vector_base::iterator;
  /// @brief The type of a const iterator to a position in the vector
  using const_iterator = typename vector_base::const_iterator;
  /// @brief The type of a const pointer to the base class
  using vector_ptr = vector_base*;
  // parasoft-begin-suppress CERT_C-EXP37-b-3 "False positive: The rule does not mention naming all parameters"
  /// @brief The function pointer type of the data operation
  using data_op_type = T* (*)(vector_ptr);
  /// @brief The function pointer type of the element accessor
  using element_op_type = T& (*)(vector_ptr, std::size_t);
  /// @brief The function pointer type of the at operation
  using at_op_type = T& (*)(vector_ptr, std::size_t);
  /// @brief The function pointer type of the front operation
  using front_op_type = T& (*)(vector_ptr);
  /// @brief The function pointer type of the back operation
  using back_op_type = T& (*)(vector_ptr);
  /// @brief The function pointer type of the begin operation
  using begin_op_type = iterator (*)(vector_ptr);
  /// @brief The function pointer type of the end operation
  using end_op_type = iterator (*)(vector_ptr);
  /// @brief The function pointer type of the push_back operation
  using push_back_op_type = void (*)(vector_ptr, T const&);
  /// @brief The function pointer type of the insert operation
  using insert_op_type = iterator (*)(vector_ptr, const_iterator, T const&);
  /// @brief The function pointer type of the erase operation
  using erase_op_type = iterator (*)(vector_ptr, const_iterator);
  /// @brief The function pointer type of the erase range operation
  using erase_range_op_type = iterator (*)(vector_ptr, const_iterator, const_iterator);
  /// @brief The function pointer type of the pop_back operation
  using pop_back_op_type = void (*)(vector_ptr);
  /// @brief The function pointer type of the clear operation
  using clear_op_type = void (*)(vector_ptr);
  /// @brief The function pointer type of the resize operation
  using resize_op_type = void (*)(vector_ptr, std::size_t);
  // parasoft-end-suppress CERT_C-EXP37-b-3

  /// @brief The pointer to the object holding the const operations
  const_operations<T> const* const_ops;
  /// @brief The function pointer of the data operation
  data_op_type data;
  /// @brief The function pointer of the element accessor
  element_op_type element;
  /// @brief The function pointer of the at operation
  at_op_type at;
  /// @brief The function pointer of the front operation
  front_op_type front;
  /// @brief The function pointer of the back operation
  back_op_type back;
  /// @brief The function pointer of the begin iterator
  begin_op_type begin;
  /// @brief The function pointer of the end iterator
  end_op_type end;
  /// @brief The function pointer of the push_back operation
  push_back_op_type push_back;
  /// @brief The function pointer of the insert operation
  insert_op_type insert;
  /// @brief The function pointer of the erase operation
  erase_op_type erase;
  /// @brief The function pointer of the erase range operation
  erase_range_op_type erase_range;
  /// @brief The function pointer of the pop_back operation
  pop_back_op_type pop_back;
  /// @brief The function pointer of the clear operation
  clear_op_type clear;
  /// @brief The function pointer of the resize operation
  resize_op_type resize;
};

/// @brief implementation of the const operations for a specific vector type
/// @tparam Vector the specific vector type
template <class Vector>
class const_operations_impl {
 public:
  /// @brief The value type of the underlying vector
  using value_type = typename Vector::value_type;

  /// @brief The type of the base class of the underlying vector
  using vector_ptr = inline_vector_detail::vector_base<value_type> const*;

  // parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
  /// @brief Cast base class pointer back to the original vector type
  /// @param vec The base class pointer
  /// @return The pointer as the original vector type
  static constexpr auto vector(vector_ptr vec) noexcept -> Vector const* { return static_cast<Vector const*>(vec); }
  // parasoft-end-suppress AUTOSAR-M2_10_1-a-2

  /// @brief Obtain a pointer to the stored elements
  /// @param ptr The base class pointer
  /// @return A raw pointer to the elements
  static constexpr auto do_data(vector_ptr ptr) noexcept -> value_type const* { return vector(ptr)->data(); }
  /// @brief Get the current number of live elements in the vector
  /// @param ptr The base class pointer
  /// @return the size
  static constexpr auto do_size(vector_ptr ptr) noexcept -> std::size_t { return vector(ptr)->size(); }
  /// @brief Get the capacity
  /// @param ptr The base class pointer
  /// @return the capacity
  static constexpr auto do_capacity(vector_ptr ptr) noexcept -> std::size_t { return vector(ptr)->capacity(); }
  /// @brief Check if the vector is empty.
  /// @param ptr The base class pointer
  /// @return @c true if the vector is empty, @c false otherwise
  static constexpr auto do_empty(vector_ptr ptr) noexcept -> bool { return vector(ptr)->empty(); }
  /// @brief Obtain a const reference to the element with the specified index
  /// @param ptr The base class pointer
  /// @param index The index of the element
  /// @return A reference to the element
  /// @pre @c index must be less than @c size()
  static constexpr auto do_element(vector_ptr ptr, std::size_t index) noexcept -> value_type const& {
    return (*vector(ptr))[index];
  }
  /// @brief Obtain a @c const reference to the element with the specified index
  /// @param ptr The base class pointer
  /// @param index The index of the element
  /// @return A reference to the element
  /// @throws std::out_of_range if the index is out of range
  template <
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<std::enable_if_t<AreExceptionsEnabled>> = nullptr>
  static constexpr auto do_at(vector_ptr ptr, std::size_t index) -> value_type const& {
    return vector(ptr)->at(index);
  }
  /// @brief When exceptions are disabled, invoking this is a precondition violation.
  /// @return Never returns
  template <
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<std::enable_if_t<!AreExceptionsEnabled>> = nullptr>
  ARENE_NORETURN static constexpr auto do_at(vector_ptr, std::size_t) -> value_type const& {
    ARENE_INVARIANT_UNREACHABLE("do_at() called when exceptions are disabled");
  }
  /// @brief Get a const reference to the first element.
  /// @param ptr The base class pointer
  /// @pre The vector must not be empty
  /// @return A reference to the element
  static constexpr auto do_front(vector_ptr ptr) noexcept -> value_type const& { return vector(ptr)->front(); }
  /// @brief Get a const reference to the last element.
  /// @param ptr The base class pointer
  /// @pre The vector must not be empty
  /// @return A reference to the element
  static constexpr auto do_back(vector_ptr ptr) noexcept -> value_type const& { return vector(ptr)->back(); }
  /// @brief Obtain a const iterator to the start of the vector.
  /// @param ptr The base class pointer
  /// @return The iterator
  static constexpr auto do_begin(vector_ptr ptr) noexcept -> typename Vector::const_iterator {
    return vector(ptr)->begin();
  }
  /// @brief Obtain a const iterator to the end of the vector.
  /// @param ptr The base class pointer
  /// @return The iterator
  static constexpr auto do_end(vector_ptr ptr) noexcept -> typename Vector::const_iterator {
    return vector(ptr)->end();
  }
};

/// @brief instance of the holder of the const operations of a specific vector type
template <class Vector>
extern constexpr const_operations<typename Vector::value_type> const_vector_operations{
    &const_operations_impl<Vector>::do_data,
    &const_operations_impl<Vector>::do_size,
    &const_operations_impl<Vector>::do_capacity,
    &const_operations_impl<Vector>::do_empty,
    &const_operations_impl<Vector>::do_element,
    &const_operations_impl<Vector>::template do_at<>,
    &const_operations_impl<Vector>::do_front,
    &const_operations_impl<Vector>::do_back,
    &const_operations_impl<Vector>::do_begin,
    &const_operations_impl<Vector>::do_end
};

/// @brief implementation of the operations for a specific vector type
/// @tparam Vector the specific vector type
template <typename Vector>
class operations_impl {
 public:
  /// @brief The value type of the underlying vector
  using value_type = typename Vector::value_type;

  /// @brief The non-const iterator type
  using iterator = typename Vector::iterator;
  /// @brief The @c const iterator type
  using const_iterator = typename Vector::const_iterator;
  /// @brief A pointer to the vector base class
  using vector_ptr = inline_vector_detail::vector_base<value_type>*;

  // parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
  /// @brief Cast base class pointer back to the original vector type
  /// @param vec The base class pointer
  /// @return The pointer as the original vector type
  static constexpr auto vector(vector_ptr vec) noexcept -> Vector* { return static_cast<Vector*>(vec); }
  // parasoft-end-suppress AUTOSAR-M2_10_1-a-2

  /// @brief Forward the call to the object's member function
  /// @param ptr The base class pointer
  /// @return The return value of the called function
  static constexpr auto do_data(vector_ptr ptr) noexcept -> value_type* { return vector(ptr)->data(); }

  /// @brief Forward the call to the object's member function
  /// @param ptr The base class pointer
  /// @param index The index of the element
  /// @return The return value of the called function
  static constexpr auto do_element(vector_ptr ptr, std::size_t index) noexcept -> value_type& {
    return (*vector(ptr))[index];
  }
  /// @brief Forward the call to the object's member function
  /// @param ptr The base class pointer
  /// @param index The index of the element
  /// @return The return value of the called function
  template <
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<std::enable_if_t<AreExceptionsEnabled>> = nullptr>
  static constexpr auto do_at(vector_ptr ptr, std::size_t index) -> value_type& {
    return vector(ptr)->at(index);
  }
  /// @brief Specialization for when exceptions are disabled, is always a precondition violation.
  /// @return Never returns.
  template <
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<std::enable_if_t<!AreExceptionsEnabled>> = nullptr>
  ARENE_NORETURN static constexpr auto do_at(vector_ptr, std::size_t) -> value_type& {
    ARENE_INVARIANT_UNREACHABLE("do_at() called when exceptions are disabled");
  }

  /// @brief Forward the call to the object's member function
  /// @param ptr The base class pointer
  /// @return The return value of the called function
  static constexpr auto do_front(vector_ptr ptr) noexcept -> value_type& { return vector(ptr)->front(); }
  /// @brief Forward the call to the object's member function
  /// @param ptr The base class pointer
  /// @return The return value of the called function
  static constexpr auto do_back(vector_ptr ptr) noexcept -> value_type& { return vector(ptr)->back(); }

  /// @brief Forward the call to the object's member function
  /// @param ptr The base class pointer
  /// @return The return value of the called function
  static constexpr auto do_begin(vector_ptr ptr) noexcept -> iterator { return vector(ptr)->begin(); }
  /// @brief Forward the call to the object's member function
  /// @param ptr The base class pointer
  /// @return The return value of the called function
  static constexpr auto do_end(vector_ptr ptr) noexcept -> iterator { return vector(ptr)->end(); }

  /// @brief Forward the call to the object's member function
  /// @param ptr The base class pointer
  /// @param value The value to copy
  static constexpr void do_push_back(vector_ptr ptr, value_type const& value) noexcept {
    vector(ptr)->push_back(value);
  }
  /// @brief Forward the call to the object's member function
  /// @param ptr The base class pointer
  /// @param pos The position to insert the new element
  /// @param element The value to copy
  /// @return The return value of the called function
  static constexpr auto do_insert(vector_ptr ptr, const_iterator pos, value_type const& element) noexcept -> iterator {
    return vector(ptr)->insert(pos, element);
  }

  /// @brief Forward the call to the object's member function
  /// @param ptr The base class pointer
  /// @param pos An iterator referring to the element to erase
  /// @return The return value of the called function
  static constexpr auto do_erase(vector_ptr ptr, const_iterator pos) noexcept -> iterator {
    return vector(ptr)->erase(pos);
  }
  /// @brief Forward the call to the object's member function
  /// @param ptr The base class pointer
  /// @param first An iterator referring to the start of the range to erase
  /// @param last An iterator referring to the end of the range to erase
  /// @return The return value of the called function
  static constexpr auto do_erase_range(vector_ptr ptr, const_iterator first, const_iterator last) noexcept -> iterator {
    return vector(ptr)->erase(first, last);
  }

  /// @brief Obtain a const iterator to the end of the vector.
  /// @param ptr The base class pointer
  static constexpr void do_pop_back(vector_ptr ptr) noexcept { vector(ptr)->pop_back(); }

  /// @brief Obtain a const iterator to the end of the vector.
  /// @param ptr The base class pointer
  static constexpr void do_clear(vector_ptr ptr) noexcept { vector(ptr)->clear(); }

  /// @brief Resize the vector to the specified size. Default-constructs any missing
  /// elements, and destroys any excess.
  /// @param ptr The base class pointer
  /// @param count The desired size
  /// @pre @c count<=Capacity, else @c ARENE_PRECONDITION violation
  static constexpr void do_resize(vector_ptr ptr, std::size_t count) noexcept { vector(ptr)->resize(count); }
};

/// @brief instance of the holder of the operations of a specific vector type
template <class Vector>
extern constexpr operations<typename Vector::value_type> vector_operations{
    &const_vector_operations<Vector>,
    &operations_impl<Vector>::do_data,
    &operations_impl<Vector>::do_element,
    &operations_impl<Vector>::template do_at<>,
    &operations_impl<Vector>::do_front,
    &operations_impl<Vector>::do_back,
    &operations_impl<Vector>::do_begin,
    &operations_impl<Vector>::do_end,
    &operations_impl<Vector>::do_push_back,
    &operations_impl<Vector>::do_insert,
    &operations_impl<Vector>::do_erase,
    &operations_impl<Vector>::do_erase_range,
    &operations_impl<Vector>::do_pop_back,
    &operations_impl<Vector>::do_clear,
    &operations_impl<Vector>::do_resize
};

// parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Passkey idiom permitted by A11-3-1 Permit #1"
/// @brief Passkey type for internal access
class ivref_passkey {
  /// @brief Prevent construction from @c {}
  explicit ivref_passkey() = default;
  /// @brief friend declaration to allow this class to construct the passkey
  template <typename T>
  friend class ::arene::base::const_vector_reference;
};
// parasoft-end-suppress AUTOSAR-A11_3_1-a-2

}  // namespace vector_reference_detail

// parasoft-begin-suppress AUTOSAR-A13_5_1-a-2 "False positive: This follows the std::map interface. The non-const
// parasoft-begin-suppress AUTOSAR-A12_1_5-a-2 "False positive: no duplication"
/// @brief A reference class to an inline_vector object, with size-erased type
/// @tparam T The type of each element
template <typename T>
class vector_reference {
  /// @brief A pointer to the base class
  inline_vector_detail::vector_base<T>* vector_ptr_;
  /// @brief A pointer to the struct holding operation implementations
  vector_reference_detail::operations<T> const* op_ptrs_;

 public:
  /// @brief The value type
  using value_type = T;
  /// @brief The size type
  using size_type = std::size_t;
  /// @brief The iterator type of the underlying vector
  using iterator = typename inline_vector_detail::vector_base<T>::iterator;
  /// @brief The const iterator type of the underlying vector
  using const_iterator = typename inline_vector_detail::vector_base<T>::const_iterator;

  /// @brief Construct a vector reference to the supplied inline_vector
  /// @tparam Capacity The maximum number of elements that can be stored
  /// @param vec The inline_vector to reference
  template <std::size_t Capacity>
  constexpr explicit vector_reference(inline_vector<T, Capacity>& vec)
      : vector_ptr_(&vec),
        op_ptrs_(&vector_reference_detail::vector_operations<inline_vector<T, Capacity>>) {}

  /// @brief Construct a vector reference to the supplied external_vector
  /// @param vec The external_vector to reference
  constexpr explicit vector_reference(external_vector<T>& vec)
      : vector_ptr_(&vec),
        op_ptrs_(&vector_reference_detail::vector_operations<external_vector<T>>) {}

  /// @brief Obtain a pointer to the stored elements
  /// @return A raw pointer to the elements
  constexpr auto data() const noexcept -> T* { return op_ptrs_->data(vector_ptr_); }

  /// @brief Get the current number of live elements in the vector
  /// @return the size
  constexpr auto size() const noexcept -> size_type { return op_ptrs_->const_ops->size(vector_ptr_); }

  /// @brief Get the capacity
  /// @return the capacity
  constexpr auto capacity() const noexcept -> size_type { return op_ptrs_->const_ops->capacity(vector_ptr_); }
  /// @brief Check if the vector is empty.
  /// @return @c true if the vector is empty, @c false otherwise
  constexpr auto max_size() const noexcept -> size_type { return op_ptrs_->const_ops->capacity(vector_ptr_); }

  /// @brief Check if the vector is empty.
  /// @return @c true if the vector is empty, @c false otherwise
  constexpr auto empty() const noexcept -> bool { return op_ptrs_->const_ops->empty(vector_ptr_); }

  /// @brief Obtain a const reference to the element with the specified index
  /// @param index The index of the element
  /// @return A reference to the element
  /// @pre @c index must be less than @c size()
  constexpr auto operator[](size_type index) const noexcept -> T& { return op_ptrs_->element(vector_ptr_, index); }
  /// @brief Obtain a @c const reference to the element with the specified index
  /// @tparam AreExceptionsEnabled Used to disable this overload if exceptions are not enabled.
  /// @param index The index of the element
  /// @return A reference to the element
  /// @throws std::out_of_range if the index is out of range
  template <
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<std::enable_if_t<AreExceptionsEnabled>> = nullptr>
  constexpr auto at(size_type index) const -> T& {
    return op_ptrs_->at(vector_ptr_, index);
  }

  /// @brief Get a const reference to the first element.
  /// @return A reference to the element
  /// @pre The vector must not be empty
  constexpr auto front() const noexcept -> T& { return op_ptrs_->front(vector_ptr_); }
  /// @brief Get a const reference to the last element.
  /// @return A reference to the element
  /// @pre The vector must not be empty
  constexpr auto back() const noexcept -> T& { return op_ptrs_->back(vector_ptr_); }

  /// @brief Obtain a const iterator to the start of the vector.
  /// @return The iterator
  constexpr auto begin() const noexcept -> iterator { return op_ptrs_->begin(vector_ptr_); }
  /// @brief Obtain a const iterator to the end of the vector.
  /// @return The iterator
  constexpr auto end() const noexcept -> iterator { return op_ptrs_->end(vector_ptr_); }
  /// @brief Obtain an const iterator to the start of the vector.
  /// @return The iterator
  constexpr auto cbegin() const noexcept -> const_iterator { return op_ptrs_->const_ops->begin(vector_ptr_); }
  /// @brief Obtain a const iterator to the end of the vector.
  /// @return The iterator
  constexpr auto cend() const noexcept -> const_iterator { return op_ptrs_->const_ops->end(vector_ptr_); }

  /// @brief Add a new element to the end of the vector as a copy of the supplied value.
  /// @param element The value to copy
  /// @pre @c size()<Capacity, else @c ARENE_PRECONDITION violation
  constexpr void push_back(T const& element) const noexcept { return op_ptrs_->push_back(vector_ptr_, element); }

  /// @brief Insert a new element at the specified position in the vector by
  /// copy-constructing or copy-assigning from the supplied value. Any existing
  /// elements from the specified position to the end of the vector are moved to
  /// make room.
  /// @param pos The position to insert the new element
  /// @param value The value to copy from
  /// @return An iterator referring to the new element
  /// @pre @c size()<Capacity, else @c ARENE_PRECONDITION violation
  constexpr auto insert(const_iterator pos, T const& value) const noexcept -> iterator {
    return op_ptrs_->insert(vector_ptr_, pos, value);
  }

  /// @brief Insert a range of elements at the specified position.
  /// @param pos An iterator referring to the location to insert the elemnets
  /// @param first An iterator referring to the start of the range to insert
  /// @param last An iterator referring to the end of the range to insert
  /// @return An iterator referring to the first inserted value if any, or representing the same position as @c pos
  /// otherwise
  /// @pre @c pos Must be a valid iterator to @c this
  template <typename InputIterator>
  constexpr auto insert(const_iterator pos, InputIterator first, InputIterator last) const -> iterator {
    auto ncpos = begin() + (pos - begin());
    // parasoft-begin-suppress AUTOSAR-A7_1_5-a "False Positive: 'element' has return type of range element, which may
    // be non-fundamental"
    for (auto&& elem : make_subrange(first, last)) {
      // parasoft-end-suppress AUTOSAR-A7_1_5-a
      std::ignore = this->insert(pos++, elem);
    }
    return ncpos;
  }

  /// @brief Erase the element at the specified position.
  /// @param iter An iterator referring to the element to erase
  /// @return An iterator to the first element following the erased element, or @c end() if there is no such element
  /// @pre @c pos Must be a valid iterator to a live element in @c this
  constexpr auto erase(const_iterator iter) const noexcept -> iterator { return op_ptrs_->erase(vector_ptr_, iter); }
  /// @brief Erase all the element in the specified range.
  /// @param first An iterator referring to the start of the range to erase
  /// @param last An iterator referring to the end of the range to erase
  /// @return An iterator to the first element following the erased elements, or @c end() if there is no such element
  /// @pre @c [first,last)  Must be a valid iterator range in @c this
  constexpr auto erase(const_iterator first, const_iterator last) const noexcept -> iterator {
    return op_ptrs_->erase_range(vector_ptr_, first, last);
  }
  /// @brief Destroy the last element in the vector and decrease the size
  /// @pre The vector must not be empty
  constexpr void pop_back() const noexcept { op_ptrs_->pop_back(vector_ptr_); }

  /// @brief Destroy all elements and set the size to zero
  constexpr void clear() const noexcept { op_ptrs_->clear(vector_ptr_); }

  /// @brief Resize the vector to the specified size. Default-constructs any missing
  /// elements, and destroys any excess.
  /// @param count The desired size
  /// @pre @c count<=Capacity, else @c ARENE_PRECONDITION violation
  constexpr void resize(size_type count) const { op_ptrs_->resize(vector_ptr_, count); }

  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"

  /// @brief Get a pointer to the underlying vector object
  /// @return A pointer to the underlying vector object
  constexpr auto get_vector_ptr(vector_reference_detail::ivref_passkey) const noexcept
      -> inline_vector_detail::vector_base<T>* {
    return vector_ptr_;
  }

  /// @brief Get a pointer to the struct holding const operation implementations
  /// @return A pointer to the struct holding const operation implementations
  constexpr auto const_operations_ptr(vector_reference_detail::ivref_passkey) const noexcept
      -> vector_reference_detail::const_operations<T> const* {
    return op_ptrs_->const_ops;
  }

  // parasoft-end-suppress CERT_C-EXP37-a
};
// parasoft-end-suppress AUTOSAR-A12_1_5-a-2
// parasoft-end-suppress AUTOSAR-A13_5_1-a-2

// parasoft-begin-suppress AUTOSAR-A12_1_5-a-2 "False positive: no duplication"
// parasoft-begin-suppress AUTOSAR-A13_5_1-a-2 "False positive: Only const operator[] provided"
/// @brief A const reference class to an inline_vector object, with size-erased type
/// @tparam T The type of each element
template <typename T>
class const_vector_reference {
  /// @brief A pointer to the base class
  inline_vector_detail::vector_base<T> const* vector_ptr_;
  /// @brief A pointer to the struct holding operation implementations
  vector_reference_detail::const_operations<T> const* op_ptrs_;

 public:
  /// @brief The value type
  using value_type = T;
  /// @brief The size type
  using size_type = std::size_t;
  /// @brief The const iterator type of the underlying vector
  using const_iterator = typename inline_vector_detail::vector_base<T>::const_iterator;
  /// @brief The iterator type to be used in this const context
  using iterator = const_iterator;

  /// @brief Construct a vector reference to the supplied inline_vector
  /// @tparam Capacity The maximum number of elements that can be stored
  /// @param vec The inline_vector to reference
  template <std::size_t Capacity>
  constexpr explicit const_vector_reference(inline_vector<T, Capacity> const& vec)
      : vector_ptr_(&vec),
        op_ptrs_(&vector_reference_detail::const_vector_operations<inline_vector<T, Capacity>>) {}

  /// @brief Construct a vector reference to the supplied external_vector
  /// @param vec The external_vector to reference
  constexpr explicit const_vector_reference(external_vector<T> const& vec)
      : vector_ptr_(&vec),
        op_ptrs_(&vector_reference_detail::const_vector_operations<external_vector<T>>) {}

  /// @brief Construct a const reference out of a non-const one
  /// @param ref The non-const reference class
  constexpr explicit const_vector_reference(vector_reference<T> const ref)
      : vector_ptr_(ref.get_vector_ptr(vector_reference_detail::ivref_passkey{})),
        op_ptrs_(ref.const_operations_ptr(vector_reference_detail::ivref_passkey{})) {}

  /// @brief Obtain a pointer to the stored elements
  /// @return A raw pointer to the elements
  constexpr auto data() const noexcept -> T const* { return op_ptrs_->data(vector_ptr_); }

  /// @brief Get the current number of live elements in the vector
  /// @return the size
  constexpr auto size() const noexcept -> size_type { return op_ptrs_->size(vector_ptr_); }

  /// @brief Get the capacity
  /// @return the capacity
  constexpr auto capacity() const noexcept -> size_type { return op_ptrs_->capacity(vector_ptr_); }
  /// @brief Check if the vector is empty.
  /// @return @c true if the vector is empty, @c false otherwise
  constexpr auto max_size() const noexcept -> size_type { return op_ptrs_->capacity(vector_ptr_); }

  /// @brief Check if the vector is empty.
  /// @return @c true if the vector is empty, @c false otherwise
  constexpr auto empty() const noexcept -> bool { return op_ptrs_->empty(vector_ptr_); }

  /// @brief Obtain a const reference to the element with the specified index
  /// @param index The index of the element
  /// @return A reference to the element
  /// @pre @c index must be less than @c size()
  constexpr auto operator[](size_type index) const noexcept -> T const& {
    return op_ptrs_->element(vector_ptr_, index);
  }
  /// @brief Obtain a @c const reference to the element with the specified index
  /// @tparam AreExceptionsEnabled Used to disable this overload if exceptions are not enabled.
  /// @param index The index of the element
  /// @return A reference to the element
  /// @throws std::out_of_range if the index is out of range
  template <
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<std::enable_if_t<AreExceptionsEnabled>> = nullptr>
  constexpr auto at(size_type index) const -> T const& {
    return op_ptrs_->at(vector_ptr_, index);
  }

  /// @brief Get a const reference to the first element.
  /// @return A reference to the first element
  /// @pre The vector must not be empty
  constexpr auto front() const noexcept -> T const& { return op_ptrs_->front(vector_ptr_); }
  /// @brief Get a const reference to the last element.
  /// @return A reference to the lastelement
  /// @pre The vector must not be empty
  constexpr auto back() const noexcept -> T const& { return op_ptrs_->back(vector_ptr_); }

  /// @brief Obtain a const iterator to the start of the vector.
  /// @return The iterator
  constexpr auto begin() const noexcept -> const_iterator { return op_ptrs_->begin(vector_ptr_); }
  /// @brief Obtain a const iterator to the end of the vector.
  /// @return The iterator
  constexpr auto end() const noexcept -> const_iterator { return op_ptrs_->end(vector_ptr_); }
  /// @brief Obtain an const iterator to the start of the vector.
  /// @return The iterator
  constexpr auto cbegin() const noexcept -> const_iterator { return op_ptrs_->begin(vector_ptr_); }
  /// @brief Obtain a const iterator to the end of the vector.
  /// @return The iterator
  constexpr auto cend() const noexcept -> const_iterator { return op_ptrs_->end(vector_ptr_); }
};
// parasoft-end-suppress AUTOSAR-A13_5_1-a-2
// parasoft-end-suppress AUTOSAR-A12_1_5-a-2

namespace vector_reference_detail {

/// @brief helper type used to define the @c make_const_vector_reference global
///     function object
///
class make_const_vector_reference_fn {
 public:
  /// @brief creates a @c const_vector_reference, deducing the value type
  ///     from the argument
  /// @tparam Vector the type of the vector
  /// @param vec @c inline_vector to create a reference from
  ///
  /// Helper to create an object of type @c const_vector_reference, using
  /// template argument deduction to determine the template argument of the
  /// result.
  ///
  template <
      class Vector,
      constraints<std::enable_if_t<
          std::is_base_of<inline_vector_detail::vector_base<typename Vector::value_type>, Vector>::value>> = nullptr>
  constexpr auto operator()(Vector const& vec) const noexcept -> const_vector_reference<typename Vector::value_type> {
    return const_vector_reference<typename Vector::value_type>{vec};
  }

  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"

  /// @brief creates a @c const_vector_reference, deducing the value type
  ///     from the argument
  /// @tparam Vector the type of the vector
  ///
  /// Helper to create an object of type @c const_vector_reference, using
  /// template argument deduction to determine the template argument of the
  /// result.
  ///
  /// @note This overload is deleted to prevent construction from an rvalue
  ///     reference.
  ///
  template <
      class Vector,
      constraints<std::enable_if_t<
          std::is_base_of<inline_vector_detail::vector_base<typename Vector::value_type>, Vector>::value>> = nullptr>
  constexpr auto operator()(Vector const&& vec) const noexcept = delete;

  // parasoft-end-suppress CERT_C-EXP37-a
};

/// @brief helper type used to define the @c make_vector_reference global
///     function object
///
class make_vector_reference_fn : make_const_vector_reference_fn {
 public:
  /// @brief creates a @c vector_reference, deducing the value type from
  ///     the argument
  /// @tparam Vector the type of the vector
  /// @param vec @c vector to create a reference from
  ///
  /// Helper to create an object of type @c vector_reference, using
  /// template argument deduction to determine the template argument of the
  /// result.
  ///
  template <
      class Vector,
      constraints<std::enable_if_t<
          std::is_base_of<inline_vector_detail::vector_base<typename Vector::value_type>, Vector>::value>> = nullptr>
  constexpr auto operator()(Vector& vec) const noexcept -> vector_reference<typename Vector::value_type> {
    return vector_reference<typename Vector::value_type>{vec};
  }

  // otherwise create a const_vector_reference
  using make_const_vector_reference_fn::operator();
};

}  // namespace vector_reference_detail

// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to
// create a per-TU reference to a global object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to
// create a per-TU reference to a global object used in multiple TUs."

namespace {

// parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: variable is initialized"

/// @def arene::base::make_vector_reference
/// @brief creates a @c vector_reference or @c
///     const_vector_reference, deducing the value type from the argument
///
/// Global function object used to create a @c vector_reference from an
/// @c inline_vector.
///
ARENE_CPP14_INLINE_VARIABLE(vector_reference_detail::make_vector_reference_fn, make_vector_reference);

/// @def arene::base::make_const_vector_reference
/// @brief creates a @c const_vector_reference, deducing the value type from
///     the argument
///
/// Global function object used to create a @c const_vector_reference from an
/// @c inline_vector.
///
ARENE_CPP14_INLINE_VARIABLE(vector_reference_detail::make_const_vector_reference_fn, make_const_vector_reference);

// parasoft-end-suppress CERT_CPP-DCL56-a

/// @brief Alias for backward compatibility
/// @tparam T The type of each element
/// @deprecated Prefer to use @c vector_reference, as it supports both @c inline_vector and @c external_vector
template <class T>
using inline_vector_reference = vector_reference<T>;

/// @brief Alias for backward compatibility
/// @tparam T The type of each element
/// @deprecated Prefer to use @c vector_reference, as it supports both @c inline_vector and @c external_vector
template <class T>
using const_inline_vector_reference = const_vector_reference<T>;

}  // namespace

// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_VECTOR_REFERENCE_HPP_
