// Copyright 2024, Toyota Motor Corporation

/// Examples for the @c units subpackage

#include <tuple>

#include "arene/base/units.hpp"

namespace units_examples_with_type_aliases {

//! [latlong_alias]
/// A type for latitude measurements
using latitude_t = double;
/// A type for longitude measurements
using longitude_t = double;

/// A struct representing a geographic position
struct geographic_position {
  /// The latitude
  latitude_t latitude;
  /// The longitude
  longitude_t longitude;
};

/// @brief Construct a geographic position from latitude and longitude
/// @param latitude The latitude in degrees
/// @param longitude The longitude in degrees
/// @returns geographic_position with the specified latitude and longitude
geographic_position make_position(latitude_t latitude, longitude_t longitude) { return {latitude, longitude}; }
//! [latlong_alias]

void foo() {
  //! [eiffel_tower_alias]
  // error latitude and longitude are in the wrong order
  latitude_t eiffel_tower_latitude = 48.8584;
  longitude_t eiffel_tower_longitude = 2.2945;
  auto eiffel_tower_position = make_position(eiffel_tower_longitude, eiffel_tower_latitude);
  //! [eiffel_tower_alias]
  std::ignore = eiffel_tower_position;
}
}  // namespace units_examples_with_type_aliases

namespace units_examples {

//! [latlong]
/// A tag type for latitude measurements
struct latitude_in_degrees {};
/// A tag type for longitude measurements
struct longitude_in_degrees {};

/// A struct representing a geographic position
struct geographic_position {
  /// The latitude
  arene::base::quantity<latitude_in_degrees> latitude;
  /// The longitude
  arene::base::quantity<longitude_in_degrees> longitude;
};

/// @brief Construct a geographic position from latitude and longitude
/// @param latitude The latitude in degrees
/// @param longitude The longitude in degrees
/// @returns geographic_position with the specified latitude and longitude
geographic_position make_position(
    arene::base::quantity<latitude_in_degrees> latitude,
    arene::base::quantity<longitude_in_degrees> longitude
) {
  return {latitude, longitude};
}
//! [latlong]

/// A tag type for a distance in kilometres
struct distance_in_kilometres {};

/// @brief Calculate the distance across the Earth's surface between two geographic positions
/// @param pos1 The first position
/// @param pos2 The second position
/// @returns arene::base::quantity<distance_in_kilometres> holding the distance between the two positions
arene::base::quantity<distance_in_kilometres> calculate_distance(geographic_position pos1, geographic_position pos2);

namespace latlong_usage {
void foo() {
  //! [eiffel_tower]
  auto eiffel_tower_position = make_position(
      arene::base::quantity<latitude_in_degrees>{48.8584},
      arene::base::quantity<longitude_in_degrees>{2.2945}
  );
  //! [eiffel_tower]
  auto taj_mahal_position = make_position(
      arene::base::quantity<latitude_in_degrees>{27.1751},
      arene::base::quantity<longitude_in_degrees>{78.0421}
  );

  auto distance = calculate_distance(eiffel_tower_position, taj_mahal_position);

  std::ignore = distance;
}
}  // namespace latlong_usage
}  // namespace units_examples

namespace {
//! [time_conversions_tags]
/// A tag type for time measured in hours
struct time_in_hours {};
/// A tag type for time measured in seconds
struct time_in_seconds {};
//! [time_conversions_tags]
}  // namespace

//! [time_conversions]
/// @brief Specialization of @c units_conversion_traits for converting @c time_in_hours to @c time_in_seconds
/// @tparam From the type of the unit being "converted"
template <>
struct arene::base::units_conversion_traits<time_in_hours, time_in_seconds> {
  /// @brief Boolean value that is @c true to indicate that @c time_in_hours can be converted to @c time_in_seconds
  ARENE_MAYBE_UNUSED static bool const compatible = true;

  /// @brief The scale factor to use when converting @c time_in_hours to @c time_in_seconds
  using scale_factor = std::ratio<3600, 1>;
};

/// @brief Specialization of @c units_conversion_traits for converting @c time_in_hours to @c time_in_seconds
/// @tparam From the type of the unit being "converted"
template <>
struct arene::base::units_conversion_traits<time_in_seconds, time_in_hours> {
  /// @brief Boolean value that is @c true to indicate that @c time_in_seconds can be converted to @c time_in_hours
  ARENE_MAYBE_UNUSED static bool const compatible = true;

  /// @brief The scale factor to use when converting @c time_in_seconds to @c time_in_hours
  using scale_factor = std::ratio<1, 3600>;
};
//! [time_conversions]

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "Allow floating point equality for examples");

namespace {
/// A tag type for a distance in metres
struct distance_in_metres {};
}  // namespace

void units_time_conversion_example() {
  //! [time_conversions_code]
  constexpr arene::base::quantity<time_in_hours> hours{42};
  constexpr arene::base::quantity<time_in_seconds> secs = hours;

  static_assert(secs.count_of<time_in_seconds>() == (42 * 3600), "Value must be right");
  constexpr arene::base::quantity<time_in_hours> hours2 = secs;
  static_assert(hours2 == hours, "Value must be right");
  //! [time_conversions_code]

  //! [conversion_comparison]
  constexpr arene::base::quantity<time_in_seconds> secs2 = secs * 2;
  static_assert(hours < secs2, "Value must be right");
  //! [conversion_comparison]

  //! [conversion_comparison_error]
  static_assert(
      !arene::base::
          is_equality_comparable_v<arene::base::quantity<time_in_hours>, arene::base::quantity<distance_in_metres>>,
      "Cannot compare time to distance"
  );
  //! [conversion_comparison_error]
}
ARENE_IGNORE_END();

void units_time_arithmetic_example() {
  //! [time_addition_and_subtraction]
  constexpr arene::base::quantity<time_in_seconds, std::uint32_t> sec1{42};
  constexpr arene::base::quantity<time_in_seconds, std::uint32_t> sec2{99};
  constexpr arene::base::quantity<time_in_seconds, std::uint32_t> sec3{sec1 + sec2};

  static_assert(sec3.count_of<time_in_seconds>() == 141, "Value must be right");
  static_assert((sec3 - sec2) == sec1, "Value must be right");

  constexpr arene::base::quantity<distance_in_metres, std::uint32_t> distance{99};

  // auto error = distance + sec1; // would be a compilation error

  //! [time_addition_and_subtraction]

  std::ignore = distance;
}

namespace {
//! [multiplication_and_division_tags]
/// A tag type representing an area in metres squared
struct area_in_metres_squared {};

/// A tag type representing a speed in metres per second
struct speed_in_metres_per_second {};
//! [multiplication_and_division_tags]
}  // namespace

//! [multiplication_and_division]
/// Specialization showing the supported operations on two distances: adding, subtracting and multiplying
template <>
struct arene::base::units_combination_traits<distance_in_metres, distance_in_metres> {
  /// The result of adding two distances is a distance
  using sum_type = distance_in_metres;
  /// The difference between two distances is a distance
  using difference_type = distance_in_metres;
  /// The product of two distances is an area
  using product_type = area_in_metres_squared;
};

/// Specialization showing the supported operations on a distance and a time: only division to create a speed is
/// supported
template <>
struct arene::base::units_combination_traits<distance_in_metres, time_in_seconds> {
  /// Dividing a distance by a time yields a speed
  using ratio_type = speed_in_metres_per_second;
};
//! [multiplication_and_division]
