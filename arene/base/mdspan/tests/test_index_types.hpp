// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_TESTS_TEST_INDEX_TYPES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_TESTS_TEST_INDEX_TYPES_HPP_

#include <gtest/gtest.h>

namespace test {

// NOLINTBEGIN(google-runtime-int)

/// @brief a list of possible index types for 'mdspan' and friends
/// @note This needs to be duplicated from
///   '/testlibs/utilities/common_test_types.hpp' due to the assumption that
///   'Types' is a class template (it is not in Googletest).
using index_types = ::testing::Types<
    signed char,
    short,
    int,
    long,
    long long,
    unsigned char,
    unsigned short,
    unsigned int,
    unsigned long,
    unsigned long long>;

// NOLINTEND(google-runtime-int)

}  // namespace test

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_TESTS_TEST_INDEX_TYPES_HPP_
