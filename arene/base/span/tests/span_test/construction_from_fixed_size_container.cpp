// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file construction_from_fixed_size_container.cpp
/// @brief Unit tests for span's construction from fixed-size containers.
///

#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

#include "arene/base/iterator.hpp"
#include "arene/base/span/tests/span_test/helpers.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/min_value_overload.hpp"
#include "arene/base/type_traits.hpp"

namespace {

namespace span_test = ::arene::base::tests::span_test;

template <typename T>
class ConstructionFromFixedSizeContainer : public span_test::SpanTest<T> {
 public:
  using typename span_test::SpanTest<T>::template_type;
  using span_test::SpanTest<T>::template_extent;
  using span_test::SpanTest<T>::backing_data;
  std::vector<::arene::base::remove_cvref_t<template_type>> contiguous_container{
      backing_data.begin(),
      ::arene::base::next(
          backing_data.begin(),
          static_cast<std::ptrdiff_t>(std::min(backing_data.size(), template_extent()))
      )
  };
};

TYPED_TEST_SUITE_P(ConstructionFromFixedSizeContainer);
/// @test Validates that @c span<T,Extent>::size() and @c span<T,Extent>::ssize() are both equal to
/// @c container.size() of the input container .
TYPED_TEST_P(ConstructionFromFixedSizeContainer, PostCtorSizeAndSSizeAreEqualToIntputContainerSize) {
  EXPECT_EQ(TypeParam{this->contiguous_container}.size(), this->contiguous_container.size());
  EXPECT_EQ(
      TypeParam{this->contiguous_container}.ssize(),
      static_cast<typename TypeParam::difference_type>(this->contiguous_container.size())
  );
}
/// @test Validates that @c span<T,Extent>::size() and @c span<T,Extent>::ssize() are both equal to
/// @c container.size() of the input container
TYPED_TEST_P(ConstructionFromFixedSizeContainer, PostCtorSizeBytesIsEqualToSizeTimesSizeofValueType) {
  auto const the_span = TypeParam{this->contiguous_container};
  EXPECT_EQ(the_span.size_bytes(), the_span.size() * sizeof(typename TypeParam::value_type));
}
/// @test Validates that @c span<T,Extent>::empty() is equal to @c container.empty() .
TYPED_TEST_P(ConstructionFromFixedSizeContainer, PostCtorIsNotEmpty) {
  EXPECT_EQ(TypeParam{this->contiguous_container}.empty(), this->contiguous_container.empty());
}
/// @test Validates that @c span<T,Extent>::data() is equal to @c container.data() .
TYPED_TEST_P(ConstructionFromFixedSizeContainer, PostCtorDataIsInputContainerData) {
  EXPECT_EQ(TypeParam{this->contiguous_container}.data(), this->contiguous_container.data());
}

REGISTER_TYPED_TEST_SUITE_P(
    ConstructionFromFixedSizeContainer,
    PostCtorSizeAndSSizeAreEqualToIntputContainerSize,
    PostCtorSizeBytesIsEqualToSizeTimesSizeofValueType,
    PostCtorIsNotEmpty,
    PostCtorDataIsInputContainerData
);

INSTANTIATE_TYPED_TEST_SUITE_P(AllSpans, ConstructionFromFixedSizeContainer, span_test::all_spans, );

}  // namespace
