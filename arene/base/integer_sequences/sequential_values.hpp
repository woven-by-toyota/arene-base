// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTEGER_SEQUENCES_SEQUENTIAL_VALUES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTEGER_SEQUENCES_SEQUENTIAL_VALUES_HPP_

// IWYU pragma: private, include "arene/base/integer_sequences.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/integer_sequences/make_integer_sequence.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"

// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace sequential_values_detail {

/// @brief The maximum number of elements supported by @c sequential_values and @c make_index_sequence_array
/// On certain platforms and compilers, there is a limit to the number of elements that can be expanded in an
/// initializer list. For the currently supported set of compilers, the lower bound of this is 2^16-1 in clang-17.
static constexpr std::size_t max_number_of_elements{65535U};

/// @brief Tests if a sequential values count is valid
/// @tparam NumElements the number of elements desired
/// @return true if <c> NumElements <= max_number_of_elements </c>
/// @return false otherwise
template <std::size_t NumElements>
extern constexpr bool is_valid_size_v = NumElements <= max_number_of_elements;

/// @brief Create an array holding the values in the provided index sequence.
/// @tparam ElementType The type of each element
/// @tparam Indices The values to put in the array
/// @return An array holding the values, The array bounds is the number of indices in the sequence.
template <
    typename ElementType,
    ElementType... Indices,
    constraints<std::enable_if_t<sequential_values_detail::is_valid_size_v<sizeof...(Indices)>>> = nullptr>
constexpr auto make_index_sequence_array(std::integer_sequence<ElementType, Indices...>)
    -> array<ElementType, sizeof...(Indices)> {
  // parasoft-begin-suppress AUTOSAR-M8_5_2-a-2 "False positive: correct initialization"
  return {Indices...};
  // parasoft-end-suppress AUTOSAR-M8_5_2-a-2
}

}  // namespace sequential_values_detail

/// @brief An array holding a sequence of integer values starting at Begin, incrementing by 1 each time.
/// @tparam ElementType The type of each element
/// @tparam Begin The first element in the sequence.
/// @tparam Count The number of elements in the sequence.
template <
    typename ElementType,
    ElementType Begin,
    std::size_t Count,
    constraints<std::enable_if_t<sequential_values_detail::is_valid_size_v<Count>>> = nullptr>
extern constexpr auto sequential_values_from = sequential_values_detail::make_index_sequence_array<ElementType>(
    make_integer_sequence_from<ElementType, Begin, static_cast<ElementType>(Count)>{}
);

/// @brief An array holding a sequence of integer values starting at Begin, ending at End and incrementing by 1 each
/// time.
/// @tparam ElementType The type of each element
/// @tparam Begin The first element in the sequence.
/// @tparam End The last element in the sequence.
template <
    typename ElementType,
    ElementType Begin,
    ElementType End,
    constraints<std::enable_if_t<sequential_values_detail::is_valid_size_v<std::size_t{End - Begin}>>> = nullptr>
extern constexpr auto sequential_values_between =
    sequential_values_from<ElementType, Begin, static_cast<std::size_t>(End - Begin)>;

/// @brief An array holding a sequence of integer values starting at 0, incrementing by 1 each time.
/// @tparam ElementType The type of each element
/// @tparam Count The number of elements in the sequence.
template <
    typename ElementType,
    std::size_t Count,
    constraints<std::enable_if_t<sequential_values_detail::is_valid_size_v<Count>>> = nullptr>
extern constexpr auto sequential_values = sequential_values_from<ElementType, 0, Count>;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INTEGER_SEQUENCES_SEQUENTIAL_VALUES_HPP_
