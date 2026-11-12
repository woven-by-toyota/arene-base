// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::ulp_distance"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_ULP_DISTANCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_ULP_DISTANCE_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/bit/bit_cast.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/detail/is_representable_as.hpp"
#include "arene/base/math/abs_cpo.hpp"
#include "arene/base/math/float_properties.hpp"
#include "arene/base/result.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/is_signed.hpp"
#include "arene/base/stdlib_choice/make_signed.hpp"
#include "arene/base/stdlib_choice/make_unsigned.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/type_manipulation/smallest_integer_for.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/utility/safe_comparisons.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a

// Due to issue https://github.com/include-what-you-use/include-what-you-use/issues/1127
// IWYU pragma: no_include "arene/base/type_manipulation/static_if.hpp"

namespace arene {
namespace base {

/// @brief determines if a type can be compared with @c ulp_distance
///
/// Users may specialize @c enable_ulp_distance_comparable for cv-unqualified
/// program-defined types.
///
/// A type that is ULP distance comparable must:
/// * be an implicit lifetime type
/// * be bit-castable to a signed integer type with the same number of bits
/// * use an IEEE 754 bit representation, where the sign bit is the most
///   significant bit and the magnitude is ordered such that adjacent
///   representable values have adjacent integer representations
///
/// If a program specializes @c enable_ulp_distance_comparable to @c true for a
/// type that does not meet the requirements listed above, the behaviour is
/// undefined.
template <class T, class = constraints<>>
extern constexpr bool enable_ulp_distance_comparable = false;

/// @brief determines if a type can be compared with @c ulp_distance
/// Specialization enabled for @c float and @c double, conditional on IEC 559 conformance
template <class T>
extern constexpr bool enable_ulp_distance_comparable<
    T,
    constraints<std::enable_if_t<std::is_same<T, float>::value || std::is_same<T, double>::value>>> =
    std::numeric_limits<T>::is_iec559;

/// @brief enumeration used in the result error type of @c ulp_distance to
///
/// Provides the error reason if @c ulp_distance cannot return a ULP distance
/// between two values.
enum class ulp_distance_error : std::uint8_t {
  /// @brief one of the arguments is incomparable
  unordered_argument,
  /// @brief the ULP distance is not representable in the specifed result value type
  distance_not_representable,
};

namespace ulp_distance_detail {

/// @brief obtain the most significant bit of a signed integer
/// @param value integer value
/// @return @c true if the most significant bit is set, @c false otherwise
template <class I>
constexpr auto most_significant_bit(I const& value) noexcept -> bool {
  return value < I{};
}

/// @brief function object to determine the distance in units in last place (bits)
/// @tparam ResultValue signed integral type used for the result value
template <class ResultValue>
class ulp_distance_fn {
  /// @brief obtain the unsigned integer type with the same number of bytes
  /// @tparam T type whose size determines the resulting unsigned integer type
  template <class T>
  using unsigned_integer_for_t = smallest_unsigned_integer_for_type<T>;

  /// @brief converts an integer to a result, checking representability
  /// @tparam T integer type to convert from
  /// @param value integer value to convert
  /// @return @c value converted to @c ResultValue on success, or
  ///   @c ulp_distance_error::distance_not_representable if @c value
  ///   cannot be represented in @c ResultValue.
  template <class T>
  static constexpr auto to_result(T const& value) noexcept -> result<ResultValue, ulp_distance_error> {
    if (!detail::is_representable_as<ResultValue>(value)) {
      return {in_place_error, ulp_distance_error::distance_not_representable};
    }
    return {in_place_value, static_cast<ResultValue>(value)};
  }

 public:
  static_assert(std::is_signed<ResultValue>::value, "'ResultValue' must be a signed type.");
  static_assert(std::is_integral<ResultValue>::value, "'ResultValue' must be an integer type.");

  /// @brief result type for @c ulp_distance
  using result_type = result<ResultValue, ulp_distance_error>;

  /// @brief returns the distance in units in last place (bits)
  /// @tparam T floating-point type to compare
  /// @param left first value to compare
  /// @param right second value to compare
  ///
  /// Returns the distance between @c x and @c y in terms of units in last
  /// place. The sign of the result indicates direction: positive if
  /// @c x < @c y, negative if @c x > @c y.
  ///
  /// @return A result containing:
  /// - the signed ULP distance from @c x to @c y on success.
  /// - @c ulp_distance_error::unordered_argument if either @c x or @c y is
  ///   incomparable
  /// - @c ulp_distance_error::distance_not_representable if the distance
  ///   cannot be represented in @c ResultValue
  ///
  /// @post If the result has a value, let <c> lower = min(x, y) </c> and
  ///   <c> upper = max(x, y) </c>. Then <c> abs(result.value()) </c>
  ///   equals the number of successive @c std::nextafter(lower, upper)
  ///   calls needed to reach @c upper from @c lower.
  ///
  /// @note @c +0.0 and @c -0.0 are treated as 0 ULPs apart, consistent
  ///   with IEEE 754 equality.
  template <
      class T,
      constraints<
          std::enable_if_t<enable_ulp_distance_comparable<T>>,
          std::enable_if_t<is_equality_comparable_v<T>>,
          std::enable_if_t<                                       //
              is_invocable_v<                                     //
                  decltype(bit_cast<unsigned_integer_for_t<T>>),  //
                  T const&                                        //
                  >                                               //
              >                                                   //
          > = nullptr>
  ARENE_BIT_CAST_CONSTEXPR auto operator()(T const& left, T const& right) const noexcept -> result_type {
    if (::arene::base::isnan(left) || ::arene::base::isnan(right)) {
      return {in_place_error, ulp_distance_error::unordered_argument};
    }

    using unsigned_integer_type = unsigned_integer_for_t<T>;
    using signed_integer_type = std::make_signed_t<unsigned_integer_type>;

    constexpr auto as_bits = bit_cast<signed_integer_type>;

    auto const left_bits = as_bits(math::abs(left));
    auto const right_bits = as_bits(math::abs(right));

    auto const left_signbit = most_significant_bit(as_bits(left));
    auto const right_signbit = most_significant_bit(as_bits(right));

    if (left_signbit == right_signbit) {
      // 'left_bits' and 'right_bits' are both non-negative (bit-casts of absolute
      // values), so their difference cannot overflow a signed subtraction.
      auto const dist = left_bits - right_bits;
      return left_signbit ? to_result(dist) : to_result(-dist);
    }

    auto const dist = static_cast<unsigned_integer_type>(
        static_cast<unsigned_integer_type>(left_bits) + static_cast<unsigned_integer_type>(right_bits)
    );

    if (right < left) {
      using unsigned_result = std::make_unsigned_t<ResultValue>;
      constexpr unsigned_result max_abs_lowest{
          static_cast<unsigned_result>(std::numeric_limits<ResultValue>::max()) + unsigned_result{1U}
      };

      if (cmp_less(max_abs_lowest, dist)) {
        return {in_place_error, ulp_distance_error::distance_not_representable};
      }

      // Negate in unsigned_result space to avoid truncation when
      // unsigned_integer_type is narrower than ResultValue (e.g. uint32_t vs
      // uint64_t). The result is then reinterpreted as a signed value via two's
      // complement, which correctly represents the minimum value of ResultValue
      // (e.g. -128 for int8_t) that has no positive counterpart.
      return {
          in_place_value,
          static_cast<ResultValue>(static_cast<unsigned_result>(-static_cast<unsigned_result>(dist)))
      };
    }

    return to_result(dist);
  }
};
}  // namespace ulp_distance_detail

/// @brief Compute the distance in units in the last place between two floating-point values.
/// @tparam ResultValue signed integral type used for the result value, defaults to @c std::int64_t
template <class ResultValue = std::int64_t>
extern constexpr auto ulp_distance = ulp_distance_detail::ulp_distance_fn<ResultValue>{};

}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_ULP_DISTANCE_HPP_
