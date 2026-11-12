// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TESTS_FP_BIT_FMT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TESTS_FP_BIT_FMT_HPP_

#include <climits>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <limits>
#include <ostream>
#include <type_traits>

#include "arene/base/bit/bit_cast.hpp"

namespace arene {
namespace base {
namespace linalg_testing {

namespace fp_bit_fmt_detail {

/// @brief stream wrapper that prints a floating-point value together
///   with its decoded IEEE-754 bit representation
/// @tparam T The floating-point type to display (@c float or @c double)
///
/// Wrapper for displaying fields of a floating point value. When streamed, it
/// emits the value followed by its decoded IEEE-754 fields and its raw bit
/// pattern in hexadecimal, in the form:
///
/// @code
/// <value> [sign=<S> exp=<raw>(<unbiased>) mant=0x<mantissa>] (0x<raw-bits>)
/// @endcode
///
/// For example:
/// @code
/// 2.39063014e-18 [sign=0 exp=68(-59) mant=0x3065ba] (0x223065ba)
/// @endcode
///
/// The decoded fields are:
/// - @c sign : the sign bit (0 for positive, 1 for negative)
/// - @c exp : the raw biased exponent, followed by the unbiased exponent in
///   parentheses
/// - @c mant : the stored significand (mantissa) field in hexadecimal, i.e.
///   without the implicit leading 1 for normal values
/// - the trailing @c 0x value is the complete object bit pattern, zero-padded to
///   the full width of @c T
template <class T>
struct fp_bit_fmt_t {
  static_assert(std::numeric_limits<T>::is_iec559, "type must satisfy IEC 559/IEEE 754");

  using bits_t = std::conditional_t<sizeof(T) == 4, std::uint32_t, std::uint64_t>;
  static_assert(sizeof(bits_t) == sizeof(T), "no matching integer width for T");

  T value;

  // NOLINTNEXTLINE(readability-identifier-length) 'os' is idiomatic
  friend auto operator<<(std::ostream& os, fp_bit_fmt_t<T> fmt) -> std::ostream& {
    // IEEE-754 field layout for the given width
    constexpr std::size_t total_bits{(sizeof(T) * CHAR_BIT)};
    constexpr std::size_t mant_bits{std::numeric_limits<T>::digits - 1U};  // stored significand bits
    constexpr std::size_t exp_bits{total_bits - 1 - mant_bits};            // remaining field
    constexpr std::size_t bias{std::numeric_limits<T>::max_exponent - 1U};

    auto const bits = arene::base::bit_cast<bits_t>(fmt.value);

    auto const sign = bits >> (total_bits - 1U);
    auto const exp_raw = (bits >> mant_bits) & ((bits_t{1} << exp_bits) - 1U);
    auto const mantissa = bits & ((bits_t{1} << mant_bits) - 1U);

    // save/restore stream state so hex/padding/precision don't leak to later output.
    auto const flags = os.flags();
    auto const fill = os.fill();
    auto const precision = os.precision();

    os << std::setprecision(std::numeric_limits<T>::max_digits10) << fmt.value  //
       << std::dec << " [sign=" << sign                                         //
       << " exp=" << exp_raw << "(" << (static_cast<std::int64_t>(exp_raw) - static_cast<std::int64_t>(bias)) << ")"
       << " mant=0x" << std::hex << mantissa << "]"
       << " (0x" << std::setfill('0') << std::setw(int{sizeof(T) * 2U}) << bits << ")";

    os.flags(flags);
    os.fill(fill);
    os.precision(precision);
    return os;
  }
};

}  // namespace fp_bit_fmt_detail

/// @brief wrap a float/double so streaming it prints "value (0xBITS)".
template <class T>
auto fp_bit_fmt(T value) -> fp_bit_fmt_detail::fp_bit_fmt_t<T> {
  return {value};
}

}  // namespace linalg_testing
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_TESTS_FP_BIT_FMT_HPP_
