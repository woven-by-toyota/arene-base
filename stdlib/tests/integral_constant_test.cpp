// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/cstdint"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"

namespace {

template <typename T, T Val>
struct type_value_pair {
  using type = T;
  static constexpr T value = Val;
};

using integral_constant_test_types = ::testing::Types<
    type_value_pair<bool, true>,                                          //
    type_value_pair<bool, false>,                                         //
    type_value_pair<char, 'a'>,                                           //
    type_value_pair<char, '0'>,                                           //
    type_value_pair<char, ':'>,                                           //
    type_value_pair<std::int16_t, -12345>,                                //
    type_value_pair<std::int16_t, 32767>,                                 //
    type_value_pair<std::uint64_t, 0>,                                    //
    type_value_pair<std::uint64_t, 8'412'734'273'779'511'625ULL>,         //
    type_value_pair<std::uint64_t, 18'446'744'073'709'551'615ULL>,        //
    type_value_pair<std::int64_t, (-9'223'372'036'854'775'807LL - 1LL)>,  //
    type_value_pair<std::int64_t, 9'223'372'036'854'775'807LL>>;

template <typename T>
class IntegralConstantTest : public testing::Test {};

TYPED_TEST_SUITE(IntegralConstantTest, integral_constant_test_types, );

/// @test  @c integral_constant has a @c type member type alias
TYPED_TEST(IntegralConstantTest, HasTypeMemberTypeAlias) {
  using Type = typename TypeParam::type;
  constexpr Type value = TypeParam::value;
  ::testing::
      StaticAssertTypeEq<typename std::integral_constant<Type, value>::type, std::integral_constant<Type, value>>();
}

/// @test  @c integral_constant has a @c value_type member type alias
TYPED_TEST(IntegralConstantTest, HasValueTypeMemberTypeAlias) {
  using Type = typename TypeParam::type;
  constexpr Type value = TypeParam::value;
  ::testing::StaticAssertTypeEq<typename std::integral_constant<Type, value>::value_type, Type>();
}

/// @test  @c integral_constant is convertible to the value type
TYPED_TEST(IntegralConstantTest, IsConvertibleToValueType) {
  using Type = typename TypeParam::type;
  constexpr Type value = TypeParam::value;
  static_assert(
      std::is_convertible_v<std::integral_constant<Type, value>, Type>,
      "Must be convertible to underlying type"
  );
}

/// @test  @c integral_constant has the right value when converted to the value type
TYPED_TEST(IntegralConstantTest, HasRightValueWhenConverted) {
  using Type = typename TypeParam::type;
  constexpr Type value = TypeParam::value;
  constexpr std::integral_constant<Type, value> constant{};
  static_assert(static_cast<Type>(constant) == value, "Conversion to type must yield correct value");
  ASSERT_EQ(static_cast<Type>(constant), value);
}

/// @test  @c integral_constant can be invoked with no args, yielding the value type
TYPED_TEST(IntegralConstantTest, CanBeInvokedWithNoArgs) {
  using Type = typename TypeParam::type;
  constexpr Type value = TypeParam::value;
  constexpr std::integral_constant<Type, value> constant{};
  ::testing::StaticAssertTypeEq<decltype(constant()), Type>();
}

/// @test Invoking @c integral_constant with no arguments returns the value
TYPED_TEST(IntegralConstantTest, InvokingWithNoArgsYieldsValue) {
  using Type = typename TypeParam::type;
  constexpr Type value = TypeParam::value;
  constexpr std::integral_constant<Type, value> constant{};
  static_assert(constant() == value, "Invocation must yield correct value");
  ASSERT_EQ(constant(), value);
}

/// @test Invoking @c integral_constant is @c noexcept
TYPED_TEST(IntegralConstantTest, InvokingIsNoexcept) {
  using Type = typename TypeParam::type;
  constexpr Type value = TypeParam::value;
  constexpr std::integral_constant<Type, value> constant{};
  static_assert(noexcept(constant()), "Invocation must be noexcept");
}

/// @test Converting @c integral_constant to the value type is @c noexcept
TYPED_TEST(IntegralConstantTest, ConversionIsNoexcept) {
  using Type = typename TypeParam::type;
  constexpr Type value = TypeParam::value;
  constexpr std::integral_constant<Type, value> constant{};
  static_assert(noexcept(static_cast<Type>(constant)), "Conversion must be noexcept");
}

/// @test @c bool_constant<value> is an alias for @c integral_constant<bool,value>
TEST(IntegralConstant, BoolConstantIsAlias) {
  ::testing::StaticAssertTypeEq<std::bool_constant<true>, std::integral_constant<bool, true>>();
  ::testing::StaticAssertTypeEq<std::bool_constant<false>, std::integral_constant<bool, false>>();
}

/// @test @c true_type is an alias for @c bool_constant<true>
TEST(IntegralConstant, TrueTypeIsAnAlias) { ::testing::StaticAssertTypeEq<std::bool_constant<true>, std::true_type>(); }

/// @test @c false_type is an alias for @c bool_constant<false>
TEST(IntegralConstant, FalseTypeIsAnAlias) {
  ::testing::StaticAssertTypeEq<std::bool_constant<false>, std::false_type>();
}

extern int32_t const i32{0};

class some_user_type {};

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
some_user_type sut{};

/// @test @c integral_constant can contain a pointer to integer
CONSTEXPR_TEST(IntegralConstant, WorksForPointerToInteger) {
  using pointer = std::int32_t const*;
  using int_const_t = std::integral_constant<pointer, &i32>;
  ::testing::StaticAssertTypeEq<typename int_const_t::type, int_const_t>();
  ::testing::StaticAssertTypeEq<typename int_const_t::value_type, pointer>();
  static_assert(std::is_convertible_v<int_const_t, pointer>, "Must be convertible to underlying type");
  constexpr int_const_t constant{};
  ASSERT_EQ(static_cast<pointer>(constant), &i32);
  ::testing::StaticAssertTypeEq<decltype(constant()), pointer>();
  ASSERT_EQ(constant(), &i32);
  static_assert(noexcept(constant()), "Invocation must be noexcept");
  static_assert(noexcept(static_cast<pointer>(constant)), "Conversion must be noexcept");
}

/// @test @c integral_constant can contain a pointer to user specified type
CONSTEXPR_TEST(IntegralConstant, WorksForPointerToUserType) {
  using pointer = some_user_type*;
  using int_const_t = std::integral_constant<pointer, &sut>;
  ::testing::StaticAssertTypeEq<typename int_const_t::type, int_const_t>();
  ::testing::StaticAssertTypeEq<typename int_const_t::value_type, pointer>();
  static_assert(std::is_convertible_v<int_const_t, pointer>, "Must be convertible to underlying type");
  constexpr int_const_t constant{};
  ASSERT_EQ(static_cast<pointer>(constant), &sut);
  ::testing::StaticAssertTypeEq<decltype(constant()), pointer>();
  ASSERT_EQ(constant(), &sut);
  static_assert(noexcept(constant()), "Invocation must be noexcept");
  static_assert(noexcept(static_cast<pointer>(constant)), "Conversion must be noexcept");
}

/// @test @c integral_constant can contain a reference to integer
CONSTEXPR_TEST(IntegralConstant, WorksForReferenceToInteger) {
  using reference = std::int32_t const&;
  using int_const_t = std::integral_constant<reference, i32>;
  ::testing::StaticAssertTypeEq<typename int_const_t::type, int_const_t>();
  ::testing::StaticAssertTypeEq<typename int_const_t::value_type, reference>();
  static_assert(std::is_convertible_v<int_const_t, reference>, "Must be convertible to underlying type");
  constexpr int_const_t constant{};
  ASSERT_EQ(&static_cast<reference>(constant), &i32);
  ::testing::StaticAssertTypeEq<decltype(constant()), reference>();
  ASSERT_EQ(&constant(), &i32);
  static_assert(noexcept(constant()), "Invocation must be noexcept");
  static_assert(noexcept(static_cast<reference>(constant)), "Conversion must be noexcept");
}

/// @test @c integral_constant can contain a reference to user defined type
CONSTEXPR_TEST(IntegralConstant, WorksForReferenceToUserType) {
  using reference = some_user_type&;
  using int_const_t = std::integral_constant<reference, sut>;
  ::testing::StaticAssertTypeEq<typename int_const_t::type, int_const_t>();
  ::testing::StaticAssertTypeEq<typename int_const_t::value_type, reference>();
  static_assert(std::is_convertible_v<int_const_t, reference>, "Must be convertible to underlying type");
  constexpr int_const_t constant{};
  ASSERT_EQ(&static_cast<reference>(constant), &sut);
  ::testing::StaticAssertTypeEq<decltype(constant()), reference>();
  ASSERT_EQ(&constant(), &sut);
  static_assert(noexcept(constant()), "Invocation must be noexcept");
  static_assert(noexcept(static_cast<reference>(constant)), "Conversion must be noexcept");
}

}  // namespace
