// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file subspan_dynamic_extents.cpp
/// @brief Unit tests for @c span<T,Extent>::subspan with dynamic extents
///

#include <gtest/gtest.h>

#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/span/tests/span_test/helpers.hpp"
#include "arene/base/stdlib_choice/declval.hpp"

namespace {

namespace span_test = ::arene::base::tests::span_test;
using ::arene::base::span;

template <typename T>
struct SubspanReturnTypeForDynamicExtent : span_test::SubspanTest<T> {};

TYPED_TEST_SUITE_P(SubspanReturnTypeForDynamicExtent);
/// @test Validates that @c span<T,Extent>::subspan<Offset,Count>() 's return type is
/// @c span<T,dynamic_extent> when @c Extent==dynamic_extent and @c Count==dynamic_extent.
TYPED_TEST_P(SubspanReturnTypeForDynamicExtent, TemplateArgsWithCountAsDynamicExtentReturnTypeIsSpanWithDynamicExtent) {
  // Defaulted dynamic extent count.
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<TypeParam const&>().template subspan<TestFixture::zero_offset>()),
      span<typename TestFixture::template_type>>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<TypeParam const&>().template subspan<TestFixture::between_zero_and_max_offset>()),
      span<typename TestFixture::template_type>>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<TypeParam const&>().template subspan<TestFixture::max_offset>()),
      span<typename TestFixture::template_type>>();

  // Explicit dynamic extent count.
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<TypeParam const&>()
                   .template subspan<TestFixture::zero_offset, ::arene::base::dynamic_extent>()),
      span<typename TestFixture::template_type>>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<TypeParam const&>()
                   .template subspan<TestFixture::between_zero_and_max_offset, ::arene::base::dynamic_extent>()),
      span<typename TestFixture::template_type>>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<TypeParam const&>()
                   .template subspan<TestFixture::max_offset, ::arene::base::dynamic_extent>()),
      span<typename TestFixture::template_type>>();
}
/// @test Validates that @c span<T,Extent>::subspan<Offset,Count>() 's return type is
/// @c span<T,Count> when @c Extent==dynamic_extent and @c Count!=dynamic_extent.
TYPED_TEST_P(SubspanReturnTypeForDynamicExtent, TemplateArgsWithCountReturnTypeIsSpanWithExtentOfCount) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<TypeParam const&>()
                   .template subspan<
                       TestFixture::zero_offset,
                       TestFixture::template_extent() - TestFixture::zero_offset>()),
      span<typename TestFixture::template_type, TestFixture::template_extent() - TestFixture::zero_offset>>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<TypeParam const&>()
                   .template subspan<
                       TestFixture::between_zero_and_max_offset,
                       TestFixture::template_extent() - TestFixture::between_zero_and_max_offset>()),
      span<
          typename TestFixture::template_type,
          TestFixture::template_extent() - TestFixture::between_zero_and_max_offset>>();
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<TypeParam const&>().template subspan<TestFixture::max_offset, 0U>()),
      span<typename TestFixture::template_type, 0U>>();
}
/// @test Validates that @c span<T,Extent>::subspan(offset,count) 's return type is
/// @c span<T,dynamic_extent> when @c Extent==dynamic_extent .
TYPED_TEST_P(SubspanReturnTypeForDynamicExtent, ParamArgsIsSpanWithDynamicExtent) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<TypeParam const&>().subspan(0U, 0U)),
      span<typename TestFixture::template_type>>();
}
REGISTER_TYPED_TEST_SUITE_P(
    SubspanReturnTypeForDynamicExtent,
    TemplateArgsWithCountAsDynamicExtentReturnTypeIsSpanWithDynamicExtent,
    TemplateArgsWithCountReturnTypeIsSpanWithExtentOfCount,
    ParamArgsIsSpanWithDynamicExtent
);

INSTANTIATE_TYPED_TEST_SUITE_P(
    MutableDynamicExtentSpans,
    SubspanReturnTypeForDynamicExtent,
    span_test::mutable_dynamic_extent_spans,
);
INSTANTIATE_TYPED_TEST_SUITE_P(
    ConstDynamicExtentSpans,
    SubspanReturnTypeForDynamicExtent,
    span_test::const_dynamic_extent_spans,
);

}  // namespace
