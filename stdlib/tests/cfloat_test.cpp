// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/cfloat"

#include "testlibs/minitest/minitest.hpp"

#ifndef FLT_MIN
#error "FLT_MIN not defined"
#endif
#ifndef FLT_MAX
#error "FLT_MAX not defined"
#endif
#ifndef FLT_DIG
#error "FLT_DIG not defined"
#endif
#ifndef FLT_EPSILON
#error "FLT_EPSILON not defined"
#endif
#ifndef FLT_EVAL_METHOD
#error "FLT_EVAL_METHOD not defined"
#endif
#ifndef FLT_MANT_DIG
#error "FLT_MANT_DIG not defined"
#endif
#ifndef FLT_MAX_EXP
#error "FLT_MAX_EXP not defined"
#endif
#ifndef FLT_MAX_10_EXP
#error "FLT_MAX_10_EXP not defined"
#endif
#ifndef FLT_MIN_EXP
#error "FLT_MIN_EXP not defined"
#endif
#ifndef FLT_MIN_10_EXP
#error "FLT_MIN_10_EXP not defined"
#endif
#ifndef FLT_RADIX
#error "FLT_RADIX not defined"
#endif
#ifndef FLT_ROUNDS
#error "FLT_ROUNDS not defined"
#endif
#ifndef DBL_MIN
#error "DBL_MIN not defined"
#endif
#ifndef DBL_MAX
#error "DBL_MAX not defined"
#endif
#ifndef DBL_DIG
#error "DBL_DIG not defined"
#endif
#ifndef DBL_EPSILON
#error "DBL_EPSILON not defined"
#endif
#ifndef DBL_MANT_DIG
#error "DBL_MANT_DIG not defined"
#endif
#ifndef DBL_MAX_EXP
#error "DBL_MAX_EXP not defined"
#endif
#ifndef DBL_MAX_10_EXP
#error "DBL_MAX_10_EXP not defined"
#endif
#ifndef DBL_MIN_EXP
#error "DBL_MIN_EXP not defined"
#endif
#ifndef DBL_MIN_10_EXP
#error "DBL_MIN_10_EXP not defined"
#endif

namespace {

/// @test @c FLT_MIN is of type @c float
TEST(FltMin, IsFloat) { ::testing::StaticAssertTypeEq<decltype(FLT_MIN), float>(); }

/// @test @c FLT_MAX is of type @c float
TEST(FltMax, IsFloat) { ::testing::StaticAssertTypeEq<decltype(FLT_MAX), float>(); }

/// @test @c DBL_MIN is of type @c double
TEST(DblMin, IsDouble) { ::testing::StaticAssertTypeEq<decltype(DBL_MIN), double>(); }

/// @test @c DBL_MAX is of type @c double
TEST(DblMax, IsDouble) { ::testing::StaticAssertTypeEq<decltype(DBL_MAX), double>(); }

}  // namespace
