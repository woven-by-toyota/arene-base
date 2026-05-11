// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/is_instantiation_of.hpp"

#include <gtest/gtest.h>

namespace {

using arene::base::is_instantiation_of_v;

struct a {};
struct b {};

template <class T>
struct unary_template1 {};

template <class T>
struct unary_template2 {};

template <class T, class U>
struct binary_template1 {};

template <class T, class U>
struct binary_template2 {};

/// @test `is_instantiation_of_v` evaluates to `true` if the type provided as the first template parameter is an
/// instantiation of the template provided as the second template parameter, and `false` otherwise, where the type is an
/// instantiation of a template with a single argument
TEST(IsInstantiationOf, Unary) {
  EXPECT_TRUE((is_instantiation_of_v<unary_template1<a>, unary_template1>));
  EXPECT_FALSE((is_instantiation_of_v<unary_template1<a>, unary_template2>));
  EXPECT_FALSE((is_instantiation_of_v<unary_template1<a>, binary_template1>));
  EXPECT_FALSE((is_instantiation_of_v<unary_template1<a>, binary_template2>));
}

/// @test `is_instantiation_of_v` evaluates to `true` if the type provided as the first template parameter is an
/// instantiation of the template provided as the second template parameter, and `false` otherwise, where the type is an
/// instantiation of a template with two arguments
TEST(IsInstantiationOf, Binary) {
  EXPECT_FALSE((is_instantiation_of_v<binary_template1<a, b>, unary_template1>));
  EXPECT_FALSE((is_instantiation_of_v<binary_template1<a, b>, unary_template2>));
  EXPECT_TRUE((is_instantiation_of_v<binary_template1<a, b>, binary_template1>));
  EXPECT_FALSE((is_instantiation_of_v<binary_template1<a, b>, binary_template2>));
}

/// @test `is_instantiation_of_v` evaluates to `false` if the type provided as the first template parameter is not an
/// instantiation of any template
TEST(IsInstantiationOf, NonTemplate) {
  EXPECT_FALSE((is_instantiation_of_v<int, unary_template1>));
  EXPECT_FALSE((is_instantiation_of_v<int, unary_template2>));
  EXPECT_FALSE((is_instantiation_of_v<int, binary_template1>));
  EXPECT_FALSE((is_instantiation_of_v<int, binary_template2>));
}

}  // namespace
