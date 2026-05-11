// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_TRANSFORM_REDUCE_FWD_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_TRANSFORM_REDUCE_FWD_HPP_

#include "arene/base/algorithm/transform_reduce.hpp"

namespace transform_reduce_test {
namespace config {

/// @brief tag type used to customize test dependencies
struct override {};

/// @brief specifies the variant of the generalized transform reduce under test
enum struct flavor { transform_reduce, inner_product, std_inner_product };

/// @brief specifies which version of @c transform_reduce is being tested
///
/// Used to configure tests for the @c transform_reduce algorithm. For example,
/// tests will check that operations are applied in a specific order for the
/// @c inner_product variants.
///
template <class>
constexpr auto version = flavor::transform_reduce;

/// @brief specifies the function under test
///
/// Used to configure tests for the @c transform_reduce algorithm. This is the function under
/// test and must be provided as a function object -- overload sets cannot be
/// passed as arguments.
///
template <class>
constexpr auto const& function_under_test = arene::base::transform_reduce;

}  // namespace config
}  // namespace transform_reduce_test
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_TRANSFORM_REDUCE_FWD_HPP_
