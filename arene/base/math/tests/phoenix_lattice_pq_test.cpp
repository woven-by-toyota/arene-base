// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/math/phoenix_lattice_pq.hpp"

#include <gtest/gtest.h>

#include "arene/base/math/gcd.hpp"
#include "arene/base/math/lcm.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @test `isqrt_pq` computes exact floor square roots at compile time and runtime.
TEST(PhoenixLatticePqTest, IsqrtPqExact) {
  STATIC_ASSERT_EQ(arene::base::isqrt_pq(0U), 0U);
  STATIC_ASSERT_EQ(arene::base::isqrt_pq(1U), 1U);
  STATIC_ASSERT_EQ(arene::base::isqrt_pq(2U), 1U);
  STATIC_ASSERT_EQ(arene::base::isqrt_pq(3U), 1U);
  STATIC_ASSERT_EQ(arene::base::isqrt_pq(4U), 2U);
  STATIC_ASSERT_EQ(arene::base::isqrt_pq(15U), 3U);
  STATIC_ASSERT_EQ(arene::base::isqrt_pq(16U), 4U);
  STATIC_ASSERT_EQ(arene::base::isqrt_pq(std::uint64_t{1000000000000ULL}), std::uint64_t{1000000ULL});
}

/// @test `phoenix_phase_from_ratio_z32` and `classify_phoenix_lattice_z32` partition Z_{2^32} into 5 lattice states.
TEST(PhoenixLatticePqTest, PhaseRatioAndLatticeClassification) {
  constexpr std::uint32_t phase_0{arene::base::phoenix_phase_from_ratio_z32(0, 360)};
  constexpr std::uint32_t phase_45{arene::base::phoenix_phase_from_ratio_z32(45, 360)};
  constexpr std::uint32_t phase_90{arene::base::phoenix_phase_from_ratio_z32(90, 360)};
  constexpr std::uint32_t phase_135{arene::base::phoenix_phase_from_ratio_z32(135, 360)};
  constexpr std::uint32_t phase_180{arene::base::phoenix_phase_from_ratio_z32(180, 360)};
  constexpr std::uint32_t phase_neg90{arene::base::phoenix_phase_from_ratio_z32(-90, 360)};
  constexpr std::uint32_t phase_270{arene::base::phoenix_phase_from_ratio_z32(270, 360)};

  STATIC_ASSERT_EQ(phase_0, 0U);
  STATIC_ASSERT_EQ(phase_neg90, phase_270);

  STATIC_ASSERT_EQ(
      arene::base::classify_phoenix_lattice_z32(arene::base::phoenix_shortest_delta_z32(phase_0, phase_0)),
      arene::base::PhoenixInterferenceState::kConstructive
  );
  STATIC_ASSERT_EQ(
      arene::base::classify_phoenix_lattice_z32(arene::base::phoenix_shortest_delta_z32(phase_45, phase_0)),
      arene::base::PhoenixInterferenceState::kPartialConstructive
  );
  STATIC_ASSERT_EQ(
      arene::base::classify_phoenix_lattice_z32(arene::base::phoenix_shortest_delta_z32(phase_90, phase_0)),
      arene::base::PhoenixInterferenceState::kQuadrature
  );
  STATIC_ASSERT_EQ(
      arene::base::classify_phoenix_lattice_z32(arene::base::phoenix_shortest_delta_z32(phase_135, phase_0)),
      arene::base::PhoenixInterferenceState::kPartialDestructive
  );
  STATIC_ASSERT_EQ(
      arene::base::classify_phoenix_lattice_z32(arene::base::phoenix_shortest_delta_z32(phase_180, phase_0)),
      arene::base::PhoenixInterferenceState::kDestructive
  );
  STATIC_ASSERT_EQ(
      arene::base::phoenix_lattice_gain_q16(arene::base::PhoenixInterferenceState::kConstructive),
      65536
  );
}

/// @test `catmull_rom_bessel_pq` reproduces a quadratic trajectory f(t) = 3*t^2 - 7*t + 50 across non-uniform gaps with
/// 0 error.
TEST(PhoenixLatticePqTest, BesselOverhauserCatmullRomExactQuadratic) {
  auto const quad = [](std::int64_t const t) constexpr noexcept -> std::int64_t {
    return (3 * t * t) - (7 * t) + 50;
  };
  // Anchors at t0 = 2, t1 = 3, t2 = 7 (3 missing frames at t = 4, 5, 6), t3 = 8
  constexpr std::int64_t y0{quad(2)};
  constexpr std::int64_t y1{quad(3)};
  constexpr std::int64_t y2{quad(7)};
  constexpr std::int64_t y3{quad(8)};

  STATIC_ASSERT_EQ(arene::base::catmull_rom_bessel_pq(y0, y1, y2, y3, 1, 4, 1, 1), quad(4));
  STATIC_ASSERT_EQ(arene::base::catmull_rom_bessel_pq(y0, y1, y2, y3, 1, 4, 1, 2), quad(5));
  STATIC_ASSERT_EQ(arene::base::catmull_rom_bessel_pq(y0, y1, y2, y3, 1, 4, 1, 3), quad(6));
}

/// @test `gcd` enforces the signed `min()` precondition before modulo, preventing `INT_MIN % -1` UB/SIGFPE.
TEST(PhoenixLatticePqTest, GcdMinPreconditionDeathTest) {
  ASSERT_DEATH(
      arene::base::gcd(std::numeric_limits<std::int32_t>::min(), std::int32_t{-1}),
      "Precondition violation: value != std::numeric_limits<Number>::min()"
  );
  ASSERT_DEATH(
      arene::base::gcd(std::numeric_limits<std::int32_t>::min(), std::int32_t{2}),
      "Precondition violation: value != std::numeric_limits<Number>::min()"
  );
}

}  // namespace
