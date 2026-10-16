// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_TESTS_FLOAT_TEST_PATTERNS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_TESTS_FLOAT_TEST_PATTERNS_HPP_

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"

namespace arene {
namespace base {
namespace testing {
namespace tests {

// The bit patterns below encode the IEEE-754 representation of each value. Assert that the platform's
// @c float and @c double are IEEE-754 so the patterns are meaningful.
static_assert(std::numeric_limits<float>::is_iec559, "float_test_patterns<float> assumes IEEE-754 single precision");
static_assert(std::numeric_limits<double>::is_iec559, "float_test_patterns<double> assumes IEEE-754 double precision");

/// @brief IEEE-754 bit patterns for a floating-point type, parameterized so tests can feed them through a bit
/// generator.
/// @tparam T The floating-point type to parameterize over; must be @c float or @c double.
template <typename T>
struct float_test_patterns;

/// @brief IEEE-754 single-precision bit patterns.
template <>
struct float_test_patterns<float> {
  using bits_type = std::uint32_t;

  static constexpr bits_type quiet_nan{0x7FC00000U};
  static constexpr bits_type signaling_nan{0x7FA00000U};
  static constexpr bits_type negative_nan{0xFFC00000U};
  static constexpr bits_type zero{0x00000000U};
  static constexpr bits_type negative_zero{0x80000000U};
  static constexpr bits_type denorm_min{0x00000001U};
  static constexpr bits_type min_normal{0x00800000U};
  static constexpr bits_type max_finite{0x7F7FFFFFU};
  static constexpr bits_type one{0x3F800000U};
  static constexpr bits_type one_plus_epsilon{0x3F800001U};
  static constexpr bits_type negative_one{0xBF800000U};
  static constexpr bits_type two{0x40000000U};
  static constexpr bits_type positive_infinity{0x7F800000U};
  static constexpr bits_type negative_infinity{0xFF800000U};
};

/// @brief IEEE-754 double-precision bit patterns.
template <>
struct float_test_patterns<double> {
  using bits_type = std::uint64_t;

  static constexpr bits_type quiet_nan{0x7FF8000000000000ULL};
  static constexpr bits_type signaling_nan{0x7FF4000000000000ULL};
  static constexpr bits_type negative_nan{0xFFF8000000000000ULL};
  static constexpr bits_type zero{0x0000000000000000ULL};
  static constexpr bits_type negative_zero{0x8000000000000000ULL};
  static constexpr bits_type denorm_min{0x0000000000000001ULL};
  static constexpr bits_type min_normal{0x0010000000000000ULL};
  static constexpr bits_type max_finite{0x7FEFFFFFFFFFFFFFULL};
  static constexpr bits_type one{0x3FF0000000000000ULL};
  static constexpr bits_type one_plus_epsilon{0x3FF0000000000001ULL};
  static constexpr bits_type negative_one{0xBFF0000000000000ULL};
  static constexpr bits_type two{0x4000000000000000ULL};
  static constexpr bits_type positive_infinity{0x7FF0000000000000ULL};
  static constexpr bits_type negative_infinity{0xFFF0000000000000ULL};
};

}  // namespace tests
}  // namespace testing
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_TESTS_FLOAT_TEST_PATTERNS_HPP_
