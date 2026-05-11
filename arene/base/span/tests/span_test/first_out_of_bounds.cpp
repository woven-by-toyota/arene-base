// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file first.cpp
/// @brief Unit tests for @c span<T,Extent>::first
///

#include <gtest/gtest.h>

#include "arene/base/span/tests/span_test/helpers.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/min_value_overload.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"

namespace {

namespace span_test = ::arene::base::tests::span_test;

template <typename T>
using FirstOutOfBoundsDeathTest = span_test::SpanTest<T>;

TYPED_TEST_SUITE_P(FirstOutOfBoundsDeathTest);
/// @test Validates that calling @c span<T,Extent>::first(count) with @c count>size() is an @c ARENE_PRECONDITION
/// violation .
TYPED_TEST_P(FirstOutOfBoundsDeathTest, ParamCountIsPreconditionViolationIfCountGreaterThanSize) {
  auto const the_span =
      TypeParam{this->backing_data.begin(), std::min(this->backing_data.size(), TestFixture::template_extent())};
  ASSERT_DEATH(std::ignore = the_span.first(the_span.size() + 1), "Precondition");
  ASSERT_DEATH(std::ignore = the_span.first(std::numeric_limits<typename TypeParam::size_type>::max()), "Precondition");
}

REGISTER_TYPED_TEST_SUITE_P(FirstOutOfBoundsDeathTest, ParamCountIsPreconditionViolationIfCountGreaterThanSize);

INSTANTIATE_TYPED_TEST_SUITE_P(AllSpans, FirstOutOfBoundsDeathTest, span_test::all_spans, );

}  // namespace
