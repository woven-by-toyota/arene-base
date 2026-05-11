// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/array.hpp"
#include "arene/base/byte.hpp"
#include "arene/base/compiler_support/asan_annotations.hpp"  // IWYU pragma: keep
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"

namespace {

using arene::base::byte;
using arene::base::span;  // NOLINT(misc-unused-using-decls) clang tidy can't see the usage for templates through macros
using arene::base::literals::operator""_byte;  // NOLINT(misc-unused-using-decls) clang tidy can't see the usage

/// @test If Address Sanitizer is enabled, accessing an array element inside a region marked with
/// `ARENE_ASAN_POISON_MEMORY_REGION` terminates the program
TEST(AreneAsanPoisonUnPoisonMemoryRegionDeathTest, APoisonedRegionFailsInAsan) {
  // This has to be done as conditional compilation because the compilers see `ARENE_IS_OFF` as unconditionally false if
  // ASAN isn't enabled; they can't see that there are different conditional compilations that cause it to be true.
#if (ARENE_IS_OFF(ARENE_ASAN_ENABLED))
  GTEST_SKIP() << "Only meaningful under asan.";
#else
  arene::base::array<byte, 10> buff{};
  ARENE_ASAN_POISON_MEMORY_REGION(buff.data(), sizeof(byte) * buff.size());
  for (std::size_t idx = 0U; idx < buff.size(); ++idx) {
    EXPECT_DEATH(buff[idx] = 1_byte, "") << "Failed to trigger asan failure for element: " << idx;
  }
#endif
}

template <typename T>
struct AreneAsanPoisonUnPoisonMemorySpanTest : public testing::Test {
  arene::base::array<T, 16> buff{};
};

struct big_type {
  arene::base::array<std::int64_t, 100> data;
};

// Manual memory poisoning doesn't work reliably for non-power-of-2 sizes for objects less than 4 bytes due to asan
// alignment restrictions. See the official doc:
// https://github.com/google/sanitizers/wiki/AddressSanitizerManualPoisoning
using element_types = ::testing::Types<byte, std::int8_t, std::int16_t, std::int32_t, std::int64_t, big_type>;

template <typename T>
using AreneAsanPoisonUnPoisonMemorySpanDeathTest = AreneAsanPoisonUnPoisonMemorySpanTest<T>;

TYPED_TEST_SUITE(AreneAsanPoisonUnPoisonMemorySpanDeathTest, element_types, );

/// @test If Address Sanitizer is enabled, accessing data inside a region marked with `ARENE_ASAN_POISON_MEMORY_SPAN`
/// terminates the program
/// @tparam TypeParam The element type of the `span`
TYPED_TEST(AreneAsanPoisonUnPoisonMemorySpanDeathTest, APoisonedSpanFailsInAsan) {
  // This has to be done as conditional compilation because the compilers see `ARENE_IS_OFF` as unconditionally false if
  // ASAN isn't enabled; they can't see that there are different conditional compilations that cause it to be true.
#if (ARENE_IS_OFF(ARENE_ASAN_ENABLED))
  GTEST_SKIP() << "Only meaningful under asan.";
#else
  ARENE_ASAN_POISON_MEMORY_SPAN(span<TypeParam>{this->buff});
  for (std::size_t idx = 0U; idx < this->buff.size(); ++idx) {
    EXPECT_DEATH(this->buff[idx] = {}, "") << "Failed to trigger asan failure for element: " << idx;
  }
#endif
}

}  // namespace
