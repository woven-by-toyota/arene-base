// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/cstddef"

#include "testlibs/minitest/minitest.hpp"

namespace {
/// @test Ensure that the @c size_t type alias is defined correctly
TEST(BuiltinTypes, SizeT) {
  ::testing::StaticAssertTypeEq<std::size_t, ::size_t>();
  ::testing::StaticAssertTypeEq<std::size_t, decltype(sizeof(0))>();
}

/// @test Ensure that the @c ptrdiff_t type alias is defined correctly
TEST(BuiltinTypes, PtrdiffT) {
  ::testing::StaticAssertTypeEq<::ptrdiff_t, std::ptrdiff_t>();
  struct local {};
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  local const dummy[2]{};
  ::testing::StaticAssertTypeEq<std::ptrdiff_t, decltype(&dummy[1] - &dummy[0])>();
}

/// @test Ensure that the @c std::nullptr_t type alias is defined correctly
TEST(BuiltinTypes, NullptrT) { ::testing::StaticAssertTypeEq<decltype(nullptr), std::nullptr_t>(); }

/// @test Ensure that the @c std::max_align_t type alias is the same as @c ::max_align_t and has appropriate alignment
TEST(BuiltinTypes, MaxAlignT) {
  ::testing::StaticAssertTypeEq<::max_align_t, std::max_align_t>();
  struct local {
    char a;
    // NOLINTNEXTLINE(google-runtime-int)
    short b;
    int c;
    // NOLINTNEXTLINE(google-runtime-int)
    long d;
    // NOLINTNEXTLINE(google-runtime-int)
    long long e;
    float f;
    double g;
    long double h;
    void* i;
    int local::*j;
    int (local::*k)(int, double);
  };
  static_assert((alignof(std::max_align_t) == alignof(::max_align_t)), "Correct alignment");
  static_assert((alignof(std::max_align_t) >= alignof(local)), "Correct alignment");
}

}  // namespace
