// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_STRING_VIEW_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_STRING_VIEW_HPP_

// IWYU pragma: private
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/copy.hpp"
#include "arene/base/algorithm/find.hpp"
#include "arene/base/compare/operators.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/detail/raw_c_string.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/iterator/reverse_iterator.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/min_value_overload.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/stdlib_choice/string.hpp"
#include "arene/base/strings/detail/lexicographic_string_compare.hpp"
#include "arene/base/strings/detail/string_length.hpp"
#include "arene/base/strings/detail/string_view_ostream_mixin.hpp"
#include "arene/base/utility/swap.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A3_1_5-a "False positive: all functions are constexpr so are intended to be inlined"

namespace arene {
namespace base {

// NOLINTBEGIN(google-explicit-constructor, hicpp-avoid-c-arrays)

// parasoft-begin-suppress AUTOSAR-A13_5_1-a-2 "False positive: no non-const overload"
/// @brief Backport of the C++17 std::string_view class.
///
/// http://en.cppreference.com/w/cpp/string/basic_string_view
///
/// This implementation is minimalist but API compatible with the std:: one.
class string_view
    : private generic_ordering_from_three_way_compare<string_view>
    , private string_view_ostream_mixin<string_view> {
  /// @brief The character type
  using character = detail::character;
  /// @brief The raw NUL-terminated C string type
  using raw_c_string = detail::raw_c_string;
  /// @brief Internal. Type alias to the underlying span
  using const_char_span = span<character const>;
  // parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "Returning an int for compatibility with std::string_view"
  /// @brief The type of comparisons that yield an integer rather than a @c strong_ordering
  using integral_comparison_result = int;
  // parasoft-end-suppress AUTOSAR-A3_9_1-b-2

 public:
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property"
  /// @brief Special marker value for end of string
  static constexpr std::size_t npos{std::numeric_limits<std::size_t>::max()};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False Positive: identifiers at class/namespace scope are exempt"

  /// @brief Alias to the type associated to a single contained character
  using element_type = const_char_span::element_type;

  /// @brief Same that element_type but without any cv-qualifier
  using value_type = const_char_span::value_type;

  /// @brief Alias for the type associated to the size of the string
  using size_type = std::size_t;
  /// @brief Alias associated to the type used for indexing
  using difference_type = std::ptrdiff_t;
  /// @brief Alias to the pointer type associated to the underlying data
  using pointer = const_char_span::pointer;
  /// @brief Const qualified version of @c pointer
  using const_pointer = const_char_span::const_pointer;
  /// @brief Alias to the reference type associated to the underlying data
  using reference = const_char_span::reference;
  /// @brief Const qualified version of @c reference
  using const_reference = const_char_span::const_reference;
  /// @brief Alias to the iterator associated with the string_view class
  using iterator = const_char_span::iterator;
  /// @brief Const qualified version of @c iterator
  using const_iterator = const_char_span::const_iterator;
  /// @brief Alias to the reverse iterator
  using reverse_iterator = const_char_span::reverse_iterator;
  /// @brief Alias to the const-qualified reverse iterator
  using const_reverse_iterator = const_char_span::const_reverse_iterator;

  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  /// @brief Default constructor, creates an empty string_view.
  ///
  /// @post <c>data() == nullptr</c>
  /// @post <c>size() == 0</c>
  constexpr string_view() noexcept = default;

  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  /// @brief Constructor for string_view from const char* string and explicit size
  ///
  /// @param str Pointer to the first character of the string. May contain null characters.
  /// @param str_length The number of @c char elements in the string.
  /// @post <c>data() == str</c>
  /// @post <c>size() == str_length</c>
  /// @pre @c str must point to an array of at least @c str_length characters, else behavior is undefined.
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays,hicpp-explicit-conversions,google-explicit-constructor)
  constexpr string_view(raw_c_string const str, std::size_t const str_length) noexcept
      : generic_ordering_from_three_way_compare<string_view>(),
        str_span_(str, str_length) {}
  // parasoft-end-suppress AUTOSAR-A7_1_3-a

  /// @brief Constructor from compile time string literal or c-array
  ///
  /// @param literal string literal used for the string_view construction
  /// @tparam N The number of @c char elements in the string
  template <std::size_t N>
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays,hicpp-explicit-conversions,google-explicit-constructor)
  inline constexpr string_view(character const (&literal)[N]) noexcept
      : string_view(literal, N - 1) {}

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Construct a string_view from a null terminated string, non-inclusive of the null terminator
  /// @param null_terminated_string null terminated string
  /// @pre Behavior is undefined if null_terminated_string is not a pointer to a null terminated sequence of characters.
  /// @post <c>data() == null_terminated_string</c>
  /// @post @c size() will be the count of characters from the first element in the string to the first instance of a
  ///       null terminator.
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays,hicpp-explicit-conversions,google-explicit-constructor)
  constexpr string_view(raw_c_string const null_terminated_string) noexcept
      : string_view(null_terminated_string, detail::string_length(null_terminated_string)) {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Construct a string_view from a @c std::string.
  ///
  /// @param str The @c std::string to create a view to.
  /// @post <c>data() == str.data()</c>
  /// @post <c>size() == str.size()</c>
  ///
  /// @note This constructor is a template rather than a non-template taking a <c>const std::string&</c> to avoid the
  ///       possibility of unknowingly constructing a temporary @c std::string implicitly by passing something that is
  ///       convertible to @c std::string such as a @c std::initializer_list<char>
  template <typename String, constraints<std::enable_if_t<std::is_same<String, std::string>::value>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions,google-explicit-constructor)
  string_view(String const& str) noexcept
      : string_view(str.data(), str.size()) {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  /// @brief Default copy constructor
  constexpr string_view(string_view const& other) noexcept = default;

  /// @brief Default move constructor
  constexpr string_view(string_view&& other) noexcept = default;

  /// @brief Default destructor
  ~string_view() = default;

  /// @brief Begin const iterator.
  ///
  /// @return An iterator that references the first character of the string.
  constexpr auto begin() const noexcept -> const_iterator { return str_span_.begin(); }

  /// @brief End const iterator.
  ///
  /// @return An iterator that is one-past the last character of the string_view.
  constexpr auto end() const noexcept -> const_iterator { return str_span_.end(); }

  /// @brief Begin const iterator.
  ///
  /// @return An iterator that references the first character of the string_view.
  constexpr auto cbegin() const noexcept -> const_iterator { return begin(); }

  /// @brief End const iterator.
  ///
  /// @return An iterator that is one-past the last character of the string_view.
  constexpr auto cend() const noexcept -> const_iterator { return end(); }

  /// @brief Obtain a reverse iterator referring to the end of the string
  ///
  /// @return An iterator that references the last character of the string_view.
  constexpr auto rbegin() const noexcept -> reverse_iterator { return str_span_.rbegin(); }

  /// @brief Obtain a reverse iterator referring to one-before the front of the string_view
  ///
  /// @return An iterator that references one-before the first character of the string_view
  constexpr auto rend() const noexcept -> reverse_iterator { return str_span_.rend(); }

  /// @brief Obtain a reverse iterator referring to the end of the string_view
  ///
  /// @return An iterator that references the last character of the string_view.
  constexpr auto crbegin() const noexcept -> const_reverse_iterator { return str_span_.rbegin(); }

  /// @brief Obtain a reverse iterator referring to one-before the front of the string_view
  ///
  /// @return An iterator that references one-before the first character of the string_view
  constexpr auto crend() const noexcept -> const_reverse_iterator { return str_span_.rend(); }

  /// @brief Get the length of the string view
  /// @return The number of 'char' values in this string.
  /// @invariant <c> size() == std::distance(begin(), end()) </c>
  constexpr auto size() const noexcept -> size_type { return str_span_.size(); }

  /// @brief Get the length of the string view
  /// @return The number of 'char' values in this string.
  /// @invariant <c> length() == std::distance(begin(), end()) </c>
  constexpr auto length() const noexcept -> size_type { return str_span_.size(); }

  /// @brief Query if this is the empty string.
  ///
  /// @return true IIF <c>size() == 0</c>.
  /// @return false otherwise
  constexpr auto empty() const noexcept -> bool { return size() == 0U; }

  /// @brief Get the maximum possible length of a @c string_view
  /// @return Return the maximum string size supported by a @c string_view
  static constexpr auto max_size() noexcept -> size_type { return std::numeric_limits<string_view::size_type>::max(); }

  /// @brief lexicographically compares two string_view, behavior similar to std::string
  ///
  /// @param other other string view to compare with
  /// @return @c 0 if the length of @c this and @c other are equal and all characters compare equal.
  /// @return @c -1 if the value of the first character that does not match is lexicographically less in @c this than
  ///          @c other .
  /// @return @c -1 if the length of @c this is less than the length of @c other and all characters in the overlapping
  ///          subset of @c this match those in @c other .
  /// @return @c 1 if the value of the first character that does not match is lexicographically greater in @c this than
  ///          @c other .
  /// @return @c 1 if the length of @c this is greater than the length of @c other and all characters in the
  ///          overlapping subset of lhs match those in @c other .
  // parasoft-begin-suppress AUTOSAR-A7_1_3-a-2 "False Positive: const is on rhs"
  constexpr auto compare(string_view const other) const noexcept -> integral_comparison_result {
    return from_strong_ordering(three_way_compare(*this, other));
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a-2

  /// @brief Compare a substring against another string view
  ///
  /// @param pos1 The offset of the substring to compare
  /// @param length1 The length of the substring to compare
  /// @param other The other string view to compare with
  /// @return Equivalent to calling <c>*this.substr(pos1, length1).compare(other)</c>.
  // parasoft-begin-suppress AUTOSAR-A7_1_3-a-2 "False Positive: const is on rhs"
  constexpr auto compare(size_type const pos1, size_type const length1, string_view const other) const noexcept
      -> integral_comparison_result {
    return substr(pos1, length1).compare(other);
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a-2

  /// @brief Compare a substring against a substring of another string view
  ///
  /// @param pos1 The offset of the substring to compare
  /// @param length1 The length of the substring to compare
  /// @param other The other string view to compare with
  /// @param pos2 The offset of the other substring to compare
  /// @param length2 The length of the other substring to compare
  /// @return Equivalent to calling <c>*this.substr(pos1, length1).compare(other.substr(pos2, length2))</c>.
  // parasoft-begin-suppress AUTOSAR-A7_1_3-a-2 "False Positive: const is on rhs"
  constexpr auto compare(
      size_type const pos1,
      size_type const length1,
      string_view const other,
      size_type const pos2,
      size_type const length2
  ) const noexcept -> integral_comparison_result {
    return substr(pos1, length1).compare(other.substr(pos2, length2));
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a-2

  /// @brief Compare a string_view against a null-terminated string
  ///
  /// @param other The other string view to compare with
  /// @return Equivalent to calling <c>*this.compare(string_view{other})</c>.
  // parasoft-begin-suppress AUTOSAR-A7_1_3-a-2 "False Positive: const is on rhs"
  constexpr auto compare(raw_c_string const other) const noexcept -> integral_comparison_result {
    return from_strong_ordering(detail::lexicographic_string_compare(str_span_, other));
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a-2

  /// @brief Compare a substring against a null terminated string
  ///
  /// @param pos1 The offset of the substring to compare
  /// @param length1 The length of the substring to compare
  /// @param other The other string view to compare with
  /// @return Equivalent to calling <c>*this.substr(pos1, length1).compare(string_view{other})</c>.
  // parasoft-begin-suppress AUTOSAR-A7_1_3-a-2 "False Positive: const is on rhs"
  constexpr auto compare(size_type const pos1, size_type const length1, raw_c_string const other) const noexcept
      -> integral_comparison_result {
    return substr(pos1, length1).compare(other);
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a-2

  /// @brief Compare a substring against a string specified as a pointer and length
  ///
  /// @param pos1 The offset of the substring to compare
  /// @param length1 The length of the substring to compare
  /// @param other Pointer to the other string to compare with
  /// @param length2 The length of the other string to compare
  /// @return Equivalent to calling <c>*this.substr(pos1, length1).compare(string_view{other, length2})</c>.
  /// @pre @c length2 is in the range of @c other otherwise behavior is undefined.
  // parasoft-begin-suppress AUTOSAR-A7_1_3-a-2 "False Positive: const is on rhs"
  constexpr auto compare(
      size_type const pos1,
      size_type const length1,
      raw_c_string const other,
      size_type const length2
  ) const noexcept -> integral_comparison_result {
    return substr(pos1, length1).compare(string_view{other, length2});
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a-2

  /// @brief Create a new string view referencing the same string as @c *this
  /// @return A copy of @c *this
  constexpr auto substr() const noexcept -> string_view { return *this; }

  /// @brief Create a new string view based on a substring of the current one.
  ///
  /// @param pos Position of the first character of the substring.
  /// @param count The requested length of the substring.
  /// @return string_view Will contain content beginning at @c pos and continuing for @c count characters. If
  ///         @c pos>=size() , or @c pos+count>=size() , the returned substring is clamped to the end of the
  ///         @c string_view .
  // parasoft-begin-suppress AUTOSAR-A7_1_3-a-2 "False positive: const is placed on the right hand side"
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  constexpr auto substr(size_type const pos, size_type const count = npos) const noexcept -> string_view {
    // parasoft-end-suppress AUTOSAR-A7_1_3-a-2
    size_type const bounded_pos{std::min(pos, size())};
    size_type const bounded_size{std::min((size() - bounded_pos), count)};
    return {arene::base::next(data(), static_cast<std::ptrdiff_t>(bounded_pos)), bounded_size};
  }

  /// @brief Swap the content of two string_view objects.
  ///
  /// @param other The other string_view to swap with.
  // parasoft-begin-suppress AUTOSAR-A15_4_5-a "False positive: There is no function that throws an exception of 'Any'
  // type"
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False Positive: 'swap' does not hide anything"
  constexpr void swap(string_view& other) noexcept { ::arene::base::swap(str_span_, other.str_span_); }
  // parasoft-end-suppress AUTOSAR-A2_10_1-d
  // parasoft-end-suppress AUTOSAR-A15_4_5-a

  /// @brief Get a pointer to the first character in the string.
  /// @return A pointer to the first character in the string.
  constexpr auto data() const noexcept -> const_pointer { return str_span_.data(); }

  /// @brief Assignment operator for the string view
  ///
  /// @param other The other string to assign from
  /// @return reference to the current string_view
  constexpr auto operator=(string_view const& other) noexcept -> string_view& = default;

  /// @brief Move assignment operator for the string view
  ///
  /// @param other The other string to move from
  /// @return reference to the current string_view
  constexpr auto operator=(string_view&& other) noexcept -> string_view& = default;

  /// @brief Get a character at position pos in the string
  ///
  /// @param pos Index of the character to access.
  /// @return A reference to the character at pos position.
  /// @pre <c>pos < size()</c> else @c ARENE_PRECONDITION violation
  constexpr auto operator[](std::size_t pos) const noexcept -> const_reference {
    ARENE_PRECONDITION(pos < size());
    return str_span_[pos];
  }

  /// @brief Copy the first <c>min(n,size())</c> characters to the destination buffer.
  ///
  /// @param dest A pointer to the destination buffer.
  /// @param n The number of characters to attempt to copy.
  /// @return The number of characters actually copied.
  constexpr auto copy(character* const dest, size_type const n) const noexcept -> size_type {
    return copy(dest, n, 0U);
  }

  // parasoft-begin-suppress AUTOSAR-A15_4_5-a "False positive: none of the expressions using 'operator=' throw"
  /// @brief Copy the first <c>min(n,size()-pos)</c> characters to the destination buffer.
  ///
  /// @param dest A pointer to the destination buffer to copy the characters to.
  /// @param n The maximum number of characters to copy
  /// @param pos The offset in the string of the first character to copy. If @c >=size() , no content is copied.
  /// @return The number of characters actually copied.
  constexpr auto copy(character* const dest, size_type const n, size_type const pos) const noexcept -> size_type {
    auto const sub = substr(pos, n);
    auto const target = span<character>{dest, sub.length()};
    std::ignore = ::arene::base::copy(sub.begin(), sub.end(), target.begin());
    return sub.size();
  }
  // parasoft-end-suppress AUTOSAR-A15_4_5-a

  /// @brief Get the first character in the string
  ///
  /// @return A reference to the first character
  /// @pre <c>empty() != false</c> else @c ARENE_PRECONDITION violation
  constexpr auto front() const noexcept -> const_reference {
    ARENE_PRECONDITION(!empty());
    return str_span_.front();
  }

  /// @brief Get the last character in the string
  ///
  /// @return A reference to the last character
  /// @pre The string must not be empty
  /// @pre <c>empty() != false</c> else @c ARENE_PRECONDITION violation
  constexpr auto back() const noexcept -> const_reference {
    ARENE_PRECONDITION(!empty());
    return str_span_.back();
  }

  /// @brief Remove the first @c n characters from the string
  ///
  /// @param count the number of characters to remove
  /// @post The string_view will be equivalent to a substring produced as if by <c>substr(n)</c>
  // parasoft-begin-suppress AUTOSAR-A7_1_3-a-2 "False positive: const is placed on the right hand side"
  constexpr void remove_prefix(size_type const count) noexcept { *this = substr(count); }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a-2

  /// @brief Remove the last @c n characters from the string
  ///
  /// @param count the number of characters to remove
  /// @post The string_view will be equivalent to a substring produced as if by
  ///       <c>substr(0, size() - std::min(count, size()))</c>
  // parasoft-begin-suppress AUTOSAR-A7_1_3-a-2 "False positive: const is placed on the right hand side"
  constexpr void remove_suffix(size_type const count) noexcept { *this = substr(0U, size() - std::min(count, size())); }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a-2

  /// @brief Check if the string starts with another string
  /// @param other The string to compare against.
  /// @return true if the first <c>other.size()</c> characters of @c this are equal to those in @c other .
  /// @return false otherwise.
  // parasoft-begin-suppress AUTOSAR-A7_1_3-a-2 "False Positive: const is on rhs"
  constexpr auto starts_with(string_view const other) const noexcept -> bool {
    return substr(0U, other.size()) == other;
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a-2

  /// @brief Check if the string starts with another string
  /// @param other The string to compare against.
  /// @return true if the first <c>string_length(other)</c> characters of @c this are equal to those in @c other .
  /// @return false otherwise.
  /// @pre @c other must be a null terminated string or else behavior is undefined.
  // parasoft-begin-suppress AUTOSAR-A7_1_3-a-2 "False Positive: const is on rhs"
  constexpr auto starts_with(raw_c_string const other) const noexcept -> bool {
    return starts_with(string_view{other});
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a-2

  /// @brief Check if the string starts with a specific character
  /// @param chr The character to check
  /// @return true if the string is not empty and the first character is @c c
  /// @return false otherwise.
  constexpr auto starts_with(character const chr) const noexcept -> bool { return (!empty()) && (front() == chr); }

  /// @brief Check if the string ends with another string
  /// @param other The other string
  /// @return true if the last <c>other.size()</c> characters of @c this are equal to those in @c other .
  /// @return false otherwise.
  // parasoft-begin-suppress AUTOSAR-A7_1_3-a-2 "False Positive: const is on rhs"
  constexpr auto ends_with(string_view const other) const noexcept -> bool {
    return (other.size() <= size()) && substr(size() - other.size(), other.size()) == other;
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a-2

  /// @brief Check if the string ends with another string
  /// @param other The other string
  /// @return true if the last <c>string_length(other)</c> characters of @c this are equal to those in @c other .
  /// @return false otherwise.
  /// @pre @c other must be a null terminated string or else behavior is undefined.
  // parasoft-begin-suppress AUTOSAR-A7_1_3-a-2 "False Positive: const is on rhs"
  constexpr auto ends_with(raw_c_string const other) const noexcept -> bool { return ends_with(string_view{other}); }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a-2

  /// @brief Check if the string ends with a specific character
  /// @param chr The character to check
  /// @return @c true if the string is not empty and the last character is @c c
  constexpr auto ends_with(character const chr) const noexcept -> bool { return (!empty()) && (back() == chr); }

  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  /// @brief Find the earliest position of the given substring within the string, starting at the specified position
  /// @param str The substring to search for
  /// @param pos The lowest position to search from. Defaults to @c 0 .
  /// @return The offset at which @c str can be found within the string, or @c npos if not found.
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
  constexpr auto find(string_view const str, size_type const pos = 0U) const noexcept -> size_type {
    if (pos > size()) {
      return npos;
    }
    // parasoft-begin-suppress AUTOSAR-A2_10_1-a "False Positive: 'index' does not hide anything"
    for (size_type index{pos}; (size() - index) >= str.size(); ++index) {
      if (substr(index, str.size()) == str) {
        return index;
      }
    }
    // parasoft-end-suppress AUTOSAR-A2_10_1-a
    return npos;
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a

  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  /// @brief Find the earliest position of the given substring within the string, starting at the specified position
  /// @param str The substring to search for
  /// @param pos The lowest position to search from. Defaults to @c 0 .
  /// @return The offset at which @c str can be found within the string, or @c npos if not found.
  /// @pre @c str must be a null terminated string or else behavior is undefined.
  constexpr auto find(raw_c_string const str, size_type const pos = 0U) const noexcept -> size_type {
    return find(string_view{str}, pos);
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a

  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  /// @brief Find the earliest position of the @c n character string starting at @c str within the string, starting at
  /// the specified position
  /// @param str The start of the string to search for
  /// @param pos The lowest position in @c this to search from
  /// @param n The length of the string to search for
  /// @return Equivalent to <c>find(string_view{str, n}, pos)</c>
  /// @pre @c str must be a pointer to a character array of at least @c n characters else behavior is undefined.
  constexpr auto find(raw_c_string const str, size_type const pos, size_type const n) const noexcept -> size_type {
    return find(string_view{str, n}, pos);
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a

  /// @brief Find the earliest position of the specified character within the string, starting at the specified position
  /// @param chr The character to search for.
  /// @param pos The lowest position to search from. Defaults to @c 0.
  /// @return The position of that character, or @c npos if not found
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  constexpr auto find(character const chr, size_type const pos = 0U) const noexcept -> size_type {
    difference_type const offset{static_cast<difference_type>(std::min(pos, size()))};
    auto const location = ::arene::base::find(begin() + offset, end(), chr);
    return location == end() ? npos : static_cast<size_type>(location - begin());
  }

  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  /// @brief Find the latest position of the given substring within the string, starting at the specified position
  /// @param str The substring to search for
  /// @param pos The highest position to search from. If <c>pos >= size()</c>, the search starts from the end of the
  ///                string_view.
  /// @return The offset at which @c str can be found within the string, or @c npos if not found.
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
  constexpr auto rfind(string_view const str, size_type const pos = npos) const noexcept -> size_type {
    // parasoft-begin-suppress AUTOSAR-A2_10_1-a "False Positive: 'index' does not hide anything"
    for (size_type index{pos > size() ? size() : pos}; index <= size(); --index) {
      if (substr(index, str.size()) == str) {
        return index;
      }
    }
    // parasoft-end-suppress AUTOSAR-A2_10_1-a "False Positive: 'index' does not hide anything"
    return npos;
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a

  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  /// @brief Find the latest position of the given substring within the string, starting at the specified position
  /// @param str The substring to search for
  /// @param pos The highest position to search from. If <c>pos >= size()</c>, the search starts from the end of the
  ///                string_view.
  /// @return The offset at which @c str can be found within the string, or @c npos if not found.
  /// @pre @c str must be a null terminated string or else behavior is undefined.
  constexpr auto rfind(raw_c_string const str, size_type const pos = npos) const noexcept -> size_type {
    return rfind(string_view{str}, pos);
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a

  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  /// @brief Find the latest position of the @c n character string starting at @c str
  /// within the string, starting at the specified position
  /// @param str The start of the string to search for
  /// @param pos The highest position in @c this to search from. If <c>pos >= size()</c>, the search starts from the
  ///                end of the string_view.
  /// @param n The length of the string to search for
  /// @return The offset at which @c str can be found within the string, or @c npos if not found.
  /// @pre @c str must be a pointer to a character array of at least @c n characters else behavior is undefined.
  constexpr auto rfind(raw_c_string const str, size_type const pos, size_type const n) const noexcept -> size_type {
    return rfind(string_view{str, n}, pos);
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a

  /// @brief Find the latest position of the specified character within the string, starting at the specified position
  /// @param chr The character to search for
  /// @param pos The highest position to search from. If <c>pos >= size()</c>, the search starts from the end of the
  ///                string_view.
  /// @return The position of that character, or @c npos if not found
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  constexpr auto rfind(character const chr, size_type const pos = npos) const noexcept -> size_type {
    auto start = rbegin();
    // parasoft-begin-suppress AUTOSAR-M5_0_7-b-2 "False positive: no floating point conversions"
    // parasoft-begin-suppress AUTOSAR-M5_0_8-a-2 "False positive: the sizes of the types are the same"
    // parasoft-begin-suppress AUTOSAR-M5_0_9-a-2 "False positive: Guaranteed in-range by precondition"
    if (pos <= size()) {
      start += static_cast<difference_type>(size() - pos);
    }
    auto const location = ::arene::base::find(start, rend(), chr);
    return location == rend() ? npos : static_cast<size_type>(rend() - location - 1);
    // parasoft-end-suppress AUTOSAR-M5_0_7-b-2
    // parasoft-end-suppress AUTOSAR-M5_0_8-a-2
    // parasoft-end-suppress AUTOSAR-M5_0_9-a-2
  }

  // parasoft-begin-suppress AUTOSAR-A15_4_5-a "False positive: 'str_span_[]' does not throw"
  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  /// @brief Finds the first character equal to any of the characters in the given character sequence.
  /// @param str The list of characters to find any of
  /// @param pos The start position to search from. Defaults to @c 0 .
  /// @return The offset of the first character in @c this that matches any character in @c str if there is one,
  ///          otherwise @c npos
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
  constexpr auto find_first_of(string_view const str, size_type const pos = 0U) const noexcept -> size_type {
    if (str.empty()) {
      return npos;
    }
    // parasoft-begin-suppress AUTOSAR-A2_10_1-a "False Positive: 'index' does not hide anything"
    for (size_type index{pos}; index < size(); ++index) {
      if (str.find(str_span_[index]) != npos) {
        return index;
      }
    }
    // parasoft-end-suppress AUTOSAR-A2_10_1-a
    return npos;
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a
  // parasoft-end-suppress AUTOSAR-A15_4_5-a

  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  /// @brief Finds the first character equal to any of the characters in the given character sequence.
  /// @param str The list of characters to find any of
  /// @param pos The start position to search from. Defaults to @c 0 .
  /// @return The offset of the first character in @c this that matches any character in @c str if there is one,
  ///          otherwise @c npos
  /// @pre @c str must be a null terminated string or else behavior is undefined.
  constexpr auto find_first_of(raw_c_string const str, size_type const pos = 0U) const noexcept -> size_type {
    return find_first_of(string_view{str}, pos);
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a

  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  /// @brief Finds the first character equal to any of the characters in the given character sequence.
  /// @param str The list of characters to find any of.
  /// @param pos The start position in @c this to search from.
  /// @param n The number of characters in @c str to consider.
  /// @return The offset of the first character in @c str after @c pos if there is one, otherwise @c npos
  /// @pre @c str must be a pointer to a character array of at least @c n characters else behavior is undefined.
  constexpr auto find_first_of(raw_c_string const str, size_type const pos, size_type const n) const noexcept
      -> size_type {
    return find_first_of(string_view{str, n}, pos);
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a

  /// @brief Finds the first character equal to the specified character
  /// @param chr The character to find
  /// @param pos The start position to search from. Defaults to @c 0 .
  /// @return The offset of the first character in @c this that matches @c chr if there is one, otherwise @c npos
  constexpr auto find_first_of(character const chr, size_type const pos = 0U) const noexcept -> size_type {
    return find(chr, pos);
  }

  // parasoft-begin-suppress AUTOSAR-A15_4_5-a "False positive: 'str_span_[]' does not throw"
  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  /// @brief Finds the first character not equal to any of the characters in the given character sequence.
  /// @param str The list of characters to compare against.
  /// @param pos The start position to search from. Defaults to @c 0 .
  /// @return The offset of the first character not in @c str after @c pos if there is one, otherwise @c npos
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
  constexpr auto find_first_not_of(string_view const str, size_type const pos = 0U) const noexcept -> size_type {
    if (str.empty()) {
      return pos < size() ? pos : npos;
    }
    // parasoft-begin-suppress AUTOSAR-A2_10_1-a "False Positive: 'index' does not hide anything"
    for (size_type index{pos}; index < size(); ++index) {
      if (str.find(str_span_[index]) == npos) {
        return index;
      }
    }
    // parasoft-end-suppress AUTOSAR-A2_10_1-a
    return npos;
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a
  // parasoft-end-suppress AUTOSAR-A15_4_5-a

  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  /// @brief Finds the first character not equal to any of the characters in the given character sequence.
  /// @param str The list of characters to compare against.
  /// @param pos The start position to search from. Defaults to @c 0 .
  /// @return The offset of the first character not in @c str after @c pos if there is one, otherwise @c npos
  /// @pre @c str must be a null terminated string or else behavior is undefined.
  constexpr auto find_first_not_of(raw_c_string const str, size_type const pos = 0U) const noexcept -> size_type {
    return find_first_not_of(string_view{str}, pos);
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a

  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  /// @brief Finds the first character not equal to any of the characters in the given character sequence.
  /// @param str The list of characters to compare against.
  /// @param pos The start position to search from in @c this .
  /// @param n The number of characters in @c str to consider.
  /// @return The offset of the first character not in @c str after @c pos if there is one, otherwise @c npos
  /// @pre @c str must be a pointer to a character array of at least @c n characters else behavior is undefined.
  constexpr auto find_first_not_of(raw_c_string const str, size_type const pos, size_type const n) const noexcept
      -> size_type {
    return find_first_not_of(string_view{str, n}, pos);
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a

  /// @brief Finds the first character not equal to the provided character.
  /// @param chr The character to find
  /// @param pos The start position to search from. Defaults to @c 0 .
  /// @return The offset of the first character not matching @c chr if there is one, otherwise @c npos
  constexpr auto find_first_not_of(character const chr, size_type const pos = 0U) const noexcept -> size_type {
    return find_first_not_of(string_view{&chr, 1U}, pos);
  }

  // parasoft-begin-suppress AUTOSAR-A15_4_5-a "False positive: 'str_span_[]' does not throw"
  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  /// @brief Finds the last character equal to one of characters in the given character sequence.
  /// @param str The list of characters to compare against.
  /// @param pos The highest position to search from. If <c>pos >= size()</c>, the search starts from the end of the
  ///                string_view. Defaults to @c npos .
  /// @return The offset of the last character matching any of those in @c str if there is one, otherwise @c npos
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
  constexpr auto find_last_of(string_view const str, size_type const pos = npos) const noexcept -> size_type {
    if (str.empty()) {
      return npos;
    }
    // parasoft-begin-suppress AUTOSAR-A2_10_1-a "False Positive: 'index' does not hide anything"
    for (size_type index{pos >= size() ? size() - 1U : pos}; index < size(); --index) {
      if (str.find(str_span_[index]) != npos) {
        return index;
      }
    }
    // parasoft-end-suppress AUTOSAR-A2_10_1-a
    return npos;
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a
  // parasoft-end-suppress AUTOSAR-A15_4_5-a

  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  /// @brief Finds the last character equal to one of characters in the given character sequence.
  /// @param str The list of characters to compare against.
  /// @param pos The highest position to search from. If <c>pos >= size()</c>, the search starts from the end of the
  ///                string_view. Defaults to @c npos .
  /// @return The offset of the last character matching any of those in @c str if there is one, otherwise @c npos
  /// @pre @c str must be a null terminated string or else behavior is undefined.
  constexpr auto find_last_of(raw_c_string const str, size_type const pos = npos) const noexcept -> size_type {
    return find_last_of(string_view{str}, pos);
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a

  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  /// @brief Finds the last character equal to one of characters in the given character sequence.
  /// @param str The list of characters to compare against.
  /// @param pos The highest position in @c this to search from. If <c>pos >= size()</c>, the search starts from the
  ///                end of the string_view.
  /// @param n The number of characters in @c str to consider.
  /// @return The offset of the last character matching any of the first @c n characters of @c str at or after @c pos
  ///          if there is one, otherwise @c npos
  /// @pre @c str must be a pointer to a character array of at least @c n characters else behavior is undefined.
  constexpr auto find_last_of(raw_c_string const str, size_type const pos, size_type const n) const noexcept
      -> size_type {
    return find_last_of(string_view{str, n}, pos);
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a

  /// @brief Finds the last character equal to the provided character.
  /// @param chr The character to find.
  /// @param pos The highest position to search from. If <c>pos >= size()</c>, the search starts from the end of the
  ///                string_view.
  /// @return The offset of the last character matching @c chr if there is one, otherwise @c npos
  constexpr auto find_last_of(character const chr, size_type const pos = npos) const noexcept -> size_type {
    return rfind(chr, pos);
  }

  // parasoft-begin-suppress AUTOSAR-A15_4_5-a "False positive: 'str_span_[]' does not throw"
  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  /// @brief Finds the last character not equal to any of the characters in the given character sequence.
  /// @param str The list of characters to consider.
  /// @param pos The highest position to search from. If <c>pos >= size()</c>, the search starts from the end of the
  ///                string_view. Defaults to @c npos
  /// @return The offset of the last character not in @c str if there is one, otherwise @c npos
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
  constexpr auto find_last_not_of(string_view const str, size_type const pos = npos) const noexcept -> size_type {
    if (str.empty()) {
      return pos < size() ? pos : size() - 1U;
    }
    // parasoft-begin-suppress AUTOSAR-A2_10_1-a "False Positive: 'index' does not hide anything"
    for (size_type index{pos >= size() ? size() - 1U : pos}; index < size(); --index) {
      if (str.find(str_span_[index]) == npos) {
        return index;
      }
    }
    // parasoft-end-suppress AUTOSAR-A2_10_1-a
    return npos;
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a
  // parasoft-end-suppress AUTOSAR-A15_4_5-a

  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  /// @brief Finds the last character not equal to any of the characters in the given character sequence.
  /// @param str The list of characters to consider.
  /// @param pos The highest position to search from. If <c>pos >= size()</c>, the search starts from the end of the
  ///                string_view. Defaults to @c npos
  /// @return The offset of the last character not in @c str if there is one, otherwise @c npos
  /// @pre @c str must be a null terminated string or else behavior is undefined.
  constexpr auto find_last_not_of(raw_c_string const str, size_type const pos = npos) const noexcept -> size_type {
    return find_last_not_of(string_view{str}, pos);
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a

  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  /// @brief Finds the last character not equal to any of the characters in the given character sequence.
  /// @param str The list of characters to consider.
  /// @param pos The highest position in @c this to search from. If <c>pos >= size()</c>, the search starts from the
  ///                end of the string_view.
  /// @param n The number of characters in @c str to consider
  /// @return The offset of the last character not in @c str at or before @c pos if there is one, otherwise @c npos
  /// @pre @c str must be a pointer to a character array of at least @c n characters else behavior is undefined.
  constexpr auto find_last_not_of(raw_c_string const str, size_type const pos, size_type const n) const noexcept
      -> size_type {
    return find_last_not_of(string_view{str, n}, pos);
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a

  /// @brief Finds the last character equal to the provided character.
  /// @param chr The character to find.
  /// @param pos The highest position to search from. If <c>pos >= size()</c>, the search starts from the end of the
  ///                string_view. Defaults to @c npos .
  /// @return The offset of the last character not equal to @c chr if there is one, otherwise @c npos
  constexpr auto find_last_not_of(character const chr, size_type const pos = npos) const noexcept -> size_type {
    return find_last_not_of(string_view{&chr, 1U}, pos);
  }

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: Does not hide any identifiers"
  /// @brief Compare two @c string_view objects lexicographically.
  ///
  /// @param lhs The first string to compare
  /// @param rhs The second string to compare
  ///
  /// @return strong_ordering::equal if the length of lhs and rhs are equal and all characters compare equal.
  /// @return strong_ordering::less if the value of the first character that does not match is lexicographically less
  /// in
  ///          lhs than rhs.
  /// @return strong_ordering::less if the length of lhs is less than the length of rhs and all characters in the
  ///          overlapping subset of rhs match those in lhs.
  /// @return strong_ordering::greater if the value of the first character that does not match is lexicographically
  ///          greater in lhs than rhs.
  /// @return strong_ordering::greater if the length of lhs is greater than the length of rhs and all characters in the
  ///          overlapping subset of lhs match those in rhs.
  static constexpr auto three_way_compare(string_view const lhs, string_view const rhs) noexcept -> strong_ordering {
    return detail::lexicographic_string_compare(lhs.str_span_, rhs.str_span_);
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  // parasoft-begin-suppress AUTOSAR-A13_5_2-a-2 "Implicit conversion to std::string is part of the API"
  /// @brief Implicit conversion to std::string. Provided since we cannot patch C++17's string_view constructor onto
  /// std::string.
  // NOLINTNEXTLINE(hicpp-explicit-conversions, google-explicit-constructor)
  operator std::string() const;
  // parasoft-end-suppress AUTOSAR-A13_5_2-a-2

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: Does not hide any identifiers"
  /// @brief quick check that string views have the same size to quickly identify non-equal strings.
  ///
  /// @param first The first string_view to compare.
  /// @param second The second string_view to compare.
  /// @return inequality_heuristic @c inequality_heuristic::definitely_not_equal if the strings have different lengths,
  /// @c inequality_heuristic::may_be_equal_or_not_equal otherwise.
  static constexpr auto fast_inequality_check(string_view const first, string_view const second) noexcept
      -> inequality_heuristic {
    return (first.size() == second.size()) ? inequality_heuristic::may_be_equal_or_not_equal
                                           : inequality_heuristic::definitely_not_equal;
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

 private:
  /// @brief A @c span over the character sequence
  const_char_span str_span_;
};
// parasoft-end-suppress AUTOSAR-A13_5_1-a-2

// parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
/// @brief Construct a @c std::string from the @c string_view
/// @param str The @c string_view to convert
/// @return A new @c std::string holding a copy of the string from the view
// NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
auto to_string(string_view const str) -> std::string;
// parasoft-end-suppress AUTOSAR-A7_1_3-a

namespace literals {

// parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
/// @brief String literal operator to construct a string_view from an annotated string literal. This @c string_view
/// operator is named "asv" (arene string view) to avoid any conflict with the sv operator in a third party code base
/// @param str A pointer to the start of the string literal characters
/// @param len The number of characters in the string literal
/// @return A @c string_view for the string literal
constexpr auto operator""_asv(detail::raw_c_string const str, std::size_t const len) noexcept -> string_view {
  return {str, len};
}
// parasoft-end-suppress AUTOSAR-A7_1_3-a

}  // namespace literals

// NOLINTEND(google-explicit-constructor, hicpp-avoid-c-arrays)

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-A3_1_5-a

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_STRING_VIEW_HPP_
