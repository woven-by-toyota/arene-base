// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_REDUCE_FWD_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_REDUCE_FWD_HPP_

#include "arene/base/algorithm/reduce.hpp"

namespace reduce_test {
namespace config {

/// @brief tag type used to customize test dependencies
struct override {};

/// @brief specifies the variant of the generalized transform reduce under test
enum struct flavor { reduce, accumulate, std_accumulate };

/// @brief specifies which version of @c reduce is being tested
///
/// Used to configure tests for the @c reduce algorithm. For example,
/// tests will check that operations are applied in a specific order for the
/// @c inner_product variants.
///
template <class>
constexpr auto version = flavor::reduce;

/// @brief specifies the function under test
///
/// Used to configure tests for the @c reduce algorithm. This is the function under
/// test and must be provided as a function object -- overload sets cannot be
/// passed as arguments.
///
template <class>
constexpr auto const& function_under_test = arene::base::reduce;

}  // namespace config
}  // namespace reduce_test
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_REDUCE_FWD_HPP_
