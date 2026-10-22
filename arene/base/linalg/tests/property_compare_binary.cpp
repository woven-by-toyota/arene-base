// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include <gtest/gtest.h>

#include "arene/base/bit/bit_cast.hpp"
#include "arene/base/linalg/tests/property_compare.hpp"
#include "arene/base/math/ulp_tolerance.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"

namespace {

using arene::base::bit_cast;
using arene::base::ulp_tolerance;

// GCC and Clang differ on the __LINE__ of a multiline macro
// clang-format off
ARENE_PROPERTY(LinalgAssertCompare, IntNotEqual) {
  ARENE_PROP_LINALG_ASSERT_COMPARE(std::int32_t{1}, std::int32_t{0});
}

ARENE_PROPERTY(LinalgAssertCompare, FloatNotEqual) {
  ARENE_PROP_LINALG_ASSERT_COMPARE(bit_cast<float>(std::int32_t{1}), bit_cast<float>(std::int32_t{0}));
}

ARENE_PROPERTY(LinalgAssertCompare, DoubleNotEqual) {
  ARENE_PROP_LINALG_ASSERT_COMPARE(bit_cast<double>(std::int64_t{1}), bit_cast<double>(std::int64_t{0}));
}

ARENE_PROPERTY(LinalgAssertCompare, FloatNotWithinTolerance) {
  ARENE_PROP_LINALG_ASSERT_COMPARE(ulp_tolerance(1), bit_cast<float>(std::int32_t{2}), bit_cast<float>(std::int32_t{0}));
}

ARENE_PROPERTY(LinalgAssertCompare, DoubleNotWithinTolerance) {
  ARENE_PROP_LINALG_ASSERT_COMPARE(ulp_tolerance(1), bit_cast<double>(std::int64_t{2}), bit_cast<double>(std::int64_t{0}));
}
// clang-format on

}  // namespace
