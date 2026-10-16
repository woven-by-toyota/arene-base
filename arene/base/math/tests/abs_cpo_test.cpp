// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/math/abs_cpo.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/math/abs.hpp"
#include "arene/base/stdlib_choice/is_signed.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/bitwise_uniform_distribution.hpp"
#include "arene/base/testing/nan_rejecting_distribution.hpp"
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_macros.hpp"
#include "arene/base/type_list/concat.hpp"
#include "testlibs/utilities/common_test_types.hpp"

// Since no arithmetic is performed, float/double values are checked for exact equality, so NaN draws must be rejected.
/// @cond
template <>
constexpr auto arene::base::testing::default_distribution<float> =
    arene::base::testing::nan_rejecting_distribution<arene::base::testing::bitwise_uniform_distribution<float>>{};
template <>
constexpr auto arene::base::testing::default_distribution<double> =
    arene::base::testing::nan_rejecting_distribution<arene::base::testing::bitwise_uniform_distribution<double>>{};
/// @endcond

namespace {

namespace math = ::arene::base::math;

template <typename T>
struct has_member_abs {
  T value;

  constexpr auto abs() const noexcept -> has_member_abs { return {arene::base::abs(value)}; }
};

template <typename T>
struct has_adl_abs {
  T value;

  friend constexpr auto abs(has_adl_abs const& wrapper) noexcept -> has_adl_abs {
    return {arene::base::abs(wrapper.value)};
  }
};

template <typename T>
struct has_member_and_adl_abs {
  T value;

  constexpr auto abs() const noexcept -> has_member_and_adl_abs { return {arene::base::abs(value)}; }

  friend constexpr auto abs(has_member_and_adl_abs const&) noexcept -> has_member_and_adl_abs {
    ARENE_INVARIANT_UNREACHABLE("This overload should never be called");
  }
};

template <typename T>
class AbsTypedTest : public testing::Test {};

using arithmetic_types = arene::base::type_lists::concat_t<  //
    testing::signed_integer_types,
    testing::unsigned_integer_types,
    testing::floating_point_types>;

TYPED_TEST_SUITE(AbsTypedTest, arithmetic_types, );

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "No arithmetic is performed so exact equality is OK even for floating point");

/// @test `math::abs` returns the same thing as `abs` when called on a fundamental arithmetic type
ARENE_TYPED_PROPERTY(AbsTypedTest, OnArithmeticType, (TypeParam value)) {
  static_assert(!math::abs_detail::has_member_abs_v<TypeParam>, "Raw value should not have member or ADL abs");
  static_assert(!math::abs_detail::has_adl_abs_v<TypeParam>, "Raw value should not have member or ADL abs");

  // For signed integral types, the absolute value of the lowest value can not be taken, so we skip this value.
  ARENE_PROP_PRE(!std::is_signed<TypeParam>::value || value != std::numeric_limits<TypeParam>::lowest());

  ARENE_PROP_ASSERT(math::abs(value) == ::arene::base::abs(value));
}

/// @test `math::abs` finds member abs and returns it
ARENE_TYPED_PROPERTY(AbsTypedTest, OnTypeWithMemberAbs, (TypeParam value)) {
  static_assert(math::abs_detail::has_member_abs_v<has_member_abs<TypeParam>>, "Should have member abs only");
  static_assert(!math::abs_detail::has_adl_abs_v<has_member_abs<TypeParam>>, "Should have member abs only");

  // For signed integral types, the absolute value of the lowest value can not be taken, so we skip this value.
  ARENE_PROP_PRE(!std::is_signed<TypeParam>::value || value != std::numeric_limits<TypeParam>::lowest());

  has_member_abs<TypeParam> const wrapped{value};
  ARENE_PROP_ASSERT(math::abs(wrapped).value == ::arene::base::abs(wrapped.value));
}

/// @test `math::abs` finds ADL abs and returns it
ARENE_TYPED_PROPERTY(AbsTypedTest, OnTypeWithAdlAbs, (TypeParam value)) {
  static_assert(!math::abs_detail::has_member_abs_v<has_adl_abs<TypeParam>>, "Should have ADL abs only");
  static_assert(math::abs_detail::has_adl_abs_v<has_adl_abs<TypeParam>>, "Should have ADL abs only");

  // For signed integral types, the absolute value of the lowest value can not be taken, so we skip this value.
  ARENE_PROP_PRE(!std::is_signed<TypeParam>::value || value != std::numeric_limits<TypeParam>::lowest());

  has_adl_abs<TypeParam> const wrapped{value};
  ARENE_PROP_ASSERT(math::abs(wrapped).value == ::arene::base::abs(wrapped.value));
}

/// @test `math::abs` prefers member abs over ADL abs
ARENE_TYPED_PROPERTY(AbsTypedTest, OnTypeWithMemberAndAdlAbs, (TypeParam value)) {
  static_assert(math::abs_detail::has_member_abs_v<has_member_and_adl_abs<TypeParam>>, "Should have member & ADL abs");
  static_assert(math::abs_detail::has_adl_abs_v<has_member_and_adl_abs<TypeParam>>, "Should have member & ADL abs");

  // For signed integral types, the absolute value of the lowest value can not be taken, so we skip this value.
  ARENE_PROP_PRE(!std::is_signed<TypeParam>::value || value != std::numeric_limits<TypeParam>::lowest());

  // This would crash if the ADL abs were selected, so the test finishing means that member abs was used.
  has_member_and_adl_abs<TypeParam> const wrapped{value};
  ARENE_PROP_ASSERT(math::abs(wrapped).value == ::arene::base::abs(wrapped.value));
}

/// @test All of the `math::abs` overloads work in constant expressions
TYPED_TEST(AbsTypedTest, WorksInConstexpr) {
  using mem_abs = has_member_abs<TypeParam>;
  using adl_abs = has_adl_abs<TypeParam>;

  static_assert(math::abs(TypeParam{}) == TypeParam{}, "Arithmetic type overload works in constexpr");
  static_assert(math::abs(mem_abs{}).value == TypeParam{}, "Member function overload works in constexpr");
  static_assert(math::abs(adl_abs{}).value == TypeParam{}, "ADL function overload works in constexpr");
}

ARENE_IGNORE_END();

// These are defined as distinct types from has_member_abs and has_adl_abs so that they can have non-constexpr abs
// functions, which is required to get GCC to evaluate the noexcept operators in the test case below as we expect.

template <typename T, bool IsNoexcept>
struct has_member_abs_with_noexcept {
  T value;

  auto abs() const noexcept(IsNoexcept) -> has_member_abs_with_noexcept { return {arene::base::abs(value)}; }
};

template <typename T, bool IsNoexcept>
struct has_adl_abs_with_noexcept {
  T value;

  friend auto abs(has_adl_abs_with_noexcept const& wrapper) noexcept(IsNoexcept) -> has_adl_abs_with_noexcept {
    return {arene::base::abs(wrapper.value)};
  }
};

/// @test All of the `math::abs` overloads pass through the noexcept-specifications of their underlying functions
TYPED_TEST(AbsTypedTest, NoexceptPassesThrough) {
  static_assert(noexcept(math::abs(TypeParam{})), "Always noexcept on arithmetic types");

  static_assert(
      noexcept(math::abs(has_member_abs_with_noexcept<TypeParam, true>{})),
      "noexcept(true) is inherited from member function"
  );
  static_assert(
      !noexcept(math::abs(has_member_abs_with_noexcept<TypeParam, false>{})),
      "noexcept(false) is inherited from member function"
  );

  static_assert(
      noexcept(math::abs(has_adl_abs_with_noexcept<TypeParam, true>{})),
      "noexcept(true) is inherited from ADL function"
  );
  static_assert(
      !noexcept(math::abs(has_adl_abs_with_noexcept<TypeParam, false>{})),
      "noexcept(false) is inherited from ADL function"
  );
}

}  // namespace
