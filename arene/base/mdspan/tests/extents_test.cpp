// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/extents.hpp"

#include <gtest/gtest.h>

#include "arene/base/algorithm/all_of.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/make_unsigned.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/is_implicitly_constructible.hpp"
#include "arene/base/utility/safe_comparisons.hpp"

namespace {
template <typename>
struct bool_wrapper {
  using type = bool;
};
// NOLINTBEGIN(google-runtime-int)
using SupportedIndexTypes = ::testing::Types<
    char,
    signed char,
    unsigned char,
    short,
    unsigned short,
    int,
    unsigned,
    long,
    unsigned long,
    long long,
    unsigned long long>;
// NOLINTEND(google-runtime-int)

template <typename IndexType>
class ExtentsTypedTest : public testing::Test {};

TYPED_TEST_SUITE(ExtentsTypedTest, SupportedIndexTypes, );

/// @test Given `extents<IndexType, Extents...>` for which `IndexType` is a valid index type for an `extent`, then
/// `extents<IndexType, Extents...>::index_type` exists and is `IndexType`.
TYPED_TEST(ExtentsTypedTest, IndexTypeAliasIsIndexType) {
  testing::StaticAssertTypeEq<typename arene::base::extents<TypeParam>::index_type, TypeParam>();
}
/// @test Given `extents<IndexType, Extents...>`, then `extents<IndexType, Extents...>::rank_type` exists and is
/// `std::size_t`.
TYPED_TEST(ExtentsTypedTest, RankTypeAliasIsSizeT) {
  testing::StaticAssertTypeEq<typename arene::base::extents<TypeParam>::rank_type, std::size_t>();
}

/// @test Given `extents<IndexType, Extents...>`, then `extents<IndexType, Extents...>::size_type` exists and is
/// `std::make_unsigned_t<IndexType>`.
TYPED_TEST(ExtentsTypedTest, SizeTypeAliasIsUnsignedIndexType) {
  testing::StaticAssertTypeEq<typename arene::base::extents<TypeParam>::size_type, std::make_unsigned_t<TypeParam>>();
}

template <template <class IndexType, std::size_t... Extents> class E>
constexpr auto has_expected_extents_signature() -> bool {
  return true;
}

/// @test Ensure that @c extents has the right template signature
TEST(Extents, TemplateSignatureCorrect) {
  static_assert(has_expected_extents_signature<arene::base::extents>(), "Must have the right template arguments");
}

/// @test Ensure that @c extents can be instantiated with an @c IndexType that is an integral
TYPED_TEST(ExtentsTypedTest, IndexTypeCanBeIntegral) {
  std::ignore = arene::base::extents<TypeParam>{};
  std::ignore = arene::base::extents<TypeParam, 1, 2, 3>{};
}

/// @test Ensure that @c extents can be instantiated with extent values that are
/// small or extent values that are equal to @c dynamic_extent
TYPED_TEST(ExtentsTypedTest, DyanmicExtentAndSmallStaticExtentsAreOkay) {
  std::ignore = arene::base::extents<TypeParam, arene::base::dynamic_extent>{};
  std::ignore = arene::base::extents<TypeParam, 1, 2, 3, arene::base::dynamic_extent, 42>{};
}

/// @test Given `extents<IndexType, Extents...>`, then `extents<IndexType, Extents...>::rank == sizeof...(Extents)`.
CONSTEXPR_TYPED_TEST(ExtentsTypedTest, RankIsCountOfExtents) {
  CONSTEXPR_ASSERT_EQ((arene::base::extents<TypeParam, 1, 2, 3, arene::base::dynamic_extent, 42>::rank()), 5U);
  CONSTEXPR_ASSERT_EQ((arene::base::extents<TypeParam, 1, arene::base::dynamic_extent, 42>::rank()), 3U);
  CONSTEXPR_ASSERT_EQ(arene::base::extents<TypeParam>::rank(), 0U);
  CONSTEXPR_ASSERT_EQ((arene::base::extents<TypeParam, arene::base::dynamic_extent>::rank()), 1U);
  CONSTEXPR_ASSERT_EQ((arene::base::extents<TypeParam, 99>::rank()), 1U);
}

/// @test Ensure that @c rank_dynamic returns the number of extents specified as @c dynamic_extent
CONSTEXPR_TYPED_TEST(ExtentsTypedTest, DynamicRankIsNumberOfDynamicExtents) {
  CONSTEXPR_ASSERT_EQ((arene::base::extents<TypeParam, 1, 2, 3, arene::base::dynamic_extent, 42>::rank_dynamic()), 1U);
  CONSTEXPR_ASSERT_EQ((arene::base::extents<TypeParam, 1, arene::base::dynamic_extent, 42>::rank_dynamic()), 1U);
  CONSTEXPR_ASSERT_EQ(arene::base::extents<TypeParam>::rank_dynamic(), 0U);
  CONSTEXPR_ASSERT_EQ((arene::base::extents<TypeParam, arene::base::dynamic_extent>::rank_dynamic()), 1U);
  CONSTEXPR_ASSERT_EQ((arene::base::extents<TypeParam, 99>::rank_dynamic()), 0U);
  CONSTEXPR_ASSERT_EQ(
      (arene::base::extents<TypeParam, 99, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13>::rank_dynamic()),
      0U
  );
  CONSTEXPR_ASSERT_EQ(
      (arene::base::extents<TypeParam, arene::base::dynamic_extent, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13>::
           rank_dynamic()),
      1U
  );
  CONSTEXPR_ASSERT_EQ(
      (arene::base::extents<
          TypeParam,
          arene::base::dynamic_extent,
          1,
          arene::base::dynamic_extent,
          3,
          arene::base::dynamic_extent,
          arene::base::dynamic_extent,
          6,
          7,
          arene::base::dynamic_extent,
          9,
          10,
          11,
          12,
          13>::rank_dynamic()),
      5U
  );
}

/// @test Ensure that @c static_extent returns the corresponding value from @c Extents
TYPED_TEST(ExtentsTypedTest, StaticExtentReturnsTemplateArgsInConstexpr) {
  using ext1 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      1,
      arene::base::dynamic_extent,
      3,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6,
      7,
      arene::base::dynamic_extent,
      9,
      10,
      11,
      12,
      13>;

  STATIC_ASSERT_EQ(ext1::static_extent(0), arene::base::dynamic_extent);
  STATIC_ASSERT_EQ(ext1::static_extent(1), 1);
  STATIC_ASSERT_EQ(ext1::static_extent(2), arene::base::dynamic_extent);
  STATIC_ASSERT_EQ(ext1::static_extent(3), 3);
  STATIC_ASSERT_EQ(ext1::static_extent(4), arene::base::dynamic_extent);
  STATIC_ASSERT_EQ(ext1::static_extent(5), arene::base::dynamic_extent);
  STATIC_ASSERT_EQ(ext1::static_extent(6), 6);
  STATIC_ASSERT_EQ(ext1::static_extent(7), 7);
  STATIC_ASSERT_EQ(ext1::static_extent(8), arene::base::dynamic_extent);
  STATIC_ASSERT_EQ(ext1::static_extent(9), 9);
  STATIC_ASSERT_EQ(ext1::static_extent(10), 10);
  STATIC_ASSERT_EQ(ext1::static_extent(11), 11);
  STATIC_ASSERT_EQ(ext1::static_extent(12), 12);
  STATIC_ASSERT_EQ(ext1::static_extent(13), 13);

  using ext2 = arene::base::extents<TypeParam, 1, arene::base::dynamic_extent, 6, 7, 9, 10, 11, 12, 13>;

  STATIC_ASSERT_EQ(ext2::static_extent(0), 1);
  STATIC_ASSERT_EQ(ext2::static_extent(1), arene::base::dynamic_extent);
  STATIC_ASSERT_EQ(ext2::static_extent(2), 6);
  STATIC_ASSERT_EQ(ext2::static_extent(3), 7);
  STATIC_ASSERT_EQ(ext2::static_extent(4), 9);
  STATIC_ASSERT_EQ(ext2::static_extent(5), 10);
  STATIC_ASSERT_EQ(ext2::static_extent(6), 11);
  STATIC_ASSERT_EQ(ext2::static_extent(7), 12);
  STATIC_ASSERT_EQ(ext2::static_extent(8), 13);
}

/// @test Ensure that @c static_extent returns the corresponding value from @c Extents
TYPED_TEST(ExtentsTypedTest, StaticExtentReturnsTemplateArgsAtRuntime) {
  using ext1 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      1,
      arene::base::dynamic_extent,
      3,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6,
      7,
      arene::base::dynamic_extent,
      9,
      10,
      11,
      12,
      13>;

  ASSERT_EQ(ext1::static_extent(0), arene::base::dynamic_extent);
  ASSERT_EQ(ext1::static_extent(1), 1);
  ASSERT_EQ(ext1::static_extent(2), arene::base::dynamic_extent);
  ASSERT_EQ(ext1::static_extent(3), 3);
  ASSERT_EQ(ext1::static_extent(4), arene::base::dynamic_extent);
  ASSERT_EQ(ext1::static_extent(5), arene::base::dynamic_extent);
  ASSERT_EQ(ext1::static_extent(6), 6);
  ASSERT_EQ(ext1::static_extent(7), 7);
  ASSERT_EQ(ext1::static_extent(8), arene::base::dynamic_extent);
  ASSERT_EQ(ext1::static_extent(9), 9);
  ASSERT_EQ(ext1::static_extent(10), 10);
  ASSERT_EQ(ext1::static_extent(11), 11);
  ASSERT_EQ(ext1::static_extent(12), 12);
  ASSERT_EQ(ext1::static_extent(13), 13);

  using ext2 = arene::base::extents<TypeParam, 100, 101, 102, 103, 104>;

  ASSERT_EQ(ext2::static_extent(0), 100);
  ASSERT_EQ(ext2::static_extent(1), 101);
  ASSERT_EQ(ext2::static_extent(2), 102);
  ASSERT_EQ(ext2::static_extent(3), 103);
  ASSERT_EQ(ext2::static_extent(4), 104);
}

/// @test Ensure that @c extent returns the actual runtime extent value for fixed extents
CONSTEXPR_TYPED_TEST(ExtentsTypedTest, ExtentReturnsTheActualExtentForAllStaticExtents) {
  using ext1 = arene::base::extents<TypeParam, 1, 3, 6, 7, 9, 10, 11, 12, 13>;

  ext1 const extent_var1{};

  CONSTEXPR_ASSERT_EQ(extent_var1.extent(0), TypeParam{1});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(1), TypeParam{3});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(2), TypeParam{6});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(3), TypeParam{7});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(4), TypeParam{9});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(5), TypeParam{10});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(6), TypeParam{11});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(7), TypeParam{12});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(8), TypeParam{13});
}

/// @test Ensure that @c static_extent returns the actual runtime extent value for fixed extents
TYPED_TEST(ExtentsTypedTest, StaticExtentReturnsTheActualExtentForAllStaticExtents) {
  using ext1 = arene::base::extents<TypeParam, 1, 3, 6, 7, 9, 10, 11, 12, 13>;

  arene::base::array<std::size_t, 9> const expected{1, 3, 6, 7, 9, 10, 11, 12, 13};

  for (std::size_t dim = 0U; dim < expected.size(); ++dim) {
    ASSERT_EQ(expected[dim], ext1::static_extent(dim));
  }
}

/// @test Ensure that @c extent returns the actual runtime extent value for fixed extents
TYPED_TEST(ExtentsTypedTest, ExtentReturnsTheActualExtentForAllStaticExtentsAtRuntime) {
  using ext1 = arene::base::extents<TypeParam, 1, 2, 3, 40, 50, 6>;

  ext1 const extent_var1{};

  CONSTEXPR_ASSERT_EQ(extent_var1.extent(0), TypeParam{1});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(1), TypeParam{2});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(2), TypeParam{3});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(3), TypeParam{40});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(4), TypeParam{50});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(5), TypeParam{6});
}

/// @test Ensure that @c extent returns the actual runtime extent value for dynamic extents, and the specified value
/// for fixed extents
CONSTEXPR_TYPED_TEST(ExtentsTypedTest, ExtentReturnsTheActualExtentForASingleDynamicExtent) {
  using ext1 = arene::base::extents<TypeParam, 1, arene::base::dynamic_extent, 6, 7, 9, 10, 11, 12, 13>;

  std::int32_t const chosen_extent = 42;
  ext1 const extent_var1{chosen_extent};

  CONSTEXPR_ASSERT_EQ(extent_var1.extent(0), TypeParam{1});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(1), static_cast<TypeParam>(chosen_extent));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(2), TypeParam{6});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(3), TypeParam{7});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(4), TypeParam{9});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(5), TypeParam{10});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(6), TypeParam{11});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(7), TypeParam{12});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(8), TypeParam{13});
}

/// @test Can pass convertible things as constructor arguments
CONSTEXPR_TYPED_TEST(ExtentsTypedTest, ExtentReturnsTheActualExtentForConvertedExtents) {
  using ext1 = arene::base::extents<TypeParam, 1, arene::base::dynamic_extent, 6, 7, 9, 10, 11, 12, 13>;

  std::int32_t const chosen_extent = 42;
  ext1 const extent_var1{
      1,
      chosen_extent,
      std::integral_constant<std::int8_t, 6>{},
      std::integral_constant<std::int16_t, 7>{},
      std::integral_constant<std::uint8_t, 9>{},
      std::integral_constant<std::uint16_t, 10>{},
      std::integral_constant<std::uint32_t, 11>{},
      std::integral_constant<std::int32_t, 12>{},
      std::integral_constant<std::int64_t, 13>{}
  };

  CONSTEXPR_ASSERT_EQ(extent_var1.extent(0), TypeParam{1});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(1), static_cast<TypeParam>(chosen_extent));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(2), TypeParam{6});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(3), TypeParam{7});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(4), TypeParam{9});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(5), TypeParam{10});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(6), TypeParam{11});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(7), TypeParam{12});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(8), TypeParam{13});
}

/// @test Ensure that @c extent returns the actual runtime extent value for dynamic extents, and the specified value
/// for fixed extents
CONSTEXPR_TEST(Extents, ExtentReturnsTheActualExtentForMultipleDynamicExtents) {
  using ext1 = arene::base::extents<
      std::int32_t,
      arene::base::dynamic_extent,
      1,
      arene::base::dynamic_extent,
      3,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6,
      7,
      arene::base::dynamic_extent,
      9,
      10,
      11,
      12,
      13>;

  std::int32_t const chosen_extent1 = 42;
  std::int32_t const chosen_extent2 = 42000;
  std::int32_t const chosen_extent3 = 42000000;
  std::int32_t const chosen_extent4 = 1234567890;
  std::int32_t const chosen_extent5 = 11;
  ext1 const extent_var1{chosen_extent1, chosen_extent2, chosen_extent3, chosen_extent4, chosen_extent5};

  CONSTEXPR_ASSERT_EQ(extent_var1.extent(0), chosen_extent1);
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(1), 1);
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(2), chosen_extent2);
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(3), 3);
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(4), chosen_extent3);
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(5), chosen_extent4);
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(6), 6);
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(7), 7);
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(8), chosen_extent5);
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(9), 9);
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(10), 10);
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(11), 11);
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(12), 12);
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(13), 13);
}

/// @test Ensure that @c extent returns the actual runtime extent value for dynamic extents, and the specified value
/// for fixed extents
TYPED_TEST(ExtentsTypedTest, ExtentReturnsTheActualExtentForMultipleDynamicExtentsAtRuntime) {
  using ext1 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      1,
      arene::base::dynamic_extent,
      3,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6,
      7,
      arene::base::dynamic_extent,
      9,
      10,
      11,
      12,
      13>;

  // NOLINTBEGIN(misc-const-correctness)
  TypeParam chosen_extent1 = 42;
  TypeParam chosen_extent2 = 30;
  TypeParam chosen_extent3 = 40;
  TypeParam chosen_extent4 = 50;
  TypeParam chosen_extent5 = 11;
  // NOLINTEND(misc-const-correctness)
  ext1 const extent_var1{chosen_extent1, chosen_extent2, chosen_extent3, chosen_extent4, chosen_extent5};

  ASSERT_EQ(extent_var1.extent(0), chosen_extent1);
  ASSERT_EQ(extent_var1.extent(1), 1);
  ASSERT_EQ(extent_var1.extent(2), chosen_extent2);
  ASSERT_EQ(extent_var1.extent(3), 3);
  ASSERT_EQ(extent_var1.extent(4), chosen_extent3);
  ASSERT_EQ(extent_var1.extent(5), chosen_extent4);
  ASSERT_EQ(extent_var1.extent(6), 6);
  ASSERT_EQ(extent_var1.extent(7), 7);
  ASSERT_EQ(extent_var1.extent(8), chosen_extent5);
  ASSERT_EQ(extent_var1.extent(9), 9);
  ASSERT_EQ(extent_var1.extent(10), 10);
  ASSERT_EQ(extent_var1.extent(11), 11);
  ASSERT_EQ(extent_var1.extent(12), 12);
  ASSERT_EQ(extent_var1.extent(13), 13);
}

/// @test Ensure that @c extent returns zero as the extent value for dynamic extents, and the specified value
/// for fixed extents, on a default-initialized @c extents object
CONSTEXPR_TYPED_TEST(ExtentsTypedTest, DefaultExtentsForDynamicExtentsIsZero) {
  using ext1 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      1,
      arene::base::dynamic_extent,
      3,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6,
      7,
      arene::base::dynamic_extent,
      9,
      10,
      11,
      12,
      13>;

  ext1 const extent_var1{};

  CONSTEXPR_ASSERT_EQ(extent_var1.extent(0), TypeParam{0});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(1), TypeParam{1});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(2), TypeParam{0});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(3), TypeParam{3});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(4), TypeParam{0});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(5), TypeParam{0});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(6), TypeParam{6});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(7), TypeParam{7});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(8), TypeParam{0});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(9), TypeParam{9});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(10), TypeParam{10});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(11), TypeParam{11});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(12), TypeParam{12});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(13), TypeParam{13});
}

template <typename IndexType, std::size_t... Extents>
extern constexpr bool extents_is_implicitly_constructible_v =
    arene::base::is_implicitly_constructible_v<arene::base::extents<IndexType, Extents...>, decltype(Extents)...>;

/// @test The constructor from a list of extents must be explicit
TYPED_TEST(ExtentsTypedTest, ConstructionFromExtentsIsExplicit) {
  STATIC_ASSERT_FALSE(
      std::is_convertible<std::int32_t, arene::base::extents<std::int32_t, arene::base::dynamic_extent>>::value
  );
  STATIC_ASSERT_FALSE(std::is_convertible<std::int32_t, arene::base::extents<std::int32_t, 5>>::value);
  STATIC_ASSERT_FALSE(extents_is_implicitly_constructible_v<TypeParam, 5>);
  STATIC_ASSERT_FALSE(extents_is_implicitly_constructible_v<TypeParam, arene::base::dynamic_extent>);
  STATIC_ASSERT_FALSE(extents_is_implicitly_constructible_v<TypeParam, 5, 23, 42>);
  STATIC_ASSERT_FALSE(extents_is_implicitly_constructible_v<TypeParam, 5, arene::base::dynamic_extent>);
  STATIC_ASSERT_FALSE(extents_is_implicitly_constructible_v<
                      TypeParam,
                      arene::base::dynamic_extent,
                      arene::base::dynamic_extent>);
}

/// @test Ensure that @c extent can be initialized with a full list of extents where the value matches the
/// specified extent for fixed extents
CONSTEXPR_TYPED_TEST(ExtentsTypedTest, CanPassFullListOfExtents) {
  using ext1 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      1,
      arene::base::dynamic_extent,
      3,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6,
      7,
      arene::base::dynamic_extent,
      9,
      10,
      11,
      12,
      13>;

  std::int32_t const chosen_extent1 = 100;
  std::int32_t const chosen_extent2 = 101;
  std::int32_t const chosen_extent3 = 102;
  std::int32_t const chosen_extent4 = 103;
  std::int32_t const chosen_extent5 = 104;

  ext1 const extent_var1{
      chosen_extent1,
      1,
      chosen_extent2,
      3,
      chosen_extent3,
      chosen_extent4,
      6,
      7,
      chosen_extent5,
      9,
      10,
      11,
      12,
      13
  };

  CONSTEXPR_ASSERT_EQ(extent_var1.extent(0), static_cast<TypeParam>(chosen_extent1));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(1), TypeParam{1});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(2), static_cast<TypeParam>(chosen_extent2));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(3), TypeParam{3});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(4), static_cast<TypeParam>(chosen_extent3));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(5), static_cast<TypeParam>(chosen_extent4));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(6), TypeParam{6});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(7), TypeParam{7});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(8), static_cast<TypeParam>(chosen_extent5));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(9), TypeParam{9});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(10), TypeParam{10});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(11), TypeParam{11});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(12), TypeParam{12});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(13), TypeParam{13});
}

/// @test Ensure that @c extent can be initialized with a full list of extents held in a @c span where the value matches
/// the specified extent for fixed extents, and is initialized from the specified value for dynamic extents
CONSTEXPR_TYPED_TEST(ExtentsTypedTest, CanPassFullListOfExtentsBySpan) {
  using ext1 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      1,
      arene::base::dynamic_extent,
      3,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6,
      7,
      arene::base::dynamic_extent,
      9,
      10,
      11,
      12,
      13>;

  std::int32_t const chosen_extent1 = 100;
  std::int32_t const chosen_extent2 = 101;
  std::int32_t const chosen_extent3 = 102;
  std::int32_t const chosen_extent4 = 103;
  std::int32_t const chosen_extent5 = 104;

  arene::base::array<std::int32_t, 14> source_extents{
      chosen_extent1,
      1,
      chosen_extent2,
      3,
      chosen_extent3,
      chosen_extent4,
      6,
      7,
      chosen_extent5,
      9,
      10,
      11,
      12,
      13
  };

  ext1 const extent_var1{arene::base::span<std::int32_t, 14>(source_extents)};

  CONSTEXPR_ASSERT_EQ(extent_var1.extent(0), static_cast<TypeParam>(chosen_extent1));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(1), TypeParam{1});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(2), static_cast<TypeParam>(chosen_extent2));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(3), TypeParam{3});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(4), static_cast<TypeParam>(chosen_extent3));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(5), static_cast<TypeParam>(chosen_extent4));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(6), TypeParam{6});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(7), TypeParam{7});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(8), static_cast<TypeParam>(chosen_extent5));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(9), TypeParam{9});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(10), TypeParam{10});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(11), TypeParam{11});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(12), TypeParam{12});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(13), TypeParam{13});
}

/// @test Ensure that @c extent can be initialized with only the dynamic extents held in a @c span where the value
/// matches the specified extent for fixed extents, and is initialized from the specified value for dynamic extents
CONSTEXPR_TYPED_TEST(ExtentsTypedTest, CanPassOnlyDynamicExtentsBySpan) {
  using ext1 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      1,
      arene::base::dynamic_extent,
      3,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6,
      7,
      arene::base::dynamic_extent,
      9,
      10,
      11,
      12,
      13>;

  std::int32_t const chosen_extent1 = 100;
  std::int32_t const chosen_extent2 = 101;
  std::int32_t const chosen_extent3 = 102;
  std::int32_t const chosen_extent4 = 103;
  std::int32_t const chosen_extent5 = 104;

  arene::base::array<std::int32_t, 5> source_extents{
      chosen_extent1,
      chosen_extent2,
      chosen_extent3,
      chosen_extent4,
      chosen_extent5,
  };

  ext1 const extent_var1{arene::base::span<std::int32_t, 5>(source_extents)};

  CONSTEXPR_ASSERT_EQ(extent_var1.extent(0), static_cast<TypeParam>(chosen_extent1));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(1), TypeParam{1});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(2), static_cast<TypeParam>(chosen_extent2));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(3), TypeParam{3});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(4), static_cast<TypeParam>(chosen_extent3));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(5), static_cast<TypeParam>(chosen_extent4));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(6), TypeParam{6});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(7), TypeParam{7});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(8), static_cast<TypeParam>(chosen_extent5));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(9), TypeParam{9});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(10), TypeParam{10});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(11), TypeParam{11});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(12), TypeParam{12});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(13), TypeParam{13});
}

/// @test Ensure that @c extent of all static extents can be initialized by a span of all the dynamic extents (i.e. an
/// empty span).
CONSTEXPR_TYPED_TEST(ExtentsTypedTest, CanPassOnlyDynamicExtentsBySpanWhenAllExtentsAreStatic) {
  constexpr std::int32_t chosen_extent1 = 100;
  constexpr std::int32_t chosen_extent2 = 101;
  constexpr std::int32_t chosen_extent3 = 102;
  constexpr std::int32_t chosen_extent4 = 103;
  constexpr std::int32_t chosen_extent5 = 104;

  using ext1 =
      arene::base::extents<TypeParam, chosen_extent1, chosen_extent2, chosen_extent3, chosen_extent4, chosen_extent5>;

  arene::base::array<std::int32_t, 0> source_extents{};
  ext1 const extent_var1{arene::base::span<std::int32_t, 0>(source_extents)};

  CONSTEXPR_ASSERT_EQ(extent_var1.extent(0), static_cast<TypeParam>(chosen_extent1));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(1), static_cast<TypeParam>(chosen_extent2));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(2), static_cast<TypeParam>(chosen_extent3));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(3), static_cast<TypeParam>(chosen_extent4));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(4), static_cast<TypeParam>(chosen_extent5));
}

/// @test Ensure that @c extent can be initialized with a full list of extents held in a @c array where the value
/// matches the specified extent for fixed extents, and is initialized from the specified value for dynamic extents
CONSTEXPR_TYPED_TEST(ExtentsTypedTest, CanPassFullListOfExtentsByArray) {
  using ext1 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      1,
      arene::base::dynamic_extent,
      3,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6,
      7,
      arene::base::dynamic_extent,
      9,
      10,
      11,
      12,
      13>;

  std::int32_t const chosen_extent1 = 100;
  std::int32_t const chosen_extent2 = 101;
  std::int32_t const chosen_extent3 = 102;
  std::int32_t const chosen_extent4 = 103;
  std::int32_t const chosen_extent5 = 104;

  // NOLINTNEXTLINE(misc-const-correctness)
  arene::base::array<std::int32_t, 14> source_extents{
      chosen_extent1,
      1,
      chosen_extent2,
      3,
      chosen_extent3,
      chosen_extent4,
      6,
      7,
      chosen_extent5,
      9,
      10,
      11,
      12,
      13
  };

  ext1 const extent_var1{source_extents};

  CONSTEXPR_ASSERT_EQ(extent_var1.extent(0), static_cast<TypeParam>(chosen_extent1));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(1), TypeParam{1});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(2), static_cast<TypeParam>(chosen_extent2));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(3), TypeParam{3});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(4), static_cast<TypeParam>(chosen_extent3));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(5), static_cast<TypeParam>(chosen_extent4));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(6), TypeParam{6});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(7), TypeParam{7});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(8), static_cast<TypeParam>(chosen_extent5));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(9), TypeParam{9});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(10), TypeParam{10});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(11), TypeParam{11});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(12), TypeParam{12});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(13), TypeParam{13});
}

/// @test Ensure that @c extent can be initialized with only the dynamic extents held in a @c array where the value
/// matches the specified extent for fixed extents, and is initialized from the specified value for dynamic extents
CONSTEXPR_TYPED_TEST(ExtentsTypedTest, CanPassOnlyDynamicExtentsByArray) {
  using ext1 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      1,
      arene::base::dynamic_extent,
      3,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6,
      7,
      arene::base::dynamic_extent,
      9,
      10,
      11,
      12,
      13>;

  std::int32_t const chosen_extent1 = 100;
  std::int32_t const chosen_extent2 = 101;
  std::int32_t const chosen_extent3 = 102;
  std::int32_t const chosen_extent4 = 103;
  std::int32_t const chosen_extent5 = 104;

  arene::base::array<std::int32_t, 5> const source_extents{
      chosen_extent1,
      chosen_extent2,
      chosen_extent3,
      chosen_extent4,
      chosen_extent5,
  };

  ext1 const extent_var1{source_extents};

  CONSTEXPR_ASSERT_EQ(extent_var1.extent(0), static_cast<TypeParam>(chosen_extent1));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(1), TypeParam{1});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(2), static_cast<TypeParam>(chosen_extent2));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(3), TypeParam{3});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(4), static_cast<TypeParam>(chosen_extent3));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(5), static_cast<TypeParam>(chosen_extent4));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(6), TypeParam{6});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(7), TypeParam{7});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(8), static_cast<TypeParam>(chosen_extent5));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(9), TypeParam{9});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(10), TypeParam{10});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(11), TypeParam{11});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(12), TypeParam{12});
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(13), TypeParam{13});
}

/// @test Ensure that @c extent can be initialized with a full list of extents where all the extents are
/// dynamic
CONSTEXPR_TYPED_TEST(ExtentsTypedTest, CanPassFullListOfDynamicExtents) {
  using ext1 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent>;

  std::int32_t const chosen_extent1 = 100;
  std::int32_t const chosen_extent2 = 101;
  std::int32_t const chosen_extent3 = 102;
  std::int32_t const chosen_extent4 = 103;
  std::int32_t const chosen_extent5 = 104;

  ext1 const extent_var1{chosen_extent1, chosen_extent2, chosen_extent3, chosen_extent4, chosen_extent5};

  CONSTEXPR_ASSERT_EQ(extent_var1.extent(0), static_cast<TypeParam>(chosen_extent1));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(1), static_cast<TypeParam>(chosen_extent2));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(2), static_cast<TypeParam>(chosen_extent3));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(3), static_cast<TypeParam>(chosen_extent4));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(4), static_cast<TypeParam>(chosen_extent5));
}

/// @test Ensure that @c extent of all static extents can be initialized by an array of all the dynamic extents (i.e. an
/// empty array).
CONSTEXPR_TYPED_TEST(ExtentsTypedTest, CanPassOnlyDynamicExtentsByArrayWhenAllExtentsAreStatic) {
  constexpr std::int32_t chosen_extent1 = 100;
  constexpr std::int32_t chosen_extent2 = 101;
  constexpr std::int32_t chosen_extent3 = 102;
  constexpr std::int32_t chosen_extent4 = 103;
  constexpr std::int32_t chosen_extent5 = 104;

  using ext1 =
      arene::base::extents<TypeParam, chosen_extent1, chosen_extent2, chosen_extent3, chosen_extent4, chosen_extent5>;

  arene::base::array<std::int32_t, 0> const source_extents{};
  ext1 const extent_var1{source_extents};

  CONSTEXPR_ASSERT_EQ(extent_var1.extent(0), static_cast<TypeParam>(chosen_extent1));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(1), static_cast<TypeParam>(chosen_extent2));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(2), static_cast<TypeParam>(chosen_extent3));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(3), static_cast<TypeParam>(chosen_extent4));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(4), static_cast<TypeParam>(chosen_extent5));
}

/// @brief A helper function which checks the @c is_extents_v type trait on all the parameters and sees if all are
/// true
/// @tparam Ts A list of types to check
/// @return @c true if all the @c Ts are @c extents specializations as per <c>is_extents_v</c>, @c false otherwise
template <typename... Ts>
constexpr auto all_are_extents(testing::Types<Ts...>) noexcept -> bool {
  return arene::base::all_of(arene::base::is_extents_v<Ts>...);
}

/// @test The @c is_extents_v type trait works correctly for @c extents and @c dextents
TYPED_TEST(ExtentsTypedTest, IsExtentsTypeTraitWorks) {
  // The underlying index type is not viewed as an `extents`.
  STATIC_ASSERT_EQ(arene::base::is_extents_v<TypeParam>, false);

  // Various types of `extents` and `dextents` that we could construct are viewed as `extents`.
  using extents_types = testing::Types<
      arene::base::extents<TypeParam>,
      arene::base::extents<TypeParam, 123>,
      arene::base::extents<TypeParam, 7, 5, 0, 3, 2>,
      arene::base::extents<TypeParam, 1, arene::base::dynamic_extent, 2, 3, arene::base::dynamic_extent>,
      arene::base::dextents<TypeParam, 0>,
      arene::base::dextents<TypeParam, 1>,
      arene::base::dextents<TypeParam, 5>>;
  STATIC_ASSERT_TRUE(all_are_extents(extents_types{}));
}

/// @test Ensure that @c extent can be initialized with a full list of extents held in a @c span where all the extents
/// are dynamic
CONSTEXPR_TYPED_TEST(ExtentsTypedTest, CanPassFullListOfDynamicExtentsViaSpan) {
  using ext1 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent>;

  std::int32_t const chosen_extent1 = 100;
  std::int32_t const chosen_extent2 = 101;
  std::int32_t const chosen_extent3 = 102;
  std::int32_t const chosen_extent4 = 103;
  std::int32_t const chosen_extent5 = 104;

  arene::base::array<std::int32_t, 5>
      source_extents{chosen_extent1, chosen_extent2, chosen_extent3, chosen_extent4, chosen_extent5};
  ext1 const extent_var1{arene::base::span<std::int32_t, 5>(source_extents)};

  CONSTEXPR_ASSERT_EQ(extent_var1.extent(0), static_cast<TypeParam>(chosen_extent1));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(1), static_cast<TypeParam>(chosen_extent2));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(2), static_cast<TypeParam>(chosen_extent3));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(3), static_cast<TypeParam>(chosen_extent4));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(4), static_cast<TypeParam>(chosen_extent5));
}

/// @test Ensure that @c extent can be initialized with a full list of extents held in a @c array where all the
/// extents are dynamic
CONSTEXPR_TYPED_TEST(ExtentsTypedTest, CanPassFullListOfDynamicExtentsViaArray) {
  using ext1 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent>;

  std::int32_t const chosen_extent1 = 100;
  std::int32_t const chosen_extent2 = 101;
  std::int32_t const chosen_extent3 = 102;
  std::int32_t const chosen_extent4 = 103;
  std::int32_t const chosen_extent5 = 104;

  // NOLINTNEXTLINE(misc-const-correctness)
  arene::base::array<std::int32_t, 5>
      source_extents{chosen_extent1, chosen_extent2, chosen_extent3, chosen_extent4, chosen_extent5};
  ext1 const extent_var1{source_extents};

  CONSTEXPR_ASSERT_EQ(extent_var1.extent(0), static_cast<TypeParam>(chosen_extent1));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(1), static_cast<TypeParam>(chosen_extent2));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(2), static_cast<TypeParam>(chosen_extent3));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(3), static_cast<TypeParam>(chosen_extent4));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(4), static_cast<TypeParam>(chosen_extent5));
}

/// @test Ensure that @c extent can be initialized with a full list of extents held in a @c span where all the extents
/// are static
CONSTEXPR_TYPED_TEST(ExtentsTypedTest, CanPassFullListOfStaticExtentsViaSpan) {
  constexpr std::int32_t chosen_extent1 = 100;
  constexpr std::int32_t chosen_extent2 = 101;
  constexpr std::int32_t chosen_extent3 = 102;
  constexpr std::int32_t chosen_extent4 = 103;
  constexpr std::int32_t chosen_extent5 = 104;

  using ext1 =
      arene::base::extents<TypeParam, chosen_extent1, chosen_extent2, chosen_extent3, chosen_extent4, chosen_extent5>;

  arene::base::array<std::int32_t, 5>
      source_extents{chosen_extent1, chosen_extent2, chosen_extent3, chosen_extent4, chosen_extent5};
  ext1 const extent_var1{arene::base::span<std::int32_t, 5>(source_extents)};

  CONSTEXPR_ASSERT_EQ(extent_var1.extent(0), static_cast<TypeParam>(chosen_extent1));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(1), static_cast<TypeParam>(chosen_extent2));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(2), static_cast<TypeParam>(chosen_extent3));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(3), static_cast<TypeParam>(chosen_extent4));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(4), static_cast<TypeParam>(chosen_extent5));
}

/// @test Ensure that @c extent can be initialized with a full list of extents held in a @c array where all the
/// extents are static
CONSTEXPR_TYPED_TEST(ExtentsTypedTest, CanPassFullListOfStaticExtentsViaArray) {
  constexpr std::int32_t chosen_extent1 = 100;
  constexpr std::int32_t chosen_extent2 = 101;
  constexpr std::int32_t chosen_extent3 = 102;
  constexpr std::int32_t chosen_extent4 = 103;
  constexpr std::int32_t chosen_extent5 = 104;

  using ext1 =
      arene::base::extents<TypeParam, chosen_extent1, chosen_extent2, chosen_extent3, chosen_extent4, chosen_extent5>;

  // NOLINTNEXTLINE(misc-const-correctness)
  arene::base::array<std::int32_t, 5>
      source_extents{chosen_extent1, chosen_extent2, chosen_extent3, chosen_extent4, chosen_extent5};
  ext1 const extent_var1{source_extents};

  CONSTEXPR_ASSERT_EQ(extent_var1.extent(0), static_cast<TypeParam>(chosen_extent1));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(1), static_cast<TypeParam>(chosen_extent2));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(2), static_cast<TypeParam>(chosen_extent3));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(3), static_cast<TypeParam>(chosen_extent4));
  CONSTEXPR_ASSERT_EQ(extent_var1.extent(4), static_cast<TypeParam>(chosen_extent5));
}

/// @test @c extents cannot be constructed with a non-zero number of
/// arguments that is neither the @c rank() nor @c rank_dynamic()
TYPED_TEST(ExtentsTypedTest, CannotConstructWithWrongNumberOfExtents) {
  using ext1 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent>;

  STATIC_ASSERT_TRUE(std::is_constructible<ext1>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext1, int>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext1, int, int>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext1, int, int, int>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext1, int, int, int, int>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<ext1, int, int, int, int, int>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext1, int, int, int, int, int, int>::value);

  using ext2 = arene::base::
      extents<TypeParam, 42, arene::base::dynamic_extent, 27, arene::base::dynamic_extent, arene::base::dynamic_extent>;

  STATIC_ASSERT_TRUE(std::is_constructible<ext2>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext2, int>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext2, int, int>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<ext2, int, int, int>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext2, int, int, int, int>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<ext2, int, int, int, int, int>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext2, int, int, int, int, int, int>::value);

  using ext3 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      1,
      arene::base::dynamic_extent,
      3,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6,
      7,
      arene::base::dynamic_extent,
      9,
      10,
      11,
      12,
      13>;

  STATIC_ASSERT_TRUE(std::is_constructible<ext3>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, int>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, int, int>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, int, int, int>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, int, int, int, int>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<ext3, int, int, int, int, int>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, int, int, int, int, int, int>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, int, int, int, int, int, int, int>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, int, int, int, int, int, int, int, int>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, int, int, int, int, int, int, int, int, int>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, int, int, int, int, int, int, int, int, int, int>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, int, int, int, int, int, int, int, int, int, int, int>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, int, int, int, int, int, int, int, int, int, int, int, int>::value);
  STATIC_ASSERT_FALSE(
      std::is_constructible<ext3, int, int, int, int, int, int, int, int, int, int, int, int, int>::value
  );
  STATIC_ASSERT_TRUE(
      std::is_constructible<ext3, int, int, int, int, int, int, int, int, int, int, int, int, int, int>::value
  );
  STATIC_ASSERT_FALSE(
      std::is_constructible<ext3, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int>::value
  );
}

/// @test @c extents cannot be constructed with a @c span that is a size that is neither the @c rank() nor @c
/// rank_dynamic()
TYPED_TEST(ExtentsTypedTest, CannotConstructWithWrongNumberOfExtentsViaSpan) {
  using ext1 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent>;

  STATIC_ASSERT_FALSE(std::is_constructible<ext1, arene::base::span<int, 0>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext1, arene::base::span<int, 1>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext1, arene::base::span<int, 2>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext1, arene::base::span<int, 3>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext1, arene::base::span<int, 4>>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<ext1, arene::base::span<int, 5>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext1, arene::base::span<int, 6>>::value);

  using ext2 = arene::base::
      extents<TypeParam, 42, arene::base::dynamic_extent, 27, arene::base::dynamic_extent, arene::base::dynamic_extent>;

  STATIC_ASSERT_FALSE(std::is_constructible<ext2, arene::base::span<int, 0>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext2, arene::base::span<int, 1>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext2, arene::base::span<int, 2>>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<ext2, arene::base::span<int, 3>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext2, arene::base::span<int, 4>>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<ext2, arene::base::span<int, 5>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext2, arene::base::span<int, 6>>::value);

  using ext3 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      1,
      arene::base::dynamic_extent,
      3,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6,
      7,
      arene::base::dynamic_extent,
      9,
      10,
      11,
      12,
      13>;

  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::span<int, 0>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::span<int, 1>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::span<int, 2>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::span<int, 3>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::span<int, 4>>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<ext3, arene::base::span<int, 5>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::span<int, 6>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::span<int, 7>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::span<int, 8>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::span<int, 9>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::span<int, 10>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::span<int, 11>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::span<int, 12>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::span<int, 13>>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<ext3, arene::base::span<int, 14>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::span<int, 15>>::value);

  using ext4 = arene::base::extents<TypeParam, 10, 20, 30, 40, 50>;

  STATIC_ASSERT_TRUE(std::is_constructible<ext4, arene::base::span<int, 0>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext4, arene::base::span<int, 1>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext4, arene::base::span<int, 2>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext4, arene::base::span<int, 3>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext4, arene::base::span<int, 4>>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<ext4, arene::base::span<int, 5>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext4, arene::base::span<int, 6>>::value);
}

/// @test @c extents is only implicitly constructible with a @c span that is a size that is  @c dynamic_rank()
TYPED_TEST(ExtentsTypedTest, ImplicitConversionFromSpanOnlyForDynamicExtents) {
  using ext1 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent>;

  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 0>, ext1>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 1>, ext1>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 2>, ext1>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 3>, ext1>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 4>, ext1>::value);
  STATIC_ASSERT_TRUE(std::is_convertible<arene::base::span<int, 5>, ext1>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 6>, ext1>::value);

  using ext2 = arene::base::
      extents<TypeParam, 42, arene::base::dynamic_extent, 27, arene::base::dynamic_extent, arene::base::dynamic_extent>;

  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 0>, ext2>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 1>, ext2>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 2>, ext2>::value);
  STATIC_ASSERT_TRUE(std::is_convertible<arene::base::span<int, 3>, ext2>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 4>, ext2>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 5>, ext2>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 6>, ext2>::value);

  using ext3 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      1,
      arene::base::dynamic_extent,
      3,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6,
      7,
      arene::base::dynamic_extent,
      9,
      10,
      11,
      12,
      13>;

  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 0>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 1>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 2>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 3>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 4>, ext3>::value);
  STATIC_ASSERT_TRUE(std::is_convertible<arene::base::span<int, 5>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 6>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 7>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 8>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 9>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 10>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 11>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 12>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 13>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 14>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 15>, ext3>::value);

  using ext4 = arene::base::extents<TypeParam, 10, 20, 30, 40, 50>;

  STATIC_ASSERT_TRUE(std::is_convertible<arene::base::span<int, 0>, ext4>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 1>, ext4>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 2>, ext4>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 3>, ext4>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 4>, ext4>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 5>, ext4>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::span<int, 6>, ext4>::value);
}

/// @test @c extents cannot be constructed with a @c array that is a size that is neither the @c rank() nor @c
/// rank_dynamic()
TYPED_TEST(ExtentsTypedTest, CannotConstructWithWrongNumberOfExtentsViaArray) {
  using ext1 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent>;

  STATIC_ASSERT_FALSE(std::is_constructible<ext1, arene::base::array<int, 0>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext1, arene::base::array<int, 1>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext1, arene::base::array<int, 2>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext1, arene::base::array<int, 3>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext1, arene::base::array<int, 4>>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<ext1, arene::base::array<int, 5>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext1, arene::base::array<int, 6>>::value);

  using ext2 = arene::base::
      extents<TypeParam, 42, arene::base::dynamic_extent, 27, arene::base::dynamic_extent, arene::base::dynamic_extent>;

  STATIC_ASSERT_FALSE(std::is_constructible<ext2, arene::base::array<int, 0>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext2, arene::base::array<int, 1>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext2, arene::base::array<int, 2>>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<ext2, arene::base::array<int, 3>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext2, arene::base::array<int, 4>>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<ext2, arene::base::array<int, 5>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext2, arene::base::array<int, 6>>::value);

  using ext3 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      1,
      arene::base::dynamic_extent,
      3,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6,
      7,
      arene::base::dynamic_extent,
      9,
      10,
      11,
      12,
      13>;

  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::array<int, 0>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::array<int, 1>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::array<int, 2>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::array<int, 3>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::array<int, 4>>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<ext3, arene::base::array<int, 5>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::array<int, 6>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::array<int, 7>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::array<int, 8>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::array<int, 9>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::array<int, 10>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::array<int, 11>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::array<int, 12>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::array<int, 13>>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<ext3, arene::base::array<int, 14>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext3, arene::base::array<int, 15>>::value);

  using ext4 = arene::base::extents<TypeParam, 10, 20, 30, 40, 50>;

  STATIC_ASSERT_TRUE(std::is_constructible<ext4, arene::base::array<int, 0>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext4, arene::base::array<int, 1>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext4, arene::base::array<int, 2>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext4, arene::base::array<int, 3>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext4, arene::base::array<int, 4>>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<ext4, arene::base::array<int, 5>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<ext4, arene::base::array<int, 6>>::value);
}

/// @test @c extents is only implicitly constructible with a @c array that is a size that is  @c dynamic_rank()
TYPED_TEST(ExtentsTypedTest, ImplicitConversionFromArrayOnlyForDynamicExtents) {
  using ext1 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent>;

  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 0>, ext1>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 1>, ext1>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 2>, ext1>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 3>, ext1>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 4>, ext1>::value);
  STATIC_ASSERT_TRUE(std::is_convertible<arene::base::array<int, 5>, ext1>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 6>, ext1>::value);

  using ext2 = arene::base::
      extents<TypeParam, 42, arene::base::dynamic_extent, 27, arene::base::dynamic_extent, arene::base::dynamic_extent>;

  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 0>, ext2>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 1>, ext2>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 2>, ext2>::value);
  STATIC_ASSERT_TRUE(std::is_convertible<arene::base::array<int, 3>, ext2>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 4>, ext2>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 5>, ext2>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 6>, ext2>::value);

  using ext3 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      1,
      arene::base::dynamic_extent,
      3,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6,
      7,
      arene::base::dynamic_extent,
      9,
      10,
      11,
      12,
      13>;

  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 0>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 1>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 2>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 3>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 4>, ext3>::value);
  STATIC_ASSERT_TRUE(std::is_convertible<arene::base::array<int, 5>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 6>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 7>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 8>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 9>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 10>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 11>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 12>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 13>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 14>, ext3>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 15>, ext3>::value);

  using ext4 = arene::base::extents<TypeParam, 10, 20, 30, 40, 50>;

  STATIC_ASSERT_TRUE(std::is_convertible<arene::base::array<int, 0>, ext4>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 1>, ext4>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 2>, ext4>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 3>, ext4>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 4>, ext4>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 5>, ext4>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<arene::base::array<int, 6>, ext4>::value);
}

/// @test Verify that @c dextents expands to @c extents with the
/// specified number of dimensions, all specified as @c dynamic_extent
TYPED_TEST(ExtentsTypedTest, DextentsIsExtentsWithAllDynamicExtents) {
  testing::StaticAssertTypeEq<arene::base::dextents<TypeParam, 0>, arene::base::extents<TypeParam>>();
  testing::StaticAssertTypeEq<
      arene::base::dextents<TypeParam, 1>,
      arene::base::extents<TypeParam, arene::base::dynamic_extent>>();
  testing::StaticAssertTypeEq<
      arene::base::dextents<TypeParam, 2>,
      arene::base::extents<TypeParam, arene::base::dynamic_extent, arene::base::dynamic_extent>>();
  testing::StaticAssertTypeEq<
      arene::base::dextents<TypeParam, 10>,
      arene::base::extents<
          TypeParam,
          arene::base::dynamic_extent,
          arene::base::dynamic_extent,
          arene::base::dynamic_extent,
          arene::base::dynamic_extent,
          arene::base::dynamic_extent,
          arene::base::dynamic_extent,
          arene::base::dynamic_extent,
          arene::base::dynamic_extent,
          arene::base::dynamic_extent,
          arene::base::dynamic_extent>>();
}

/// @test Can construct one @c extents object from another with a different index type if the list of static extents
/// values exactly matches
TYPED_TEST(ExtentsTypedTest, CanConstructFromAnotherExtentsObjectIfFixedExtentsMatch) {
  STATIC_ASSERT_TRUE(std::is_constructible<
                     arene::base::extents<TypeParam, 1, 2, 3, 4>,
                     arene::base::extents<std::uint8_t, 1, 2, 3, 4>>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<
                     arene::base::extents<TypeParam, 1, 2, arene::base::dynamic_extent, 4>,
                     arene::base::extents<std::uint8_t, 1, 2, arene::base::dynamic_extent, 4>>::value);
  STATIC_ASSERT_TRUE(std::is_nothrow_constructible<
                     arene::base::extents<TypeParam, 1, 2, 3, 4>,
                     arene::base::extents<std::uint8_t, 1, 2, 3, 4>>::value);
  STATIC_ASSERT_TRUE(std::is_nothrow_constructible<
                     arene::base::extents<TypeParam, 1, 2, arene::base::dynamic_extent, 4>,
                     arene::base::extents<std::uint8_t, 1, 2, arene::base::dynamic_extent, 4>>::value);
}

template <typename T1, typename T2>
extern constexpr bool max_is_greater_or_equal_v =
    arene::base::cmp_greater_equal(std::numeric_limits<T1>::max(), std::numeric_limits<T2>::max());

/// @test Can implicitly construct an @c extents object from another if the source type has a smaller index type
TYPED_TEST(ExtentsTypedTest, CanImplicitlyConstructFromAnotherExtentsObjectIfSourceTypeHasSmallerIndex) {
  ASSERT_EQ(
      (std::is_convertible<
          arene::base::extents<std::uint8_t, 1, 2, 3, 4>,
          arene::base::extents<TypeParam, 1, 2, 3, 4>>::value),
      (max_is_greater_or_equal_v<TypeParam, std::uint8_t>)
  );
  ASSERT_EQ(
      (std::is_convertible<
          arene::base::extents<std::uint8_t, 1, 2, 3, arene::base::dynamic_extent>,
          arene::base::extents<TypeParam, 1, 2, 3, arene::base::dynamic_extent>>::value),
      (max_is_greater_or_equal_v<TypeParam, std::uint8_t>)
  );
  ASSERT_EQ(
      (std::is_convertible<arene::base::extents<std::int8_t, 1, 2, 3, 4>, arene::base::extents<TypeParam, 1, 2, 3, 4>>::
           value),
      (max_is_greater_or_equal_v<TypeParam, std::int8_t>)
  );
  ASSERT_EQ(
      (std::is_convertible<
          arene::base::extents<std::int8_t, 1, 2, 3, arene::base::dynamic_extent>,
          arene::base::extents<TypeParam, 1, 2, 3, arene::base::dynamic_extent>>::value),
      (max_is_greater_or_equal_v<TypeParam, std::int8_t>)
  );
  ASSERT_EQ(
      (std::is_convertible<
          arene::base::extents<std::uint16_t, 1, 2, 3, 4>,
          arene::base::extents<TypeParam, 1, 2, 3, 4>>::value),
      (max_is_greater_or_equal_v<TypeParam, std::uint16_t>)
  );
  ASSERT_EQ(
      (std::is_convertible<
          arene::base::extents<std::uint16_t, 1, 2, 3, arene::base::dynamic_extent>,
          arene::base::extents<TypeParam, 1, 2, 3, arene::base::dynamic_extent>>::value),
      (max_is_greater_or_equal_v<TypeParam, std::uint16_t>)
  );
  ASSERT_EQ(
      (std::is_convertible<
          arene::base::extents<std::int16_t, 1, 2, 3, 4>,
          arene::base::extents<TypeParam, 1, 2, 3, 4>>::value),
      (max_is_greater_or_equal_v<TypeParam, std::int16_t>)
  );
  ASSERT_EQ(
      (std::is_convertible<
          arene::base::extents<std::int16_t, 1, 2, 3, arene::base::dynamic_extent>,
          arene::base::extents<TypeParam, 1, 2, 3, arene::base::dynamic_extent>>::value),
      (max_is_greater_or_equal_v<TypeParam, std::int16_t>)
  );
  ASSERT_EQ(
      (std::is_convertible<
          arene::base::extents<std::uint32_t, 1, 2, 3, 4>,
          arene::base::extents<TypeParam, 1, 2, 3, 4>>::value),
      (max_is_greater_or_equal_v<TypeParam, std::uint32_t>)
  );
  ASSERT_EQ(
      (std::is_convertible<
          arene::base::extents<std::uint32_t, 1, 2, 3, arene::base::dynamic_extent>,
          arene::base::extents<TypeParam, 1, 2, 3, arene::base::dynamic_extent>>::value),
      (max_is_greater_or_equal_v<TypeParam, std::uint32_t>)
  );
  ASSERT_EQ(
      (std::is_convertible<
          arene::base::extents<std::int32_t, 1, 2, 3, 4>,
          arene::base::extents<TypeParam, 1, 2, 3, 4>>::value),
      (max_is_greater_or_equal_v<TypeParam, std::int32_t>)
  );
  ASSERT_EQ(
      (std::is_convertible<
          arene::base::extents<std::int32_t, 1, 2, 3, arene::base::dynamic_extent>,
          arene::base::extents<TypeParam, 1, 2, 3, arene::base::dynamic_extent>>::value),
      (max_is_greater_or_equal_v<TypeParam, std::int32_t>)
  );
  ASSERT_EQ(
      (std::is_convertible<
          arene::base::extents<std::uint64_t, 1, 2, 3, 4>,
          arene::base::extents<TypeParam, 1, 2, 3, 4>>::value),
      (max_is_greater_or_equal_v<TypeParam, std::uint64_t>)
  );
  ASSERT_EQ(
      (std::is_convertible<
          arene::base::extents<std::uint64_t, 1, 2, 3, arene::base::dynamic_extent>,
          arene::base::extents<TypeParam, 1, 2, 3, arene::base::dynamic_extent>>::value),
      (max_is_greater_or_equal_v<TypeParam, std::uint64_t>)
  );
  ASSERT_EQ(
      (std::is_convertible<
          arene::base::extents<std::int64_t, 1, 2, 3, 4>,
          arene::base::extents<TypeParam, 1, 2, 3, 4>>::value),
      (max_is_greater_or_equal_v<TypeParam, std::int64_t>)
  );
  ASSERT_EQ(
      (std::is_convertible<
          arene::base::extents<std::int64_t, 1, 2, 3, arene::base::dynamic_extent>,
          arene::base::extents<TypeParam, 1, 2, 3, arene::base::dynamic_extent>>::value),
      (max_is_greater_or_equal_v<TypeParam, std::int64_t>)
  );
}

/// @test Cannot implicitly construct an @c extents object from another if the source type has a dynamic extent where
/// the target has a fixed extent
TYPED_TEST(ExtentsTypedTest, CannotImplicitlyConstructFromAnotherExtentsObjectIfSourceHasDynamicExtentForFixedExtent) {
  ASSERT_FALSE((std::is_convertible<
                arene::base::extents<std::uint8_t, 1, arene::base::dynamic_extent, 3, 4>,
                arene::base::extents<TypeParam, 1, 2, 3, 4>>::value));
  ASSERT_FALSE((std::is_convertible<
                arene::base::extents<std::uint8_t, 1, arene::base::dynamic_extent, 3, arene::base::dynamic_extent>,
                arene::base::extents<TypeParam, 1, 2, 3, arene::base::dynamic_extent>>::value));
  ASSERT_FALSE((std::is_convertible<
                arene::base::extents<std::int8_t, 1, arene::base::dynamic_extent, 3, 4>,
                arene::base::extents<TypeParam, 1, 2, 3, 4>>::value));
  ASSERT_FALSE((std::is_convertible<
                arene::base::extents<std::int8_t, 1, arene::base::dynamic_extent, 3, arene::base::dynamic_extent>,
                arene::base::extents<TypeParam, 1, 2, 3, arene::base::dynamic_extent>>::value));

  ASSERT_FALSE((std::is_convertible<
                arene::base::extents<std::uint16_t, 1, arene::base::dynamic_extent, 3, 4>,
                arene::base::extents<TypeParam, 1, 2, 3, 4>>::value));
  ASSERT_FALSE((std::is_convertible<
                arene::base::extents<std::uint16_t, 1, arene::base::dynamic_extent, 3, arene::base::dynamic_extent>,
                arene::base::extents<TypeParam, 1, 2, 3, arene::base::dynamic_extent>>::value));
  ASSERT_FALSE((std::is_convertible<
                arene::base::extents<std::int16_t, 1, arene::base::dynamic_extent, 3, 4>,
                arene::base::extents<TypeParam, 1, 2, 3, 4>>::value));
  ASSERT_FALSE((std::is_convertible<
                arene::base::extents<std::int16_t, 1, arene::base::dynamic_extent, 3, arene::base::dynamic_extent>,
                arene::base::extents<TypeParam, 1, 2, 3, arene::base::dynamic_extent>>::value));
  ASSERT_FALSE((std::is_convertible<
                arene::base::extents<std::uint32_t, 1, arene::base::dynamic_extent, 3, 4>,
                arene::base::extents<TypeParam, 1, 2, 3, 4>>::value));
  ASSERT_FALSE((std::is_convertible<
                arene::base::extents<std::uint32_t, 1, arene::base::dynamic_extent, 3, arene::base::dynamic_extent>,
                arene::base::extents<TypeParam, 1, 2, 3, arene::base::dynamic_extent>>::value));
  ASSERT_FALSE((std::is_convertible<
                arene::base::extents<std::int32_t, 1, arene::base::dynamic_extent, 3, 4>,
                arene::base::extents<TypeParam, 1, 2, 3, 4>>::value));
  ASSERT_FALSE((std::is_convertible<
                arene::base::extents<std::int32_t, 1, arene::base::dynamic_extent, 3, arene::base::dynamic_extent>,
                arene::base::extents<TypeParam, 1, 2, 3, arene::base::dynamic_extent>>::value));
  ASSERT_FALSE((std::is_convertible<
                arene::base::extents<std::uint64_t, 1, arene::base::dynamic_extent, 3, 4>,
                arene::base::extents<TypeParam, 1, 2, 3, 4>>::value));
  ASSERT_FALSE((std::is_convertible<
                arene::base::extents<std::uint64_t, 1, arene::base::dynamic_extent, 3, arene::base::dynamic_extent>,
                arene::base::extents<TypeParam, 1, 2, 3, arene::base::dynamic_extent>>::value));
  ASSERT_FALSE((std::is_convertible<
                arene::base::extents<std::int64_t, 1, arene::base::dynamic_extent, 3, 4>,
                arene::base::extents<TypeParam, 1, 2, 3, 4>>::value));
  ASSERT_FALSE((std::is_convertible<
                arene::base::extents<std::int64_t, 1, arene::base::dynamic_extent, 3, arene::base::dynamic_extent>,
                arene::base::extents<TypeParam, 1, 2, 3, arene::base::dynamic_extent>>::value));
}

/// @test Can construct an @c extents object from another if the source type has a dynamic extent where the source has
/// a fixed extent
TYPED_TEST(ExtentsTypedTest, CanConstructFromAnotherExtentsObjectWithDynamicInsteadOfFixed) {
  STATIC_ASSERT_TRUE(std::is_constructible<
                     arene::base::extents<TypeParam, 1, 2, 3, 4>,
                     arene::base::extents<std::uint8_t, 1, 2, 3, arene::base::dynamic_extent>>::value);
  STATIC_ASSERT_TRUE(
      std::is_constructible<
          arene::base::extents<TypeParam, 1, 2, arene::base::dynamic_extent, 4>,
          arene::base::extents<std::uint8_t, 1, arene::base::dynamic_extent, arene::base::dynamic_extent, 4>>::value
  );
  STATIC_ASSERT_TRUE(std::is_nothrow_constructible<
                     arene::base::extents<TypeParam, 1, 2, 3, 4>,
                     arene::base::extents<std::uint8_t, 1, 2, 3, arene::base::dynamic_extent>>::value);
  STATIC_ASSERT_TRUE(
      std::is_nothrow_constructible<
          arene::base::extents<TypeParam, 1, 2, arene::base::dynamic_extent, 4>,
          arene::base::extents<std::uint8_t, 1, arene::base::dynamic_extent, arene::base::dynamic_extent, 4>>::value
  );
}

/// @test Can construct an @c extents object from another if the source type has a fixed extent where the source has a
/// dynamic extent
TYPED_TEST(ExtentsTypedTest, CanConstructFromAnotherExtentsObjectWithFixedInsteadOfDynamic) {
  STATIC_ASSERT_TRUE(std::is_constructible<
                     arene::base::extents<TypeParam, 1, 2, 3, arene::base::dynamic_extent>,
                     arene::base::extents<std::uint8_t, 1, 2, 3, 4>>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<
                     arene::base::extents<TypeParam, 1, arene::base::dynamic_extent, arene::base::dynamic_extent, 4>,
                     arene::base::extents<std::uint8_t, 1, 2, arene::base::dynamic_extent, 4>>::value);
}

/// @test Cannot construct an @c extents object from another if the source type has a fixed extent where the source
/// has a different fixed extent
TYPED_TEST(ExtentsTypedTest, CannotConstructFromAnotherExtentsObjectIfFixedExtentsDoNotMatch) {
  STATIC_ASSERT_FALSE(std::is_constructible<
                      arene::base::extents<TypeParam, 1, 2, 3, 4>,
                      arene::base::extents<std::uint8_t, 1, 2, 3, 99>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<
                      arene::base::extents<TypeParam, 1, 2, arene::base::dynamic_extent, 4>,
                      arene::base::extents<std::uint8_t, 1, 99, arene::base::dynamic_extent, 4>>::value);
}

/// @test Cannot construct an @c extents object from another if the source type has the wrong number of extents
TYPED_TEST(ExtentsTypedTest, CannotConstructFromAnotherExtentsObjectWithWrongNumberOfExtents) {
  STATIC_ASSERT_FALSE(
      std::is_constructible<arene::base::extents<TypeParam, 1, 2, 3, 4>, arene::base::extents<std::uint8_t, 1, 2, 3>>::
          value
  );
  STATIC_ASSERT_FALSE(std::is_constructible<
                      arene::base::extents<TypeParam, 1, 2, 3, 4>,
                      arene::base::extents<std::uint8_t, 1, 2, 3, 4, 5>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<
                      arene::base::extents<TypeParam, 1, 2, arene::base::dynamic_extent, 4>,
                      arene::base::extents<std::uint8_t, 1, 2, arene::base::dynamic_extent, 4, 5>>::value);
  STATIC_ASSERT_FALSE(std::is_constructible<
                      arene::base::extents<TypeParam, 1, 2, arene::base::dynamic_extent, 4>,
                      arene::base::extents<std::uint8_t, 1, 2, arene::base::dynamic_extent>>::value);
}

/// @test Constructing an @c extents object from another copies the dynamic extents values from the source
TYPED_TEST(ExtentsTypedTest, ConstructingFromAnotherExtentObjectCopiesTheExtentsForDynamicExtents) {
  using source_type1 = arene::base::extents<
      std::uint8_t,
      1,
      2,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6>;
  using target_type1 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      2,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6>;

  constexpr std::int32_t chosen1 = 30;
  constexpr std::int16_t chosen2 = 40;
  constexpr std::uint64_t chosen3 = 50;
  source_type1 const source1{chosen1, chosen2, chosen3};

  target_type1 const target1{source1};

  ASSERT_EQ(target1.extent(0), 1);
  ASSERT_EQ(target1.extent(1), 2);
  ASSERT_EQ(target1.extent(2), chosen1);
  ASSERT_EQ(target1.extent(3), chosen2);
  ASSERT_EQ(target1.extent(4), chosen3);
  ASSERT_EQ(target1.extent(5), 6);

  using target_type2 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      2,
      arene::base::dynamic_extent,
      chosen2,
      arene::base::dynamic_extent,
      6>;

  target_type2 const target2{source1};

  ASSERT_EQ(target2.extent(0), 1);
  ASSERT_EQ(target2.extent(1), 2);
  ASSERT_EQ(target2.extent(2), chosen1);
  ASSERT_EQ(target2.extent(3), chosen2);
  ASSERT_EQ(target2.extent(4), chosen3);
  ASSERT_EQ(target2.extent(5), 6);

  using target_type3 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      2,
      arene::base::dynamic_extent,
      chosen2 + 1,
      arene::base::dynamic_extent,
      6>;

  source_type1 const source2{chosen1, chosen2 + 1, chosen3};
  // NOLINTNEXTLINE(misc-const-correctness)
  target_type3 target3{source2};
  ASSERT_EQ(target3.extent(0), 1);
  ASSERT_EQ(target3.extent(1), 2);
  ASSERT_EQ(target3.extent(2), chosen1);
  ASSERT_EQ(target3.extent(3), chosen2 + 1);
  ASSERT_EQ(target3.extent(4), chosen3);
  ASSERT_EQ(target3.extent(5), 6);

  using target_type4 = arene::base::extents<
      TypeParam,
      arene::base::dynamic_extent,
      2,
      3,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6>;
  source_type1 const source3{3, chosen2, chosen3};

  // NOLINTNEXTLINE(misc-const-correctness)
  target_type4 target4{source3};
  ASSERT_EQ(target4.extent(0), 1);
  ASSERT_EQ(target4.extent(1), 2);
  ASSERT_EQ(target4.extent(2), 3);
  ASSERT_EQ(target4.extent(3), chosen2);
  ASSERT_EQ(target4.extent(4), chosen3);
  ASSERT_EQ(target4.extent(5), 6);

  constexpr source_type1 source4{3, chosen2, chosen3};

  // NOLINTNEXTLINE(misc-const-correctness)
  constexpr target_type4 target5{source4};
  ASSERT_EQ(target5.extent(0), 1);
  ASSERT_EQ(target5.extent(1), 2);
  ASSERT_EQ(target5.extent(2), 3);
  ASSERT_EQ(target5.extent(3), chosen2);
  ASSERT_EQ(target5.extent(4), chosen3);
  ASSERT_EQ(target5.extent(5), 6);

  using source_type2 = arene::base::extents<TypeParam, 1, 2, 3, chosen2, chosen3, 6>;

  constexpr source_type2 source5{};
  // NOLINTNEXTLINE(misc-const-correctness)
  constexpr target_type4 target6{source5};
  ASSERT_EQ(target6.extent(0), 1);
  ASSERT_EQ(target6.extent(1), 2);
  ASSERT_EQ(target6.extent(2), 3);
  ASSERT_EQ(target6.extent(3), chosen2);
  ASSERT_EQ(target6.extent(4), chosen3);
  ASSERT_EQ(target6.extent(5), 6);

  using target_type5 = arene::base::extents<TypeParam, 1, 2, chosen1, chosen2 + 1, chosen3, 6>;

  target_type5 target7{source2};
  ASSERT_EQ(target7.extent(0), 1);
  ASSERT_EQ(target7.extent(1), 2);
  ASSERT_EQ(target7.extent(2), chosen1);
  ASSERT_EQ(target7.extent(3), chosen2 + 1);
  ASSERT_EQ(target7.extent(4), chosen3);
  ASSERT_EQ(target7.extent(5), 6);

  target7 = target_type5{source2};

  ASSERT_EQ(target7.extent(0), 1);
  ASSERT_EQ(target7.extent(1), 2);
  ASSERT_EQ(target7.extent(2), chosen1);
  ASSERT_EQ(target7.extent(3), chosen2 + 1);
  ASSERT_EQ(target7.extent(4), chosen3);
  ASSERT_EQ(target7.extent(5), 6);

  using target_type8 = arene::base::extents<TypeParam, 1, 2, chosen1, chosen2, chosen3, 3>;
  target_type8 target8{};
  ASSERT_EQ(target8.extent(0), 1);
  ASSERT_EQ(target8.extent(1), 2);
  ASSERT_EQ(target8.extent(2), chosen1);
  ASSERT_EQ(target8.extent(3), chosen2);
  ASSERT_EQ(target8.extent(4), chosen3);
  ASSERT_EQ(target8.extent(5), 3);

  target8 = target_type8{};
  ASSERT_EQ(target8.extent(0), 1);
  ASSERT_EQ(target8.extent(1), 2);
  ASSERT_EQ(target8.extent(2), chosen1);
  ASSERT_EQ(target8.extent(3), chosen2);
  ASSERT_EQ(target8.extent(4), chosen3);
  ASSERT_EQ(target8.extent(5), 3);

  source_type1 const source6{chosen1, chosen2, chosen3 + 1};
  using target_type9 = arene::base::extents<TypeParam, 1, 2, chosen1, chosen2, chosen3 + 1, 6>;
  target_type9 target9{source6};
  ASSERT_EQ(target9.extent(0), 1);
  ASSERT_EQ(target9.extent(1), 2);
  ASSERT_EQ(target9.extent(2), chosen1);
  ASSERT_EQ(target9.extent(3), chosen2);
  ASSERT_EQ(target9.extent(4), chosen3 + 1);
  ASSERT_EQ(target9.extent(5), 6);

  target9 = target_type9{source6};
  ASSERT_EQ(target9.extent(0), 1);
  ASSERT_EQ(target9.extent(1), 2);
  ASSERT_EQ(target9.extent(2), chosen1);
  ASSERT_EQ(target9.extent(3), chosen2);
  ASSERT_EQ(target9.extent(4), chosen3 + 1);
  ASSERT_EQ(target9.extent(5), 6);

  using target_type10 = arene::base::extents<TypeParam, 1, 2, chosen1 + 1, chosen2, chosen3, 6>;

  source_type1 const source7{chosen1 + 1, chosen2, chosen3};
  target_type10 target10{source7};
  ASSERT_EQ(target10.extent(0), 1);
  ASSERT_EQ(target10.extent(1), 2);
  ASSERT_EQ(target10.extent(2), chosen1 + 1);
  ASSERT_EQ(target10.extent(3), chosen2);
  ASSERT_EQ(target10.extent(4), chosen3);
  ASSERT_EQ(target10.extent(5), 6);

  target10 = target_type10{source7};
  ASSERT_EQ(target10.extent(0), 1);
  ASSERT_EQ(target10.extent(1), 2);
  ASSERT_EQ(target10.extent(2), chosen1 + 1);
  ASSERT_EQ(target10.extent(3), chosen2);
  ASSERT_EQ(target10.extent(4), chosen3);
  ASSERT_EQ(target10.extent(5), 6);
}

/// @test Constructing an @c extents object from another is OK if the dynamic extents values from the source match the
/// fixed extents
TYPED_TEST(ExtentsTypedTest, ConstructingFromAnotherExtentObjectIsOKIfTheSourceDynamicExtentsMatchTargetFixedExtents) {
  using source_type1 = arene::base::extents<
      std::uint8_t,
      1,
      2,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6>;

  constexpr std::int32_t chosen1 = 30;
  constexpr std::int16_t chosen2 = 41;
  constexpr std::uint64_t chosen3 = 50;
  using target_type5 = arene::base::extents<TypeParam, 1, 2, chosen1, chosen2, chosen3, 6>;

  source_type1 const source2{chosen1, chosen2, chosen3};
  // NOLINTNEXTLINE(misc-const-correctness)
  target_type5 target5{source2};
  ASSERT_EQ(target5.extent(0), 1);
  ASSERT_EQ(target5.extent(1), 2);
  ASSERT_EQ(target5.extent(2), chosen1);
  ASSERT_EQ(target5.extent(3), chosen2);
  ASSERT_EQ(target5.extent(4), chosen3);
  ASSERT_EQ(target5.extent(5), 6);

  constexpr source_type1 source3{chosen1, chosen2, chosen3};
  constexpr target_type5 target6{source3};
  ASSERT_EQ(target6.extent(0), 1);
  ASSERT_EQ(target6.extent(1), 2);
  ASSERT_EQ(target6.extent(2), chosen1);
  ASSERT_EQ(target6.extent(3), chosen2);
  ASSERT_EQ(target6.extent(4), chosen3);
  ASSERT_EQ(target6.extent(5), 6);
}

/// @test The template parameters for @c extents are deduced when constructing with @c make_extents
TYPED_TEST(ExtentsTypedTest, MakeExtentsDeducesTemplateArgs) {
  constexpr auto empty_extents = arene::base::make_extents();
  ::testing::StaticAssertTypeEq<decltype(empty_extents), arene::base::extents<std::size_t> const>();

  constexpr auto extents_42 = arene::base::make_extents(42);
  ::testing::StaticAssertTypeEq<decltype(extents_42), arene::base::dextents<std::size_t, 1> const>();

  ASSERT_EQ(extents_42.extent(0), 42);

  constexpr auto extents_99_42_123 = arene::base::make_extents(99, 42, 123);
  ::testing::StaticAssertTypeEq<decltype(extents_99_42_123), arene::base::dextents<std::size_t, 3> const>();

  ASSERT_EQ(extents_99_42_123.extent(0), 99);
  ASSERT_EQ(extents_99_42_123.extent(1), 42);
  ASSERT_EQ(extents_99_42_123.extent(2), 123);
}

/// @test The static extents for @c extents are deduced when constructing with @c make_extents using @c
/// integral_constant
TYPED_TEST(ExtentsTypedTest, MakeExtentsDeducesTemplateArgsFromIntegralConstant) {
  ::testing::StaticAssertTypeEq<
      decltype(arene::base::make_extents(std::integral_constant<int, 42>{})),
      arene::base::extents<std::size_t, 42>>();
  ::testing::StaticAssertTypeEq<
      decltype(arene::base::
                   make_extents(std::integral_constant<int, 42>{}, std::integral_constant<std::uint64_t, 1234>{})),
      arene::base::extents<std::size_t, 42, 1234>>();
}

template <class Type, Type Value>
struct integral_constant_like {
  static constexpr auto value = Value;
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr operator Type() const noexcept { return value; }
};

/// @test The static extents for @c extents are deduced when constructing with @c make_extents using
/// integral-constant-likes
TYPED_TEST(ExtentsTypedTest, MakeExtentsDeducesTemplateArgsFromIntegralConstantLikes) {
  ::testing::StaticAssertTypeEq<
      decltype(arene::base::make_extents(integral_constant_like<int, 42>{})),
      arene::base::extents<std::size_t, 42>>();
  ::testing::StaticAssertTypeEq<
      decltype(arene::base::
                   make_extents(std::integral_constant<int, 42>{}, integral_constant_like<std::uint64_t, 1234>{})),
      arene::base::extents<std::size_t, 42, 1234>>();
}

/// @test The static extents for @c extents are deduced when constructing with @c make_extents using a mix of @c
/// integral_constant values and normal integers
TYPED_TEST(ExtentsTypedTest, MakeExtentsDeducesTemplateArgsFromMixOfIntegralConstantsAndIntegers) {
  constexpr auto mixed_extents = arene::base::make_extents(
      std::integral_constant<int, 42>{},
      99,
      123,
      std::integral_constant<std::uint64_t, 1234>{}
  );

  ::testing::StaticAssertTypeEq<
      decltype(mixed_extents),
      arene::base::extents<std::size_t, 42, arene::base::dynamic_extent, arene::base::dynamic_extent, 1234> const>();

  ASSERT_EQ(mixed_extents.extent(0), 42);
  ASSERT_EQ(mixed_extents.extent(1), 99);
  ASSERT_EQ(mixed_extents.extent(2), 123);
  ASSERT_EQ(mixed_extents.extent(3), 1234);
}

class convertible_to_size_t {
  std::size_t value_;

 public:
  explicit convertible_to_size_t(std::size_t value)
      : value_(value) {}

  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  operator std::size_t() const noexcept { return value_; }
};

/// @test The static extents for @c extents are deduced when constructing with @c make_extents using a mix of @c
/// integral_constant values, normal integers and types-convertible-to-integers
TYPED_TEST(ExtentsTypedTest, MakeExtentsDeducesTemplateArgsFromMixOfIntegralConstantsIntegersAndConvertibleTypes) {
  auto const mixed_extents = arene::base::make_extents(
      std::integral_constant<int, 42>{},
      99,
      convertible_to_size_t(123),
      std::integral_constant<std::uint64_t, 1234>{}
  );

  ::testing::StaticAssertTypeEq<
      decltype(mixed_extents),
      arene::base::extents<std::size_t, 42, arene::base::dynamic_extent, arene::base::dynamic_extent, 1234> const>();

  ASSERT_EQ(mixed_extents.extent(0), 42);
  ASSERT_EQ(mixed_extents.extent(1), 99);
  ASSERT_EQ(mixed_extents.extent(2), 123);
  ASSERT_EQ(mixed_extents.extent(3), 1234);
}

template <typename SourceType, typename DestType>
auto check_assignment_equality() -> bool {
  SourceType source{};
  DestType dest{source};

  if (dest != source) {
    return false;
  }
  if (source != dest) {
    return false;
  }
  source = SourceType{dest};
  if (dest != source) {
    return false;
  }
  if (source != dest) {
    return false;
  }

  return true;
}

template <typename SourceType, std::size_t... Extents, typename... DestTypes>
auto check_all_assignment_equality(::testing::Types<DestTypes...>, std::index_sequence<Extents...>) -> bool {
  return arene::base::all_of({check_assignment_equality<
      arene::base::extents<SourceType, Extents...>,
      arene::base::extents<DestTypes, Extents...>>()...});
}

/// @test Extents objects compare equal if they have the same rank and values
TYPED_TEST(ExtentsTypedTest, ExtentsAreEqualIfTheyHaveSameValues) {
  using source_type = arene::base::extents<
      std::uint8_t,
      1,
      2,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6>;

  constexpr std::int32_t chosen1 = 30;
  constexpr std::int16_t chosen2 = 41;
  constexpr std::uint64_t chosen3 = 50;
  using target_type = arene::base::extents<TypeParam, 1, 2, chosen1, chosen2, chosen3, 6>;

  source_type const source{chosen1, chosen2, chosen3};
  // NOLINTNEXTLINE(misc-const-correctness)
  target_type target{};
  ASSERT_EQ(source, source);
  ASSERT_EQ(target, target);
  ASSERT_EQ(source, target);
  ASSERT_EQ(target, source);

  ASSERT_FALSE(source != source);
  ASSERT_FALSE(target != target);
  ASSERT_FALSE(source != target);
  ASSERT_FALSE(target != source);

  ASSERT_TRUE(check_all_assignment_equality<TypeParam>(SupportedIndexTypes{}, std::index_sequence<14, 4, 2, 87, 3>{}));
}

/// @test Extents objects compare not equal if they have different rank
TYPED_TEST(ExtentsTypedTest, ExtentsAreNotEqualIfTheyHaveDifferentRank) {
  using source_type6 = arene::base::extents<
      std::uint8_t,
      1,
      2,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6>;

  constexpr std::int32_t chosen1 = 30;
  constexpr std::int16_t chosen2 = 41;
  constexpr std::uint64_t chosen3 = 50;
  using target_type5 = arene::base::extents<TypeParam, 1, 2, chosen1, chosen2, chosen3>;
  using target_type6 = arene::base::extents<TypeParam, 1, 2, chosen1, chosen2, chosen3, 3>;

  source_type6 const source2{chosen1, chosen2, chosen3};
  target_type5 const target5{};
  target_type6 const target6{};
  ASSERT_EQ(source2, source2);
  ASSERT_EQ(target5, target5);
  ASSERT_EQ(target6, target6);
  ASSERT_NE(source2, target5);
  ASSERT_NE(target5, source2);
  ASSERT_NE(target5, target6);
  ASSERT_NE(target6, target5);
}

/// @test Extents objects compare not equal if they have different values
TYPED_TEST(ExtentsTypedTest, ExtentsAreNotEqualIfTheyHaveDifferentValuesAndSameRank) {
  using source_type = arene::base::extents<
      std::uint8_t,
      1,
      2,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6>;

  constexpr std::int32_t chosen1 = 30;
  constexpr std::int16_t chosen2 = 41;
  constexpr std::uint64_t chosen3 = 50;
  using target_type1 = arene::base::extents<TypeParam, 1, 2, chosen1, chosen2, chosen3 + 1, 6>;
  using target_type2 = arene::base::extents<TypeParam, 1, 2, chosen1 + 1, chosen2, chosen3, 6>;

  source_type const source1{chosen1, chosen2, chosen3};
  source_type const source2{chosen1, chosen2 + 1, chosen3};
  target_type1 const target1{};
  target_type2 const target2{};
  ASSERT_EQ(source1, source1);
  ASSERT_EQ(source2, source2);
  ASSERT_EQ(target1, target1);
  ASSERT_EQ(target2, target2);
  ASSERT_NE(source1, source2);
  ASSERT_NE(target1, source1);
  ASSERT_NE(source1, target1);
  ASSERT_NE(target1, target2);
  ASSERT_NE(target2, target1);
  ASSERT_NE(target2, source1);
  ASSERT_NE(source1, target2);
}

/// @test Equality comparison of @c extents is @c noexcept
TYPED_TEST(ExtentsTypedTest, EqualityIsNoexcept) {
  using source_type = arene::base::extents<
      std::uint8_t,
      1,
      2,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6>;
  constexpr std::int32_t chosen1 = 30;
  constexpr std::int16_t chosen2 = 41;
  constexpr std::uint64_t chosen3 = 50;

  using target_type5 = arene::base::extents<TypeParam, 1, 2, chosen1, chosen2, chosen3>;
  using target_type6 = arene::base::extents<TypeParam, 1, 2, chosen1, chosen2, chosen3, 3>;

  ASSERT_TRUE(arene::base::is_nothrow_equality_comparable_v<source_type>);
  ASSERT_TRUE(arene::base::is_nothrow_equality_comparable_v<target_type5>);
  ASSERT_TRUE(arene::base::is_nothrow_equality_comparable_v<target_type6>);
  ASSERT_TRUE((arene::base::is_nothrow_equality_comparable_v<source_type, target_type5>));
  ASSERT_TRUE((arene::base::is_nothrow_equality_comparable_v<source_type, target_type6>));
  ASSERT_TRUE((arene::base::is_nothrow_equality_comparable_v<target_type5, source_type>));
  ASSERT_TRUE((arene::base::is_nothrow_equality_comparable_v<target_type6, source_type>));

  ASSERT_TRUE(arene::base::is_nothrow_inequality_comparable_v<source_type>);
  ASSERT_TRUE(arene::base::is_nothrow_inequality_comparable_v<target_type5>);
  ASSERT_TRUE(arene::base::is_nothrow_inequality_comparable_v<target_type6>);
  ASSERT_TRUE((arene::base::is_nothrow_inequality_comparable_v<source_type, target_type5>));
  ASSERT_TRUE((arene::base::is_nothrow_inequality_comparable_v<source_type, target_type6>));
  ASSERT_TRUE((arene::base::is_nothrow_inequality_comparable_v<target_type5, source_type>));
  ASSERT_TRUE((arene::base::is_nothrow_inequality_comparable_v<target_type6, source_type>));
}

/// @test Extents objects compare not equal if they have different values
CONSTEXPR_TYPED_TEST(ExtentsTypedTest, ExtentsAreNotEqualIfTheyHaveDifferentValuesAndSameRankInConstexpr) {
  using source_type = arene::base::extents<
      std::uint8_t,
      1,
      2,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      arene::base::dynamic_extent,
      6>;

  constexpr std::int32_t chosen1 = 30;
  constexpr std::int16_t chosen2 = 41;
  constexpr std::uint64_t chosen3 = 50;
  using target_type1 = arene::base::extents<TypeParam, 1, 2, chosen1, chosen2, chosen3 + 1, 6>;
  using target_type2 = arene::base::extents<TypeParam, 1, 2, chosen1 + 1, chosen2, chosen3, 6>;

  source_type const source1{chosen1, chosen2, chosen3};
  source_type const source2{chosen1, chosen2 + 1, chosen3};
  target_type1 const target1{};
  target_type2 const target2{};
  CONSTEXPR_ASSERT_EQ(source1, source1);
  CONSTEXPR_ASSERT_EQ(source2, source2);
  CONSTEXPR_ASSERT_EQ(target1, target1);
  CONSTEXPR_ASSERT_EQ(target2, target2);
  CONSTEXPR_ASSERT(source1 != source2);
  CONSTEXPR_ASSERT(target1 != source1);
  CONSTEXPR_ASSERT(source1 != target1);
  CONSTEXPR_ASSERT(target1 != target2);
  CONSTEXPR_ASSERT(target2 != target1);
  CONSTEXPR_ASSERT(target2 != source1);
  CONSTEXPR_ASSERT(source1 != target2);
}

}  // namespace
