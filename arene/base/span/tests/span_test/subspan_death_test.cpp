// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file subspan_death_test.cpp
/// @brief Death tests for @c span<T,Extent>::subspan
///

#include <gtest/gtest.h>

#include "arene/base/span/tests/span_test/helpers.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/max_value_overload.hpp"
#include "arene/base/stdlib_choice/min_value_overload.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"

namespace {

namespace span_test = ::arene::base::tests::span_test;

template <typename T>
struct SubspanOutOfBoundsDeathTest : span_test::SpanTest<T> {};

TYPED_TEST_SUITE_P(SubspanOutOfBoundsDeathTest);
/// @test Validates that it is an @c ARENE_PRECONDITION violation to call @c span<T,Extent>::subspan(offset,count) with
/// either of the following:
/// - @c offset>size()
/// - @c count!=dynamic_extent and @c offset+count>size() .
TYPED_TEST_P(SubspanOutOfBoundsDeathTest, ParamArgsIsPreconditionViolationIfOffsetPlusCountIsGreaterThanSize) {
  auto const the_span =
      TypeParam{this->backing_data.begin(), std::min(this->backing_data.size(), TestFixture::template_extent())};
  ASSERT_DEATH(std::ignore = the_span.subspan(the_span.size() + 1), "Precondition");
  ASSERT_DEATH(
      std::ignore = the_span.subspan(std::numeric_limits<typename TypeParam::size_type>::max()),
      "Precondition"
  );
  ASSERT_DEATH(std::ignore = the_span.subspan(0U, the_span.size() + 1), "Precondition");
  ASSERT_DEATH(
      std::ignore =
          the_span.subspan(std::min<std::size_t>(the_span.size(), 1U), std::max<std::size_t>(the_span.size(), 1U)),
      "Precondition"
  );
}

REGISTER_TYPED_TEST_SUITE_P(
    SubspanOutOfBoundsDeathTest,
    ParamArgsIsPreconditionViolationIfOffsetPlusCountIsGreaterThanSize
);

INSTANTIATE_TYPED_TEST_SUITE_P(AllSpans, SubspanOutOfBoundsDeathTest, span_test::all_spans, );

}  // namespace
