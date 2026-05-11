// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/platform_queries/has_builtin_atomic.hpp"

#include <cstdint>

#include <gtest/gtest.h>

#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @test Ensure `ARENE_HAS_BUILTIN_ATOMIC` property check is defined
TEST(BuiltinAtomic, HasBuiltinAtomicIsProperlyDefinedAsConfig) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_HAS_BUILTIN_ATOMIC));
}

/// @test Verify atomic intrinsics are available on GCC and Clang
TEST(BuiltinAtomic, AtomicIntrinsicsAvailableOnGccAndClang) {
#if defined(__GNUC__) || defined(__clang__)
  STATIC_ASSERT_TRUE(ARENE_IS_ON(ARENE_HAS_BUILTIN_ATOMIC));
#else
  GTEST_SKIP() << "Not on GCC or Clang";
#endif
}

/// @test Verify atomic capabilities
TEST(BuiltinAtomic, AtomicCapabilities) {
#if ARENE_IS_ON(ARENE_HAS_BUILTIN_ATOMIC)
  // Test that we can actually use atomic operations if they're reported as available
  std::uint64_t value = 42;
  std::uint64_t result = __atomic_add_fetch(&value, 10, __ATOMIC_SEQ_CST);
  EXPECT_EQ(result, 52);
  EXPECT_EQ(value, 52);

  result = __atomic_fetch_add(&value, 5, __ATOMIC_SEQ_CST);
  EXPECT_EQ(result, 52);
  EXPECT_EQ(value, 57);

  result = __atomic_load_n(&value, __ATOMIC_SEQ_CST);
  EXPECT_EQ(result, 57);
#else
  GTEST_SKIP() << "Atomic operations not available on this platform";
#endif
}

}  // namespace
