// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_SORT_FWD_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_SORT_FWD_HPP_

#include "arene/base/algorithm/sort.hpp"

namespace sort_test {
namespace config {

/// @brief tag type used to customize test dependencies
struct override {};

/// @brief specifies the sort algorithm under test
enum struct flavor {
  sort,
  std_sort,
  stable_sort,
  std_stable_sort,
};

/// @brief specifies which version of sort is being tested
///
/// Used to configure tests for a sort algorithm. For example, tests will
/// enable constexpr testing if this value is @c std_sort.
///
template <class>
constexpr flavor version = flavor::sort;

/// @brief specifies the function under test
///
/// Used to configure test for a sort algorithm. This is the function under
/// test and must be provided as a function object -- overload sets cannot be
/// passed as arguments.
///
template <class>
constexpr auto const& function_under_test = arene::base::sort;

}  // namespace config
}  // namespace sort_test

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_SORT_FWD_HPP_
