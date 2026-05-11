// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/type_list/remove_duplicates.hpp"
#include "stdlib/include/cstdint"
#include "stdlib/include/type_traits"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"

namespace {
using ::testing::StaticAssertTypeEq;

using integer_sequence_test_types =                                 //
    arene::base::type_lists::remove_duplicates_t<::testing::Types<  //
        char,
        unsigned char,
        signed char,
        int,
        unsigned,
        std::int8_t,
        std::uint8_t,
        std::int16_t,
        std::uint16_t,
        std::int32_t,
        std::uint32_t,
        std::int64_t,
        std::uint64_t>>;

template <class T>
struct IntegerSequenceTest : testing::Test {};

TYPED_TEST_SUITE(IntegerSequenceTest, integer_sequence_test_types, );

/// @test @c integer_sequence has a @c value_type member type alias
TYPED_TEST(IntegerSequenceTest, HasValueTypeMemberTypeAlias) {
  StaticAssertTypeEq<TypeParam, typename std::integer_sequence<TypeParam>::value_type>();
  StaticAssertTypeEq<TypeParam, typename std::integer_sequence<TypeParam, 0>::value_type>();
  StaticAssertTypeEq<TypeParam, typename std::integer_sequence<TypeParam, 0, 0>::value_type>();
  StaticAssertTypeEq<TypeParam, typename std::integer_sequence<TypeParam, 0, 0, 0>::value_type>();
}

/// @test @c integer_sequence has a @c value_type member type alias if the integer type is @c bool
TEST(BoolIntegerSequenceTest, HasValueTypeMemberTypeAlias) {
  StaticAssertTypeEq<bool, std::integer_sequence<bool>::value_type>();
  StaticAssertTypeEq<bool, std::integer_sequence<bool, false>::value_type>();
  StaticAssertTypeEq<bool, std::integer_sequence<bool, false, false>::value_type>();
  StaticAssertTypeEq<bool, std::integer_sequence<bool, false, false, false>::value_type>();
}

/// @test The static member function @c size of @c integer_sequence returns the number of elements
CONSTEXPR_TYPED_TEST(IntegerSequenceTest, SizeMemberReturnsNumberOfElements) {
  ASSERT_EQ(0, (std::integer_sequence<TypeParam>::size()));
  ASSERT_EQ(1, (std::integer_sequence<TypeParam, 0>::size()));
  ASSERT_EQ(1, (std::integer_sequence<TypeParam, 1>::size()));
  ASSERT_EQ(1, (std::integer_sequence<TypeParam, 2>::size()));
  ASSERT_EQ(1, (std::integer_sequence<TypeParam, 3>::size()));
  ASSERT_EQ(2, (std::integer_sequence<TypeParam, 0, 1>::size()));
  ASSERT_EQ(2, (std::integer_sequence<TypeParam, 1, 0>::size()));
  ASSERT_EQ(2, (std::integer_sequence<TypeParam, 1, 1>::size()));
  ASSERT_EQ(2, (std::integer_sequence<TypeParam, 2, 3>::size()));
  ASSERT_EQ(3, (std::integer_sequence<TypeParam, 0, 1, 2>::size()));
  ASSERT_EQ(3, (std::integer_sequence<TypeParam, 2, 1, 2>::size()));
  ASSERT_EQ(3, (std::integer_sequence<TypeParam, 0, 0, 0>::size()));
  ASSERT_EQ(3, (std::integer_sequence<TypeParam, 1, 2, 1>::size()));
}

/// @test The static member function @c size of @c integer_sequence returns the number of elements if the integer type
/// is @c bool
TEST(BoolIntegerSequenceTest, SizeMemberReturnsNumberOfElements) {
  ASSERT_EQ(0, (std::integer_sequence<bool>::size()));
  ASSERT_EQ(1, (std::integer_sequence<bool, false>::size()));
  ASSERT_EQ(1, (std::integer_sequence<bool, true>::size()));
  ASSERT_EQ(2, (std::integer_sequence<bool, false, false>::size()));
  ASSERT_EQ(2, (std::integer_sequence<bool, false, true>::size()));
  ASSERT_EQ(2, (std::integer_sequence<bool, true, false>::size()));
  ASSERT_EQ(2, (std::integer_sequence<bool, true, true>::size()));
  ASSERT_EQ(3, (std::integer_sequence<bool, false, false, false>::size()));
  ASSERT_EQ(3, (std::integer_sequence<bool, false, false, true>::size()));
  ASSERT_EQ(3, (std::integer_sequence<bool, false, true, false>::size()));
  ASSERT_EQ(3, (std::integer_sequence<bool, false, true, true>::size()));
  ASSERT_EQ(3, (std::integer_sequence<bool, true, false, false>::size()));
  ASSERT_EQ(3, (std::integer_sequence<bool, true, false, true>::size()));
  ASSERT_EQ(3, (std::integer_sequence<bool, true, true, false>::size()));
  ASSERT_EQ(3, (std::integer_sequence<bool, true, true, true>::size()));
}

/// @test The static member function @c size of @c integer_sequence is noexcept
TYPED_TEST(IntegerSequenceTest, SizeMemberIsNoexcept) {
  ASSERT_TRUE(noexcept(std::integer_sequence<TypeParam>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<TypeParam, 0>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<TypeParam, 1>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<TypeParam, 2>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<TypeParam, 3>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<TypeParam, 0, 1>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<TypeParam, 1, 0>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<TypeParam, 1, 1>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<TypeParam, 2, 3>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<TypeParam, 0, 1, 2>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<TypeParam, 2, 1, 2>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<TypeParam, 0, 0, 0>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<TypeParam, 1, 2, 1>::size()));
}

/// @test The static member function @c size of @c integer_sequence is noexcept if the integer type is @c bool
TEST(BoolIntegerSequenceTest, SizeMemberIsNoexcept) {
  ASSERT_TRUE(noexcept(std::integer_sequence<bool>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<bool, false>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<bool, true>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<bool, false, false>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<bool, false, true>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<bool, true, false>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<bool, true, true>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<bool, false, false, false>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<bool, false, false, true>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<bool, false, true, false>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<bool, false, true, true>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<bool, true, false, false>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<bool, true, false, true>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<bool, true, true, false>::size()));
  ASSERT_TRUE(noexcept(std::integer_sequence<bool, true, true, true>::size()));
}

/// @brief a helper function template that compares a parameter pack of @c int to the elements in an @c integer_sequence
/// @tparam Expected expected element sequence
/// @tparam T actual integer type
/// @tparam Actual actual element sequence
/// @return @c true if the sequences are equal, otherwise @c false
///
/// Compares two sequences of integers. The @c Expected sequence elements are
/// cast to type @c T before comparison.
///
template <int... Expected, class T, T... Actual>
constexpr auto elements_are_equal_to(std::integer_sequence<T, Actual...>) -> bool {
  // this value *cannot* be declared const if sequences contain more than zero elements
  // NOLINTNEXTLINE(misc-const-correctness)
  bool all_equal = true;

  // avoid creating a zero element array
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  bool const unused[1 + sizeof...(Actual)]{all_equal &= (static_cast<T>(Expected) == Actual)...};
  static_cast<void>(unused);

  return all_equal;
}

/// @test A sequence of elements in an @c integer_sequence can be deduced and used in a pack expansion
CONSTEXPR_TYPED_TEST(IntegerSequenceTest, SequenceCanBeDeducedAndUsedInAPackExpansion) {
  using T = TypeParam;

  // negative int values are cast to T (e.g. size_t(-1))

  ASSERT_TRUE(elements_are_equal_to<>(std::integer_sequence<T>{}));

  ASSERT_TRUE(elements_are_equal_to<0>(std::integer_sequence<T, 0>{}));
  ASSERT_TRUE(elements_are_equal_to<1>(std::integer_sequence<T, 1>{}));
  ASSERT_TRUE(elements_are_equal_to<2>(std::integer_sequence<T, 2>{}));

  ASSERT_TRUE(elements_are_equal_to<0, 0>(std::integer_sequence<T, 0, 0>{}));
  ASSERT_TRUE(elements_are_equal_to<0, 3>(std::integer_sequence<T, 0, 3>{}));
  ASSERT_TRUE(elements_are_equal_to<-1, 1>(std::integer_sequence<T, T(-1), 1>{}));

  ASSERT_TRUE(elements_are_equal_to<0, 0, 0>(std::integer_sequence<T, 0, 0, 0>{}));
  ASSERT_TRUE(elements_are_equal_to<0, 1, 2>(std::integer_sequence<T, 0, 1, 2>{}));
  ASSERT_TRUE(elements_are_equal_to<-1, 0, 1>(std::integer_sequence<T, T(-1), 0, 1>{}));

  ASSERT_FALSE(elements_are_equal_to<1>(std::integer_sequence<T, 0>{}));
  ASSERT_FALSE(elements_are_equal_to<0>(std::integer_sequence<T, 1>{}));
  ASSERT_FALSE(elements_are_equal_to<0>(std::integer_sequence<T, 2>{}));

  ASSERT_FALSE(elements_are_equal_to<0, 1>(std::integer_sequence<T, 0, 0>{}));
  ASSERT_FALSE(elements_are_equal_to<1, 0>(std::integer_sequence<T, 0, 1>{}));
  ASSERT_FALSE(elements_are_equal_to<3, 0>(std::integer_sequence<T, 0, 3>{}));

  ASSERT_FALSE(elements_are_equal_to<0, 1, 2>(std::integer_sequence<T, 0, 0, 0>{}));
  ASSERT_FALSE(elements_are_equal_to<0, 0, 0>(std::integer_sequence<T, 0, 1, 2>{}));
  ASSERT_FALSE(elements_are_equal_to<0, 0, 0>(std::integer_sequence<T, T(-1), 0, 1>{}));
}

/// @test A sequence of elements in an @c integer_sequence can be deduced and used in a pack expansion if the integer
/// type is @c bool
CONSTEXPR_TEST(BoolIntegerSequenceTest, SequenceCanBeDeducedAndUsedInAPackExpansion) {
  ASSERT_TRUE(elements_are_equal_to<>(std::integer_sequence<bool>{}));

  ASSERT_TRUE(elements_are_equal_to<0>(std::integer_sequence<bool, false>{}));
  ASSERT_TRUE(elements_are_equal_to<1>(std::integer_sequence<bool, true>{}));

  ASSERT_TRUE(elements_are_equal_to<0, 0>(std::integer_sequence<bool, false, false>{}));
  ASSERT_TRUE(elements_are_equal_to<0, 1>(std::integer_sequence<bool, false, true>{}));
  ASSERT_TRUE(elements_are_equal_to<1, 0>(std::integer_sequence<bool, true, false>{}));
  ASSERT_TRUE(elements_are_equal_to<1, 1>(std::integer_sequence<bool, true, true>{}));

  ASSERT_TRUE(elements_are_equal_to<0, 0, 0>(std::integer_sequence<bool, false, false, false>{}));
  ASSERT_TRUE(elements_are_equal_to<0, 1, 0>(std::integer_sequence<bool, false, true, false>{}));
  ASSERT_TRUE(elements_are_equal_to<1, 1, 1>(std::integer_sequence<bool, true, true, true>{}));

  ASSERT_FALSE(elements_are_equal_to<1>(std::integer_sequence<bool, false>{}));
  ASSERT_FALSE(elements_are_equal_to<0>(std::integer_sequence<bool, true>{}));

  ASSERT_FALSE(elements_are_equal_to<1, 0>(std::integer_sequence<bool, false, false>{}));
  ASSERT_FALSE(elements_are_equal_to<1, 1>(std::integer_sequence<bool, false, false>{}));
  ASSERT_FALSE(elements_are_equal_to<1, 0>(std::integer_sequence<bool, false, true>{}));
  ASSERT_FALSE(elements_are_equal_to<1, 1>(std::integer_sequence<bool, false, true>{}));
}

/// @test @c make_integer_sequence with size N is an alias to an @c integer_sequence with elements from 0 to N - 1
TYPED_TEST(IntegerSequenceTest, MakeIntegerSequenceIsAliasToIntegerSequence) {
  StaticAssertTypeEq<std::make_integer_sequence<TypeParam, 0>, std::integer_sequence<TypeParam>>();

  StaticAssertTypeEq<std::make_integer_sequence<TypeParam, 1>, std::integer_sequence<TypeParam, 0>>();

  StaticAssertTypeEq<std::make_integer_sequence<TypeParam, 2>, std::integer_sequence<TypeParam, 0, 1>>();

  StaticAssertTypeEq<std::make_integer_sequence<TypeParam, 3>, std::integer_sequence<TypeParam, 0, 1, 2>>();

  StaticAssertTypeEq<
      std::make_integer_sequence<TypeParam, 10>,
      std::integer_sequence<TypeParam, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9>>();

  StaticAssertTypeEq<
      std::make_integer_sequence<TypeParam, 20>,
      std::integer_sequence<
          TypeParam,
          // clang-format off
          0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
          10, 11, 12, 13, 14, 15, 16, 17, 18, 19
          // clang-format on
          >>();

  StaticAssertTypeEq<
      std::make_integer_sequence<TypeParam, 50>,
      std::integer_sequence<
          TypeParam,
          // clang-format off
          0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
          10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
          20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
          30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
          40, 41, 42, 43, 44, 45, 46, 47, 48, 49
          // clang-format on
          >>();
}

/// @test @c make_integer_sequence with integer type @c bool and size N is an
/// alias to an @c integer_sequence with either 0 elements or 1 element
TEST(BoolIntegerSequenceTest, MakeIntegerSequenceIsAliasToIntegerSequence) {
  StaticAssertTypeEq<std::make_integer_sequence<bool, false>, std::integer_sequence<bool>>();
  StaticAssertTypeEq<std::make_integer_sequence<bool, true>, std::integer_sequence<bool, false>>();

  // NOLINTBEGIN(modernize-use-bool-literals,readability-implicit-bool-conversion)

  StaticAssertTypeEq<std::make_integer_sequence<bool, 0>, std::integer_sequence<bool>>();
  StaticAssertTypeEq<std::make_integer_sequence<bool, 1>, std::integer_sequence<bool, false>>();

  ARENE_IGNORE_START();

  ARENE_IGNORE_CLANG(
      "-Wc++11-narrowing",
      "This is an explicit test with narrowing as consumers may build without warnings enabled."
  );
  ARENE_IGNORE_ARMCLANG(
      "-Wc++11-narrowing",
      "This is an explicit test with narrowing as consumers may build without warnings enabled."
  );
  ARENE_IGNORE_GCC(
      "-Wnarrowing",
      "This is an explicit test with narrowing as consumers may build without warnings enabled."
  );

  // the following test cases require a narrowing conversion from int to bool
  StaticAssertTypeEq<std::make_integer_sequence<bool, 2>, std::integer_sequence<bool, false>>();
  StaticAssertTypeEq<std::make_integer_sequence<bool, 10>, std::integer_sequence<bool, false>>();
  StaticAssertTypeEq<std::make_integer_sequence<bool, -1>, std::integer_sequence<bool, false>>();

  ARENE_IGNORE_END();

  // NOLINTEND(modernize-use-bool-literals,readability-implicit-bool-conversion)
}

/// @test @c index_sequence is an alias for @c integer_sequence with type @c size_t
TEST(IntegerSequenceTest, IndexSequenceIsAliasWithSizeT) {
  StaticAssertTypeEq<std::integer_sequence<std::size_t>, std::index_sequence<>>();

  StaticAssertTypeEq<std::integer_sequence<std::size_t, 0>, std::index_sequence<0>>();
  StaticAssertTypeEq<std::integer_sequence<std::size_t, 1>, std::index_sequence<1>>();

  StaticAssertTypeEq<std::integer_sequence<std::size_t, 0, 1>, std::index_sequence<0, 1>>();
  StaticAssertTypeEq<std::integer_sequence<std::size_t, 1, 1>, std::index_sequence<1, 1>>();
  StaticAssertTypeEq<std::integer_sequence<std::size_t, 1, 0>, std::index_sequence<1, 0>>();
}

/// @test @c make_index_sequence is an alias for @c make_integer_sequence with type @c size_t
TEST(IntegerSequenceTest, MakeIndexSequenceIsAliasWithSizeT) {
  StaticAssertTypeEq<std::make_integer_sequence<std::size_t, 0>, std::make_index_sequence<0>>();
  StaticAssertTypeEq<std::make_integer_sequence<std::size_t, 1>, std::make_index_sequence<1>>();
  StaticAssertTypeEq<std::make_integer_sequence<std::size_t, 3>, std::make_index_sequence<3>>();
  StaticAssertTypeEq<std::make_integer_sequence<std::size_t, 5>, std::make_index_sequence<5>>();
}

/// @test @c index_sequence_for is an alias for @c make_integer_sequence with size determined from number of types in
/// the parameter pack
TEST(IntegerSequenceTest, IndexSequenceForIsAliasWithSizeEqualToNumberOfTypes) {
  StaticAssertTypeEq<std::make_index_sequence<0>, std::index_sequence_for<>>();

  StaticAssertTypeEq<std::make_index_sequence<1>, std::index_sequence_for<int>>();
  StaticAssertTypeEq<std::make_index_sequence<1>, std::index_sequence_for<int const>>();
  StaticAssertTypeEq<std::make_index_sequence<1>, std::index_sequence_for<int&>>();
  StaticAssertTypeEq<std::make_index_sequence<1>, std::index_sequence_for<int*>>();

  StaticAssertTypeEq<std::make_index_sequence<2>, std::index_sequence_for<int, int>>();
  StaticAssertTypeEq<std::make_index_sequence<2>, std::index_sequence_for<int, void>>();
  StaticAssertTypeEq<std::make_index_sequence<2>, std::index_sequence_for<void, int>>();
  StaticAssertTypeEq<std::make_index_sequence<2>, std::index_sequence_for<void, void>>();
}

}  // namespace
