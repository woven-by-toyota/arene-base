// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file element_access.cpp
/// @brief Unit tests for @c span<T,Extent>::front and @c span<T,Extent>::back
///

#include <gtest/gtest.h>

#include "arene/base/span/tests/span_test/helpers.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"

namespace {

namespace span_test = ::arene::base::tests::span_test;

template <typename T>
struct FrontOfEmptySpanDeathTest : span_test::SpanTest<T> {};

TYPED_TEST_SUITE_P(FrontOfEmptySpanDeathTest);
/// @test Validates that calling @c span<T,Extent>::front() is an @c ARENE_PRECONDTION violation if
/// @c span<T,Extent>::empty()==true .
TYPED_TEST_P(FrontOfEmptySpanDeathTest, IsPreconditionViolation) {
  auto const the_span = TypeParam{this->backing_data.begin(), 0};
  EXPECT_DEATH(std::ignore = the_span.front();, "Precondition");
}

REGISTER_TYPED_TEST_SUITE_P(FrontOfEmptySpanDeathTest, IsPreconditionViolation);

INSTANTIATE_TYPED_TEST_SUITE_P(MutableEmptySpans, FrontOfEmptySpanDeathTest, span_test::mutable_empty_spans, );
INSTANTIATE_TYPED_TEST_SUITE_P(ConstEmptySpans, FrontOfEmptySpanDeathTest, span_test::const_empty_spans, );

template <typename T>
struct BackOfEmptySpanDeathTest : span_test::SpanTest<T> {};

TYPED_TEST_SUITE_P(BackOfEmptySpanDeathTest);
/// @test Validates that calling @c span<T,Extent>::back() is an @c ARENE_PRECONDTION violation if
/// @c span<T,Extent>::empty()==true .
TYPED_TEST_P(BackOfEmptySpanDeathTest, IsPreconditionViolation) {
  auto const the_span = TypeParam{this->backing_data.begin(), 0};
  EXPECT_DEATH(std::ignore = the_span.back();, "Precondition");
}

REGISTER_TYPED_TEST_SUITE_P(BackOfEmptySpanDeathTest, IsPreconditionViolation);

INSTANTIATE_TYPED_TEST_SUITE_P(MutableEmptySpans, BackOfEmptySpanDeathTest, span_test::mutable_empty_spans, );
INSTANTIATE_TYPED_TEST_SUITE_P(ConstEmptySpans, BackOfEmptySpanDeathTest, span_test::const_empty_spans, );

}  // namespace
