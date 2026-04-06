// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_IOTA_FWD_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_IOTA_FWD_HPP_

#include "arene/base/algorithm/iota.hpp"

namespace iota_test {
namespace config {

/// @brief tag type used to customize test dependencies
struct override {};

/// @brief specifies which version of @c iota is being tested
///
/// Used to configure tests for the @c iota algorithm. For example, tests will
/// enable constexpr testing if this value is @c false.
///
template <class>
constexpr auto std_version = false;

/// @brief specifies the function under test
///
/// Used to configure test for the @c iota algorithm. This is the function under
/// test and must be provided as a function object -- overload sets cannot be
/// passed as arguments.
///
template <class>
constexpr auto const& iota = arene::base::iota;

}  // namespace config
}  // namespace iota_test

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TESTS_IOTA_FWD_HPP_
