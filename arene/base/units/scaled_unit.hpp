#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_SCALED_UNIT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_SCALED_UNIT_HPP_

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/math/float_properties.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/is_floating_point.hpp"
#include "arene/base/stdlib_choice/ratio.hpp"
#include "arene/base/type_info/type_name_string.hpp"
#include "arene/base/type_list/index_of.hpp"
#include "arene/base/type_list/remove_nth.hpp"
#include "arene/base/type_list/sort.hpp"
#include "arene/base/type_traits/is_one_of.hpp"
#include "arene/base/units/is_unit.hpp"
#include "arene/base/units/quantity_unit.hpp"

namespace arene {
namespace base {

namespace scaled_unit_detail {

/// @brief Helper trait to check if the specified type is an instance of @c std::ratio
/// @tparam Type the type to check
template <typename Type>
extern constexpr bool is_ratio_v = false;

/// @brief Helper trait to check if the specified type is an instance of @c std::ratio
/// @tparam Numerator the numerator of the ratio
/// @tparam Denominator the denominator of the ratio
template <std::intmax_t Numerator, std::intmax_t Denominator>
extern constexpr bool is_ratio_v<std::ratio<Numerator, Denominator>> = true;

/// @brief Helper trait to check if the specified type is an instance of @c std::ratio equivalent to 1
/// @tparam Type the type to check
template <typename Type, typename = constraints<>>
extern constexpr bool is_identity_ratio_v = false;

/// @brief Helper trait to check if the specified type is an instance of @c std::ratio equivalent to 1
/// @tparam Type the type to check
template <typename Type>
extern constexpr bool is_identity_ratio_v<Type, constraints<std::enable_if_t<is_ratio_v<Type>>>> =
    (Type::num == Type::den);

/// @brief Helper trait to check for a valid floating point wrapper scale: the @c Type must be a class with a public
/// floating point @c constexpr @c static data member named @c value that is not infinity, NaN, zero or subnormal
/// @tparam Type the type to check
template <typename Type, typename = constraints<>>
extern constexpr bool is_floating_point_wrapper_scale_v = false;

/// @brief Helper trait to check for a valid floating point wrapper scale: the @c Type must be a class with a public
/// floating point @c constexpr @c static data member named @c value that is not infinity, NaN, zero or subnormal
/// @tparam Type the type to check
template <typename Type>
extern constexpr bool is_floating_point_wrapper_scale_v<
    Type,
    constraints<
        std::enable_if_t<std::is_floating_point<decltype(Type::value)>::value>,
        std::enable_if_t<isnormal(Type::value)>>> = true;

/// @brief Helper trait to check for a valid scale: either a ratio or a valid floating point wrapper
/// @tparam Type the type to check
template <typename Type>
extern constexpr bool is_valid_scale_v = is_floating_point_wrapper_scale_v<Type> || is_ratio_v<Type>;

/// @brief Implementation class that represents the inverse of a floating point wrapper scale
/// @tparam Type the type to invert
template <typename Type>
class inverse_unit_scale {
 public:
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "This member is the public API"
  /// @brief The resulting scale factor
  static constexpr double value{1. / Type::value};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a
};

/// @brief Helper class to select the implementation of an inverse of a scale
/// @tparam Type the type to invert
template <typename Type, typename = constraints<>>
class inverse_unit_scale_helper {
  static_assert(is_floating_point_wrapper_scale_v<Type>, "The type being inverted must be a floating point wrapper");

 public:
  /// @brief A type representing the inverse scale
  using type = inverse_unit_scale<Type>;
};

/// @brief Implementation class for a quantity unit that is another unit scaled by some scale factor
/// @tparam BaseUnit the underlying unit
/// @tparam Scale the scale factor
template <typename BaseUnit, typename Scale>
class scaled_unit_impl : public quantity_unit<scaled_unit_impl<BaseUnit, Scale>, typename BaseUnit::unit_kind_type> {
 public:
  /// @brief The scale factor
  using scale_factor = Scale;
  /// @brief Denotes that quantity kind is implicit and does not mark a
  /// boundary when aggregating and simplifying quantity kind types.
  using is_implicit = void;
};

/// @brief Implementation class determining the representation type for a quantity unit that is another unit scaled by
/// some scale factor
/// @tparam BaseUnit the underlying unit
/// @tparam Scale the scale factor
template <typename BaseUnit, typename Scale, typename = constraints<>>
class scaled_unit_helper {
  static_assert(is_unit_v<BaseUnit>, "The supplied BaseUnit must be a valid quantity unit");
  static_assert(is_valid_scale_v<Scale>, "The scale factor must be a ratio or a class wrapping a floating point scale");

 public:
  /// @brief The type to use to represent the scaled unit
  using type = scaled_unit_impl<BaseUnit, Scale>;
};

}  // namespace scaled_unit_detail

/// @brief Type alias for a quantity unit that is another unit scaled by some scale factor
/// @tparam BaseUnit the underlying unit
/// @tparam Scale the scale factor
template <typename BaseUnit, typename Scale>
using scaled_unit = typename scaled_unit_detail::scaled_unit_helper<BaseUnit, Scale>::type;

/// @brief A type that represents the inverse of a scale factor
/// @tparam Type the type to invert
template <typename Type>
using inverse_unit_scale_t = typename scaled_unit_detail::inverse_unit_scale_helper<Type>::type;

namespace scaled_unit_detail {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief Helper function to get the scale value from a class wrapping a floating point scale
/// @tparam Type the class wrapping the scale
/// @return The scale factor
template <typename Type, constraints<std::enable_if_t<std::is_floating_point<decltype(Type::value)>::value>> = nullptr>
constexpr auto scale_value() noexcept -> double {
  return Type::value;
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief Helper function to get the scale value from a ratio
/// @tparam Type the ratio
/// @return The scale factor
template <typename Type, constraints<std::enable_if_t<is_ratio_v<Type>>> = nullptr>
constexpr auto scale_value() noexcept -> double {
  return static_cast<double>(Type::num) / static_cast<double>(Type::den);
}

/// @brief Helper function to get the resulting scale factor from a combination of scale factors
/// @param values the scale factors being combined
/// @return The combined scale factor
inline constexpr auto combined_scale_value(std::initializer_list<double> values) noexcept -> double {
  double scale{1.};
  for (auto const elem : values) {
    // Multiply all the scale factors together
    scale *= elem;
  }
  return scale;
}

/// @brief A template representing a scale factor combined from multiple individual scale factors
/// @tparam Scales the scale factors that are combined
template <typename... Scales>
class combined_scales {
 public:
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "This member is the public API"
  /// @brief The numeric value of the combined scale factor
  static constexpr double value{combined_scale_value({scale_value<Scales>()...})};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a
};

/// @brief Helper template for canonicalizing the result of combining scale factors, so empty lists are represented as
/// an identity ratio, single element lists are represented as the list element, and multi-element lists have a
/// consistent order
/// @tparam Scales the scale factors being combined
template <typename Scales>
class canonicalize_scale {
 public:
  /// @brief The resulting scale factor type
  using type = Scales;
};

/// @brief Helper template for canonicalizing the result of combining scale factors, so empty lists are represented as
/// an identity ratio, single element lists are represented as the list element, and multi-element lists have a
/// consistent order
template <>
class canonicalize_scale<combined_scales<>> {
 public:
  /// @brief The resulting scale factor type
  using type = std::ratio<1, 1>;
};

/// @brief Helper template for canonicalizing the result of combining scale factors, so empty lists are represented as
/// an identity ratio, single element lists are represented as the list element, and multi-element lists have a
/// consistent order
/// @tparam Scale the solitary scale factor in the list
template <typename Scale>
class canonicalize_scale<combined_scales<Scale>> {
 public:
  /// @brief The resulting scale factor type
  using type = Scale;
};

/// @brief Helper template that compares two scale factors to determine which should be ordered before the other when
/// combining.
///
/// There should be at most one ratio in a list being sorted with this predicate. The resulting sorted order places the
/// ratio (if there is one) first, followed by the other entries in order of their type name.
/// @tparam LhsScale The left-hand scale being compared
/// @tparam RhsScale The right-hand scale being compared
template <typename LhsScale, typename RhsScale>
class scale_sorts_before {
 public:
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "This member is the public API"
  // parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: type_name_v is initialized"
  /// @brief The result of the comparison
  static constexpr bool value{
      !is_ratio_v<RhsScale> && (is_ratio_v<LhsScale> || (type_name_v<LhsScale> < type_name_v<RhsScale>))
  };
  // parasoft-end-suppress CERT_CPP-DCL56-a
  // parasoft-end-suppress AUTOSAR-M11_0_1-a
};

/// @brief Helper template for canonicalizing the result of combining scale factors, so empty lists are represented as
/// an identity ratio, single element lists are represented as the list element, and multi-element lists have a
/// consistent order
///
/// The ratio goes first if there is one, then the rest, ordered by type name
/// @tparam Scales the combined scale factors
template <typename... Scales>
class canonicalize_scale<combined_scales<Scales...>> {
 public:
  /// @brief The resulting scale factor type
  using type = type_lists::sort_t<combined_scales<Scales...>, scale_sorts_before>;
};

/// @brief Helper class for combining two scales
/// @tparam Scale1 the first scale to combine
/// @tparam Scale2 the second scale to combine
template <typename Scale1, typename Scale2, typename = constraints<>>
class combine_scales_helper {
 public:
  /// @brief The resulting scale factor type
  using type = combined_scales<Scale1, Scale2>;
};

/// @brief Type alias for combining two scales. The result is a type that represents the product of the two scales
/// @tparam Scale1 the first scale to combine
/// @tparam Scale2 the second scale to combine
template <typename Scale1, typename Scale2>
using combine_scales_t = typename canonicalize_scale<typename combine_scales_helper<Scale1, Scale2>::type>::type;

/// @brief Helper class for combining two scales. The result is a type that represents the product of the two scales
/// @tparam Scale1 the first scale to combine
/// @tparam Scale2 the second scale to combine
///
/// @details This is where they are both ratios; the result is equivalent to <c>std::ratio_multiply<Scale1,Scale2></c>
template <typename Scale1, typename Scale2>
class combine_scales_helper<
    Scale1,
    Scale2,
    constraints<std::enable_if_t<is_ratio_v<Scale1>>, std::enable_if_t<is_ratio_v<Scale2>>>> {
 public:
  /// @brief The resulting scale factor type
  using type = std::ratio_multiply<Scale1, Scale2>;
};

/// @brief Helper class for combining two scales. The result is a type that represents the product of the two scales
/// @tparam Scale1 the first scale to combine
/// @tparam Scale2 the second scale to combine
///
/// @details This is where the second is a ratio, but not the first: other code assumes ratios are first, so switch the
/// order.
template <typename Scale1, typename Scale2>
class combine_scales_helper<
    Scale1,
    Scale2,
    constraints<std::enable_if_t<!is_ratio_v<Scale1>>, std::enable_if_t<is_ratio_v<Scale2>>>> {
 public:
  /// @brief The resulting scale factor type
  using type = combine_scales_t<Scale2, Scale1>;
};

/// @brief Helper class for combining two scales. The result is a type that represents the product of the two scales
/// @tparam Scales1 the individual elements of the first scale to combine
/// @tparam Scale2 the second scale to combine
///
/// @details This is where the first is already a combined scale
template <typename... Scales1, typename Scale2>
class combine_scales_helper<
    combined_scales<Scales1...>,
    Scale2,
    constraints<
        std::enable_if_t<!is_ratio_v<Scale2>>,
        std::enable_if_t<!is_one_of_v<inverse_unit_scale_t<Scale2>, Scales1...>>>> {
 public:
  /// @brief The resulting scale factor type
  using type = combined_scales<Scales1..., Scale2>;
};

/// @brief Helper class for combining two scales. The result is a type that represents the product of the two scales
/// @tparam Scales1 the individual elements of the first scale to combine
/// @tparam Scale2 the second scale to combine
///
/// @details This is where the first is already a combined scale and the second is an inverse of one of the elements
template <typename... Scales1, typename Scale2>
class combine_scales_helper<
    combined_scales<Scales1...>,
    Scale2,
    constraints<
        std::enable_if_t<!is_ratio_v<Scale2>>,
        std::enable_if_t<is_one_of_v<inverse_unit_scale_t<Scale2>, Scales1...>>>> {
 public:
  /// @brief The resulting scale factor type
  using type = type_lists::remove_nth_t<
      combined_scales<Scales1...>,
      type_lists::index_of_v<combined_scales<Scales1...>, inverse_unit_scale_t<Scale2>>>;
};

/// @brief Helper class for combining two scales. The result is a type that represents the product of the two scales
/// @tparam Scale1 the first scale to combine
/// @tparam FirstScale2 the first element of the second scale to combine
/// @tparam OtherScales2 the remaining elements of the second scale to combine
///
/// @details This is where the first is a ratio, and the second does not have a ratio element
template <typename Scale1, typename FirstScale2, typename... OtherScales2>
class combine_scales_helper<
    Scale1,
    combined_scales<FirstScale2, OtherScales2...>,
    constraints<
        std::enable_if_t<is_ratio_v<Scale1>>,
        std::enable_if_t<!is_identity_ratio_v<Scale1>>,
        std::enable_if_t<!is_ratio_v<FirstScale2>>>> {
 public:
  /// @brief The resulting scale factor type
  using type = combined_scales<Scale1, FirstScale2, OtherScales2...>;
};

/// @brief Helper class for combining two scales. The result is a type that represents the product of the two scales
/// @tparam Scale1 the first scale to combine
/// @tparam FirstScale2 the first element of the second scale to combine
/// @tparam OtherScales2 the remaining elements of the second scale to combine
///
/// @details This is where the first is a ratio, and the second has a ratio element
template <typename Scale1, typename FirstScale2, typename... OtherScales2>
class combine_scales_helper<
    Scale1,
    combined_scales<FirstScale2, OtherScales2...>,
    constraints<std::enable_if_t<is_ratio_v<Scale1>>, std::enable_if_t<is_ratio_v<FirstScale2>>>> {
 public:
  /// @brief The resulting scale factor type
  using type = combine_scales_t<std::ratio_multiply<Scale1, FirstScale2>, combined_scales<OtherScales2...>>;
};

/// @brief Helper class for combining two scales. The result is a type that represents the product of the two scales
/// @tparam Scale1 the first scale to combine
/// @tparam Scales2 the elements of the second scale to combine
///
/// @details This is where the first is an identity ratio, and the second is a combined scale
template <typename Scale1, typename... Scales2>
class combine_scales_helper<
    Scale1,
    combined_scales<Scales2...>,
    constraints<std::enable_if_t<is_identity_ratio_v<Scale1>>>> {
 public:
  /// @brief The resulting scale factor type
  using type = combined_scales<Scales2...>;
};

/// @brief Implementation class for a quantity unit that is another unit scaled by some scale factor
/// @tparam BaseUnit the underlying unit
/// @tparam BaseScale the existing scale factor
/// @tparam Scale the additional scale factor
template <typename BaseUnit, typename BaseScale, typename Scale>
class scaled_unit_helper<
    scaled_unit_impl<BaseUnit, BaseScale>,
    Scale,
    constraints<std::enable_if_t<!is_identity_ratio_v<Scale>>>> {
  static_assert(is_valid_scale_v<Scale>, "The scale factor must be a ratio or a class wrapping a floating point scale");

 public:
  /// @brief The type to use to represent the scaled unit
  using type = scaled_unit<BaseUnit, combine_scales_t<BaseScale, Scale>>;
};

/// @brief Implementation class for a quantity unit that is another unit scaled by some scale factor
/// @tparam BaseUnit the underlying unit
/// @tparam Scale the scale factor
///
/// This is for the case that the @c Scale is a ratio of 1
template <typename BaseUnit, typename Scale>
class scaled_unit_helper<BaseUnit, Scale, constraints<std::enable_if_t<is_identity_ratio_v<Scale>>>> {
  static_assert(is_unit_v<BaseUnit>, "The supplied BaseUnit must be a valid quantity unit");
  static_assert(is_valid_scale_v<Scale>, "The scale factor must be a ratio or a class wrapping a floating point scale");

 public:
  /// @brief The type to use to represent the scaled unit
  using type = BaseUnit;
};

/// @brief Helper class to select the implementation of an inverse of a scale
/// @tparam Type the type to invert
template <typename Type>
class inverse_unit_scale_helper<Type, constraints<std::enable_if_t<is_ratio_v<Type>>>> {
 public:
  /// @brief A type representing the inverse scale
  using type = std::ratio<Type::den, Type::num>;
};

/// @brief Helper class to select the implementation of an inverse of a scale
/// @tparam Type the type to invert
template <typename Type>
class inverse_unit_scale_helper<inverse_unit_scale<Type>> {
 public:
  /// @brief A type representing the inverse scale
  using type = Type;
};

}  // namespace scaled_unit_detail

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_SCALED_UNIT_HPP_
