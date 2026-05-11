// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file construction_default.cpp
/// @brief Unit tests for span's default constructor.
///

#include <gtest/gtest.h>

#include "arene/base/span/tests/span_test/helpers.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

namespace span_test = ::arene::base::tests::span_test;

template <typename T>
struct DefaultConstruction : span_test::SpanTest<T> {};

TYPED_TEST_SUITE_P(DefaultConstruction);

/// @test Validates that @c span<T,dynamic_extent>::size() and @c span<T,dynamic_extent>::ssize() are both equal to
/// @c 0 for a default constructed span .
TYPED_TEST_P(DefaultConstruction, PostCtorSizeAndSSizeAreZero) {
  STATIC_ASSERT_EQ(TypeParam{}.size(), 0U);
  STATIC_ASSERT_EQ(TypeParam{}.ssize(), 0);
}
/// @test Validates that @c span<T,dynamic_extent>::size_bytes() is @c 0 for a default constructed span .
TYPED_TEST_P(DefaultConstruction, PostCtorSizeBytesIsZero) { STATIC_ASSERT_EQ(TypeParam{}.size_bytes(), 0U); }
/// @test Validates that @c span<T,dynamic_extent>::empty() is @c true for a default constructed span .
TYPED_TEST_P(DefaultConstruction, PostCtorIsEmpty) { STATIC_ASSERT_TRUE(TypeParam{}.empty()); }
/// @test Validates that @c span<T,dynamic_extent>::data() is @c nullptr for a default constructed span .
TYPED_TEST_P(DefaultConstruction, PostCtorDataIsNullptr) { STATIC_ASSERT_EQ(TypeParam{}.data(), nullptr); }

REGISTER_TYPED_TEST_SUITE_P(
    DefaultConstruction,
    PostCtorSizeAndSSizeAreZero,
    PostCtorSizeBytesIsZero,
    PostCtorIsEmpty,
    PostCtorDataIsNullptr
);

INSTANTIATE_TYPED_TEST_SUITE_P(AllDynamicExtentSpans, DefaultConstruction, span_test::all_dynamic_extent_spans, );

}  // namespace
