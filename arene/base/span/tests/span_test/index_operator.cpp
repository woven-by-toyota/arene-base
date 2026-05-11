// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file index_operator.cpp
/// @brief Unit tests for  @c span<T,Extent>::operator[]
///

#include <gtest/gtest.h>

#include "arene/base/iterator/next.hpp"
#include "arene/base/span/tests/span_test/helpers.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/min_value_overload.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

namespace span_test = ::arene::base::tests::span_test;

template <typename T>
struct IndexOperatorInBounds : span_test::SpanTest<T> {};

TYPED_TEST_SUITE_P(IndexOperatorInBounds);
/// @test Validates that @c span<T,Extent>::operator[] 's return type is @c span<T,Extent>::reference .
TYPED_TEST_P(IndexOperatorInBounds, ReturnTypeIsReference) {
  ::testing::StaticAssertTypeEq<decltype(std::declval<TypeParam&>()[0]), typename TypeParam::reference>();
  ::testing::StaticAssertTypeEq<decltype(std::declval<TypeParam const&>()[0]), typename TypeParam::reference>();
}
/// @test Validates that @c span<T,Extent>::operator[] is @c noexcept .
TYPED_TEST_P(IndexOperatorInBounds, IsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam&>()[0]));
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam const&>()[0]));
}
/// @test Validates that @c span<T,Extent>::operator[](index) returns a reference to the @c index 'th element in the
/// span's view.
TYPED_TEST_P(IndexOperatorInBounds, ReturnsReferenceToElementAtIndex) {
  auto const the_span =
      TypeParam{this->backing_data.begin(), std::min(this->backing_data.size(), TestFixture::template_extent())};
  for (std::size_t index = 0; index < the_span.size(); ++index) {
    EXPECT_EQ(
        &the_span[index],
        &*::arene::base::next(the_span.data(), static_cast<typename TypeParam::difference_type>(index))
    );
  }
}

REGISTER_TYPED_TEST_SUITE_P(IndexOperatorInBounds, ReturnTypeIsReference, IsNoexcept, ReturnsReferenceToElementAtIndex);

INSTANTIATE_TYPED_TEST_SUITE_P(AllSpans, IndexOperatorInBounds, span_test::all_spans, );

}  // namespace
