// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_DURATION_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_DURATION_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <chrono>
// IWYU pragma: friend "stdlib_detail/.*"

#include "arene/base/constraints.hpp"
#include "arene/base/math/gcd.hpp"
#include "arene/base/math/lcm.hpp"
#include "arene/base/type_traits/is_instantiation_of.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "stdlib/include/stdlib_detail/common_type.hpp"
#include "stdlib/include/stdlib_detail/cstdint.hpp"
#include "stdlib/include/stdlib_detail/divides.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/equal_to.hpp"
#include "stdlib/include/stdlib_detail/is_convertible.hpp"
#include "stdlib/include/stdlib_detail/is_copy_constructible.hpp"
#include "stdlib/include/stdlib_detail/is_default_constructible.hpp"
#include "stdlib/include/stdlib_detail/is_floating_point.hpp"
#include "stdlib/include/stdlib_detail/less.hpp"
#include "stdlib/include/stdlib_detail/minus.hpp"
#include "stdlib/include/stdlib_detail/modulus.hpp"
#include "stdlib/include/stdlib_detail/multiplies.hpp"
#include "stdlib/include/stdlib_detail/numeric_limits.hpp"
#include "stdlib/include/stdlib_detail/plus.hpp"
#include "stdlib/include/stdlib_detail/ratio.hpp"

namespace std {
namespace chrono {

/// @brief Forward declaration of duration class template
template <typename Rep, typename Period = ratio<1>>
class duration;

}  // namespace chrono

/// @brief Specialization of common_type for two duration types
/// @tparam Rep1 Representation type of the first duration
/// @tparam Period1 Period type of the first duration
/// @tparam Rep2 Representation type of the second duration
/// @tparam Period2 Period type of the second duration
template <typename Rep1, typename Period1, typename Rep2, typename Period2>
class common_type<chrono::duration<Rep1, Period1>, chrono::duration<Rep2, Period2>> {
 private:
  /// @brief The common representation type
  using common_rep = common_type_t<Rep1, Rep2>;

  /// @brief The greatest common divisor of the two periods
  using gcd_period =
      ratio<::arene::base::gcd(Period1::num, Period2::num), ::arene::base::lcm(Period1::den, Period2::den)>;

 public:
  /// @brief The common duration type with the gcd period and common representation
  using type = chrono::duration<common_rep, gcd_period>;
};

namespace chrono {

/// @brief Trait to indicate if a representation type should be treated as floating-point
///
/// This trait determines whether implicit conversions between duration types are allowed.
/// If the representation type is treated as floating-point, implicit conversions that might
/// lose precision are permitted. Otherwise, such conversions are forbidden.
///
/// @tparam Rep The representation type to check
///
/// @note This trait can be specialized by users for custom representation types
template <typename Rep>
class treat_as_floating_point : public is_floating_point<Rep> {};

/// @brief Helper variable template for treat_as_floating_point
/// @tparam Rep The representation type to check
template <typename Rep>
constexpr bool treat_as_floating_point_v = treat_as_floating_point<Rep>::value;

/// @brief A customizable trait to indicate the zero, lowest, and highest values of a given representation type
/// @tparam Rep The representation type
template <typename Rep>
class duration_values {
 public:
  /// @brief Get the zero value of the given Rep type (formally, its additive identity)
  /// @return An instance of the zero value
  static constexpr auto zero() noexcept(is_nothrow_move_constructible_v<Rep>) -> Rep {
    // parasoft-begin-suppress AUTOSAR-A5_2_2-a "The C++14 standard specifies exactly this implementation."
    return Rep(0);  // explicit 0 following [time.traits.duration_values] to avoid uninitialized values
    // parasoft-end-suppress AUTOSAR-A5_2_2-a
  }

  /// @brief Get the lowest value of the given Rep type, which must compare <c><= zero()</c>
  /// @return An instance of the lowest value
  static constexpr auto min() noexcept(noexcept(numeric_limits<Rep>::lowest())) -> Rep {
    return numeric_limits<Rep>::lowest();
  }

  /// @brief Get the highest value of the given Rep type, which must compare <c>> zero()</c>
  /// @return An instance of the highest value
  static constexpr auto max() noexcept(noexcept(numeric_limits<Rep>::max())) -> Rep {
    return numeric_limits<Rep>::max();
  }
};

namespace chrono_detail {

/// @brief Variable template indicating if the given operation is noexcept for the given duration types
/// @tparam BinaryOp Function object representing the operation to check
/// @tparam Duration1 The first duration type
/// @tparam Duration2 The second duration type
template <
    typename BinaryOp,
    typename Duration1,
    typename Duration2,
    typename Common = common_type_t<Duration1, Duration2>>
extern constexpr bool duration_op_noexcept_v =
    is_nothrow_constructible_v<Common, Duration1 const&> && is_nothrow_constructible_v<Common, Duration2 const&> &&
    ::arene::base::is_nothrow_invocable_v<BinaryOp, typename Common::rep const&, typename Common::rep const&>;

/// @brief Variable template indicating if the given operation is noexcept for the given scalar types
/// @tparam BinaryOp Function object representing the operation to check
/// @tparam Rep1 The first scalar type
/// @tparam Rep2 The second scalar type
template <typename BinaryOp, typename Rep1, typename Rep2, typename Common = common_type_t<Rep1, Rep2>>
extern constexpr bool duration_scalar_op_noexcept_v =
    is_nothrow_constructible_v<Common, Rep1 const&> && is_nothrow_constructible_v<Common, Rep2 const&> &&
    ::arene::base::is_nothrow_invocable_v<BinaryOp, Rep1 const&, Rep2 const&>;

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"

/// @brief Helper for duration_cast implementation that handles different conversion cases
/// @tparam ToDuration The target duration type
/// @tparam ConversionFactor The conversion factor ratio
/// @tparam CommonRep The common representation type used for the conversion
/// @tparam Rep The representation type of the source duration
/// @param count The current number of ticks
/// @return The new duration
///
/// Note: This handles the case where ConversionFactor::num == 1 and ConversionFactor::den == 1 (section 2.1 of the C++
/// standard).
template <typename ToDuration, typename ConversionFactor, typename CommonRep, typename Rep>
constexpr auto duration_cast_impl(
    Rep const& count,
    std::true_type /* ConversionFactor::num == 1 */,
    std::true_type /* ConversionFactor::den == 1 */
) noexcept(std::is_nothrow_constructible_v<ToDuration, typename ToDuration::rep>) -> ToDuration {
  return ToDuration{static_cast<typename ToDuration::rep>(static_cast<CommonRep>(count))};
}

/// @brief Helper for duration_cast implementation that handles different conversion cases
/// @tparam ToDuration The target duration type
/// @tparam ConversionFactor The conversion factor ratio
/// @tparam CommonRep The common representation type used for the conversion
/// @tparam Rep The representation type of the source duration
/// @param count The current number of ticks
/// @return The new duration
///
/// Note: This handles the case where ConversionFactor::num != 1 and ConversionFactor::den == 1 (section 2.2 of the C++
/// standard).
template <typename ToDuration, typename ConversionFactor, typename CommonRep, typename Rep>
constexpr auto duration_cast_impl(
    Rep const& count,
    std::false_type /* ConversionFactor::num != 1 */,
    std::true_type /* ConversionFactor::den == 1 */
) noexcept(noexcept(std::declval<CommonRep const&>() * std::declval<CommonRep const&>()) && //
           std::is_nothrow_constructible_v<ToDuration, typename ToDuration::rep>)
    -> ToDuration {
  return ToDuration{static_cast<typename ToDuration::rep>(
      static_cast<CommonRep>(count) * static_cast<CommonRep>(ConversionFactor::num)
  )};
}

/// @brief Helper for duration_cast implementation that handles different conversion cases
/// @tparam ToDuration The target duration type
/// @tparam ConversionFactor The conversion factor ratio
/// @tparam CommonRep The common representation type used for the conversion
/// @tparam Rep The representation type of the source duration
/// @param count The current number of ticks
/// @return The new duration
///
/// Note: This handles the case where ConversionFactor::num == 1 and ConversionFactor::den != 1 (section 2.3 of the C++
/// standard).
template <typename ToDuration, typename ConversionFactor, typename CommonRep, typename Rep>
constexpr auto duration_cast_impl(
    Rep const& count,
    std::true_type /* ConversionFactor::num == 1 */,
    std::false_type /* ConversionFactor::den != 1 */
) noexcept(noexcept(std::declval<CommonRep const&>() / std::declval<CommonRep const&>()) &&  //
           std::is_nothrow_constructible_v<ToDuration, typename ToDuration::rep>)
    -> ToDuration {
  return ToDuration{static_cast<typename ToDuration::rep>(
      static_cast<CommonRep>(count) / static_cast<CommonRep>(ConversionFactor::den)
  )};
}

/// @brief Helper for duration_cast implementation that handles different conversion cases
/// @tparam ToDuration The target duration type
/// @tparam ConversionFactor The conversion factor ratio
/// @tparam CommonRep The common representation type used for the conversion
/// @tparam Rep The representation type of the source duration
/// @param count The current number of ticks
/// @return The new duration
///
/// Note: This handles the default case where ConversionFactor::num != 1 and ConversionFactor::den != 1 (section 2.4 of
/// the C++ standard).
template <typename ToDuration, typename ConversionFactor, typename CommonRep, typename Rep>
constexpr auto duration_cast_impl(
    Rep const& count,
    std::false_type /* ConversionFactor::num != 1 */,
    std::false_type /* ConversionFactor::den != 1 */
) noexcept(noexcept(std::declval<CommonRep const&>() / std::declval<CommonRep const&>()) &&  //
           noexcept(std::declval<CommonRep const&>() * std::declval<CommonRep const&>()) &&  //
           std::is_nothrow_constructible_v<ToDuration, typename ToDuration::rep>)
    -> ToDuration {
  return ToDuration{static_cast<typename ToDuration::rep>(
      static_cast<CommonRep>(count) * static_cast<CommonRep>(ConversionFactor::num) /
      static_cast<CommonRep>(ConversionFactor::den)
  )};
}

// parasoft-end-suppress CERT_C-EXP37-a-3

/// @brief Helper to detect if duration conversion would cause overflow
/// @tparam FromPeriod The period of the source duration
/// @tparam ToPeriod The period of the target duration
template <typename FromPeriod, typename ToPeriod>
class check_conversion_overflow {
 private:
  // parasoft-begin-suppress AUTOSAR-M0_1_3-c "False positive: gcd_num and gcd_den are used."
  /// @brief The greatest common divisor of the numerators of the two periods
  static constexpr auto gcd_num = ::arene::base::gcd(FromPeriod::num, ToPeriod::num);

  /// @brief The greatest common divisor of the denominators of the two periods
  static constexpr auto gcd_den = ::arene::base::gcd(FromPeriod::den, ToPeriod::den);
  // parasoft-end-suppress AUTOSAR-M0_1_3-c

  /// @brief The @c FromPeriod reduced by the greatest common divisors
  using reduced_from_period = ratio<FromPeriod::num / gcd_num, FromPeriod::den / gcd_den>;

  /// @brief The @c ToPeriod reduced by the greatest common divisors
  using reduced_to_period = ratio<ToPeriod::num / gcd_num, ToPeriod::den / gcd_den>;

  /// @brief The max representable value
  static constexpr auto max = numeric_limits<intmax_t>::max();

 public:
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public
  // property,
  /// @brief True if @c FromPeriod can be converted to @c ToPeriod without risk of overflow.
  static constexpr auto value = reduced_from_period::num <= (max / reduced_to_period::den) &&
                                reduced_to_period::num <= (max / reduced_from_period::den);
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2
};

}  // namespace chrono_detail

/// @brief Convert a duration to a different duration type
/// @tparam ToDuration The target duration type to convert to
/// @tparam Rep The representation type of the source duration
/// @tparam Period The period type of the source duration
/// @param duration The source duration to convert
/// @return A duration of type ToDuration converted from the source duration
///
/// @note This function only participates in overload resolution if ToDuration is an instantiation of duration
template <
    typename ToDuration,
    typename Rep,
    typename Period,
    arene::base::constraints<enable_if_t<::arene::base::is_instantiation_of_v<ToDuration, duration>>> = nullptr,
    typename ConversionFactor = ratio_divide<Period, typename ToDuration::period>,
    typename CommonRep = std::common_type_t<typename ToDuration::rep, Rep, intmax_t>>
constexpr auto duration_cast(duration<Rep, Period> const& duration) noexcept(
    noexcept(duration.count()) && noexcept(chrono_detail::duration_cast_impl<ToDuration, ConversionFactor, CommonRep>(
        duration.count(),
        std::bool_constant<ConversionFactor::num == 1>{},
        std::bool_constant<ConversionFactor::den == 1>{}
    ))
) -> ToDuration {
  return chrono_detail::duration_cast_impl<ToDuration, ConversionFactor, CommonRep>(
      duration.count(),
      std::bool_constant<ConversionFactor::num == 1>{},
      std::bool_constant<ConversionFactor::den == 1>{}
  );
}

// parasoft-begin-suppress AUTOSAR-A14_5_1-a "False positive: The single
// argument template constructor of 'duration' does not hide copy/move
// constructors via use of SFINAE."
// parasoft-begin-suppress AUTOSAR-A12_1_5-a "False positive: delegating constructors would not reduce duplication"
// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"

/// @brief A time interval represented as a count of ticks of a given period
///
/// The duration class template represents a time interval. It consists of a count of ticks
/// of type Rep and a tick period, where the tick period is a compile-time rational fraction
/// representing the number of seconds from one tick to the next.
///
/// @tparam Rep An arithmetic type or a class emulating an arithmetic type, representing the
///             type of the tick count. Must not be a specialization of duration.
/// @tparam Period A specialization of std::ratio, representing the tick period in seconds.
///                Must be positive. Defaults to std::ratio<1> (1 second).
template <typename Rep, typename Period>
class duration {
  static_assert(
      !::arene::base::is_instantiation_of_v<Rep, duration>,
      "Rep must not be a specialization of std::chrono::duration"
  );
  static_assert(ratio_detail::is_ratio<Period>::value, "Period must be a specialization of std::ratio");
  static_assert(Period::num > 0, "Period must be positive");

 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: these declarations are scoped and do not hide anything"
  /// @brief The arithmetic type representing the number of ticks
  using rep = Rep;
  /// @brief The period type representing the tick period in seconds
  using period = Period;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  /// @brief Default constructor
  /// Initializes the duration with a zero tick count
  constexpr duration() noexcept(is_nothrow_default_constructible_v<rep>)
      : rep_{} {}

  /// @brief Constructor from a representation value
  /// @tparam Rep2 The type of the input representation value, must be convertible to Rep
  /// @param rep_in The tick count value to initialize the duration with
  template <
      typename Rep2,
      arene::base::constraints<
          enable_if_t<is_convertible_v<Rep2 const&, rep>>,
          enable_if_t<treat_as_floating_point_v<rep> || !treat_as_floating_point_v<Rep2>>> = nullptr>
  constexpr explicit duration(Rep2 const& rep_in
  ) noexcept(is_nothrow_copy_constructible_v<rep> && is_nothrow_constructible_v<rep, Rep2 const&>)
      : rep_{static_cast<rep>(rep_in)} {}

  /// @brief Constructor from another duration type
  /// @tparam Rep2 The representation type of the source duration
  /// @tparam Period2 The period type of the source duration
  /// @param other The source duration to convert from
  ///
  /// Note: The standard specifies that "This constructor shall not participate in overload resolution unless no
  /// overflow is induced in the conversion". It is unclear what overflow should be checked, and different standard
  /// library implementations take different approaches. This implementation aims for compatibility with the
  /// approaches taken by libc++ and libstdc++.
  template <
      typename Rep2,
      typename Period2,
      arene::base::constraints<
          enable_if_t<is_convertible_v<Rep2 const&, rep>>,
          enable_if_t<chrono_detail::check_conversion_overflow<Period2, period>::value>,
          enable_if_t<
              treat_as_floating_point_v<rep> ||
              (ratio_divide<Period2, period>::den == 1 && !treat_as_floating_point_v<Rep2>)>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr duration(duration<Rep2, Period2> const& other
  ) noexcept(noexcept(duration_cast<duration>(std::declval<duration<Rep2, Period2> const&>()).count()) && //
             is_nothrow_constructible_v<rep, decltype(duration_cast<duration>(
                     std::declval<duration<Rep2, Period2> const&>()).count())>)
      : rep_{duration_cast<duration>(other).count()} {}

  /// @brief Returns the tick count
  /// @return The tick count as a value of type rep
  constexpr auto count() const noexcept(is_nothrow_copy_constructible_v<rep>) -> rep { return rep_; }

  /// @brief Unary plus operator
  /// @return A copy of this duration
  constexpr auto operator+() const noexcept(is_nothrow_copy_constructible_v<duration>) -> duration { return *this; }

  /// @brief Unary minus operator
  /// @return A duration with negated representation
  constexpr auto operator-() const noexcept(is_nothrow_constructible_v<duration, rep>) -> duration {
    return duration{-rep_};
  }

  /// @brief Pre-increment operator
  /// @return Reference to this duration after incrementing
  constexpr auto operator++() noexcept(noexcept(++declval<rep&>())) -> duration& {
    ++rep_;
    return *this;
  }

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  // parasoft-begin-suppress AUTOSAR-A3_9_1-b "False positive: postincrement requires an int parameter"
  // parasoft-begin-suppress AUTOSAR-M5_2_10-a "These operator definitions are written to follow the standard
  // specification exactly."

  /// @brief Post-increment operator
  /// @return Copy of this duration before incrementing
  constexpr auto operator++(int) noexcept(is_nothrow_copy_constructible_v<duration>&& noexcept(++declval<rep&>()))
      -> duration {
    return duration{rep_++};
  }

  /// @brief Pre-decrement operator
  /// @return Reference to this duration after decrementing
  constexpr auto operator--() noexcept(noexcept(--declval<rep&>())) -> duration& {
    --rep_;
    return *this;
  }

  /// @brief Post-decrement operator
  /// @return Copy of this duration before decrementing
  constexpr auto operator--(int) noexcept(is_nothrow_copy_constructible_v<duration>&& noexcept(--declval<rep&>()))
      -> duration {
    return duration{rep_--};
  }

  // parasoft-end-suppress AUTOSAR-A3_9_1-b
  // parasoft-end-suppress CERT_C-EXP37-a-3
  // parasoft-begin-suppress AUTOSAR-M5_2_10-a "These operator definitions are written to follow the standard
  // specification exactly."

  /// @brief Compound addition assignment operator
  /// @param rhs The duration to add
  /// @return Reference to this duration after addition
  constexpr auto operator+=(duration const& rhs) noexcept(noexcept(declval<rep&>() += rhs.count())) -> duration& {
    rep_ += rhs.count();
    return *this;
  }

  /// @brief Compound subtraction assignment operator
  /// @param rhs The duration to subtract
  /// @return Reference to this duration after subtraction
  constexpr auto operator-=(duration const& rhs) noexcept(noexcept(declval<rep&>() -= rhs.count())) -> duration& {
    rep_ -= rhs.count();
    return *this;
  }

  /// @brief Compound multiplication assignment operator
  /// @param rhs The scalar to multiply by
  /// @return Reference to this duration after multiplication
  constexpr auto operator*=(rep const& rhs) noexcept(noexcept(declval<rep&>() *= rhs)) -> duration& {
    rep_ *= rhs;
    return *this;
  }

  /// @brief Compound division assignment operator
  /// @param rhs The scalar to divide by
  /// @return Reference to this duration after division
  constexpr auto operator/=(rep const& rhs) noexcept(noexcept(declval<rep&>() /= rhs)) -> duration& {
    rep_ /= rhs;
    return *this;
  }

  /// @brief Compound modulo assignment operator with scalar
  /// @param rhs The scalar to compute modulo with
  /// @return Reference to this duration after modulo operation
  constexpr auto operator%=(rep const& rhs) noexcept(noexcept(declval<rep&>() %= rhs)) -> duration& {
    rep_ %= rhs;
    return *this;
  }

  /// @brief Compound modulo assignment operator with duration
  /// @param rhs The duration to compute modulo with
  /// @return Reference to this duration after modulo operation
  constexpr auto operator%=(duration const& rhs) noexcept(noexcept(declval<rep&>() %= rhs.count())) -> duration& {
    rep_ %= rhs.count();
    return *this;
  }

  /// @brief Get a duration of length zero, corresponding to no time passing
  /// @return A duration of length zero
  static constexpr auto zero() noexcept(noexcept(duration(duration_values<rep>::zero()))) -> duration {
    return duration(duration_values<rep>::zero());
  }

  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: these declarations are scoped and do not hide anything"
  /// @brief Get a minimally positive duration (for signed representations, a maximally negative duration)
  /// @return A minimal duration
  static constexpr auto min() noexcept(noexcept(duration(duration_values<rep>::min()))) -> duration {
    return duration(duration_values<rep>::min());
  }

  /// @brief Get a maximally positive duration
  /// @return A maximal duration
  static constexpr auto max() noexcept(noexcept(duration(duration_values<rep>::max()))) -> duration {
    return duration(duration_values<rep>::max());
  }
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

 private:
  /// @brief The current number of ticks
  rep rep_;
};

// parasoft-end-suppress AUTOSAR-A14_5_1-a
// parasoft-end-suppress AUTOSAR-A12_1_5-a
// parasoft-end-suppress AUTOSAR-M2_10_1-a

// parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Mixed comparisons permitted by A13-5-5 Permit #1. These also cannot be
// unconditionally noexcept because we do not control the rep type."
// parasoft-begin-suppress AUTOSAR-M6_2_2-a "This comparison is defined by the standard."

/// @brief Equality comparison operator for duration types
/// @tparam Rep1 Representation type of the first duration
/// @tparam Period1 Period type of the first duration
/// @tparam Rep2 Representation type of the second duration
/// @tparam Period2 Period type of the second duration
/// @param lhs Left-hand side duration
/// @param rhs Right-hand side duration
/// @return True if the durations represent the same time interval, false otherwise
template <typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr auto operator==(
    duration<Rep1, Period1> const& lhs,
    duration<Rep2, Period2> const& rhs
) noexcept(chrono_detail::duration_op_noexcept_v<equal_to<>, duration<Rep1, Period1>, duration<Rep2, Period2>>)
    -> bool {
  using common_type = std::common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;
  return common_type{lhs}.count() == common_type{rhs}.count();
}

// parasoft-end-suppress AUTOSAR-M6_2_2-a

/// @brief Inequality comparison operator for duration types
/// @tparam Rep1 Representation type of the first duration
/// @tparam Period1 Period type of the first duration
/// @tparam Rep2 Representation type of the second duration
/// @tparam Period2 Period type of the second duration
/// @param lhs Left-hand side duration
/// @param rhs Right-hand side duration
/// @return True if the durations represent different time intervals, false otherwise
template <typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr auto operator!=(duration<Rep1, Period1> const& lhs, duration<Rep2, Period2> const& rhs) noexcept(
    noexcept(lhs == rhs)
) -> bool {
  return !(lhs == rhs);
}

/// @brief Less-than comparison operator for duration types
/// @tparam Rep1 Representation type of the first duration
/// @tparam Period1 Period type of the first duration
/// @tparam Rep2 Representation type of the second duration
/// @tparam Period2 Period type of the second duration
/// @param lhs Left-hand side duration
/// @param rhs Right-hand side duration
/// @return True if lhs represents a shorter time interval than rhs, false otherwise
template <typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr auto operator<(
    duration<Rep1, Period1> const& lhs,
    duration<Rep2, Period2> const& rhs
) noexcept(chrono_detail::duration_op_noexcept_v<less<>, duration<Rep1, Period1>, duration<Rep2, Period2>>) -> bool {
  using common_type = std::common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;
  return common_type{lhs}.count() < common_type{rhs}.count();
}

/// @brief Less-than-or-equal comparison operator for duration types
/// @tparam Rep1 Representation type of the first duration
/// @tparam Period1 Period type of the first duration
/// @tparam Rep2 Representation type of the second duration
/// @tparam Period2 Period type of the second duration
/// @param lhs Left-hand side duration
/// @param rhs Right-hand side duration
/// @return True if lhs represents a shorter or equal time interval than rhs, false otherwise
template <typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr auto operator<=(duration<Rep1, Period1> const& lhs, duration<Rep2, Period2> const& rhs) noexcept(
    noexcept(rhs < lhs)
) -> bool {
  return !(rhs < lhs);
}

/// @brief Greater-than comparison operator for duration types
/// @tparam Rep1 Representation type of the first duration
/// @tparam Period1 Period type of the first duration
/// @tparam Rep2 Representation type of the second duration
/// @tparam Period2 Period type of the second duration
/// @param lhs Left-hand side duration
/// @param rhs Right-hand side duration
/// @return True if lhs represents a longer time interval than rhs, false otherwise
template <typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr auto operator>(duration<Rep1, Period1> const& lhs, duration<Rep2, Period2> const& rhs) noexcept(
    noexcept(rhs < lhs)
) -> bool {
  return rhs < lhs;
}

/// @brief Greater-than-or-equal comparison operator for duration types
/// @tparam Rep1 Representation type of the first duration
/// @tparam Period1 Period type of the first duration
/// @tparam Rep2 Representation type of the second duration
/// @tparam Period2 Period type of the second duration
/// @param lhs Left-hand side duration
/// @param rhs Right-hand side duration
/// @return True if lhs represents a longer or equal time interval than rhs, false otherwise
template <typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr auto operator>=(duration<Rep1, Period1> const& lhs, duration<Rep2, Period2> const& rhs) noexcept(
    noexcept(lhs < rhs)
) -> bool {
  return !(lhs < rhs);
}

// parasoft-end-suppress AUTOSAR-A13_5_5-b-2

// parasoft-begin-suppress AUTOSAR-M5_17_1-a "These operator definitions are written to follow the standard
// specification exactly."

/// @brief Addition operator for duration types
/// @tparam Rep1 Representation type of the first duration
/// @tparam Period1 Period type of the first duration
/// @tparam Rep2 Representation type of the second duration
/// @tparam Period2 Period type of the second duration
/// @param lhs Left-hand side duration
/// @param rhs Right-hand side duration
/// @return A duration representing the sum of the two durations
template <typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr auto operator+(
    duration<Rep1, Period1> const& lhs,
    duration<Rep2, Period2> const& rhs
) noexcept(chrono_detail::duration_op_noexcept_v<plus<>, duration<Rep1, Period1>, duration<Rep2, Period2>>)
    -> std::common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>> {
  using common_duration = std::common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;
  return common_duration{common_duration{lhs}.count() + common_duration{rhs}.count()};
}

/// @brief Subtraction operator for duration types
/// @tparam Rep1 Representation type of the first duration
/// @tparam Period1 Period type of the first duration
/// @tparam Rep2 Representation type of the second duration
/// @tparam Period2 Period type of the second duration
/// @param lhs Left-hand side duration
/// @param rhs Right-hand side duration
/// @return A duration representing the difference of the two durations
template <typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr auto operator-(
    duration<Rep1, Period1> const& lhs,
    duration<Rep2, Period2> const& rhs
) noexcept(chrono_detail::duration_op_noexcept_v<minus<>, duration<Rep1, Period1>, duration<Rep2, Period2>>)
    -> std::common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>> {
  using common_duration = std::common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;
  return common_duration{common_duration{lhs}.count() - common_duration{rhs}.count()};
}

/// @brief Multiplication operator for duration and scalar
/// @tparam Rep1 Representation type of the duration
/// @tparam Period Period type of the duration
/// @tparam Rep2 Type of the scalar
/// @param dur The duration
/// @param scalar The scalar
/// @return A duration representing the duration multiplied by the scalar
template <
    typename Rep1,
    typename Period,
    typename Rep2,
    arene::base::constraints<enable_if_t<is_convertible_v<Rep2, std::common_type_t<Rep1, Rep2>>>> = nullptr>
constexpr auto operator*(
    duration<Rep1, Period> const& dur,
    Rep2 const& scalar
) noexcept(chrono_detail::duration_scalar_op_noexcept_v<multiplies<>, Rep1, Rep2>)
    -> duration<std::common_type_t<Rep1, Rep2>, Period> {
  using common_rep = std::common_type_t<Rep1, Rep2>;
  return duration<common_rep, Period>{common_rep{dur.count()} * common_rep{scalar}};
}

/// @brief Multiplication operator for scalar and duration
/// @tparam Rep1 Type of the scalar
/// @tparam Rep2 Representation type of the duration
/// @tparam Period Period type of the duration
/// @param scalar The scalar
/// @param dur The duration
/// @return A duration representing the scalar multiplied by the duration
template <
    typename Rep1,
    typename Rep2,
    typename Period,
    arene::base::constraints<enable_if_t<is_convertible_v<Rep1, std::common_type_t<Rep1, Rep2>>>> = nullptr>
constexpr auto operator*(Rep1 const& scalar, duration<Rep2, Period> const& dur) noexcept(noexcept(dur * scalar))
    -> duration<std::common_type_t<Rep1, Rep2>, Period> {
  return dur * scalar;
}

/// @brief Division operator for duration and scalar
/// @tparam Rep1 Representation type of the duration
/// @tparam Period Period type of the duration
/// @tparam Rep2 Type of the scalar
/// @param dur The duration
/// @param scalar The scalar
/// @return A duration representing the duration divided by the scalar
template <
    typename Rep1,
    typename Period,
    typename Rep2,
    arene::base::constraints<
        enable_if_t<is_convertible_v<Rep2, std::common_type_t<Rep1, Rep2>>>,
        enable_if_t<!::arene::base::is_instantiation_of_v<Rep2, duration>>> = nullptr>
constexpr auto operator/(
    duration<Rep1, Period> const& dur,
    Rep2 const& scalar
) noexcept(chrono_detail::duration_scalar_op_noexcept_v<divides<>, Rep1, Rep2>)
    -> duration<std::common_type_t<Rep1, Rep2>, Period> {
  using common_rep = std::common_type_t<Rep1, Rep2>;
  return duration<common_rep, Period>{common_rep{dur.count()} / common_rep{scalar}};
}

/// @brief Division operator for two durations
/// @tparam Rep1 Representation type of the first duration
/// @tparam Period1 Period type of the first duration
/// @tparam Rep2 Representation type of the second duration
/// @tparam Period2 Period type of the second duration
/// @param lhs Left-hand side duration
/// @param rhs Right-hand side duration
/// @return The ratio of the two durations as a scalar
template <typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr auto operator/(
    duration<Rep1, Period1> const& lhs,
    duration<Rep2, Period2> const& rhs
) noexcept(chrono_detail::duration_op_noexcept_v<divides<>, duration<Rep1, Period1>, duration<Rep2, Period2>>)
    -> std::common_type_t<Rep1, Rep2> {
  using common_duration = std::common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;
  return std::common_type_t<Rep1, Rep2>{common_duration{lhs}.count() / common_duration{rhs}.count()};
}

/// @brief Modulo operator for duration and scalar
/// @tparam Rep1 Representation type of the duration
/// @tparam Period Period type of the duration
/// @tparam Rep2 Type of the scalar
/// @param dur The duration
/// @param scalar The scalar
/// @return A duration representing the duration modulo the scalar
template <
    typename Rep1,
    typename Period,
    typename Rep2,
    arene::base::constraints<
        enable_if_t<is_convertible_v<Rep2, std::common_type_t<Rep1, Rep2>>>,
        enable_if_t<!::arene::base::is_instantiation_of_v<Rep2, duration>>> = nullptr>
constexpr auto operator%(
    duration<Rep1, Period> const& dur,
    Rep2 const& scalar
) noexcept(chrono_detail::duration_scalar_op_noexcept_v<modulus<>, Rep1, Rep2>)
    -> duration<std::common_type_t<Rep1, Rep2>, Period> {
  using common_rep = std::common_type_t<Rep1, Rep2>;
  return duration<common_rep, Period>{common_rep{dur.count()} % common_rep{scalar}};
}

/// @brief Modulo operator for two durations
/// @tparam Rep1 Representation type of the first duration
/// @tparam Period1 Period type of the first duration
/// @tparam Rep2 Representation type of the second duration
/// @tparam Period2 Period type of the second duration
/// @param lhs Left-hand side duration
/// @param rhs Right-hand side duration
/// @return A duration representing the first duration modulo the second
template <typename Rep1, typename Period1, typename Rep2, typename Period2>
constexpr auto operator%(
    duration<Rep1, Period1> const& lhs,
    duration<Rep2, Period2> const& rhs
) noexcept(chrono_detail::duration_op_noexcept_v<modulus<>, duration<Rep1, Period1>, duration<Rep2, Period2>>)
    -> std::common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>> {
  using common_duration = std::common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;
  return common_duration{common_duration{lhs}.count() % common_duration{rhs}.count()};
}

// parasoft-end-suppress AUTOSAR-M5_17_1-a

/// @brief Nanoseconds convenience type alias
using nanoseconds = duration<std::int64_t, std::nano>;

/// @brief Microseconds convenience type alias
using microseconds = duration<std::int64_t, std::micro>;

/// @brief Milliseconds convenience type alias
using milliseconds = duration<std::int64_t, std::milli>;

/// @brief Seconds convenience type alias
using seconds = duration<std::int64_t>;

// NOLINTBEGIN(readability-magic-numbers) These are named constants to replace users' magic numbers
/// @brief Seconds convenience type alias
using minutes = duration<std::int32_t, std::ratio<60>>;

/// @brief Hours convenience type alias
using hours = duration<std::int32_t, std::ratio<3600>>;
// NOLINTEND(readability-magic-numbers)

}  // namespace chrono
}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_DURATION_HPP_
