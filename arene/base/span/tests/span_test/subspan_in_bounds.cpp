// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file subspan_in_bounds.cpp
/// @brief Unit tests for bounds checking of @c span<T,Extent>::subspan
///

#include <gmock/gmock.h>

#include "arene/base/detail/dynamic_extent.hpp"
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
struct SubspanInBounds : span_test::SubspanTest<T> {};

TYPED_TEST_SUITE_P(SubspanInBounds);
/// @test Validates that @c span<T,Extent>::subspan<Offset,Count>() is @c noexcept
TYPED_TEST_P(SubspanInBounds, TemplateArgsIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam const&>().template subspan<TestFixture::zero_offset>()));
}
/// @test Validates that @c span<T,Extent>::subspan<Offset,Count>() , with @c Count==dynamic_extent , returns a span
/// with an equivalent view as one constructed as @c span<T>{next(data(),Offset),size()-Offset} .
TYPED_TEST_P(
    SubspanInBounds,
    TemplateArgsWithCountAsDynamicExtentReturnsSpanEquivalentToSpanConstructedFromDataAdvancedOffset
) {
  auto const the_span =
      TypeParam{this->backing_data.begin(), std::min(this->backing_data.size(), TestFixture::template_extent())};
  EXPECT_THAT(
      the_span.template subspan<TestFixture::zero_offset>(),
      span_test::equivalent_span_to(span<typename TestFixture::template_type>{
          ::arene::base::next(
              the_span.data(),
              static_cast<typename TypeParam::difference_type>(TestFixture::zero_offset)
          ),
          the_span.size() - TestFixture::zero_offset
      })
  );
  EXPECT_THAT(
      the_span.template subspan<TestFixture::between_zero_and_max_offset>(),
      span_test::equivalent_span_to(span<typename TestFixture::template_type>{
          ::arene::base::next(
              the_span.data(),
              static_cast<typename TypeParam::difference_type>(TestFixture::between_zero_and_max_offset)
          ),
          the_span.size() - TestFixture::between_zero_and_max_offset
      })
  );
  EXPECT_THAT(
      the_span.template subspan<TestFixture::max_offset>(),
      span_test::equivalent_span_to(span<typename TestFixture::template_type>{
          ::arene::base::next(
              the_span.data(),
              static_cast<typename TypeParam::difference_type>(TestFixture::max_offset)
          ),
          the_span.size() - TestFixture::max_offset
      })
  );
}
/// @test Validates that @c span<T,Extent>::subspan<Offset,Count>() , with @c Count!=dynamic_extent , returns a span
/// with an equivalent view as one constructed as @c span<T>{next(data(),Offset),Count} .
TYPED_TEST_P(
    SubspanInBounds,
    TemplateArgsWithCountReturnsSpanEquivalentToSpanConstructedFromDataAdvancedOffsetWithSizeCount
) {
  auto const the_span =
      TypeParam{this->backing_data.begin(), std::min(this->backing_data.size(), TestFixture::template_extent())};
  constexpr std::size_t count = TestFixture::template_extent() > 1 ? 1 : 0;
  EXPECT_THAT(
      (the_span.template subspan<TestFixture::zero_offset, count>()),
      span_test::equivalent_span_to(span<typename TestFixture::template_type>{
          ::arene::base::next(
              the_span.data(),
              static_cast<typename TypeParam::difference_type>(TestFixture::zero_offset)
          ),
          count
      })
  );
  EXPECT_THAT(
      (the_span.template subspan<TestFixture::between_zero_and_max_offset, count>()),
      span_test::equivalent_span_to(span<typename TestFixture::template_type>{
          ::arene::base::next(
              the_span.data(),
              static_cast<typename TypeParam::difference_type>(TestFixture::between_zero_and_max_offset)
          ),
          count
      })
  );
  EXPECT_THAT(
      (the_span.template subspan<TestFixture::max_offset, 0U>()),
      span_test::equivalent_span_to(span<typename TestFixture::template_type>{
          ::arene::base::next(
              the_span.data(),
              static_cast<typename TypeParam::difference_type>(TestFixture::max_offset)
          ),
          0U
      })
  );
}
/// @test Validates that @c span<T,Extent>::subspan(offset,count) is @c noexcept
TYPED_TEST_P(SubspanInBounds, ParamArgsIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam const&>().subspan(0U)));
  STATIC_ASSERT_TRUE(noexcept(std::declval<TypeParam const&>().subspan(0U, 0U)));
}
/// @test Validates that @c span<T,Extent>::subspan(offset,count) , with @c count==dynamic_extent , returns a span
/// with an equivalent view as one constructed as @c span<T>{next(data(),offset),size()-offset} .
TYPED_TEST_P(
    SubspanInBounds,
    ParamArgsWithCountAsDynamicOffsetReturnsSpanEquivalentToSpanConstructedFromDataAdvancedOffset
) {
  auto const the_span =
      TypeParam{this->backing_data.begin(), std::min(this->backing_data.size(), TestFixture::template_extent())};
  for (auto offset : {TestFixture::zero_offset, TestFixture::between_zero_and_max_offset, TestFixture::max_offset}) {
    SCOPED_TRACE(offset);
    auto const expected_span = span<typename TestFixture::template_type>{
        ::arene::base::next(the_span.data(), static_cast<typename TypeParam::difference_type>(offset)),
        the_span.size() - offset
    };
    EXPECT_THAT(the_span.subspan(offset), span_test::equivalent_span_to(expected_span));
    EXPECT_THAT(the_span.subspan(offset, ::arene::base::dynamic_extent), span_test::equivalent_span_to(expected_span));
  }
}
/// @test Validates that @c span<T,Extent>::subspan(offset,count) , with @c Count!=dynamic_extent , returns a span
/// with an equivalent view as one constructed as @c span<T>{next(data(),offset),count} .
TYPED_TEST_P(SubspanInBounds, ParamArgsReturnsSpanEquivalentToSpanConstructedFromDataAdvancedOffsetAndCount) {
  auto const the_span =
      TypeParam{this->backing_data.begin(), std::min(this->backing_data.size(), TestFixture::template_extent())};
  for (auto offset : {TestFixture::zero_offset, TestFixture::between_zero_and_max_offset, TestFixture::max_offset}) {
    SCOPED_TRACE(offset);
    auto const count = std::min<std::size_t>(1, the_span.size() - offset);
    auto const expected_span = span<typename TestFixture::template_type>{
        ::arene::base::next(the_span.data(), static_cast<typename TypeParam::difference_type>(offset)),
        count
    };
    EXPECT_THAT(the_span.subspan(offset, count), span_test::equivalent_span_to(expected_span));
  }
}

REGISTER_TYPED_TEST_SUITE_P(
    SubspanInBounds,
    TemplateArgsIsNoexcept,
    TemplateArgsWithCountAsDynamicExtentReturnsSpanEquivalentToSpanConstructedFromDataAdvancedOffset,
    TemplateArgsWithCountReturnsSpanEquivalentToSpanConstructedFromDataAdvancedOffsetWithSizeCount,
    ParamArgsIsNoexcept,
    ParamArgsWithCountAsDynamicOffsetReturnsSpanEquivalentToSpanConstructedFromDataAdvancedOffset,
    ParamArgsReturnsSpanEquivalentToSpanConstructedFromDataAdvancedOffsetAndCount
);

INSTANTIATE_TYPED_TEST_SUITE_P(AllSpans, SubspanInBounds, span_test::all_spans, );

}  // namespace
