// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file element_access.cpp
/// @brief Unit tests for @c span<T,Extent>::front and @c span<T,Extent>::back
///

#include <gtest/gtest.h>

#include "arene/base/iterator/next.hpp"
#include "arene/base/span/tests/span_test/helpers.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/min_value_overload.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

namespace span_test = ::arene::base::tests::span_test;

template <typename T>
struct FrontOfNonEmptySpan : span_test::SpanTest<T> {};

TYPED_TEST_SUITE_P(FrontOfNonEmptySpan);
/// @test Validates that @c span<T,Extent>::front() 's return type is @c span<T,Extent>::reference .
TYPED_TEST_P(FrontOfNonEmptySpan, ReturnTypeIsReference) {
  ::testing::StaticAssertTypeEq<decltype(std::declval<TypeParam&>().front()), typename TypeParam::reference>();
  ::testing::StaticAssertTypeEq<decltype(std::declval<TypeParam const&>().front()), typename TypeParam::reference>();
}
/// @test Validates that @c span<T,Extent>::front() is @c noexcept.
TYPED_TEST_P(FrontOfNonEmptySpan, IsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam&>().front()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam const&>().front()));
}
/// @test Validates that @c span<T,Extent>::front() returns a reference to the first element in the span .
TYPED_TEST_P(FrontOfNonEmptySpan, ReturnsReferenceToFirstElement) {
  auto const the_span =
      TypeParam{this->backing_data.begin(), std::min(this->backing_data.size(), TestFixture::template_extent())};
  EXPECT_EQ(&the_span.front(), the_span.data());
}

REGISTER_TYPED_TEST_SUITE_P(FrontOfNonEmptySpan, ReturnTypeIsReference, IsNoexcept, ReturnsReferenceToFirstElement);

INSTANTIATE_TYPED_TEST_SUITE_P(NonEmptySpans, FrontOfNonEmptySpan, span_test::non_empty_spans, );

template <typename T>
struct BackOfNonEmptySpan : span_test::SpanTest<T> {};

TYPED_TEST_SUITE_P(BackOfNonEmptySpan);
/// @test Validates that @c span<T,Extent>::back() 's return type is @c span<T,Extent>::reference .
TYPED_TEST_P(BackOfNonEmptySpan, ReturnTypeIsReference) {
  ::testing::StaticAssertTypeEq<decltype(std::declval<TypeParam&>().back()), typename TypeParam::reference>();
  ::testing::StaticAssertTypeEq<decltype(std::declval<TypeParam const&>().back()), typename TypeParam::reference>();
}
/// @test Validates that @c span<T,Extent>::back() is @c noexcept.
TYPED_TEST_P(BackOfNonEmptySpan, IsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam&>().back()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam const&>().back()));
}
/// @test Validates that @c span<T,Extent>::back() returns a reference to the last element in the span .
TYPED_TEST_P(BackOfNonEmptySpan, ReturnsReferenceToLastElement) {
  auto const the_span =
      TypeParam{this->backing_data.begin(), std::min(this->backing_data.size(), TestFixture::template_extent())};
  EXPECT_EQ(&the_span.back(), &*::arene::base::next(the_span.data(), the_span.ssize() - 1));
}

REGISTER_TYPED_TEST_SUITE_P(BackOfNonEmptySpan, ReturnTypeIsReference, IsNoexcept, ReturnsReferenceToLastElement);

INSTANTIATE_TYPED_TEST_SUITE_P(NonEmptySpans, BackOfNonEmptySpan, span_test::non_empty_spans, );

}  // namespace
