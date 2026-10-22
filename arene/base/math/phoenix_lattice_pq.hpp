// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_PHOENIX_LATTICE_PQ_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_PHOENIX_LATTICE_PQ_HPP_

// IWYU pragma: private, include "arene/base/math.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/math/abs.hpp"
#include "arene/base/stdlib_choice/climits.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/is_unsigned.hpp"

namespace arene {
namespace base {

/// @brief Exact golden-ratio harmonic numerator in p/q rational arithmetic (phi ~= 4181 / 2584).
inline constexpr std::int64_t kPhoenixPhiNumPq{4181};

/// @brief Exact golden-ratio harmonic denominator in p/q rational arithmetic (phi ~= 4181 / 2584).
inline constexpr std::int64_t kPhoenixPhiDenPq{2584};

/// @brief 5-state Laser Phoenix Interference Lattice phase coherence classes on Z_{2^32}.
enum class PhoenixInterferenceState : std::uint8_t {
  kDestructive = 0U,          ///< ~180 deg phase opposition (null gain)
  kPartialDestructive = 1U,   ///< ~135 deg partial opposition
  kQuadrature = 2U,           ///< ~90 deg orthogonal quadrature
  kPartialConstructive = 3U,  ///< ~45 deg partial alignment
  kConstructive = 4U          ///< ~0 deg phase lock (full constructive alignment)
};

/// @brief Calculates the exact integer floor square root floor(sqrt(value)) with zero floating-point operations.
/// @tparam UnsignedIntegral An unsigned integral type.
/// @param value Non-negative integer input.
/// @return Floor of the square root of @c value.
template <
    typename UnsignedIntegral,
    constraints<
        std::enable_if_t<std::is_integral<UnsignedIntegral>::value>,
        std::enable_if_t<std::is_unsigned<UnsignedIntegral>::value>> = nullptr>
constexpr auto isqrt_pq(UnsignedIntegral const value) noexcept -> UnsignedIntegral {
  if (value < static_cast<UnsignedIntegral>(2U)) {
    return value;
  }

  constexpr UnsignedIntegral bit_width{
      static_cast<UnsignedIntegral>(sizeof(UnsignedIntegral) * static_cast<UnsignedIntegral>(CHAR_BIT))
  };
  UnsignedIntegral bit{static_cast<UnsignedIntegral>(UnsignedIntegral{1U} << (bit_width - 2U))};
  while (bit > value) {
    bit = static_cast<UnsignedIntegral>(bit >> 2U);
  }

  UnsignedIntegral rem{value};
  UnsignedIntegral res{0U};
  while (bit != UnsignedIntegral{0U}) {
    UnsignedIntegral const cand{static_cast<UnsignedIntegral>(res + bit)};
    if (rem >= cand) {
      rem = static_cast<UnsignedIntegral>(rem - cand);
      res = static_cast<UnsignedIntegral>((res >> 1U) + bit);
    } else {
      res = static_cast<UnsignedIntegral>(res >> 1U);
    }
    bit = static_cast<UnsignedIntegral>(bit >> 2U);
  }
  return res;
}

/// @brief Maps a rational turn fraction (numerator / denominator) to the Z_{2^32} modular phase group.
/// @param numerator Signed turn numerator.
/// @param denominator Positive turn denominator (e.g. 36000 for centi-degrees).
/// @pre @c denominator > 0
/// @return Modular phase in Z_{2^32} where 2^32 == 1 full revolution.
constexpr auto phoenix_phase_from_ratio_z32(std::int64_t const numerator, std::int64_t const denominator) noexcept
    -> std::uint32_t {
  ARENE_PRECONDITION(denominator > 0);
  std::int64_t rem{numerator % denominator};
  if (rem < 0) {
    rem += denominator;
  }
  std::uint64_t const scaled{
      (static_cast<std::uint64_t>(rem) << 32U) + (static_cast<std::uint64_t>(denominator) >> 1U)
  };
  return static_cast<std::uint32_t>(scaled / static_cast<std::uint64_t>(denominator));
}

/// @brief Computes the shortest signed angular residual (target - current) on Z_{2^32}.
/// @param target_z32 Target phase in Z_{2^32}.
/// @param current_z32 Current phase in Z_{2^32}.
/// @return Signed residual in [-2^31, 2^31 - 1].
constexpr auto phoenix_shortest_delta_z32(std::uint32_t const target_z32, std::uint32_t const current_z32) noexcept
    -> std::int32_t {
  return static_cast<std::int32_t>(target_z32 - current_z32);
}

/// @brief Classifies a Z_{2^32} phase residual into the 5-state Laser Phoenix Interference Lattice.
/// @param delta_z32 Signed phase difference in Z_{2^32}.
/// @return Corresponding PhoenixInterferenceState in {0 deg, 45 deg, 90 deg, 135 deg, 180 deg}.
constexpr auto classify_phoenix_lattice_z32(std::int32_t const delta_z32) noexcept -> PhoenixInterferenceState {
  // Fold signed residual to unsigned magnitude in [0, 2^31]
  std::uint32_t const abs_delta{
      (delta_z32 == (-2147483647 - 1)) ? 2147483648U : static_cast<std::uint32_t>(abs(delta_z32))
  };
  // Sector thresholds at 22.5 deg (1/16 turn), 67.5 deg (3/16), 112.5 deg (5/16), 157.5 deg (7/16)
  if (abs_delta < 0x10000000U) {
    return PhoenixInterferenceState::kConstructive;
  }
  if (abs_delta < 0x30000000U) {
    return PhoenixInterferenceState::kPartialConstructive;
  }
  if (abs_delta < 0x50000000U) {
    return PhoenixInterferenceState::kQuadrature;
  }
  if (abs_delta < 0x70000000U) {
    return PhoenixInterferenceState::kPartialDestructive;
  }
  return PhoenixInterferenceState::kDestructive;
}

/// @brief Returns the Q16 coherence gain (65536 == 1.0) for a Laser Phoenix Interference Lattice state.
/// @param state Phoenix lattice state.
/// @return Q16 gain in [0, 65536].
constexpr auto phoenix_lattice_gain_q16(PhoenixInterferenceState const state) noexcept -> std::int64_t {
  switch (state) {
    case PhoenixInterferenceState::kConstructive:
      return 65536;
    case PhoenixInterferenceState::kPartialConstructive:
      return 60547;
    case PhoenixInterferenceState::kQuadrature:
      return 46341;
    case PhoenixInterferenceState::kPartialDestructive:
      return 19220;
    case PhoenixInterferenceState::kDestructive:
    default:
      return 0;
  }
}

/// @brief Evaluates exact non-uniform Bessel-Overhauser p/q Catmull-Rom cubic Hermite interpolation at a missing step.
///
/// Reproduces any linear or quadratic trajectory with zero mathematical error across arbitrary non-uniform anchor gaps.
/// @param y0 Anchor value at t0.
/// @param y1 Anchor value at t1.
/// @param y2 Anchor value at t2.
/// @param y3 Anchor value at t3.
/// @param h0 Positive step interval (t1 - t0).
/// @param h1 Positive dropout gap (t2 - t1).
/// @param h2 Positive step interval (t3 - t2).
/// @param step_num Offset (t - t1) inside the dropout gap [0, h1].
/// @pre h0 > 0 && h1 > 0 && h2 > 0 && step_num >= 0 && step_num <= h1
/// @return Exact integer-rounded cubic Hermite value at t = t1 + step_num.
constexpr auto catmull_rom_bessel_pq(
    std::int64_t const y0,
    std::int64_t const y1,
    std::int64_t const y2,
    std::int64_t const y3,
    std::int64_t const h0,
    std::int64_t const h1,
    std::int64_t const h2,
    std::int64_t const step_num
) noexcept -> std::int64_t {
  ARENE_PRECONDITION((h0 > 0) && (h1 > 0) && (h2 > 0));
  ARENE_PRECONDITION((step_num >= 0) && (step_num <= h1));

  // Bessel-Overhauser parabolic tangents scaled by h1
  std::int64_t const m1_num{(h1 * h1 * (y1 - y0)) + (h0 * h0 * (y2 - y1))};
  std::int64_t const m1_den{h0 * (h0 + h1)};
  std::int64_t const m1{m1_num / m1_den};

  std::int64_t const m2_num{((h2 * h2 * (y2 - y1)) + (h1 * h1 * (y3 - y2))) * h1};
  std::int64_t const m2_den{h2 * h1 * (h1 + h2)};
  std::int64_t const m2{m2_num / m2_den};

  // Virtual uniform control points p0 = y2 - 2*m1, p3 = y1 + 2*m2
  std::int64_t const p0{y2 - (2 * m1)};
  std::int64_t const p1{y1};
  std::int64_t const p2{y2};
  std::int64_t const p3{y1 + (2 * m2)};

  std::int64_t const a0{2 * p1};
  std::int64_t const a1{p2 - p0};
  std::int64_t const a2{(2 * p0) - (5 * p1) + (4 * p2) - p3};
  std::int64_t const a3{(-p0) + (3 * p1) - (3 * p2) + p3};

  std::int64_t const q{h1};
  std::int64_t const q2{q * q};
  std::int64_t const q3{q2 * q};
  std::int64_t const p{step_num};
  std::int64_t const p2_step{p * p};
  std::int64_t const p3_step{p2_step * p};

  std::int64_t const num{(a0 * q3) + (a1 * p * q2) + (a2 * p2_step * q) + (a3 * p3_step)};
  std::int64_t const den{2 * q3};
  if (num >= 0) {
    return (num + (den / 2)) / den;
  }
  return -((-num + (den / 2)) / den);
}

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_PHOENIX_LATTICE_PQ_HPP_
