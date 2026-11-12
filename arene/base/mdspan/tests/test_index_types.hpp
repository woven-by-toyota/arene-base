// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_TESTS_TEST_INDEX_TYPES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_TESTS_TEST_INDEX_TYPES_HPP_

#include "arene/base/type_list/concat.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace test {

// NOLINTBEGIN(google-runtime-int)

/// @brief a list of possible index types for 'mdspan' and friends
using index_types = arene::base::type_lists::concat_t<  //
    testing::signed_integer_types,
    testing::unsigned_integer_types>;

// NOLINTEND(google-runtime-int)

}  // namespace test

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_TESTS_TEST_INDEX_TYPES_HPP_
