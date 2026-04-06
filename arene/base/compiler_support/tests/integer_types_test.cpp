// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/integer_types.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstdint.hpp"

namespace {
/// @test Ensure that the @c arene::base::uint32_t is the same as @c std::uint32_t
TEST(IntegerTypes, Uint32T) { ::testing::StaticAssertTypeEq<arene::base::uint32_t, std::uint32_t>(); }

/// @test Ensure that the @c arene::base::uint8_t is the same as @c std::uint8_t
TEST(IntegerTypes, Uint8T) { ::testing::StaticAssertTypeEq<arene::base::uint8_t, std::uint8_t>(); }

/// @test Ensure that the @c arene::base::uint16_t is the same as @c std::uint16_t
TEST(IntegerTypes, Uint16T) { ::testing::StaticAssertTypeEq<arene::base::uint16_t, std::uint16_t>(); }

/// @test Ensure that the @c arene::base::uint64_t is the same as @c std::uint64_t
TEST(IntegerTypes, Uint64T) { ::testing::StaticAssertTypeEq<arene::base::uint64_t, std::uint64_t>(); }

/// @test Ensure that the @c arene::base::int32_t is the same as @c std::int32_t
TEST(IntegerTypes, Int32T) { ::testing::StaticAssertTypeEq<arene::base::int32_t, std::int32_t>(); }

/// @test Ensure that the @c arene::base::int8_t is the same as @c std::int8_t
TEST(IntegerTypes, Int8T) { ::testing::StaticAssertTypeEq<arene::base::int8_t, std::int8_t>(); }

/// @test Ensure that the @c arene::base::int16_t is the same as @c std::int16_t
TEST(IntegerTypes, Int16T) { ::testing::StaticAssertTypeEq<arene::base::int16_t, std::int16_t>(); }

/// @test Ensure that the @c arene::base::int64_t is the same as @c std::int64_t
TEST(IntegerTypes, Int64T) { ::testing::StaticAssertTypeEq<arene::base::int64_t, std::int64_t>(); }

}  // namespace
