// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/array/to_array.hpp"
#include "arene/base/math/float_properties.hpp"
#include "arene/base/math/tests/float_utilities.hpp"
#include "arene/base/math/ulp_tolerance.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_macros.hpp"
#include "testlibs/utilities/float32.hpp"

namespace {

using arene::base::isnan;
using arene::base::ulp_tolerance;

constexpr auto comparators = arene::base::to_array({
    ulp_tolerance(0LL),
    ulp_tolerance(1LL),
    ulp_tolerance(4LL),
    ulp_tolerance(5LL),
    ulp_tolerance(6LL),
    ulp_tolerance(10LL),
    // NOLINTNEXTLINE(google-runtime-int) Using long long in all comparators for clarity since it has a literal
    ulp_tolerance(std::numeric_limits<long long>::max()),
});

template <class T>
struct UlpTolerance : ::testing::Test {};

using test_types = ::testing::Types<float, double, ::testing::float32>;

TYPED_TEST_SUITE(UlpTolerance, test_types, );

/// @test All non-NaN values are within any @c ulp_tolerance of themselves
ARENE_TYPED_PROPERTY(UlpTolerance, SameValueIsAlwaysInTolerance, (TypeParam val)) {
  ARENE_PROP_PRE(!isnan(val));

  for (auto const& comp : comparators) {
    ARENE_PROP_ASSERT(comp(val, val));
  }
}

/// @test If a starting value is shifted by N ULPs in either direction, it compares equal to the original value when the
/// comparator has a tolerance of N or greater
ARENE_TYPED_PROPERTY(UlpTolerance, ToleranceBehaviourMatchesSizeOfShift, (TypeParam val)) {
  ARENE_PROP_PRE(!isnan(val));

  constexpr std::int32_t shift_size{5};
  TypeParam const shifted_up{arene::base::float_testing::shift_in_direction(val, shift_size, true)};
  TypeParam const shifted_down{arene::base::float_testing::shift_in_direction(val, shift_size, false)};

  for (auto const& comp : comparators) {
    bool const should_pass{comp.tolerance() >= shift_size};

    if (!isnan(shifted_up)) {
      ARENE_PROP_ASSERT(comp(val, shifted_up) == should_pass);
      ARENE_PROP_ASSERT(comp(shifted_up, val) == should_pass);
    }

    if (!isnan(shifted_down)) {
      ARENE_PROP_ASSERT(comp(val, shifted_down) == should_pass);
      ARENE_PROP_ASSERT(comp(shifted_down, val) == should_pass);
    }
  }
}

/// @test No value compares within tolerance of a NaN
ARENE_TYPED_PROPERTY(UlpTolerance, NanIsAlwaysOutOfTolerance, (TypeParam val)) {
  ARENE_PROP_PRE(!isnan(val));
  constexpr auto nan = std::numeric_limits<TypeParam>::quiet_NaN();

  for (auto const& comp : comparators) {
    ARENE_PROP_ASSERT(!comp(val, nan));
    ARENE_PROP_ASSERT(!comp(nan, val));
  }
}

/// @test A comparator with negative tolerance always returns false, even with the same argument twice
ARENE_TYPED_PROPERTY(UlpTolerance, NegativeToleranceIsAlwaysOutOfTolerance, (TypeParam left, TypeParam right)) {
  for (auto const& comp : comparators) {
    if (comp.tolerance() == 0) {
      continue;
    }

    auto const negative_comp = ulp_tolerance(-comp.tolerance());
    ARENE_PROP_ASSERT(!negative_comp(left, right));
    ARENE_PROP_ASSERT(!negative_comp(left, left));
    ARENE_PROP_ASSERT(!negative_comp(right, right));
  }
}

}  // namespace
