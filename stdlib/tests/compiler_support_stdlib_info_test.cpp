// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "testlibs/minitest/minitest.hpp"

namespace {
/// @test Ensure @c ARENE_STDLIB_LIBCXX is false
TEST(CompilerSupportStdLibInfo, AreneStdLibCxxIsFalse) {
  static_assert(ARENE_IS_OFF(ARENE_STDLIB_LIBCXX), "libc++ is not used if a target links against '//stdlib'.");
}

/// @test Ensure @c ARENE_STDLIB_LIBSTDCXX is false
TEST(CompilerSupportStdLibInfo, AreneStdLibStdCxxIsFalse) {
  static_assert(ARENE_IS_OFF(ARENE_STDLIB_LIBSTDCXX), "libstdc++ is not used if a target links against '//stdlib'.");
}

/// @test Ensure @c ARENE_STDLIB_LIBARENECXX is true
TEST(CompilerSupportStdLibInfo, AreneStdlibAreneCxxIsTrue) {
  static_assert(
      ARENE_IS_ON(ARENE_STDLIB_LIBARENECXX),
      "ARENE_STDLIB_LIBARENECXX should be on when a target links against '//stdlib'."
  );
}

}  // namespace
