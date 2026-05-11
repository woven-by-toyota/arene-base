// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/mdspan/submdspan.hpp"
#include "arene/base/mdspan/tests/submdspan_slices_handling_death_test_common.hpp"  // IWYU pragma: keep
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/make_tuple.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
// IWYU pragma: no_include "testlibs/minitest/minitest.hpp"

namespace {

struct test_config {
  template <class Slice>
  static auto function_under_test(Slice slice) {
    return arene::base::submdspan(
        arene::base::mdspan<std::int32_t, arene::base::extents<std::int8_t, 3>, arene::base::layout_left>{nullptr},
        slice
    );
  }
};

using test_config_type = ::testing::Types<test_config>;

INSTANTIATE_TYPED_TEST_SUITE_P(SubmdspanDeathTest, SubmdspanSliceHandlingCommonDeathTest, test_config_type, );

}  // namespace
