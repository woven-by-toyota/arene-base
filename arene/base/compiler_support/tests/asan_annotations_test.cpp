// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/asan_annotations.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/byte/byte.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"

namespace {

using arene::base::byte;
using arene::base::span;  // NOLINT(misc-unused-using-decls) clang tidy can't see the usage for templates through macros
using arene::base::literals::operator""_byte;

/// @test Invoking `ARENE_ASAN_UNPOISON_MEMORY_REGION` reverses the marking of `ARENE_ASAN_POISON_MEMORY_REGION`, so
/// accessing memory in that region does not trigger Address Sanitizer
TEST(AreneAsanPoisonUnPoisonMemoryRegion, AnUnpoisonedRegionPassesInAsan) {
  arene::base::array<byte, 10> buff{};
  ARENE_ASAN_POISON_MEMORY_REGION(buff.data(), sizeof(byte) * buff.size());
  ARENE_ASAN_UNPOISON_MEMORY_REGION(buff.data(), sizeof(byte) * buff.size());
  for (std::size_t idx = 0; idx < buff.size(); ++idx) {
    // what this read does doesn't matter, we just have to read something
    EXPECT_EQ(buff[idx], 0_byte) << "Failed to read at: " << idx;
  }
}

/// @test Accessing elements of an array outside the region marker with `ARENE_ASAN_POISON_MEMORY_REGION` does not
/// trigger Address Sanitizer
TEST(AreneAsanPoisonUnPoisonMemoryRegion, RegionPoisonedDoesNotPoisonOutsideOfSize) {
  arene::base::array<byte, 10> buff{};
  auto const start_offset = buff.size() / 4;
  auto const num_elements = buff.size() / 2;
  ARENE_ASAN_POISON_MEMORY_REGION(&buff[start_offset], sizeof(byte) * num_elements);
  for (std::size_t idx = 0; idx < start_offset; ++idx) {
    // what this read does doesn't matter, we just have to read something
    EXPECT_EQ(buff[idx], 0_byte) << "Failed to read at: " << idx;
  }
  for (std::size_t idx = start_offset + num_elements + 1; idx < buff.size(); ++idx) {
    // what this read does doesn't matter, we just have to read something
    EXPECT_EQ(buff[idx], 0_byte) << "Failed to read at: " << idx;
  }
}

/// @test If Address Sanitizer is *not* enabled, accessing an array element inside a region marked with
/// `ARENE_ASAN_POISON_MEMORY_REGION` is OK
TEST(AreneAsanPoisonUnPoisonMemoryRegion, PoisoningIsANoopIfNotUnderAsan) {
  // This has to be done as conditional compilation because the compilers see `ARENE_IS_OFF` as unconditionally false if
  // ASAN isn't enabled; they can't see that there are different conditional compilations that cause it to be true.
#if (ARENE_IS_OFF(ARENE_ASAN_ENABLED))
  arene::base::array<byte, 10> buff{};
  ARENE_ASAN_POISON_MEMORY_REGION(buff.data(), sizeof(byte) * buff.size());
  for (std::size_t idx = 0U; idx < buff.size(); ++idx) {
    EXPECT_NO_FATAL_FAILURE(buff[idx];) << "Triggered failure for element: " << idx;
  }
#else
  GTEST_SKIP() << "Only meaningful not under asan.";
#endif
}

template <typename T>
struct AreneAsanPoisonUnPoisonMemorySpanTest : public testing::Test {
  arene::base::array<T, 16> buff{};
};

struct big_type {
  arene::base::array<std::int64_t, 100> data;
};

auto operator==(big_type const& lhs, big_type const& rhs) -> bool { return lhs.data == rhs.data; }

// Manual memory poisoning doesn't work reliably for non-power-of-2 sizes for objects less than 4 bytes due to asan
// alignment restrictions. See the official doc:
// https://github.com/google/sanitizers/wiki/AddressSanitizerManualPoisoning
using element_types = ::testing::Types<byte, std::int8_t, std::int16_t, std::int32_t, std::int64_t, big_type>;

TYPED_TEST_SUITE(AreneAsanPoisonUnPoisonMemorySpanTest, element_types, );

/// @test Invoking `ARENE_ASAN_UNPOISON_MEMORY_SPAN` reverses the marking of `ARENE_ASAN_POISON_MEMORY_SPAN`, so
/// accessing memory in that region does not trigger Address Sanitizer
/// @tparam TypeParam The element type of the `span`
TYPED_TEST(AreneAsanPoisonUnPoisonMemorySpanTest, UnpoisonedRegionsAreSafeToRead) {
  ARENE_ASAN_POISON_MEMORY_SPAN(span<TypeParam>{this->buff});
  ARENE_ASAN_UNPOISON_MEMORY_SPAN(span<TypeParam>{this->buff});
  TypeParam const some_val{};
  for (std::size_t idx = 0; idx < this->buff.size(); ++idx) {
    // what this read does doesn't matter, we just have to read something
    EXPECT_EQ(this->buff[idx], some_val) << "Failed to read at: " << idx;
  }
}

/// @test Accessing elements of a `span` outside the region marker with `ARENE_ASAN_POISON_MEMORY_SPAN` does not
/// trigger Address Sanitizer
/// @tparam TypeParam The element type of the `span`
TYPED_TEST(AreneAsanPoisonUnPoisonMemorySpanTest, RegionsOutsideAPoisonedRegionAreSafeToRead) {
  auto const start_offset = this->buff.size() / 4;
  auto const num_elements = this->buff.size() / 2;
  ARENE_ASAN_POISON_MEMORY_SPAN((span<TypeParam>{&this->buff[start_offset], num_elements}));
  TypeParam const some_val{};
  for (std::size_t idx = 0; idx < start_offset; ++idx) {
    // what this read does doesn't matter, we just have to read something
    EXPECT_EQ(this->buff[idx], some_val) << "Failed to read at: " << idx;
  }
  for (std::size_t idx = start_offset + num_elements + 1; idx < this->buff.size(); ++idx) {
    // what this read does doesn't matter, we just have to read something
    EXPECT_EQ(this->buff[idx], some_val) << "Failed to read at: " << idx;
  }
}

/// @test If Address Sanitizer is *not* enabled, accessing data inside a region marked with
/// `ARENE_ASAN_POISON_MEMORY_SPAN` is OK
/// @tparam TypeParam The element type of the `span`
TYPED_TEST(AreneAsanPoisonUnPoisonMemorySpanTest, PoisoningIsANoopNotUnderAsan) {
  // This has to be done as conditional compilation because the compilers see `ARENE_IS_OFF` as unconditionally false if
  // ASAN isn't enabled; they can't see that there are different conditional compilations that cause it to be true.
#if (ARENE_IS_OFF(ARENE_ASAN_ENABLED))
  ARENE_ASAN_POISON_MEMORY_SPAN(span<TypeParam>{this->buff});
  for (std::size_t idx = 0U; idx < this->buff.size(); ++idx) {
    EXPECT_NO_FATAL_FAILURE(this->buff[idx];) << "Triggered failure for element: " << idx;
  }
#else
  GTEST_SKIP() << "Only meaningful not under asan.";
#endif
}

}  // namespace
