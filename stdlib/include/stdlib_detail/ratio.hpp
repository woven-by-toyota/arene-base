// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_RATIO_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_RATIO_HPP_

// IWYU pragma: private, include <ratio>
// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

#include "arene/base/math/abs.hpp"
#include "arene/base/math/gcd.hpp"
#include "arene/base/utility/swap.hpp"
#include "stdlib/include/stdlib_detail/cstdint.hpp"
#include "stdlib/include/stdlib_detail/numeric_limits.hpp"

namespace std {

/// @brief A compile-time ratio of two integers, automatically reduced to lowest terms
/// @tparam N The unreduced numerator of the ratio
/// @tparam D The unreduced denominator of the ratio
/// @pre The denominator must not be 0, and both numerator and denominator must have representable absolute values
template <intmax_t N, intmax_t D = 1>
class ratio {
  static_assert(D != 0, "Denominator of a ratio must not be 0");
  // parasoft-begin-suppress CERT_C-PRE31-c "False positive: neither static_assert nor min are macros"
  static_assert(N != numeric_limits<intmax_t>::min(), "Numerator of ratio must have a representable absolute value");
  static_assert(D != numeric_limits<intmax_t>::min(), "Denominator of ratio must have a representable absolute value");
  // parasoft-end-suppress CERT_C-PRE31-c
  /// @brief The greatest common divisor of the numerator and denominator, used to reduce them to lowest terms
  static constexpr intmax_t gcd{::arene::base::gcd(N, D)};
  /// @brief The sign of the denominator parameter (used to adjust the sign of the reduced numerator)
  // parasoft-begin-suppress AUTOSAR-A5_16_1-a "False positive: this is not a sub-expression"
  static constexpr intmax_t d_sign{D > 0 ? 1 : -1};
  // parasoft-end-suppress AUTOSAR-A5_16_1-a

 public:
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "This is part of the C++14 standard API"
  /// @brief The numerator of the ratio, reduced to lowest terms
  static constexpr intmax_t num{d_sign * N / gcd};
  /// @brief The denominator of the ratio, reduced to lowest terms
  static constexpr intmax_t den{::arene::base::abs(D) / gcd};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a
  /// @brief The type of an equivalent ratio whose *reduced* numerator and denominator are the same as this ratio
  using type = ratio<num, den>;
};

namespace ratio_detail {
/// @brief A type trait to check if a type is a specialization of @c std::ratio as required in [ratio.general]
/// @tparam T The type to check; @c std::false_type if this is not a @c std::ratio specialization
template <typename T>
class is_ratio : public std::false_type {};

/// @brief A type trait to check if a type is a specialization of @c std::ratio as required in [ratio.general]
/// @tparam N The numerator of a ratio being passed through the check
/// @tparam D The denominator of a ratio being passed through the check
template <intmax_t N, intmax_t D>
class is_ratio<ratio<N, D>> : public std::true_type {};

/// @brief A helper for multiplication to enforce the type requirement in [ratio.general] and improve readability
/// @tparam R1 The first of two ratios to be multiplied
/// @tparam R2 The second of two ratios to be multiplied
template <class R1, class R2>
class ratio_multiply_impl {
  static_assert(is_ratio<R1>::value, "Parameters to ratio arithmetic operations must be specializations of std::ratio");
  static_assert(is_ratio<R2>::value, "Parameters to ratio arithmetic operations must be specializations of std::ratio");
  // parasoft-begin-suppress AUTOSAR-M0_1_3-c "False positive: these variables are used"
  /// @brief The GCD of the numerator of R1 with the denominator of R2, used to pre-emptively reduce both
  static constexpr intmax_t gcd_12{::arene::base::gcd(R1::num, R2::den)};
  /// @brief The GCD of the numerator of R2 with the denominator of R1, used to pre-emptively reduce both
  static constexpr intmax_t gcd_21{::arene::base::gcd(R2::num, R1::den)};
  // parasoft-end-suppress AUTOSAR-M0_1_3-c

 public:
  /// @brief The result of the multiplication
  using type = ratio<(R1::num / gcd_12) * (R2::num / gcd_21), (R1::den / gcd_21) * (R2::den / gcd_12)>;
};

/// @brief A helper for addition to enforce the type requirement in [ratio.general] and improve readability
/// @tparam R1 The first of two ratios to be added
/// @tparam R2 The second of two ratios to be added
template <class R1, class R2>
class ratio_add_impl {
  static_assert(is_ratio<R1>::value, "Parameters to ratio arithmetic operations must be specializations of std::ratio");
  static_assert(is_ratio<R2>::value, "Parameters to ratio arithmetic operations must be specializations of std::ratio");
  // parasoft-begin-suppress AUTOSAR-M0_1_3-c "False positive: these variables are used"
  /// @brief The GCD of the numerators of the two ratios, used to pre-emptively reduce both
  static constexpr intmax_t gcd_num{(R1::num == 0 && R2::num == 0) ? 1 : ::arene::base::gcd(R1::num, R2::num)};
  /// @brief The GCD of the denominators of the two ratios, used to pre-emptively reduce both
  static constexpr intmax_t gcd_den{::arene::base::gcd(R2::den, R1::den)};
  // parasoft-end-suppress AUTOSAR-M0_1_3-c

  /// @brief A few terms pulled out of the main addition to reduce the magnitudes of the operands as much as possible
  using left = ratio<gcd_num, R2::den>;
  /// @brief The main part of the addition, with each of the operands reduced by their GCD with something else
  using right =
      ratio<(R1::num / gcd_num) * (R2::den / gcd_den) + (R2::num / gcd_num) * (R1::den / gcd_den), R1::den / gcd_den>;

 public:
  /// @brief The result of the addition
  using type = typename ratio_multiply_impl<left, right>::type;
};

/// @brief A helper for equality comparison to enforce the type requirement in [ratio.general]
/// @tparam R1 The first of two ratios to be compared
/// @tparam R2 The second of two ratios to be compared
template <class R1, class R2>
class ratio_equal_impl {
  static_assert(is_ratio<R1>::value, "Parameters to ratio comparison operations must be specializations of std::ratio");
  static_assert(is_ratio<R2>::value, "Parameters to ratio comparison operations must be specializations of std::ratio");

 public:
  /// @brief The result of the comparison
  using type = integral_constant<bool, R1::num == R2::num && R1::den == R2::den>;
};

/// @brief A type used to represent a fraction for non-template computations
struct fraction {
  /// @brief The numerator of the fraction
  intmax_t num;
  /// @brief The denominator of the fraction
  intmax_t den;
};

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

/// @brief Check if two fractions can be directly compared (by cross multiplying them) without overflowing
/// @param left The left side of the comparison
/// @param right The right side of the comparison
/// @return @c true if <tt>left < right</tt> can be directly computed without overflowing; @c false if not
constexpr auto can_directly_compare(fraction const& left, fraction const& right) noexcept -> bool {
  return (left.num == 0) || (right.num == 0) ||
         ((left.num <= numeric_limits<intmax_t>::max() / right.den) &&
          (right.num <= numeric_limits<intmax_t>::max() / left.den));
}

/// @brief Check if the left fraction is less than the right or not
/// @param left The fraction on the left side of the comparison
/// @param right The fraction on the right side of the comparison
/// @return @c true if <tt>left < right</tt>, @c false if not
constexpr auto fraction_less(fraction left, fraction right) noexcept -> bool {
  bool flip_direction{false};

  // Check shortcuts we can make based on the signs of the parameters
  if (left.num < 0) {
    if (right.num >= 0) {
      return true;  // left is negative, right isn't => definitely less
    }
    // Make both arguments positive and flip the comparison order
    flip_direction = !flip_direction;
    left.num = -left.num;
    right.num = -right.num;
  }
  if (right.num < 0) {
    return false;  // right is negative, left isn't => definitely not less
  }

  // If one fraction is greater than 1 and the other isn't, the one that's greater than 1 is greater
  if ((left.num > left.den) != (right.num > right.den)) {
    return right.num > right.den;
  }

  // We'll repeatedly reduce the fractions until we can compare them successfully
  while (!(can_directly_compare(left, right))) {
    // If we can't multiply at all then we need to decompose the two into integral parts and fractional parts
    flip_direction = !flip_direction;
    ::arene::base::swap(left.num, left.den);
    ::arene::base::swap(right.num, right.den);

    // If the two have different integral parts, then we only need to compare the integral parts (which always works)
    intmax_t const left_integ{left.num / left.den};
    intmax_t const right_integ{right.num / right.den};
    if (left_integ != right_integ) {
      left = {left_integ, 1};
      right = {right_integ, 1};
      break;
    }

    // If the two have the same integral part, we subtract off the integral part and go again
    left.num %= left.den;
    right.num %= right.den;
  }

  return flip_direction ? right.num * left.den < right.den * left.num : left.num * right.den < left.den * right.num;
}

// parasoft-end-suppress AUTOSAR-A7_1_5-a-2

/// @brief A helper for less-than comparison to enforce the type requirement in [ratio.general]
/// @tparam R1 The first of two ratios to be compared
/// @tparam R2 The second of two ratios to be compared
template <class R1, class R2>
class ratio_less_impl {
  static_assert(is_ratio<R1>::value, "Parameters to ratio comparison operations must be specializations of std::ratio");
  static_assert(is_ratio<R2>::value, "Parameters to ratio comparison operations must be specializations of std::ratio");

 public:
  /// @brief The result of the comparison
  using type = integral_constant<bool, fraction_less(fraction{R1::num, R1::den}, fraction{R2::num, R2::den})>;
};

}  // namespace ratio_detail

/// @brief Defines the ratio yielded by multiplying the two given ratios
/// @tparam R1 The first multiplicand
/// @tparam R2 The second multiplicand
///
/// @note Performs reduction before multiplication so this should always work as long as the result is representable.
template <class R1, class R2>
using ratio_multiply = typename ratio_detail::ratio_multiply_impl<R1, R2>::type;

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: typedef *is* preceded by a comment with @brief"

/// @brief Defines the ratio yielded by dividing the two given ratios
/// @tparam R1 The dividend
/// @tparam R2 The divisor
///
/// @note Performs reduction before division so this should always work as long as the result is representable.
template <class R1, class R2>
using ratio_divide = ratio_multiply<R1, ratio<R2::den, R2::num>>;

// parasoft-end-suppress AUTOSAR-A2_7_3-a

/// @brief Defines the ratio yielded by adding the two given ratios
/// @tparam R1 The first addend
/// @tparam R2 The second addend
///
/// @note Performs reduction before addition so this should always work as long as the result is representable.
template <class R1, class R2>
using ratio_add = typename ratio_detail::ratio_add_impl<R1, R2>::type;

/// @brief Defines the ratio yielded by subtracting the two given ratios
/// @tparam R1 The minuend
/// @tparam R2 The subtrahend
///
/// @note Performs reduction before subtraction so this should always work as long as the result is representable.
template <class R1, class R2>
using ratio_subtract = ratio_add<R1, ratio<-R2::num, R2::den>>;

/// @brief Inherits from @c true_type if the first ratio is equal to the second, @c false_type if not
/// @tparam R1 The first ratio
/// @tparam R2 The second ratio
template <class R1, class R2>
class ratio_equal : public ratio_detail::ratio_equal_impl<R1, R2>::type {};

/// @brief Inherits from @c true_type if the first ratio is not equal to the second, @c false_type if they are equal
/// @tparam R1 The first ratio
/// @tparam R2 The second ratio
template <class R1, class R2>
class ratio_not_equal : public integral_constant<bool, !ratio_equal<R1, R2>::value> {};

/// @brief Inherits from @c true_type if the first ratio is less than the second, @c false_type if not
/// @tparam R1 The first ratio
/// @tparam R2 The second ratio
template <class R1, class R2>
class ratio_less : public ratio_detail::ratio_less_impl<R1, R2>::type {};

/// @brief Inherits from @c true_type if the first ratio is less than or equal to the second, @c false_type if not
/// @tparam R1 The first ratio
/// @tparam R2 The second ratio
template <class R1, class R2>
class ratio_less_equal : public integral_constant<bool, !ratio_less<R2, R1>::value> {};

/// @brief Inherits from @c true_type if the first ratio is greater than the second, @c false_type if not
/// @tparam R1 The first ratio
/// @tparam R2 The second ratio
template <class R1, class R2>
class ratio_greater : public integral_constant<bool, ratio_less<R2, R1>::value> {};

/// @brief Inherits from @c true_type if the first ratio is greater than or equal to the second, @c false_type if not
/// @tparam R1 The first ratio
/// @tparam R2 The second ratio
template <class R1, class R2>
class ratio_greater_equal : public integral_constant<bool, !ratio_less<R1, R2>::value> {};

// NOLINTBEGIN(readability-magic-numbers) These are named constants to replace users' magic numbers
/// @brief The SI prefix for 10^(-18), expressed as a @c ratio
using atto = ratio<1, 1'000'000'000'000'000'000>;
/// @brief The SI prefix for 10^(-15), expressed as a @c ratio
using femto = ratio<1, 1'000'000'000'000'000>;
/// @brief The SI prefix for 10^(-12), expressed as a @c ratio
using pico = ratio<1, 1'000'000'000'000>;
/// @brief The SI prefix for 10^(-9), expressed as a @c ratio
using nano = ratio<1, 1'000'000'000>;
/// @brief The SI prefix for 10^(-6), expressed as a @c ratio
using micro = ratio<1, 1'000'000>;
/// @brief The SI prefix for 10^(-3), expressed as a @c ratio
using milli = ratio<1, 1'000>;
/// @brief The SI prefix for 10^(-2), expressed as a @c ratio
using centi = ratio<1, 100>;
/// @brief The SI prefix for 10^(-1), expressed as a @c ratio
using deci = ratio<1, 10>;
/// @brief The SI prefix for 10^1, expressed as a @c ratio
using deca = ratio<10, 1>;
/// @brief The SI prefix for 10^2, expressed as a @c ratio
using hecto = ratio<100, 1>;
/// @brief The SI prefix for 10^3, expressed as a @c ratio
using kilo = ratio<1'000, 1>;
/// @brief The SI prefix for 10^6, expressed as a @c ratio
using mega = ratio<1'000'000, 1>;
/// @brief The SI prefix for 10^9, expressed as a @c ratio
using giga = ratio<1'000'000'000, 1>;
/// @brief The SI prefix for 10^12, expressed as a @c ratio
using tera = ratio<1'000'000'000'000, 1>;
/// @brief The SI prefix for 10^15, expressed as a @c ratio
using peta = ratio<1'000'000'000'000'000, 1>;
/// @brief The SI prefix for 10^18, expressed as a @c ratio
using exa = ratio<1'000'000'000'000'000'000, 1>;
// NOLINTEND(readability-magic-numbers)

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_RATIO_HPP_
