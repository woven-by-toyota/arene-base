// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/bit/bit_cast.hpp"

#include <gtest/gtest.h>

#include "arene/base/algorithm/all_of.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/is_trivially_copyable.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

ARENE_IGNORE_START();
ARENE_IGNORE_ALL(
    "-Wfloat-equal",
    "Not performing arithmetic operations on floating point values, so exact equality is ok."
);

namespace {
struct wrapped_float;
}  // namespace

// For reference, see:
//   https://en.wikipedia.org/wiki/Single-precision_floating-point_format
//   https://en.wikipedia.org/wiki/Double-precision_floating-point_format

// Tests below require IEC 559 compliance for the bit-pattern round-trips.
static_assert(std::numeric_limits<float>::is_iec559, "float must be IEC 559");
static_assert(std::numeric_limits<double>::is_iec559, "double must be IEC 559");

namespace {

using ::arene::base::bit_cast;
using ::arene::base::is_invocable_v;
using ::arene::base::is_nothrow_invocable_v;
using ::arene::base::to_array;

// Cross-check: ARENE_HAS_CONSTEXPR_BIT_CAST must track __builtin_bit_cast availability.
#if ARENE_HAS_BUILTIN(__builtin_bit_cast)
static_assert(ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST), "must report ON when __builtin_bit_cast is available");
#else
static_assert(!ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST), "must report OFF when __builtin_bit_cast is unavailable");
#endif

/// @brief Assert that every pair in @c data round-trips in both directions.
template <class T, class U, std::size_t N>
constexpr auto check_bit_cast_pairs(::arene::base::array<std::pair<T, U>, N> const& data) -> bool {
  struct check_bit_cast_pair {
    constexpr auto operator()(std::pair<T, U> const& value) {
      if (bit_cast<U>(value.first) != value.second) {
        return false;
      }
      if (value.first != bit_cast<T>(value.second)) {
        return false;
      }
      return true;
    }
  };

  return arene::base::all_of(data.begin(), data.end(), check_bit_cast_pair{});
}

/// @test `bit_cast` is an identity on `std::int32_t`.
CONDITIONALLY_CONSTEXPR_TEST(BitCast, Int32ToInt32, ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)) {
  using I = std::numeric_limits<std::int32_t>;
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays, modernize-avoid-c-arrays) to_array takes a C-array
  std::pair<std::int32_t, std::int32_t> const raw[] = {
      {I::max(), I::max()},
      {I::min(), I::min()},
      {I::lowest(), I::lowest()},
      {1, 1},
      {-1, -1},
      {0, 0},
  };
  CONSTEXPR_ASSERT(check_bit_cast_pairs(to_array(raw)));
}

/// @test `bit_cast` between `std::int32_t` and `float` yields the IEEE 754 single-precision bit pattern.
CONDITIONALLY_CONSTEXPR_TEST(BitCast, Int32ToFloat, ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)) {
  using F = std::numeric_limits<float>;
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays, modernize-avoid-c-arrays) to_array takes a C-array
  std::pair<std::int32_t, float> const raw[] = {
      // clang-format off
      {std::int32_t{0b0'00000000'00000000000000000000001L}, F::denorm_min()},
      {std::int32_t{0b0'00000000'11111111111111111111111L}, F::min() - F::denorm_min()},
      {std::int32_t{0b0'00000001'00000000000000000000000L}, F::min()},
      {std::int32_t{0b0'11111110'11111111111111111111111L}, F::max()},
      {std::int32_t{0b0'01111111'00000000000000000000000L}, float{1}},
      {std::int32_t{0b0'01111111'00000000000000000000001L}, float{1} + F::epsilon()},
      {std::int32_t{0b0'00000000'00000000000000000000000L}, float{}},
      {std::int32_t{0b0'10000000'00000000000000000000000L}, float{2}},
      {std::int32_t{0b0'11111111'00000000000000000000000L}, F::infinity()},
      // clang-format on
  };
  CONSTEXPR_ASSERT(check_bit_cast_pairs(to_array(raw)));
}

/// @test `bit_cast` between `std::int64_t` and `double` yields the IEEE 754 double-precision bit pattern.
CONDITIONALLY_CONSTEXPR_TEST(BitCast, Int64ToDouble, ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)) {
  using D = std::numeric_limits<double>;
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays, modernize-avoid-c-arrays) to_array takes a C-array
  std::pair<std::int64_t, double> const raw[] = {
      // clang-format off
      {std::int64_t{0b0'00000000000'0000000000000000000000000000000000000000000000000001LL}, D::denorm_min()},
      {std::int64_t{0b0'00000000000'1111111111111111111111111111111111111111111111111111LL}, D::min() - D::denorm_min()},
      {std::int64_t{0b0'00000000001'0000000000000000000000000000000000000000000000000000LL}, D::min()},
      {std::int64_t{0b0'11111111110'1111111111111111111111111111111111111111111111111111LL}, D::max()},
      {std::int64_t{0b0'01111111111'0000000000000000000000000000000000000000000000000000LL}, double{1}},
      {std::int64_t{0b0'01111111111'0000000000000000000000000000000000000000000000000001LL}, double{1} + D::epsilon()},
      {std::int64_t{0b0'00000000000'0000000000000000000000000000000000000000000000000000LL}, double{}},
      {std::int64_t{0b0'10000000000'0000000000000000000000000000000000000000000000000000LL}, double{2}},
      {std::int64_t{0b0'11111111111'0000000000000000000000000000000000000000000000000000LL}, D::infinity()},
      // clang-format on
  };
  CONSTEXPR_ASSERT(check_bit_cast_pairs(to_array(raw)));
}

/// @brief A user-defined trivially copyable wrapper around `float`.
struct wrapped_float {
  float value{};

  wrapped_float() = default;
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions) Intentional implicit conversion for test data
  constexpr wrapped_float(float const value_in) noexcept
      : value{value_in} {}

  friend constexpr auto operator!=(wrapped_float const lhs, wrapped_float const rhs) noexcept -> bool {
    return lhs.value != rhs.value;
  }
};

static_assert(
    std::is_trivially_copyable<wrapped_float>::value,
    "`wrapped_float` must be trivially copyable to be a valid `bit_cast` source or destination"
);

/// @test `bit_cast` can convert between a user-defined trivially copyable wrapper and its underlying type.
CONDITIONALLY_CONSTEXPR_TEST(BitCast, FloatToCustomType, ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)) {
  using F = std::numeric_limits<float>;
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays, modernize-avoid-c-arrays) to_array takes a C-array
  std::pair<wrapped_float, float> const raw[] = {
      // clang-format off
      {wrapped_float{F::denorm_min()},            F::denorm_min()},
      {wrapped_float{F::min() - F::denorm_min()}, F::min() - F::denorm_min()},
      {wrapped_float{F::min()},                   F::min()},
      {wrapped_float{F::max()},                   F::max()},
      {wrapped_float{float{1}},                   float{1}},
      {wrapped_float{float{1} + F::epsilon()},    float{1} + F::epsilon()},
      {wrapped_float{float{}},                    float{}},
      {wrapped_float{float{2}},                   float{2}},
      {wrapped_float{F::infinity()},              F::infinity()},
      // clang-format on
  };
  CONSTEXPR_ASSERT(check_bit_cast_pairs(to_array(raw)));
}

/// @test `bit_cast<To>` is invocable when `sizeof(To) == sizeof(From)` and both types are trivially copyable.
TEST(BitCast, IsInvocableWithMatchingTriviallyCopyableTypes) {
  static_assert(is_invocable_v<decltype(bit_cast<std::int32_t>), std::int32_t>, "must be invocable: int32 <- int32");
  static_assert(is_invocable_v<decltype(bit_cast<std::int32_t>), float>, "must be invocable: int32 <- float");
  static_assert(is_invocable_v<decltype(bit_cast<std::int64_t>), double>, "must be invocable: int64 <- double");
  static_assert(is_invocable_v<decltype(bit_cast<float>), std::int32_t>, "must be invocable: float <- int32");
  static_assert(is_invocable_v<decltype(bit_cast<double>), std::int64_t>, "must be invocable: double <- int64");
  static_assert(is_invocable_v<decltype(bit_cast<wrapped_float>), float>, "must be invocable: wrapped_float <- float");
  static_assert(is_invocable_v<decltype(bit_cast<float>), wrapped_float>, "must be invocable: float <- wrapped_float");
}

/// @test `bit_cast<To>` is not invocable when `sizeof(To) != sizeof(From)`.
TEST(BitCast, NotInvocableWithMismatchedSizes) {
  static_assert(!is_invocable_v<decltype(bit_cast<std::int16_t>), std::int8_t>, "must reject int16 <- int8");
  static_assert(!is_invocable_v<decltype(bit_cast<std::int8_t>), std::int16_t>, "must reject int8 <- int16");
  static_assert(!is_invocable_v<decltype(bit_cast<float>), double>, "must reject float <- double");
  static_assert(!is_invocable_v<decltype(bit_cast<double>), float>, "must reject double <- float");
}

// The following non-trivially-copyable test types intentionally omit some of the five special member functions.
// NOLINTBEGIN(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)

/// @brief A type whose eligible special member functions are all deleted or user-provided; not trivially copyable.
struct no_eligible_special_memb_func {
  int value{};

  no_eligible_special_memb_func() = default;
  no_eligible_special_memb_func(no_eligible_special_memb_func const&);
  auto operator=(no_eligible_special_memb_func const&) -> no_eligible_special_memb_func& = delete;
};

/// @brief A type with a user-provided copy constructor; not trivially copyable.
struct non_trivial_copy_ctor {
  int value{};

  non_trivial_copy_ctor() = default;
  non_trivial_copy_ctor(non_trivial_copy_ctor const&);
  auto operator=(non_trivial_copy_ctor const&) -> non_trivial_copy_ctor& = delete;
};

/// @brief A type with a user-provided destructor; not trivially copyable.
struct non_trivial_dtor {
  int value{};

  ~non_trivial_dtor();
};

// NOLINTEND(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)

static_assert(sizeof(int) == sizeof(no_eligible_special_memb_func), "same-sized");
static_assert(sizeof(int) == sizeof(non_trivial_copy_ctor), "same-sized");
static_assert(sizeof(int) == sizeof(non_trivial_dtor), "same-sized");

static_assert(
    !std::is_trivially_copyable<no_eligible_special_memb_func>::value,
    "`no_eligible_special_memb_func` is intended to not be trivially copyable"
);
static_assert(
    !std::is_trivially_copyable<non_trivial_copy_ctor>::value,
    "`non_trivial_copy_ctor` is intended to not be trivially copyable"
);
static_assert(
    !std::is_trivially_copyable<non_trivial_dtor>::value,
    "`non_trivial_dtor` is intended to not be trivially copyable"
);

/// @test `bit_cast<To>` is not invocable when the destination type is not trivially copyable.
TEST(BitCast, NotInvocableWithNonTriviallyCopyableDestination) {
  static_assert(!is_invocable_v<decltype(bit_cast<no_eligible_special_memb_func>), int>, "must reject no-eligible-smf");
  static_assert(!is_invocable_v<decltype(bit_cast<non_trivial_copy_ctor>), int>, "must reject non-trivial copy ctor");
  static_assert(!is_invocable_v<decltype(bit_cast<non_trivial_dtor>), int>, "must reject non-trivial dtor");
}

/// @test `bit_cast<To>` is not invocable when the source type is not trivially copyable.
TEST(BitCast, NotInvocableWithNonTriviallyCopyableSource) {
  static_assert(!is_invocable_v<decltype(bit_cast<int>), no_eligible_special_memb_func>, "must reject no-eligible-smf");
  static_assert(!is_invocable_v<decltype(bit_cast<int>), non_trivial_copy_ctor>, "must reject non-trivial copy ctor");
  static_assert(!is_invocable_v<decltype(bit_cast<int>), non_trivial_dtor>, "must reject non-trivial dtor");
}

/// @test `bit_cast` is declared `noexcept` for all accepted type combinations.
TEST(BitCast, IsNoexcept) {
  static_assert(is_nothrow_invocable_v<decltype(bit_cast<float>), float>, "float <- float must be noexcept");
  static_assert(is_nothrow_invocable_v<decltype(bit_cast<float>), std::int32_t>, "float <- int32 must be noexcept");
  static_assert(is_nothrow_invocable_v<decltype(bit_cast<std::int32_t>), float>, "int32 <- float must be noexcept");
  static_assert(
      is_nothrow_invocable_v<decltype(bit_cast<float>), wrapped_float>,
      "float <- wrapped_float must be noexcept"
  );
  static_assert(
      is_nothrow_invocable_v<decltype(bit_cast<wrapped_float>), float>,
      "wrapped_float <- float must be noexcept"
  );
}

ARENE_IGNORE_END();

}  // namespace
