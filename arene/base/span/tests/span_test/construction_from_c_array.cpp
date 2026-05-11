// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file construction_from_c_array.cpp
/// @brief Unit tests for span's construction from C-style arrays.
///

#include <gtest/gtest.h>

#include "arene/base/span/tests/span_test/helpers.hpp"

namespace {

namespace span_test = ::arene::base::tests::span_test;

template <typename T>
struct ConstructionFromCArray : span_test::SpanTest<T> {};

TYPED_TEST_SUITE_P(ConstructionFromCArray);
/// @test Validates that @c span<T,Extent>::size() and @c span<T,Extent>::ssize() are both equal to
/// the size of the input C-array .
TYPED_TEST_P(ConstructionFromCArray, PostCtorSizeAndSSizeAreEqualToArraySize) {
  EXPECT_EQ(TypeParam{this->backing_data.as_c_array()}.size(), this->backing_data.size());
  EXPECT_EQ(
      TypeParam{this->backing_data.as_c_array()}.ssize(),
      static_cast<typename TypeParam::difference_type>(this->backing_data.size())
  );
}
/// @test Validates that @c span<T,Extent>::size_bytes() is @c sizeof(T)*size() .
TYPED_TEST_P(ConstructionFromCArray, PostCtorSizeBytesIsEqualToSizeTimesSizeofValueType) {
  auto const the_span = TypeParam{this->backing_data.as_c_array()};
  EXPECT_EQ(the_span.size_bytes(), the_span.size() * sizeof(typename TypeParam::value_type));
}
/// @test Validates that @c span<T,Extent>::empty() is @c false .
TYPED_TEST_P(ConstructionFromCArray, PostCtorIsNotEmpty) {
  EXPECT_FALSE(TypeParam{this->backing_data.as_c_array()}.empty());
}
/// @test Validates that @c span<T,Extent>::data() is equal to the @c data parameter to the constructor .
TYPED_TEST_P(ConstructionFromCArray, PostCtorDataIsInputPtr) {
  EXPECT_EQ(TypeParam{this->backing_data.as_c_array()}.data(), &(this->backing_data.as_c_array()[0]));
}

REGISTER_TYPED_TEST_SUITE_P(
    ConstructionFromCArray,
    PostCtorSizeAndSSizeAreEqualToArraySize,
    PostCtorSizeBytesIsEqualToSizeTimesSizeofValueType,
    PostCtorIsNotEmpty,
    PostCtorDataIsInputPtr
);

INSTANTIATE_TYPED_TEST_SUITE_P(AllDynamicExtentSpans, ConstructionFromCArray, span_test::all_dynamic_extent_spans, );

}  // namespace
