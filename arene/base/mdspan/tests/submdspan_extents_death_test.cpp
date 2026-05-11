// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/submdspan_extents.hpp"
#include "arene/base/mdspan/tests/submdspan_slices_handling_death_test_common.hpp"  // IWYU pragma: keep
#include "arene/base/stdlib_choice/cstdint.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/make_tuple.hpp"
// IWYU pragma: no_include "testlibs/minitest/minitest.hpp"

namespace {

struct test_config {
  template <class Slice>
  static auto function_under_test(Slice slice) {
    return arene::base::submdspan_extents(extents<std::int8_t, 3>{}, slice);
  }
};

using test_config_type = ::testing::Types<test_config>;

INSTANTIATE_TYPED_TEST_SUITE_P(SubmdspanExtentsDeathTest, SubmdspanSliceHandlingCommonDeathTest, test_config_type, );

}  // namespace
