// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/mdspan/detail/extract_dynamic_extents_for.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/tests/test_index_types.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/is_signed.hpp"
#include "arene/base/stdlib_choice/is_unsigned.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using arene::base::array;
using arene::base::dextents;
using arene::base::mdspan_detail::extract_dynamic_extents_for;

template <class T>
struct MdspanDetailExtractDynamicExtentsFor : ::testing::Test {};

TYPED_TEST_SUITE(MdspanDetailExtractDynamicExtentsFor, ::test::index_types, );

/// @test @c extract_dynamic_extents_for triggers a precondition violation when an extents value is negative
CONDITIONAL_TYPED_TEST(
    MdspanDetailExtractDynamicExtentsFor,
    IsNonNegativePrecondition,
    std::is_signed<TypeParam>::value
) {
  ASSERT_DEATH(
      (extract_dynamic_extents_for<dextents<TypeParam, 1>>(array<TypeParam, 1>{static_cast<TypeParam>(-1)})),
      "Precondition"
  );
}

/// @test @c extract_dynamic_extents_for triggers a precondition violation when an extents value is not representable in
/// the specified type
CONDITIONAL_TYPED_TEST(
    MdspanDetailExtractDynamicExtentsFor,
    IsRepresentableAs,
    !(std::is_unsigned<TypeParam>::value && sizeof(TypeParam) >= sizeof(std::uint64_t))
) {
  ASSERT_DEATH(
      (extract_dynamic_extents_for<dextents<TypeParam, 1>>(array<std::uint64_t, 1>{static_cast<std::uint64_t>(-1)})),
      "Precondition"
  );
}

}  // namespace
