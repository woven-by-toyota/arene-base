// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_INTEGER_SEQUENCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_INTEGER_SEQUENCE_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <utility>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/conditional.hpp"
#include "stdlib/include/stdlib_detail/cstddef.hpp"
#include "stdlib/include/stdlib_detail/cstdint.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/is_integral.hpp"
#include "stdlib/include/stdlib_detail/is_signed.hpp"

namespace std {

/// @brief Implements a compile-time sequence of integers
/// @tparam Type an integer type to use for the sequence of elements
/// @tparam Ints a non-type parameter pack of elements defining the sequence
///
/// Represents a compile-time sequence of integers of type @c Type. When used as
/// an argument to a function template, the parameter pack defining the sequence
/// can be deduced and used in a pack expansion.
///
template <typename Type, Type... Ints>
class integer_sequence {
 public:
  static_assert(is_integral_v<Type>, "'Type' must be an integer.");

  /// @brief The sequence integer type
  using value_type = Type;

  /// @brief Returns the number of elements in the sequence
  /// @return @c sizeof...(Ints)
  static constexpr auto size() noexcept -> size_t { return sizeof...(Ints); }
};

namespace integer_sequence_detail {

/// @brief helper for quickly expanding the elements in a sequence
/// @tparam T integer type
/// @tparam DoDouble if true, double the number of elements in the sequence
///
/// Helper type used to reduce the number of template instantiations for a large
/// integer sequence.
///
template <class T, bool DoDouble>
class fast_expand_sequence;

/// @brief helper for constructing an integer sequence from @c 0 to <tt>N - 1</tt>
/// @tparam T integer type
/// @tparam N remaining number of sequence elements to generate
/// @tparam Is current sequence
///
/// Primary template for expanding the sequence of elements.
///
template <typename T, size_t N, T... Is>
class make_sequence_impl : public fast_expand_sequence<T, (N >= sizeof...(Is))>::template type<N, Is...> {};

/// @brief recursive base case specialization
/// @tparam T integer type
/// @tparam Is sequence of elements
///
/// If @c N is @c 0, construction of the sequence is complete
///
template <typename T>
class make_sequence_impl<T, 0> {
 public:
  /// @brief completed integer sequence type
  using type = integer_sequence<T>;
};

/// @brief recursive base case specialization
/// @tparam T integer type
/// @tparam Is sequence of elements
///
/// If @c N is @c 0, construction of the sequence is complete
///
template <typename T, T... Is>
class make_sequence_impl<T, 0, Is...> {
 public:
  /// @brief completed integer sequence type
  using type = integer_sequence<T, Is...>;
};

/// @brief initial make sequence specialization
/// @tparam T integer type
/// @tparam N remaining number of sequence elements to generate
///
/// If the sequence @c Is is empty, reduce remaining by 1 and create a sequence
/// with the zero element.
///
template <typename T, size_t N>
class make_sequence_impl<T, N> : public make_sequence_impl<T, N - 1, T{}> {};

/// @brief specialization if the sequence cannot be doubled
/// @tparam T integer type
/// @tparam DoDouble false for this primary template
///
template <class T, bool DoDouble>
class fast_expand_sequence {
 public:
  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"

  /// @brief finishes the sequence since it can no longer be doubled
  /// @tparam Is sequence head
  /// @tparam Js sequence tail
  /// @return helper type containing the completed sequence
  ///
  template <T... Is, T... Js>
  static constexpr auto finish_sequence(integer_sequence<T, Js...>)
      -> make_sequence_impl<T, 0, Is..., sizeof...(Is) + Js...>;

  // parasoft-end-suppress CERT_C-EXP37-a-3

  /// @brief finish the current sequence
  /// @tparam N remaining number of sequence elements to generate
  /// @tparam Is current sequence
  ///
  template <size_t N, T... Is>
  using type = decltype(finish_sequence<Is...>(typename make_sequence_impl<T, N>::type{}));
};

/// @brief specialization if the sequence can be doubled
/// @tparam T integer type
///
template <class T>
class fast_expand_sequence<T, true> {
 public:
  /// @brief double the current sequence
  /// @tparam N remaining number of sequence elements to generate
  /// @tparam Is current sequence
  ///
  template <size_t N, T... Is>
  using type = make_sequence_impl<T, N - sizeof...(Is), Is..., sizeof...(Is) + Is...>;
};

/// @brief Helper variable used to determine if an integer value is negative
/// @tparam Type integer type
/// @tparam Value integer value to check
///
/// The primary template is always @c false.
///
template <typename Type, Type Value, typename = void>
extern constexpr bool is_negative_v = false;

/// @brief Helper variable used to used to determine if an integer value is negative
/// @tparam Type integer type
/// @tparam Value integer value to check
///
/// Template specialization for signed integer types.
///
/// The value is defined to be <tt>int64_t{Value} < 0</tt>. An explicit
/// promotion to @c int64_t is used to avoid warnings of implicit conversions of
/// smaller types (e.g. @c bool) to @c int.
///
template <typename Type, Type Value>
extern constexpr bool is_negative_v<Type, Value, enable_if_t<is_signed_v<Type>>> = int64_t{Value} < 0;

/// @brief Helper type to check that the requested sequence size is non-negative
/// @tparam Type integer type
/// @tparam Size sequence size
///
/// The primary template is the successful case where the size is non-negative.
///
template <typename Type, Type Size, bool = is_negative_v<Type, Size>>
class make_integer_sequence : public make_sequence_impl<Type, Size> {};

/// @brief Helper type to check that the requested sequence size is non-negative
/// @tparam Type integer type
/// @tparam Size sequence size
///
/// Template specialization instantiated if @c Size is a negative value.
///
template <typename Type, Type Size>
class make_integer_sequence<Type, Size, true> {
 public:
  // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is
  // a compile-time assert and can have no side-effects"
  static_assert(!is_negative_v<Type, Size>, "'Size' must be non-negative.");
  // parasoft-end-suppress CERT_C-PRE31-c-3

  /// @brief an unused type alias provided to reduce compiler error messages
  ///
  using type = integer_sequence<Type, 0>;
};

/// @}

}  // namespace integer_sequence_detail

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: Declaration *is* preceeded by the @brief tag."
/// @brief Helper alias template to simplify creation of @c std::integer_sequence types
/// with <tt>0</tt>, <tt>1</tt>, ..., <tt>N - 1</tt> as @c Ints.
/// @tparam Type an integer type to use for the sequence of elements
/// @tparam Size size of the sequence
///
/// If @c N is negative the program is ill-formed. The alias template
/// @c make_integer_sequence denotes a specialization of @c integer_sequence
/// with @c N template non-type arguments. The type
/// @c make_integer_sequence<T, N> denotes the type
/// <tt>integer_sequence<T, 0, 1, ..., N-1></tt>.
template <typename Type, Type Size>
using make_integer_sequence = typename integer_sequence_detail::make_integer_sequence<Type, Size>::type;
// parasoft-end-suppress AUTOSAR-A2_7_3-a "False positive: Declaration *is* preceeded by the @brief tag."

/// @brief A helper alias template for <tt>integer_sequence<size_t, Ints></tt>
/// @tparam Ints a non-type parameter pack of elements defining the sequence
template <size_t... Ints>
using index_sequence = integer_sequence<size_t, Ints...>;

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: there is an @brief tag."
/// @brief A helper alias template for <tt>make_integer_sequence<size_t, Size></tt>
/// @tparam Size size of the sequence
template <size_t Size>
using make_index_sequence = make_integer_sequence<size_t, Size>;
// parasoft-end-suppress AUTOSAR-A2_7_3-a "False positive: there is an @brief tag."

/// @brief A helper alias template convert any type parameter pack into an index sequence of the same length
/// @tparam Types parameter pack of types
template <typename... Types>
using index_sequence_for = make_index_sequence<sizeof...(Types)>;

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_INTEGER_SEQUENCE_HPP_
