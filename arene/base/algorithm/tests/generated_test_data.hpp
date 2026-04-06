// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_GENERATED_TEST_DATA_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_GENERATED_TEST_DATA_HPP_

#include "arene/base/array/array.hpp"
#include "testlibs/utilities/tuplet.hpp"

namespace algorithm_test {

// collection of arrays with randomly generated data (by wovey), where elements are in the range [0, 9]
constexpr auto test_data = testing::make_tuplet(
    // Size 2
    arene::base::to_array({1, 0}),
    arene::base::to_array({4, 3}),
    arene::base::to_array({3, 2}),
    arene::base::to_array({1, 8}),
    arene::base::to_array({1, 9}),
    arene::base::to_array({6, 0}),
    arene::base::to_array({0, 1}),
    arene::base::to_array({3, 3}),
    arene::base::to_array({8, 9}),
    arene::base::to_array({0, 8}),

    // Size 3
    arene::base::to_array({7, 7, 4}),
    arene::base::to_array({4, 5, 2}),
    arene::base::to_array({6, 4, 2}),
    arene::base::to_array({0, 7, 1}),
    arene::base::to_array({0, 6, 5}),
    arene::base::to_array({6, 9, 8}),
    arene::base::to_array({5, 2, 9}),
    arene::base::to_array({6, 0, 4}),
    arene::base::to_array({8, 0, 5}),
    arene::base::to_array({0, 7, 8}),

    // Size 4
    arene::base::to_array({0, 6, 9, 8}),
    arene::base::to_array({3, 8, 7, 7}),
    arene::base::to_array({7, 4, 4, 3}),
    arene::base::to_array({4, 3, 2, 4}),
    arene::base::to_array({5, 1, 7, 4}),
    arene::base::to_array({9, 6, 8, 3}),
    arene::base::to_array({4, 4, 3, 0}),
    arene::base::to_array({5, 1, 6, 8}),
    arene::base::to_array({7, 2, 8, 2}),
    arene::base::to_array({7, 0, 0, 9}),

    // Size 5
    arene::base::to_array({8, 6, 3, 7, 9}),
    arene::base::to_array({4, 4, 0, 8, 3}),
    arene::base::to_array({6, 5, 3, 0, 2}),
    arene::base::to_array({1, 6, 3, 7, 9}),
    arene::base::to_array({6, 1, 9, 0, 2}),
    arene::base::to_array({4, 1, 8, 8, 9}),
    arene::base::to_array({2, 8, 5, 8, 6}),
    arene::base::to_array({8, 0, 6, 3, 2}),
    arene::base::to_array({6, 4, 1, 4, 3}),
    arene::base::to_array({7, 0, 0, 4, 4}),

    // Size 6
    arene::base::to_array({7, 3, 5, 3, 2, 1}),
    arene::base::to_array({9, 2, 0, 3, 2, 4}),
    arene::base::to_array({1, 5, 0, 8, 7, 6}),
    arene::base::to_array({7, 2, 6, 3, 6, 4}),
    arene::base::to_array({4, 8, 8, 9, 3, 2}),
    arene::base::to_array({7, 3, 9, 1, 8, 0}),
    arene::base::to_array({0, 6, 7, 5, 2, 2}),
    arene::base::to_array({0, 4, 2, 4, 1, 8}),
    arene::base::to_array({7, 4, 1, 9, 7, 7}),
    arene::base::to_array({8, 1, 4, 2, 4, 2}),

    // Size 7
    arene::base::to_array({1, 8, 5, 8, 0, 6, 5}),
    arene::base::to_array({4, 9, 7, 3, 2, 3, 6}),
    arene::base::to_array({3, 4, 9, 4, 3, 0, 8}),
    arene::base::to_array({9, 7, 0, 0, 4, 2, 5}),
    arene::base::to_array({8, 5, 8, 1, 7, 5, 0}),
    arene::base::to_array({6, 1, 9, 0, 2, 5, 9}),
    arene::base::to_array({5, 4, 6, 4, 2, 4, 1}),
    arene::base::to_array({3, 3, 7, 9, 5, 8, 7}),
    arene::base::to_array({9, 0, 4, 7, 0, 7, 8}),
    arene::base::to_array({4, 9, 4, 8, 2, 9, 1}),

    // Size 8
    arene::base::to_array({4, 4, 6, 6, 4, 9, 5, 0}),
    arene::base::to_array({8, 1, 3, 5, 2, 0, 0, 7}),
    arene::base::to_array({3, 9, 9, 2, 3, 4, 6, 1}),
    arene::base::to_array({4, 5, 8, 5, 7, 0, 2, 7}),
    arene::base::to_array({1, 5, 0, 9, 2, 6, 6, 0}),
    arene::base::to_array({6, 6, 7, 5, 8, 9, 2, 1}),
    arene::base::to_array({5, 4, 7, 8, 6, 8, 6, 3}),
    arene::base::to_array({1, 8, 5, 0, 8, 3, 2, 2}),
    arene::base::to_array({9, 7, 3, 9, 4, 4, 7, 4}),
    arene::base::to_array({2, 6, 2, 0, 7, 1, 5, 1}),

    // Size 9
    arene::base::to_array({7, 4, 4, 0, 1, 6, 6, 3, 2}),
    arene::base::to_array({4, 3, 5, 2, 0, 5, 9, 1, 9}),
    arene::base::to_array({9, 7, 5, 5, 7, 0, 8, 0, 6}),
    arene::base::to_array({1, 2, 4, 5, 4, 7, 0, 5, 7}),
    arene::base::to_array({8, 0, 8, 0, 5, 1, 2, 0, 4}),
    arene::base::to_array({6, 1, 1, 6, 5, 9, 1, 3, 2}),
    arene::base::to_array({5, 8, 2, 2, 9, 7, 4, 8, 3}),
    arene::base::to_array({2, 6, 4, 4, 2, 1, 6, 6, 0}),
    arene::base::to_array({7, 6, 4, 1, 5, 4, 4, 7, 1}),
    arene::base::to_array({0, 4, 6, 0, 2, 6, 0, 8, 7}),

    // Size 10
    arene::base::to_array({4, 9, 3, 3, 0, 9, 8, 4, 6, 8}),
    arene::base::to_array({1, 0, 8, 1, 5, 8, 8, 4, 3, 9}),
    arene::base::to_array({0, 9, 2, 7, 8, 4, 1, 2, 6, 6}),
    arene::base::to_array({6, 7, 6, 5, 1, 5, 5, 3, 2, 7}),
    arene::base::to_array({1, 4, 0, 9, 8, 8, 0, 5, 4, 3}),
    arene::base::to_array({7, 8, 1, 7, 7, 0, 0, 9, 5, 1}),
    arene::base::to_array({3, 0, 7, 6, 6, 8, 4, 9, 2, 5}),
    arene::base::to_array({8, 2, 6, 4, 8, 4, 7, 5, 9, 2}),
    arene::base::to_array({2, 3, 3, 0, 2, 1, 2, 6, 4, 6}),
    arene::base::to_array({5, 1, 4, 8, 0, 5, 4, 0, 6, 0}),

    // Size 15
    arene::base::to_array({7, 5, 9, 8, 2, 1, 0, 4, 7, 5, 2, 6, 3, 0, 8}),
    arene::base::to_array({6, 9, 4, 6, 1, 2, 7, 7, 3, 5, 0, 7, 8, 1, 3}),
    arene::base::to_array({8, 4, 1, 4, 0, 2, 3, 9, 6, 8, 5, 7, 0, 8, 2}),
    arene::base::to_array({0, 2, 1, 0, 3, 2, 4, 8, 4, 9, 2, 6, 9, 5, 4}),
    arene::base::to_array({9, 6, 2, 0, 5, 7, 6, 4, 0, 7, 3, 9, 5, 2, 1}),
    arene::base::to_array({4, 7, 6, 7, 1, 5, 5, 8, 1, 6, 2, 3, 7, 9, 0}),
    arene::base::to_array({1, 1, 8, 6, 4, 3, 5, 7, 0, 1, 4, 4, 9, 8, 7}),
    arene::base::to_array({8, 0, 3, 2, 9, 7, 7, 5, 2, 2, 2, 6, 4, 4, 1}),
    arene::base::to_array({3, 4, 2, 0, 3, 0, 1, 1, 9, 5, 5, 8, 1, 7, 6}),
    arene::base::to_array({2, 9, 2, 7, 8, 1, 4, 5, 9, 4, 6, 0, 8, 2, 3}),

    // Size 20
    arene::base::to_array({5, 4, 0, 3, 7, 9, 8, 4, 3, 0, 2, 6, 9, 1, 4, 5, 3, 8, 1, 0}),
    arene::base::to_array({6, 7, 5, 2, 1, 8, 0, 6, 9, 3, 2, 4, 7, 8, 0, 9, 5, 6, 8, 7}),
    arene::base::to_array({8, 4, 8, 6, 6, 5, 1, 2, 6, 4, 9, 4, 8, 8, 0, 7, 5, 0, 9, 3}),
    arene::base::to_array({2, 9, 7, 2, 1, 1, 7, 0, 2, 8, 4, 5, 2, 3, 5, 4, 6, 9, 0, 1}),
    arene::base::to_array({3, 7, 0, 9, 1, 6, 3, 3, 8, 4, 7, 1, 4, 8, 2, 5, 8, 2, 6, 7}),
    arene::base::to_array({0, 4, 6, 4, 4, 3, 1, 9, 1, 8, 5, 0, 8, 7, 9, 9, 3, 6, 5, 8}),
    arene::base::to_array({9, 8, 5, 1, 2, 7, 8, 2, 3, 3, 9, 5, 0, 2, 1, 6, 4, 4, 7, 9}),
    arene::base::to_array({1, 0, 8, 3, 9, 6, 0, 2, 4, 4, 5, 7, 5, 6, 8, 8, 7, 3, 1, 0}),
    arene::base::to_array({7, 1, 6, 9, 8, 5, 8, 0, 2, 6, 6, 4, 2, 5, 0, 5, 8, 1, 7, 3}),
    arene::base::to_array({4, 2, 0, 9, 1, 3, 5, 7, 8, 3, 1, 9, 2, 1, 4, 6, 1, 0, 6, 5}),

    // Size 25
    arene::base::to_array({2, 7, 0, 9, 4, 3, 3, 2, 1, 8, 6, 8, 5, 1, 7, 2, 6, 0, 4, 4, 9, 8, 3, 7, 5}),
    arene::base::to_array({8, 1, 9, 5, 0, 6, 4, 2, 6, 3, 1, 0, 8, 6, 0, 2, 9, 5, 7, 4, 3, 1, 8, 3, 6}),
    arene::base::to_array({5, 8, 0, 9, 2, 7, 9, 7, 4, 4, 6, 0, 2, 6, 1, 3, 9, 0, 4, 9, 5, 8, 1, 6, 4}),
    arene::base::to_array({9, 3, 4, 0, 2, 3, 8, 6, 7, 1, 9, 5, 0, 7, 6, 2, 5, 5, 1, 4, 8, 0, 3, 8, 7}),
    arene::base::to_array({6, 2, 5, 2, 0, 4, 9, 1, 5, 8, 2, 3, 6, 9, 1, 4, 8, 3, 7, 6, 0, 5, 2, 4, 9}),
    arene::base::to_array({3, 0, 8, 6, 5, 9, 5, 3, 1, 7, 4, 4, 7, 0, 9, 8, 1, 6, 2, 2, 6, 1, 9, 0, 6}),
    arene::base::to_array({7, 9, 3, 6, 4, 2, 0, 5, 8, 2, 7, 3, 6, 5, 9, 4, 1, 7, 8, 9, 0, 8, 2, 6, 4}),
    arene::base::to_array({4, 8, 1, 0, 7, 3, 8, 3, 6, 6, 2, 2, 9, 6, 4, 7, 0, 0, 5, 5, 3, 1, 7, 8, 5}),
    arene::base::to_array({1, 2, 6, 5, 9, 7, 7, 4, 8, 3, 2, 8, 0, 9, 5, 5, 1, 6, 4, 4, 7, 0, 8, 3, 2}),
    arene::base::to_array({0, 4, 5, 9, 8, 1, 2, 0, 6, 7, 3, 3, 9, 4, 8, 2, 1, 5, 7, 6, 5, 9, 0, 4, 6}),

    // Size 50
    arene::base::to_array({7, 4, 1, 9, 2, 7, 6, 5, 8, 9, 0, 3, 4, 4, 6, 5, 0, 8, 7, 2, 3, 9, 2, 6, 4,
                           3, 8, 0, 7, 9, 0, 4, 1, 3, 5, 2, 5, 7, 3, 8, 1, 6, 2, 7, 4, 0, 9, 8, 5, 6}),
    arene::base::to_array({3, 6, 9, 8, 0, 4, 3, 2, 1, 5, 7, 0, 6, 4, 1, 1, 9, 0, 9, 8, 4, 7, 2, 8, 3,
                           4, 5, 0, 6, 8, 9, 3, 7, 2, 5, 1, 4, 8, 0, 6, 2, 3, 7, 9, 1, 0, 5, 9, 6, 4}),
    arene::base::to_array({1, 5, 0, 3, 8, 4, 6, 7, 9, 2, 0, 6, 1, 3, 4, 9, 5, 7, 8, 2, 6, 7, 1, 4, 3,
                           8, 0, 9, 5, 2, 4, 7, 6, 0, 3, 5, 9, 8, 2, 1, 4, 7, 0, 6, 1, 9, 5, 3, 2, 8}),
    arene::base::to_array({8, 2, 6, 9, 7, 0, 4, 1, 3, 5, 0, 8, 4, 6, 7, 9, 2, 5, 3, 1, 0, 4, 9, 7, 8,
                           6, 5, 2, 3, 8, 1, 9, 4, 7, 0, 5, 2, 3, 8, 1, 6, 4, 9, 7, 0, 5, 8, 2, 1, 3}),
    arene::base::to_array({6, 0, 3, 5, 8, 2, 4, 7, 9, 1, 0, 6, 4, 9, 7, 2, 3, 5, 1, 8, 0, 4, 7, 9, 6,
                           5, 8, 3, 1, 2, 4, 0, 9, 7, 6, 3, 8, 2, 5, 1, 0, 4, 7, 9, 6, 8, 3, 2, 5, 1}),
    arene::base::to_array({0, 4, 7, 9, 6, 8, 3, 2, 5, 1, 9, 7, 4, 0, 8, 3, 6, 2, 5, 1, 9, 4, 7, 0, 6,
                           8, 3, 1, 2, 5, 4, 9, 0, 7, 8, 6, 3, 2, 1, 5, 9, 0, 4, 6, 7, 8, 3, 2, 5, 1}),
    arene::base::to_array({4, 9, 0, 7, 6, 3, 8, 2, 1, 5, 7, 6, 9, 0, 8, 4, 1, 2, 3, 5, 0, 6, 7, 9, 4,
                           8, 3, 2, 1, 5, 6, 0, 4, 9, 7, 8, 3, 2, 1, 5, 0, 6, 4, 8, 9, 7, 3, 2, 5, 1}),
    arene::base::to_array({9, 7, 6, 4, 0, 8, 3, 2, 1, 5, 4, 9, 0, 7, 8, 6, 3, 2, 1, 5, 7, 9, 0, 4, 6,
                           8, 3, 1, 2, 5, 0, 9, 4, 7, 8, 6, 3, 2, 1, 5, 9, 4, 0, 6, 7, 8, 3, 2, 5, 1}),
    arene::base::to_array({2, 0, 6, 7, 4, 8, 9, 3, 1, 5, 6, 4, 7, 0, 9, 8, 3, 1, 2, 5, 0, 9, 4, 7, 8,
                           6, 3, 2, 1, 5, 7, 0, 4, 9, 6, 8, 3, 1, 2, 5, 4, 0, 9, 7, 6, 8, 3, 2, 5, 1}),
    arene::base::to_array({5, 8, 4, 9, 0, 6, 7, 3, 1, 2, 0, 9, 4, 7, 6, 8, 3, 2, 1, 5, 7, 0, 4, 9, 6,
                           8, 3, 2, 1, 5, 4, 0, 9, 7, 8, 6, 3, 2, 1, 5, 9, 0, 4, 6, 8, 7, 3, 2, 5, 1})
);

}  // namespace algorithm_test

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_GENERATED_TEST_DATA_HPP_
