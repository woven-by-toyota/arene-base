// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/math/float_properties.hpp"
#include "arene/base/math/tests/float_utilities.hpp"
#include "arene/base/math/ulp_distance.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_macros.hpp"
#include "testlibs/utilities/float32.hpp"

namespace {

using arene::base::ulp_distance;

template <class T>
struct UlpDistance : ::testing::Test {};

using test_types = ::testing::Types<float, double, ::testing::float32>;

TYPED_TEST_SUITE(UlpDistance, test_types, );

/// @test @c ulp_distance of any non-NaN value with itself is always zero.
ARENE_TYPED_PROPERTY(UlpDistance, SameValueIsAlwaysRepresentable, (TypeParam val)) {
  ARENE_PROP_PRE(!arene::base::isnan(val));

  ARENE_PROP_ASSERT(0 == ulp_distance<>(val, val).value());
}

/// @test For types narrower than @c double, the ULP distance between any
/// two non-NaN values is always representable in @c std::int64_t.
ARENE_TYPED_PROPERTY(UlpDistance, NonDoubleDistanceAlwaysRepresentable, (TypeParam left, TypeParam right)) {
  if (!std::is_same<TypeParam, double>::value) {
    ARENE_PROP_PRE(!arene::base::isnan(left));
    ARENE_PROP_PRE(!arene::base::isnan(right));

    ARENE_PROP_ASSERT(ulp_distance<std::int64_t>(left, right).has_value());
  }
}

/// @test The ULP distance between two values equals the number of
/// successive @c std::nextafter calls needed to walk from one to the other.
ARENE_TYPED_PROPERTY(UlpDistance, ResultMatchesNextAfter, (std::uint8_t steps, TypeParam val)) {
  ARENE_PROP_PRE(!arene::base::isnan(val));

  auto const shifted_val = arene::base::float_testing::shift_in_direction(val, steps, val < TypeParam{});

  // If stepping from val -> shifted_val crosses 0, then the final comparison won't be correct as ulp_distance treats
  // +0.0 and -0.0 as having a distance of 0. Discard cases where val and shifted_val don't have the same sign.
  ARENE_PROP_PRE((val > TypeParam{}) == (shifted_val > TypeParam{}));

  if (val < TypeParam{}) {
    ARENE_PROP_ASSERT(static_cast<std::int64_t>(steps) == ulp_distance<>(val, shifted_val).value());
  } else {
    ARENE_PROP_ASSERT(static_cast<std::int64_t>(steps) == ulp_distance<>(shifted_val, val).value());
  }
}

}  // namespace
