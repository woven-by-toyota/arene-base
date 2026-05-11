// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/functional/is_noexcept_part_of_function_type.hpp"

#include <gtest/gtest.h>

namespace {

/// @test In C++14 @c noexcept is not part of function type but in C++17 it is
TEST(IsNoexceptPartOfFunctionTypeTest, CppVersionTest) {
  static_assert(
      (__cplusplus == 201402L ? !arene::base::is_noexcept_part_of_function_type_v<int> : true),
      "noexcept not part of function type in C++14"
  );
  static_assert(
      (__cplusplus == 201703L ? arene::base::is_noexcept_part_of_function_type_v<int> : true),
      "noexcept part of function type in C++17"
  );
}

}  // namespace
