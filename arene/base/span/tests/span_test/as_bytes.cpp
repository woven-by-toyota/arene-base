// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file as_bytes.cpp
/// @brief Unit tests for @c as_bytes and @c as_writable_bytes .
///

#include <gtest/gtest.h>

#include "arene/base/byte/byte.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/span/tests/span_test/helpers.hpp"
#include "arene/base/stdlib_choice/min_value_overload.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

namespace span_test = ::arene::base::tests::span_test;
using ::arene::base::span;

template <typename T>
class AsBytesTest : public span_test::SpanTest<T> {
 public:
  using typename span_test::SpanTest<T>::template_type;
  using span_test::SpanTest<T>::template_extent;
  using span_test::SpanTest<T>::backing_data;
  T const the_span{backing_data.begin(), std::min(this->backing_data.size(), template_extent())};
};

TYPED_TEST_SUITE_P(AsBytesTest);
/// @test Validates that @c as_bytes<T,Extent>(span<T,Extent>) 's return type is either:
/// - <c>span<::arene::base::byte const,N></c> where @c N=Extent*sizeof(T) if @c Extent!=dynamic_extent
/// - <c>span<::arene::base::byte const,dynamic_extent></c> if @c Extent==dynamic_extent
TYPED_TEST_P(
    AsBytesTest,
    ReturnTypeIsSpanToConstByteWithDynamicExtentForDynamicExtentInputOrSizeOfValueTypeTimesExtentForStaticExtentInput
) {
  constexpr auto expected_extent = TestFixture::template_extent() == ::arene::base::dynamic_extent
                                       ? ::arene::base::dynamic_extent
                                       : sizeof(typename TestFixture::template_type) * TestFixture::template_extent();
  ::testing::StaticAssertTypeEq<
      decltype(::arene::base::as_bytes(this->the_span)),
      span<::arene::base::byte const, expected_extent>>();
}
/// @test Validates that @c as_bytes<T,Extent>(span<T,Extent>) is @c noexcept
TYPED_TEST_P(AsBytesTest, IsNoexcept) { STATIC_ASSERT_TRUE(noexcept(::arene::base::as_bytes(this->the_span))); }
/// @test Validates that @c as_bytes<T,Extent>(span<T,Extent>) returns a span's whose size is equal to
/// @c span.size_bytes() .
TYPED_TEST_P(AsBytesTest, ReturnedSpanSizeIsInputSizeBytes) {
  EXPECT_EQ(::arene::base::as_bytes(this->the_span).size(), this->the_span.size_bytes());
}
/// @test Validates that @c as_bytes<T,Extent>(span<T,Extent>) returns a span's whose @c data() points to @c span.data()
/// @c reinterpret_cast 'd to <c>::arene::base::byte const*</c> .
TYPED_TEST_P(AsBytesTest, ReturnedSpanIsViewOntoSameContent) {
  EXPECT_EQ(
      ::arene::base::as_bytes(this->the_span).data(),
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast) explicitly testing for address equivalence.
      reinterpret_cast<::arene::base::byte const*>(this->the_span.data())
  );
}

REGISTER_TYPED_TEST_SUITE_P(
    AsBytesTest,
    ReturnTypeIsSpanToConstByteWithDynamicExtentForDynamicExtentInputOrSizeOfValueTypeTimesExtentForStaticExtentInput,
    IsNoexcept,
    ReturnedSpanSizeIsInputSizeBytes,
    ReturnedSpanIsViewOntoSameContent
);

INSTANTIATE_TYPED_TEST_SUITE_P(AllSpans, AsBytesTest, span_test::all_spans, );

template <typename T>
using AsWriteableBytesTest = AsBytesTest<T>;

TYPED_TEST_SUITE_P(AsWriteableBytesTest);
/// @test Validates that @c as_writeable_bytes<T,Extent>(span<T,Extent>) 's return type is either:
/// - @c span<::arene::base::byte,N> where @c N=Extent*sizeof(T) if @c Extent!=dynamic_extent
/// - @c span<::arene::base::byte,dynamic_extent> if @c Extent==dynamic_extent
TYPED_TEST_P(
    AsWriteableBytesTest,
    ReturnTypeIsSpanToByteWithDynamicExtentForDynamicExtentInputOrSizeOfValueTypeTimesExtentForStaticExtentInput
) {
  constexpr auto expected_extent = TestFixture::template_extent() == ::arene::base::dynamic_extent
                                       ? ::arene::base::dynamic_extent
                                       : sizeof(typename TestFixture::template_type) * TestFixture::template_extent();
  ::testing::StaticAssertTypeEq<
      decltype(::arene::base::as_writable_bytes(this->the_span)),
      span<::arene::base::byte, expected_extent>>();
}
/// @test Validates that @c as_writeable_bytes<T,Extent>(span<T,Extent>) is @c noexcept
TYPED_TEST_P(AsWriteableBytesTest, IsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(::arene::base::as_writable_bytes(this->the_span)));
}
/// @test Validates that @c as_writable_bytes<T,Extent>(span<T,Extent>) returns a span's whose size is equal to
/// @c span.size_bytes() .
TYPED_TEST_P(AsWriteableBytesTest, ReturnedSpanSizeIsInputSizeBytes) {
  EXPECT_EQ(::arene::base::as_writable_bytes(this->the_span).size(), this->the_span.size_bytes());
}
/// @test Validates that @c as_writable_bytes<T,Extent>(span<T,Extent>) returns a span's whose @c data() points to
/// @c span.data()@c reinterpret_cast 'd to @c ::arene::base::byte* .
TYPED_TEST_P(AsWriteableBytesTest, ReturnedSpanIsViewOntoSameContent) {
  EXPECT_EQ(
      ::arene::base::as_writable_bytes(this->the_span).data(),
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast) explicitly testing for address equivalence.
      reinterpret_cast<::arene::base::byte*>(this->the_span.data())
  );
}

REGISTER_TYPED_TEST_SUITE_P(
    AsWriteableBytesTest,
    ReturnTypeIsSpanToByteWithDynamicExtentForDynamicExtentInputOrSizeOfValueTypeTimesExtentForStaticExtentInput,
    IsNoexcept,
    ReturnedSpanSizeIsInputSizeBytes,
    ReturnedSpanIsViewOntoSameContent
);

INSTANTIATE_TYPED_TEST_SUITE_P(
    MutableDynamicExtentSpans,
    AsWriteableBytesTest,
    span_test::mutable_dynamic_extent_spans,
);
INSTANTIATE_TYPED_TEST_SUITE_P(
    MutableStaticExtentSpans,
    AsWriteableBytesTest,
    span_test::mutable_static_extent_spans,
);

}  // namespace
