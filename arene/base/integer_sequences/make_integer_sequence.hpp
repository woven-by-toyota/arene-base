// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTEGER_SEQUENCES_MAKE_INTEGER_SEQUENCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTEGER_SEQUENCES_MAKE_INTEGER_SEQUENCE_HPP_

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"

namespace arene {
namespace base {

namespace make_integer_sequence_detail {

// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
/// @brief Helper for creating an integer_sequence starting from a particular value
/// @tparam Type the integer type
/// @tparam Begin the starting value of the sequence
/// @tparam Is the values to increment by
/// @return The integer sequence starting at @c Begin
template <typename Type, Type Begin, std::size_t... Is>
auto impl(std::index_sequence<Is...>) -> std::integer_sequence<Type, (Begin + Type{Is})...>;
// parasoft-end-suppress CERT_C-EXP37-a

/// @brief Helper for checking if the specified @c Begin and @c Size are valid for a given @c Type
/// @tparam Type the integer type
/// @tparam Begin the starting value of the sequence
/// @tparam Size The size of the desired sequence
template <typename Type, Type Begin, Type Size, bool = (Size > Type{})>
extern constexpr bool is_valid_range_v{Size == Type{}};

/// @brief Helper for checking if the specified @c Begin and @c Size are valid for a given @c Type
/// @tparam Type the integer type
/// @tparam Begin the starting value of the sequence
/// @tparam Size The size of the desired sequence
template <typename Type, Type Begin, Type Size>
extern constexpr bool is_valid_range_v<Type, Begin, Size, true>{
    Begin <= (std::numeric_limits<Type>::max() - (Size - 1))
};

/// @brief Implementation of make_integer_sequence_from
/// @tparam Type the integer type
/// @tparam Begin the starting value of the sequence
/// @tparam Size the size of the sequence
template <typename Type, Type Begin, Type Size, bool = is_valid_range_v<Type, Begin, Size>>
struct make_from_impl {
  static_assert(Size >= Type{}, "Size must be non-negative");

  /// @brief The maximum value of @c Type
  static constexpr auto max = std::numeric_limits<Type>::max();

  static_assert(
      (Size == 0) || (Begin <= (max - (Size - 1))),
      "Sequence with the given Begin and Size would overflow the representable range of Type"
  );

  /// @brief The resulting integer sequence type
  using type = decltype(make_integer_sequence_detail::impl<Type, Begin>(std::make_index_sequence<Size>{}));
};

/// @brief Implementation of make_integer_sequence_from (precondition not satisfied)
/// @tparam Type the integer type
/// @tparam Begin the starting value of the sequence
/// @tparam Size the size of the sequence
template <typename Type, Type Begin, Type Size>
struct make_from_impl<Type, Begin, Size, false> {
  static_assert(Size >= Type{}, "Size must be non-negative");

  /// @brief The maximum value of @c Type
  static constexpr auto max = std::numeric_limits<Type>::max();

  static_assert(
      (Size == 0) || (Begin <= (max - (Size - 1))),
      "Sequence with the given Begin and Size would overflow the representable range of Type"
  );
};

}  // namespace make_integer_sequence_detail

/// @brief Create an integer_sequence starting from a particular value
/// @tparam Type the integer type
/// @tparam Begin the starting value of the sequence
/// @tparam Size the size of the sequence
/// @pre Requires that the sequence does not overflow the maximum value representable by @c Type
/// @pre Requires <c> Size >= 0 </c>
template <typename Type, Type Begin, Type Size>
using make_integer_sequence_from = typename make_integer_sequence_detail::make_from_impl<Type, Begin, Size>::type;

namespace make_integer_sequence_detail {

/// @brief Implementation of make_integer_sequence_between
/// @tparam Type the integer type
/// @tparam Begin the starting value of the sequence
/// @tparam End the ending value of the sequence (not included)
template <typename Type, Type Begin, Type End, bool = Begin <= End>
struct make_between_impl {
  /// @brief The resulting integer sequence type
  using type = make_integer_sequence_from<Type, Begin, Type{End - Begin}>;
};

/// @brief Implementation of make_integer_sequence_between (precondition not satisfied)
/// @tparam Type the integer type
/// @tparam Begin the starting value of the sequence
/// @tparam End the ending value of the sequence (not included)
template <typename Type, Type Begin, Type End>
struct make_between_impl<Type, Begin, End, false> {
  static_assert(Begin <= End, "Precondition Begin <= End not satisfied");
};

}  // namespace make_integer_sequence_detail

/// @brief Create an integer_sequence between two values [Begin, End)
/// @tparam Type the integer type
/// @tparam Begin the starting value of the sequence
/// @tparam End the ending value of the sequence (not included)
/// @pre Requires that <tt>End >= Begin</tt>
template <typename Type, Type Begin, Type End>
using make_integer_sequence_between = typename make_integer_sequence_detail::make_between_impl<Type, Begin, End>::type;

/// @brief A helper alias template for <tt>make_integer_sequence_from<size_t, Begin, Size></tt>
/// @tparam Begin the starting value of the sequence
/// @tparam Size the size of the sequence
/// @pre Requires that the sequence does not overflow the maximum value representable by @c size_t
template <std::size_t Begin, std::size_t Size>
using make_index_sequence_from = make_integer_sequence_from<std::size_t, Begin, Size>;

/// @brief A helper alias template for <tt>make_integer_sequence_between<size_t, Begin, End></tt>
/// @tparam Begin the starting value of the sequence
/// @tparam End the ending value of the sequence (not included)
/// @pre Requires that <tt>End >= Begin</tt>
template <std::size_t Begin, std::size_t End>
using make_index_sequence_between = make_integer_sequence_between<std::size_t, Begin, End>;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTEGER_SEQUENCES_MAKE_INTEGER_SEQUENCE_HPP_
