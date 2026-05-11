// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_INLINE_STRING_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_INLINE_STRING_HPP_

// IWYU pragma: private
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

#include "arene/base/algorithm/copy.hpp"
#include "arene/base/algorithm/fill.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/compare/operators.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/detail/exceptions.hpp"
#include "arene/base/detail/raw_c_string.hpp"
#include "arene/base/detail/wrapped_iterator.hpp"  // IWYU pragma: keep
#include "arene/base/iterator/distance.hpp"
#include "arene/base/iterator/reverse_iterator.hpp"
#include "arene/base/optional/optional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/stdlib_choice/min_value_overload.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/strings/detail/inline_string_std_string_mixin.hpp"  // IWYU pragma: keep
#include "arene/base/strings/null_terminated_string_view.hpp"
#include "arene/base/strings/string_view.hpp"
#include "arene/base/type_traits/denotes_range.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "arene/base/utility/make_subrange.hpp"
#include "arene/base/utility/safe_comparisons.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {

namespace inline_string_detail {

///
/// @brief Base class for @c inline_string which detemplatizes functionality not dependent on the buffer size.
///
class inline_string_base {
 public:
  /// @brief The type used for @c size() and @c length()
  using size_type = std::size_t;

 protected:
  /// @brief passkey class for constructing iterators.
  class iterator_passkey {
   public:
    /// @brief The default constructor is explicit to disable construction with just @c {}
    constexpr explicit iterator_passkey() noexcept = default;
  };

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Default constructor, protected to prevent slicing
  constexpr inline_string_base() = default;
  /// @brief Copy constructor, protected to prevent slicing
  constexpr inline_string_base(inline_string_base const&) = default;
  /// @brief Move constructor, protected to prevent slicing
  constexpr inline_string_base(inline_string_base&&) = default;
  /// @brief Copy assignment operator, protected to prevent slicing
  constexpr auto operator=(inline_string_base const&) -> inline_string_base& = default;
  /// @brief Move assignment operator, protected to prevent slicing
  constexpr auto operator=(inline_string_base&&) -> inline_string_base& = default;
  /// @brief Destructor, protected to prevent slicing
  ~inline_string_base() = default;
  // parasoft-end-suppress CERT_C-EXP37-a-3

 public:
  /// @brief The type of the elements contained in the string.
  using value_type = detail::character;
  /// @brief The type of a reference to an element in the string.
  using reference = value_type&;
  /// @brief The type of a const reference to an element in the string.
  using const_reference = value_type const&;
  /// @brief The type of a pointer to an element in the string.
  using pointer = value_type*;
  /// @brief The type of a const pointer to an element in the string
  using const_pointer = value_type const*;
  /// @brief The non-const iterator type
  using iterator = ::arene::base::detail::wrapped_iterator<pointer, iterator_passkey>;
  /// @brief The const iterator type
  using const_iterator = ::arene::base::detail::wrapped_iterator<const_pointer, iterator_passkey>;
  /// @brief The type of a non-const reverse iterator
  using reverse_iterator = ::arene::base::reverse_iterator<iterator>;
  /// @brief The type of a const reverse iterator
  using const_reverse_iterator = ::arene::base::reverse_iterator<const_iterator>;
  /// @brief The difference type between two iterators.
  using difference_type = typename iterator::difference_type;

  /// @brief Get the length of the string
  ///
  /// @return The number of characters in the string, non-inclusive of the null-terminator. Equivalent to
  /// @c distance(begin(),end())
  constexpr auto size() const noexcept -> size_type { return string_length_; }

  /// @brief Get the length of the string
  ///
  /// @return The same as @c size()
  constexpr auto length() const noexcept -> size_type { return size(); }

  // parasoft-begin-suppress AUTOSAR-A8_4_7-b-2 "Base class passed by reference to avoid slicing"
  /// @brief Helper to detemplatize fast_inequality_check to remove uncovered branches for inline_string instantiations.
  ///
  /// @tparam SizeType The type used for the size comparison
  /// @param lhs The left hand size
  /// @param rhs The right hand size
  /// @return inequality_heuristic::definitely_not_equal if lhs != rhs
  /// @return inequality_heuristic::may_be_equal_or_not_equal if lhs == rhs
  ///
  static constexpr auto common_fast_inequality_check(inline_string_base const& lhs, string_view const& rhs) noexcept
      -> inequality_heuristic {
    return lhs.size() != rhs.size() ? inequality_heuristic::definitely_not_equal
                                    : inequality_heuristic::may_be_equal_or_not_equal;
  }
  // parasoft-end-suppress AUTOSAR-A8_4_7-b-2

 protected:
  /// @brief sets the size to a new value
  ///
  /// @param new_size the new size to set
  /// @post @c size() will be @c new_size
  constexpr void set_size(size_type const new_size) noexcept { string_length_ = new_size; }

 private:
  /// @brief The current size of the string
  size_type string_length_{0U};
};

}  // namespace inline_string_detail

// parasoft-begin-suppress AUTOSAR-A10_1_1-a-2 "False positive: 'generic_ordering_from_three_way_compare' is an
// interface"
/// @brief A string type with a fixed maximum capacity, where the data is stored internally without allocations.
///
/// @tparam MaxSize the maximum capacity of the string, _not_ including the null terminator. Must be non-zero, and
///         less than @c std::numeric_limits<difference_type>::max()
template <std::size_t MaxSize>
// NOLINTNEXTLINE(hicpp-special-member-functions) provides converting ctors that are equivalent to copy/move.
class inline_string
    : public inline_string_detail::inline_string_base
    , public detail::mixin_at<inline_string<MaxSize>, typename inline_string_detail::inline_string_base::size_type>
    , inline_string_std_string_mixin<inline_string<MaxSize>>
    , generic_ordering_from_three_way_compare<inline_string<MaxSize>> {
  /// @brief Internal type alias for the base class
  using ordering_base = generic_ordering_from_three_way_compare<inline_string<MaxSize>>;

  // parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "Returning an int for compatibility with std::string_view"
  /// @brief The type of comparisons that yield an integer rather than a @c strong_ordering
  using integral_comparison_result = int;
  // parasoft-end-suppress AUTOSAR-A3_9_1-b-2

 public:
  // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
  // side-effects"
  static_assert(
      cmp_less(MaxSize, std::numeric_limits<difference_type>::max()),
      "Must have ability to get difference between iterators"
  );
  // parasoft-end-suppress CERT_C-PRE31-c-3

  // member types
  using typename inline_string_base::size_type;
  using typename inline_string_base::value_type;

  using typename inline_string_base::const_reference;
  using typename inline_string_base::reference;

  using typename inline_string_base::const_pointer;
  using typename inline_string_base::pointer;

  using typename inline_string_base::const_iterator;
  using typename inline_string_base::const_reverse_iterator;
  using typename inline_string_base::iterator;
  using typename inline_string_base::reverse_iterator;

  using inline_string_std_string_mixin<inline_string>::three_way_compare;
  using inline_string_std_string_mixin<inline_string>::fast_inequality_check;

  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property"
  /// @brief Special marker for element not in string.
  static constexpr size_type npos{string_view::npos};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2

  /// @brief Use the default destructor
  ~inline_string() = default;

  /// @brief Default-construct an empty string
  constexpr inline_string() noexcept = default;

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Default copy construction
  constexpr inline_string(inline_string const&) = default;
  // parasoft-end-suppress CERT_C-EXP37-a-3

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Default move construction
  constexpr inline_string(inline_string&&) = default;
  // parasoft-end-suppress CERT_C-EXP37-a-3

  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Construct from the provided NUL-terminated string.
  ///
  /// @param str The source string
  /// @pre The length of @c str must be less than @c MaxLength , else @c ARENE_PRECONDITION violation.
  /// @pre @c str must point to a NUL-terminated string, else behavior is undefined.
  /// @post @c size() is the length of @c str
  /// @post The content of this string is equivalent to the content of @c str
  constexpr explicit inline_string(detail::raw_c_string const str) noexcept
      : inline_string{} {
    copy_from(str);
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2
  // parasoft-end-suppress AUTOSAR-A7_1_3-a

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Construct from the source string literal
  ///
  /// @tparam N the size of the source character array. Must be @c <=(MaxLength+1)
  /// @param str The string to copy from
  /// @pre @c str must have a NUL terminator somewhere in it
  /// @post The content of this string compares equal to @c str
  template <std::size_t N>
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays, hicpp-explicit-conversions) string literal implementation can safely convert
  constexpr inline_string(detail::character const (&str)[N]) noexcept
      : inline_string{} {
    static_assert(N - 1UL <= MaxSize, "String literal must fit into MaxLength");

    // Like std::string, copy until the first NUL; if there's no NUL, it's not really a string => precondition violation
    string_view as_view{&str[0], N};
    auto const first_nul_idx = as_view.find('\0');
    ARENE_PRECONDITION(first_nul_idx < as_view.size());
    as_view = as_view.substr(0UL, first_nul_idx);

    // We've just made sure that as_view.size() <= N - 1UL <= MaxSize, so we don't need the checked copy_from
    copy_from_unchecked(as_view);
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Construct from the provided string_view.
  ///
  /// @param str The source string
  /// @pre The length of @c str must be less than @c MaxLength , else @c ARENE_PRECONDITION violation.
  /// @pre @c str must point to a NUL-terminated string, else behavior is undefined.
  /// @post @c size() is the length of @c str
  /// @post The content of this string is equivalent to the content of @c str
  constexpr explicit inline_string(string_view const str) noexcept
      : inline_string{} {
    copy_from(str);
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2
  // parasoft-end-suppress AUTOSAR-A7_1_3-a

  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  /// @brief Attempt to construct from a @c string_view .
  ///
  /// @param str The source string.
  /// @return optional<inline_string> If @c str.size()>MaxLength , a null optional, else an optional populated as if
  ///          via @c inline_string{str} .
  ///
  static constexpr auto try_construct(string_view const str) noexcept -> optional<inline_string> {
    if (str.length() > MaxSize) {
      return nullopt;
    }
    return {inline_string{str}};
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Implicit conversion from another @c inline_string which can provably fit in this @c inline_string.
  ///
  /// @tparam OtherMaxLength The length of the other string. Must be @c <= MaxLength
  /// @param other The other string
  /// @post @c size()==other.size()
  /// @post The content of this string is equivalent to the content of @c other
  template <size_type OtherMaxLength, constraints<std::enable_if_t<(OtherMaxLength <= MaxSize)>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions) Morally this is a copy-ctor not a conversion
  constexpr inline_string(inline_string<OtherMaxLength> const& other) noexcept
      : inline_string{} {
    copy_from(other);
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Explicit conversion from another @c inline_string which cannot provably fit in this @c inline_string.
  ///
  /// @tparam OtherMaxLength The length of the other string
  /// @param other The other string
  /// @pre @c other.size() is less than or equal to @c MaxLength else @c ARENE_PRECONDITION violation
  /// @post @c size()==other.size()
  /// @post The content of this string is equivalent to the content of @c other
  template <size_type OtherMaxLength, constraints<std::enable_if_t<(OtherMaxLength > MaxSize)>> = nullptr>
  constexpr explicit inline_string(inline_string<OtherMaxLength> const& other) noexcept
      : inline_string{} {
    copy_from(other);
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a "False positive: This constructor delegates to another, which does
  // initialize the base class"

  /// @brief Construct a pre-filled @c inline_string of a given length.
  ///
  /// @param len The length of the constructed string.
  /// @param chr The character to fill the string.
  /// @pre @c len must be less than or equal to @c MaxLength , else @c ARENE_PRECONDITION violation.
  /// @post @c size() is equal to @c len .
  /// @post This string is filled with @c chr .
  constexpr inline_string(size_type len, detail::character chr) noexcept
      : inline_string{} {
    resize(len, chr);
  }

  // parasoft-end-suppress AUTOSAR-A12_1_1-a

  /// @brief Attempt to construct from another @c inline_string with a capacity that is less than or equal to the
  /// capacity of this.
  ///
  /// @tparam OtherMaxSize The capacity of the other string
  /// @param str The source string.
  /// @return If @c str.size()>MaxLength , a null optional, else an optional populated as if via @c inline_string{str} .
  template <size_type OtherMaxSize, constraints<std::enable_if_t<(OtherMaxSize <= MaxSize)>> = nullptr>
  static constexpr auto try_construct(inline_string<OtherMaxSize> const& str) noexcept -> optional<inline_string> {
    return {inline_string{str}};
  }
  /// @brief Attempt to construct from another @c inline_string with a larger capacity.
  ///
  /// @tparam OtherMaxSize The capacity of the other string
  /// @param str The source string.
  /// @return If @c str.size()>MaxLength , a null optional, else an optional populated as if via @c inline_string{str} .
  template <size_type OtherMaxSize, constraints<std::enable_if_t<(OtherMaxSize > MaxSize)>> = nullptr>
  static constexpr auto try_construct(inline_string<OtherMaxSize> const& str) noexcept -> optional<inline_string> {
    if (str.length() > MaxSize) {
      return nullopt;
    }
    return {inline_string{str}};
  }

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Construction from a sequence of characters represented as a pair of iterators.
  ///
  /// @tparam InputIterator The type of the iterators
  /// @param first The start of the iterator range
  /// @param last The end of the iterator range
  /// @pre @c std::distance(first,last)<=MaxSize else @c ARENE_PRECONDITION violation.
  template <typename InputIterator, constraints<std::enable_if_t<is_input_iterator_v<InputIterator>>> = nullptr>
  constexpr explicit inline_string(
      InputIterator const first,
      InputIterator const last
  ) noexcept(denotes_nothrow_iterable_range_v<InputIterator>)
      : inline_string{} {
    copy_from(typename std::iterator_traits<InputIterator>::iterator_category{}, first, last);
  }
  // parasoft-end-suppress AUTOSAR-A12_1_1-a-2

  /// @brief Attempt to construct from a sequence of characters.
  ///
  /// @param first An iterator referring to the start of the source string
  /// @param last An iterator referring to the end of the source string
  /// @return optional<inline_string> If the length of the range is greater than @c MaxLength , a null optional, else an
  /// optional populated with the elements in the range @c [first,last) .
  ///
  template <typename InputIterator, constraints<std::enable_if_t<is_input_iterator_v<InputIterator>>> = nullptr>
  static constexpr auto
  try_construct(InputIterator first, InputIterator last) noexcept(denotes_nothrow_iterable_range_v<InputIterator>)
      -> optional<inline_string> {
    return inline_string::do_try_construct(
        typename std::iterator_traits<InputIterator>::iterator_category{},
        first,
        last
    );
  }

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Default copy assignment
  /// @return A reference to @c *this
  constexpr auto operator=(inline_string const&) -> inline_string& = default;
  // parasoft-end-suppress CERT_C-EXP37-a-3

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Default move assignment
  /// @return A reference to @c *this
  constexpr auto operator=(inline_string&&) -> inline_string& = default;
  // parasoft-end-suppress CERT_C-EXP37-a-3

  /// @brief Assign from a string_view.
  ///
  /// @param str The source string
  /// @return inline_string& @c *this
  /// @pre The length of @c str must be less than @c MaxLength , else @c ARENE_PRECONDITION violation.
  /// @post @c size() is the length of @c str
  /// @post The content of this string is equivalent to the content of @c str
  constexpr auto operator=(string_view str) noexcept -> inline_string& {
    copy_from(str);
    return *this;
  }

  /// @brief Assign from a nul-terminated string
  ///
  /// @param str The source string
  /// @return inline_string& @c *this
  /// @pre The length of @c str must be less than @c MaxLength , else @c ARENE_PRECONDITION violation.
  /// @pre @c str must point to a NUL-terminated string, else behavior is undefined.
  /// @post @c size() is the length of @c str
  /// @post The content of this string is equivalent to the content of @c str
  constexpr auto operator=(detail::raw_c_string str) noexcept -> inline_string& {
    copy_from(str);
    return *this;
  }

  /// @brief Assign from an initializer list.
  ///
  /// @param str The source string
  /// @return inline_string& @c *this
  /// @pre The length of @c str must be less than @c MaxLength , else @c ARENE_PRECONDITION violation.
  /// @post @c size() is the length of @c str
  /// @post The content of this string is equivalent to the content of @c str
  constexpr auto operator=(std::initializer_list<detail::character> str) noexcept -> inline_string& {
    copy_from(string_view(str.begin(), str.size()));
    return *this;
  }

  /// @brief Assign from a single character
  ///
  /// @param chr The source character
  /// @return inline_string& @c *this
  /// @post @c size() is @c 1
  /// @post The content of this string is @c chr
  constexpr auto operator=(detail::character chr) noexcept -> inline_string& {
    copy_from_unchecked(string_view(&chr, 1U));
    return *this;
  }

  /// @brief Assign from another string.
  ///
  /// @tparam OtherMaxLength the max length of the other string
  /// @param other The source string
  /// @return inline_string& @c *this
  /// @pre @c other.size() is less than or equal to @c MaxLength else @c ARENE_PRECONDITION violation
  /// @post @c size()==other.size()
  /// @post The content of this string is equivalent to the content of @c other
  template <std::size_t OtherMaxLength>
  constexpr auto operator=(inline_string<OtherMaxLength> const& other) noexcept -> inline_string& {
    copy_from(other);
    return *this;
  }

  /// @brief Assign from a string_view.
  ///
  /// @param str The source string
  /// @return inline_string& @c *this
  /// @pre The length of @c str must be less than @c MaxLength , else @c ARENE_PRECONDITION violation.
  /// @post @c size() is the length of @c str
  /// @post The content of this string is equivalent to the content of @c str
  constexpr auto assign(string_view str) noexcept -> inline_string& { return *this = str; }

  /// @brief Assign from a string_view.
  ///
  /// @param str The source string
  /// @param pos The offset in @c str of the substring to assign
  /// @return inline_string& @c *this
  /// @pre  @c str.substr(pos).size() must be less than @c MaxLength , else @c ARENE_PRECONDITION violation.
  /// @post @c size() is @c str.substr(pos).size()
  /// @post The content of this string is equivalent to the content of @c str.substr(pos)
  constexpr auto assign(string_view str, size_type pos) noexcept -> inline_string& { return assign(str.substr(pos)); }

  /// @brief Assign from a substring of a string_view starting at the specified position, with the specified max length.
  ///
  /// @param str The source string
  /// @param pos The offset in @c str of the substring to assign
  /// @param count The maximum number of characters in the substring
  /// @return inline_string& @c *this
  /// @pre  @c str.substr(pos,count).size() must be less than @c MaxLength , else @c ARENE_PRECONDITION violation.
  /// @post @c size() is @c str.substr(pos).size()
  /// @post The content of this string is equivalent to the content of @c str.substr(pos,count)
  constexpr auto assign(string_view str, size_type pos, size_type count) noexcept -> inline_string& {
    return assign(str.substr(pos, count));
  }

  /// @brief Assign from an initializer list of characters.
  ///
  /// @param str The source initializer list
  /// @return inline_string& @c *this
  /// @pre The length of @c str must be less than @c MaxLength , else @c ARENE_PRECONDITION violation.
  /// @post @c size() is the length of @c str
  /// @post The content of this string is equivalent to the content of @c str
  constexpr auto assign(std::initializer_list<detail::character> str) noexcept -> inline_string& { return *this = str; }

  /// @brief Assign from a string specified as pointer and length to the current string.
  ///
  /// @param str The start if the string to assign
  /// @param count The number of characters in the string to assign
  /// @return inline_string& @c *this
  /// @pre str must not be @c nullptr , else @c ARENE_PRECONDITION violation.
  /// @pre str must be a null-terminated string, else behavior is undefined.
  /// @pre count must be less than the length of @c str, else behavior is undefined.
  /// @pre The length of @c str must be less than @c MaxLength , else @c ARENE_PRECONDITION violation.
  /// @post @c size() is the length of @c str
  /// @post The content of this string is equivalent to the content of @c str
  constexpr auto assign(detail::raw_c_string str, size_type count) noexcept -> inline_string& {
    return assign(string_view{str, count});
  }

  /// @brief Assign from a single character
  ///
  /// @param chr The source character
  /// @return inline_string& @c *this
  /// @post @c size() is @c 1
  /// @post The content of this string is @c chr
  constexpr auto assign(detail::character chr) noexcept -> inline_string& { return *this = chr; }

  /// @brief Assign repeated copies of a single character
  ///
  /// @param count The number of times to repeat the character
  /// @param chr The source character
  /// @return inline_string& @c *this
  /// @pre @c count<=MaxLength , else @c ARENE_PRECONDITION violation.
  constexpr auto assign(size_type count, detail::character chr) noexcept -> inline_string& {
    ARENE_PRECONDITION(count <= MaxSize);
    clear();
    resize(count, chr);
    return *this;
  }

  /// @brief Insert the supplied string at the specified position.
  ///
  /// @param pos The insertion position
  /// @param str The string to insert
  /// @return inline_string& @c *this
  /// @pre @c pos<=size() else @c ARENE_PRECONDITION violation.
  /// @pre @c pos+str.length()<max_size() , else @c ARENE_PRECONDITION violation.
  /// @post @c size() is increased by @c str.size() .
  /// @post @c str is inserted at @c pos
  constexpr auto insert(size_type pos, string_view str) noexcept -> inline_string& {
    std::ignore = ::arene::base::copy(str.begin(), str.end(), prepare_insert(pos, str.size()));
    return *this;
  }

  /// @brief Insert the substring of the supplied string starting from @c pos2 at @c pos1.
  ///
  /// @param pos1 The insertion position
  /// @param str The string holding the substring to insert
  /// @param pos2 The position of the start of the substring to insert. If @c >=str.size(), this is a no-op.
  /// @return inline_string& @c *this
  /// @pre @c pos1<=size() else @c ARENE_PRECONDITION violation.
  /// @pre @c pos1+(str.length()-pos2)<max_size() , else @c ARENE_PRECONDITION violation.
  /// @post @c size() is increased by @c (str.size()-pos2) .
  /// @post The range @c [str.begin()+pos2,str.end()) is inserted at @c pos1
  constexpr auto insert(size_type pos1, string_view str, size_type pos2) noexcept -> inline_string& {
    return insert(pos1, str.substr(pos2));
  }

  /// @brief Insert the substring of the supplied string of max length @c length2 starting from @c pos2 at @c pos1.
  ///
  /// @param pos1 The insertion position
  /// @param str The string holding the substring to insert
  /// @param pos2 The position of the start of the substring to insert. If @c >=str.size(), this is a no-op.
  /// @param length2 The maximum length of the substring to insert
  /// @return inline_string& @c *this
  /// @pre @c pos1<=size() else @c ARENE_PRECONDITION violation.
  /// @pre @c pos1+min(length2,str.size()-pos2)<max_size() , else @c ARENE_PRECONDITION violation.
  /// @post @c size() is increased by @c min(length2,str.size()-pos2) .
  /// @post The range @c [str.begin()+pos2,str.begin()+min(length2,str.size()-pos2)) is inserted at @c pos1
  constexpr auto insert(size_type pos1, string_view str, size_type pos2, size_type length2) noexcept -> inline_string& {
    return insert(pos1, str.substr(pos2, length2));
  }

  /// @brief Insert the string of @c count characters starting at @c str at @c pos
  ///
  /// @param pos The insertion position
  /// @param str The start of the string to insert
  /// @param count The length of the string to insert
  /// @return inline_string& @c *this
  /// @pre @c pos<=size() else @c ARENE_PRECONDITION violation.
  /// @pre @c pos+count<max_size() , else @c ARENE_PRECONDITION violation.
  /// @pre @c str+count must be in the valid range of @c str else behavior is undefined.
  /// @post @c size() is increased by @c count .
  /// @post @c str is inserted at @c pos
  constexpr auto insert(size_type pos, detail::raw_c_string str, size_type count) noexcept -> inline_string& {
    return insert(pos, string_view{str, count});
  }

  /// @brief Insert @c count copies of the specified character starting at position @c pos
  ///
  /// @param pos The insertion position
  /// @param count The number of times to insert the character
  /// @param chr The character to insert
  /// @return inline_string& @c *this
  /// @pre @c pos<=size() else @c ARENE_PRECONDITION violation.
  /// @pre @c pos+count<max_size() , else @c ARENE_PRECONDITION violation.
  /// @post @c size() is increased by @c count .
  /// @post @c count copies of @c chr are inserted beginning at @c pos
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  constexpr auto insert(size_type pos, size_type count, detail::character chr) noexcept -> inline_string& {
    auto const insert_begin = prepare_insert(pos, count);
    ::arene::base::fill(insert_begin, insert_begin + static_cast<difference_type>(count), chr);
    return *this;
  }

  /// @brief Insert the specified character starting at position @c pos
  ///
  /// @param pos The insertion position
  /// @param chr The character to insert
  /// @return iterator An iterator referring to the inserted character
  /// @pre @c pos must be a valid iterator into @c *this else behavior is undefined.
  /// @pre @c size()+1<max_size() , else @c ARENE_PRECONDITION violation.
  /// @post @c size() is increased by @c 1 .
  /// @post @c chr is inserted at @c pos
  constexpr auto insert(const_iterator pos, detail::character chr) noexcept -> iterator { return insert(pos, 1U, chr); }

  /// @brief Insert @c count copies of a character into the string at the specified position.
  ///
  /// @param pos An iterator representing the position to insert
  /// @param count The number of times to insert the character
  /// @param chr The character to insert
  /// @return iterator An iterator referring to the start of the sequence of inserted characters.
  /// @pre @c pos must be a valid iterator into @c *this else behavior is undefined.
  /// @pre @c pos+count<max_size() , else @c ARENE_PRECONDITION violation.
  /// @post @c size() is increased by @c count .
  /// @post @c count copies of @c chr are inserted beginning at @c pos
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  constexpr auto insert(const_iterator pos, size_type count, detail::character chr) noexcept -> iterator {
    auto const offset = pos - cbegin();
    std::ignore = insert(static_cast<size_type>(offset), count, chr);
    return begin() + offset;
  }

  /// @brief Insert a series of characters into the string at the specified position.
  ///
  /// @param pos An iterator representing the position to insert
  /// @param chars The characters to insert
  /// @return iterator An iterator referring to the start of the sequence of inserted characters.
  /// @pre @c pos must be a valid iterator into @c *this else behavior is undefined.
  /// @pre @c pos+chars.size()<max_size() , else @c ARENE_PRECONDITION violation.
  /// @post @c size() is increased by @c chars.size() .
  /// @post @c chars are inserted beginning at @c pos
  constexpr auto insert(const_iterator pos, std::initializer_list<detail::character> chars) noexcept -> iterator {
    auto const offset = pos - cbegin();
    std::ignore = insert(static_cast<size_type>(offset), string_view{chars.begin(), chars.size()});
    return begin() + offset;
  }

  /// @brief Insert a series of characters from an iterator range into the string at the specified position.
  ///
  /// @tparam InputIterator The type of the input iterator.
  /// @param pos An iterator representing the position to insert
  /// @param first The start of the input range
  /// @param last The end of the input range
  /// @return iterator An iterator referring to the start of the sequence of inserted characters.
  /// @pre @c size()+std::distance(first,last)<=max_size() else @c ARENE_PRECONDITION violation.
  /// @pre @c pos must be a valid iterator into @c *this
  /// @pre @c InputIterator must be an input iterator
  /// @pre @c first and @c last must form a valid iterator range
  template <typename InputIterator, constraints<std::enable_if_t<base::is_input_iterator_v<InputIterator>>> = nullptr>
  constexpr auto insert(
      const_iterator pos,
      InputIterator first,
      InputIterator last
  ) noexcept(denotes_nothrow_iterable_range_v<InputIterator>) -> iterator {
    return do_insert(typename std::iterator_traits<InputIterator>::iterator_category{}, pos, first, last);
  }

  /// @brief Erase all the characters in the string.
  /// @return inline_string& @c *this
  constexpr auto erase() noexcept -> inline_string& {
    clear();
    return *this;
  }

  /// @brief Erase characters from the string starting at the specified position to the end of the string
  /// @param pos The position of the first character to remove. If @c >=size() is a noop.
  /// @return inline_string& @c *this
  /// @post If @c pos<size() , @c size() is reduced by the difference between the original @c size() and @c pos .
  constexpr auto erase(size_type pos) -> inline_string& {
    // This can wrap around if pos > size(), but in that case the body of 2-arg erase will be skipped.
    return erase(pos, this->size() - pos);
  }

  /// @brief Erase the specified number of characters in the string starting at the specified position.
  ///
  /// @param pos The position of the characters to remove. If @c pos>=size() this is a noop.
  /// @param count The maximum number of characters to remove.
  /// @return inline_string& @c *this
  /// @post If @c pos<size() , then @c size() is reduced by @c min(count,size()-pos) .
  /// @post If @c pos<size() , then the characters in the range @c [pos,min(size(),count+size())) are removed, and the
  ///       characters in the range [min(size(),count+size()),size()) are moved to be located starting at @c pos .
  constexpr auto erase(size_type pos, size_type count) noexcept -> inline_string& {
    if (pos < this->size()) {
      count = std::min(count, this->size() - pos);
      // From here, pos < size(), count <= size(), and pos + count <= size()
      size_type const erase_end{pos + count};
      std::ignore = ::arene::base::copy(
          begin() + static_cast<difference_type>(erase_end),
          end(),
          begin() + static_cast<difference_type>(pos)
      );
      set_null_terminator_to(this->size() - count);
    }

    return *this;
  }

  /// @brief Erase the character at the specified range.
  /// @pre @c pos must be a valid iterator into the current string
  /// @param pos The iterator referring to the character to erase
  /// @return An iterator referring to the character after the removed range,
  /// or @c end() if there is no such character
  constexpr auto erase(const_iterator pos) noexcept -> iterator {
    auto const offset = pos - cbegin();
    ARENE_PRECONDITION((offset >= 0) && (static_cast<size_type>(offset) <= this->size()));
    std::ignore = erase(static_cast<size_type>(offset), 1U);
    return begin() + offset;
  }

  /// @brief Erase the characters in the specified range.
  /// @pre @c first and @c last must form a valid iterator range into the
  /// current string
  /// @param first The start of the range
  /// @param last The end of the range
  /// @return An iterator referring to the character after the removed range,
  /// or @c end() if there is no such character
  constexpr auto erase(const_iterator first, const_iterator last) noexcept -> iterator {
    ARENE_PRECONDITION(last >= first);
    auto const offset = first - cbegin();
    ARENE_PRECONDITION(offset >= 0);
    std::ignore = erase(static_cast<size_type>(offset), static_cast<size_type>(last - first));
    return begin() + offset;
  }

  /// @brief Replace a substring with another string.
  ///
  /// @param pos The start of the substring to replace.
  /// @param count The length of the substring to replace
  /// @param str The string to replace the substring with
  /// @return inline_string& @c *this
  /// @post The characters in the range @c [pos,std::min(pos+count,size())) are replaced by the characters in @c str .
  /// @post If @c count<str.size() , then the remaining characters from @c str are inserted at the position equivalent
  ///       to @c pos+count .
  /// @post If @c count<str.size(), then @c size() will increase by the difference.
  /// @pre @c pos<=size() else @c ARENE_PRECONDITION violation.
  /// @pre If @c count<str.size(), then @c size()+(str.size()-count)<=max_size() else @c ARENE_PRECONDITION violation.
  constexpr auto replace(size_type pos, size_type count, string_view str) noexcept -> inline_string& {
    prepare_replace(pos, count, str.length());
    std::ignore = ::arene::base::copy(str.begin(), str.end(), begin() + static_cast<difference_type>(pos));
    return *this;
  }

  /// @brief Replace a substring with another substring.
  ///
  /// @param pos The start of the substring to replace.
  /// @param count The length of the substring to replace
  /// @param str The string to replace the substring with
  /// @param pos2 The offset into @c str to start the replacement string. If @c pos2>str.size() , this is a no-op.
  /// @return inline_string& @c *this
  /// @post The characters in the range @c [pos,std::min(pos+count,size())) are replaced by the characters in the range
  ///       @c [pos2,str.size()) .
  /// @post If @c count<(str.size()-pos2) , then the remaining characters from @c str are inserted at the position
  ///       equivalent to @c pos+count .
  /// @post If @c count<(str.size()-pos2) , then @c size() will increase by the difference.
  /// @pre @c pos<=size() else @c ARENE_PRECONDITION violation.
  /// @pre If @c count<(str.size()-pos2), then @c size()+(str.size()-pos2-count)<=max_size() else @c ARENE_PRECONDITION
  ///      violation.
  constexpr auto replace(size_type pos, size_type count, string_view str, size_type pos2) noexcept -> inline_string& {
    return replace(pos, count, str.substr(pos2));
  }

  /// @brief Replace a substring with another substring.
  ///
  /// @param pos The start of the substring to replace.
  /// @param count The length of the substring to replace
  /// @param str The string to replace the substring with
  /// @param pos2 The offset into @c str to start the replacement string. If @c pos2>str.size() , this is a no-op.
  /// @param count2 The count into @c str to cap the replacement.
  /// @return inline_string& @c *this
  /// @post The characters in the range @c [pos,std::min(pos+count,size())) are replaced by the characters in the range
  ///       @c [pos2,std::min(count2,str.size())) .
  /// @post If @c count<(std::min(count2,str.size())-pos2) , then the remaining characters from @c str are inserted at
  ///       the position equivalent to @c pos+count .
  /// @post If @c count<(std::min(count2,str.size())-pos2) , then @c size() will increase by the difference.
  /// @pre @c pos<=size() else @c ARENE_PRECONDITION violation.
  /// @pre If @c count<(std::min(count2,str.size())-pos2), then @c size()+(str.size()-pos2-count)<=max_size() else
  ///      @c ARENE_PRECONDITION violation.
  constexpr auto replace(size_type pos, size_type count, string_view str, size_type pos2, size_type count2) noexcept
      -> inline_string& {
    return replace(pos, count, str.substr(pos2, count2));
  }

  /// @brief Replace a substring with a c-string.
  ///
  /// @param pos The start of the substring to replace.
  /// @param count The length of the substring to replace
  /// @param str The string to replace the substring with
  /// @param count2 The count into @c str to cap the replacement.
  /// @return inline_string& @c *this
  /// @post The characters in the range @c [pos,std::min(pos+count,size())) are replaced by the characters in the range
  ///       @c [str,str+count) .
  /// @post If @c count<count2 , then the remaining characters from @c str are inserted at
  ///       the position equivalent to @c pos+count .
  /// @post If @c count<count2 , then @c size() will increase by the difference.
  /// @pre @c pos<=size() else @c ARENE_PRECONDITION violation.
  /// @pre @c str+count must be in the valid range of @c str else behavior is undefined.
  /// @pre If @c count<count2, then @c size()+(count2-count)<=max_size() else @c ARENE_PRECONDITION violation.
  constexpr auto replace(size_type pos, size_type count, detail::raw_c_string str, size_type count2) noexcept
      -> inline_string& {
    return replace(pos, count, string_view{str, count2});
  }

  /// @brief Replace a substring with repeated copies of a given character.
  ///
  /// @param pos The start of the substring to replace.
  /// @param count The length of the substring to replace
  /// @param count2 The number of characters to fill.
  /// @param chr The character to fill with.
  /// @return inline_string& @c *this
  /// @post The characters in the range @c [pos,std::min(pos+count,size())) are replaced by @c count2 copies of chr
  /// @post If @c count<count2 , then the remaining copies of @c chr are inserted at the position equivalent
  ///       to @c pos+count , and @c size() will increase by the difference
  /// @post If @c count>count2 , then the remaining characters in the replacement range will be erased, and @c size()
  ///       will decrease by the difference.
  /// @pre @c pos<=size() else @c ARENE_PRECONDITION violation.
  /// @pre If @c count<count2, then @c size()+(count2-count)<=max_size() else @c ARENE_PRECONDITION violation.
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  constexpr auto replace(size_type pos, size_type count, size_type count2, detail::character chr) noexcept
      -> inline_string& {
    prepare_replace(pos, count, count2);
    auto const fill_begin = begin() + static_cast<difference_type>(pos);
    ::arene::base::fill(fill_begin, fill_begin + static_cast<difference_type>(count2), chr);
    return *this;
  }

  /// @brief Replace a substring with another string.
  ///
  /// @param first The start of the substring to replace.
  /// @param last The end of the substring to replace
  /// @param str The string to replace the substring with
  /// @return inline_string& @c *this
  /// @post The characters in the range @c [first,last) are replaced by the characters in @c str .
  /// @post If @c std::distance(first,last)<str.size() , then the remaining characters from @c str are inserted
  ///       at @c last , and  @c size() will increase by the difference.
  /// @post If @c std::distance(first,last)>str.size(), then the remain characters from @c [first,last) are erased and
  ///       @c size() will decrease by the difference.
  /// @pre @c pos<=size() else @c ARENE_PRECONDITION violation.
  /// @pre Let @c dist=std::distance(first,last) ; if @c dist<str.size(), then
  ///      @c size()+(str.size()-dist)<=max_size() else @c ARENE_PRECONDITION violation.
  constexpr auto replace(const_iterator first, const_iterator last, string_view str) noexcept -> inline_string& {
    ARENE_PRECONDITION(last >= first);
    auto const offset = first - cbegin();
    ARENE_PRECONDITION(offset >= 0);
    return replace(static_cast<size_type>(offset), static_cast<size_type>(last - first), str);
  }

  /// @brief Replace a substring with another substring.
  ///
  /// @param first The start of the substring to replace.
  /// @param last The end of the substring to replace
  /// @param str The string to replace the substring with
  /// @param pos The offset into @c str to start the replacement string. If @c pos>str.size() , this is a no-op.
  /// @return inline_string& @c *this
  /// @post The characters in the range @c [first,last) are replaced by the characters in the
  ///       range @c [str.begin()+pos,str.end()) .
  /// @post If @c std::distance(first,last)<(str.size()-pos) , then the remaining characters from @c str are inserted
  ///       at @c last , and  @c size() will increase by the difference.
  /// @post If @c std::distance(first,last)>(str.size()-pos), then the remain characters from @c [first,last) are erased
  ///       and @c size() will decrease by the difference.
  /// @pre @c pos<=size() else @c ARENE_PRECONDITION violation.
  /// @pre Let @c dist=std::distance(first,last) ; if @c dist<(str.size()-pos), then
  ///      @c size()+(str.size()-pos-dist)<=max_size() else @c ARENE_PRECONDITION violation.
  constexpr auto replace(const_iterator first, const_iterator last, string_view str, size_type pos) noexcept
      -> inline_string& {
    return replace(first, last, str.substr(pos));
  }

  /// @brief Replace a substring with another substring.
  ///
  /// @param first The start of the substring to replace.
  /// @param last The end of the substring to replace
  /// @param str The string to replace the substring with
  /// @param pos The offset into @c str to start the replacement string. If @c pos>str.size() , this is a no-op.
  /// @param count The count into @c str to cap the replacement.
  /// @return inline_string& @c *this
  /// @post The characters in the range @c [first,last) are replaced by the characters in the
  ///       range @c [str.begin()+pos,str.begin()+std::min(str.size(),count)) .
  /// @post If @c std::distance(first,last)<std::min(str.size()-pos,count) , then the remaining characters from @c str
  ///       are inserted at @c last , and  @c size() will increase by the difference.
  /// @post If @c std::distance(first,last)>std::min(str.size()-pos,count), then the remain characters from
  ///       @c [first,last) are erased and @c size() will decrease by the difference.
  /// @pre @c pos<=size() else @c ARENE_PRECONDITION violation.
  /// @pre Let @c dist=std::distance(first,last) and @c length=std::min(str.size()-pos,count) ;
  ///      if @c dist<length, then @c size()+(length-dist)<=max_size() else @c ARENE_PRECONDITION violation.
  constexpr auto
  replace(const_iterator first, const_iterator last, string_view str, size_type pos, size_type count) noexcept
      -> inline_string& {
    return replace(first, last, str.substr(pos, count));
  }

  /// @brief Replace a substring with a c-string.
  ///
  /// @param first The start of the substring to replace.
  /// @param last The end of the substring to replace
  /// @param str The string to replace the substring with
  /// @param count The count into @c str to cap the replacement.
  /// @return inline_string& @c *this
  /// @post The characters in the range @c [first,last) are replaced by the characters in the
  ///       range @c [str+pos,str+count) .
  /// @post If @c std::distance(first,last)<count , then the remaining characters from @c str
  ///       are inserted at @c last , and  @c size() will increase by the difference.
  /// @post If @c std::distance(first,last)>count, then the remain characters from
  ///       @c [first,last) are erased and @c size() will decrease by the difference.
  /// @pre @c pos<=size() else @c ARENE_PRECONDITION violation.
  /// @pre @c str+count must be in the valid range of @c str else behavior is undefined.
  /// @pre Let @c dist=std::distance(first,last) ; if @c dist<count, then @c size()+(count-dist)<=max_size() else
  ///      @c ARENE_PRECONDITION violation.
  constexpr auto replace(const_iterator first, const_iterator last, detail::raw_c_string str, size_type count) noexcept
      -> inline_string& {
    return replace(first, last, string_view{str, count});
  }

  /// @brief Replace a substring with repeated copies of a character.
  ///
  /// @param first The start of the substring to replace.
  /// @param last The end of the substring to replace
  /// @param count The count into @c str to cap the replacement.
  /// @param chr The character to fill with.
  /// @return inline_string& @c *this
  /// @post The characters in the range @c [first,last) are replaced by @c count repeated copies of @c chr
  /// @post If @c std::distance(first,last)<count , then the remaining characters are inserted at @c last , and
  ///       @c size() will increase by the difference.
  /// @post If @c std::distance(first,last)>count, then the remain characters from @c [first,last) are erased and
  ///       @c size() will decrease by the difference.
  /// @pre @c pos<=size() else @c ARENE_PRECONDITION violation.
  /// @pre Let @c dist=std::distance(first,last) ; if @c dist<count, then @c size()+(count-dist)<=max_size() else
  ///      @c ARENE_PRECONDITION violation.
  constexpr auto replace(const_iterator first, const_iterator last, size_type count, detail::character chr) noexcept
      -> inline_string& {
    ARENE_PRECONDITION(last >= first);
    auto const offset = first - cbegin();
    ARENE_PRECONDITION(offset >= 0);
    return replace(static_cast<size_type>(offset), static_cast<size_type>(last - first), count, chr);
  }

  /// @brief Replace a substring with a sequence of characters.
  ///
  /// @param first The start of the substring to replace.
  /// @param last The end of the substring to replace
  /// @param chars the sequence of characters to replace
  /// @return inline_string& @c *this
  /// @post The characters in the range @c [first,last) are replaced by the characters from @c chars
  /// @post If @c std::distance(first,last)<chars.size() , then the remaining characters from @c chars
  ///       are inserted at @c last , and  @c size() will increase by the difference.
  /// @post If @c std::distance(first,last)>chars.size(), then the remain characters from
  ///       @c [first,last) are erased and @c size() will decrease by the difference.
  /// @pre @c pos<=size() else @c ARENE_PRECONDITION violation.
  /// @pre Let @c dist=std::distance(first,last) ; if @c dist<chars.size(), then @c size()+(str.size()-dist)<=max_size()
  ///      else @c ARENE_PRECONDITION violation.
  constexpr auto
  replace(const_iterator first, const_iterator last, std::initializer_list<detail::character> chars) noexcept
      -> inline_string& {
    return replace(first, last, string_view{chars.begin(), chars.size()});
  }

  /// @brief Replace a substring with another substring.
  ///
  /// @tparam InputIterator The iterator type
  /// @param first The start of the substring to replace.
  /// @param last The end of the substring to replace
  /// @param first2 The start of the substring to replace the substring with
  /// @param last2 The end of the substring to replace the substring with.
  /// @return inline_string& @c *this
  /// @post The characters in the range @c [first,last) are replaced by the characters in the range @c [first2,last2) .
  /// @post If @c std::distance(first,last)<std::distance(first2,last2) , then the remaining characters from
  ///       @c [first2,last2] are inserted at @c last , and  @c size() will increase by the difference.
  /// @post If @c std::distance(first,last)>std::distance(first2,last2), then the remain characters from
  ///       @c [first,last) are erased and @c size() will decrease by the difference.
  /// @pre @c pos<=size() else @c ARENE_PRECONDITION violation.
  /// @pre Let @c dist=std::distance(first,last) and @c length=std::distance(first,last) ;
  ///      if @c dist<length, then @c size()+(length-dist)<=max_size() else @c ARENE_PRECONDITION violation.
  template <
      typename InputIterator,
      constraints<
          std::enable_if_t<base::is_input_iterator_v<InputIterator>>,
          std::enable_if_t<!base::is_random_access_iterator_v<InputIterator>>> = nullptr>
  constexpr auto replace(
      const_iterator first,
      const_iterator last,
      InputIterator first2,
      InputIterator last2
  ) noexcept(denotes_nothrow_iterable_range_v<InputIterator>) -> inline_string& {
    return replace(first, last, inline_string{first2, last2});
  }
  /// @brief Replace a substring with another substring.
  ///
  /// @tparam RandomAccessIterator The iterator type
  /// @param first The start of the substring to replace.
  /// @param last The end of the substring to replace
  /// @param first2 The start of the substring to replace the substring with
  /// @param last2 The end of the substring to replace the substring with.
  /// @return inline_string& @c *this
  /// @post The characters in the range @c [first,last) are replaced by the characters in the range @c [first2,last2) .
  /// @post If @c std::distance(first,last)<std::distance(first2,last2) , then the remaining characters from
  ///       @c [first2,last2] are inserted at @c last , and  @c size() will increase by the difference.
  /// @post If @c std::distance(first,last)>std::distance(first2,last2), then the remain characters from
  ///       @c [first,last) are erased and @c size() will decrease by the difference.
  /// @pre @c pos<=size() else @c ARENE_PRECONDITION violation.
  /// @pre Let @c dist=std::distance(first,last) and @c length=std::distance(first,last) ;
  ///      if @c dist<length, then @c size()+(length-dist)<=max_size() else @c ARENE_PRECONDITION violation.
  template <
      typename RandomAccessIterator,
      constraints<std::enable_if_t<base::is_random_access_iterator_v<RandomAccessIterator>>> = nullptr>
  constexpr auto replace(
      const_iterator first,
      const_iterator last,
      RandomAccessIterator first2,
      RandomAccessIterator last2
  ) noexcept(denotes_nothrow_iterable_range_v<RandomAccessIterator>) -> inline_string& {
    auto const length_of_replacement = last2 - first2;
    ARENE_PRECONDITION(length_of_replacement >= 0);
    ARENE_PRECONDITION(last >= first);
    auto const offset = first - cbegin();
    ARENE_PRECONDITION(offset >= 0);
    inline_string::size_type pos{static_cast<inline_string::size_type>(offset)};
    prepare_replace(
        pos,
        static_cast<inline_string::size_type>(last - first),
        static_cast<inline_string::size_type>(length_of_replacement)
    );
    std::ignore = ::arene::base::copy(first2, last2, begin() + offset);
    return *this;
  }

  /// @brief Remove the last character.
  /// @post @c size() is reduced by 1.
  /// @pre The string must not be empty, else @c ARENE_PRECONDITION violation.
  constexpr void pop_back() noexcept {
    ARENE_PRECONDITION(!empty());
    std::ignore = erase(this->size() - 1U);
  }

  /// @brief Copy the string into the specified buffer up to the specified maximum length.
  ///
  /// @param str A pointer to the target buffer
  /// @param count The maximum number of characters to copy
  /// @return The number of characters copied
  /// @pre @c str+n must be valid , else behavior is undefined.
  constexpr auto copy(detail::character* str, size_type count) const -> size_type {
    return string_view{*this}.copy(str, count);
  }

  /// @brief Copy the substring starting at the specified position into the specified buffer up to the specified maximum
  /// length.
  ///
  /// @param str A pointer to the target buffer
  /// @param count The maximum number of characters to copy
  /// @param pos The starting position to copy from. If @c >size() , no characters are copied.
  /// @return The number of characters copied
  /// @pre @c str[min(count,size()-pos)] must be valid , else behavior is undefined.
  constexpr auto copy(detail::character* str, size_type count, size_type pos) const -> size_type {
    return string_view{*this}.copy(str, count, pos);
  }

  /// @brief Produce a copy of the string.
  /// @return inline_string A copy of @c *this
  ARENE_NODISCARD constexpr auto substr() const noexcept -> inline_string { return *this; }

  /// @brief Produce substring of the string starting at an offset.
  ///
  /// @param pos The start of the substring to return.
  /// @return inline_string A new string holding the characters in the range @c [begin()+pos,end()) . If @c pos>=size()
  ///          , returns an empty string.
  ARENE_NODISCARD constexpr auto substr(size_type pos) const noexcept -> inline_string {
    return inline_string{string_view{*this}.substr(pos)};
  }

  /// @brief Produce a substring of the string with an offset and length.
  ///
  /// @param pos The start of the substring to return
  /// @param n The maximum number of characters to return
  /// @return inline_string A new string holding the characters in the range
  ///          @c [begin()+pos,begin()+std::min(size()-pos,count)) . If @c pos>=size() , returns an empty string.
  ARENE_NODISCARD constexpr auto substr(size_type pos, size_type n) const noexcept -> inline_string {
    return inline_string{string_view{*this}.substr(pos, n)};
  }

  /// @brief Check if the string is empty
  ///
  /// @return true If @c size()==0 .
  /// @return false If @c size()!=0 .
  ARENE_NODISCARD constexpr auto empty() const noexcept -> bool { return this->size() == 0U; }

  /// @brief Return a pointer to the NUL-terminated string held in @c *this
  ///
  /// @return a pointer to the NUL-terminated string held in @c *this
  ARENE_NODISCARD constexpr auto c_str() const noexcept -> detail::raw_c_string { return data(); }

  /// @brief Return a pointer to the NUL-terminated string held in @c *this
  ///
  /// @return a pointer to the NUL-terminated string held in @c *this
  ARENE_NODISCARD constexpr auto data() const noexcept -> detail::raw_c_string { return buffer_.data(); }

  /// @brief Return a pointer to the NUL-terminated string held in @c *this
  ///
  /// @return a pointer to the NUL-terminated string held in @c *this
  ARENE_NODISCARD constexpr auto data() noexcept -> detail::character* { return buffer_.data(); }

  /// @brief The maximum length of the string
  /// @return size_type @c MaxSize
  static constexpr auto capacity() -> std::size_t { return MaxSize; }

  /// @brief The maximum length of the string
  /// @return size_type @c MaxSize
  static constexpr auto max_size() -> std::size_t { return capacity(); }

  /// @brief Clear the string, so it is empty.
  /// @post size()==0 .
  /// @post @c data()[0]=='\0' .
  constexpr void clear() noexcept { set_null_terminator_to(0U); }

  /// @brief Resize the string. If the new size is larger than the old size, appending copies of @c new_char to make the
  /// string the required length
  ///
  /// @param new_size The new size of the string
  /// @param new_char The character to append if extending the string. Defaults to @c '\0' .
  /// @pre @c new_size<=max_size() else @c ARENE_PRECONDITION violation.
  // Argument order matches the order in @c std::string::resize
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  constexpr void resize(size_type new_size, detail::character new_char = '\0') noexcept {
    ARENE_PRECONDITION(new_size <= max_size());
    inline_string::iterator original_end{end()};
    set_null_terminator_to(new_size);
    ::arene::base::fill(std::min(original_end, end()), end(), new_char);
  }

  // parasoft-begin-suppress AUTOSAR-A2_10_1-a "False positive: 'index' does not hide anything"
  /// @brief Retrieve the @c index -th character in the string.
  ///
  /// @param index The index of the character to retrieve
  /// @return A reference to that character
  /// @pre @c index<=size() else @c ARENE_PRECONDITION violation.
  ARENE_NODISCARD constexpr auto operator[](size_type index) const noexcept -> detail::character const& {
    ARENE_PRECONDITION(index <= this->size());
    return buffer_[index];
  }

  /// @brief Retrieve the @c index -th character in the string.
  ///
  /// @param index The index of the character to retrieve
  /// @return A reference to that character
  /// @pre @c index<=size() else @c ARENE_PRECONDITION violation.
  /// @warning Setting @c index[size()] to anything other than @c 0 is undefined behaviour.
  ARENE_NODISCARD constexpr auto operator[](size_type index) noexcept -> detail::character& {
    ARENE_PRECONDITION(index <= this->size());
    return buffer_[index];
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-a

  /// @brief Get the first character in the string
  ///
  /// @return A reference to the first character
  /// @pre @c size()>0 , else @c ARENE_PRECONDITION violation.
  ARENE_NODISCARD constexpr auto front() const noexcept -> detail::character const& {
    ARENE_PRECONDITION(!empty());
    return buffer_.front();
  }

  /// @brief Get the first character in the string
  ///
  /// @return A reference to the first character
  /// @pre @c size()>0 , else @c ARENE_PRECONDITION violation.
  ARENE_NODISCARD constexpr auto front() noexcept -> detail::character& {
    ARENE_PRECONDITION(!empty());
    return buffer_.front();
  }

  /// @brief Get the last character in the string
  ///
  /// @return A reference to the last character
  /// @pre @c size()>0 , else @c ARENE_PRECONDITION violation.
  ARENE_NODISCARD constexpr auto back() const noexcept -> detail::character const& {
    ARENE_PRECONDITION(!empty());
    return buffer_[this->size() - 1U];
  }

  /// @brief Get the last character in the string
  ///
  /// @return A reference to the last character
  /// @pre @c size()>0 , else @c ARENE_PRECONDITION violation.
  ARENE_NODISCARD constexpr auto back() noexcept -> detail::character& {
    ARENE_PRECONDITION(!empty());
    return buffer_[this->size() - 1U];
  }

  /// @brief Iterator to the first position in the string.
  ///
  /// @return iterator An iterator to that refers to the first element if the string is not empty, or @c end()
  ///          otherwise
  ARENE_NODISCARD constexpr auto begin() noexcept -> iterator {
    return iterator(iterator_passkey{}, buffer_.begin().base());
  }

  /// @brief Iterator to the first position in the string.
  ///
  /// @return const_iterator An iterator to that refers to the first element if the string is not empty, or @c end()
  ///          otherwise
  ARENE_NODISCARD constexpr auto begin() const noexcept -> const_iterator {
    return const_iterator(iterator_passkey{}, buffer_.begin().base());
  }
  /// @brief Iterator to the first position in the string.
  ///
  /// @return const_iterator An iterator to that refers to the first element if the string is not empty, or @c end()
  ///          otherwise
  ARENE_NODISCARD constexpr auto cbegin() const noexcept -> const_iterator { return begin(); }

  /// @brief The one-past-the-last position in the string.
  ///
  /// @return iterator An iterator to that refers to one-past-the-last position in the string.
  ARENE_NODISCARD constexpr auto end() noexcept -> iterator {
    return begin() + static_cast<difference_type>(this->size());
  }

  /// @brief Return an iterator to one-past-the-last position in the string.
  ///
  /// @return const_iterator An iterator to that refers to one-past-the-last position in the string.
  ARENE_NODISCARD constexpr auto end() const noexcept -> const_iterator {
    return begin() + static_cast<difference_type>(this->size());
  }
  /// @brief Return an iterator to one-past-the-last position in the string.
  ///
  /// @return const_iterator An iterator to that refers to one-past-the-last position in the string.
  ARENE_NODISCARD constexpr auto cend() const noexcept -> const_iterator { return end(); }

  /// @brief Return an iterator to the start of the reversed string
  ///
  /// @return reverse_iterator An iterator equivalent to @c reverse_iterator(end())
  ARENE_NODISCARD constexpr auto rbegin() noexcept -> reverse_iterator { return reverse_iterator(end()); }

  /// @brief Return an iterator to the start of the reversed string
  ///
  /// @return const_reverse_iterator An iterator equivalent to @c const_reverse_iterator(end())
  ARENE_NODISCARD auto rbegin() const noexcept -> const_reverse_iterator { return const_reverse_iterator(end()); }

  /// @brief Return an iterator to the start of the reversed string
  ///
  /// @return const_reverse_iterator An iterator equivalent to @c const_reverse_iterator(end())
  ARENE_NODISCARD auto crbegin() const noexcept -> const_reverse_iterator { return rbegin(); }

  /// @brief Return an iterator to the end of the reversed string
  ///
  /// @return reverse_iterator An iterator equivalent to @c reverse_iterator(begin())
  ARENE_NODISCARD constexpr auto rend() noexcept -> reverse_iterator { return reverse_iterator(begin()); }

  /// @brief Return an iterator to the end of the reversed string
  ///
  /// @return const_reverse_iterator An iterator equivalent to @c const_reverse_iterator(begin())
  ARENE_NODISCARD auto rend() const noexcept -> const_reverse_iterator { return const_reverse_iterator(begin()); }

  /// @brief Return an iterator to the end of the reversed string
  ///
  /// @return const_reverse_iterator An iterator equivalent to @c const_reverse_iterator(begin())
  ARENE_NODISCARD auto crend() const noexcept -> const_reverse_iterator { return rend(); }

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "These functions are called with qualified names so they can't be hidden"
  /// @brief Compare two strings for lexicographical ordering.
  ///
  /// @tparam OtherMaxLength The max length of the other string
  /// @param lhs The first string
  /// @param rhs The second string
  /// @return strong_ordering::equal If both strings have the same @c size() and equivalent characters.
  /// @return strong_ordering::less If @c lhs is lexicographically before @c rhs .
  /// @return strong_ordering::greater If @c lhs is lexicographically after @c rhs .
  template <std::size_t OtherMaxLength>
  ARENE_NODISCARD static constexpr auto
  three_way_compare(inline_string const& lhs, inline_string<OtherMaxLength> const& rhs) noexcept -> strong_ordering {
    return string_view::three_way_compare(string_view{lhs}, string_view{rhs});
  }

  /// @brief Compare two strings for lexicographical ordering.
  ///
  /// @param lhs The first string
  /// @param rhs The second string
  /// @return strong_ordering::equal If both strings have the same @c size() and equivalent characters.
  /// @return strong_ordering::less If @c lhs is lexicographically before @c rhs .
  /// @return strong_ordering::greater If @c lhs is lexicographically after @c rhs .
  ARENE_NODISCARD static constexpr auto three_way_compare(inline_string const& lhs, string_view rhs) noexcept
      -> strong_ordering {
    return string_view::three_way_compare(string_view{lhs}, rhs);
  }

  /// @brief Compare two strings for lexicographical ordering.
  ///
  /// @param lhs The first string
  /// @param rhs The second string
  /// @return strong_ordering::equal If both strings have the same @c size() and equivalent characters.
  /// @return strong_ordering::less If @c lhs is lexicographically before @c rhs .
  /// @return strong_ordering::greater If @c lhs is lexicographically after @c rhs .
  ARENE_NODISCARD static constexpr auto three_way_compare(inline_string const& lhs, detail::raw_c_string rhs) noexcept
      -> strong_ordering {
    return string_view::three_way_compare(string_view{lhs}, rhs);
  }

  /// @brief Compare two strings for lexicographical ordering.
  ///
  /// @param lhs The first string
  /// @param rhs The second string
  /// @return strong_ordering::equal If both strings have the same @c size() and equivalent characters.
  /// @return strong_ordering::less If @c lhs is lexicographically before @c rhs .
  /// @return strong_ordering::greater If @c lhs is lexicographically after @c rhs .
  ARENE_NODISCARD static constexpr auto
  three_way_compare(inline_string const& lhs, null_terminated_string_view rhs) noexcept -> strong_ordering {
    return inline_string::three_way_compare(lhs, rhs.c_str());
  }

  /// @brief Determine if two strings are definitely not equal.
  ///
  /// @tparam OtherMaxLength The max length of the second string
  /// @param lhs The first string
  /// @param rhs The second string
  /// @return inequality_heuristic::definitely_not_equal If @c lhs.size()!=rhs.size() .
  /// @return inequality_heuristic::may_be_equal_or_not_equal If @c lhs.size()==rhs.size() .
  template <std::size_t OtherMaxLength>
  ARENE_NODISCARD static constexpr auto
  fast_inequality_check(inline_string const& lhs, inline_string<OtherMaxLength> const& rhs) noexcept
      -> inequality_heuristic {
    return inline_string::fast_inequality_check(lhs, string_view{rhs});
  }

  /// @brief Determine if two strings are definitely not equal.
  ///
  /// @param lhs The first string
  /// @param rhs The second string
  /// @return inequality_heuristic::definitely_not_equal If @c lhs.size()!=rhs.size() .
  /// @return inequality_heuristic::may_be_equal_or_not_equal If @c lhs.size()==rhs.size() .
  ARENE_NODISCARD static constexpr auto fast_inequality_check(inline_string const& lhs, string_view rhs) noexcept
      -> inequality_heuristic {
    return inline_string_base::common_fast_inequality_check(lhs, rhs);
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  /// @brief Append a string to the current string
  ///
  /// @param rhs The string to append
  /// @return inline_string& @c *this
  /// @pre @c size()+rhs.size()<=max_size() else @c ARENE_PRECONDITION violation.
  /// @post @c size() is increased by @c rhs.size()
  /// @post The final characters in the string will be a substring equivalent to @c rhs .
  constexpr auto append(string_view rhs) noexcept -> inline_string& {
    ARENE_PRECONDITION(this->size() + rhs.size() <= max_size());
    std::ignore = ::arene::base::copy(rhs.begin(), rhs.end(), begin() + static_cast<difference_type>(this->size()));
    set_null_terminator_to(this->size() + rhs.size());
    return *this;
  }

  /// @brief Append a character to the current string
  ///
  /// @param rhs The character to append
  /// @return inline_string& @c *this
  /// @pre @c size()+1<=max_size() else @c ARENE_PRECONDITION violation.
  /// @post @c size() is increased by @c 1
  /// @post @c back()==rhs
  constexpr auto append(detail::character rhs) noexcept -> inline_string& { return append(string_view{&rhs, 1U}); }

  /// @brief Append a series of characters from an initializer list to the string.
  ///
  /// @param rhs The characters to append
  /// @return inline_string& @c *this
  /// @pre @c size()+rhs.size()<=max_size() else @c ARENE_PRECONDITION violation.
  /// @post @c size() is increased by @c rhs.size()
  /// @post The final characters in the string will be a substring equivalent to @c rhs .
  constexpr auto append(std::initializer_list<detail::character> rhs) noexcept -> inline_string& {
    return append(string_view{rhs.begin(), rhs.size()});
  }

  /// @brief Append a substring of a string to the current string
  ///
  /// @param rhs The string containing the substring to append
  /// @param pos The offset into @c rhs of the substring to append
  /// @return inline_string& @c *this
  /// @pre @c size()+rhs.substr(pos).size()<=max_size() else @c ARENE_PRECONDITION violation.
  /// @post @c size() is increased by @c rhs.substr(pos).size()
  /// @post The final characters in the string will be a substring equivalent to @c rhs.substr(pos) .
  constexpr auto append(string_view rhs, size_type pos) noexcept -> inline_string& { return append(rhs.substr(pos)); }

  /// @brief Append a substring of a string to the current string
  ///
  /// @param rhs The string containing the substring to append
  /// @param pos The offset into @c rhs of the substring to append
  /// @param count The maximum length of the substring to append
  /// @return inline_string& @c *this
  /// @pre @c size()+rhs.substr(pos,count).size()<=max_size() else @c ARENE_PRECONDITION violation.
  /// @post @c size() is increased by @c rhs.substr(pos,count).size()
  /// @post The final characters in the string will be a substring equivalent to @c rhs.substr(pos,count) .
  constexpr auto append(string_view rhs, size_type pos, size_type count) noexcept -> inline_string& {
    return append(rhs.substr(pos, count));
  }

  /// @brief Append a string specified as pointer and length to the current string
  ///
  /// @param rhs The start if the string to append
  /// @param count The number of characters in the string to append
  /// @return inline_string& @c *this
  /// @pre @c size()+count<=max_size() else @c ARENE_PRECONDITION violation.
  /// @post @c size() is increased by @c count
  /// @post The last @c count characters in the string are the @c count characters from @c rhs .
  constexpr auto append(detail::raw_c_string rhs, size_type count) noexcept -> inline_string& {
    return append(string_view{rhs, count});
  }

  /// @brief Append the specified number of copies of a character to the string
  ///
  /// @param count The number of characters to append
  /// @param chr The character to append
  /// @return inline_string& @c *this
  /// @pre @c size()+count<=max_size() else @c ARENE_PRECONDITION violation.
  /// @post @c size() is increased by @c count
  /// @post The last @c count characters in the string are @c chr .
  // Argument order matches the order in @c std::string::append
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  constexpr auto append(size_type count, detail::character chr) noexcept -> inline_string& {
    resize(this->size() + count, chr);
    return *this;
  }

  /// @brief Append a character to the current string
  ///
  /// @param rhs The character to append
  /// @pre @c size()+1<=max_size() else @c ARENE_PRECONDITION violation.
  /// @post @c size() is increased by @c 1
  /// @post @c back()==rhs
  constexpr void push_back(detail::character rhs) noexcept { std::ignore = append(rhs); }

  /// @brief Append a string to the current string
  ///
  /// @param rhs The string to append
  /// @return inline_string& @c *this
  /// @pre @c size()+rhs.size()<=max_size() else @c ARENE_PRECONDITION violation.
  /// @post @c size() is increased by @c rhs.size()
  /// @post The final characters in the string will be a substring equivalent to @c rhs .
  constexpr auto operator+=(string_view rhs) noexcept -> inline_string& { return append(rhs); }

  /// @brief Append a character to the current string
  ///
  /// @param rhs The character to append
  /// @return inline_string& @c *this
  /// @pre @c size()+1<=max_size() else @c ARENE_PRECONDITION violation.
  /// @post @c size() is increased by @c 1
  /// @post @c back()==rhs
  constexpr auto operator+=(detail::character rhs) noexcept -> inline_string& { return append(rhs); }

  /// @brief Append a series of characters from an initializer list to the string.
  ///
  /// @param rhs The characters to append
  /// @return inline_string& @c *this
  /// @pre @c size()+rhs.size()<=max_size() else @c ARENE_PRECONDITION violation.
  /// @post @c size() is increased by @c rhs.size()
  /// @post The final characters in the string will be a substring equivalent to @c rhs .
  constexpr auto operator+=(std::initializer_list<detail::character> rhs) noexcept -> inline_string& {
    return append(rhs);
  }

  // parasoft-begin-suppress AUTOSAR-A13_5_2-a-2 "Implicit conversion to string_view is part of the API"
  /// @brief Convert to string view
  ///
  /// Implicit conversion to @c string_view supports the use of @c string_view as a vocabulary type for accepting string
  /// parameters.
  /// @return A string view referring to the characters of @c *this
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  ARENE_NODISCARD constexpr operator string_view() const noexcept {
    // We specify MaxLength as a maximum valid value to eliminate the
    // precondition in base::span constructor.
    return {c_str(), std::min(this->size(), max_size())};
  }
  // parasoft-end-suppress AUTOSAR-A13_5_2-a-2

  // parasoft-begin-suppress AUTOSAR-A13_5_2-a-2 "Implicit conversion to null_terminated_string_view is part of the API"
  /// @brief Convert to null terminated string view
  ///
  /// Implicit conversion to @c null_terminated_string_view supports the use of @c null_terminated_string_view as a
  /// vocabulary type for accepting string parameters.
  /// @return A null-terminated string view referring to the characters of @c *this
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
  ARENE_NODISCARD constexpr operator null_terminated_string_view() const noexcept {
    return null_terminated_string_view{c_str()};
  }
  // parasoft-end-suppress AUTOSAR-A13_5_2-a-2

  /// @brief Find the earliest position of the given substring within the string, starting at the specified position.
  ///
  /// @param str The substring to search for.
  /// @param pos The lowest position to search from. Defaults to 0.
  /// @return size_type The offset at which @c str can be found within the string, or @c npos if not found.
  ARENE_NODISCARD constexpr auto find(string_view str, size_type pos = 0U) const noexcept -> size_type {
    return string_view{*this}.find(str, pos);
  }

  /// @brief Find the earliest position of the specified character within the string.
  ///
  /// @param chr The character to search for
  /// @return The position of that character, or @c npos if not found
  ARENE_NODISCARD constexpr auto find(detail::character chr) const noexcept -> size_type {
    return find(string_view{&chr, 1U});
  }

  /// @brief Find the earliest position of the specified character within the string, starting at the specified position
  ///
  /// @param chr The character to search for
  /// @param pos The lowest position to search from
  /// @return The position of that character, or @c npos if not found
  ARENE_NODISCARD constexpr auto find(detail::character chr, size_type pos) const noexcept -> size_type {
    return find(string_view{&chr, 1U}, pos);
  }

  /// @brief Find the earliest position of the given substring within the string.
  ///
  /// @param str The substring to search for
  /// @return size_type The offset at which @c str can be found within the string, or @c npos if not found.
  ARENE_NODISCARD constexpr auto find(detail::raw_c_string str) const noexcept -> size_type {
    return find(string_view{str});
  }

  /// @brief Find the earliest position of the given substring within the string, starting at the specified position
  ///
  /// @param str The substring to search for
  /// @param pos The lowest position to search from
  /// @return size_type The offset at which @c str can be found within the string, or @c npos if not found.
  ARENE_NODISCARD constexpr auto find(detail::raw_c_string str, size_type pos) const noexcept -> size_type {
    return find(string_view{str}, pos);
  }

  /// @brief Find the earliest position of the @c n character string starting at @c str within the string, starting at
  /// the specified position
  ///
  /// @param str The start of the string to search for
  /// @param pos The lowest position to search from
  /// @param count The length of the string to search for
  /// @return size_type The offset at which @c str can be found within the string, or @c npos if not found.
  ARENE_NODISCARD constexpr auto find(detail::raw_c_string str, size_type pos, size_type count) const noexcept
      -> size_type {
    return find(string_view{str, count}, pos);
  }

  /// @brief Find the latest position of the given substring within the string.
  ///
  /// @param str The substring to search for
  /// @return size_type The offset at which @c str can be found within the string, or @c npos if not found.
  ARENE_NODISCARD constexpr auto rfind(string_view str) const noexcept -> size_type { return rfind(str, this->size()); }

  /// @brief Find the latest position of the given substring within the string, starting at the specified position
  ///
  /// @param str The substring to search for
  /// @param pos The highest position to search from
  /// @return size_type The offset at which @c str can be found within the string, or @c npos if not found.
  ARENE_NODISCARD constexpr auto rfind(string_view str, size_type pos) const noexcept -> size_type {
    return string_view{*this}.rfind(str, pos);
  }

  /// @brief Find the latest position of the specified character within the string
  ///
  /// @param chr The character to search for
  /// @return The position of that character, or @c npos if not found
  ARENE_NODISCARD constexpr auto rfind(detail::character chr) const noexcept -> size_type {
    return rfind(string_view{&chr, 1U});
  }

  /// @brief Find the latest position of the specified character within the string, starting at the specified position.
  ///
  /// @param chr The character to search for
  /// @param pos The highest position to search from
  /// @return The position of that character, or @c npos if not found
  ARENE_NODISCARD constexpr auto rfind(detail::character chr, size_type pos) const noexcept -> size_type {
    return rfind(string_view{&chr, 1U}, pos);
  }

  /// @brief Find the latest position of the given substring within the string.
  ///
  /// @param str The substring to search for
  /// @return size_type The offset at which @c str can be found within the string, or @c npos if not found.
  ARENE_NODISCARD constexpr auto rfind(detail::raw_c_string str) const noexcept -> size_type {
    return rfind(string_view{str});
  }

  /// @brief Find the latest position of the given substring within the string, starting at the specified position
  ///
  /// @param str The substring to search for
  /// @param pos The highest position to search from
  /// @return size_type The offset at which @c str can be found within the string, or @c npos if not found.
  ARENE_NODISCARD constexpr auto rfind(detail::raw_c_string str, size_type pos) const noexcept -> size_type {
    return rfind(string_view{str}, pos);
  }

  /// @brief Find the latest position of the @c n character string starting at @c str within the string, starting at the
  /// specified position
  ///
  /// @param str The start of the string to search for
  /// @param pos The highest position to search from
  /// @param count The length of the string to search for
  /// @return size_type The offset at which @c str can be found within the string, or @c npos if not found.
  ARENE_NODISCARD constexpr auto rfind(detail::raw_c_string str, size_type pos, size_type count) const noexcept
      -> size_type {
    return rfind(string_view{str, count}, pos);
  }

  /// @brief Find the earliest position such that the character at that position is one of those in the provided string.
  ///
  /// @param str The list of characters to find
  /// @return The offset of the first character in @c str if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_first_of(string_view str) const noexcept -> size_type {
    return find_first_of(str, 0U);
  }

  /// @brief Find the earliest position from the provided start position onwards such that the character at that
  /// position is one of those in the provided string.
  ///
  /// @param str The list of characters to find
  /// @param pos The start position
  /// @return The offset of the first character in @c str after @c pos if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_first_of(string_view str, size_type pos) const noexcept -> size_type {
    return string_view{*this}.find_first_of(str, pos);
  }

  /// @brief Find the earliest position such that the character at that position is one of those in the provided string.
  ///
  /// @param str The list of characters to find
  /// @return The offset of the first character in @c str if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_first_of(detail::raw_c_string str) const noexcept -> size_type {
    return find_first_of(string_view{str}, 0U);
  }

  /// @brief Find the earliest position from the provided start position onwards such that the character at that
  /// position is one of those in the provided string.
  ///
  /// @param str The list of characters to find
  /// @param pos The start position
  /// @return The offset of the first character in @c str after @c pos if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_first_of(detail::raw_c_string str, size_type pos) const noexcept -> size_type {
    return find_first_of(string_view{str}, pos);
  }

  /// @brief Find the earliest position from the provided start position onwards such that the character at that
  /// position is one of those in the provided string.
  ///
  /// @param str The list of characters to find
  /// @param pos The start position
  /// @param count The number of characters in @c str
  /// @return The offset of the first character in @c str after @c pos if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_first_of(detail::raw_c_string str, size_type pos, size_type count) const noexcept
      -> size_type {
    return find_first_of(string_view{str, count}, pos);
  }

  /// @brief Find the earliest position such that the character at that position is the provided character.
  ///
  /// @param chr The character to find
  /// @return The offset of the first character matching @c chr if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_first_of(detail::character chr) const noexcept -> size_type { return find(chr); }

  /// @brief Find the earliest position from the provided start position onwards such that the character at that
  /// position is the provided character.
  ///
  /// @param chr The character to find
  /// @param pos The start position
  /// @return The offset of the first character matching @c chr after @c pos if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_first_of(detail::character chr, size_type pos) const noexcept -> size_type {
    return find(chr, pos);
  }

  /// @brief Find the earliest position such that the character at that position is not one of those in the provided
  /// string.
  ///
  /// @param str The list of characters to find
  /// @return The offset of the first character not in @c str if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_first_not_of(string_view str) const noexcept -> size_type {
    return find_first_not_of(str, 0U);
  }

  /// @brief Find the earliest position from the provided start position onwards such that the character at that
  /// position is one of those in the provided string.
  ///
  /// @param str The list of characters to find
  /// @param pos The start position
  /// @return The offset of the first character not in @c str after @c pos if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_first_not_of(string_view str, size_type pos) const noexcept -> size_type {
    return string_view{*this}.find_first_not_of(str, pos);
  }

  /// @brief Find the earliest position such that the character at that position is not one of those in the provided
  /// string.
  ///
  /// @param str The list of characters to find
  /// @return The offset of the first character not in @c str if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_first_not_of(detail::raw_c_string str) const noexcept -> size_type {
    return find_first_not_of(string_view{str}, 0U);
  }

  /// @brief Find the earliest position from the provided start position onwards such that the character at that
  /// position is not one of those in the provided string.
  ///
  /// @param str The list of characters to find
  /// @param pos The start position
  /// @return The offset of the first character not in @c str after @c pos if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_first_not_of(detail::raw_c_string str, size_type pos) const noexcept
      -> size_type {
    return find_first_not_of(string_view{str}, pos);
  }

  /// @brief Find the earliest position from the provided start position onwards such that the character at that
  /// position is not one of those in the provided string.
  ///
  /// @param str The list of characters to find
  /// @param pos The start position
  /// @param count The number of characters in @c str
  /// @return The offset of the first character not in @c str after @c pos if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_first_not_of(detail::raw_c_string str, size_type pos, size_type count)
      const noexcept -> size_type {
    return find_first_not_of(string_view{str, count}, pos);
  }

  /// @brief Find the earliest position such that the character at that position is the provided character.
  ///
  /// @param chr The character to find
  /// @return The offset of the first character not matching @c chr if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_first_not_of(detail::character chr) const noexcept -> size_type {
    return find_first_not_of(string_view{&chr, 1U});
  }

  /// @brief Find the earliest position from the provided start position onwards such that the character at that
  /// position is not the provided character.
  ///
  /// @param chr The character to find
  /// @param pos The start position
  /// @return The offset of the first character not matching @c chr after @c pos if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_first_not_of(detail::character chr, size_type pos) const noexcept -> size_type {
    return find_first_not_of(string_view{&chr, 1U}, pos);
  }

  /// @brief Find the last position such that the character at that position is one of those in the provided string.
  ///
  /// @param str The list of characters to find
  /// @return The offset of the last character matching those in @c str if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_last_of(string_view str) const noexcept -> size_type {
    return find_last_of(str, npos);
  }

  /// @brief Find the last position from the provided start position backwards such that the character at that position
  /// is one of those in the provided string.
  ///
  /// @param str The list of characters to find
  /// @param pos The start position
  /// @return The offset of the last character matching those in @c str before or at @c pos if there is one, otherwise
  /// @c npos
  ARENE_NODISCARD constexpr auto find_last_of(string_view str, size_type pos) const noexcept -> size_type {
    return string_view{*this}.find_last_of(str, pos);
  }

  /// @brief Find the last position such that the character at that position is one of those in the provided string.
  ///
  /// @param str The list of characters to find
  /// @return The offset of the last character in @c str if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_last_of(detail::raw_c_string str) const noexcept -> size_type {
    return find_last_of(string_view{str}, npos);
  }

  /// @brief Find the last position from the provided start position backwards such that the character at that position
  /// is one of those in the provided string.
  ///
  /// @param str The list of characters to find
  /// @param pos The start position
  /// @return The offset of the last character matching the characters in @c str at or before @c pos if there is one,
  /// otherwise @c npos
  ARENE_NODISCARD constexpr auto find_last_of(detail::raw_c_string str, size_type pos) const noexcept -> size_type {
    return find_last_of(string_view{str}, pos);
  }

  /// @brief Find the latest position from the provided start position backwards such that the character at that
  /// position is one of those in the provided string.
  ///
  /// @param str The list of characters to find
  /// @param pos The start position
  /// @param count The number of characters in @c str
  /// @return The offset of the last character matching the characters in the first @c n characters of @c str at or
  /// before @c pos if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_last_of(detail::raw_c_string str, size_type pos, size_type count) const noexcept
      -> size_type {
    return find_last_of(string_view{str, count}, pos);
  }

  /// @brief Find the latest position such that the character at that position is the provided character.
  ///
  /// @param chr The character to find
  /// @return The offset of the last character matching @c chr if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_last_of(detail::character chr) const noexcept -> size_type { return rfind(chr); }

  /// @brief Find the latest position from the provided start position backwards such that the character at that
  /// position is the provided character.
  ///
  /// @param chr The character to find
  /// @param pos The start position
  /// @return The offset of the last character matching @c chr at or before @c pos if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_last_of(detail::character chr, size_type pos) const noexcept -> size_type {
    return rfind(chr, pos);
  }

  /// @brief Find the latest position such that the character at that position is not one of those in the provided
  /// string.
  ///
  /// @param str The list of characters to find
  /// @return The offset of the last character not in @c str if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_last_not_of(string_view str) const noexcept -> size_type {
    return find_last_not_of(str, npos);
  }

  /// @brief Find the latest position from the provided start position backwards such that the character at that
  /// position is not one of those in the provided string.
  ///
  /// @param str The list of characters to find
  /// @param pos The start position
  /// @return The offset of the last character not in @c str at or before @c pos if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_last_not_of(string_view str, size_type pos) const noexcept -> size_type {
    return string_view{*this}.find_last_not_of(str, pos);
  }

  /// @brief Find the latest position such that the character at that position is not one of those in the provided
  /// string.
  ///
  /// @param str The list of characters to find
  /// @return The offset of the last character not in @c str if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_last_not_of(detail::raw_c_string str) const noexcept -> size_type {
    return find_last_not_of(string_view{str});
  }

  /// @brief Find the latest position from the provided start position backwards such that the character at that
  /// position is not one of those in the provided string.
  ///
  /// @param str The list of characters to find
  /// @param pos The start position
  /// @return The offset of the last character not in @c str at or before @c pos if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_last_not_of(detail::raw_c_string str, size_type pos) const noexcept -> size_type {
    return find_last_not_of(string_view{str}, pos);
  }

  /// @brief Find the latest position from the provided start position backwards such that the character at that
  /// position is not one of those in the provided string.
  ///
  /// @param str The list of characters to find
  /// @param pos The start position
  /// @param count The number of characters in @c str
  /// @return The offset of the last character not in @c str at or before @c pos if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_last_not_of(detail::raw_c_string str, size_type pos, size_type count)
      const noexcept -> size_type {
    return find_last_not_of(string_view{str, count}, pos);
  }

  /// @brief Find the latest position such that the character at that position is not the provided character.
  ///
  /// @param chr The character to find
  /// @return The offset of the last character not matching @c chr if there is one, otherwise @c npos
  ARENE_NODISCARD constexpr auto find_last_not_of(detail::character chr) const noexcept -> size_type {
    return find_last_not_of(string_view{&chr, 1U});
  }

  /// @brief Find the latest position from the provided start position backwards such that the character at that
  /// position is not the provided character.
  ///
  /// @param chr The character to find
  /// @param pos The start position
  /// @return The offset of the last character not matching @c chr at or before @c pos if there is one, otherwise
  /// @c npos
  ARENE_NODISCARD constexpr auto find_last_not_of(detail::character chr, size_type pos) const noexcept -> size_type {
    return find_last_not_of(string_view{&chr, 1U}, pos);
  }

  /// @brief Check if the string starts with a character.
  ///
  /// @param chr The character to check
  /// @return inline_string @c true if the string starts with the character, @c false otherwise
  ARENE_NODISCARD constexpr auto starts_with(detail::character chr) const noexcept -> bool {
    return string_view{*this}.starts_with(chr);
  }

  /// @brief Check if the string starts with another string.
  ///
  /// @param str The string to check
  /// @return inline_string @c true if the string starts with the string, @c false otherwise
  ARENE_NODISCARD constexpr auto starts_with(string_view str) const noexcept -> bool {
    return string_view{*this}.starts_with(str);
  }

  /// @brief Check if the string ends with a character.
  ///
  /// @param chr The character to check
  /// @return inline_string @c true if the string ends with the character, @c false otherwise
  ARENE_NODISCARD constexpr auto ends_with(detail::character chr) const noexcept -> bool {
    return string_view{*this}.ends_with(chr);
  }

  /// @brief Check if the string ends with another string.
  ///
  /// @param str The string to check
  /// @return inline_string @c true if the string ends with the string, @c false otherwise
  ARENE_NODISCARD constexpr auto ends_with(string_view str) const noexcept -> bool {
    return string_view{*this}.ends_with(str);
  }

  /// @brief Compare against another string, to see if it is less than, equal to or greater than the other.
  ///
  /// @param other The other string
  /// @return inline_string @c -1 if @c *this is less than @c other, @c 0 if they are equal, and @c 1 if @c *this is
  /// greater than @c other
  ARENE_NODISCARD constexpr auto compare(string_view other) const noexcept -> integral_comparison_result {
    return string_view{*this}.compare(other);
  }

  /// @brief Compare a substring against another string, to see if it is less than, equal to or greater than the other.
  ///
  /// @param pos The start of the substring to compare
  /// @param count The length of the substring to compare
  /// @param other The other string
  /// @return inline_string @c substr(pos,n).compare(other)
  ARENE_NODISCARD constexpr auto compare(size_type pos, size_type count, string_view other) const noexcept
      -> integral_comparison_result {
    return string_view{*this}.substr(pos, count).compare(other);
  }

  /// @brief Compare a substring against a substring of another string, to see if it is less than, equal to or greater
  /// than the other.
  ///
  /// @param pos The start of the substring to compare
  /// @param count The length of the substring to compare
  /// @param other The other string
  /// @param pos2 The start of the substring in the other string
  /// @return inline_string @c compare(pos,n,other.substr(pos2))
  ARENE_NODISCARD constexpr auto compare(size_type pos, size_type count, string_view other, size_type pos2)
      const noexcept -> integral_comparison_result {
    return compare(pos, count, other.substr(pos2));
  }

  /// @brief Compare a substring against a substring of another string, to see if it is less than, equal to or greater
  /// than the other.
  ///
  /// @param pos The start of the substring to compare
  /// @param length1 The length of the substring to compare
  /// @param other The other string
  /// @param pos2 The start of the substring in the other string
  /// @param length2 The length of the substring in the other string
  /// @return inline_string @c compare(pos,n,other.substr(pos2,length2))
  ARENE_NODISCARD constexpr auto
  compare(size_type pos, size_type length1, string_view other, size_type pos2, size_type length2) const noexcept
      -> integral_comparison_result {
    return compare(pos, length1, other.substr(pos2, length2));
  }

  /// @brief Compare a substring against another string, to see if it is less than, equal to or greater than the other.
  ///
  /// @param pos The start of the substring to compare
  /// @param length1 The length of the substring to compare
  /// @param other The start of the other string
  /// @param length2 The length of the other string
  /// @return inline_string @c compare(pos,n,string_view{other,length2})
  ARENE_NODISCARD constexpr auto
  compare(size_type pos, size_type length1, detail::raw_c_string other, size_type length2) const noexcept
      -> integral_comparison_result {
    return compare(pos, length1, string_view{other, length2});
  }

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  /// @brief Concatenate two strings
  /// @tparam OtherMaxLength the max length of @c rhs
  /// @param lhs The first string to concatenate
  /// @param rhs The second string to concatenate
  /// @return A @c inline_string<MaxLength+OtherMaxLength> holding the concatenation of the two strings.
  template <std::size_t OtherMaxLength>
  ARENE_NODISCARD friend constexpr auto
  operator+(inline_string const& lhs, inline_string<OtherMaxLength> const& rhs) noexcept
      -> inline_string<MaxSize + OtherMaxLength> {
    inline_string<MaxSize + OtherMaxLength> res{lhs};
    std::ignore = res.append(rhs);
    return res;
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: This is a namespace scope function"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  /// @brief Concatenate two strings
  ///
  /// @param lhs The first string to concatenate
  /// @param rhs The second string to concatenate
  /// @return inline_string holding the concatenation of the two strings.
  /// @pre @c lhs.size()+rhs.size()<=lhs.max_size() else @c ARENE_PRECONDITION violation.
  ARENE_NODISCARD friend constexpr auto operator+(inline_string const& lhs, string_view const rhs) noexcept
      -> inline_string {
    ARENE_PRECONDITION(lhs.size() + rhs.size() <= lhs.max_size());
    inline_string res{lhs};
    std::ignore = res.append(rhs);
    return res;
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2

  // parasoft-begin-suppress AUTOSAR-A0_1_3-a-2 "False positive: This is a namespace scope function"
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  /// @brief Concatenate two strings
  ///
  /// @param lhs The first string to concatenate
  /// @param rhs The second string to concatenate
  /// @return inline_string holding the concatenation of the two strings.
  /// @pre @c lhs.size()+rhs.size()<=rhs.max_size() else @c ARENE_PRECONDITION violation.
  ARENE_NODISCARD friend constexpr auto operator+(string_view const lhs, inline_string const& rhs) noexcept
      -> inline_string {
    ARENE_PRECONDITION(lhs.size() + rhs.size() <= rhs.max_size());
    inline_string res{lhs};
    std::ignore = res.append(rhs);
    return res;
  }
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
  // parasoft-end-suppress AUTOSAR-A0_1_3-a-2

 private:
  /// @brief Tag-dispatch implementation helpers for @c inline_string::try_construct(first,last) .
  ///
  /// @tparam Itr The type of the iterator.
  /// @param first The start of the sequence to construct from.
  /// @param last The end of the sequence to construct from.
  /// @return optional<inline_string> If @c std::distance(first,last)>MaxLength a null optional, else an optional
  ///         populated is if via @c inline_string{first,last} .
  template <typename Itr>
  static constexpr auto do_try_construct(std::random_access_iterator_tag, Itr first, Itr last) noexcept
      -> optional<inline_string> {
    if (::arene::base::distance(first, last) > static_cast<difference_type>(MaxSize)) {
      return nullopt;
    }
    return {inline_string{first, last}};
  }

  /// @brief Tag-dispatch implementation helpers for @c inline_string::try_construct(first,last) .
  ///
  /// @tparam Itr The type of the iterator.
  /// @param first The start of the sequence to construct from.
  /// @param last The end of the sequence to construct from.
  /// @return optional<inline_string> If @c std::distance(first,last)>MaxLength a null optional, else an optional
  ///         populated is if via @c inline_string{first,last} .
  template <typename Itr>
  static constexpr auto do_try_construct(std::input_iterator_tag, Itr first, Itr last) noexcept
      -> optional<inline_string> {
    inline_string str;
    while ((str.size() < str.max_size()) && (first != last)) {
      str.push_back(*(first++));
    }
    if (first != last) {
      return nullopt;
    }
    return str;
  }

  /// @brief Tag-dispatch implementation helpers for @c inline_string::insert(pos,first,last) .
  ///
  /// @tparam Itr The type of the iterator.
  /// @param pos The location to start the insert
  /// @param first The start of the sequence to insert.
  /// @param last The end of the sequence to insert.
  /// @return iterator An iterator to the beginning of the insertion.
  template <typename Itr>
  constexpr auto do_insert(std::input_iterator_tag, const_iterator pos, Itr first, Itr last) -> iterator {
    auto const offset = pos - cbegin();
    std::ignore = insert(static_cast<size_type>(offset), inline_string{first, last});
    return begin() + offset;
  }
  /// @brief Tag-dispatch implementation helpers for @c inline_string::insert(pos,first,last) .
  ///
  /// @tparam Itr The type of the iterator.
  /// @param pos The location to start the insert
  /// @param first The start of the sequence to insert.
  /// @param last The end of the sequence to insert.
  /// @return iterator An iterator to the beginning of the insertion.
  template <typename Itr>
  constexpr auto do_insert(std::random_access_iterator_tag, const_iterator pos, Itr first, Itr last) -> iterator {
    auto const offset = pos - cbegin();
    auto const distance = last - first;
    ARENE_PRECONDITION(distance >= 0);
    auto const insertion_location = prepare_insert(static_cast<size_type>(offset), static_cast<size_type>(distance));
    std::ignore = ::arene::base::copy(first, last, insertion_location);
    return insertion_location;
  }

  /// @brief Copy from the source span without bounds checking
  ///
  /// @param str the source data
  /// @post @c size()==str.size()
  /// @post The content of this string is equivalent to the content of @c str
  constexpr void copy_from_unchecked(string_view str) noexcept {
    std::ignore = arene::base::copy(str.begin(), str.end(), begin());
    set_null_terminator_to(str.size());
  }

  /// @brief Copy from the source span with bounds checking.
  ///
  /// @param str the source data
  /// @pre @c str.size() is less than or equal to @c MaxLength else @c ARENE_PRECONDITION violation
  /// @post @c size()==str.size()
  /// @post The content of this string is equivalent to the content of @c str
  constexpr void copy_from(string_view str) noexcept {
    ARENE_PRECONDITION(str.size() <= MaxSize);
    copy_from_unchecked(str);
  }

  /// @brief Copy from the source string
  ///
  /// @tparam SourceMaxLength The length of the source string. Must be less than or equal to @c MaxLength
  /// @param str the source data
  /// @post @c size()==str.size()
  /// @post The content of this string is equivalent to the content of @c str
  template <std::size_t SourceMaxLength, constraints<std::enable_if_t<SourceMaxLength <= MaxSize>> = nullptr>
  constexpr void copy_from(inline_string<SourceMaxLength> str) noexcept {
    copy_from_unchecked(string_view(str));
  }

  /// @brief Copy from the source string
  ///
  /// @tparam SourceMaxLength The length of the source string. Must be greater than @c MaxLength
  /// @param str the source data
  /// @pre @c str.size() is less than or equal to @c MaxLength else @c ARENE_PRECONDITION violation
  /// @post @c size()==str.size()
  /// @post The content of this string is equivalent to the content of @c str
  template <std::size_t SourceMaxLength, constraints<std::enable_if_t<(SourceMaxLength > MaxSize)>> = nullptr>
  constexpr void copy_from(inline_string<SourceMaxLength> str) noexcept {
    copy_from(string_view(str));
  }

  /// @brief Copy from the source NUL-terminated string
  ///
  /// @param str Pointer to the source string
  /// @pre @c str must not be a null pointer, else @c ARENE_PRECONDITION violation.
  /// @pre The length of @c str must be less than @c MaxLength , else @c ARENE_PRECONDITION violation.
  /// @pre @c str must point to a NUL-terminated string, else behavior is undefined.
  /// @post @c size() is the length of @c str
  /// @post The content of this string is equivalent to the content of @c str
  constexpr void copy_from(detail::raw_c_string str) noexcept {
    ARENE_PRECONDITION(str != nullptr);
    copy_from(string_view{str});
  }

  /// @brief Copy from a pair of iterators
  ///
  /// @tparam Itr The type of the iterator.
  /// @param first The start of the sequence to insert.
  /// @param last The end of the sequence to insert.
  /// @pre @c std::distance(first,last)<=MaxSize else @c ARENE_PRECONDITION violation
  /// @post @c size() is the length of @c str
  /// @post The content of this string is equivalent to the content of @c str
  template <typename Itr>
  constexpr void copy_from(std::input_iterator_tag, Itr first, Itr last) noexcept {
    for (auto const chr : make_subrange(first, last)) {
      push_back(chr);
    }
  }
  /// @brief Copy from a pair of iterators
  ///
  /// @tparam Itr The type of the iterator.
  /// @param first The start of the sequence to insert.
  /// @param last The end of the sequence to insert.
  /// @pre @c std::distance(first,last)<=MaxSize else @c ARENE_PRECONDITION violation
  /// @post @c size() is the length of @c str
  /// @post The content of this string is equivalent to the content of @c str
  template <typename Itr>
  constexpr void copy_from(std::random_access_iterator_tag, Itr first, Itr last) noexcept {
    auto const source_length = ::arene::base::distance(first, last);
    ARENE_PRECONDITION(source_length >= 0);
    auto const usource_length = static_cast<size_type>(source_length);
    ARENE_PRECONDITION(usource_length <= max_size());
    std::ignore = ::arene::base::copy(first, last, begin());
    set_null_terminator_to(usource_length);
  }

  /// @brief Prepare for an insert of the specified length at the specified position.
  ///
  /// Increases the length and moves the characters, so the @c insertion_length characters at @c pos can be
  /// overwritten.
  /// @param pos The position the insert will occur.
  /// @param insertion_length The length of the insertion.
  /// @return iterator An iterator to the location to perform the insertion.
  /// @pre @c pos<=size() else @c ARENE_PRECONDITION violation.
  /// @pre @c MaxLength-size()>=insertion_length else @c ARENE_PRECONDITION violation.
  /// @post @c size() is increased by @c insertion_length .
  /// @post The characters in the range @c [pos,insertion_length) are now located in the range
  ///       @c [insertion_length,size())
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  constexpr auto prepare_insert(size_type pos, size_type insertion_length) noexcept -> iterator {
    ARENE_PRECONDITION(pos <= this->size());
    ARENE_PRECONDITION(max_size() - this->size() >= insertion_length);
    std::ignore = ::arene::base::copy(
        rbegin(),
        rbegin() + static_cast<std::ptrdiff_t>(this->size() - pos),
        rbegin() - static_cast<std::ptrdiff_t>(insertion_length)
    );
    set_null_terminator_to(this->size() + insertion_length);
    return begin() + static_cast<difference_type>(pos);
  }

  /// @brief Prepare for a replacement of the specified length at the specified position with a new string of the
  /// specified replacement length.
  ///
  /// Increases or decreases the length and moves the characters, so the @c original_length characters at @c pos can
  /// be overwritten with @c replacement_length new characters.
  /// @param pos The position the insert will occur
  /// @param original_length The number of characters being replaced
  /// @param replacement_length The number of characters in the replacement
  /// @pre @c pos<=size() else @c ARENE_PRECONDITION violation.
  /// @pre @c size()-removal_length+replacement_length<=(MaxLength) else @c ARENE_PRECONDITION violation.
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  constexpr void prepare_replace(size_type pos, size_type original_length, size_type replacement_length) {
    ARENE_PRECONDITION(pos <= this->size());
    auto const removal_length = std::min(original_length, this->size() - pos);
    ARENE_PRECONDITION(this->size() - removal_length + replacement_length <= MaxSize);
    if (removal_length > replacement_length) {
      std::ignore = erase(pos + replacement_length, removal_length - replacement_length);
    } else {
      std::ignore = prepare_insert(pos + removal_length, replacement_length - removal_length);
    }
  }

  /// @brief Sets the null terminator to the desired index.
  ///
  /// @param pos The index into buffer to place the null terminator.
  /// @post @c size()==pos
  /// @post @c buffer_[size()]=='\0'
  constexpr void set_null_terminator_to(size_type pos) {
    this->set_size(pos);
    buffer_[this->size()] = '\0';
  }

  /// @brief The stored characters
  arene::base::array<detail::character, MaxSize + 1> buffer_{};
};
// parasoft-end-suppress AUTOSAR-A10_1_1-a-2

template <std::size_t MaxLength>
constexpr std::size_t inline_string<MaxLength>::npos;

namespace inline_string_detail {

/// @brief helper for @c is_inline_string_v
/// @tparam Type type to check
template <typename Type, typename = constraints<>>
extern constexpr bool is_inline_string_v = false;

/// @brief helper for @c is_inline_string_v
/// @tparam Capacity capacity of the inline string
template <std::size_t Capacity>
extern constexpr bool is_inline_string_v<inline_string<Capacity>> = true;

}  // namespace inline_string_detail

/// @brief Trait to query if a type is an @c inline_string.
/// @tparam Type The type to check.
///
/// @c true if @c Type is a cv-qualified instantiation of @c inline_string. Otherwise, @c false.
template <typename Type>
extern constexpr bool is_inline_string_v = inline_string_detail::is_inline_string_v<std::remove_cv_t<Type>>;

/// @brief Trait to query if a type is an @c inline_string.
/// @tparam Type The type to check.
///
/// Inherits from @c std::true_type if @c Type is an
/// instantiation of @c inline_string, else @c std::false_type.
template <typename Type>
class is_inline_string : public std::integral_constant<bool, is_inline_string_v<Type>> {};

/// @brief Trait to query if a type is an @c inline_string.
/// @tparam Type The type to check.
///
/// @c true if @c Type is a cv-qualified instantiation of @c inline_string. Otherwise, @c false.
template <typename Type>
extern constexpr bool is_inline_buffer_string_v = is_inline_string_v<Type>;

/// @brief Trait to query if a type is an @c inline_string.
/// @tparam Type The type to check.
///
/// Inherits from @c std::true_type if @c Type is an
/// instantiation of @c inline_string, else @c std::false_type.
template <typename Type>
using is_inline_buffer_string = is_inline_string<Type>;

namespace inline_string_detail {

/// @brief helper for constructing an @c inline_string
class make_inline_string_impl_fn {
 public:
  /// @brief Construct from a C-array of characters (e.g. a string literal)
  /// @tparam N the size of the source character array
  /// @param str The string to copy from
  /// @pre @c str must have a NUL terminator somewhere in it
  /// @post The content of this string compares equal to @c str
  /// @return @c str as an @c inline_string
  template <std::size_t N, constraints<std::enable_if_t<N != 0>> = nullptr>
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays) allows use of string literals
  constexpr auto operator()(detail::character const (&str)[N]) const noexcept -> inline_string<N - 1U> {
    return inline_string<N - 1U>{str};
  }
};

}  // namespace inline_string_detail

/// @def arene::base::make_inline_string
/// @brief helper for constructing an @c inline_string that deduces the capacity template parameter
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(inline_string_detail::make_inline_string_impl_fn, make_inline_string);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_INLINE_STRING_HPP_
