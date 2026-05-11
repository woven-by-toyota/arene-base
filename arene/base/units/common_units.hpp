// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_COMMON_UNITS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_COMMON_UNITS_HPP_

#include "arene/base/math/pi.hpp"
#include "arene/base/stdlib_choice/ratio.hpp"
#include "arene/base/units/quantity_kind.hpp"
#include "arene/base/units/quantity_origin.hpp"
#include "arene/base/units/quantity_unit.hpp"
#include "arene/base/units/scaled_unit.hpp"

namespace arene {
namespace base {
namespace units {

namespace scales {
/// @brief A class to represent a scale factor of Pi
class pi {
 public:
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "This member is the public API"
  /// @brief The value of the scale factor
  static constexpr double value{arene::base::numbers::pi};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a
};
}  // namespace scales

/// @brief Base quantity kind for all numeric quantity kinds
class number : public quantity_kind<number> {};

/// @brief Base quantity kind for the linear extent in space between any two points
class length;
/// @brief The SI base quantity unit for length
class meter : public quantity_unit<meter, length> {};
/// @brief Base quantity kind for the linear extent in space between any two points
class length : public quantity_kind<length, meter> {};

/// @brief Base quantity kind for the property of a body which expresses itself in terms of inertia with regard to
/// changes in its state of motion as well as its gravitational attraction to other bodies
class mass;
/// @brief A quantity unit for mass
///
/// This is used as the base unit, rather than @c kilogram so that the SI prefixes work appropriately, and we don't have
/// @c kilokilogram or @c millikilogram
///
/// The default unit for @c mass is still @c kilogram
class gram : public quantity_unit<gram, mass> {};
/// @brief The SI base quantity unit for mass
using kilogram = scaled_unit<gram, std::kilo>;
/// @brief Base quantity kind for thr property of a body which expresses itself in terms of inertia with regard to
/// changes in its state of motion as well as its gravitational attraction to other bodies
class mass : public quantity_kind<mass, kilogram> {};

/// @brief Base quantity kind for the time difference between two events
class time;
/// @brief The SI base quantity unit for time
class second : public quantity_unit<second, time> {};
// parasoft-begin-suppress CERT_CPP-DCL51-f-3 "False positive: time not reserved in this context"
/// @brief Base quantity kind for time
class time : public quantity_kind<time, second> {};
// parasoft-end-suppress CERT_CPP-DCL51-f-3

/// @brief An alternative name for @c time
using duration = time;

/// @brief Base quantity kind for the quotient of the net electric charge transferred through a surface in a
/// quasi-infinitessimal time interval, and the duration of that time interval
class electric_current;
/// @brief The SI base quantity unit for electric current
class ampere : public quantity_unit<ampere, electric_current> {};
/// @brief Base quantity kind for the quotient of the net electric charge transferred through a surface in a
/// quasi-infinitessimal time interval, and the duration of that time interval
class electric_current : public quantity_kind<electric_current, ampere> {};

/// @brief A tag type for absolute zero, the origin of the Kelvin scale
class absolute_zero : public quantity_origin<absolute_zero> {};

/// @brief Base quantity kind for the partial derivative of internal energy with respect to entropy at constant volume
/// and constant number of particles in the system
class thermodynamic_temperature;
/// @brief Alias for thermodynamic temperature
using temperature = thermodynamic_temperature;
/// @brief Base quantity kind for the partial derivative of internal energy with respect to entropy at constant volume
/// and constant number of particles in the system
class kelvin : public quantity_unit<kelvin, thermodynamic_temperature> {};
/// @brief Base quantity kind for thermodynamic temperature
class thermodynamic_temperature : public quantity_kind<thermodynamic_temperature, absolute_zero, kelvin> {};

/// @brief Quantity kind for the number of entities of a given kind in a system
class number_of_entities : public quantity_kind<number_of_entities, number> {};
/// @brief Base quantity kind for the quotient of the number of entites of a given kind in a system and the Avogadro
/// constant
class amount_of_substance;
/// @brief The SI base quantity unit for amount of substance
class mole : public quantity_unit<mole, amount_of_substance> {};
/// @brief Base quantity kind for the quotient of the number of entites of a given kind in a system and the Avogadro
/// constant
class amount_of_substance : public quantity_kind<amount_of_substance, number_of_entities, mole> {};

/// @brief Base quantity kind for the density of luminous flux with respect to solid angle in a specified direction
class luminous_intensity;
/// @brief The SI base quantity unit for luminous intensity
class candela : public quantity_unit<candela, luminous_intensity> {};
/// @brief Base quantity kind for the density of luminous flux with respect to solid angle in a specified direction
class luminous_intensity : public quantity_kind<luminous_intensity, candela> {};

/// @brief A quantity unit for minutes
// NOLINTNEXTLINE(readability-magic-numbers)
using minute = scaled_unit<second, std::ratio<60, 1>>;
/// @brief A quantity unit for hours
// NOLINTNEXTLINE(readability-magic-numbers)
using hour = scaled_unit<minute, std::ratio<60, 1>>;
/// @brief A quantity unit for milliseconds
using millisecond = scaled_unit<second, std::milli>;
/// @brief A quantity unit for microseconds
using microsecond = scaled_unit<second, std::micro>;
/// @brief A quantity unit for nanoseconds
using nanosecond = scaled_unit<second, std::nano>;

/// @brief A quantity unit for kilometers
using kilometer = scaled_unit<meter, std::kilo>;
/// @brief A quantity unit for centimeters
using centimeter = scaled_unit<meter, std::centi>;
/// @brief A quantity unit for millimeters
using millimeter = scaled_unit<meter, std::milli>;
/// @brief A quantity unit for micrometers
using micrometer = scaled_unit<meter, std::micro>;
/// @brief A quantity unit for miles
// NOLINTNEXTLINE(readability-magic-numbers)
using mile = scaled_unit<meter, std::ratio<1609344, 1000>>;

/// @brief A quantity unit for kilograms
using kilogram = scaled_unit<gram, std::kilo>;
/// @brief A quantity unit for milligrams
using milligram = scaled_unit<gram, std::milli>;
/// @brief A quantity unit for micrograms
using microgram = scaled_unit<gram, std::micro>;

/// @brief An alternative name for Ampere
using amp = ampere;
/// @brief A quantity unit for milliamps
using milliamp = scaled_unit<ampere, std::milli>;

/// @brief A quantity kind for a measure of a geometric figure formed by two rays emanating from a common point, given
/// by the arc length of the included arc of a circle centred at the common point divided by the radius of that circle
class angular_measure;

/// @brief A quantity unit for @c angular_measure
class radian : public quantity_unit<radian, angular_measure> {};

/// @brief A quantity kind for a measure of a geometric figure formed by two rays emanating from a common point, given
/// by the arc length of the included arc of a circle centred at the common point divided by the radius of that circle
class angular_measure : public quantity_kind<angular_measure, number, radian> {};

/// @brief An alternate name for @c angular_measure
using plane_angle = angular_measure;

/// @brief A quantity unit for @c angular_measure
///
/// Pi radians is 180 degrees
class angular_degree
    : public quantity_unit<
          angular_degree,
          // NOLINTNEXTLINE(readability-magic-numbers)
          scaled_unit<scaled_unit<radian, inverse_unit_scale_t<scales::pi>>, std::ratio<180, 1>>> {};

}  // namespace units
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_COMMON_UNITS_HPP_
