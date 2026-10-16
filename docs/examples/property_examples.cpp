// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Examples for property-based testing

#include <iostream>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_macros.hpp"

// clang-format off

namespace property {
//! [arene_property]
ARENE_PROPERTY(Arithmetic, AddCommutes, (int a, int b)) {
  ARENE_PROP_ASSERT(a + b == b + a);
}
//! [arene_property]
}

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "This example illustrates equality, even for floating point");

namespace typed_property {
//! [arene_typed_property]
template <class T>
struct Arithmetic : ::testing::Test {};

using arithmetic_types = ::testing::Types<int, double>;

TYPED_TEST_SUITE(Arithmetic, arithmetic_types, );

ARENE_TYPED_PROPERTY(Arithmetic, AddCommutes, (TypeParam a, TypeParam b)) {
  ARENE_PROP_ASSERT(a + b == b + a);
}
//! [arene_typed_property]
}

auto function_using_prop_check() {
//! [arene_prop_check]
auto const summary = ARENE_PROP_CHECK(
  "addition commutes",
  (int a, int b),
  {
    ARENE_PROP_ASSERT(a + b == b + a);
  }
);
std::cerr << summary << "\n";
//! [arene_prop_check]
}

ARENE_IGNORE_END();

// clang-format on
