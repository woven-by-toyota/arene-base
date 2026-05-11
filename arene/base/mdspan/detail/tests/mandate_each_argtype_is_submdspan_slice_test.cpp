// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/detail/mandate_each_argtype_is_submdspan_slice.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstddef.hpp"

namespace {

/// @test helper @c mandate_each_argtype_is_submdspan_slice returns @c nullptr
TEST(SubmdspanDetailMandateEachArgtypeIsSubmdspanSlice, MandateHelpersReturnsNullptPtr) {
  // this is only called at compile-time so we invoke it at runtime for line coverage
  using arene::base::mdspan_detail::mandate_each_argtype_is_submdspan_slice;

  ASSERT_EQ(nullptr, mandate_each_argtype_is_submdspan_slice<int>());
  ASSERT_EQ(nullptr, mandate_each_argtype_is_submdspan_slice<std::size_t>());
}

}  // namespace
