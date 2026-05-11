// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file details.cpp
/// @brief Unit tests for the content of @c span_detail that need explicit covering .
///

#include <gtest/gtest.h>

#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @test Check that @c get_dynamic_extent returns @c dynamic_extent
TEST(DetailGetDynamicExtent, GetDynamicExtentReturnsCorrectValue) {
  ASSERT_EQ(arene::base::span_detail::get_dynamic_extent(), arene::base::dynamic_extent);
}

/// @test Check that @c is_dynamic_extent returns @c true if the supplied value is @c dynamic_extent @c false otherwise
TEST(DetailIsDynamicExtent, IsDynamicExtentReturnsCorrectValue) {
  std::size_t extent = 0;
  ASSERT_FALSE(arene::base::span_detail::is_dynamic_extent(extent));
  extent = arene::base::dynamic_extent;
  ASSERT_TRUE(arene::base::span_detail::is_dynamic_extent(extent));
  extent = 42;
  ASSERT_FALSE(arene::base::span_detail::is_dynamic_extent(extent));
  extent = arene::base::dynamic_extent - 1;
  ASSERT_FALSE(arene::base::span_detail::is_dynamic_extent(extent));
}

/// @test Check that `dynamic_extent` is always a valid source extent.
TEST(DetailIsValidSourceExtent, IsTrueIfMyExtentIsDynamicRegardlessOfTargetExtent) {
  STATIC_ASSERT_TRUE(::arene::base::span_detail::is_valid_source_extent(::arene::base::dynamic_extent, 0U));
  STATIC_ASSERT_TRUE(::arene::base::span_detail::is_valid_source_extent(::arene::base::dynamic_extent, 1U));
  STATIC_ASSERT_TRUE(
      ::arene::base::span_detail::is_valid_source_extent(::arene::base::dynamic_extent, ::arene::base::dynamic_extent)
  );
}

/// @test Check that a source extent is valid if equal to target extent.
TEST(DetailIsValidSourceExtent, IsTrueIfMyExtentEqualsTargetExtent) {
  STATIC_ASSERT_TRUE(::arene::base::span_detail::is_valid_source_extent(0U, 0U));
  STATIC_ASSERT_TRUE(::arene::base::span_detail::is_valid_source_extent(1U, 1U));
  STATIC_ASSERT_TRUE(::arene::base::span_detail::is_valid_source_extent(
      std::numeric_limits<std::size_t>::max(),
      std::numeric_limits<std::size_t>::max()
  ));
}

/// @test Check that a source extent is not valid if not equal to target extent (excluding `dynamic_extent`).
TEST(DetailIsValidSourceExtent, IsFalseIfMyExtentDoesNotEqualTargetExtent) {
  STATIC_ASSERT_FALSE(::arene::base::span_detail::is_valid_source_extent(0U, 1U));
  STATIC_ASSERT_FALSE(::arene::base::span_detail::is_valid_source_extent(1U, 0U));
  STATIC_ASSERT_FALSE(::arene::base::span_detail::is_valid_source_extent(
      std::numeric_limits<std::size_t>::lowest(),
      std::numeric_limits<std::size_t>::max()
  ));
}

/// @test Check that `dynamic_extent` is a valid runtime size.
TEST(DetailIsValidRuntimeSize, IsTrueIfMyExtentIsDynamicAndTargetSizeIsLessThanOrEqualMaxSize) {
  STATIC_ASSERT_TRUE(::arene::base::span_detail::is_valid_runtime_size(::arene::base::dynamic_extent, 0U, 0U));
  STATIC_ASSERT_TRUE(::arene::base::span_detail::is_valid_runtime_size(::arene::base::dynamic_extent, 0U, 1U));
  STATIC_ASSERT_TRUE(::arene::base::span_detail::is_valid_runtime_size(
      ::arene::base::dynamic_extent,
      std::numeric_limits<std::size_t>::max() / 2,
      std::numeric_limits<std::size_t>::max()
  ));
}

/// @test Check that a source extent is a valid runtime size if equal to target extent.
TEST(DetailIsValidRuntimeSize, IsTrueIfMyExtentEqualsTargetSizeAndTargetSizeIsLessThanOrEqualMaxSize) {
  STATIC_ASSERT_TRUE(::arene::base::span_detail::is_valid_runtime_size(0U, 0U, 0U));
  STATIC_ASSERT_TRUE(::arene::base::span_detail::is_valid_runtime_size(0U, 0U, 1U));
  STATIC_ASSERT_TRUE(::arene::base::span_detail::is_valid_runtime_size(1U, 1U, 1U));
  STATIC_ASSERT_TRUE(::arene::base::span_detail::is_valid_runtime_size(
      std::numeric_limits<std::size_t>::max() / 2,
      std::numeric_limits<std::size_t>::max() / 2,
      std::numeric_limits<std::size_t>::max()
  ));
}

/// @test Check that a target extent larger than max size is not a valid runtime size.
TEST(DetailIsValidRuntimeSize, IsFalseIfMyExtentIsDynamicAndTargetSizeIsGreaterThanMaxSize) {
  STATIC_ASSERT_FALSE(::arene::base::span_detail::is_valid_runtime_size(::arene::base::dynamic_extent, 1U, 0U));
  STATIC_ASSERT_FALSE(::arene::base::span_detail::is_valid_runtime_size(::arene::base::dynamic_extent, 2U, 1U));
  STATIC_ASSERT_FALSE(::arene::base::span_detail::is_valid_runtime_size(
      ::arene::base::dynamic_extent,
      std::numeric_limits<std::size_t>::max(),
      std::numeric_limits<std::size_t>::max() / 2
  ));
}

/// @test Check that a source extent not equal to target extent is not a valid runtime size.
TEST(DetailIsValidRuntimeSize, IsFalseIfMyExtentDoesNotEqualTargetExtent) {
  STATIC_ASSERT_FALSE(::arene::base::span_detail::is_valid_runtime_size(0U, 1U, std::numeric_limits<std::size_t>::max())
  );
  STATIC_ASSERT_FALSE(::arene::base::span_detail::is_valid_runtime_size(1U, 0U, std::numeric_limits<std::size_t>::max())
  );
  STATIC_ASSERT_FALSE(::arene::base::span_detail::is_valid_runtime_size(
      std::numeric_limits<std::size_t>::lowest(),
      std::numeric_limits<std::size_t>::max(),
      std::numeric_limits<std::size_t>::max()
  ));
}

/// @test Check that the subspan extent is equal to count if count is not `dynamic_extent`.
TEST(DetailExtentForSubspan, IsCountIfCountIsNotDynamicExtent) {
  STATIC_ASSERT_EQ((::arene::base::span_detail::extent_for_subspan<::arene::base::dynamic_extent, 0, 0>()), 0);
  STATIC_ASSERT_EQ((::arene::base::span_detail::extent_for_subspan<::arene::base::dynamic_extent, 0, 1>()), 1);
  STATIC_ASSERT_EQ((::arene::base::span_detail::extent_for_subspan<1, 0, 0>()), 0);
  STATIC_ASSERT_EQ((::arene::base::span_detail::extent_for_subspan<1, 0, 1>()), 1);
}

/// @test Check that the subspan extent is `dynamic_extent` if extent and count are also `dynamic_extent`.
TEST(DetailExtentForSubspan, IsDynamicExtentIfCountAndExtentAreDynamicExtent) {
  STATIC_ASSERT_EQ(
      (::arene::base::span_detail::extent_for_subspan<::arene::base::dynamic_extent, 0, ::arene::base::dynamic_extent>()
      ),
      ::arene::base::dynamic_extent
  );
  STATIC_ASSERT_EQ(
      (::arene::base::span_detail::extent_for_subspan<::arene::base::dynamic_extent, 1, ::arene::base::dynamic_extent>()
      ),
      ::arene::base::dynamic_extent
  );
  STATIC_ASSERT_EQ(
      (::arene::base::span_detail::extent_for_subspan<
          ::arene::base::dynamic_extent,
          ::arene::base::dynamic_extent,
          ::arene::base::dynamic_extent>()),
      ::arene::base::dynamic_extent
  );
}

/// @test Check that count is the difference of extent and offset if only `Count` is equal to `dynamic_extent`.
TEST(DetailExtentForSubspan, IsExtentMinusOffsetIfCountIsDynamicExtentAndExtentIsNotDynamicExtent) {
  STATIC_ASSERT_EQ((::arene::base::span_detail::extent_for_subspan<0, 0, ::arene::base::dynamic_extent>()), 0);
  STATIC_ASSERT_EQ((::arene::base::span_detail::extent_for_subspan<10, 5, ::arene::base::dynamic_extent>()), 5);
  STATIC_ASSERT_EQ((::arene::base::span_detail::extent_for_subspan<10, 0, ::arene::base::dynamic_extent>()), 10);
}

}  // namespace
