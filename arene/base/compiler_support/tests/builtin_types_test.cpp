// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/builtin_types.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"

namespace {
/// @test Ensure that the @c arene::base::size_t is the same as @c std::size_t
TEST(BuiltinTypes, SizeT) { ::testing::StaticAssertTypeEq<arene::base::size_t, std::size_t>(); }

/// @test Ensure that the @c arene::base::ptrdiff_t is the same as @c std::ptrdiff_t
TEST(BuiltinTypes, PtrdiffT) { ::testing::StaticAssertTypeEq<arene::base::ptrdiff_t, std::ptrdiff_t>(); }

/// @test Ensure that the @c arene::base::nullptr_t is the same as @c std::nullptr_t
TEST(BuiltinTypes, NullptrT) { ::testing::StaticAssertTypeEq<arene::base::nullptr_t, std::nullptr_t>(); }

/// @test Ensure that the @c arene::base::max_align_t is the same as @c std::max_align_t
TEST(BuiltinTypes, MaxAlignT) { ASSERT_EQ(alignof(arene::base::max_align_t), alignof(std::max_align_t)); }

/// @test Ensure that the @c arene::base::uintptr_t is the same as @c std::uintptr_t
TEST(BuiltinTypes, UintptrT) { ::testing::StaticAssertTypeEq<arene::base::uintptr_t, std::uintptr_t>(); }

}  // namespace
