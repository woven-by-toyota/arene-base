// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/submdspan_mapping_result.hpp"

#include <gtest/gtest.h>

#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @test Check that submdspan_mapping_result has the required fields and that they are default-initialized.
CONSTEXPR_TEST(SubmdspanMappingResultTest, HasRequiredFieldsWhichAreDefaultInitialized) {
  using mapping = arene::base::layout_right::mapping<arene::base::extents<std::size_t>>;

  CONSTEXPR_ASSERT(arene::base::submdspan_mapping_result<mapping>{}.offset == std::size_t{});
  CONSTEXPR_ASSERT(arene::base::submdspan_mapping_result<mapping>{}.mapping == mapping{});
}

}  // namespace
