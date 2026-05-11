// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_NUMERIC_LIMITS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_NUMERIC_LIMITS_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A3_9_1-b "The C++ standard explicitly specifies the use of int"
// parasoft-begin-suppress AUTOSAR-M11_0_1-a "The C++ standard explicitly specifies that members are public"

// IWYU pragma: private, include <limits>
// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress AUTOSAR-A18_0_1-a-2 "The C++ headers are not available"
// parasoft-begin-suppress AUTOSAR-A1_1_1-d-2 "The C++ headers are not available"
// NOLINTNEXTLINE(hicpp-deprecated-headers)
#include <float.h>
// NOLINTNEXTLINE(hicpp-deprecated-headers)
#include <limits.h>
// parasoft-end-suppress AUTOSAR-A1_1_1-d-2
// parasoft-end-suppress AUTOSAR-A18_0_1-a-2

#include "arene/base/constraints.hpp"
#include "stdlib/include/stdlib_detail/cstdint.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/is_signed.hpp"

// parasoft-begin-suppress AUTOSAR-A16_0_1-a "Preprocessor used to prevent compilation in unsupported configurations"
// parasoft-begin-suppress AUTOSAR-A16_6_1-a "Preprocessor used to prevent compilation in unsupported configurations"
// parasoft-begin-suppress AUTOSAR-A16_0_1-b "Preprocessor used to prevent compilation in unsupported configurations"
#if defined(__FAST_MATH__)
#error "Fast math not supported as does not conform to IEC559"
#endif

// parasoft-begin-suppress AUTOSAR-M16_0_7-a "Macro only checked if defined"
#if defined(__FINITE_MATH_ONLY__) && (__FINITE_MATH_ONLY__ == 1)
#error "Finite math only not supported does not conform to IEC559"
#endif
// parasoft-end-suppress AUTOSAR-M16_0_7-a

// parasoft-begin-suppress AUTOSAR-M16_0_7-a "Macro only checked if defined"
#if defined(__NO_TRAPPING_MATH__) && (__NO_TRAPPING_MATH__ == 1)
#error "Disabling trapping math not supported as it does not conform to IEC559"
#endif
// parasoft-end-suppress AUTOSAR-M16_0_7-a

// parasoft-begin-suppress AUTOSAR-M16_0_7-a "Macro only checked if defined"
#if defined(__GCC_IEC_559) && (__GCC_IEC_559 == 0)
#error "Unsupported floating point mode that does not conform to IEC559 enabled"
#endif
// parasoft-end-suppress AUTOSAR-M16_0_7-a
// parasoft-end-suppress AUTOSAR-A16_0_1-a
// parasoft-end-suppress AUTOSAR-A16_6_1-a
// parasoft-end-suppress AUTOSAR-A16_0_1-b

namespace std {

// NOLINTNEXTLINE(readability-magic-numbers)
static_assert(CHAR_BIT == 8, "Values only correct for platforms with 8-bit bytes");

// parasoft-begin-suppress AUTOSAR-A7_2_3-a "Specified as a non-scoped enum in the C++ standard"
/// @brief Enumeration describing possible rounding styles
enum float_round_style : std::int8_t {
  /// @brief Rounding is indeterminable
  round_indeterminate = -1,
  /// @brief Round towards zero
  round_toward_zero = 0,
  /// @brief Round towards nearest
  round_to_nearest = 1,
  /// @brief Round towards positive infinity
  round_toward_infinity = 2,
  /// @brief Round towards negative infinity
  round_toward_neg_infinity = 3
};
// parasoft-end-suppress AUTOSAR-A7_2_3-a

// parasoft-begin-suppress AUTOSAR-A7_2_3-a "Specified as a non-scoped enum in the C++ standard"
/// @brief Enumeration describing possible denormalized value representation properties
enum float_denorm_style : std::int8_t {
  /// @brief It cannot be determined if this type has denormalized values
  denorm_indeterminate = -1,
  /// @brief This type has no denormalized values
  denorm_absent = 0,
  /// @brief This type has denormalized values
  denorm_present = 1
};
// parasoft-end-suppress AUTOSAR-A7_2_3-a

namespace numeric_limits_detail {

/// @brief Internal helper to define properties for integer types
template <std::size_t>
struct int_info;

/// @brief Internal helper to define properties for 1-byte integer types
template <>
struct int_info<1> {
  /// @brief The number of decimal digits
  static constexpr int digits10{2};
  /// @brief The number of decimal digits when unsigned
  static constexpr int unsigned_digits10{2};
  /// @brief The minimum value of a signed integer with this size
  static constexpr int8_t signed_min{-128};
  /// @brief The maximum value of a signed integer with this size
  static constexpr int8_t signed_max{127};
  /// @brief The maximum value of an unsigned integer with this size
  static constexpr uint8_t unsigned_max{255U};
};

/// @brief Internal helper to define properties for 2-byte integer types
template <>
struct int_info<2> {
  /// @brief The number of decimal digits
  static constexpr int digits10{4};
  /// @brief The number of decimal digits when unsigned
  static constexpr int unsigned_digits10{4};
  /// @brief The minimum value of a signed integer with this size
  static constexpr int16_t signed_min{-32768};
  /// @brief The minimum value of a signed integer with this size
  static constexpr int16_t signed_max{32767};
  /// @brief The maximum value of an unsigned integer with this size
  static constexpr uint16_t unsigned_max{65535U};
};

/// @brief Internal helper to define properties for 4-byte integer types
template <>
struct int_info<4> {
  /// @brief The number of decimal digits
  static constexpr int digits10{9};
  /// @brief The number of decimal digits when unsigned
  static constexpr int unsigned_digits10{9};
  // parasoft-begin-suppress CERT_C-STR34-b "False positive: No char types used here"
  /// @brief The maximum value of a signed integer with this size
  static constexpr int32_t signed_max{static_cast<int32_t>(2'147'483'647L)};
  // parasoft-end-suppress CERT_C-STR34-b
  /// @brief The minimum value of a signed integer with this size
  static constexpr int32_t signed_min{static_cast<int32_t>(-signed_max - 1)};
  /// @brief The maximum value of an unsigned integer with this size
  static constexpr uint32_t unsigned_max{static_cast<uint32_t>(4'294'967'295U)};
};

/// @brief Internal helper to define properties for 8-byte integer types
template <>
// NOLINTNEXTLINE(readability-magic-numbers)
struct int_info<8> {
  /// @brief The number of decimal digits
  static constexpr int digits10{18};
  /// @brief The number of decimal digits when unsigned
  static constexpr int unsigned_digits10{19};
  // parasoft-begin-suppress CERT_C-STR34-b "False positive: No char types used here"
  /// @brief The maximum value of a signed integer with this size
  static constexpr int64_t signed_max{static_cast<int64_t>(9'223'372'036'854'775'807LL)};
  // parasoft-end-suppress CERT_C-STR34-b
  /// @brief The minimum value of a signed integer with this size
  static constexpr int64_t signed_min{-signed_max - static_cast<int64_t>(1)};
  /// @brief The maximum value of an unsigned integer with this size
  static constexpr uint64_t unsigned_max{static_cast<uint64_t>(18'446'744'073'709'551'615ULL)};
};

/// @brief Implementation template for @c numeric_limits
/// @tparam T The type being checked
template <typename T, typename = arene::base::constraints<>>
class numeric_limits_impl {
 public:
  /// @brief Is this a specialization for which the other values are meaningful? @c true if so, @c false otherwise
  static constexpr bool is_specialized{false};
  /// @brief Is the type an integral type? @c true if so, @c false otherwise
  static constexpr bool is_integer{false};
  /// @brief Is the type an exact type? @c true if so, @c false otherwise
  static constexpr bool is_exact{false};
  /// @brief Is the type a signed type? @c true if so, @c false otherwise
  static constexpr bool is_signed{false};
  /// @brief The base used for the representation.
  static constexpr int radix{0};
  /// @brief The number of @c radix digits in the representation, ignoring any sign bit
  static constexpr int digits{0};
  /// @brief The maximum number of base-10  digits that can be completely represented in values of the type
  static constexpr int digits10{0};
  /// @brief @c true if the set of values representable by the type is bounded, otherwise @c false
  static constexpr bool is_bounded{false};
  /// @brief @c true if the set of values representable by the type is modulo, otherwise @c false
  static constexpr bool is_modulo{false};
  /// @brief @c true if there are trap representations of this type, otherwise @c false
  static constexpr bool traps{false};

  /// @brief For floating point types, the number of base-10  digits required to ensure that values which differ can be
  /// differentiated
  static constexpr int max_digits10{0};
  /// @brief For floating point types, the minimum negative integer such that @c radix raised to the power of one less
  /// than that integer is a normalized floating point number.
  static constexpr int min_exponent{0};
  /// @brief For floating point types, the minimum negative integer such that 10 raised to the power of one less than
  /// that integer is in the range of normalized floating point numbers.
  static constexpr int min_exponent10{0};
  /// @brief For floating point types, the maximum positive integer such that @c radix raised to the power of one less
  /// than that integer is a representable finite floating point number.
  static constexpr int max_exponent{0};
  /// @brief For floating point types, the maximum positive integer such that 10 raised to the power of one less than
  /// that integer is in the range of representable finite floating point numbers.
  static constexpr int max_exponent10{0};
  /// @brief For floating point types, @c true if the type has a representation of positive infinity, otherwise @c false
  static constexpr bool has_infinity{false};
  /// @brief For floating point types, @c true if the type has a representation of a quiet (non-signaling)
  /// "Not-a-Number", otherwise @c false
  // NOLINTNEXTLINE(readability-identifier-naming)
  static constexpr bool has_quiet_NaN{false};
  /// @brief For floating point types, @c true if the type has a representation of a signaling "Not-a-Number", otherwise
  /// @c false
  // NOLINTNEXTLINE(readability-identifier-naming)
  static constexpr bool has_signaling_NaN{false};
  /// @brief For floating point types, an indicator of whether the type has a representation of denormalized values
  static constexpr float_denorm_style has_denorm{denorm_absent};
  /// @brief For floating point types, @c true if loss of accuracy is detected as a denormalization loss rather than an
  /// inexact result, otherwise @c false
  static constexpr bool has_denorm_loss{false};
  /// @brief For floating point types, @c true if the type adheres to the IEC559 standard, otherwise @c false
  static constexpr bool is_iec559{false};
  /// @brief For floating point types, @c true if tinyness is detected before rounding, otherwise @c false
  static constexpr bool tinyness_before{false};
  /// @brief For floating point types, an indicator of the default rounding style
  static constexpr float_round_style round_style{round_toward_zero};

  /// @brief The maximum finite value
  /// @return The maximum value
  // parasoft-begin-suppress AUTOSAR-A5_2_2-a "False positive: No C-style cast is used"
  static constexpr auto max() noexcept -> T { return T(); }
  // parasoft-end-suppress AUTOSAR-A5_2_2-a

  /// @brief The minimum finite value; the minimum positive normalized value for floating point types
  /// @return The minimum value
  static constexpr auto min() noexcept -> T { return T(); }
  /// @brief The minimum finite value such that there is no finite value @c y such that @c y<lowest()
  /// @return The minimum value
  static constexpr auto lowest() noexcept -> T { return T(); }

  /// @brief For floating point types, the epsilon value: the difference between 1 and the least representable value
  /// greater than 1
  /// @return The epsilon value
  static constexpr auto epsilon() noexcept -> T { return T(); }
  /// @brief For floating point types, the maximum rounding error
  /// @return The maximum rounding error
  static constexpr auto round_error() noexcept -> T { return T(); }
  /// @brief For floating point types where @c has_infinity is @c true, a representation of positive infinity
  /// @return Positive infinity
  static constexpr auto infinity() noexcept -> T { return T(); }
  /// @brief For floating point types where @c has_quiet_NaN is @c true, a representation of a quiet NaN
  /// @return A quiet NaN value
  // NOLINTNEXTLINE(readability-identifier-naming)
  static constexpr auto quiet_NaN() noexcept -> T { return T(); }
  /// @brief For floating point types where @c has_signaling_NaN is @c true, a representation of a signaling NaN
  /// @return A signaling NaN value
  // NOLINTNEXTLINE(readability-identifier-naming)
  static constexpr auto signaling_NaN() noexcept -> T { return T(); }
  /// @brief For floating point types, the minimum positive denormalized value. If the representation has no
  /// denormalized values, the minimum positive normalized value.
  /// @return The minimum positive denormalized value
  static constexpr auto denorm_min() noexcept -> T { return T(); }
};

/// @brief Specialization of the implementation template for @c numeric_limits for integral types
/// @tparam T The type being checked
template <typename T>
class numeric_limits_impl<T, arene::base::constraints<enable_if_t<is_integral_v<T>>>> {
 public:
  /// @brief Is this a specialization for which the other values are meaningful? @c true if so, @c false otherwise
  static constexpr bool is_specialized{true};
  /// @brief Is the type an integral type? @c true if so, @c false otherwise
  static constexpr bool is_integer{true};
  /// @brief Is the type an exact type? @c true if so, @c false otherwise
  static constexpr bool is_exact{true};
  // parasoft-begin-suppress AUTOSAR-A3_3_2-a "False positive: The initializer is constant"
  // parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: is_signed_v is initialized"
  /// @brief Is the type a signed type? @c true if so, @c false otherwise
  static constexpr bool is_signed{is_signed_v<T>};
  // parasoft-end-suppress CERT_CPP-DCL56-a
  // parasoft-end-suppress AUTOSAR-A3_3_2-a
  /// @brief The base used for the representation.
  static constexpr int radix{2};
  // parasoft-begin-suppress AUTOSAR-A5_16_1-a "False positive: Conditional operator not used as subexpression"
  /// @brief The number of @c radix digits in the representation, ignoring any sign bit
  static constexpr int digits{static_cast<int>(
      is_signed ? (sizeof(T) * static_cast<unsigned>(CHAR_BIT) - 1U) : (sizeof(T) * static_cast<unsigned>(CHAR_BIT))
  )};
  // parasoft-end-suppress AUTOSAR-A5_16_1-a
  // parasoft-begin-suppress AUTOSAR-A5_16_1-a "False positive: Conditional operator not used as subexpression"
  /// @brief The maximum number of base-10  digits that can be completely represented in values of the type
  static constexpr int digits10{is_signed ? int_info<sizeof(T)>::digits10 : int_info<sizeof(T)>::unsigned_digits10};
  // parasoft-end-suppress AUTOSAR-A5_16_1-a
  /// @brief @c true if the set of values representable by the type is bounded, otherwise @c false
  static constexpr bool is_bounded{true};
  /// @brief @c true if the set of values representable by the type is modulo, otherwise @c false
  static constexpr bool is_modulo{!is_signed};
  /// @brief @c true if there are trap representations of this type, otherwise @c false
  static constexpr bool traps{true};

  /// @brief For floating point types, the number of base-10  digits required to ensure that values which differ can be
  /// differentiated
  static constexpr int max_digits10{0};
  /// @brief For floating point types, the minimum negative integer such that @c radix raised to the power of one less
  /// than that integer is a normalized floating point number.
  static constexpr int min_exponent{0};
  /// @brief For floating point types, the minimum negative integer such that 10 raised to the power of one less than
  /// that integer is in the range of normalized floating point numbers.
  static constexpr int min_exponent10{0};
  /// @brief For floating point types, the maximum positive integer such that @c radix raised to the power of one less
  /// than that integer is a representable finite floating point number.
  static constexpr int max_exponent{0};
  /// @brief For floating point types, the maximum positive integer such that 10 raised to the power of one less than
  /// that integer is in the range of representable finite floating point numbers.
  static constexpr int max_exponent10{0};
  /// @brief For floating point types, @c true if the type has a representation of positive infinity, otherwise @c false
  static constexpr bool has_infinity{false};
  /// @brief For floating point types, @c true if the type has a representation of a quiet (non-signaling)
  /// "Not-a-Number", otherwise @c false
  // NOLINTNEXTLINE(readability-identifier-naming)
  static constexpr bool has_quiet_NaN{false};
  /// @brief For floating point types, @c true if the type has a representation of a signaling "Not-a-Number", otherwise
  /// @c false
  // NOLINTNEXTLINE(readability-identifier-naming)
  static constexpr bool has_signaling_NaN{false};
  /// @brief For floating point types, an indicator of whether the type has a representation of denormalized values
  static constexpr float_denorm_style has_denorm{denorm_absent};
  /// @brief For floating point types, @c true if loss of accuracy is detected as a denormalization loss rather than an
  /// inexact result, otherwise @c false
  static constexpr bool has_denorm_loss{false};
  /// @brief For floating point types, @c true if the type adheres to the IEC559 standard, otherwise @c false
  static constexpr bool is_iec559{false};
  /// @brief For floating point types, @c true if tinyness is detected before rounding, otherwise @c false
  static constexpr bool tinyness_before{false};
  /// @brief For floating point types, an indicator of the default rounding style
  static constexpr float_round_style round_style{round_toward_zero};

  /// @brief The maximum finite value
  /// @return The maximum value
  static constexpr auto max() noexcept -> T {
    return is_signed ? static_cast<T>(int_info<sizeof(T)>::signed_max)
                     : static_cast<T>(int_info<sizeof(T)>::unsigned_max);
  }
  /// @brief The minimum finite value; the minimum positive normalized value for floating point types
  /// @return The minimum value
  static constexpr auto min() noexcept -> T {
    return is_signed ? static_cast<T>(int_info<sizeof(T)>::signed_min) : static_cast<T>(0);
  }
  /// @brief The minimum finite value such that there is no finite value @c y such that @c y<lowest()
  /// @return The minimum value
  static constexpr auto lowest() noexcept -> T { return min(); }

  /// @brief For floating point types, the epsilon value: the difference between 1 and the least representable value
  /// greater than 1
  /// @return The epsilon value
  static constexpr auto epsilon() noexcept -> T { return T{}; }
  /// @brief For floating point types, the maximum rounding error
  /// @return The maximum rounding error
  static constexpr auto round_error() noexcept -> T { return T{}; }
  /// @brief For floating point types where @c has_infinity is @c true, a representation of positive infinity
  /// @return Positive infinity
  static constexpr auto infinity() noexcept -> T { return T{}; }
  /// @brief For floating point types where @c has_quiet_NaN is @c true, a representation of a quiet NaN
  /// @return A quiet NaN value
  // NOLINTNEXTLINE(readability-identifier-naming)
  static constexpr auto quiet_NaN() noexcept -> T { return T{}; }
  /// @brief For floating point types where @c has_signaling_NaN is @c true, a representation of a signaling NaN
  /// @return A signaling NaN value
  // NOLINTNEXTLINE(readability-identifier-naming)
  static constexpr auto signaling_NaN() noexcept -> T { return T{}; }
  /// @brief For floating point types, the minimum positive denormalized value. If the representation has no
  /// denormalized values, the minimum positive normalized value.
  /// @return The minimum positive denormalized value
  static constexpr auto denorm_min() noexcept -> T { return T{}; }
};

/// @brief Specialization of the implementation template for @c numeric_limits for @c bool
template <>
class numeric_limits_impl<bool, arene::base::constraints<>> {
 public:
  /// @brief Is this a specialization for which the other values are meaningful? @c true if so, @c false otherwise
  static constexpr bool is_specialized{true};
  /// @brief Is the type an integral type? @c true if so, @c false otherwise
  static constexpr bool is_integer{true};
  /// @brief Is the type an exact type? @c true if so, @c false otherwise
  static constexpr bool is_exact{true};
  /// @brief Is the type a signed type? @c true if so, @c false otherwise
  static constexpr bool is_signed{false};
  /// @brief The base used for the representation.
  static constexpr int radix{2};
  /// @brief The number of @c radix digits in the representation, ignoring any sign bit
  static constexpr int digits{1};
  /// @brief The maximum number of base-10  digits that can be completely represented in values of the type
  static constexpr int digits10{0};
  /// @brief @c true if the set of values representable by the type is bounded, otherwise @c false
  static constexpr bool is_bounded{true};
  /// @brief @c true if the set of values representable by the type is modulo, otherwise @c false
  static constexpr bool is_modulo{false};
  /// @brief @c true if there are trap representations of this type, otherwise @c false
  static constexpr bool traps{false};

  /// @brief For floating point types, the number of base-10  digits required to ensure that values which differ can be
  /// differentiated
  static constexpr int max_digits10{0};
  /// @brief For floating point types, the minimum negative integer such that @c radix raised to the power of one less
  /// than that integer is a normalized floating point number.
  static constexpr int min_exponent{0};
  /// @brief For floating point types, the minimum negative integer such that 10 raised to the power of one less than
  /// that integer is in the range of normalized floating point numbers.
  static constexpr int min_exponent10{0};
  /// @brief For floating point types, the maximum positive integer such that @c radix raised to the power of one less
  /// than that integer is a representable finite floating point number.
  static constexpr int max_exponent{0};
  /// @brief For floating point types, the maximum positive integer such that 10 raised to the power of one less than
  /// that integer is in the range of representable finite floating point numbers.
  static constexpr int max_exponent10{0};
  /// @brief For floating point types, @c true if the type has a representation of positive infinity, otherwise @c false
  static constexpr bool has_infinity{false};
  /// @brief For floating point types, @c true if the type has a representation of a quiet (non-signaling)
  /// "Not-a-Number", otherwise @c false
  // NOLINTNEXTLINE(readability-identifier-naming)
  static constexpr bool has_quiet_NaN{false};
  /// @brief For floating point types, @c true if the type has a representation of a signaling "Not-a-Number", otherwise
  /// @c false
  // NOLINTNEXTLINE(readability-identifier-naming)
  static constexpr bool has_signaling_NaN{false};
  /// @brief For floating point types, an indicator of whether the type has a representation of denormalized values
  static constexpr float_denorm_style has_denorm{denorm_absent};
  /// @brief For floating point types, @c true if loss of accuracy is detected as a denormalization loss rather than an
  /// inexact result, otherwise @c false
  static constexpr bool has_denorm_loss{false};
  /// @brief For floating point types, @c true if the type adheres to the IEC559 standard, otherwise @c false
  static constexpr bool is_iec559{false};
  /// @brief For floating point types, @c true if tinyness is detected before rounding, otherwise @c false
  static constexpr bool tinyness_before{false};
  /// @brief For floating point types, an indicator of the default rounding style
  static constexpr float_round_style round_style{round_toward_zero};

  /// @brief The maximum finite value
  /// @return The maximum value
  static constexpr auto max() noexcept -> bool { return true; }
  /// @brief The minimum finite value; the minimum positive normalized value for floating point types
  /// @return The minimum value
  static constexpr auto min() noexcept -> bool { return false; }
  /// @brief The minimum finite value such that there is no finite value @c y such that @c y<lowest()
  /// @return The minimum value
  static constexpr auto lowest() noexcept -> bool { return min(); }

  /// @brief For floating point types, the epsilon value: the difference between 1 and the least representable value
  /// greater than 1
  /// @return The epsilon value
  static constexpr auto epsilon() noexcept -> bool { return false; }
  /// @brief For floating point types, the maximum rounding error
  /// @return The maximum rounding error
  static constexpr auto round_error() noexcept -> bool { return false; }
  /// @brief For floating point types where @c has_infinity is @c true, a representation of positive infinity
  /// @return Positive infinity
  static constexpr auto infinity() noexcept -> bool { return false; }
  /// @brief For floating point types where @c has_quiet_NaN is @c true, a representation of a quiet NaN
  /// @return A quiet NaN value
  // NOLINTNEXTLINE(readability-identifier-naming)
  static constexpr auto quiet_NaN() noexcept -> bool { return false; }
  /// @brief For floating point types where @c has_signaling_NaN is @c true, a representation of a signaling NaN
  /// @return A signaling NaN value
  // NOLINTNEXTLINE(readability-identifier-naming)
  static constexpr auto signaling_NaN() noexcept -> bool { return false; }
  /// @brief For floating point types, the minimum positive denormalized value. If the representation has no
  /// denormalized values, the minimum positive normalized value.
  /// @return The minimum positive denormalized value
  static constexpr auto denorm_min() noexcept -> bool { return false; }
};

/// @brief Numerator for @c log10(2) as a fraction
///
/// @c log10_2_num/log10_2_den is the same as @c log10(2) to 7 decimal places, which is more than we need to calculate
/// the number of decimal digits
constexpr int log10_2_num{643};
/// @brief Denominator for @c log10(2) as a fraction
constexpr int log10_2_den{2136};

/// @brief Specialization of the implementation template for @c numeric_limits for @c float
template <>
class numeric_limits_impl<float, arene::base::constraints<>> {
 public:
  /// @brief Is this a specialization for which the other values are meaningful? @c true if so, @c false otherwise
  static constexpr bool is_specialized{true};
  /// @brief Is the type an integral type? @c true if so, @c false otherwise
  static constexpr bool is_integer{false};
  /// @brief Is the type an exact type? @c true if so, @c false otherwise
  static constexpr bool is_exact{true};
  /// @brief Is the type a signed type? @c true if so, @c false otherwise
  static constexpr bool is_signed{true};
  /// @brief The base used for the representation.
  static constexpr int radix{FLT_RADIX};
  /// @brief The number of @c radix digits in the representation, ignoring any sign bit
  static constexpr int digits{FLT_MANT_DIG};
  /// @brief The maximum number of base-10  digits that can be completely represented in values of the type
  static constexpr int digits10{FLT_DIG};
  /// @brief @c true if the set of values representable by the type is bounded, otherwise @c false
  static constexpr bool is_bounded{true};
  /// @brief @c true if the set of values representable by the type is modulo, otherwise @c false
  static constexpr bool is_modulo{false};
  /// @brief @c true if there are trap representations of this type, otherwise @c false
  static constexpr bool traps{false};

  /// @brief For floating point types, the number of base-10  digits required to ensure that values which differ can be
  /// differentiated
  static constexpr int max_digits10{1 + ((log10_2_den - 1 + (digits * log10_2_num)) / log10_2_den)};
  /// @brief For floating point types, the minimum negative integer such that @c radix raised to the power of one less
  /// than that integer is a normalized floating point number.
  static constexpr int min_exponent{FLT_MIN_EXP};
  /// @brief For floating point types, the minimum negative integer such that 10 raised to the power of one less than
  /// that integer is in the range of normalized floating point numbers.
  static constexpr int min_exponent10{FLT_MIN_10_EXP};
  /// @brief For floating point types, the maximum positive integer such that @c radix raised to the power of one less
  /// than that integer is a representable finite floating point number.
  static constexpr int max_exponent{FLT_MAX_EXP};
  /// @brief For floating point types, the maximum positive integer such that 10 raised to the power of one less than
  /// that integer is in the range of representable finite floating point numbers.
  static constexpr int max_exponent10{FLT_MAX_10_EXP};
  /// @brief For floating point types, @c true if the type has a representation of positive infinity, otherwise @c false
  static constexpr bool has_infinity{true};
  /// @brief For floating point types, @c true if the type has a representation of a quiet (non-signaling)
  /// "Not-a-Number", otherwise @c false
  // NOLINTNEXTLINE(readability-identifier-naming)
  static constexpr bool has_quiet_NaN{true};
  /// @brief For floating point types, @c true if the type has a representation of a signaling "Not-a-Number", otherwise
  /// @c false
  // NOLINTNEXTLINE(readability-identifier-naming)
  static constexpr bool has_signaling_NaN{true};
  /// @brief For floating point types, an indicator of whether the type has a representation of denormalized values
  static constexpr float_denorm_style has_denorm{denorm_present};
  /// @brief For floating point types, @c true if loss of accuracy is detected as a denormalization loss rather than an
  /// inexact result, otherwise @c false
  static constexpr bool has_denorm_loss{false};
  /// @brief For floating point types, @c true if the type adheres to the IEC559 standard, otherwise @c false
  static constexpr bool is_iec559{true};
  /// @brief For floating point types, @c true if tinyness is detected before rounding, otherwise @c false
  static constexpr bool tinyness_before{false};
  /// @brief For floating point types, an indicator of the default rounding style
  static constexpr float_round_style round_style{round_to_nearest};

  /// @brief The maximum finite value
  /// @return The maximum value
  static constexpr auto max() noexcept -> float { return FLT_MAX; }
  /// @brief The minimum finite value; the minimum positive normalized value for floating point types
  /// @return The minimum value
  static constexpr auto min() noexcept -> float { return FLT_MIN; }
  /// @brief The minimum finite value such that there is no finite value @c y such that @c y<lowest()
  /// @return The minimum value
  static constexpr auto lowest() noexcept -> float { return -FLT_MAX; }

  /// @brief For floating point types, the epsilon value: the difference between 1 and the least representable value
  /// greater than 1
  /// @return The epsilon value
  static constexpr auto epsilon() noexcept -> float { return FLT_EPSILON; }
  /// @brief For floating point types, the maximum rounding error
  /// @return The maximum rounding error
  // NOLINTNEXTLINE(readability-magic-numbers)
  static constexpr auto round_error() noexcept -> float {
    constexpr float rounding_error_value{0.5F};
    return rounding_error_value;
  }
  /// @brief For floating point types where @c has_infinity is @c true, a representation of positive infinity
  /// @return Positive infinity
  static constexpr auto infinity() noexcept -> float { return __builtin_inff(); }
  /// @brief For floating point types where @c has_quiet_NaN is @c true, a representation of a quiet NaN
  /// @return A quiet NaN value
  // NOLINTNEXTLINE(readability-identifier-naming)
  static constexpr auto quiet_NaN() noexcept -> float {
    // parasoft-begin-suppress AUTOSAR-A27_0_4-d "String literal expected for builtin"
    return __builtin_nanf("0");
    // parasoft-end-suppress AUTOSAR-A27_0_4-d
  }
  /// @brief For floating point types where @c has_signaling_NaN is @c true, a representation of a signaling NaN
  /// @return A signaling NaN value
  // NOLINTNEXTLINE(readability-identifier-naming)
  static constexpr auto signaling_NaN() noexcept -> float {
    // parasoft-begin-suppress AUTOSAR-A27_0_4-d "String literal expected for builtin"
    return __builtin_nansf("0");
    // parasoft-end-suppress AUTOSAR-A27_0_4-d
  }
  /// @brief For floating point types, the minimum positive denormalized value. If the representation has no
  /// denormalized values, the minimum positive normalized value.
  /// @return The minimum positive denormalized value
  static constexpr auto denorm_min() noexcept -> float {
    // NOLINTNEXTLINE(google-runtime-int)
    static_assert(FLT_MANT_DIG < (sizeof(unsigned long long) * CHAR_BIT), "Ensure shift doesn't overflow");
    // parasoft-begin-suppress AUTOSAR-A5_2_2-a "False positive: No C-style cast present"
    // parasoft-begin-suppress AUTOSAR-M5_8_1-a "False positive: static_assert used to ensure no overflow"
    // The minimum "normal" has a minimum exponent, an implied leading significant bit of 1 and a zero mantissa field.
    // The minimum "denormal" value has minimum exponent, an implied leading significant bit of 0 and a mantissa field
    // with a 1 in the least significant bit. This is the minimum "normal" value divided by 2 to the power of (1 less
    // than the number of mantissa digits).
    constexpr float min_value{FLT_MIN / static_cast<float>(1ULL << static_cast<unsigned>(FLT_MANT_DIG - 1))};
    // parasoft-end-suppress AUTOSAR-M5_8_1-a
    // parasoft-end-suppress AUTOSAR-A5_2_2-a
    return min_value;
  }
};

/// @brief Specialization of the implementation template for @c numeric_limits for @c double
template <>
class numeric_limits_impl<double, arene::base::constraints<>> {
 public:
  /// @brief Is this a specialization for which the other values are meaningful? @c true if so, @c false otherwise
  static constexpr bool is_specialized{true};
  /// @brief Is the type an integral type? @c true if so, @c false otherwise
  static constexpr bool is_integer{false};
  /// @brief Is the type an exact type? @c true if so, @c false otherwise
  static constexpr bool is_exact{true};
  /// @brief Is the type a signed type? @c true if so, @c false otherwise
  static constexpr bool is_signed{true};
  /// @brief The base used for the representation.
  static constexpr int radix{FLT_RADIX};
  /// @brief The number of @c radix digits in the representation, ignoring any sign bit
  static constexpr int digits{DBL_MANT_DIG};
  /// @brief The maximum number of base-10  digits that can be completely represented in values of the type
  static constexpr int digits10{DBL_DIG};
  /// @brief @c true if the set of values representable by the type is bounded, otherwise @c false
  static constexpr bool is_bounded{true};
  /// @brief @c true if the set of values representable by the type is modulo, otherwise @c false
  static constexpr bool is_modulo{false};
  /// @brief @c true if there are trap representations of this type, otherwise @c false
  static constexpr bool traps{false};

  /// @brief For floating point types, the number of base-10  digits required to ensure that values which differ can be
  /// differentiated
  static constexpr int max_digits10{1 + ((log10_2_den - 1 + (digits * log10_2_num)) / log10_2_den)};
  /// @brief For floating point types, the minimum negative integer such that @c radix raised to the power of one less
  /// than that integer is a normalized floating point number.
  static constexpr int min_exponent{DBL_MIN_EXP};
  /// @brief For floating point types, the minimum negative integer such that 10 raised to the power of one less than
  /// that integer is in the range of normalized floating point numbers.
  static constexpr int min_exponent10{DBL_MIN_10_EXP};
  /// @brief For floating point types, the maximum positive integer such that @c radix raised to the power of one less
  /// than that integer is a representable finite floating point number.
  static constexpr int max_exponent{DBL_MAX_EXP};
  /// @brief For floating point types, the maximum positive integer such that 10 raised to the power of one less than
  /// that integer is in the range of representable finite floating point numbers.
  static constexpr int max_exponent10{DBL_MAX_10_EXP};
  /// @brief For floating point types, @c true if the type has a representation of positive infinity, otherwise @c
  /// false
  static constexpr bool has_infinity{true};
  /// @brief For floating point types, @c true if the type has a representation of a quiet (non-signaling)
  /// "Not-a-Number", otherwise @c false
  // NOLINTNEXTLINE(readability-identifier-naming)
  static constexpr bool has_quiet_NaN{true};
  /// @brief For floating point types, @c true if the type has a representation of a signaling "Not-a-Number",
  /// otherwise
  /// @c false
  // NOLINTNEXTLINE(readability-identifier-naming)
  static constexpr bool has_signaling_NaN{true};
  /// @brief For floating point types, an indicator of whether the type has a representation of denormalized values
  static constexpr float_denorm_style has_denorm{denorm_present};
  /// @brief For floating point types, @c true if loss of accuracy is detected as a denormalization loss rather than an
  /// inexact result, otherwise @c false
  static constexpr bool has_denorm_loss{false};
  /// @brief For floating point types, @c true if the type adheres to the IEC559 standard, otherwise @c false
  static constexpr bool is_iec559{true};
  /// @brief For floating point types, @c true if tinyness is detected before rounding, otherwise @c false
  static constexpr bool tinyness_before{false};
  /// @brief For floating point types, an indicator of the default rounding style
  static constexpr float_round_style round_style{round_to_nearest};

  // parasoft-begin-suppress AUTOSAR-A5_2_2-a "False positive: No C-style cast present"
  /// @brief The maximum finite value
  /// @return The maximum value
  static constexpr auto max() noexcept -> double { return DBL_MAX; }
  // parasoft-end-suppress AUTOSAR-A5_2_2-a

  // parasoft-begin-suppress AUTOSAR-A5_2_2-a "False positive: No C-style cast present"
  /// @brief The minimum finite value; the minimum positive normalized value for floating point types
  /// @return The minimum value
  static constexpr auto min() noexcept -> double { return DBL_MIN; }
  // parasoft-end-suppress AUTOSAR-A5_2_2-a

  // parasoft-begin-suppress AUTOSAR-A5_2_2-a "False positive: No C-style cast present"
  /// @brief The minimum finite value such that there is no finite value @c y such that @c y<lowest()
  /// @return The minimum value
  static constexpr auto lowest() noexcept -> double { return -DBL_MAX; }
  // parasoft-end-suppress AUTOSAR-A5_2_2-a

  // parasoft-begin-suppress AUTOSAR-A5_2_2-a "False positive: No C-style cast present"
  /// @brief For floating point types, the epsilon value: the difference between 1 and the least representable value
  /// greater than 1
  /// @return The epsilon value
  static constexpr auto epsilon() noexcept -> double { return DBL_EPSILON; }
  // parasoft-end-suppress AUTOSAR-A5_2_2-a

  /// @brief For floating point types, the maximum rounding error
  /// @return The maximum rounding error
  // NOLINTNEXTLINE(readability-magic-numbers)
  static constexpr auto round_error() noexcept -> double {
    constexpr double rounding_error_value{0.5};
    return rounding_error_value;
  }
  /// @brief For floating point types where @c has_infinity is @c true, a representation of positive infinity
  /// @return Positive infinity
  static constexpr auto infinity() noexcept -> double { return __builtin_inf(); }
  /// @brief For floating point types where @c has_quiet_NaN is @c true, a representation of a quiet NaN
  /// @return A quiet NaN value
  // NOLINTNEXTLINE(readability-identifier-naming)
  static constexpr auto quiet_NaN() noexcept -> double {
    // parasoft-begin-suppress AUTOSAR-A27_0_4-d "String literal expected for builtin"
    return __builtin_nan("0");
    // parasoft-end-suppress AUTOSAR-A27_0_4-d
  }
  /// @brief For floating point types where @c has_signaling_NaN is @c true, a representation of a signaling NaN
  /// @return A signaling NaN value
  // NOLINTNEXTLINE(readability-identifier-naming)
  static constexpr auto signaling_NaN() noexcept -> double {
    // parasoft-begin-suppress AUTOSAR-A27_0_4-d "String literal expected for builtin"
    return __builtin_nans("0");
    // parasoft-end-suppress AUTOSAR-A27_0_4-d
  }
  /// @brief For floating point types, the minimum positive denormalized value. If the representation has no
  /// denormalized values, the minimum positive normalized value.
  /// @return The minimum positive denormalized value
  static constexpr auto denorm_min() noexcept -> double {
    // NOLINTNEXTLINE(google-runtime-int)
    static_assert(DBL_MANT_DIG < (sizeof(unsigned long long) * CHAR_BIT), "Ensure shift doesn't overflow");
    // parasoft-begin-suppress AUTOSAR-A5_2_2-a "False positive: No C-style cast present"
    // parasoft-begin-suppress AUTOSAR-M5_8_1-a "False positive: static_assert used to ensure no overflow"
    // The minimum "normal" has a minimum exponent, an implied leading significant bit of 1 and a zero mantissa field.
    // The minimum "denormal" value has minimum exponent, an implied leading significant bit of 0 and a mantissa field
    // with a 1 in the least significant bit. This is the minimum "normal" value divided by 2 to the power of (1 less
    // than the number of mantissa digits).
    constexpr double min_value{DBL_MIN / static_cast<double>(1ULL << static_cast<unsigned>(DBL_MANT_DIG - 1))};
    // parasoft-end-suppress AUTOSAR-M5_8_1-a
    // parasoft-end-suppress AUTOSAR-A5_2_2-a
    return min_value;
  }
};

}  // namespace numeric_limits_detail

/// @brief Traits class to provide information about the representation of arithmetic types.
///
/// The primary template is for non-arithmetic types, and provides default values of all the traits, which are zero, @c
/// false, or default-constructed as appropriate. Specializations are provided for all built-in arithmetic types which
/// provide appropriate values of all fields.
///
/// @tparam T The type for which to query information
template <typename T>
class numeric_limits : public numeric_limits_detail::numeric_limits_impl<T> {};

/// @brief Traits class to provide information about the representation of arithmetic types.
///
/// The primary template is for non-arithmetic types, and provides default values of all the traits, which are zero, @c
/// false, or default-constructed as appropriate. Specializations are provided for all built-in arithmetic types which
/// provide appropriate values of all fields.
///
/// @tparam T The type for which to query information
template <typename T>
class numeric_limits<T const> : public numeric_limits_detail::numeric_limits_impl<T> {};

// parasoft-begin-suppress AUTOSAR-A2_11_1-a "Use of volatile required by C++ standard"
/// @brief Traits class to provide information about the representation of arithmetic types.
///
/// The primary template is for non-arithmetic types, and provides default values of all the traits, which are zero, @c
/// false, or default-constructed as appropriate. Specializations are provided for all built-in arithmetic types which
/// provide appropriate values of all fields.
///
/// @tparam T The type for which to query information
template <typename T>
class numeric_limits<T volatile> : public numeric_limits_detail::numeric_limits_impl<T> {};
// parasoft-end-suppress AUTOSAR-A2_11_1-a

// parasoft-begin-suppress AUTOSAR-A2_11_1-a "Use of volatile required by C++ standard"
/// @brief Traits class to provide information about the representation of arithmetic types.
///
/// The primary template is for non-arithmetic types, and provides default values of all the traits, which are zero, @c
/// false, or default-constructed as appropriate. Specializations are provided for all built-in arithmetic types which
/// provide appropriate values of all fields.
///
/// @tparam T The type for which to query information
template <typename T>
class numeric_limits<T const volatile> : public numeric_limits_detail::numeric_limits_impl<T> {};
// parasoft-end-suppress AUTOSAR-A2_11_1-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_NUMERIC_LIMITS_HPP_
