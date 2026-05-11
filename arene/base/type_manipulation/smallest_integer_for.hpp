// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_MANIPULATION_SMALLEST_INTEGER_FOR_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_MANIPULATION_SMALLEST_INTEGER_FOR_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/type_traits/conditional.hpp"

namespace arene {
namespace base {

// Note: limits are extracted into constexpr variables to work around a GCC 9 bug where nested
// conditional_t aliases produce incorrect results when conditions contain parenthesized function
// calls like (std::numeric_limits<T>::max)().
namespace smallest_integer_for_detail {
constexpr std::uint64_t max_uint8{(std::numeric_limits<std::uint8_t>::max)()};
constexpr std::uint64_t max_uint16{(std::numeric_limits<std::uint16_t>::max)()};
constexpr std::uint64_t max_uint32{(std::numeric_limits<std::uint32_t>::max)()};
constexpr std::int64_t max_int8{(std::numeric_limits<std::int8_t>::max)()};
constexpr std::int64_t min_int8{(std::numeric_limits<std::int8_t>::min)()};
constexpr std::int64_t max_int16{(std::numeric_limits<std::int16_t>::max)()};
constexpr std::int64_t min_int16{(std::numeric_limits<std::int16_t>::min)()};
constexpr std::int64_t max_int32{(std::numeric_limits<std::int32_t>::max)()};
constexpr std::int64_t min_int32{(std::numeric_limits<std::int32_t>::min)()};
}  // namespace smallest_integer_for_detail

/// @brief Calculates the smallest unsigned integer type that can be used to hold
/// values up to and including @c MaxValue.
/// @tparam MaxValue The maximum value to store
template <std::uint64_t MaxValue>
using smallest_unsigned_integer_for = conditional_t<
    (MaxValue <= smallest_integer_for_detail::max_uint8),
    std::uint8_t,
    conditional_t<
        (MaxValue <= smallest_integer_for_detail::max_uint16),
        std::uint16_t,
        conditional_t<(MaxValue <= smallest_integer_for_detail::max_uint32), std::uint32_t, std::uint64_t>>>;

/// @brief Calculates the smallest unsigned integer type that can be used to hold
/// values from MinValue up to and including MaxValue.
/// @tparam MinValue The minimum value to store
/// @tparam MaxValue The maximum value to store, must be greater than or equal to @c MinValue
template <std::int64_t MinValue, std::int64_t MaxValue, constraints<std::enable_if_t<(MaxValue >= MinValue)>> = nullptr>
using smallest_signed_integer_for = conditional_t<
    (MaxValue <= smallest_integer_for_detail::max_int8) && (MinValue >= smallest_integer_for_detail::min_int8),
    std::int8_t,
    conditional_t<
        (MaxValue <= smallest_integer_for_detail::max_int16) && (MinValue >= smallest_integer_for_detail::min_int16),
        std::int16_t,
        conditional_t<
            (MaxValue <= smallest_integer_for_detail::max_int32) &&
                (MinValue >= smallest_integer_for_detail::min_int32),
            std::int32_t,
            std::int64_t>>>;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_MANIPULATION_SMALLEST_INTEGER_FOR_HPP_
