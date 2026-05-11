// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_INLINE_STRING_REFERENCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_INLINE_STRING_REFERENCE_HPP_

// IWYU pragma: private
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compare/operators.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/detail/exceptions.hpp"
#include "arene/base/detail/raw_c_string.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/strings/inline_string.hpp"
#include "arene/base/strings/string_view.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

namespace arene {
namespace base {

// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
// parasoft-begin-suppress AUTOSAR-A13_5_1-a "False positive:" rule specifies
// that a const overload of 'operator[]' is required if a non-const overload of
// 'operator[]' is defined. These classes only define const overloads."

// forward declaration
class const_inline_string_reference;

namespace inline_string_reference_detail {

/// @brief function pointers to be used inside const member functions
struct const_operations {
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: does not hide anything."
  /// @brief The type of the base class
  using string_base = inline_string_detail::inline_string_base;
  /// @brief The type of a const pointer to the base class
  using string_ptr = string_base const*;
  /// @brief The type of a size at the base class
  using size_type = string_base::size_type;
  /// @brief The type of a const iterator at the base class
  using const_iterator = string_base::const_iterator;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d "False positive: does not hide anything."

  // parasoft-begin-suppress CERT_C-EXP37-b-3 "False positive: The rule does not mention naming all parameters"
  /// @brief The function pointer type of the data operation
  using data_op_type = detail::raw_c_string (*)(string_ptr);
  /// @brief The function pointer type of the size operation
  using size_op_type = size_type (*)(string_ptr);
  /// @brief The function pointer type of the capacity operation
  using capacity_op_type = size_type (*)(string_ptr);
  /// @brief The function pointer type of the empty operation
  using empty_op_type = bool (*)(string_ptr);
  /// @brief The function pointer type of the element accessor
  using element_op_type = detail::character const& (*)(string_ptr, size_type);
  /// @brief The function pointer type of the at operation
  using at_op_type = detail::character const& (*)(string_ptr, size_type);
  /// @brief The function pointer type of the front operation
  using front_op_type = detail::character const& (*)(string_ptr);
  /// @brief The function pointer type of the back operation
  using back_op_type = detail::character const& (*)(string_ptr);
  /// @brief The function pointer type of the begin operation
  using begin_op_type = const_iterator (*)(string_ptr);
  /// @brief The function pointer type of the end operation
  using end_op_type = const_iterator (*)(string_ptr);
  /// @brief The function pointer type of the to_string_view operation
  using to_string_view_op_type = string_view (*)(string_ptr);
  // parasoft-end-suppress CERT_C-EXP37-b-3 "False positive: The rule does not mention naming all parameters"

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
  /// @brief The function pointer of the to_string_view operation
  to_string_view_op_type to_string_view;
};

/// @brief Implementation of the const operations of a specific MaxSize
/// @tparam MaxSize The maximum capacity of the string, _not_ including the null terminator
template <std::size_t MaxSize>
class const_operations_impl {
 public:
  /// @brief The type of the underlying string
  using this_string = inline_string<MaxSize>;

  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: does not hide anything."
  /// @brief The type of a pointer to the base class of the underlying string
  using string_ptr = const_operations::string_ptr;
  /// @brief The type of a size at the base class
  using size_type = const_operations::size_type;
  /// @brief The type of a const iterator at the base class
  using const_iterator = const_operations::const_iterator;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d "False positive: does not hide anything."

  /// @brief Cast base class pointer back to the underlying string type
  /// @param ptr The base class pointer
  /// @return The pointer as the underlying string type
  static constexpr auto string(string_ptr ptr) noexcept -> this_string const* {
    return static_cast<inline_string<MaxSize> const*>(ptr);
  }

  /// @brief Return a pointer to the NUL-terminated string held in @c *this
  /// @param ptr The base class pointer
  /// @return a pointer to the NUL-terminated string held in @c *this
  static constexpr auto do_data(string_ptr ptr) noexcept -> detail::raw_c_string { return string(ptr)->data(); }

  /// @brief Get the length of the string
  /// @param ptr The base class pointer
  /// @return The number of characters in the string, non-inclusive of the null-terminator
  static constexpr auto do_size(string_ptr ptr) noexcept -> size_type { return string(ptr)->size(); }

  /// @brief The maximum length of the string
  /// @param ptr The base class pointer
  /// @return size_type @c MaxSize
  static constexpr auto do_capacity(string_ptr ptr) noexcept -> size_type { return string(ptr)->capacity(); }

  /// @brief Check if the string is empty
  /// @param ptr The base class pointer
  /// @return @c true if the string is empty, @c false otherwise
  static constexpr auto do_empty(string_ptr ptr) noexcept -> bool { return string(ptr)->empty(); }

  /// @brief Retrieve the @c index -th character in the string
  /// @param ptr The base class pointer
  /// @param index The index of the character to retrieve
  /// @return A reference to that character
  static constexpr auto do_element(string_ptr ptr, size_type index) noexcept -> detail::character const& {
    return (*string(ptr))[index];
  }
  /// @brief Retrieve the @c index -th character in the string
  /// @tparam AreExceptionsEnabled Used to disable this overload if exceptions are not enabled.
  /// @param ptr The base class pointer
  /// @param index The index of the character to retrieve
  /// @return A reference to that character
  template <
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<std::enable_if_t<AreExceptionsEnabled>> = nullptr>
  static constexpr auto do_at(string_ptr ptr, size_type index) -> detail::character const& {
    return string(ptr)->at(index);
  }
  /// @brief When exceptions are disabled, invoking this is a precondition violation.
  /// @tparam AreExceptionsEnabled Used to enable this overload if exceptions are not enabled.
  /// @return Never returns
  template <
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<std::enable_if_t<!AreExceptionsEnabled>> = nullptr>
  ARENE_NORETURN static auto do_at(string_ptr, std::size_t) -> detail::character const& {
    ARENE_INVARIANT_UNREACHABLE("do_at() called when exceptions are disabled");
  }

  /// @brief Get the first character in the string
  /// @param ptr The base class pointer
  /// @return A reference to the first character
  static constexpr auto do_front(string_ptr ptr) noexcept -> detail::character const& { return string(ptr)->front(); }
  /// @brief Get the last character in the string
  /// @param ptr The base class pointer
  /// @return A reference to the last character
  static constexpr auto do_back(string_ptr ptr) noexcept -> detail::character const& { return string(ptr)->back(); }

  /// @brief Iterator to the first position in the string
  /// @param ptr The base class pointer
  /// @return An iterator to the first element if not empty, @c end() otherwise
  static constexpr auto do_begin(string_ptr ptr) noexcept -> const_iterator { return string(ptr)->begin(); }
  /// @brief The one-past-the-last position in the string
  /// @param ptr The base class pointer
  /// @return An iterator to the one-past-the-last position in the string
  static constexpr auto do_end(string_ptr ptr) noexcept -> const_iterator { return string(ptr)->end(); }

  /// @brief Convert to string view
  /// @param ptr The base class pointer
  /// @return A string_view object referencing the data
  static constexpr auto do_to_string_view(string_ptr ptr) noexcept -> string_view {
    return {do_data(ptr), do_size(ptr)};
  }
};

/// @brief Instance of a holder of the const operations of a specific size
/// @tparam MaxSize The maximum capacity of the string, _not_ including the null terminator
template <std::size_t MaxSize>
extern constexpr const_operations const_string_operations{
    &const_operations_impl<MaxSize>::do_data,
    &const_operations_impl<MaxSize>::do_size,
    &const_operations_impl<MaxSize>::do_capacity,
    &const_operations_impl<MaxSize>::do_empty,
    &const_operations_impl<MaxSize>::do_element,
    &const_operations_impl<MaxSize>::do_at,
    &const_operations_impl<MaxSize>::do_front,
    &const_operations_impl<MaxSize>::do_back,
    &const_operations_impl<MaxSize>::do_begin,
    &const_operations_impl<MaxSize>::do_end,
    &const_operations_impl<MaxSize>::do_to_string_view,
};

/// @brief function pointers to be used inside member functions
struct operations {
  /// @brief The type of the base class
  using string_base = inline_string_detail::inline_string_base;
  /// @brief The type of a pointer to the base class
  using string_ptr = string_base*;
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: does not hide anything."
  /// @brief The type of a size at the base class
  using size_type = string_base::size_type;
  /// @brief The type of an iterator at the base class
  using iterator = string_base::iterator;
  /// @brief The type of a const iterator at the base class
  using const_iterator = string_base::const_iterator;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d "False positive: does not hide anything."

  // parasoft-begin-suppress CERT_C-EXP37-b-3 "False positive: The rule does not mention naming all parameters"
  /// @brief The function pointer type of the element accessor
  using element_op_type = detail::character& (*)(string_ptr, size_type);
  /// @brief The function pointer type of the at operation
  using at_op_type = detail::character& (*)(string_ptr, size_type);
  /// @brief The function pointer type of the front operation
  using front_op_type = detail::character& (*)(string_ptr);
  /// @brief The function pointer type of the back operation
  using back_op_type = detail::character& (*)(string_ptr);
  /// @brief The function pointer type of the begin operation
  using begin_op_type = iterator (*)(string_ptr);
  /// @brief The function pointer type of the end operation
  using end_op_type = iterator (*)(string_ptr);
  /// @brief The function pointer type of the to_string_view operation
  using to_string_view_op_type = string_view (*)(string_ptr);
  /// @brief The function pointer type of the assign operation
  using assign_op_type = string_ptr (*)(string_ptr, string_view);
  /// @brief The function pointer type of the append operation
  using append_op_type = string_ptr (*)(string_ptr, string_view);
  /// @brief The function pointer type of the append count char operation
  using append_count_char_op_type = string_ptr (*)(string_ptr, size_type, detail::character);
  /// @brief The function pointer type of the erase operation
  using erase_op_type = string_ptr (*)(string_ptr, size_type, size_type);
  /// @brief The function pointer type of the erase by iterator operation
  using erase_it_op_type = iterator (*)(string_ptr, const_iterator, const_iterator);
  // parasoft-end-suppress CERT_C-EXP37-b-3 "False positive: The rule does not mention naming all parameters"

  /// @brief The pointer to the object holding the const operations
  const_operations const* const_ops;
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
  /// @brief The function pointer of the to_string_view operation
  to_string_view_op_type to_string_view;
  /// @brief The function pointer of the assign operation
  assign_op_type assign;
  /// @brief The function pointer of the append operation
  append_op_type append;
  /// @brief The function pointer of the append count char operation
  append_count_char_op_type append_count_char;
  /// @brief The function pointer of the erase operation
  erase_op_type erase;
  /// @brief The function pointer of the erase by iterator operation
  erase_it_op_type erase_it;
};

/// @brief Implementation of the operations of a specific size
/// @tparam MaxSize The maximum capacity of the string, _not_ including the null terminator
template <std::size_t MaxSize>
class operations_impl {
 public:
  /// @brief The type of the underlying string
  using this_string = inline_string<MaxSize>;
  /// @brief The type of a pointer to the base class of the underlying string
  using string_ptr = inline_string_detail::inline_string_base*;

  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: does not hide anything."
  /// @brief The type of a size at the base class
  using size_type = operations::size_type;
  /// @brief The type of an iterator at the base class
  using iterator = operations::iterator;
  /// @brief The type of a const iterator at the base class
  using const_iterator = operations::const_iterator;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d "False positive: does not hide anything."

  /// @brief Cast base class pointer back to the underlying string type
  /// @param ptr The base class pointer
  /// @return The pointer as the underlying string type
  static constexpr auto string(string_ptr ptr) noexcept -> this_string* {
    return static_cast<inline_string<MaxSize>*>(ptr);
  }

  /// @brief Retrieve the @c index -th character in the string
  /// @param ptr The base class pointer
  /// @param index The index of the character to retrieve
  /// @return A reference to that character
  static constexpr auto do_element(string_ptr ptr, size_type index) noexcept -> detail::character& {
    return (*string(ptr))[index];
  }
  /// @brief Retrieve the @c index -th character in the string
  /// @tparam AreExceptionsEnabled Used to disable this overload if exceptions are not enabled.
  /// @param ptr The base class pointer
  /// @param index The index of the character to retrieve
  /// @return A reference to that character
  template <
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<std::enable_if_t<AreExceptionsEnabled>> = nullptr>
  static constexpr auto do_at(string_ptr ptr, size_type index) -> detail::character& {
    return string(ptr)->at(index);
  }
  /// @brief When exceptions are disabled, invoking this is a precondition violation.
  /// @tparam AreExceptionsEnabled Used to enable this overload if exceptions are not enabled.
  /// @return Never returns
  template <
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<std::enable_if_t<!AreExceptionsEnabled>> = nullptr>
  ARENE_NORETURN static auto do_at(string_ptr, std::size_t) -> detail::character& {
    ARENE_INVARIANT_UNREACHABLE("do_at() called when exceptions are disabled");
  }

  /// @brief Get the first character in the string
  /// @param ptr The base class pointer
  /// @return A reference to the first character
  static constexpr auto do_front(string_ptr ptr) noexcept -> detail::character& { return string(ptr)->front(); }
  /// @brief Get the last character in the string
  /// @param ptr The base class pointer
  /// @return A reference to the last character
  static constexpr auto do_back(string_ptr ptr) noexcept -> detail::character& { return string(ptr)->back(); }

  /// @brief Iterator to the first position in the string
  /// @param ptr The base class pointer
  /// @return An iterator to the first element if not empty, @c end() otherwise
  static constexpr auto do_begin(string_ptr ptr) noexcept -> iterator { return string(ptr)->begin(); }
  /// @brief The one-past-the-last position in the string
  /// @param ptr The base class pointer
  /// @return An iterator to the one-past-the-last position in the string
  static constexpr auto do_end(string_ptr ptr) noexcept -> iterator { return string(ptr)->end(); }

  /// @brief Convert to string view
  /// @param ptr The base class pointer
  /// @return A string_view object referencing the data
  static constexpr auto do_to_string_view(string_ptr ptr) noexcept -> string_view {
    return {string(ptr)->data(), string(ptr)->size()};
  }

  /// @brief Assign from a string_view
  /// @param ptr The source string
  /// @param rhs The string to append
  /// @return A pointer to the underlying object as the base class
  static constexpr auto do_assign(string_ptr ptr, string_view rhs) noexcept -> string_ptr {
    return &string(ptr)->assign(rhs);
  }

  /// @brief Append a string to the current string
  /// @param ptr The source string
  /// @param rhs The string to append
  /// @return A pointer to the underlying object as the base class
  static constexpr auto do_append(string_ptr ptr, string_view rhs) noexcept -> string_ptr {
    return &string(ptr)->append(rhs);
  }
  /// @brief Append the specified number of copies of a character to the string
  /// @param ptr The source string
  /// @param count The number of characters to append
  /// @param chr The character to append
  /// @return A pointer to the underlying object as the base class
  static constexpr auto do_append_count_char(string_ptr ptr, size_type count, detail::character chr) noexcept
      -> string_ptr {
    return &string(ptr)->append(count, chr);
  }

  /// @brief Erase the specified number of characters in the string starting at the specified position.
  /// @param ptr The source string
  /// @param pos The position of the characters to remove. If @c >size() is a noop.
  /// @param count The maximum number of characters to remove.
  /// @return A reference to this object
  static constexpr auto do_erase(string_ptr ptr, size_type pos, size_type count) noexcept -> string_ptr {
    return &string(ptr)->erase(pos, count);
  }
  /// @brief Erase the characters in the specified range.
  /// @param ptr The source string
  /// @param first The start of the range
  /// @param last The end of the range
  /// @return An iterator referring to the character after the removed range, or @c end()
  static constexpr auto do_erase_it(string_ptr ptr, const_iterator first, const_iterator last) noexcept -> iterator {
    return string(ptr)->erase(first, last);
  }
};

/// @brief Instance of a holder of the operations of a specific size
/// @tparam MaxSize The maximum capacity of the string, _not_ including the null terminator
template <std::size_t MaxSize>
extern constexpr operations string_operations{
    &const_string_operations<MaxSize>,
    &operations_impl<MaxSize>::do_element,
    &operations_impl<MaxSize>::do_at,
    &operations_impl<MaxSize>::do_front,
    &operations_impl<MaxSize>::do_back,
    &operations_impl<MaxSize>::do_begin,
    &operations_impl<MaxSize>::do_end,
    &operations_impl<MaxSize>::do_to_string_view,
    &operations_impl<MaxSize>::do_assign,
    &operations_impl<MaxSize>::do_append,
    &operations_impl<MaxSize>::do_append_count_char,
    &operations_impl<MaxSize>::do_erase,
    &operations_impl<MaxSize>::do_erase_it,
};

/// @brief Passkey type for internal access
class ref_passkey {
  /// @brief Prevent construction from @c {}
  explicit ref_passkey() = default;
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a "Passkey idiom permitted by A11-3-1 Permit #1"
  /// @brief friend declaration to allow this class to construct the passkey
  friend class ::arene::base::const_inline_string_reference;
  // parasoft-end-suppress AUTOSAR-A11_3_1-a "Passkey idiom permitted by A11-3-1 Permit #1"
};

}  // namespace inline_string_reference_detail

/// @brief A reference class to an inline_string object, with size-erased type
class inline_string_reference : generic_ordering_from_three_way_compare<inline_string_reference> {
  /// @brief A pointer to the underlying object as the base class
  inline_string_detail::inline_string_base* string_ptr_;
  /// @brief A pointer to the struct holding operation implementations
  inline_string_reference_detail::operations const* op_ptrs_;

 public:
  /// @brief The type of the base class
  using string_base = inline_string_detail::inline_string_base;

  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: does not hide anything."
  /// @brief The type of a size at the base class
  using size_type = string_base::size_type;
  /// @brief The type of an iterator at the base class
  using iterator = string_base::iterator;
  /// @brief The type of a const iterator at the base class
  using const_iterator = string_base::const_iterator;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d "False positive: does not hide anything."

  /// @brief Construct a reference to the supplied inline_string
  /// @tparam MaxSize The maximum capacity of the string, _not_ including the null terminator
  /// @param str Reference to the inline_string
  template <std::size_t MaxSize>
  constexpr explicit inline_string_reference(inline_string<MaxSize>& str) noexcept
      : generic_ordering_from_three_way_compare<inline_string_reference>(),
        string_ptr_(&str),
        op_ptrs_(&inline_string_reference_detail::string_operations<MaxSize>) {}

  /// @brief Return a pointer to the NUL-terminated string held in @c *this
  /// @return a pointer to the NUL-terminated string held in @c *this
  ARENE_NODISCARD constexpr auto c_str() const noexcept -> detail::raw_c_string {
    return op_ptrs_->const_ops->data(string_ptr_);
  }
  /// @brief Return a pointer to the NUL-terminated string held in @c *this
  /// @return A pointer to the NUL-terminated string held in @c *this
  ARENE_NODISCARD constexpr auto data() const noexcept -> detail::raw_c_string {
    return op_ptrs_->const_ops->data(string_ptr_);
  }

  /// @brief Get the length of the string
  /// @return The number of characters in the string, non-inclusive of the null-terminator
  constexpr auto size() const noexcept -> size_type { return op_ptrs_->const_ops->size(string_ptr_); }

  /// @brief The maximum length of the string
  /// @return size_type @c MaxSize
  constexpr auto capacity() const noexcept -> size_type { return op_ptrs_->const_ops->capacity(string_ptr_); }

  /// @brief Check if the string is empty
  /// @return @c true if the string is empty, @c false otherwise
  constexpr auto empty() const noexcept -> bool { return op_ptrs_->const_ops->empty(string_ptr_); }

  /// @brief Retrieve the @c index -th character in the string
  /// @param index The index of the character to retrieve
  /// @return A reference to that character
  ARENE_NODISCARD constexpr auto operator[](size_type const index) const noexcept -> detail::character& {
    return op_ptrs_->element(string_ptr_, index);
  }
  /// @brief Retrieve the @c index -th character in the string
  /// @tparam AreExceptionsEnabled Used to disable this overload if exceptions are not enabled.
  /// @param index The index of the character to retrieve
  /// @return A reference to that character
  template <
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<std::enable_if_t<AreExceptionsEnabled>> = nullptr>
  constexpr auto at(size_type const index) const -> detail::character& {
    return op_ptrs_->at(string_ptr_, index);
  }

  /// @brief Get the first character in the string
  /// @return A reference to the first character
  constexpr auto front() const noexcept -> detail::character& { return op_ptrs_->front(string_ptr_); }
  /// @brief Get the last character in the string
  /// @return A reference to the last character
  constexpr auto back() const noexcept -> detail::character& { return op_ptrs_->back(string_ptr_); }

  /// @brief Return an iterator to the first position in the string
  /// @return An iterator to the first element if not empty, @c end() otherwise
  constexpr auto begin() const noexcept -> iterator { return op_ptrs_->begin(string_ptr_); }
  /// @brief The one-past-the-last position in the string
  /// @return An iterator to the one-past-the-last position in the string
  constexpr auto end() const noexcept -> iterator { return op_ptrs_->end(string_ptr_); }

  /// @brief Convert to string view
  /// @return A string_view object referencing the data
  constexpr auto to_string_view() const noexcept -> string_view { return op_ptrs_->to_string_view(string_ptr_); }

  /// @brief Assign from a string_view
  /// @param rhs The string to append
  /// @return A reference to this object
  constexpr auto assign(string_view const rhs) const noexcept -> inline_string_reference const& {
    std::ignore = op_ptrs_->assign(string_ptr_, rhs);
    return *this;
  }

  /// @brief Append a string to the current string
  /// @param rhs The string to append
  /// @return A reference to this object
  constexpr auto append(string_view const rhs) const noexcept -> inline_string_reference const& {
    std::ignore = op_ptrs_->append(string_ptr_, rhs);
    return *this;
  }
  /// @brief Append the specified number of copies of a character to the string
  /// @param count The number of characters to append
  /// @param chr The character to append
  /// @return A reference to this object
  constexpr auto append(size_type const count, detail::character const chr) const noexcept
      -> inline_string_reference const& {
    std::ignore = op_ptrs_->append_count_char(string_ptr_, count, chr);
    return *this;
  }

  /// @brief Append a string to the current string
  /// @param rhs The string to append
  /// @return A reference to this object
  constexpr auto operator+=(string_view const rhs) const noexcept -> inline_string_reference const& {
    return this->append(rhs);
  }
  /// @brief Append a character to the current string
  /// @param rhs The character to append
  /// @return A reference to this object
  constexpr auto operator+=(detail::character const rhs) const noexcept -> inline_string_reference const& {
    return this->append(string_view{&rhs, 1U});
  }

  /// @brief Append a string to the current string
  /// @param rhs The string to append
  /// @return A reference to this object
  constexpr auto push_back(string_view const rhs) const noexcept -> inline_string_reference const& {
    return this->operator+=(rhs);
  }
  /// @brief Append a character to the current string
  /// @param rhs The character to append
  /// @return A reference to this object
  constexpr auto push_back(detail::character const rhs) const noexcept -> inline_string_reference const& {
    return this->operator+=(rhs);
  }

  /// @brief Erase the specified number of characters in the string starting at the specified position.
  /// @param pos The position of the characters to remove. If @c pos>size() this is a noop.
  /// @param count The maximum number of characters to remove.
  /// @return A reference to this object
  constexpr auto erase(size_type const pos, size_type const count) const noexcept -> inline_string_reference const& {
    std::ignore = op_ptrs_->erase(string_ptr_, pos, count);
    return *this;
  }
  /// @brief Erase the characters in the specified range.
  /// @param first The start of the range
  /// @param last The end of the range
  /// @return An iterator referring to the character after the removed range, or @c end()
  constexpr auto erase(const_iterator const first, const_iterator const last) const noexcept -> iterator {
    return op_ptrs_->erase_it(string_ptr_, first, last);
  }

  /// @brief Compare two strings for lexicographical ordering, forwarded to string_view's
  /// @param lhs The first string representation
  /// @param rhs The second string representation
  /// @return strong_ordering equivalent to string_view's 3 way compare result
  ARENE_NODISCARD static constexpr auto
  three_way_compare(inline_string_reference const lhs, inline_string_reference const rhs) noexcept -> strong_ordering {
    return string_view::three_way_compare(lhs.to_string_view(), rhs.to_string_view());
  }
  /// @brief Compare two strings for lexicographical ordering, forwarded to string_view's
  /// @param lhs The first string representation
  /// @param rhs The second string representation
  /// @return strong_ordering equivalent to string_view's 3 way compare result
  ARENE_NODISCARD static constexpr auto
  three_way_compare(inline_string_reference const lhs, string_view const rhs) noexcept -> strong_ordering {
    return string_view::three_way_compare(lhs.to_string_view(), rhs);
  }

  /// @brief Get a pointer to the underlying string object pointer
  /// @return A pointer to the underlying string object pointer
  constexpr auto get_string_ptr(inline_string_reference_detail::ref_passkey) const noexcept
      -> inline_string_detail::inline_string_base* {
    return string_ptr_;
  }

  /// @brief Get a pointer to the struct holding const operation implementations
  /// @return A pointer to the struct holding const operation implementations
  constexpr auto const_operations_ptr(inline_string_reference_detail::ref_passkey) const noexcept
      -> inline_string_reference_detail::const_operations const* {
    return op_ptrs_->const_ops;
  }
};

// parasoft-begin-suppress AUTOSAR-A12_1_5-a "False positive: there is no common constructor to delegate to."
/// @brief A const reference class to an inline_string object, with size-erased type
class const_inline_string_reference : generic_ordering_from_three_way_compare<const_inline_string_reference> {
  /// @brief A pointer to the underlying string object
  inline_string_detail::inline_string_base const* string_ptr_;
  /// @brief A pointer to the struct holding function pointers to the const operation implementations
  inline_string_reference_detail::const_operations const* const_op_ptrs_;

 public:
  /// @brief The type of the base class
  using string_base = inline_string_detail::inline_string_base;

  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: does not hide anything."
  /// @brief The type of a size at the base class
  using size_type = string_base::size_type;
  /// @brief The type of a const iterator at the base class
  using const_iterator = string_base::const_iterator;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d "False positive: does not hide anything."

  /// @brief Construct a const reference to the supplied inline_string
  /// @tparam MaxSize The maximum capacity of the string, _not_ including the null terminator
  /// @param str Const reference to the inline_string
  template <std::size_t MaxSize>
  constexpr explicit const_inline_string_reference(inline_string<MaxSize> const& str) noexcept
      : generic_ordering_from_three_way_compare<const_inline_string_reference>(),
        string_ptr_(&str),
        const_op_ptrs_(&inline_string_reference_detail::const_string_operations<MaxSize>) {}

  /// @brief Construct a const reference out of a non-const one
  /// @param ref The non-const reference class
  constexpr explicit const_inline_string_reference(inline_string_reference const& ref) noexcept
      : generic_ordering_from_three_way_compare<const_inline_string_reference>(),
        string_ptr_(ref.get_string_ptr(inline_string_reference_detail::ref_passkey{})),
        const_op_ptrs_(ref.const_operations_ptr(inline_string_reference_detail::ref_passkey{})) {}

  /// @brief Return a pointer to the NUL-terminated string held in @c *this
  /// @return A pointer to the NUL-terminated string held in @c *this
  ARENE_NODISCARD constexpr auto c_str() const noexcept -> detail::raw_c_string {
    return const_op_ptrs_->data(string_ptr_);
  }
  /// @brief Return a pointer to the NUL-terminated string held in @c *this
  /// @return A pointer to the NUL-terminated string held in @c *this
  ARENE_NODISCARD constexpr auto data() const noexcept -> detail::raw_c_string {
    return const_op_ptrs_->data(string_ptr_);
  }

  /// @brief Get the length of the string
  /// @return The number of characters in the string, non-inclusive of the null-terminator
  constexpr auto size() const noexcept -> size_type { return const_op_ptrs_->size(string_ptr_); }

  /// @brief The maximum length of the string
  /// @return size_type @c MaxSize
  constexpr auto capacity() const noexcept -> size_type { return const_op_ptrs_->capacity(string_ptr_); }

  /// @brief Check if the string is empty
  /// @return @c true if the string is empty, @c false otherwise
  constexpr auto empty() const noexcept -> bool { return const_op_ptrs_->empty(string_ptr_); }

  /// @brief Retrieve the @c index -th character in the string
  /// @param index The index of the character to retrieve
  /// @return A reference to that character
  ARENE_NODISCARD constexpr auto operator[](size_type const index) const noexcept -> detail::character const& {
    return const_op_ptrs_->element(string_ptr_, index);
  }
  /// @brief Retrieve the @c index -th character in the string
  /// @tparam AreExceptionsEnabled Used to disable this overload if exceptions are not enabled.
  /// @param index The index of the character to retrieve
  /// @return A reference to that character
  template <
      bool AreExceptionsEnabled = detail::are_exceptions_enabled::value,
      constraints<std::enable_if_t<AreExceptionsEnabled>> = nullptr>
  constexpr auto at(size_type const index) const -> detail::character const& {
    return const_op_ptrs_->at(string_ptr_, index);
  }

  /// @brief Get the first character in the string
  /// @return A reference to the first character
  constexpr auto front() const noexcept -> detail::character const& { return const_op_ptrs_->front(string_ptr_); }
  /// @brief Get the last character in the string
  /// @return A reference to the last character
  constexpr auto back() const noexcept -> detail::character const& { return const_op_ptrs_->back(string_ptr_); }

  /// @brief Return an iterator to the first position in the string
  /// @return A const iterator to the first element if not empty, @c end() otherwise
  constexpr auto begin() const noexcept -> const_iterator { return const_op_ptrs_->begin(string_ptr_); }
  /// @brief The one-past-the-last position in the string
  /// @return A const iterator to one-past-the-last position in the string
  constexpr auto end() const noexcept -> const_iterator { return const_op_ptrs_->end(string_ptr_); }

  /// @brief Convert to string view
  /// @return string_view A string_view object referencing the data
  constexpr auto to_string_view() const noexcept -> string_view { return const_op_ptrs_->to_string_view(string_ptr_); }

  /// @brief Compare two strings for lexicographical ordering, forwarded to string_view's
  /// @param lhs The first string representation
  /// @param rhs The second string representation
  /// @return strong_ordering equivalent to string_view's 3 way compare result
  ARENE_NODISCARD static constexpr auto three_way_compare(
      const_inline_string_reference const lhs,
      const_inline_string_reference const rhs
  ) noexcept -> strong_ordering {
    return string_view::three_way_compare(lhs.to_string_view(), rhs.to_string_view());
  }
  /// @brief Compare two strings for lexicographical ordering, forwarded to string_view's
  /// @param lhs The first string representation
  /// @param rhs The second string representation
  /// @return strong_ordering equivalent to string_view's 3 way compare result
  ARENE_NODISCARD static constexpr auto three_way_compare(
      const_inline_string_reference const lhs,
      inline_string_reference const rhs
  ) noexcept -> strong_ordering {
    return string_view::three_way_compare(lhs.to_string_view(), rhs.to_string_view());
  }
  /// @brief Compare two strings for lexicographical ordering, forwarded to string_view's
  /// @param lhs The first string representation
  /// @param rhs The second string representation
  /// @return strong_ordering equivalent to string_view's 3 way compare result
  ARENE_NODISCARD static constexpr auto
  three_way_compare(const_inline_string_reference const lhs, string_view const rhs) noexcept -> strong_ordering {
    return string_view::three_way_compare(lhs.to_string_view(), rhs);
  }
};
// parasoft-end-suppress AUTOSAR-A12_1_5-a "False positive: there is no common constructor to delegate to."

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_INLINE_STRING_REFERENCE_HPP_
