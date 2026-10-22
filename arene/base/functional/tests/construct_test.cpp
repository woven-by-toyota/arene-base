// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/functional/construct.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "testlibs/utilities/configurable_value.hpp"
#include "testlibs/utilities/counting_wrapper.hpp"

namespace {

using arene::base::construct;

using counted_ival = ::testing::counting_wrapper<std::int32_t>;
using counted_dval = ::testing::counting_wrapper<double>;
using nothrow_single_arg = ::testing::configurable_value<std::int32_t>;
using throwing_single_arg = ::testing::configurable_value<std::int32_t, ::testing::throws_on::value_construct>;

/// @brief A user-defined type with a conditionally noexcept multi-argument constructor.
/// @tparam ThrowsOnValueConstruct A specification for whether the constructor is marked as @c noexcept(false).
template <bool ThrowsOnValueConstruct>
struct throwable_multi_arg {
  std::int32_t int_val_;
  double dbl_val_;
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  constexpr throwable_multi_arg(std::int32_t int_val, double dbl_val) noexcept(!ThrowsOnValueConstruct)
      : int_val_(int_val),
        dbl_val_(dbl_val) {}
};
/// @brief A user-defined type with a noexcept multi-argument constructor.
using nothrow_multi_arg = throwable_multi_arg<false>;
/// @brief A user-defined type with a multi-argument constructor that is not noexcept.
using throwing_multi_arg = throwable_multi_arg<true>;

/// @brief A type that is only constructible from itself, not from unrelated types such as @c int.
struct not_constructible_from_int {
  not_constructible_from_int() = default;
  ~not_constructible_from_int() = default;
  not_constructible_from_int(not_constructible_from_int const&) = default;
  not_constructible_from_int(not_constructible_from_int&&) = default;
  auto operator=(not_constructible_from_int const&) -> not_constructible_from_int& = default;
  auto operator=(not_constructible_from_int&&) -> not_constructible_from_int& = default;
};

template <typename T>
class ConstructPrimitiveTest : public ::testing::Test {};

using primitive_types = ::testing::Types<
    std::int8_t,
    std::int16_t,
    std::int32_t,
    std::int64_t,
    std::uint8_t,
    std::uint16_t,
    std::uint32_t,
    std::uint64_t,
    float,
    double>;

TYPED_TEST_SUITE(ConstructPrimitiveTest, primitive_types, );

/// @test Given @c construct<To> for a primitive type, when called with a value argument, then it returns a copy of
/// that value.
CONSTEXPR_TYPED_TEST(ConstructPrimitiveTest, ConstructsFromValue) {
  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Testing for unmodified exact copy of constructed value");
  CONSTEXPR_ASSERT_EQ(construct<TypeParam>(TypeParam{1}), TypeParam{1});
  ARENE_IGNORE_END();
}

/// @test Given @c construct<To> for a primitive type, when called with no arguments, then it returns a
/// zero-initialized value.
CONSTEXPR_TYPED_TEST(ConstructPrimitiveTest, DefaultConstructsToZero) {
  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Testing for exact zero from default construction");
  CONSTEXPR_ASSERT_EQ(construct<TypeParam>(), TypeParam{});
  ARENE_IGNORE_END();
}

/// @test Given @c construct<To> for a primitive type, then the call operator is always @c noexcept.
CONSTEXPR_TYPED_TEST(ConstructPrimitiveTest, IsNoexcept) {
  CONSTEXPR_ASSERT(noexcept(construct<TypeParam>(TypeParam{1})));
  CONSTEXPR_ASSERT(noexcept(construct<TypeParam>()));
}

/// @test Given @c construct<To> with a user-defined type and a single compatible argument, then the returned object is
/// correctly constructed.
CONSTEXPR_TEST(ConstructValueTest, ConstructsUserDefinedTypeFromSingleArg) {
  auto const result = construct<nothrow_single_arg>(std::int32_t{42});
  CONSTEXPR_ASSERT_EQ(result.value, std::int32_t{42});
}

/// @test Given @c construct<To> with a user-defined type and multiple compatible arguments, then the returned object
/// is correctly constructed.
CONSTEXPR_TEST(ConstructValueTest, ConstructsUserDefinedTypeFromMultipleArgs) {
  auto const result = construct<nothrow_multi_arg>(std::int32_t{1}, 2.5);
  CONSTEXPR_ASSERT_EQ(result.int_val_, std::int32_t{1});
  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "2.5 is exactly representable; construction does not alter the value");
  CONSTEXPR_ASSERT_EQ(result.dbl_val_, 2.5);
  ARENE_IGNORE_END();
}

/// @test Given @c construct<To> where @c To is nothrow-constructible from the given arguments, then the call operator
/// is @c noexcept.
CONSTEXPR_TEST(ConstructNoexceptTest, IsNoexceptWhenConstructionIsNoexcept) {
  CONSTEXPR_ASSERT((arene::base::is_nothrow_invocable_v<decltype(construct<nothrow_single_arg>), std::int32_t>));
  CONSTEXPR_ASSERT((arene::base::is_nothrow_invocable_v<decltype(construct<nothrow_multi_arg>), std::int32_t, double>));
}

/// @test Given @c construct<To> where @c To's constructor from the given arguments is not @c noexcept, then the call
/// operator is also not @c noexcept.
CONSTEXPR_TEST(ConstructNoexceptTest, IsNotNoexceptWhenConstructionIsNotNoexcept) {
  CONSTEXPR_ASSERT_FALSE((arene::base::is_nothrow_invocable_v<decltype(construct<throwing_single_arg>), std::int32_t>));
  CONSTEXPR_ASSERT_FALSE(
      (arene::base::is_nothrow_invocable_v<decltype(construct<throwing_multi_arg>), std::int32_t, double>)
  );
}

/// @test Given @c construct<To>, when @c To is a user-defined type constructible from the given argument types, then
/// the call operator participates in overload resolution.
CONSTEXPR_TEST(ConstructConstraintTest, IsInvocableWhenConstructible) {
  CONSTEXPR_ASSERT((arene::base::is_invocable_v<decltype(construct<nothrow_single_arg>), std::int32_t>));
  CONSTEXPR_ASSERT((arene::base::is_invocable_v<decltype(construct<nothrow_multi_arg>), std::int32_t, double>));
}

/// @test Given @c construct<To>, when @c To is not constructible from the given argument types, then the call
/// operator does not participate in overload resolution.
CONSTEXPR_TEST(ConstructConstraintTest, IsNotInvocableWhenNotConstructible) {
  CONSTEXPR_ASSERT_FALSE((arene::base::is_invocable_v<decltype(construct<not_constructible_from_int>), std::int32_t>));
  CONSTEXPR_ASSERT_FALSE((arene::base::is_invocable_v<decltype(construct<std::int32_t>), not_constructible_from_int>));
  CONSTEXPR_ASSERT_FALSE(
      (arene::base::is_invocable_v<decltype(construct<nothrow_single_arg>), not_constructible_from_int>)
  );
  CONSTEXPR_ASSERT_FALSE((arene::base::is_invocable_v<decltype(construct<std::int32_t>), std::int32_t, std::int32_t>));
}

struct ForwardingUdt {
  counted_ival ival_;
  counted_dval dval_;

  template <typename CI, typename CD>
  ForwardingUdt(CI&& ival, CD&& dval) noexcept
      : ival_(std::forward<CI>(ival)),
        dval_(std::forward<CD>(dval)) {}
};

class ConstructForwardingTest : public ::testing::Test {
 protected:
  std::int32_t const ival_value_{42};
  double const dval_value_{3.14};
};

/// @test Given @c construct<To>, when called with an lvalue argument, then the argument is copied, not moved.
TEST_F(ConstructForwardingTest, ForwardsLvalueAsCopy) {
  counted_ival::count = {};
  counted_dval::count = {};
  auto ival = counted_ival{ival_value_};
  auto const dval = counted_dval{dval_value_};
  auto const result = construct<ForwardingUdt>(ival, dval);
  EXPECT_EQ(counted_ival::count.copy_ctor, 1U);
  EXPECT_EQ(counted_ival::count.move_ctor, 0U);
  EXPECT_EQ(counted_dval::count.copy_ctor, 1U);
  EXPECT_EQ(counted_dval::count.move_ctor, 0U);
}

/// @test Given @c construct<To>, when called with an rvalue argument, then the argument is moved, not copied.
TEST_F(ConstructForwardingTest, ForwardsRvalueAsMove) {
  counted_ival::count = {};
  counted_dval::count = {};
  auto ival = counted_ival{ival_value_};
  auto const dval = counted_dval{dval_value_};
  // NOLINTNEXTLINE(hicpp-move-const-arg) Intentionally moving a const object to validate that the ref qual is maintained.
  auto const result = construct<ForwardingUdt>(std::move(ival), std::move(dval));
  EXPECT_EQ(counted_ival::count.copy_ctor, 0U);
  EXPECT_EQ(counted_ival::count.move_ctor, 1U);
  EXPECT_EQ(counted_dval::count.copy_ctor, 1U);
  EXPECT_EQ(counted_dval::count.move_ctor, 0U);
}

}  // namespace
