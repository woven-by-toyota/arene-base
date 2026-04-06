// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_QUANTITY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_QUANTITY_HPP_

// IWYU pragma: private, include "arene/base/units.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

#include "arene/base/compare/compare_three_way.hpp"
#include "arene/base/compare/operators.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/math/abs.hpp"
#include "arene/base/math/gcd.hpp"
#include "arene/base/stdlib_choice/common_type.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/is_signed.hpp"
#include "arene/base/stdlib_choice/is_unsigned.hpp"
#include "arene/base/stdlib_choice/make_unsigned.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/stdlib_choice/ratio.hpp"
#include "arene/base/type_traits/always_false.hpp"
#include "arene/base/units/units_conversion_traits.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

// IWYU pragma: begin_keep
template <typename QuantityType, typename Rep = double>
class quantity;
// IWYU pragma: end_keep

namespace quantity_detail {
/// @brief Boolean variable that is @c true if the supplied type is an instantiation of @c quantity, @c false otherwise
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_quantity_v = false;

/// @brief Specialization of a boolean variable for the case where the supplied type is an instantiation of @c quantity.
/// Always @c true
/// @tparam QuantityType The physical unit type for the quantity
/// @tparam Rep The representation type for the quantity
template <typename QuantityType, typename Rep>
extern constexpr bool is_quantity_v<quantity<QuantityType, Rep>> = true;

}  // namespace quantity_detail

/// @brief Traits class with information about if and how a physical unit of type @c QuantityType1 can be combined with
/// a physical unit of type @c QuantityType2.
///
/// The physical unit types themselves are just tags used for the @c quantity class template rather than holding any
/// values directly.
///
/// By default only addition and subtraction of identical quantity types are permitted.
///
/// Specializations may provide a @c sum_type type alias, specifying the physical unit type for the result of adding a
/// quantity with the a physical unit type of @c QuantityType1 to a quantity with a physical unit type of @c
/// QuantityType2
///
/// Specializations may provide a @c difference_type type alias, specifying the physical unit type for the result of
/// subtracting a quantity with the a physical unit type of @c QuantityType2 from a quantity with a physical unit type
/// of @c QuantityType1
///
/// Specializations may provide a @c product_type type alias, specifying the physical unit type for the result of
/// multiplying a quantity with the a physical unit type of @c QuantityType1 by a quantity with a physical unit type of
/// @c QuantityType2
///
/// Specializations may provide a @c ratio_type type alias, specifying the physical unit type for the result of
/// dividing a quantity with the a physical unit type of @c QuantityType1 by a quantity with a physical unit type of
/// @c QuantityType2
///
/// @tparam QuantityType1 The quantity type for the left-hand operand of an operation
/// @tparam QuantityType2 The quantity type for the righ-hand operand of an operation
template <typename QuantityType1, typename QuantityType2>
struct units_combination_traits {};

/// @brief Specialization to allow addition and subtraction of a quantity type from itself
/// @tparam QuantityType The quantity type for the operands of an operation
template <typename QuantityType>
struct units_combination_traits<QuantityType, QuantityType> {
  /// @brief The physical quantity type resulting from adding quantities with a physical quantity type of @c
  /// QuantityType
  using sum_type = QuantityType;
  /// @brief The physical quantity type resulting from subtracting quantities with a physical quantity type of @c
  /// QuantityType
  using difference_type = QuantityType;
};

namespace quantity_detail {
/// @brief Obtain the absolute value of a signed integer, as the corresponding unsigned type, thus allowing the absolute
/// value of the negative limit to be represented.
/// @tparam T The type of the integer
/// @param value The value for which the absolute value is required
/// @return std::make_unsigned_t<T> holding the absolute value of the argument
template <typename T, constraints<std::enable_if_t<std::is_signed<T>::value>> = nullptr>
constexpr auto unsigned_abs(T value) noexcept -> std::make_unsigned_t<T> {
  if (value == std::numeric_limits<T>::min()) {
    constexpr std::make_unsigned_t<T> abs_min{static_cast<std::make_unsigned_t<T>>(std::numeric_limits<T>::max()) + 1U};
    return abs_min;
  }
  return static_cast<std::make_unsigned_t<T>>(arene::base::abs(value));
}

/// @brief Obtain the absolute value of an unsigned integer, which is just the identity operation
/// @tparam T The type of the integer
/// @param value The value for which the absolute value is required
/// @return T @c value
template <typename T, constraints<std::enable_if_t<std::is_unsigned<T>::value>> = nullptr>
constexpr auto unsigned_abs(T value) noexcept -> T {
  return value;
}

/// @brief Overload of @c scale_value_by for scaling a value into an unsigned integer target by a signed scale factor,
/// which causes a compilation error
/// @tparam ScaleFactor The scale factor
/// @tparam TargetRep The target representation
/// @tparam Rep The source representation
/// @param value The value to scale
/// @return TargetRep Never returns due to compilation error triggered by @c static_assert
template <
    typename ScaleFactor,
    typename TargetRep,
    typename Rep,
    constraints<
        std::enable_if_t<std::is_unsigned<TargetRep>::value>,
        std::enable_if_t<((ScaleFactor::num < 0) != (ScaleFactor::den < 0))>> = nullptr>
constexpr auto scale_value_by(Rep value) noexcept -> TargetRep {
  constexpr bool unsigned_scaled_by_positive_factor = always_false_v<ScaleFactor>;
  static_assert(
      unsigned_scaled_by_positive_factor,
      "Scaling a value by a negative scale factor may yield a negative result that cannot be represented in "
      "an unsigned field"
  );
  return static_cast<TargetRep>(value);
}

/// @brief Overload of @c scale_value_by for scaling a value by a positive scale factor into an unsigned integer target
/// representation, where the value may overflow the target representation, which causes a compilation error
/// @tparam ScaleFactor The scale factor
/// @tparam TargetRep The target representation
/// @tparam Rep The source representation
/// @param value The value to scale
/// @return TargetRep Never returns due to compilation error triggered by @c static_assert
template <
    typename ScaleFactor,
    typename TargetRep,
    typename Rep,
    constraints<
        std::enable_if_t<std::is_unsigned<TargetRep>::value>,  //
        std::enable_if_t<std::is_integral<Rep>::value>,
        std::enable_if_t<
            (static_cast<std::uint64_t>(std::numeric_limits<Rep>::max()) >
             (std::numeric_limits<std::uint64_t>::max() / unsigned_abs(ScaleFactor::num)))>,
        std::enable_if_t<((ScaleFactor::num < 0) == (ScaleFactor::den < 0))>> = nullptr>
constexpr auto scale_value_by(Rep value) noexcept -> TargetRep {
  constexpr bool scaling_calculation_does_not_overflow = always_false_v<ScaleFactor>;
  static_assert(
      scaling_calculation_does_not_overflow,
      "The scaling calculation will overflow internally; please use a floating point representation"
  );
  return static_cast<TargetRep>(value);
}

/// @brief Scale an integral value by a positive scale factor into an unsigned integer target representation, where the
/// value is guaranteed not to overflow the target representation
/// @tparam ScaleFactor The scale factor
/// @tparam TargetRep The target representation
/// @tparam Rep The source representation
/// @param value The value to scale
/// @return TargetRep @c value scaled by @c ScaleFactor
template <
    typename ScaleFactor,
    typename TargetRep,
    typename Rep,
    constraints<
        std::enable_if_t<std::is_unsigned<TargetRep>::value>,  //
        std::enable_if_t<std::is_integral<Rep>::value>,        //
        std::enable_if_t<ScaleFactor::den != 0>,
        std::enable_if_t<
            (static_cast<std::uint64_t>(std::numeric_limits<Rep>::max()) <=
             (std::numeric_limits<std::uint64_t>::max() / unsigned_abs(ScaleFactor::num)))>,
        std::enable_if_t<((ScaleFactor::num < 0) == (ScaleFactor::den < 0))>> = nullptr>
constexpr auto scale_value_by(Rep value) noexcept -> TargetRep {
  if (value == Rep{}) {
    return static_cast<TargetRep>(value);
  }
  std::uintmax_t const initial_value{value};
  constexpr auto unsigned_den = quantity_detail::unsigned_abs(ScaleFactor::den);
  constexpr auto unsigned_num = quantity_detail::unsigned_abs(ScaleFactor::num);
  auto const initial_gcd = gcd(initial_value, unsigned_den);
  auto const den = unsigned_den / initial_gcd;
  auto const smaller_value = initial_value / initial_gcd;
  // NOLINTNEXTLINE(clang-analyzer-core.DivideZero)
  auto const initial_result = (smaller_value / den) * unsigned_num;
  auto const remainder = smaller_value % den;
  auto const final_result = initial_result + remainder * unsigned_num / den;
  ARENE_INVARIANT(final_result <= static_cast<std::uint64_t>(std::numeric_limits<TargetRep>::max()));
  return static_cast<TargetRep>(final_result);
}

/// @brief Scale a signed integral value into a signed integer target representation
/// @tparam ScaleFactor The scale factor
/// @tparam TargetRep The target representation
/// @tparam Rep The source representation
/// @param value The value to scale
/// @return TargetRep @c value scaled by @c ScaleFactor
template <
    typename ScaleFactor,
    typename TargetRep,
    typename Rep,
    constraints<
        std::enable_if_t<std::is_integral<TargetRep>::value>,  //
        std::enable_if_t<std::is_integral<Rep>::value>,        //
        std::enable_if_t<std::is_signed<TargetRep>::value>,    //
        std::enable_if_t<std::is_signed<Rep>::value>> = nullptr>
constexpr auto scale_value_by(Rep value) noexcept -> TargetRep {
  constexpr bool sign_flipped{(ScaleFactor::num < 0) != (ScaleFactor::den < 0)};
  bool const value_is_negative{value < static_cast<Rep>(0)};
  bool const result_is_negative{sign_flipped != value_is_negative};
  constexpr auto unsigned_den = quantity_detail::unsigned_abs(ScaleFactor::den);
  constexpr auto unsigned_num = quantity_detail::unsigned_abs(ScaleFactor::num);
  auto const unsigned_result = scale_value_by<std::ratio<unsigned_num, unsigned_den>, std::make_unsigned_t<TargetRep>>(
      quantity_detail::unsigned_abs(value)
  );
  if (result_is_negative) {
    ARENE_INVARIANT(unsigned_result <= quantity_detail::unsigned_abs(std::numeric_limits<TargetRep>::min()));
    return static_cast<TargetRep>(-static_cast<std::int64_t>(unsigned_result));
  }
  ARENE_INVARIANT(
      unsigned_result <= static_cast<std::make_unsigned_t<TargetRep>>(std::numeric_limits<TargetRep>::max())
  );
  return static_cast<TargetRep>(unsigned_result);
}

/// @brief Scale a value from one representation to another, where either the source or target is non-integral
/// @tparam ScaleFactor The scale factor
/// @tparam TargetRep The target representation
/// @tparam Rep The source representation
/// @param value The value to scale
/// @return TargetRep @c value scaled by @c ScaleFactor
template <
    typename ScaleFactor,
    typename TargetRep,
    typename Rep,
    constraints<std::enable_if_t<!std::is_integral<Rep>::value || !std::is_integral<TargetRep>::value>> = nullptr>
constexpr auto scale_value_by(Rep value) noexcept -> TargetRep {
  using intermediate_type = typename std::common_type<TargetRep, Rep>::type;
  intermediate_type const scaled_intermediate{
      static_cast<intermediate_type>(value) * static_cast<intermediate_type>(ScaleFactor::num) /
      static_cast<intermediate_type>(ScaleFactor::den)
  };
  return static_cast<TargetRep>(scaled_intermediate);
}

/// @brief Constant indicating if there is a common type to use when converting from @c SourceQuantityType to @c
/// TargetQuantityType.
///
/// @tparam SourceQuantityType The physical quantity type of the source
/// @tparam TargetQuantityType The physical quantity type of the target
///
/// The value is @c true if there is, @c false otherwise
template <typename SourceQuantityType, typename TargetQuantityType, typename = constraints<>>
extern constexpr bool has_common_type_specified_v = false;

/// @brief Constant indicating if there is a common type to use when converting from @c SourceQuantityType to @c
/// TargetQuantityType.
///
/// @tparam SourceQuantityType The physical quantity type of the source
/// @tparam TargetQuantityType The physical quantity type of the target
///
/// The value is @c true if there is, @c false otherwise. This specialization handles the case where @c
/// units_conversion_traits specifies a common type
template <typename SourceQuantityType, typename TargetQuantityType>
extern constexpr bool has_common_type_specified_v<
    SourceQuantityType,
    TargetQuantityType,
    constraints<typename units_conversion_traits<SourceQuantityType, TargetQuantityType>::common_type>> = true;

/// @brief Constant indicating if there is a scale factor to use when converting from @c SourceQuantityType to @c
/// TargetQuantityType.
///
/// @tparam SourceQuantityType The physical quantity type of the source
/// @tparam TargetQuantityType The physical quantity type of the target
///
/// The value is @c true if there is, @c false otherwise
template <typename SourceQuantityType, typename TargetQuantityType, typename = constraints<>>
extern constexpr bool has_scale_factor_specified_v = false;

/// @brief Constant indicating if there is a scale factor to use when converting from @c SourceQuantityType to @c
/// TargetQuantityType.
///
/// @tparam SourceQuantityType The physical quantity type of the source
/// @tparam TargetQuantityType The physical quantity type of the target
///
/// The value is @c true if there is, @c false otherwise. This specialization handles the case where @c
/// units_conversion_traits specifies a scale factor
template <typename SourceQuantityType, typename TargetQuantityType>
extern constexpr bool has_scale_factor_specified_v<
    SourceQuantityType,
    TargetQuantityType,
    constraints<typename units_conversion_traits<SourceQuantityType, TargetQuantityType>::scale_factor>> = true;

/// @brief Constant indicating if quantities with a unit type of @c SourceQuantityType can be explicitly converted to
/// quantities with a unit type of @c TargetQuantityType.
///
/// @tparam SourceQuantityType The physical quantity type of the source
/// @tparam TargetQuantityType The physical quantity type of the target
///
/// Values can be converted if the Physical Unit Types are "compatible", or if there is a common type specified
///
/// The value is @c true if values can be converted, @c false otherwise.
template <typename SourceQuantityType, typename TargetQuantityType>
extern constexpr bool is_explicitly_convertible_v =
    units_conversion_traits<SourceQuantityType, TargetQuantityType>::compatible ||
    has_common_type_specified_v<SourceQuantityType, TargetQuantityType>;

/// @brief Constant indicating if quantities with a unit type of @c SourceQuantityType can be implicitly converted to
/// quantities with a unit type of @c TargetQuantityType.
///
/// @tparam SourceQuantityType The physical quantity type of the source
/// @tparam TargetQuantityType The physical quantity type of the target
///
/// Values can be converted if the Physical Unit Types are "compatible"
///
/// The value is @c true if values can be converted, @c false otherwise.
template <typename SourceQuantityType, typename TargetQuantityType>
extern constexpr bool is_implicitly_convertible_v =
    units_conversion_traits<SourceQuantityType, TargetQuantityType>::compatible;

/// @brief The type of a scale factor that is the product of the two supplied scale factors
///
/// @tparam ScaleFactor1 The first scale factor
/// @tparam ScaleFactor2 The second scale factor
template <typename ScaleFactor1, typename ScaleFactor2>
using combine_scale_factors_t = std::ratio_multiply<ScaleFactor1, ScaleFactor2>;

/// @brief A helper class for retrieving the scale factor for converting quantities with a physical units type of @c
/// SourceQuantityType to quantities with a physical units type of @c TargetQuantityType
///
/// @tparam SourceQuantityType The physical quantity type of the source
/// @tparam TargetQuantityType The physical quantity type of the target
template <typename SourceQuantityType, typename TargetQuantityType, typename = constraints<>>
struct scale_factor_helper {
  static_assert(
      has_scale_factor_specified_v<SourceQuantityType, TargetQuantityType>,
      "No scale factor specified for conversion between types"
  );
  /// @brief The scale factor for the conversion
  using type = typename units_conversion_traits<SourceQuantityType, TargetQuantityType>::scale_factor;
};

/// @brief The scale factor for converting quantities with a physical units type of @c SourceQuantityType to quantities
/// with a physical units type of @c TargetQuantityType
///
/// @tparam SourceQuantityType The physical quantity type of the source
/// @tparam TargetQuantityType The physical quantity type of the target
template <typename SourceQuantityType, typename TargetQuantityType>
using scale_factor = typename scale_factor_helper<SourceQuantityType, TargetQuantityType>::type;

/// @brief A boolean constant indicating if the common type for conversion between the source and target physical units
/// types is one of them
///
/// @tparam SourceQuantityType The physical quantity type of the source
/// @tparam TargetQuantityType The physical quantity type of the target
///
/// The value is @c true if one or other is the common type, @c false otherwise
template <typename SourceQuantityType, typename TargetQuantityType, typename = constraints<>>
extern constexpr bool common_type_is_source_or_target_v = false;

/// @brief A boolean constant indicating if the common type for conversion between the source and target physical units
/// types is one of them. This specialization handles the case that there is a specified common type
///
/// @tparam SourceQuantityType The physical quantity type of the source
/// @tparam TargetQuantityType The physical quantity type of the target
///
/// The value is @c true if one or other is the common type, @c false otherwise
template <typename SourceQuantityType, typename TargetQuantityType>
extern constexpr bool common_type_is_source_or_target_v<
    SourceQuantityType,
    TargetQuantityType,
    constraints<std::enable_if_t<has_common_type_specified_v<SourceQuantityType, TargetQuantityType>>>> =
    std::is_same<
        typename units_conversion_traits<SourceQuantityType, TargetQuantityType>::common_type,
        SourceQuantityType>::value ||
    std::is_same<
        typename units_conversion_traits<SourceQuantityType, TargetQuantityType>::common_type,
        TargetQuantityType>::value;

/// @brief A specialization of @c scale_factor_helper for retrieving the scale factor between two physical units types
/// when there is a common type specified for conversion, but it is neither one nor the other.
///
/// In this case, the conversion is done via converting from the source to the common type, and then from the common
/// type to the target.
///
/// @tparam SourceQuantityType The physical quantity type of the source
/// @tparam TargetQuantityType The physical quantity type of the target
template <typename SourceQuantityType, typename TargetQuantityType>
struct scale_factor_helper<
    SourceQuantityType,
    TargetQuantityType,
    constraints<
        std::enable_if_t<!is_implicitly_convertible_v<SourceQuantityType, TargetQuantityType>>,
        std::enable_if_t<has_common_type_specified_v<SourceQuantityType, TargetQuantityType>>,
        std::enable_if_t<!common_type_is_source_or_target_v<SourceQuantityType, TargetQuantityType>>>> {
  /// @brief The common type between two unit types
  using common_type = typename units_conversion_traits<SourceQuantityType, TargetQuantityType>::common_type;
  /// @brief The type of the scale factor that is the product of the two unit types
  using type = combine_scale_factors_t<
      scale_factor<SourceQuantityType, common_type>,
      scale_factor<common_type, TargetQuantityType>>;
};

/// @brief Boolean constant indicating if the scaled value for converting from @c SourceQuantityType to @c
/// TargetQuantityType is guaranteed to be in the range of @c TargetRep if the source value is stored in @c SourceRep
///
/// @tparam SourceQuantityType The physical quantity type of the source
/// @tparam TargetQuantityType The physical quantity type of the target
/// @tparam SourceRep The representation type of the source
/// @tparam TargetRep The representation type of the target
template <
    typename SourceQuantityType,
    typename SourceRep,
    typename TargetQuantityType,
    typename TargetRep,
    typename = constraints<>>
extern constexpr bool scaled_value_in_range_v = false;

/// @brief Boolean constant indicating if the maximum value of @c SourceRep when scaled by @c ScaleFactor fits in @c
/// TargetRep
///
/// @tparam ScaleFactor The scale factor to use
/// @tparam SourceRep The representation type of the source
/// @tparam TargetRep The representation type of the target
template <typename ScaleFactor, typename SourceRep, typename TargetRep, typename = constraints<>>
extern constexpr bool scaled_max_fits_v = false;

/// @brief Specialization of @c scaled_max_fits_v for the case where the target is not integral
///
/// @tparam ScaleFactor The scale factor to use
/// @tparam SourceRep The representation type of the source
/// @tparam TargetRep The representation type of the target
template <typename ScaleFactor, typename SourceRep, typename TargetRep>
extern constexpr bool scaled_max_fits_v<
    ScaleFactor,
    SourceRep,
    TargetRep,
    constraints<std::enable_if_t<!std::is_integral<TargetRep>::value>>> = true;

/// @brief Specialization of @c scaled_max_fits_v for the case where both the source and target are integral
///
/// @tparam ScaleFactor The scale factor to use
/// @tparam SourceRep The representation type of the source
/// @tparam TargetRep The representation type of the target
template <typename ScaleFactor, typename SourceRep, typename TargetRep>
extern constexpr bool scaled_max_fits_v<
    ScaleFactor,
    SourceRep,
    TargetRep,
    constraints<
        std::enable_if_t<std::is_integral<SourceRep>::value>,
        std::enable_if_t<std::is_integral<TargetRep>::value>>> =
    (static_cast<std::uint64_t>(std::numeric_limits<SourceRep>::max()) <=
     (std::numeric_limits<std::uint64_t>::max() / quantity_detail::unsigned_abs(ScaleFactor::num))) &&
    ((static_cast<std::uint64_t>(std::numeric_limits<SourceRep>::max()) *
      quantity_detail::unsigned_abs(ScaleFactor::num) / quantity_detail::unsigned_abs(ScaleFactor::den)) <=
     static_cast<std::uint64_t>(std::numeric_limits<TargetRep>::max()));

/// @brief Specialization of @c scaled_value_in_range_v for the case that the source physical quantity should be
/// implicitly convertible to the target physical quantity.
///
/// @tparam SourceQuantityType The physical quantity type of the source
/// @tparam TargetQuantityType The physical quantity type of the target
/// @tparam SourceRep The representation type of the source
/// @tparam TargetRep The representation type of the target
template <typename SourceQuantityType, typename SourceRep, typename TargetQuantityType, typename TargetRep>
extern constexpr bool scaled_value_in_range_v<
    SourceQuantityType,
    SourceRep,
    TargetQuantityType,
    TargetRep,
    constraints<std::enable_if_t<is_implicitly_convertible_v<SourceQuantityType, TargetQuantityType>>>> =
    scaled_max_fits_v<scale_factor<SourceQuantityType, TargetQuantityType>, SourceRep, TargetRep>;

}  // namespace quantity_detail

/// @brief A class representing a quantity of a physical units type.
///
/// Conversions and arithmetic are supported if indicated by the appropriate traits for the physical units tag type.
/// This allows the mathematical operations of engineering equations to be performed using instances of @c quantity,
/// ensuring that the units of the result are correct, and preventing errors due to passing incorrect values or values
/// in the wrong units.
///
/// @tparam QuantityType A tag type representing the physical quantity
/// @tparam Rep The arithmetic type to use for holding the value
template <typename QuantityType, typename Rep>
class quantity : generic_ordering_from_three_way_compare<quantity<QuantityType, Rep>> {
  /// @brief The type of the base class for defining ordering
  using ordering_base = generic_ordering_from_three_way_compare<quantity<QuantityType, Rep>>;

  /// @brief The stored value
  Rep value_;

 public:
  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Default construct with a default-constructed representation value
  constexpr quantity() noexcept
      : quantity(Rep{}) {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a
  /// @brief Construct a @c quantity holding the specified representation value
  /// @param store_value The value to store
  explicit constexpr quantity(Rep store_value) noexcept
      : ordering_base{},
        value_(std::move(store_value)) {}

  // parasoft-begin-suppress AUTOSAR-A12_1_1-a-2 "False positive: This constructor delegates to another, which does
  // initialize the base class"
  /// @brief Implicitly convert from a quantity with a different physical units type or representation type, scaling the
  /// value as appropriate.
  ///
  /// @pre the physical unit types are marked as being implicitly convertible, and the scaled value is guaranteed to be
  /// in range, for integral representations.
  /// @tparam OtherQuantityType The physical units type of the source quantity
  /// @tparam OtherRep The representation type of the source quantity
  /// @param other The other quantity
  template <
      typename OtherQuantityType,
      typename OtherRep,
      constraints<
          std::enable_if_t<quantity_detail::is_implicitly_convertible_v<OtherQuantityType, QuantityType>>,
          std::enable_if_t<quantity_detail::scaled_value_in_range_v<OtherQuantityType, OtherRep, QuantityType, Rep>>> =
          nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr quantity(quantity<OtherQuantityType, OtherRep> const& other) noexcept
      : quantity(other.template count_of<QuantityType, Rep>()) {}
  // parasoft-end-suppress AUTOSAR-A12_1_1-a

  /// @brief Get the numeric representation of the stored quantity, for the specified physical units type
  ///
  /// The value is scaled as appropriate
  ///
  /// @tparam OtherQuantityType The physical units type for which to retrieve the count
  /// @tparam OtherRep The representation type to return the count in; defaults to @c Rep
  /// @return OtherRep holding the scaled value
  /// @pre The physical units type @c QuantityType must be marked as explicitly convertible to @c OtherQuantityType
  template <
      typename OtherQuantityType,
      typename OtherRep = Rep,
      constraints<std::enable_if_t<quantity_detail::is_explicitly_convertible_v<QuantityType, OtherQuantityType>>> =
          nullptr>
  constexpr auto count_of() const noexcept -> OtherRep {
    return quantity_detail::scale_value_by<quantity_detail::scale_factor<QuantityType, OtherQuantityType>, OtherRep>(
        value_
    );
  }

  /// @brief Convert the stored quantity to the specified physical units type and representation
  ///
  /// The value is scaled as appropriate
  ///
  /// @tparam OtherQuantityType The physical units type for which to retrieve the count
  /// @tparam OtherRep The representation type to return the count in; defaults to @c Rep
  /// @return quantity<OtherQuantityType,OtherRep> holding the scaled value
  /// @pre The physical units type @c QuantityType must be marked as explicitly convertible to @c OtherQuantityType
  template <
      typename OtherQuantityType,
      typename OtherRep = Rep,
      constraints<std::enable_if_t<quantity_detail::is_explicitly_convertible_v<QuantityType, OtherQuantityType>>> =
          nullptr>
  constexpr auto as() const noexcept -> quantity<OtherQuantityType, OtherRep> {
    return quantity<OtherQuantityType, OtherRep>{count_of<OtherQuantityType, OtherRep>()};
  }

  /// @brief three-way comparison between two quantities
  ///
  /// @param lhs The first object to compare
  /// @param rhs The second object to compare
  /// @return strong_ordering @c strong_ordering::less if @c lhs is less than @c rhs, @c strong_ordering::equal if they
  /// hold the same value, and @c strong_ordering::greater if @c lhs is greater than @c rhs
  static constexpr auto three_way_compare(quantity const& lhs, quantity const& rhs) noexcept -> strong_ordering {
    return compare_three_way{}(lhs.value_, rhs.value_);
  }

  /// @brief three-way comparison between two quantities with the same physical units but different representations
  ///
  /// @param lhs The first object to compare
  /// @param rhs The second object to compare
  /// @return strong_ordering @c strong_ordering::less if @c lhs is less than @c rhs, @c strong_ordering::equal if they
  /// hold the same value, and @c strong_ordering::greater if @c lhs is greater than @c rhs
  template <typename OtherRep>
  static constexpr auto three_way_compare(quantity const& lhs, quantity<QuantityType, OtherRep> const& rhs) noexcept
      -> strong_ordering {
    using common_rep = std::common_type_t<Rep, OtherRep>;
    return compare_three_way{}(static_cast<common_rep>(lhs.value_), rhs.template count_of<QuantityType, common_rep>());
  }

  /// @{
  /// @brief three-way comparison between two quantities with different physical units
  ///
  /// @param lhs The first object to compare
  /// @param rhs The second object to compare
  /// @return strong_ordering @c strong_ordering::less if @c lhs is less than @c rhs, @c strong_ordering::equal if they
  /// hold the same value, and @c strong_ordering::greater if @c lhs is greater than @c rhs
  template <
      typename OtherQuantityType,
      typename OtherRep,
      constraints<
          std::enable_if_t<quantity_detail::is_implicitly_convertible_v<OtherQuantityType, QuantityType>>,
          std::enable_if_t<quantity_detail::scaled_value_in_range_v<
              OtherQuantityType,
              OtherRep,
              QuantityType,
              std::common_type_t<Rep, OtherRep>>>> = nullptr>
  static constexpr auto
  three_way_compare(quantity const& lhs, quantity<OtherQuantityType, OtherRep> const& rhs) noexcept -> strong_ordering {
    using common_rep = std::common_type_t<Rep, OtherRep>;
    return compare_three_way{
    }(static_cast<common_rep>(lhs.value_), rhs.template count_of<OtherQuantityType, common_rep>());
  }

  /// @brief three-way comparison between two quantities with different physical units where one quantity type is not
  /// within the scaled range of the other
  ///
  /// @param lhs The first object to compare
  /// @param rhs The second object to compare
  /// @return strong_ordering @c strong_ordering::less if @c lhs is less than @c rhs, @c strong_ordering::equal if they
  /// hold the same value, and @c strong_ordering::greater if @c lhs is greater than @c rhs
  template <
      typename OtherQuantityType,
      typename OtherRep,
      constraints<
          std::enable_if_t<quantity_detail::is_implicitly_convertible_v<OtherQuantityType, QuantityType>>,
          std::enable_if_t<!quantity_detail::scaled_value_in_range_v<
              OtherQuantityType,
              OtherRep,
              QuantityType,
              std::common_type_t<Rep, OtherRep>>>> = nullptr>
  static constexpr auto
  three_way_compare(quantity const& lhs, quantity<OtherQuantityType, OtherRep> const& rhs) noexcept -> strong_ordering {
    using common_rep = std::common_type_t<Rep, OtherRep>;
    using rhs_to_lhs_scale_factor = quantity_detail::scale_factor<OtherQuantityType, QuantityType>;
    constexpr common_rep max_safely_in_range{static_cast<common_rep>(
        (static_cast<std::uint64_t>(std::numeric_limits<OtherRep>::max()) /
         quantity_detail::unsigned_abs(rhs_to_lhs_scale_factor::num)) *
        quantity_detail::unsigned_abs(rhs_to_lhs_scale_factor::den)
    )};
    constexpr common_rep single_increment_delta{static_cast<common_rep>(
        quantity_detail::unsigned_abs(rhs_to_lhs_scale_factor::den) %
        quantity_detail::unsigned_abs(rhs_to_lhs_scale_factor::num)
    )};
    constexpr common_rep max_in_range{static_cast<common_rep>(max_safely_in_range + single_increment_delta - 1)};

    common_rep const common_rhs{rhs.template count_of<OtherQuantityType, common_rep>()};
    if (common_rhs >= max_in_range) {
      return strong_ordering::less;
    }
    if (quantity_detail::unsigned_abs(common_rhs) > quantity_detail::unsigned_abs(max_in_range)) {
      return strong_ordering::greater;
    }
    return compare_three_way{}(lhs.as<QuantityType, common_rep>(), rhs.template as<QuantityType, common_rep>());
  }
  /// @}

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: cannot apply static here"
  /// @brief Add two quantities of the same type
  /// @tparam Q Template parameter introduced to provide constraints; equivalent to @c QuantityType
  /// @param lhs The first quantity
  /// @param rhs The second quantity
  /// @return quantity<sum_type,Rep> A @c quantity with the physical units type specified as the @c
  /// sum_type for @c QuantityType and @c QuantityType, and a representation of @c Rep, holding the sum of the two
  /// quantity values
  template <typename Q = QuantityType, constraints<typename units_combination_traits<Q, Q>::sum_type> = nullptr>
  friend constexpr auto operator+(quantity const& lhs, quantity const& rhs) noexcept
      -> quantity<typename units_combination_traits<Q, Q>::sum_type, Rep> {
    return quantity<typename units_combination_traits<Q, Q>::sum_type, Rep>{
        static_cast<Rep>(lhs.count_of<Q>() + rhs.template count_of<Q>())
    };
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: cannot apply static here"
  /// @{
  /// @brief Add two quantities
  /// @tparam OtherQuantityType The physical units type of the other quantity
  /// @tparam OtherRep The representation type of the other quantity
  /// @tparam Q Template parameter introduced to provide constraints; equivalent to @c QuantityType
  /// @tparam R Template parameter introduced to provide constraints; equivalent to @c Rep
  /// @param lhs The first quantity
  /// @param rhs The second quantity
  /// @return quantity<sum_type,CommonRep> A @c quantity with the physical units type specified as the @c
  /// sum_type for @c QuantityType and @c OtherQuantityType, and a representation which is the common type of @c Rep
  /// and @c OtherRep, holding the sum of the two quantity values
  template <
      typename OtherQuantityType,
      typename OtherRep,
      typename Q = QuantityType,
      typename R = Rep,
      typename CommonRep = std::common_type_t<Rep, OtherRep>,
      constraints<typename units_combination_traits<QuantityType, OtherQuantityType>::sum_type> = nullptr>
  friend constexpr auto operator+(quantity const& lhs, quantity<OtherQuantityType, OtherRep> const& rhs) noexcept
      -> quantity<typename units_combination_traits<QuantityType, OtherQuantityType>::sum_type, CommonRep> {
    return quantity<typename units_combination_traits<QuantityType, OtherQuantityType>::sum_type, CommonRep>{
        static_cast<CommonRep>(lhs.count_of<Q, CommonRep>() + rhs.template count_of<OtherQuantityType, CommonRep>())
    };
  }
  /// @}
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: cannot apply static here"
  /// @{
  /// @brief Subtract two quantities
  /// @tparam OtherQuantityType The physical units type of the other quantity
  /// @tparam OtherRep The representation type of the other quantity
  /// @tparam Q Template parameter introduced to provide constraints; equivalent to @c QuantityType
  /// @tparam R Template parameter introduced to provide constraints; equivalent to @c Rep
  /// @param lhs The first quantity
  /// @param rhs The second quantity
  /// @return quantity<difference_type,CommonRep> A @c quantity with the physical units type specified as the @c
  /// difference_type for @c QuantityType and @c OtherQuantityType, and a representation which is the common type of @c
  /// Rep and @c OtherRep, holding the difference of the two quantity values
  template <
      typename OtherQuantityType,
      typename OtherRep,
      typename Q = QuantityType,
      typename R = Rep,
      typename CommonRep = std::common_type_t<Rep, OtherRep>,
      constraints<typename units_combination_traits<QuantityType, OtherQuantityType>::difference_type> = nullptr>
  friend constexpr auto operator-(quantity const& lhs, quantity<OtherQuantityType, OtherRep> const& rhs) noexcept
      -> quantity<typename units_combination_traits<QuantityType, OtherQuantityType>::difference_type, CommonRep> {
    return quantity<typename units_combination_traits<QuantityType, OtherQuantityType>::difference_type, CommonRep>{
        static_cast<CommonRep>(lhs.count_of<Q, CommonRep>() - rhs.template count_of<OtherQuantityType, CommonRep>())
    };
  }
  /// @}
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: cannot apply static here"
  /// @{
  /// @brief Multiply a quantity by a scalar
  /// @tparam Scalar The type of the scalar
  /// @tparam Q Template parameter introduced to provide constraints; equivalent to @c QuantityType
  /// @tparam R Template parameter introduced to provide constraints; equivalent to @c Rep
  /// @tparam CommonRep Template parameter introduced to provide constraints; the common representation type of @c Rep
  /// and @c Scalar
  /// @param lhs The left hand operand
  /// @param rhs The right hand operand
  /// @return quantity<Q,CommonRep> A @c quantity with the same physical units type, and a representation which is the
  /// common type of @c Rep and @c Scalar, holding the product of the quantity value and the scalar value
  template <
      typename Scalar,
      typename Q = QuantityType,
      typename R = Rep,
      constraints<std::enable_if_t<!quantity_detail::is_quantity_v<Scalar>>> = nullptr,
      typename CommonRep = std::common_type_t<Rep, Scalar>>
  friend constexpr auto operator*(quantity const& lhs, Scalar const& rhs) noexcept -> quantity<Q, CommonRep> {
    return quantity<Q, CommonRep>{lhs.count_of<Q, CommonRep>() * static_cast<CommonRep>(rhs)};
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: cannot apply static here"
  /// @brief Multiply a quantity by a scalar
  /// @tparam Scalar The type of the scalar
  /// @tparam Q Template parameter introduced to provide constraints; equivalent to @c QuantityType
  /// @tparam R Template parameter introduced to provide constraints; equivalent to @c Rep
  /// @tparam CommonRep Template parameter introduced to provide constraints; the common representation type of @c Rep
  /// and @c Scalar
  /// @param lhs The left hand operand
  /// @param rhs The right hand operand
  /// @return quantity<Q,CommonRep> A @c quantity with the same physical units type, and a representation which is the
  /// common type of @c Rep and @c Scalar, holding the quantity value multiplied by the scalar value
  template <
      typename Scalar,
      typename Q = QuantityType,
      typename R = Rep,
      constraints<std::enable_if_t<!quantity_detail::is_quantity_v<Scalar>>> = nullptr,
      typename CommonRep = std::common_type_t<Rep, Scalar>>
  friend constexpr auto operator*(Scalar const& lhs, quantity const& rhs) noexcept -> quantity<Q, CommonRep> {
    return rhs * lhs;
  }
  /// @}
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: cannot apply static here"
  /// @{
  /// @brief Multiply two quantities
  /// @tparam OtherQuantityType The physical units type of the other quantity
  /// @tparam OtherRep The representation type of the other quantity
  /// @tparam Q Template parameter introduced to provide constraints; equivalent to @c QuantityType
  /// @tparam R Template parameter introduced to provide constraints; equivalent to @c Rep
  /// @tparam CommonRep Template parameter introduced to provide constraints; the common representation type of @c Rep
  /// and @c OtherRep
  /// @param lhs The left hand operand
  /// @param rhs The right hand operand
  /// @return quantity<product_type,CommonRep> A @c quantity with the physical units type specified as the @c
  /// product_type for @c QuantityType and @c OtherQuantityType, and a representation which is the common type of @c Rep
  /// and @c OtherRep, holding the product of the two quantity values
  /// @pre There is a @c product_type specified in @c units_combination_traits<QuantityType,OtherQuantityType>
  template <
      typename OtherQuantityType,
      typename OtherRep,
      typename Q = QuantityType,
      typename R = Rep,
      typename CommonRep = std::common_type_t<Rep, OtherRep>,
      constraints<
          typename units_combination_traits<QuantityType, OtherQuantityType>::product_type,  //
          std::enable_if_t<!std::is_integral<CommonRep>::value>> = nullptr>
  friend constexpr auto operator*(quantity const& lhs, quantity<OtherQuantityType, OtherRep> const& rhs) noexcept
      -> quantity<typename units_combination_traits<QuantityType, OtherQuantityType>::product_type, CommonRep> {
    return quantity<typename units_combination_traits<QuantityType, OtherQuantityType>::product_type, CommonRep>{
        static_cast<CommonRep>(lhs.count_of<Q, CommonRep>() * rhs.template count_of<OtherQuantityType, CommonRep>())
    };
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: cannot apply static here"
  /// @brief Multiply two quantities
  /// @tparam OtherQuantityType The physical units type of the other quantity
  /// @tparam OtherRep The representation type of the other quantity
  /// @tparam Q Template parameter introduced to provide constraints; equivalent to @c QuantityType
  /// @tparam R Template parameter introduced to provide constraints; equivalent to @c Rep
  /// @tparam CommonRep Template parameter introduced to provide constraints; the common representation type of @c Rep
  /// and @c OtherRep
  /// @param lhs The left hand operand
  /// @param rhs The right hand operand
  /// @return quantity<product_type,CommonRep> A @c quantity with the physical units type specified as the @c
  /// product_type for @c QuantityType and @c OtherQuantityType, and a representation which is the common type of @c Rep
  /// and @c OtherRep, holding the left hand quantity value multiplied by the right hand quantity value
  /// @pre There is a @c product_type specified in @c units_combination_traits<QuantityType,OtherQuantityType>
  template <
      typename OtherQuantityType,
      typename OtherRep,
      typename Q = QuantityType,
      typename R = Rep,
      typename CommonRep = std::common_type_t<Rep, OtherRep>,
      constraints<
          typename units_combination_traits<QuantityType, OtherQuantityType>::product_type,  //
          std::enable_if_t<std::is_integral<CommonRep>::value>,
          std::enable_if_t<
              std::is_signed<CommonRep>::value || (std::is_unsigned<R>::value && std::is_unsigned<OtherRep>::value)>> =
          nullptr>
  friend constexpr auto operator*(quantity const& lhs, quantity<OtherQuantityType, OtherRep> const& rhs) noexcept
      -> quantity<typename units_combination_traits<QuantityType, OtherQuantityType>::product_type, CommonRep> {
    return quantity<typename units_combination_traits<QuantityType, OtherQuantityType>::product_type, CommonRep>{
        static_cast<CommonRep>(lhs.count_of<Q, CommonRep>() * rhs.template count_of<OtherQuantityType, CommonRep>())
    };
  }
  /// @}
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: cannot apply static here"
  /// @brief Divide a quantity by a scalar
  /// @tparam Scalar The type of the scalar
  /// @tparam Q Template parameter introduced to provide constraints; equivalent to @c QuantityType
  /// @tparam R Template parameter introduced to provide constraints; equivalent to @c Rep
  /// @tparam CommonRep Template parameter introduced to provide constraints; the common representation type of @c Rep
  /// and @c Scalar
  /// @param lhs The left hand operand
  /// @param rhs The right hand operand
  /// @return quantity<Q,CommonRep> A @c quantity with the same physical units type, and a representation which is the
  /// common type of @c Rep and @c Scalar, holding the quantity value divided by the scalar value
  template <
      typename Scalar,
      typename Q = QuantityType,
      typename R = Rep,
      constraints<std::enable_if_t<!quantity_detail::is_quantity_v<Scalar>>> = nullptr,
      typename CommonRep = std::common_type_t<Rep, Scalar>,
      constraints<std::enable_if_t<!std::is_integral<CommonRep>::value>> = nullptr>
  friend constexpr auto operator/(quantity const& lhs, Scalar const& rhs) noexcept -> quantity<Q, CommonRep> {
    return quantity<Q, CommonRep>{static_cast<CommonRep>(lhs.count_of<Q, CommonRep>() / static_cast<CommonRep>(rhs))};
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: cannot apply static here"
  /// @brief Divide two quantities
  /// @tparam OtherQuantityType The physical units type of the other quantity
  /// @tparam OtherRep The representation type of the other quantity
  /// @tparam Q Template parameter introduced to provide constraints; equivalent to @c QuantityType
  /// @tparam R Template parameter introduced to provide constraints; equivalent to @c Rep
  /// @tparam CommonRep Template parameter introduced to provide constraints; the common representation type of @c Rep
  /// and @c OtherRep
  /// @param lhs The left hand operand
  /// @param rhs The right hand operand
  /// @return quantity<ratio_type,CommonRep> A @c quantity with the physical units type specified as the @c
  /// ratio_type for @c QuantityType and @c OtherQuantityType, and a representation which is the common type of @c Rep
  /// and @c OtherRep, holding the left hand quantity value divided by the right hand quantity value
  /// @pre There is a @c ratio_type specified in @c units_combination_traits<QuantityType,OtherQuantityType>
  template <
      typename OtherQuantityType,
      typename OtherRep,
      typename Q = QuantityType,
      typename R = Rep,
      typename CommonRep = std::common_type_t<Rep, OtherRep>,
      constraints<typename units_combination_traits<QuantityType, OtherQuantityType>::ratio_type> = nullptr>
  friend constexpr auto operator/(quantity const& lhs, quantity<OtherQuantityType, OtherRep> const& rhs) noexcept
      -> quantity<typename units_combination_traits<QuantityType, OtherQuantityType>::ratio_type, CommonRep> {
    return quantity<typename units_combination_traits<QuantityType, OtherQuantityType>::ratio_type, CommonRep>{
        static_cast<CommonRep>(lhs.count_of<Q, CommonRep>() / rhs.template count_of<OtherQuantityType, CommonRep>())
    };
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
};
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_QUANTITY_HPP_
