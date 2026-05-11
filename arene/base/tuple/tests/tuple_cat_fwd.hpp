// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_TESTS_TUPLE_CAT_FWD_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_TESTS_TUPLE_CAT_FWD_HPP_

#include "arene/base/tuple/tuple_cat.hpp"

namespace tuple_cat_test {
namespace config {

/// @brief tag type used to customize test dependencies
struct override {};

/// @brief specifies the tuple_cat implementation under test
enum struct flavor {
  tuple_cat,
  std_tuple_cat,
};

/// @brief specifies which version of tuple_cat is being tested
///
/// Used to configure tests for tuple_cat. Specialize this template on
/// @c override to select the flavor.
template <class>
constexpr flavor version = flavor::tuple_cat;

/// @brief specifies the function under test
///
/// Used to configure tests for tuple_cat. This is the function under test and
/// must be provided as a function object -- overload sets cannot be passed as
/// arguments.
template <class>
constexpr auto const& function_under_test = arene::base::tuple_cat;

}  // namespace config
}  // namespace tuple_cat_test

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_TESTS_TUPLE_CAT_FWD_HPP_
