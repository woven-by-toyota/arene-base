// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file construction_from_pointer_and_size.cpp
/// @brief Unit tests for span's construction from pointer and size.
///

#include <gtest/gtest.h>

#include "arene/base/iterator/distance.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/span/tests/span_test/helpers.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"

namespace {

namespace span_test = ::arene::base::tests::span_test;

template <typename T>
struct ConstructionFromPtrAndSize : span_test::SpanTest<T> {};

TYPED_TEST_SUITE_P(ConstructionFromPtrAndSize);
/// @test Validates that @c span<T,Extent>::size() and @c span<T,Extent>::ssize() are both equal to
/// the @c data_length parameter to the constructor .
TYPED_TEST_P(ConstructionFromPtrAndSize, PostCtorSizeAndSSizeAreEqualToDataLength) {
  for (std::ptrdiff_t start_offset = 0;
       start_offset < ::arene::base::distance(this->backing_data.begin(), this->backing_data.end());
       ++start_offset) {
    auto const begin = ::arene::base::next(this->backing_data.begin(), start_offset);
    for (std::size_t length = 0U;
         length <= static_cast<std::size_t>(::arene::base::distance(begin, this->backing_data.end()));
         ++length) {
      auto const the_span = TypeParam{begin, length};
      EXPECT_EQ(the_span.size(), length);
      EXPECT_EQ(the_span.ssize(), static_cast<typename TypeParam::difference_type>(length));
    }
  }
}
/// @test Validates that @c span<T,Extent>::size_bytes() is @c sizeof(T)*size() .
TYPED_TEST_P(ConstructionFromPtrAndSize, PostCtorSizeBytesIsEqualToSizeTimesSizeofValueType) {
  for (std::ptrdiff_t start_offset = 0;
       start_offset < ::arene::base::distance(this->backing_data.begin(), this->backing_data.end());
       ++start_offset) {
    auto const begin = ::arene::base::next(this->backing_data.begin(), start_offset);
    for (std::size_t length = 0U;
         length <= static_cast<std::size_t>(::arene::base::distance(begin, this->backing_data.end()));
         ++length) {
      auto const the_span = TypeParam{begin, length};
      EXPECT_EQ(the_span.size_bytes(), the_span.size() * sizeof(typename TypeParam::value_type));
    }
  }
}
/// @test Validates that @c span<T,Extent>::empty() is @c true if @c data_length==0 .
TYPED_TEST_P(ConstructionFromPtrAndSize, PostCtorIsEmptyIfDataLengthIsZero) {
  for (std::ptrdiff_t start_offset = 0;
       start_offset < ::arene::base::distance(this->backing_data.begin(), this->backing_data.end());
       ++start_offset) {
    auto const begin = ::arene::base::next(this->backing_data.begin(), start_offset);
    auto const the_span = TypeParam{begin, 0U};
    EXPECT_TRUE(the_span.empty());
  }
}
/// @test Validates that @c span<T,Extent>::empty() is @c false if @c data_length!=0 .
TYPED_TEST_P(ConstructionFromPtrAndSize, PostCtorIsNotEmptyIfDataLengthGreaterThanZero) {
  for (std::ptrdiff_t start_offset = 0;
       start_offset < ::arene::base::distance(this->backing_data.begin(), this->backing_data.end());
       ++start_offset) {
    auto const begin = ::arene::base::next(this->backing_data.begin(), start_offset);
    for (std::size_t length = 1U;
         length <= static_cast<std::size_t>(::arene::base::distance(begin, this->backing_data.end()));
         ++length) {
      auto const the_span = TypeParam{begin, length};
      EXPECT_FALSE(the_span.empty());
    }
  }
}
/// @test Validates that @c span<T,Extent>::data() is equal to the @c data parameter to the constructor .
TYPED_TEST_P(ConstructionFromPtrAndSize, PostCtorDataIsInputPtr) {
  for (std::ptrdiff_t start_offset = 0;
       start_offset < ::arene::base::distance(this->backing_data.begin(), this->backing_data.end());
       ++start_offset) {
    auto const begin = ::arene::base::next(this->backing_data.begin(), start_offset);
    for (std::size_t length = 0U;
         length <= static_cast<std::size_t>(::arene::base::distance(begin, this->backing_data.end()));
         ++length) {
      auto const the_span = TypeParam{begin, length};
      EXPECT_EQ(the_span.data(), begin);
    }
  }
}

REGISTER_TYPED_TEST_SUITE_P(
    ConstructionFromPtrAndSize,
    PostCtorSizeAndSSizeAreEqualToDataLength,
    PostCtorSizeBytesIsEqualToSizeTimesSizeofValueType,
    PostCtorIsEmptyIfDataLengthIsZero,
    PostCtorIsNotEmptyIfDataLengthGreaterThanZero,
    PostCtorDataIsInputPtr
);

INSTANTIATE_TYPED_TEST_SUITE_P(
    AllDynamicExtentSpans,
    ConstructionFromPtrAndSize,
    span_test::all_dynamic_extent_spans,
);

}  // namespace
