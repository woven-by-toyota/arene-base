// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file first.cpp
/// @brief Unit tests for @c span<T,Extent>::first
///

#include <gmock/gmock.h>

#include "arene/base/span/span.hpp"
#include "arene/base/span/tests/span_test/equivalent_span_to.hpp"
#include "arene/base/span/tests/span_test/helpers.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/min_value_overload.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

namespace span_test = ::arene::base::tests::span_test;
using ::arene::base::span;

template <typename T>
class FirstInBounds : public span_test::SpanTest<T> {
 public:
  using typename span_test::SpanTest<T>::template_type;
  using span_test::SpanTest<T>::template_extent;

  static constexpr std::size_t zero_count = 0U;
  static constexpr std::size_t max_count = std::min(span_test::max_data_size, template_extent());
  static constexpr std::size_t between_zero_and_max_count = std::min<std::size_t>(1U, max_count);
};

TYPED_TEST_SUITE_P(FirstInBounds);
/// @test Validates that @c span<T,Extent>::first<Count>() 's return type is @c span<T,Count> .
TYPED_TEST_P(FirstInBounds, TemplateCountReturnTypeIsSpanWithExtentOfCount) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<TypeParam const&>().template first<TestFixture::zero_count>()),
      span<typename TestFixture::template_type, TestFixture::zero_count>>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<TypeParam const&>().template first<TestFixture::between_zero_and_max_count>()),
      span<typename TestFixture::template_type, TestFixture::between_zero_and_max_count>>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<TypeParam const&>().template first<TestFixture::max_count>()),
      span<typename TestFixture::template_type, TestFixture::max_count>>();
}
/// @test Validates that @c span<T,Extent>::first<Count>() is @c noexcept
TYPED_TEST_P(FirstInBounds, TemplateCountIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam const&>().template first<TestFixture::zero_count>()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam const&>().template first<TestFixture::between_zero_and_max_count>()
  ));
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam const&>().template first<TestFixture::max_count>()));
}
/// @test Validates that @c span<T,Extent>::first<Count>() 's return span is equivalent to a span constructed as
/// @c span<T,Count>{data(),Count} .
TYPED_TEST_P(FirstInBounds, TemplateCountReturnsSpanEquivalentToSpanConstructedFromDataAndCount) {
  auto const the_span =
      TypeParam{this->backing_data.begin(), std::min(this->backing_data.size(), TestFixture::template_extent())};
  EXPECT_THAT(
      the_span.template first<TestFixture::zero_count>(),
      span_test::equivalent_span_to(
          span<typename TestFixture::template_type, TestFixture::zero_count>{the_span.data(), TestFixture::zero_count}
      )
  );
  EXPECT_THAT(
      the_span.template first<TestFixture::between_zero_and_max_count>(),
      span_test::equivalent_span_to(span<typename TestFixture::template_type, TestFixture::between_zero_and_max_count>{
          the_span.data(),
          TestFixture::between_zero_and_max_count
      })
  );
  EXPECT_THAT(
      the_span.template first<TestFixture::max_count>(),
      span_test::equivalent_span_to(
          span<typename TestFixture::template_type, TestFixture::max_count>{the_span.data(), TestFixture::max_count}
      )
  );
}
/// @test Validates that @c span<T,Extent>::first(count) 's return type is @c span<T,dynamic_extent> .
TYPED_TEST_P(FirstInBounds, ParamCountReturnTypeIsSpanWithDynamicExtent) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<TypeParam const&>().first(TestFixture::zero_count)),
      span<typename TestFixture::template_type>>();
}
/// @test Validates that @c span<T,Extent>::first(count) is @c noexcept
TYPED_TEST_P(FirstInBounds, ParamCountIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam const&>().first(TestFixture::zero_count)));
}
/// @test Validates that @c span<T,Extent>::first<Count>() 's return span is equivalent to a span constructed as
/// @c span<T>{data(),count} .
TYPED_TEST_P(FirstInBounds, ParamCountReturnsSpanEquivalentToSpanConstructedFromDataAndCount) {
  auto const the_span =
      TypeParam{this->backing_data.begin(), std::min(this->backing_data.size(), TestFixture::template_extent())};
  for (auto count : {TestFixture::zero_count, TestFixture::between_zero_and_max_count, TestFixture::max_count}) {
    SCOPED_TRACE(count);
    EXPECT_THAT(
        the_span.first(count),
        span_test::equivalent_span_to(span<typename TestFixture::template_type>{the_span.data(), count})
    );
  }
}

REGISTER_TYPED_TEST_SUITE_P(
    FirstInBounds,
    TemplateCountReturnTypeIsSpanWithExtentOfCount,
    TemplateCountIsNoexcept,
    TemplateCountReturnsSpanEquivalentToSpanConstructedFromDataAndCount,
    ParamCountReturnTypeIsSpanWithDynamicExtent,
    ParamCountIsNoexcept,
    ParamCountReturnsSpanEquivalentToSpanConstructedFromDataAndCount
);

INSTANTIATE_TYPED_TEST_SUITE_P(AllSpans, FirstInBounds, span_test::all_spans, );

}  // namespace
