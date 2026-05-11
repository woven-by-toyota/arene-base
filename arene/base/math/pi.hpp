// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_PI_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_PI_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace numbers {

namespace pi_detail {
/// @brief Undefined function used in the primary definition of @c pi_v to trigger an error if a type other than @c
/// float or @c double is used to instantiate it, without an explicit specialization
/// @tparam Type The type used to instantiate @c pi_v
/// @return This function is declared to return a value of the specified type, but no definition is provided, so nothing
/// is actually returned
template <typename Type>
auto instantiation_of_unspecialized_value() -> Type;

/// @brief The value of Pi, in the specified floating point type. @c Type must be @c float or @c double , or a type for
/// which an explicit specialization of @c pi_v has been made
/// @tparam Type The chosen type
template <typename Type, typename = constraints<>>
extern constexpr Type pi_v{pi_detail::instantiation_of_unspecialized_value<Type>()};

/// @brief The value of Pi as a @c float
template <typename Type>
extern constexpr float pi_v<Type, constraints<std::enable_if_t<std::is_same<Type, float>::value>>>{
    3.141592653589793238462643383279F
};

/// @brief The value of Pi as a @c double
template <typename Type>
extern constexpr double pi_v<Type, constraints<std::enable_if_t<std::is_same<Type, double>::value>>>{
    3.141592653589793238462643383279
};

}  // namespace pi_detail

/// @brief The value of Pi, in the specified floating point type. @c Type must be @c float or @c double , or a type for
/// which an explicit specialization of @c pi_v has been made
/// @tparam Type The chosen type
template <typename Type>
extern constexpr Type pi_v{pi_detail::pi_v<Type>};

// parasoft-begin-suppress AUTOSAR-M7_3_3-a CERT_CPP-DCL59-a "Per-TU references all refer to the same object"
namespace {
// parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: pi_v is initialized"
/// @brief The value of Pi as a @c double
// NOLINTNEXTLINE(readability-identifier-length)
ARENE_MAYBE_UNUSED constexpr double const& pi{pi_v<double>};
// parasoft-end-suppress CERT_CPP-DCL56-a
}  // namespace
   // parasoft-end-suppress AUTOSAR-M7_3_3-a CERT_CPP-DCL59-a
   //
}  // namespace numbers
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_PI_HPP_
