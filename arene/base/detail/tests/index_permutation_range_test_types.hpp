// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_TESTS_INDEX_PERMUTATION_RANGE_TEST_TYPES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_TESTS_INDEX_PERMUTATION_RANGE_TEST_TYPES_HPP_

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"  // IWYU pragma: export
#include "arene/base/stdlib_choice/cstddef.hpp"

namespace test {
using extents_types = ::testing::Types<  //
    arene::base::array<std::size_t, 0>,
    arene::base::array<std::size_t, 1>,
    arene::base::array<std::size_t, 3>  //
    >;
}  // namespace test

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_TESTS_INDEX_PERMUTATION_RANGE_TEST_TYPES_HPP_
