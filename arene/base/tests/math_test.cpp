// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file math_test.cpp
/// @brief Provides unit tests to validate export of content in math_test.hpp
///

#include "arene/base/math.hpp"

#include <gtest/gtest.h>

namespace {

/// @test `log2` is declared
TEST(Math, Log2) { using arene::base::log2; }

/// @test `copysign` is declared
TEST(Math, copysign) { using arene::base::copysign; }

/// @test `gcd` is declared
TEST(Math, gcd) { using arene::base::gcd; }

/// @test `isfinite` is declared
TEST(Math, isfinite) { using arene::base::isfinite; }

/// @test `isnormal` is declared
TEST(Math, isnormal) { using arene::base::isnormal; }

/// @test `isinf` is declared
TEST(Math, isinf) { using arene::base::isinf; }

/// @test `isnan` is declared
TEST(Math, isnan) { using arene::base::isnan; }

/// @test `lcm` is declared
TEST(Math, lcm) { using arene::base::lcm; }

/// @test `signbit` is declared
TEST(Math, signbit) { using arene::base::signbit; }

/// @test `power_of_2` is declared
TEST(Math, PowerOf2) { using arene::base::power_of_2; }
/// @test `is_power_of_2` is declared
TEST(Math, IsPowerOf2) { using arene::base::is_power_of_2; }
/// @test `make_power_of_2_sequence` is declared
TEST(Math, MakePowerOf2Sequence) { using arene::base::make_power_of_2_sequence; }

/// @test `pi` is declared
TEST(Math, Pi) { using arene::base::numbers::pi; }

/// @test `pi_v` is declared
TEST(Math, PiV) { using arene::base::numbers::pi_v; }

}  // namespace
