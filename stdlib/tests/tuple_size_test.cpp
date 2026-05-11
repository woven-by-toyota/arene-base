// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/cstddef"
#include "stdlib/include/tuple"
#include "stdlib/include/type_traits"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"

// NOLINTBEGIN(hicpp-avoid-c-arrays,readability-identifier-length,llvm-qualified-auto)

namespace test {

template <std::size_t N>
auto integral_constant_base(std::integral_constant<std::size_t, N>) -> std::integral_constant<std::size_t, N>;

template <class T>
using integral_constant_base_t = decltype(integral_constant_base(std::declval<T>()));

}  // namespace test

namespace {

template <class T>
struct TupleSize : testing::Test {};

template <std::size_t I, class T>
struct test_case {
  static constexpr auto expected_value = I;
  using tuple_type = T;
};

// clang-format off
using tuple_types = ::testing::Types<
  test_case<0, std::tuple<>>,
  test_case<1, std::tuple<int>>,
  test_case<1, std::tuple<float>>,
  test_case<1, std::tuple<void*>>,
  test_case<3, std::tuple<int, float, void*>>,
  test_case<4, std::tuple<int, int, int, int>>,
  test_case<4, std::tuple<int, const int, volatile int, const volatile int>>,
  test_case<3, std::tuple<int, int&, int const&>>,
  test_case<5, std::tuple<void*, char, int, float, double>>
>;
// clang-format on

TYPED_TEST_SUITE(TupleSize, tuple_types, );

/// @test @c tuple_size<Tuple> has base characteristic integral_constant<size_t, N>
TYPED_TEST(TupleSize, TupleSizeBaseCharacteristic) {
  static_assert(
      testing::is_unambiguously_publicly_derived_from_v<
          test::integral_constant_base_t<std::tuple_size<typename TypeParam::tuple_type>>,
          std::integral_constant<std::size_t, TypeParam::expected_value>>,
      "'tuple_size<T>' must have base characteristic 'integral_constant<size_t, N>'"
  );
}

/// @test @c tuple_size_v<Tuple> defines the same value as @c tuple_size<Tuple>::value
TYPED_TEST(TupleSize, TupleSizeTraitsMatch) {
  static_assert(
      std::tuple_size_v<typename TypeParam::tuple_type> == std::tuple_size<typename TypeParam::tuple_type>::value,
      "'tuple_size_v<>' and 'tuple_size<>::value' must specify the same value"

  );
}

/// @test @c tuple_size<T const> has the same base characteristic as @c tuple_size<T>
TYPED_TEST(TupleSize, ConstQualifiedType) {
  static_assert(
      std::is_same_v<
          test::integral_constant_base_t<std::tuple_size<typename TypeParam::tuple_type const>>,
          test::integral_constant_base_t<std::tuple_size<typename TypeParam::tuple_type>>>,
      "'tuple_size<T const>' must have the same base characteristic as 'tuple_size<T>'"
  );
}

/// @test @c tuple_size<T volatile> has the same base characteristic as @c tuple_size<T>
TYPED_TEST(TupleSize, VolatileQualifiedType) {
  static_assert(
      std::is_same_v<
          test::integral_constant_base_t<std::tuple_size<typename TypeParam::tuple_type volatile>>,
          test::integral_constant_base_t<std::tuple_size<typename TypeParam::tuple_type>>>,
      "'tuple_size<T volatile>' must have the same base characteristic as 'tuple_size<T>'"
  );
}

/// @test @c tuple_size<T const volatile> has the same base characteristic as @c tuple_size<T>
TYPED_TEST(TupleSize, ConstVolatileQualifiedType) {
  static_assert(
      std::is_same_v<
          test::integral_constant_base_t<std::tuple_size<typename TypeParam::tuple_type const volatile>>,
          test::integral_constant_base_t<std::tuple_size<typename TypeParam::tuple_type>>>,
      "'tuple_size<T const volatile>' must have the same base characteristic as 'tuple_size<T>'"
  );
}

/// @brief check if a type is defined/complete
/// @tparam T type to check
///
/// @{

// https://devblogs.microsoft.com/oldnewthing/20190710-00/?p=102678
template <class, class = void>
constexpr bool is_type_complete_v = false;

template <class T>
constexpr bool is_type_complete_v<T, std::void_t<decltype(sizeof(T))>> = true;
/// @}

/// @test @c tuple_size<T&> is undefined
TYPED_TEST(TupleSize, ReferencesUndefined) {
  static_assert(
      !is_type_complete_v<std::tuple_size<typename TypeParam::tuple_type&>>,
      "'tuple_size<T&>' is undefined for references to tuples"
  );
}

}  // namespace

// NOLINTEND(hicpp-avoid-c-arrays,readability-identifier-length,llvm-qualified-auto)
