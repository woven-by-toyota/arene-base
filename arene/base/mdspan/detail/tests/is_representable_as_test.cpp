// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/detail/is_representable_as.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/type_identity.hpp"

namespace {

using arene::base::type_identity;
using arene::base::mdspan_detail::is_representable_as;

template <class T>
class MdspanDetailIsRepresentableAsTest : public testing::Test {};

using TestTypes = ::testing::Types<  //
    std::int32_t,
    std::uint32_t>;

template <class T>
struct larger_type {};
template <>
struct larger_type<std::int32_t> : type_identity<std::int64_t> {};
template <>
struct larger_type<std::uint32_t> : type_identity<std::uint64_t> {};
template <class T>
using larger_type_t = typename larger_type<T>::type;

template <class T>
struct smaller_type {};
template <>
struct smaller_type<std::int32_t> : type_identity<std::int16_t> {};
template <>
struct smaller_type<std::uint32_t> : type_identity<std::uint16_t> {};
template <class T>
using smaller_type_t = typename smaller_type<T>::type;

TYPED_TEST_SUITE(MdspanDetailIsRepresentableAsTest, TestTypes, );

/// @test @c is_representable_as with a smaller integral type
CONSTEXPR_TYPED_TEST(MdspanDetailIsRepresentableAsTest, WithSmaller) {
  auto const small_value = TypeParam{42};
  CONSTEXPR_ASSERT(is_representable_as<smaller_type_t<TypeParam>>(small_value));

  auto const big_value = TypeParam{std::numeric_limits<TypeParam>::max()};
  CONSTEXPR_ASSERT(!is_representable_as<smaller_type_t<TypeParam>>(big_value));
}

/// @test @c is_representable_as with a larger integral type
CONSTEXPR_TYPED_TEST(MdspanDetailIsRepresentableAsTest, WithLarger) {
  auto const value = TypeParam{42};
  CONSTEXPR_ASSERT(is_representable_as<larger_type_t<TypeParam>>(value));
}

template <class T>
struct user_defined_type {
  T value;
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr operator T() const noexcept { return static_cast<T>(value); }
};

/// @test @c is_representable_as can cast a user defined type with a conversion operator to a smaller integral type
CONSTEXPR_TYPED_TEST(MdspanDetailIsRepresentableAsTest, UserDefinedTypeWithSmallerWithIntermediateConversion) {
  auto const small_value = user_defined_type<TypeParam>{42};
  CONSTEXPR_ASSERT(is_representable_as<smaller_type_t<TypeParam>>(small_value));

  auto const big_value = user_defined_type<TypeParam>{std::numeric_limits<TypeParam>::max()};
  CONSTEXPR_ASSERT(is_representable_as<smaller_type_t<TypeParam>>(big_value));
}

/// @test @c is_representable_as can cast a user defined type with a conversion operator to a larger integral type
CONSTEXPR_TYPED_TEST(MdspanDetailIsRepresentableAsTest, UserDefinedTypeWithLargerWithIntermediateConversion) {
  auto const value = user_defined_type<TypeParam>{42};
  CONSTEXPR_ASSERT(is_representable_as<larger_type_t<TypeParam>>(value));
}

}  // namespace
