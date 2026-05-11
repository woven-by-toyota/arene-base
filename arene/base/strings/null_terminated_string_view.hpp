// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_NULL_TERMINATED_STRING_VIEW_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_NULL_TERMINATED_STRING_VIEW_HPP_

// IWYU pragma: private
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compare/operators.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/detail/raw_c_string.hpp"
#include "arene/base/detail/wrapped_iterator.hpp"  // IWYU pragma: keep
#include "arene/base/iterator/advance.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/string.hpp"
#include "arene/base/strings/detail/lexicographic_string_compare.hpp"
#include "arene/base/strings/detail/string_length.hpp"
#include "arene/base/strings/detail/string_view_ostream_mixin.hpp"  // IWYU pragma: keep
#include "arene/base/strings/string_view.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {

// Exception: AUTOSAR Rule M5-0-15 (required, implementation, automated)
//   Array indexing shall be the only form of pointer arithmetic.
//
// The purpose of this class is to abstract out the use of pointer arithmetic necessary to process null-terminated
// strings and string literals. Pointer arithmetic is only used to adjust the pointer within the array containing the
// string up until the null-terminator, as covered by M5-0-16 and M5-0-17, unless iterators are incremented unchecked.

// parasoft-begin-suppress AUTOSAR-A12_1_5-a-2 "False positive: delegating constructors are used"
/// @brief A read-only string-view that provides a view onto a null-terminated string.
///
/// This string type can be used to pass around null-terminated strings more safely and explicitly compared to passing
/// a 'const char*', making it easier to query the length of the string, compare strings, compute sub-strings, etc.
///
///  This class just holds a reference to a string stored elsewhere. The null_terminated_string_view does not own the
///  elements of the string.
///
///  You should prefer to use string_view over this class unless the extra storage of the size_t required by
///  string_view is prohibitively expensive compared to the cost of repeatedly calculating the string length
///  every time it is used.
class null_terminated_string_view
    : generic_ordering_from_three_way_compare<null_terminated_string_view>
    , string_view_ostream_mixin<null_terminated_string_view> {
  /// @brief simple tag passkey type to protect iterator construction.
  class passkey {
   public:
    /// @brief explicit private ctor to prevent construction from {}
    constexpr explicit passkey() = default;
  };

  /// @brief Internal type alias for the base class
  using compare_base = generic_ordering_from_three_way_compare<null_terminated_string_view>;

  // parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "Returning an int for compatibility with std::string_view"
  /// @brief The type of comparisons that yield an integer rather than a @c strong_ordering
  using integral_comparison_result = int;
  // parasoft-end-suppress AUTOSAR-A3_9_1-b-2

 public:
  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief construct an empty string-view.
  constexpr null_terminated_string_view() noexcept
      : null_terminated_string_view("") {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  /// @brief default copy constructor
  constexpr null_terminated_string_view(null_terminated_string_view const&) = default;

  /// @brief default copy assignment
  constexpr auto operator=(null_terminated_string_view const&) -> null_terminated_string_view& = default;

  /// @brief default move constructor
  constexpr null_terminated_string_view(null_terminated_string_view&&) = default;

  /// @brief default move assignment
  constexpr auto operator=(null_terminated_string_view&&) -> null_terminated_string_view& = default;

  /// @brief default destructor
  ~null_terminated_string_view() = default;

  /// @brief constructing from a nullptr literal is always an error.
  explicit null_terminated_string_view(std::nullptr_t) = delete;

  // parasoft-begin-suppress AUTOSAR-A7_1_3-a-2 "False Positive: const is on rhs"
  /// @brief construct a view on an existing null-terminated string.
  ///
  /// @param string A pointer to the first character of the null-terminated
  /// string. Caller must ensure that the string is properly null-terminated.
  /// Passing a nullptr has undefined behaviour.
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
  constexpr explicit null_terminated_string_view(detail::raw_c_string const string) noexcept;
  // parasoft-end-suppress AUTOSAR-A7_1_3-a-2

  /// @brief Implicit construction from a std::string (which is required to be
  /// null-terminated).
  ///
  /// The resulting string view is invalidated by any operation that modifies
  /// the string.
  ///
  /// @param string The source string
  ///
  /// @note This constructor is a template rather than a non-template taking a
  /// <c>const std::string&</c> to avoid the possibility of unknowingly
  /// constructing a temporary @c std::string implicitly by passing something
  /// that is convertible to @c std::string.
  /// e.g. a @c std::initializer_list<char>
  template <typename String, constraints<std::enable_if_t<std::is_same<String, std::string>::value>> = nullptr>
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  null_terminated_string_view(String const& string) noexcept
      : compare_base{},
        string_(string.c_str()) {}

  /// @brief This type represents the sentinel of the range of characters.
  ///
  /// The return-type of @c end() method.
  class sentinel {};

  /// @brief The value type of this view
  using value_type = detail::character;
  /// @brief The pointer type of this view
  using pointer = value_type*;
  /// @brief The const pointer type of this view
  using const_pointer = value_type const*;
  /// @brief The type of a reference to an element in the view
  using reference = value_type&;
  /// @brief The type of a reference to an element in the view
  using const_reference = value_type const&;
  /// @brief The type of const iterator used by this view.
  using const_iterator = ::arene::base::detail::wrapped_iterator<const_pointer, passkey>;
  /// @brief The type of iterator used by this view.
  using iterator = const_iterator;
  /// @brief The type used for sizes in this view
  using size_type = std::size_t;
  /// @brief The type used for differences between iterators in this view
  using difference_type = typename iterator::difference_type;

  // parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Comparison of iterator with sentinel"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: comparison operators are permitted"
  /// @brief Equality comparison for a null terminated string view's iterator and the end-of-string sentinel.
  ///
  /// @param iter The iterator to compare against the sentinel.
  /// @return true if the iterator points to a @c '\0' character.
  /// @return false Otherwise.
  ARENE_NODISCARD friend constexpr auto operator==(iterator const iter, sentinel) noexcept -> bool {
    return *iter.base() == '\0';
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  // parasoft-end-suppress AUTOSAR-A13_5_5-b-2

  // parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Comparison of iterator with sentinel"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: comparison operators are permitted"
  /// @brief Equality comparison for a null terminated string view's iterator and the end-of-string sentinel.
  ///
  /// @param iter The iterator to compare against the sentinel.
  /// @return true if the iterator points to a @c '\0' character.
  /// @return false Otherwise.
  ARENE_NODISCARD friend constexpr auto operator!=(iterator const iter, sentinel) noexcept -> bool {
    return !(iter == null_terminated_string_view::sentinel{});
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  // parasoft-end-suppress AUTOSAR-A13_5_5-b-2

  /// @brief Get an iterator to the first element of the string.
  /// @return const_iterator An iterator to the first character in the string.
  ARENE_NODISCARD constexpr auto begin() const noexcept -> const_iterator { return const_iterator{passkey{}, string_}; }

  /// @brief Get the sentinel (end-iterator) of this string.
  /// @return sentinel The sentinel comparable to an iterator to mark the end of the string.
  // If we make this a static method then we end up getting warnings at the usage site about calling a static member
  // function using .end() syntax. So instead we rather keep this as a non-static member function.
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  ARENE_NODISCARD static constexpr auto end() noexcept -> sentinel { return {}; }

  /// @brief Query if this string is the empty string.
  /// @return true if the string is empty, false otherwise
  ARENE_NODISCARD constexpr auto empty() const noexcept -> bool { return begin() == end(); }

  /// @brief Query the length of the string.
  ///
  /// @return The number of 'char' elements in the null-terminate string, not including the null-terminator.
  ///
  /// @note This operation needs to traverse the string to determine the length and so may be expensive to call
  /// repeatedly. If you need to call @c length() repeatedly then either cache the value or convert this object to a
  /// @c string_view first and use that instead.
  ARENE_NODISCARD constexpr auto length() const noexcept -> size_type { return detail::string_length(string_); }

  /// @brief Query the length of the string.
  ///
  /// @return The number of 'char' elements in the null-terminate string, not including the null-terminator.
  ///
  /// @note This operation needs to traverse the string to determine the length and so may be expensive to call
  /// repeatedly. If you need to call @c size() repeatedly then either cache the value or convert this object to a @c
  /// string_view first and use that instead.
  ARENE_NODISCARD constexpr auto size() const noexcept -> size_type { return length(); }

  /// @brief Obtain a C-style null-terminated string pointer.
  /// @return const_pointer A pointer to the first character in the string.
  ARENE_NODISCARD constexpr auto c_str() const noexcept -> const_pointer { return string_; }

  /// @brief Obtain a C-style null-terminated string pointer.
  /// @return const_pointer A pointer to the first character in the string.
  ARENE_NODISCARD constexpr auto data() const noexcept -> const_pointer { return string_; }

  // parasoft-begin-suppress AUTOSAR-A13_5_2-a-2 "Implicit conversion to string_view is part of the API"
  /// @brief Implicit conversion to string_view.
  ///
  /// @return A string_view that spans the characters of *this, minus the null-terminator.
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  constexpr operator string_view() const noexcept { return {data(), length()}; }
  // parasoft-end-suppress AUTOSAR-A13_5_2-a-2

  /// @brief Compute the sub-string starting at the specified offset within the string.
  ///
  /// @param offset The index of the first character that the returned substring should start at.
  ///
  /// @return null_terminated_string_view A view which starts at the character at index @c offset and continues to
  /// the end of the string. If @c offset is larger than @c length() then returns the empty string view.
  ///
  /// @note This operation has complexity of O(min(offset, length())).
  // parasoft-begin-suppress AUTOSAR-A15_4_5-a "False positive: There is no function that throws an excpetion of 'Any'
  // type"
  ARENE_NODISCARD constexpr auto substr(std::size_t offset) const noexcept -> null_terminated_string_view;
  // parasoft-end-suppress AUTOSAR-A15_4_5-a

  /// @brief Compute the sub-string starting at the specified offset and with the specified maximum length.
  ///
  /// @param offset The index of the first charcter of the substring. If this value is greater than @c length() then
  /// returns the empty sub-string.
  /// @param max_length The maximum length of the returned substring. The returned substring may be shorter than this
  /// length if there are fewer than @c max_length elements after @c offset.
  ///
  /// @return string_view A substring referencing the elements of *this starting at index @c offset with a length at
  ///         most @c max_length.
  ///
  /// @note This operation has complexity of O(min(offset+max_length, length()))
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  ARENE_NODISCARD constexpr auto substr(std::size_t offset, std::size_t max_length) const noexcept -> string_view;

  /// @brief Compare this null-terminated string to another null-terminated string.
  ///
  /// @param other The other string to compare *this to.
  ///
  /// @return integral_comparison_result A negative value if @c *this lexicographically precedes @c other, a positive
  /// value if @c other lexicographically precedes @c *this, or zero if the two strings are equal.
  // parasoft-begin-suppress AUTOSAR-A7_1_3-a-2 "False Positive: const is on rhs"
  constexpr auto compare(null_terminated_string_view const other) const noexcept -> integral_comparison_result {
    return from_strong_ordering(three_way_compare(*this, other));
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a-2

  /// @brief Compare this null-terminated string to a string_view.
  ///
  /// @param other The other string to compare *this to.
  ///
  /// @return A negative value if @c *this lexicographically precedes @c other, a positive value if @c other
  /// lexicographically precedes @c *this, or zero if the two strings are equal.
  constexpr auto compare(string_view const& other) const noexcept -> integral_comparison_result {
    return from_strong_ordering(three_way_compare(*this, other));
  }

  /// @brief Compare this null-terminated string to a std::string.
  ///
  /// @param other The other string to compare *this to.
  ///
  /// @return A negative value if @c *this lexicographically precedes @c other, a positive value if @c other
  /// lexicographically precedes @c *this, or zero if the two strings are equal.
  ///
  /// @note This constructor is a template rather than a non-template taking a <c>const std::string&</c> to handle when
  /// @c std::string is only declared, not defined.
  template <typename String, constraints<std::enable_if_t<std::is_same<String, std::string>::value>> = nullptr>
  auto compare(String const& other) const noexcept -> integral_comparison_result {
    return compare(string_view(other));
  }

  /// @brief Compare two null-terminated strings
  ///
  /// @param lhs The first string to compare
  /// @param rhs The second string to compare
  ///
  /// @return strong_ordering::less if @c lhs lexicographically precedes @c rhs
  /// @return strong_ordering::greater if @c rhs lexicographically precedes @c lhs
  /// @return strong_ordering::equal if the two strings are equal.
  static constexpr auto three_way_compare(
      null_terminated_string_view const lhs,
      null_terminated_string_view const rhs
  ) noexcept -> strong_ordering {
    return detail::lexicographic_string_compare(lhs.c_str(), rhs.c_str());
  }

  /// @brief Compare two null-terminated strings
  ///
  /// @param lhs The first string to compare
  /// @param rhs The second string to compare
  ///
  /// @return strong_ordering::less if @c lhs lexicographically precedes @c rhs
  /// @return strong_ordering::greater if @c rhs lexicographically precedes @c lhs
  /// @return strong_ordering::equal if the two strings are equal.
  static constexpr auto
  three_way_compare(null_terminated_string_view const lhs, detail::raw_c_string const rhs) noexcept -> strong_ordering {
    return three_way_compare(lhs, null_terminated_string_view(rhs));
  }

  /// @brief Compare a null-terminated string to a @c std::string
  ///
  /// @param lhs The first string to compare
  /// @param rhs The second string to compare
  ///
  /// @return strong_ordering::less if @c lhs lexicographically precedes @c rhs
  /// @return strong_ordering::greater if @c rhs lexicographically precedes @c lhs
  /// @return strong_ordering::equal if the two strings are equal.
  ///
  /// @note This constructor is a template rather than a non-template taking a <c>const std::string&</c> to handle when
  /// @c std::string is only declared, not defined.
  template <typename String, constraints<std::enable_if_t<std::is_same<String, std::string>::value>> = nullptr>
  static auto three_way_compare(null_terminated_string_view const lhs, String const& rhs) noexcept -> strong_ordering {
    return three_way_compare(lhs, string_view{rhs});
  }

  /// @brief Compare a null-terminated string to a @c string_view
  ///
  /// @param lhs The first string to compare
  /// @param rhs The second string to compare
  ///
  /// @return strong_ordering::less if @c lhs lexicographically precedes @c rhs
  /// @return strong_ordering::greater if @c rhs lexicographically precedes @c lhs
  /// @return strong_ordering::equal if the two strings are equal.
  static constexpr auto three_way_compare(null_terminated_string_view const lhs, string_view const rhs) noexcept
      -> strong_ordering {
    return opposite_ordering(detail::lexicographic_string_compare({rhs.data(), rhs.size()}, lhs.c_str()));
  }

 private:
  /// @brief The pointer to the NUL-terminated string
  detail::raw_c_string string_;
};
// parasoft-end-suppress AUTOSAR-A12_1_5-a-2 "False positive: delegating constructors are used"

/// @brief Compute the sub-string starting at the specified offset within the string.
///
/// @param offset The index of the first character that the returned substring should start at.
///
/// @return null_terminated_string_view A view which starts at the character at index @c offset and continues to
/// the end of the string. If @c offset is larger than @c length() then returns the empty string view.
///
/// @note This operation has complexity of O(min(offset, length())).
// parasoft-begin-suppress AUTOSAR-A15_4_5-a "False positive: There is no function that throws an excpetion of 'Any'
// type"
ARENE_NODISCARD inline constexpr auto null_terminated_string_view::substr(std::size_t offset) const noexcept
    -> null_terminated_string_view {
  detail::raw_c_string start{string_};
  while ((offset != 0U) && (*start != '\0')) {
    --offset;
    arene::base::advance(start, 1);
  }

  return null_terminated_string_view{start};
}
// parasoft-end-suppress AUTOSAR-A15_4_5-a

// parasoft-begin-suppress AUTOSAR-A7_1_3-a-2 "False Positive: const is on rhs"
/// @brief construct a view on an existing null-terminated string.
///
/// @param string A pointer to the first character of the null-terminated
/// string. Caller must ensure that the string is properly null-terminated.
/// Passing a nullptr has undefined behaviour.
inline constexpr null_terminated_string_view::null_terminated_string_view(detail::raw_c_string const string) noexcept
    : compare_base{},
      string_(string) {
  ARENE_PRECONDITION(string != nullptr);
}
// parasoft-end-suppress AUTOSAR-A7_1_3-a-2

/// @brief Compute the sub-string starting at the specified offset and with the specified maximum length.
///
/// @param offset The index of the first charcter of the substring. If this value is greater than @c length() then
/// returns the empty sub-string.
/// @param max_length The maximum length of the returned substring. The returned substring may be shorter than this
/// length if there are fewer than @c max_length elements after @c offset.
///
/// @return string_view A substring referencing the elements of *this starting at index @c offset with a length at
///         most @c max_length.
///
/// @note This operation has complexity of O(min(offset+max_length, length()))
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
ARENE_NODISCARD inline constexpr auto null_terminated_string_view::substr(std::size_t offset, std::size_t max_length)
    const noexcept -> string_view {
  auto first = begin();
  while ((offset != 0U) && (first != end())) {
    ++first;
    --offset;
  }

  auto last = first;
  while ((max_length != 0U) && (last != end())) {
    ++last;
    --max_length;
  }

  return {first.base(), static_cast<std::size_t>(last - first)};
}

namespace literals {

// parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
/// @brief Construct a null-terminated string view from a string literal.
///
/// For example:
/// @code
/// using namespace literals;
/// auto sv = "a null terminated string"_ntsv;
/// @endcode
///
/// @param str Pointer to the string literal.
/// @return The null_terminated_string_view referencing the string literal.
constexpr auto operator""_ntsv(detail::raw_c_string const str, std::size_t /*size*/) noexcept
    -> null_terminated_string_view {
  return null_terminated_string_view{str};
}
// parasoft-end-suppress AUTOSAR-A7_1_3-a

}  // namespace literals

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M2_10_1-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_NULL_TERMINATED_STRING_VIEW_HPP_
