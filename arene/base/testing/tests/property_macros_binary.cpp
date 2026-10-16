// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/testing/detail/any_prng_ref.hpp"
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_macros.hpp"

// NOLINTBEGIN(readability-identifier-length,misc-redundant-expression)

namespace {

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "These tests check exact equality, even for floating point types");

ARENE_PROPERTY(AreneProperty, FailureNoParams) { ARENE_PROP_ASSERT(false); }

ARENE_PROPERTY(AreneProperty, FailureWithEmptyParams, ()) { ARENE_PROP_ASSERT(false); }

ARENE_PROPERTY(AreneProperty, FailureWithIntIntParams, (int, int)) { ARENE_PROP_ASSERT(false); }

ARENE_PROPERTY(AreneProperty, FailureWithMultipleAssertions) {
  ARENE_PROP_ASSERT(true);
  ARENE_PROP_ASSERT(1 + 1 == 1);
  ARENE_PROP_ASSERT(false);
}

ARENE_PROPERTY(AreneProperty, DiscardsReached) { ARENE_PROP_PRE(false); }

ARENE_PROPERTY(AreneProperty, EmptyBody) {}

ARENE_PROPERTY(AreneProperty, PrngAvailable) {
  auto const unused = prng();
  std::ignore = unused;
}

ARENE_PROPERTY(AreneProperty, SatisfiedProperty) { ARENE_PROP_ASSERT(1 + 1 == 2); }

template <class T>
struct AreneTypedProperty : ::testing::Test {};

using arithmetic_types = ::testing::Types<std::size_t, double>;

TYPED_TEST_SUITE(AreneTypedProperty, arithmetic_types, );

ARENE_TYPED_PROPERTY(AreneTypedProperty, FailureNoParams) { ARENE_PROP_ASSERT(false); }

ARENE_TYPED_PROPERTY(AreneTypedProperty, FailureWithEmptyParams, ()) { ARENE_PROP_ASSERT(false); }

ARENE_TYPED_PROPERTY(AreneTypedProperty, FailureWithIntTypeParams, (TypeParam, TypeParam)) { ARENE_PROP_ASSERT(false); }

ARENE_TYPED_PROPERTY(AreneTypedProperty, FailureWithMultipleAssertions, (TypeParam a)) {
  ARENE_PROP_ASSERT(true);

  // This will fail reliably for both integral and floating point types, unlike something like a+1 == a
  ARENE_PROP_ASSERT(a != TypeParam{} && a == TypeParam{});

  ARENE_PROP_ASSERT(false);
}

ARENE_TYPED_PROPERTY(AreneTypedProperty, DiscardsReached) { ARENE_PROP_PRE(false); }

ARENE_TYPED_PROPERTY(AreneTypedProperty, EmptyBody) {}

ARENE_TYPED_PROPERTY(AreneTypedProperty, PrngAvailable) {
  auto const unused = prng();
  std::ignore = unused;
}

ARENE_TYPED_PROPERTY(AreneTypedProperty, SatisfiedProperty, (TypeParam a)) {
  if (a == a) {
    ARENE_PROP_ASSERT(a + TypeParam{} == a);
  }
}

ARENE_IGNORE_END();

}  // namespace

// NOLINTEND(readability-identifier-length,misc-redundant-expression)
