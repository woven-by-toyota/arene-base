// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/climits"

#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"

#ifndef CHAR_BIT
#error "CHAR_BIT not defined"
#endif
#ifndef SCHAR_MIN
#error "SCHAR_MIN not defined"
#endif
#ifndef SCHAR_MAX
#error "SCHAR_MAX not defined"
#endif
#ifndef UCHAR_MAX
#error "UCHAR_MAX not defined"
#endif
#ifndef CHAR_MIN
#error "CHAR_MIN not defined"
#endif
#ifndef CHAR_MAX
#error "CHAR_MAX not defined"
#endif
#ifndef MB_LEN_MAX
#error "MB_LEN_MAX not defined"
#endif
#ifndef SHRT_MIN
#error "SHRT_MIN not defined"
#endif
#ifndef SHRT_MAX
#error "SHRT_MAX not defined"
#endif
#ifndef USHRT_MAX
#error "USHRT_MAX not defined"
#endif
#ifndef INT_MIN
#error "INT_MIN not defined"
#endif
#ifndef INT_MAX
#error "INT_MAX not defined"
#endif
#ifndef UINT_MAX
#error "UINT_MAX not defined"
#endif
#ifndef LONG_MIN
#error "LONG_MIN not defined"
#endif
#ifndef LONG_MAX
#error "LONG_MAX not defined"
#endif
#ifndef ULONG_MAX
#error "ULONG_MAX not defined"
#endif
#ifndef LLONG_MIN
#error "LLONG_MIN not defined"
#endif
#ifndef LLONG_MAX
#error "LLONG_MAX not defined"
#endif
#ifndef ULLONG_MAX
#error "ULLONG_MAX not defined"
#endif

namespace {

/// @test @c CHAR_BIT must be an integer
TEST(CharBit, IsIntegral) { static_assert(std::is_integral_v<decltype(CHAR_BIT)>, "Must be integral"); }

// The type of the macro constants must be the "promoted type" of the type for which they refer (@c int for @c char and
// @c short)

/// @test @c CHAR_BIT must be 8
TEST(CharBit, Has8BitsPerByte) { static_assert(CHAR_BIT == 8, "Must be 8 bits per byte"); }

/// @test @c CHAR_MIN is of type @c int
TEST(CharMin, IsInt) { ::testing::StaticAssertTypeEq<decltype(CHAR_MIN), int>(); }

/// @test @c CHAR_MAX is of type @c int
TEST(CharMax, IsInt) { ::testing::StaticAssertTypeEq<decltype(CHAR_MAX), int>(); }

/// @test @c SCHAR_MIN is of type @c int
TEST(SCharMin, IsInt) { ::testing::StaticAssertTypeEq<decltype(SCHAR_MIN), int>(); }

/// @test @c SCHAR_MAX is of type @c int
TEST(SCharMax, IsInt) { ::testing::StaticAssertTypeEq<decltype(SCHAR_MAX), int>(); }

/// @test @c UCHAR_MAX is of type @c int
TEST(UCharMax, IsInt) { ::testing::StaticAssertTypeEq<decltype(UCHAR_MAX), int>(); }

/// @test @c SHRT_MIN is of type @c int
TEST(ShrtMin, IsInt) { ::testing::StaticAssertTypeEq<decltype(SHRT_MIN), int>(); }

/// @test @c SHRT_MAX is of type @c int
TEST(ShrtMax, IsInt) { ::testing::StaticAssertTypeEq<decltype(SHRT_MAX), int>(); }

/// @test @c USHRT_MAX is of type @c int
TEST(UShrtMax, IsInt) { ::testing::StaticAssertTypeEq<decltype(USHRT_MAX), int>(); }

/// @test @c INT_MIN is of type @c int
TEST(IntMin, IsInt) { ::testing::StaticAssertTypeEq<decltype(INT_MIN), int>(); }

/// @test @c INT_MAX is of type @c int
TEST(IntMax, IsInt) { ::testing::StaticAssertTypeEq<decltype(INT_MAX), int>(); }

/// @test @c UINT_MAX is of type @c unsigned @c int
TEST(UIntMax, IsUnsignedInt) { ::testing::StaticAssertTypeEq<decltype(UINT_MAX), unsigned>(); }

/// @test @c LONG_MIN is of type @c long
TEST(LongMin, IsLong) {
  // NOLINTNEXTLINE(google-runtime-int)
  ::testing::StaticAssertTypeEq<decltype(LONG_MIN), long>();
}

/// @test @c LONG_MAX is of type @c long
TEST(LongMax, IsLong) {
  // NOLINTNEXTLINE(google-runtime-int)
  ::testing::StaticAssertTypeEq<decltype(LONG_MAX), long>();
}

/// @test @c ULONG_MAX is of type @c unsigned @c long
TEST(ULongMax, IsUnsignedLong) {
  // NOLINTNEXTLINE(google-runtime-int)
  ::testing::StaticAssertTypeEq<decltype(ULONG_MAX), unsigned long>();
}

/// @test @c LONG LONG_MIN is of type @c long long
TEST(LongLongMin, IsLongLong) {
  // NOLINTNEXTLINE(google-runtime-int)
  ::testing::StaticAssertTypeEq<decltype(LLONG_MIN), long long>();
}

/// @test @c LONG LONG_MAX is of type @c long long
TEST(LongLongMax, IsLongLong) {
  // NOLINTNEXTLINE(google-runtime-int)
  ::testing::StaticAssertTypeEq<decltype(LLONG_MAX), long long>();
}

/// @test @c ULONG LONG_MAX is of type @c unsigned @c long long
TEST(ULongLongMax, IsUnsignedLongLong) {
  // NOLINTNEXTLINE(google-runtime-int)
  ::testing::StaticAssertTypeEq<decltype(ULLONG_MAX), unsigned long long>();
}

}  // namespace
