// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/detail/representable_cast.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/type_identity.hpp"
#include "arene/base/utility/safe_comparisons.hpp"

// NOLINTBEGIN(readability-identifier-length)

namespace {

using arene::base::cmp_equal;
using arene::base::type_identity;
using arene::base::mdspan_detail::representable_cast;

template <class T>
class MdspanDetailRepresentableCastTest : public testing::Test {};

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

TYPED_TEST_SUITE(MdspanDetailRepresentableCastTest, TestTypes, );

/// @test @c representable_cast can cast an integral type to a smaller type
CONSTEXPR_TYPED_TEST(MdspanDetailRepresentableCastTest, CastToSmaller) {
  auto const from = TypeParam{42};
  auto const to = representable_cast<smaller_type_t<TypeParam>>(from);

  CONSTEXPR_ASSERT(cmp_equal(from, to));
  CONSTEXPR_ASSERT(  //
      std::is_same<  //
          smaller_type_t<TypeParam>,
          decltype(arene::base::mdspan_detail::representable_cast<smaller_type_t<TypeParam>>(from))  //
          >::value
  );
}

/// @test @c representable_cast can cast an integral type to a larger type
CONSTEXPR_TYPED_TEST(MdspanDetailRepresentableCastTest, CastToLarger) {
  auto const from = TypeParam{42};
  auto const to = representable_cast<larger_type_t<TypeParam>>(from);

  CONSTEXPR_ASSERT(cmp_equal(from, to));
  CONSTEXPR_ASSERT(  //
      std::is_same<  //
          larger_type_t<TypeParam>,
          decltype(arene::base::mdspan_detail::representable_cast<larger_type_t<TypeParam>>(from))  //
          >::value
  );
}

template <class T>
struct user_defined_type {
  // implicit conversion is intentional -- it allows it to be used as an intermediate conversion
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr operator T() const noexcept { return T{42}; }
};

/// @test @c representable_cast can cast a user defined type with a conversion operator to a smaller integral type
CONSTEXPR_TYPED_TEST(MdspanDetailRepresentableCastTest, UserDefinedTypeToSmallerWithIntermediateConversion) {
  auto const from = user_defined_type<TypeParam>{};
  auto const to = representable_cast<smaller_type_t<TypeParam>>(from);

  CONSTEXPR_ASSERT(cmp_equal(static_cast<TypeParam>(from), to));
  CONSTEXPR_ASSERT(  //
      std::is_same<  //
          smaller_type_t<TypeParam>,
          decltype(arene::base::mdspan_detail::representable_cast<smaller_type_t<TypeParam>>(from))  //
          >::value
  );
}

/// @test @c representable_cast can cast a user defined type with a conversion operator to a larger integral type
CONSTEXPR_TYPED_TEST(MdspanDetailRepresentableCastTest, UserDefinedTypeToLargerWithIntermediateConversion) {
  auto const from = user_defined_type<TypeParam>{};
  auto const to = representable_cast<larger_type_t<TypeParam>>(from);

  CONSTEXPR_ASSERT(cmp_equal(static_cast<TypeParam>(from), to));
  CONSTEXPR_ASSERT(  //
      std::is_same<  //
          larger_type_t<TypeParam>,
          decltype(arene::base::mdspan_detail::representable_cast<larger_type_t<TypeParam>>(from))  //
          >::value
  );
}

}  // namespace

// NOLINTEND(readability-identifier-length)
