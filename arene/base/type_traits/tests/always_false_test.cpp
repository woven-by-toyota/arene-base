// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/always_false.hpp"

#include <array>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::always_false_v;

using types = ::testing::Types<char, int, double, std::string, std::vector<int>, std::array<int, 10>>;

template <typename T>
class AlwaysFalseTest : public ::testing::Test {};

TYPED_TEST_SUITE(AlwaysFalseTest, types, );

/// @test `always_false_v` always evaluates to false, whatever type parameter is supplied.
/// @tparam TypeParam The type to use as the parameter to `always_false_v`
TYPED_TEST(AlwaysFalseTest, IsAlwaysFalse) { STATIC_ASSERT_FALSE(always_false_v<TypeParam>); }

template <typename T>
struct a_template_never_instanced_does_not_trigger_static_assert {
  static_assert(always_false_v<T>, "Always false was true for uninstantiated template!");
};

}  // namespace
