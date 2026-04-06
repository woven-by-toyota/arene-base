// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file mdspan_test.cpp
/// @brief Provides unit tests to validate export of content in mdspan.hpp
///

#include "arene/base/mdspan.hpp"

#include <gtest/gtest.h>

namespace {

/// @test `extents` is declared
TEST(Mdspan, Extents) { using arene::base::extents; }

/// @test `default_accessor` is declared
TEST(Mdspan, DefaultAccessor) { using arene::base::default_accessor; }

/// @test `layout_right` and `layout_right::mapping` are declared
TEST(Mdspan, LayoutRight) { using arene::base::layout_right; }

/// @test `layout_left` and `layout_left::mapping` are declared
TEST(Mdspan, LayoutLeft) { using arene::base::layout_left; }

/// @test `layout_stride` and `layout_stride::mapping` are declared
TEST(Mdspan, LayoutStride) { using arene::base::layout_stride; }

/// @test `is_layout_mapping_v` is declared
TEST(Mdspan, IsLayoutMappingV) { using arene::base::is_layout_mapping_v; }

/// @test `is_layout_mapping_policy_v` is declared
TEST(Mdspan, IsLayoutMappingPolicyV) { using arene::base::is_layout_mapping_policy_v; }

/// @test `is_accessor_policy` is declared
TEST(Mdspan, IsAccessorPolicyV) { using arene::base::is_accessor_policy_v; }

/// @test `mdspan` is declared
TEST(Mdspan, MdspanIsDeclared) { using arene::base::mdspan; }

/// @test `submdspan_mapping_result` is declared
TEST(Mdspan, SubmdspanMappingResultIsDeclared) { using arene::base::submdspan_mapping_result; }

/// @test `full_extent` is declared
TEST(Mdspan, FullExtentIsDeclared) { using arene::base::full_extent; }

/// @test `full_extent_t` is declared
TEST(Mdspan, FullExtentTIsDeclared) { using arene::base::full_extent_t; }

/// @test `submdspan_subextents_type` is declared
TEST(Mdspan, SubmdspanSubextentType) { using arene::base::submdspan_subextents_type; }

/// @test `submdspan_subextents_type_t` is declared
TEST(Mdspan, SubmdspanSubextentTypeT) { using arene::base::submdspan_subextents_type_t; }

}  // namespace
