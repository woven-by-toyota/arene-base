// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/utility/safe_comparisons.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/make_unsigned.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list/type_list.hpp"

namespace {
using ::arene::base::cmp_equal;
using ::arene::base::cmp_greater;
using ::arene::base::cmp_greater_equal;
using ::arene::base::cmp_less;
using ::arene::base::cmp_less_equal;
using ::arene::base::cmp_not_equal;
using ::arene::base::type_list;

///
/// @brief Helper to cast a value to its unsigned equivalent
///
/// @tparam T The type of the value to cast.
/// @param value The value to cast.
/// @return static_cast<std::make_unsigned_t<T>>(value)
///
template <typename T>
constexpr auto make_unsigned(T value) {
  return static_cast<std::make_unsigned_t<T>>(value);
}

/// @brief Creates a type_list containing T,std::make_unsigned_t<T>
template <typename T>
using signed_unsigned_tl = type_list<T, std::make_unsigned_t<T>>;

/// @brief The integer types to use for testing.
using integer_types = ::testing::Types<
    signed_unsigned_tl<std::int8_t>,
    signed_unsigned_tl<std::int16_t>,
    signed_unsigned_tl<std::int32_t>,
    signed_unsigned_tl<std::int64_t>>;

/// @brief A helper struct to extract the types stored in signed_unsigned_tl
template <typename T>
class signed_unsigned_pairing;

/// @brief A helper struct to extract the types stored in signed_unsigned_tl
template <typename S, typename U>
class signed_unsigned_pairing<type_list<S, U>> {
 public:
  /// @brief The signed type.
  using signed_type = S;
  /// @brief The unsigned type.
  using unsigned_type = U;
};

template <typename T>
class SafeCmpTest : public ::testing::Test {
 protected:
  using pairing = signed_unsigned_pairing<T>;

  static constexpr typename pairing::signed_type signed_zero{0};
  static constexpr typename pairing::unsigned_type unsigned_zero{0U};
  static constexpr typename pairing::signed_type signed_small_positive{1};
  static constexpr typename pairing::unsigned_type unsigned_small_positive{1U};
  static constexpr
      typename pairing::signed_type signed_max_positive{std::numeric_limits<typename pairing::signed_type>::max()};
  static constexpr
      typename pairing::unsigned_type unsigned_max_positive{std::numeric_limits<typename pairing::unsigned_type>::max()
      };
  static constexpr typename pairing::signed_type signed_small_negative{-1};
  static constexpr
      typename pairing::signed_type signed_max_negative{std::numeric_limits<typename pairing::signed_type>::lowest()};
};

template <typename T>
using CmpEqualTest = SafeCmpTest<T>;

TYPED_TEST_SUITE(CmpEqualTest, integer_types, );
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value==std::is_signed<Int2>::value , then @c arene::base::cmp_equal(int1,int2) is
/// equivalent to @c int1==int2 .
TYPED_TEST(CmpEqualTest, OperandsOfSameSignednessCompareAsIfDirectComparison) {
  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::signed_zero, TestFixture::signed_zero),
      TestFixture::signed_zero == TestFixture::signed_zero
  );
  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::unsigned_zero, TestFixture::unsigned_zero),
      TestFixture::unsigned_zero == TestFixture::unsigned_zero
  );

  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::signed_small_positive, TestFixture::signed_small_positive),
      TestFixture::signed_small_positive == TestFixture::signed_small_positive
  );
  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::unsigned_small_positive, TestFixture::unsigned_small_positive),
      TestFixture::unsigned_small_positive == TestFixture::unsigned_small_positive
  );

  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::signed_max_positive, TestFixture::signed_max_positive),
      TestFixture::signed_max_positive == TestFixture::signed_max_positive
  );
  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::signed_max_positive, TestFixture::signed_max_positive),
      TestFixture::signed_max_positive == TestFixture::signed_max_positive
  );

  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::signed_small_negative, TestFixture::signed_small_negative),
      TestFixture::signed_small_negative == TestFixture::signed_small_negative
  );
  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::signed_max_negative, TestFixture::signed_max_negative),
      TestFixture::signed_max_negative == TestFixture::signed_max_negative
  );

  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::signed_zero, TestFixture::signed_small_negative),
      TestFixture::signed_zero == TestFixture::signed_small_negative
  );
  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::unsigned_zero, TestFixture::unsigned_small_positive),
      TestFixture::unsigned_zero == TestFixture::unsigned_small_positive
  );
}
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value is @c true and @c std::is_signed<Int2>::value is @c false , and given @c int1<0 ,
/// then @c arene::base::cmp_equal(int1,int2) returns @c false for all @c int1.
TYPED_TEST(CmpEqualTest, LhsOperandSignedAndNegativeComparesFalseToAllUnsignedValues) {
  STATIC_ASSERT_FALSE(cmp_equal(TestFixture::signed_small_negative, TestFixture::unsigned_zero));
  STATIC_ASSERT_FALSE(cmp_equal(TestFixture::signed_small_negative, TestFixture::unsigned_small_positive));
  STATIC_ASSERT_FALSE(cmp_equal(TestFixture::signed_small_negative, TestFixture::unsigned_max_positive));

  STATIC_ASSERT_FALSE(cmp_equal(TestFixture::signed_max_negative, TestFixture::unsigned_zero));
  STATIC_ASSERT_FALSE(cmp_equal(TestFixture::signed_max_negative, TestFixture::unsigned_small_positive));
  STATIC_ASSERT_FALSE(cmp_equal(TestFixture::signed_max_negative, TestFixture::unsigned_max_positive));
}
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value is @c false and @c std::is_signed<Int2>::value is @c true , and given @c int2<0 ,
/// then @c arene::base::cmp_equal(int1,int2) returns @c false for all @c int2.
TYPED_TEST(CmpEqualTest, RhsOperandSignedAndNegativeComparesFalseToAllUnsignedValues) {
  STATIC_ASSERT_FALSE(cmp_equal(TestFixture::unsigned_zero, TestFixture::signed_small_negative));
  STATIC_ASSERT_FALSE(cmp_equal(TestFixture::unsigned_small_positive, TestFixture::signed_small_negative));
  STATIC_ASSERT_FALSE(cmp_equal(TestFixture::unsigned_max_positive, TestFixture::signed_small_negative));

  STATIC_ASSERT_FALSE(cmp_equal(TestFixture::unsigned_zero, TestFixture::signed_max_negative));
  STATIC_ASSERT_FALSE(cmp_equal(TestFixture::unsigned_small_positive, TestFixture::signed_max_negative));
  STATIC_ASSERT_FALSE(cmp_equal(TestFixture::unsigned_max_positive, TestFixture::signed_max_negative));
}
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value is @c true and @c std::is_signed<Int2>::value is @c false , and given @c int1>=0 ,
/// then @c arene::base::cmp_equal(int1,int2) is equivalent to @c int1==int2 with @c int1 cast to an equivalent unsigned
/// type.
TYPED_TEST(CmpEqualTest, LhsOperandSignedAndPositiveComparesAsIfViaUnsignedComparisonToAllUnsignedValues) {
  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::signed_zero, TestFixture::unsigned_zero),
      make_unsigned(TestFixture::signed_zero) == TestFixture::unsigned_zero
  );
  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::signed_zero, TestFixture::unsigned_small_positive),
      make_unsigned(TestFixture::signed_zero) == TestFixture::unsigned_small_positive
  );
  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::signed_zero, TestFixture::unsigned_max_positive),
      make_unsigned(TestFixture::signed_zero) == TestFixture::unsigned_max_positive
  );

  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::signed_small_positive, TestFixture::unsigned_zero),
      make_unsigned(TestFixture::signed_small_positive) == TestFixture::unsigned_zero
  );
  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::signed_small_positive, TestFixture::unsigned_small_positive),
      make_unsigned(TestFixture::signed_small_positive) == TestFixture::unsigned_small_positive
  );
  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::signed_small_positive, TestFixture::unsigned_max_positive),
      make_unsigned(TestFixture::signed_small_positive) == TestFixture::unsigned_max_positive
  );

  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::signed_max_positive, TestFixture::unsigned_zero),
      make_unsigned(TestFixture::signed_max_positive) == TestFixture::unsigned_zero
  );
  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::signed_max_positive, TestFixture::unsigned_small_positive),
      make_unsigned(TestFixture::signed_max_positive) == TestFixture::unsigned_small_positive
  );
  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::signed_max_positive, TestFixture::unsigned_max_positive),
      make_unsigned(TestFixture::signed_max_positive) == TestFixture::unsigned_max_positive
  );
}
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value is @c false and @c std::is_signed<Int2>::value is @c true , and given @c int2>=0 ,
/// then @c arene::base::cmp_equal(int1,int2) is equivalent to @c int1==int2 with @c int2 cast to an equivalent unsigned
/// type.
TYPED_TEST(CmpEqualTest, RhsOperandSignedAndPositiveComparesAsIfViaUnsignedComparisonToAllUnsignedValues) {
  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::unsigned_zero, TestFixture::signed_zero),
      TestFixture::unsigned_zero == make_unsigned(TestFixture::signed_zero)
  );
  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::unsigned_small_positive, TestFixture::signed_zero),
      TestFixture::unsigned_small_positive == make_unsigned(TestFixture::signed_zero)
  );
  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::unsigned_max_positive, TestFixture::signed_zero),
      TestFixture::unsigned_max_positive == make_unsigned(TestFixture::signed_zero)
  );

  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::unsigned_zero, TestFixture::signed_small_positive),
      TestFixture::unsigned_zero == make_unsigned(TestFixture::signed_small_positive)
  );
  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::unsigned_small_positive, TestFixture::signed_small_positive),
      TestFixture::unsigned_small_positive == make_unsigned(TestFixture::signed_small_positive)
  );
  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::unsigned_max_positive, TestFixture::signed_small_positive),
      TestFixture::unsigned_max_positive == make_unsigned(TestFixture::signed_small_positive)
  );

  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::unsigned_zero, TestFixture::signed_max_positive),
      TestFixture::unsigned_zero == make_unsigned(TestFixture::signed_max_positive)
  );
  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::unsigned_small_positive, TestFixture::signed_max_positive),
      TestFixture::unsigned_small_positive == make_unsigned(TestFixture::signed_max_positive)
  );
  STATIC_ASSERT_EQ(
      cmp_equal(TestFixture::unsigned_max_positive, TestFixture::signed_max_positive),
      TestFixture::unsigned_max_positive == make_unsigned(TestFixture::signed_max_positive)
  );
}

template <typename T>
class CmpNotEqualTest : public SafeCmpTest<T> {};

TYPED_TEST_SUITE(CmpNotEqualTest, integer_types, );
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value==std::is_signed<Int2>::value , then @c arene::base::cmp_not_equal(int1,int2) is
/// equivalent to @c int1!=int2 .
TYPED_TEST(CmpNotEqualTest, OperandsOfSameSignednessCompareAsIfDirectComparison) {
  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::signed_zero, TestFixture::signed_zero),
      TestFixture::signed_zero != TestFixture::signed_zero
  );
  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::unsigned_zero, TestFixture::unsigned_zero),
      TestFixture::unsigned_zero != TestFixture::unsigned_zero
  );

  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::signed_small_positive, TestFixture::signed_small_positive),
      TestFixture::signed_small_positive != TestFixture::signed_small_positive
  );
  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::unsigned_small_positive, TestFixture::unsigned_small_positive),
      TestFixture::unsigned_small_positive != TestFixture::unsigned_small_positive
  );

  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::signed_max_positive, TestFixture::signed_max_positive),
      TestFixture::signed_max_positive != TestFixture::signed_max_positive
  );
  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::signed_max_positive, TestFixture::signed_max_positive),
      TestFixture::signed_max_positive != TestFixture::signed_max_positive
  );

  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::signed_small_negative, TestFixture::signed_small_negative),
      TestFixture::signed_small_negative != TestFixture::signed_small_negative
  );
  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::signed_max_negative, TestFixture::signed_max_negative),
      TestFixture::signed_max_negative != TestFixture::signed_max_negative
  );

  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::signed_zero, TestFixture::signed_small_negative),
      TestFixture::signed_zero != TestFixture::signed_small_negative
  );
  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::unsigned_zero, TestFixture::unsigned_small_positive),
      TestFixture::unsigned_zero != TestFixture::unsigned_small_positive
  );
}
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value is @c true and @c std::is_signed<Int2>::value is @c false , and given @c int1<0 ,
/// then @c arene::base::cmp_not_equal(int1,int2) returns @c true for all @c int1.
TYPED_TEST(CmpNotEqualTest, LhsOperandSignedAndNegativeComparesTrueToAllUnsignedValues) {
  STATIC_ASSERT_TRUE(cmp_not_equal(TestFixture::signed_small_negative, TestFixture::unsigned_zero));
  STATIC_ASSERT_TRUE(cmp_not_equal(TestFixture::signed_small_negative, TestFixture::unsigned_small_positive));
  STATIC_ASSERT_TRUE(cmp_not_equal(TestFixture::signed_small_negative, TestFixture::unsigned_max_positive));

  STATIC_ASSERT_TRUE(cmp_not_equal(TestFixture::signed_max_negative, TestFixture::unsigned_zero));
  STATIC_ASSERT_TRUE(cmp_not_equal(TestFixture::signed_max_negative, TestFixture::unsigned_small_positive));
  STATIC_ASSERT_TRUE(cmp_not_equal(TestFixture::signed_max_negative, TestFixture::unsigned_max_positive));
}
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value is @c false and @c std::is_signed<Int2>::value is @c true , and given @c int2<0 ,
/// then @c arene::base::cmp_not_equal(int1,int2) returns @c true for all @c int2.
TYPED_TEST(CmpNotEqualTest, RhsOperandSignedAndNegativeComparesTrueToAllUnsignedValues) {
  STATIC_ASSERT_TRUE(cmp_not_equal(TestFixture::unsigned_zero, TestFixture::signed_small_negative));
  STATIC_ASSERT_TRUE(cmp_not_equal(TestFixture::unsigned_small_positive, TestFixture::signed_small_negative));
  STATIC_ASSERT_TRUE(cmp_not_equal(TestFixture::unsigned_max_positive, TestFixture::signed_small_negative));

  STATIC_ASSERT_TRUE(cmp_not_equal(TestFixture::unsigned_zero, TestFixture::signed_max_negative));
  STATIC_ASSERT_TRUE(cmp_not_equal(TestFixture::unsigned_small_positive, TestFixture::signed_max_negative));
  STATIC_ASSERT_TRUE(cmp_not_equal(TestFixture::unsigned_max_positive, TestFixture::signed_max_negative));
}
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value is @c true and @c std::is_signed<Int2>::value is @c false , and given @c int1>=0 ,
/// then @c arene::base::cmp_not_equal(int1,int2) is equivalent to @c int1!=int2 with @c int1 cast to an equivalent
/// unsigned type.
TYPED_TEST(CmpNotEqualTest, LhsOperandSignedAndPositiveComparesAsIfViaUnsignedComparisonToAllUnsignedValues) {
  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::signed_zero, TestFixture::unsigned_zero),
      make_unsigned(TestFixture::signed_zero) != TestFixture::unsigned_zero
  );
  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::signed_zero, TestFixture::unsigned_small_positive),
      make_unsigned(TestFixture::signed_zero) != TestFixture::unsigned_small_positive
  );
  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::signed_zero, TestFixture::unsigned_max_positive),
      make_unsigned(TestFixture::signed_zero) != TestFixture::unsigned_max_positive
  );

  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::signed_small_positive, TestFixture::unsigned_zero),
      make_unsigned(TestFixture::signed_small_positive) != TestFixture::unsigned_zero
  );
  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::signed_small_positive, TestFixture::unsigned_small_positive),
      make_unsigned(TestFixture::signed_small_positive) != TestFixture::unsigned_small_positive
  );
  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::signed_small_positive, TestFixture::unsigned_max_positive),
      make_unsigned(TestFixture::signed_small_positive) != TestFixture::unsigned_max_positive
  );

  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::signed_max_positive, TestFixture::unsigned_zero),
      make_unsigned(TestFixture::signed_max_positive) != TestFixture::unsigned_zero
  );
  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::signed_max_positive, TestFixture::unsigned_small_positive),
      make_unsigned(TestFixture::signed_max_positive) != TestFixture::unsigned_small_positive
  );
  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::signed_max_positive, TestFixture::unsigned_max_positive),
      make_unsigned(TestFixture::signed_max_positive) != TestFixture::unsigned_max_positive
  );
}
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value is @c false and @c std::is_signed<Int2>::value is @c true , and given @c int2>=0 ,
/// then @c arene::base::cmp_not_equal(int1,int2) is equivalent to @c int1!=int2 with @c int2 cast to an equivalent
/// unsigned type.
TYPED_TEST(CmpNotEqualTest, RhsOperandSignedAndPositiveComparesAsIfViaUnsignedComparisonToAllUnsignedValues) {
  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::unsigned_zero, TestFixture::signed_zero),
      TestFixture::unsigned_zero != make_unsigned(TestFixture::signed_zero)
  );
  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::unsigned_small_positive, TestFixture::signed_zero),
      TestFixture::unsigned_small_positive != make_unsigned(TestFixture::signed_zero)
  );
  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::unsigned_max_positive, TestFixture::signed_zero),
      TestFixture::unsigned_max_positive != make_unsigned(TestFixture::signed_zero)
  );

  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::unsigned_zero, TestFixture::signed_small_positive),
      TestFixture::unsigned_zero != make_unsigned(TestFixture::signed_small_positive)
  );
  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::unsigned_small_positive, TestFixture::signed_small_positive),
      TestFixture::unsigned_small_positive != make_unsigned(TestFixture::signed_small_positive)
  );
  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::unsigned_max_positive, TestFixture::signed_small_positive),
      TestFixture::unsigned_max_positive != make_unsigned(TestFixture::signed_small_positive)
  );

  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::unsigned_zero, TestFixture::signed_max_positive),
      TestFixture::unsigned_zero != make_unsigned(TestFixture::signed_max_positive)
  );
  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::unsigned_small_positive, TestFixture::signed_max_positive),
      TestFixture::unsigned_small_positive != make_unsigned(TestFixture::signed_max_positive)
  );
  STATIC_ASSERT_EQ(
      cmp_not_equal(TestFixture::unsigned_max_positive, TestFixture::signed_max_positive),
      TestFixture::unsigned_max_positive != make_unsigned(TestFixture::signed_max_positive)
  );
}

template <typename T>
class CmpLessTest : public SafeCmpTest<T> {};

TYPED_TEST_SUITE(CmpLessTest, integer_types, );
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value==std::is_signed<Int2>::value , then @c arene::base::cmp_less(int1,int2) is
/// equivalent to @c int1<int2 .
TYPED_TEST(CmpLessTest, OperandsOfSameSignednessCompareAsIfDirectComparison) {
  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::signed_zero, TestFixture::signed_zero),
      TestFixture::signed_zero < TestFixture::signed_zero
  );
  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::unsigned_zero, TestFixture::unsigned_zero),
      TestFixture::unsigned_zero < TestFixture::unsigned_zero
  );

  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::signed_small_positive, TestFixture::signed_small_positive),
      TestFixture::signed_small_positive < TestFixture::signed_small_positive
  );
  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::unsigned_small_positive, TestFixture::unsigned_small_positive),
      TestFixture::unsigned_small_positive < TestFixture::unsigned_small_positive
  );

  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::signed_max_positive, TestFixture::signed_max_positive),
      TestFixture::signed_max_positive < TestFixture::signed_max_positive
  );
  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::signed_max_positive, TestFixture::signed_max_positive),
      TestFixture::signed_max_positive < TestFixture::signed_max_positive
  );

  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::signed_small_negative, TestFixture::signed_small_negative),
      TestFixture::signed_small_negative < TestFixture::signed_small_negative
  );
  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::signed_max_negative, TestFixture::signed_max_negative),
      TestFixture::signed_max_negative < TestFixture::signed_max_negative
  );

  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::signed_zero, TestFixture::signed_small_negative),
      TestFixture::signed_zero < TestFixture::signed_small_negative
  );
  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::unsigned_zero, TestFixture::unsigned_small_positive),
      TestFixture::unsigned_zero < TestFixture::unsigned_small_positive
  );
}
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value is @c true and @c std::is_signed<Int2>::value is @c false , and given @c int1<0 ,
/// then @c arene::base::cmp_less(int1,int2) returns @c true for all @c int1.
TYPED_TEST(CmpLessTest, LhsOperandSignedAndNegativeComparesTrueToAllUnsignedValues) {
  STATIC_ASSERT_TRUE(cmp_less(TestFixture::signed_small_negative, TestFixture::unsigned_zero));
  STATIC_ASSERT_TRUE(cmp_less(TestFixture::signed_small_negative, TestFixture::unsigned_small_positive));
  STATIC_ASSERT_TRUE(cmp_less(TestFixture::signed_small_negative, TestFixture::unsigned_max_positive));

  STATIC_ASSERT_TRUE(cmp_less(TestFixture::signed_max_negative, TestFixture::unsigned_zero));
  STATIC_ASSERT_TRUE(cmp_less(TestFixture::signed_max_negative, TestFixture::unsigned_small_positive));
  STATIC_ASSERT_TRUE(cmp_less(TestFixture::signed_max_negative, TestFixture::unsigned_max_positive));
}
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value is @c false and @c std::is_signed<Int2>::value is @c true , and given @c int2<0 ,
/// then @c arene::base::cmp_less(int1,int2) returns @c false for all @c int2.
TYPED_TEST(CmpLessTest, RhsOperandSignedAndNegativeComparesFalseToAllUnsignedValues) {
  STATIC_ASSERT_FALSE(cmp_less(TestFixture::unsigned_zero, TestFixture::signed_small_negative));
  STATIC_ASSERT_FALSE(cmp_less(TestFixture::unsigned_small_positive, TestFixture::signed_small_negative));
  STATIC_ASSERT_FALSE(cmp_less(TestFixture::unsigned_max_positive, TestFixture::signed_small_negative));

  STATIC_ASSERT_FALSE(cmp_less(TestFixture::unsigned_zero, TestFixture::signed_max_negative));
  STATIC_ASSERT_FALSE(cmp_less(TestFixture::unsigned_small_positive, TestFixture::signed_max_negative));
  STATIC_ASSERT_FALSE(cmp_less(TestFixture::unsigned_max_positive, TestFixture::signed_max_negative));
}
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value is @c true and @c std::is_signed<Int2>::value is @c false , and given @c int1>=0 ,
/// then @c arene::base::cmp_less(int1,int2) is equivalent to @c int1<int2 with @c int1 cast to an equivalent
/// unsigned type.
TYPED_TEST(CmpLessTest, LhsOperandSignedAndPositiveComparesAsIfViaUnsignedComparisonToAllUnsignedValues) {
  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::signed_zero, TestFixture::unsigned_zero),
      make_unsigned(TestFixture::signed_zero) < TestFixture::unsigned_zero
  );
  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::signed_zero, TestFixture::unsigned_small_positive),
      make_unsigned(TestFixture::signed_zero) < TestFixture::unsigned_small_positive
  );
  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::signed_zero, TestFixture::unsigned_max_positive),
      make_unsigned(TestFixture::signed_zero) < TestFixture::unsigned_max_positive
  );

  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::signed_small_positive, TestFixture::unsigned_zero),
      make_unsigned(TestFixture::signed_small_positive) < TestFixture::unsigned_zero
  );
  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::signed_small_positive, TestFixture::unsigned_small_positive),
      make_unsigned(TestFixture::signed_small_positive) < TestFixture::unsigned_small_positive
  );
  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::signed_small_positive, TestFixture::unsigned_max_positive),
      make_unsigned(TestFixture::signed_small_positive) < TestFixture::unsigned_max_positive
  );

  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::signed_max_positive, TestFixture::unsigned_zero),
      make_unsigned(TestFixture::signed_max_positive) < TestFixture::unsigned_zero
  );
  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::signed_max_positive, TestFixture::unsigned_small_positive),
      make_unsigned(TestFixture::signed_max_positive) < TestFixture::unsigned_small_positive
  );
  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::signed_max_positive, TestFixture::unsigned_max_positive),
      make_unsigned(TestFixture::signed_max_positive) < TestFixture::unsigned_max_positive
  );
}
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value is @c false and @c std::is_signed<Int2>::value is @c true , and given @c int2>=0 ,
/// then @c arene::base::cmp_less(int1,int2) is equivalent to @c int1<int2 with @c int2 cast to an equivalent
/// unsigned type.
TYPED_TEST(CmpLessTest, RhsOperandSignedAndPositiveComparesAsIfViaUnsignedComparisonToAllUnsignedValues) {
  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::unsigned_zero, TestFixture::signed_zero),
      TestFixture::unsigned_zero < make_unsigned(TestFixture::signed_zero)
  );
  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::unsigned_small_positive, TestFixture::signed_zero),
      TestFixture::unsigned_small_positive < make_unsigned(TestFixture::signed_zero)
  );
  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::unsigned_max_positive, TestFixture::signed_zero),
      TestFixture::unsigned_max_positive < make_unsigned(TestFixture::signed_zero)
  );

  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::unsigned_zero, TestFixture::signed_small_positive),
      TestFixture::unsigned_zero < make_unsigned(TestFixture::signed_small_positive)
  );
  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::unsigned_small_positive, TestFixture::signed_small_positive),
      TestFixture::unsigned_small_positive < make_unsigned(TestFixture::signed_small_positive)
  );
  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::unsigned_max_positive, TestFixture::signed_small_positive),
      TestFixture::unsigned_max_positive < make_unsigned(TestFixture::signed_small_positive)
  );

  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::unsigned_zero, TestFixture::signed_max_positive),
      TestFixture::unsigned_zero < make_unsigned(TestFixture::signed_max_positive)
  );
  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::unsigned_small_positive, TestFixture::signed_max_positive),
      TestFixture::unsigned_small_positive < make_unsigned(TestFixture::signed_max_positive)
  );
  STATIC_ASSERT_EQ(
      cmp_less(TestFixture::unsigned_max_positive, TestFixture::signed_max_positive),
      TestFixture::unsigned_max_positive < make_unsigned(TestFixture::signed_max_positive)
  );
}

template <typename T>
class CmpLessEqualTest : public SafeCmpTest<T> {};

TYPED_TEST_SUITE(CmpLessEqualTest, integer_types, );
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value==std::is_signed<Int2>::value , then @c arene::base::cmp_less_equal(int1,int2) is
/// equivalent to @c int1<=int2 .
TYPED_TEST(CmpLessEqualTest, OperandsOfSameSignednessCompareAsIfDirectComparison) {
  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::signed_zero, TestFixture::signed_zero),
      TestFixture::signed_zero <= TestFixture::signed_zero
  );
  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::unsigned_zero, TestFixture::unsigned_zero),
      TestFixture::unsigned_zero <= TestFixture::unsigned_zero
  );

  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::signed_small_positive, TestFixture::signed_small_positive),
      TestFixture::signed_small_positive <= TestFixture::signed_small_positive
  );
  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::unsigned_small_positive, TestFixture::unsigned_small_positive),
      TestFixture::unsigned_small_positive <= TestFixture::unsigned_small_positive
  );

  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::signed_max_positive, TestFixture::signed_max_positive),
      TestFixture::signed_max_positive <= TestFixture::signed_max_positive
  );
  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::signed_max_positive, TestFixture::signed_max_positive),
      TestFixture::signed_max_positive <= TestFixture::signed_max_positive
  );

  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::signed_small_negative, TestFixture::signed_small_negative),
      TestFixture::signed_small_negative <= TestFixture::signed_small_negative
  );
  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::signed_max_negative, TestFixture::signed_max_negative),
      TestFixture::signed_max_negative <= TestFixture::signed_max_negative
  );

  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::signed_zero, TestFixture::signed_small_negative),
      TestFixture::signed_zero <= TestFixture::signed_small_negative
  );
  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::unsigned_zero, TestFixture::unsigned_small_positive),
      TestFixture::unsigned_zero <= TestFixture::unsigned_small_positive
  );
}
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value is @c true and @c std::is_signed<Int2>::value is @c false , and given @c int1<0 ,
/// then @c arene::base::cmp_less_equal(int1,int2) returns @c true for all @c int1.
TYPED_TEST(CmpLessEqualTest, LhsOperandSignedAndNegativeComparesTrueToAllUnsignedValues) {
  STATIC_ASSERT_TRUE(cmp_less_equal(TestFixture::signed_small_negative, TestFixture::unsigned_zero));
  STATIC_ASSERT_TRUE(cmp_less_equal(TestFixture::signed_small_negative, TestFixture::unsigned_small_positive));
  STATIC_ASSERT_TRUE(cmp_less_equal(TestFixture::signed_small_negative, TestFixture::unsigned_max_positive));

  STATIC_ASSERT_TRUE(cmp_less_equal(TestFixture::signed_max_negative, TestFixture::unsigned_zero));
  STATIC_ASSERT_TRUE(cmp_less_equal(TestFixture::signed_max_negative, TestFixture::unsigned_small_positive));
  STATIC_ASSERT_TRUE(cmp_less_equal(TestFixture::signed_max_negative, TestFixture::unsigned_max_positive));
}
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value is @c false and @c std::is_signed<Int2>::value is @c true , and given @c int2<0 ,
/// then @c arene::base::cmp_less_equal(int1,int2) returns @c false for all @c int2.
TYPED_TEST(CmpLessEqualTest, RhsOperandSignedAndNegativeComparesFalseToAllUnsignedValues) {
  STATIC_ASSERT_FALSE(cmp_less_equal(TestFixture::unsigned_zero, TestFixture::signed_small_negative));
  STATIC_ASSERT_FALSE(cmp_less_equal(TestFixture::unsigned_small_positive, TestFixture::signed_small_negative));
  STATIC_ASSERT_FALSE(cmp_less_equal(TestFixture::unsigned_max_positive, TestFixture::signed_small_negative));

  STATIC_ASSERT_FALSE(cmp_less_equal(TestFixture::unsigned_zero, TestFixture::signed_max_negative));
  STATIC_ASSERT_FALSE(cmp_less_equal(TestFixture::unsigned_small_positive, TestFixture::signed_max_negative));
  STATIC_ASSERT_FALSE(cmp_less_equal(TestFixture::unsigned_max_positive, TestFixture::signed_max_negative));
}
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value is @c true and @c std::is_signed<Int2>::value is @c false , and given @c int1>=0 ,
/// then @c arene::base::cmp_less_equal(int1,int2) is equivalent to @c int1<=int2 with @c int1 cast to an equivalent
/// unsigned type.
TYPED_TEST(CmpLessEqualTest, LhsOperandSignedAndPositiveComparesAsIfViaUnsignedComparisonToAllUnsignedValues) {
  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::signed_zero, TestFixture::unsigned_zero),
      make_unsigned(TestFixture::signed_zero) <= TestFixture::unsigned_zero
  );
  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::signed_zero, TestFixture::unsigned_small_positive),
      make_unsigned(TestFixture::signed_zero) <= TestFixture::unsigned_small_positive
  );
  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::signed_zero, TestFixture::unsigned_max_positive),
      make_unsigned(TestFixture::signed_zero) <= TestFixture::unsigned_max_positive
  );

  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::signed_small_positive, TestFixture::unsigned_zero),
      make_unsigned(TestFixture::signed_small_positive) <= TestFixture::unsigned_zero
  );
  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::signed_small_positive, TestFixture::unsigned_small_positive),
      make_unsigned(TestFixture::signed_small_positive) <= TestFixture::unsigned_small_positive
  );
  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::signed_small_positive, TestFixture::unsigned_max_positive),
      make_unsigned(TestFixture::signed_small_positive) <= TestFixture::unsigned_max_positive
  );

  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::signed_max_positive, TestFixture::unsigned_zero),
      make_unsigned(TestFixture::signed_max_positive) <= TestFixture::unsigned_zero
  );
  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::signed_max_positive, TestFixture::unsigned_small_positive),
      make_unsigned(TestFixture::signed_max_positive) <= TestFixture::unsigned_small_positive
  );
  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::signed_max_positive, TestFixture::unsigned_max_positive),
      make_unsigned(TestFixture::signed_max_positive) <= TestFixture::unsigned_max_positive
  );
}
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value is @c false and @c std::is_signed<Int2>::value is @c true , and given @c int2>=0 ,
/// then @c arene::base::cmp_less_equal(int1,int2) is equivalent to @c int1<=int2 with @c int2 cast to an equivalent
/// unsigned type.
TYPED_TEST(CmpLessEqualTest, RhsOperandSignedAndPositiveComparesAsIfViaUnsignedComparisonToAllUnsignedValues) {
  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::unsigned_zero, TestFixture::signed_zero),
      TestFixture::unsigned_zero <= make_unsigned(TestFixture::signed_zero)
  );
  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::unsigned_small_positive, TestFixture::signed_zero),
      TestFixture::unsigned_small_positive <= make_unsigned(TestFixture::signed_zero)
  );
  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::unsigned_max_positive, TestFixture::signed_zero),
      TestFixture::unsigned_max_positive <= make_unsigned(TestFixture::signed_zero)
  );

  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::unsigned_zero, TestFixture::signed_small_positive),
      TestFixture::unsigned_zero <= make_unsigned(TestFixture::signed_small_positive)
  );
  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::unsigned_small_positive, TestFixture::signed_small_positive),
      TestFixture::unsigned_small_positive <= make_unsigned(TestFixture::signed_small_positive)
  );
  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::unsigned_max_positive, TestFixture::signed_small_positive),
      TestFixture::unsigned_max_positive <= make_unsigned(TestFixture::signed_small_positive)
  );

  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::unsigned_zero, TestFixture::signed_max_positive),
      TestFixture::unsigned_zero <= make_unsigned(TestFixture::signed_max_positive)
  );
  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::unsigned_small_positive, TestFixture::signed_max_positive),
      TestFixture::unsigned_small_positive <= make_unsigned(TestFixture::signed_max_positive)
  );
  STATIC_ASSERT_EQ(
      cmp_less_equal(TestFixture::unsigned_max_positive, TestFixture::signed_max_positive),
      TestFixture::unsigned_max_positive <= make_unsigned(TestFixture::signed_max_positive)
  );
}

template <typename T>
class CmpGreaterTest : public SafeCmpTest<T> {};

TYPED_TEST_SUITE(CmpGreaterTest, integer_types, );
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value==std::is_signed<Int2>::value , then @c arene::base::cmp_greater(int1,int2) is
/// equivalent to @c int1>int2 .
TYPED_TEST(CmpGreaterTest, OperandsOfSameSignednessCompareAsIfDirectComparison) {
  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::signed_zero, TestFixture::signed_zero),
      TestFixture::signed_zero > TestFixture::signed_zero
  );
  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::unsigned_zero, TestFixture::unsigned_zero),
      TestFixture::unsigned_zero > TestFixture::unsigned_zero
  );

  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::signed_small_positive, TestFixture::signed_small_positive),
      TestFixture::signed_small_positive > TestFixture::signed_small_positive
  );
  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::unsigned_small_positive, TestFixture::unsigned_small_positive),
      TestFixture::unsigned_small_positive > TestFixture::unsigned_small_positive
  );

  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::signed_max_positive, TestFixture::signed_max_positive),
      TestFixture::signed_max_positive > TestFixture::signed_max_positive
  );
  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::signed_max_positive, TestFixture::signed_max_positive),
      TestFixture::signed_max_positive > TestFixture::signed_max_positive
  );

  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::signed_small_negative, TestFixture::signed_small_negative),
      TestFixture::signed_small_negative > TestFixture::signed_small_negative
  );
  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::signed_max_negative, TestFixture::signed_max_negative),
      TestFixture::signed_max_negative > TestFixture::signed_max_negative
  );

  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::signed_zero, TestFixture::signed_small_negative),
      TestFixture::signed_zero > TestFixture::signed_small_negative
  );
  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::unsigned_zero, TestFixture::unsigned_small_positive),
      TestFixture::unsigned_zero > TestFixture::unsigned_small_positive
  );
}
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value is @c true and @c std::is_signed<Int2>::value is @c false , and given @c int1<0 ,
/// then @c arene::base::cmp_greater(int1,int2) returns @c false for all @c int1.
TYPED_TEST(CmpGreaterTest, LhsOperandSignedAndNegativeComparesFalseToAllUnsignedValues) {
  STATIC_ASSERT_FALSE(cmp_greater(TestFixture::signed_small_negative, TestFixture::unsigned_zero));
  STATIC_ASSERT_FALSE(cmp_greater(TestFixture::signed_small_negative, TestFixture::unsigned_small_positive));
  STATIC_ASSERT_FALSE(cmp_greater(TestFixture::signed_small_negative, TestFixture::unsigned_max_positive));

  STATIC_ASSERT_FALSE(cmp_greater(TestFixture::signed_max_negative, TestFixture::unsigned_zero));
  STATIC_ASSERT_FALSE(cmp_greater(TestFixture::signed_max_negative, TestFixture::unsigned_small_positive));
  STATIC_ASSERT_FALSE(cmp_greater(TestFixture::signed_max_negative, TestFixture::unsigned_max_positive));
}
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value is @c false and @c std::is_signed<Int2>::value is @c true , and given @c int2<0 ,
/// then @c arene::base::cmp_greater(int1,int2) returns @c true for all @c int2.
TYPED_TEST(CmpGreaterTest, RhsOperandSignedAndNegativeComparesTrueToAllUnsignedValues) {
  STATIC_ASSERT_TRUE(cmp_greater(TestFixture::unsigned_zero, TestFixture::signed_small_negative));
  STATIC_ASSERT_TRUE(cmp_greater(TestFixture::unsigned_small_positive, TestFixture::signed_small_negative));
  STATIC_ASSERT_TRUE(cmp_greater(TestFixture::unsigned_max_positive, TestFixture::signed_small_negative));

  STATIC_ASSERT_TRUE(cmp_greater(TestFixture::unsigned_zero, TestFixture::signed_max_negative));
  STATIC_ASSERT_TRUE(cmp_greater(TestFixture::unsigned_small_positive, TestFixture::signed_max_negative));
  STATIC_ASSERT_TRUE(cmp_greater(TestFixture::unsigned_max_positive, TestFixture::signed_max_negative));
}
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value is @c true and @c std::is_signed<Int2>::value is @c false , and given @c int1>=0 ,
/// then @c arene::base::cmp_greater(int1,int2) is equivalent to @c int1>int2 with @c int1 cast to an equivalent
/// unsigned type.
TYPED_TEST(CmpGreaterTest, LhsOperandSignedAndPositiveComparesAsIfViaUnsignedComparisonToAllUnsignedValues) {
  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::signed_zero, TestFixture::unsigned_zero),
      make_unsigned(TestFixture::signed_zero) > TestFixture::unsigned_zero
  );
  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::signed_zero, TestFixture::unsigned_small_positive),
      make_unsigned(TestFixture::signed_zero) > TestFixture::unsigned_small_positive
  );
  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::signed_zero, TestFixture::unsigned_max_positive),
      make_unsigned(TestFixture::signed_zero) > TestFixture::unsigned_max_positive
  );

  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::signed_small_positive, TestFixture::unsigned_zero),
      make_unsigned(TestFixture::signed_small_positive) > TestFixture::unsigned_zero
  );
  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::signed_small_positive, TestFixture::unsigned_small_positive),
      make_unsigned(TestFixture::signed_small_positive) > TestFixture::unsigned_small_positive
  );
  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::signed_small_positive, TestFixture::unsigned_max_positive),
      make_unsigned(TestFixture::signed_small_positive) > TestFixture::unsigned_max_positive
  );

  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::signed_max_positive, TestFixture::unsigned_zero),
      make_unsigned(TestFixture::signed_max_positive) > TestFixture::unsigned_zero
  );
  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::signed_max_positive, TestFixture::unsigned_small_positive),
      make_unsigned(TestFixture::signed_max_positive) > TestFixture::unsigned_small_positive
  );
  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::signed_max_positive, TestFixture::unsigned_max_positive),
      make_unsigned(TestFixture::signed_max_positive) > TestFixture::unsigned_max_positive
  );
}
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value is @c false and @c std::is_signed<Int2>::value is @c true , and given @c int2>=0 ,
/// then @c arene::base::cmp_greater(int1,int2) is equivalent to @c int1>int2 with @c int2 cast to an equivalent
/// unsigned type.
TYPED_TEST(CmpGreaterTest, RhsOperandSignedAndPositiveComparesAsIfViaUnsignedComparisonToAllUnsignedValues) {
  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::unsigned_zero, TestFixture::signed_zero),
      TestFixture::unsigned_zero > make_unsigned(TestFixture::signed_zero)
  );
  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::unsigned_small_positive, TestFixture::signed_zero),
      TestFixture::unsigned_small_positive > make_unsigned(TestFixture::signed_zero)
  );
  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::unsigned_max_positive, TestFixture::signed_zero),
      TestFixture::unsigned_max_positive > make_unsigned(TestFixture::signed_zero)
  );

  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::unsigned_zero, TestFixture::signed_small_positive),
      TestFixture::unsigned_zero > make_unsigned(TestFixture::signed_small_positive)
  );
  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::unsigned_small_positive, TestFixture::signed_small_positive),
      TestFixture::unsigned_small_positive > make_unsigned(TestFixture::signed_small_positive)
  );
  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::unsigned_max_positive, TestFixture::signed_small_positive),
      TestFixture::unsigned_max_positive > make_unsigned(TestFixture::signed_small_positive)
  );

  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::unsigned_zero, TestFixture::signed_max_positive),
      TestFixture::unsigned_zero > make_unsigned(TestFixture::signed_max_positive)
  );
  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::unsigned_small_positive, TestFixture::signed_max_positive),
      TestFixture::unsigned_small_positive > make_unsigned(TestFixture::signed_max_positive)
  );
  STATIC_ASSERT_EQ(
      cmp_greater(TestFixture::unsigned_max_positive, TestFixture::signed_max_positive),
      TestFixture::unsigned_max_positive > make_unsigned(TestFixture::signed_max_positive)
  );
}

template <typename T>
class CmpGreaterEqualTest : public SafeCmpTest<T> {};

TYPED_TEST_SUITE(CmpGreaterEqualTest, integer_types, );
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value==std::is_signed<Int2>::value , then @c arene::base::cmp_greater_equal(int1,int2) is
/// equivalent to @c int1>=int2 .
TYPED_TEST(CmpGreaterEqualTest, OperandsOfSameSignednessCompareAsIfDirectComparison) {
  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::signed_zero, TestFixture::signed_zero),
      TestFixture::signed_zero >= TestFixture::signed_zero
  );
  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::unsigned_zero, TestFixture::unsigned_zero),
      TestFixture::unsigned_zero >= TestFixture::unsigned_zero
  );

  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::signed_small_positive, TestFixture::signed_small_positive),
      TestFixture::signed_small_positive >= TestFixture::signed_small_positive
  );
  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::unsigned_small_positive, TestFixture::unsigned_small_positive),
      TestFixture::unsigned_small_positive >= TestFixture::unsigned_small_positive
  );

  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::signed_max_positive, TestFixture::signed_max_positive),
      TestFixture::signed_max_positive >= TestFixture::signed_max_positive
  );
  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::signed_max_positive, TestFixture::signed_max_positive),
      TestFixture::signed_max_positive >= TestFixture::signed_max_positive
  );

  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::signed_small_negative, TestFixture::signed_small_negative),
      TestFixture::signed_small_negative >= TestFixture::signed_small_negative
  );
  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::signed_max_negative, TestFixture::signed_max_negative),
      TestFixture::signed_max_negative >= TestFixture::signed_max_negative
  );

  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::signed_zero, TestFixture::signed_small_negative),
      TestFixture::signed_zero >= TestFixture::signed_small_negative
  );
  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::unsigned_zero, TestFixture::unsigned_small_positive),
      TestFixture::unsigned_zero >= TestFixture::unsigned_small_positive
  );
}
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value is @c true and @c std::is_signed<Int2>::value is @c false , and given @c int1<0 ,
/// then @c arene::base::cmp_greater_equal(int1,int2) returns @c false for all @c int1.
TYPED_TEST(CmpGreaterEqualTest, LhsOperandSignedAndNegativeComparesFalseToAllUnsignedValues) {
  STATIC_ASSERT_FALSE(cmp_greater_equal(TestFixture::signed_small_negative, TestFixture::unsigned_zero));
  STATIC_ASSERT_FALSE(cmp_greater_equal(TestFixture::signed_small_negative, TestFixture::unsigned_small_positive));
  STATIC_ASSERT_FALSE(cmp_greater_equal(TestFixture::signed_small_negative, TestFixture::unsigned_max_positive));

  STATIC_ASSERT_FALSE(cmp_greater_equal(TestFixture::signed_max_negative, TestFixture::unsigned_zero));
  STATIC_ASSERT_FALSE(cmp_greater_equal(TestFixture::signed_max_negative, TestFixture::unsigned_small_positive));
  STATIC_ASSERT_FALSE(cmp_greater_equal(TestFixture::signed_max_negative, TestFixture::unsigned_max_positive));
}
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value is @c false and @c std::is_signed<Int2>::value is @c true , and given @c int2<0 ,
/// then @c arene::base::cmp_greater_equal(int1,int2) returns @c true for all @c int2.
TYPED_TEST(CmpGreaterEqualTest, RhsOperandSignedAndNegativeComparesTrueToAllUnsignedValues) {
  STATIC_ASSERT_TRUE(cmp_greater_equal(TestFixture::unsigned_zero, TestFixture::signed_small_negative));
  STATIC_ASSERT_TRUE(cmp_greater_equal(TestFixture::unsigned_small_positive, TestFixture::signed_small_negative));
  STATIC_ASSERT_TRUE(cmp_greater_equal(TestFixture::unsigned_max_positive, TestFixture::signed_small_negative));

  STATIC_ASSERT_TRUE(cmp_greater_equal(TestFixture::unsigned_zero, TestFixture::signed_max_negative));
  STATIC_ASSERT_TRUE(cmp_greater_equal(TestFixture::unsigned_small_positive, TestFixture::signed_max_negative));
  STATIC_ASSERT_TRUE(cmp_greater_equal(TestFixture::unsigned_max_positive, TestFixture::signed_max_negative));
}
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value is @c true and @c std::is_signed<Int2>::value is @c false , and given @c int1>=0 ,
/// then @c arene::base::cmp_greater_equal(int1,int2) is equivalent to @c int1>=int2 with @c int1 cast to an equivalent
/// unsigned type.
TYPED_TEST(CmpGreaterEqualTest, LhsOperandSignedAndPositiveComparesAsIfViaUnsignedComparisonToAllUnsignedValues) {
  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::signed_zero, TestFixture::unsigned_zero),
      make_unsigned(TestFixture::signed_zero) >= TestFixture::unsigned_zero
  );
  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::signed_zero, TestFixture::unsigned_small_positive),
      make_unsigned(TestFixture::signed_zero) >= TestFixture::unsigned_small_positive
  );
  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::signed_zero, TestFixture::unsigned_max_positive),
      make_unsigned(TestFixture::signed_zero) >= TestFixture::unsigned_max_positive
  );

  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::signed_small_positive, TestFixture::unsigned_zero),
      make_unsigned(TestFixture::signed_small_positive) >= TestFixture::unsigned_zero
  );
  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::signed_small_positive, TestFixture::unsigned_small_positive),
      make_unsigned(TestFixture::signed_small_positive) >= TestFixture::unsigned_small_positive
  );
  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::signed_small_positive, TestFixture::unsigned_max_positive),
      make_unsigned(TestFixture::signed_small_positive) >= TestFixture::unsigned_max_positive
  );

  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::signed_max_positive, TestFixture::unsigned_zero),
      make_unsigned(TestFixture::signed_max_positive) >= TestFixture::unsigned_zero
  );
  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::signed_max_positive, TestFixture::unsigned_small_positive),
      make_unsigned(TestFixture::signed_max_positive) >= TestFixture::unsigned_small_positive
  );
  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::signed_max_positive, TestFixture::unsigned_max_positive),
      make_unsigned(TestFixture::signed_max_positive) >= TestFixture::unsigned_max_positive
  );
}
/// @test Given two integral operands, <c>Int1 int1</c> and <c>Int2 int2</c>, where,
/// @c std::is_signed<Int1>::value is @c false and @c std::is_signed<Int2>::value is @c true , and given @c int2>=0 ,
/// then @c arene::base::cmp_greater_equal(int1,int2) is equivalent to @c int1>=int2 with @c int2 cast to an equivalent
/// unsigned type.
TYPED_TEST(CmpGreaterEqualTest, RhsOperandSignedAndPositiveComparesAsIfViaUnsignedComparisonToAllUnsignedValues) {
  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::unsigned_zero, TestFixture::signed_zero),
      TestFixture::unsigned_zero >= make_unsigned(TestFixture::signed_zero)
  );
  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::unsigned_small_positive, TestFixture::signed_zero),
      TestFixture::unsigned_small_positive >= make_unsigned(TestFixture::signed_zero)
  );
  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::unsigned_max_positive, TestFixture::signed_zero),
      TestFixture::unsigned_max_positive >= make_unsigned(TestFixture::signed_zero)
  );

  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::unsigned_zero, TestFixture::signed_small_positive),
      TestFixture::unsigned_zero >= make_unsigned(TestFixture::signed_small_positive)
  );
  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::unsigned_small_positive, TestFixture::signed_small_positive),
      TestFixture::unsigned_small_positive >= make_unsigned(TestFixture::signed_small_positive)
  );
  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::unsigned_max_positive, TestFixture::signed_small_positive),
      TestFixture::unsigned_max_positive >= make_unsigned(TestFixture::signed_small_positive)
  );

  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::unsigned_zero, TestFixture::signed_max_positive),
      TestFixture::unsigned_zero >= make_unsigned(TestFixture::signed_max_positive)
  );
  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::unsigned_small_positive, TestFixture::signed_max_positive),
      TestFixture::unsigned_small_positive >= make_unsigned(TestFixture::signed_max_positive)
  );
  STATIC_ASSERT_EQ(
      cmp_greater_equal(TestFixture::unsigned_max_positive, TestFixture::signed_max_positive),
      TestFixture::unsigned_max_positive >= make_unsigned(TestFixture::signed_max_positive)
  );
}

}  // namespace
