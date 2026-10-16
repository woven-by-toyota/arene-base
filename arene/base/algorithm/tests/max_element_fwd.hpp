// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_MAX_ELEMENT_FWD_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_MAX_ELEMENT_FWD_HPP_

#include "arene/base/algorithm/max_element.hpp"

namespace max_element_test {
namespace config {

/// @brief tag type used to customize test dependencies
struct override {};

/// @brief specifies the variant of the generalized transform max_element under test
enum struct flavor { max_element, std_max_element };

/// @brief specifies which version of @c max_element is being tested
///
/// Used to configure tests for the @c max_element algorithm. For example,
/// tests may not run at compile-time for the @c std variant.
template <class>
constexpr auto version = flavor::max_element;

/// @brief specifies the function under test
///
/// Used to configure tests for the @c max_element algorithm. This is the function under
/// test and must be provided as a function object -- overload sets cannot be
/// passed as arguments.
template <class>
constexpr auto const& function_under_test = arene::base::max_element;

}  // namespace config
}  // namespace max_element_test
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_MAX_ELEMENT_FWD_HPP_
