// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/variant/detail/bad_variant_access_exceptions_disabled.hpp"

#include <gtest/gtest.h>

#include "arene/base/variant/traits.hpp"

namespace {

using ::arene::base::variant_npos;
using ::arene::base::variant_detail::variant_must_have_value;

/// @test When `arene::base::variant_detail::variant_must_have_value(requested, index)` is invoked with
/// `requested==index`, then it is a nop.
TEST(VariantMustHaveValue, RequestedEqualsIndexIsNop) { variant_must_have_value(0U, 0U); }

/// @test When `arene::base::variant_detail::variant_must_have_value(requested, index)` is invoked with
/// `index==variant_npos`, then it is an `ARENE_PRECONDITION` violation.
TEST(VariantMustHaveValueDeathTest, IndexEqualsVariantNposIsPreconditionViolation) {
  ASSERT_DEATH(variant_must_have_value(0U, variant_npos), "Precondition");
}

/// @test When `arene::base::variant_detail::variant_must_have_value(requested, index)` is invoked with
/// `requested!=index`, then it is an `ARENE_PRECONDITION` violation.
TEST(VariantMustHaveValueDeathTest, RequestedNotEqualsIndexIsPreconditionViolation) {
  ASSERT_DEATH(variant_must_have_value(0U, 1U), "Precondition");
  ASSERT_DEATH(variant_must_have_value(1U, 0U), "Precondition");
  ASSERT_DEATH(variant_must_have_value(1U, 2U), "Precondition");
  ASSERT_DEATH(variant_must_have_value(2U, 1U), "Precondition");
}

}  // namespace
