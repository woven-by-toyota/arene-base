// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file last.cpp
/// @brief Unit tests for @c span<T,Extent>::last
///

#include <gmock/gmock.h>

#include "arene/base/iterator/next.hpp"
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
class LastInBounds : public span_test::SpanTest<T> {
 public:
  using typename span_test::SpanTest<T>::template_type;
  using span_test::SpanTest<T>::template_extent;

  static constexpr std::size_t zero_count = 0U;
  static constexpr std::size_t max_count = std::min(span_test::max_data_size, template_extent());
  static constexpr std::size_t between_zero_and_max_count = std::min<std::size_t>(1U, max_count);
};

TYPED_TEST_SUITE_P(LastInBounds);
/// @test Validates that @c span<T,Extent>::last<Count>() 's return type is @c span<T,Count> .
TYPED_TEST_P(LastInBounds, TemplateCountReturnTypeIsSpanWithExtentOfCount) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<TypeParam const&>().template last<TestFixture::zero_count>()),
      span<typename TestFixture::template_type, TestFixture::zero_count>>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<TypeParam const&>().template last<TestFixture::between_zero_and_max_count>()),
      span<typename TestFixture::template_type, TestFixture::between_zero_and_max_count>>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<TypeParam const&>().template last<TestFixture::max_count>()),
      span<typename TestFixture::template_type, TestFixture::max_count>>();
}
/// @test Validates that @c span<T,Extent>::last<Count>() is @c noexcept
TYPED_TEST_P(LastInBounds, TemplateCountIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam const&>().template last<TestFixture::zero_count>()));
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam const&>().template last<TestFixture::between_zero_and_max_count>())
  );
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam const&>().template last<TestFixture::max_count>()));
}
/// @test Validates that @c span<T,Extent>::last<Count>() 's return span is equivalent to a span constructed as
/// @c span<T>{next(data(),size-Count),Count} .
TYPED_TEST_P(LastInBounds, TemplateCountReturnsSpanEquivalentToSpanConstructedFromDataAdvancedSizeMinusCountAndCount) {
  auto const the_span =
      TypeParam{this->backing_data.begin(), std::min(this->backing_data.size(), TestFixture::template_extent())};
  auto const last_start = [&the_span](std::size_t count) {
    return ::arene::base::next(
        the_span.data(),
        static_cast<typename TypeParam::difference_type>(the_span.size() - count)
    );
  };
  EXPECT_THAT(
      the_span.template last<TestFixture::zero_count>(),
      span_test::equivalent_span_to(span<typename TestFixture::template_type, TestFixture::zero_count>{
          last_start(TestFixture::zero_count),
          TestFixture::zero_count
      })
  );
  EXPECT_THAT(
      the_span.template last<TestFixture::between_zero_and_max_count>(),
      span_test::equivalent_span_to(span<typename TestFixture::template_type, TestFixture::between_zero_and_max_count>{
          last_start(TestFixture::between_zero_and_max_count),
          TestFixture::between_zero_and_max_count
      })
  );
  EXPECT_THAT(
      the_span.template last<TestFixture::max_count>(),
      span_test::equivalent_span_to(span<typename TestFixture::template_type, TestFixture::max_count>{
          last_start(TestFixture::max_count),
          TestFixture::max_count
      })
  );
}
/// @test Validates that @c span<T,Extent>::last(count) 's return type is @c span<T,dynamic_extent> .
TYPED_TEST_P(LastInBounds, ParamCountReturnTypeIsSpanWithDynamicExtent) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<TypeParam const&>().last(TestFixture::zero_count)),
      span<typename TestFixture::template_type>>();
}
/// @test Validates that @c span<T,Extent>::last(count) is @c noexcept .
TYPED_TEST_P(LastInBounds, ParamCountIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam const&>().last(TestFixture::zero_count)));
}
/// @test Validates that @c span<T,Extent>::last(count) 's return span is equivalent to a span constructed as
/// @c span<T>{next(data(),size-count),count} .
TYPED_TEST_P(LastInBounds, ParamCountReturnsSpanEquivalentToSpanConstructedFromDataAdvancedSizeMinusCountAndCount) {
  auto const the_span =
      TypeParam{this->backing_data.begin(), std::min(this->backing_data.size(), TestFixture::template_extent())};
  auto const last_start = [&the_span](std::size_t count) {
    return ::arene::base::next(
        the_span.data(),
        static_cast<typename TypeParam::difference_type>(the_span.size() - count)
    );
  };
  for (auto count : {TestFixture::zero_count, TestFixture::between_zero_and_max_count, TestFixture::max_count}) {
    SCOPED_TRACE(count);
    EXPECT_THAT(
        the_span.last(count),
        span_test::equivalent_span_to(span<typename TestFixture::template_type>{last_start(count), count})
    );
  }
}

REGISTER_TYPED_TEST_SUITE_P(
    LastInBounds,
    TemplateCountReturnTypeIsSpanWithExtentOfCount,
    TemplateCountIsNoexcept,
    TemplateCountReturnsSpanEquivalentToSpanConstructedFromDataAdvancedSizeMinusCountAndCount,
    ParamCountReturnTypeIsSpanWithDynamicExtent,
    ParamCountIsNoexcept,
    ParamCountReturnsSpanEquivalentToSpanConstructedFromDataAdvancedSizeMinusCountAndCount
);

INSTANTIATE_TYPED_TEST_SUITE_P(AllSpans, LastInBounds, span_test::all_spans, );

}  // namespace
