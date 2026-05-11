// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/initializer_list"

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/cstdint"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

class some_class {
  std::int32_t val_;

 public:
  explicit constexpr some_class(std::int32_t value) noexcept
      : val_(value) {}

  friend constexpr auto operator==(some_class const& lhs, some_class const& rhs) noexcept -> bool {
    return lhs.val_ == rhs.val_;
  }
};

using init_list_types = arene::base::type_lists::concat_unique_t<
    testing::signed_integer_types,
    testing::unsigned_integer_types,
    testing::enum_types,
    testing::floating_point_types,
    testing::Types<some_class>>;

template <typename T>
class InitializerList : public testing::Test {};

TYPED_TEST_SUITE(InitializerList, init_list_types, );

/// @test Can construct an initializer list from empty braces
CONSTEXPR_TYPED_TEST(InitializerList, CanConstructFromEmptyBraces) {
  std::initializer_list<TypeParam> const list{};
  static_cast<void>(list);
}

/// @test Constructing an initializer list from empty braces is noexcept
TYPED_TEST(InitializerList, DefaultInitIsNoexcept) {
  static_assert(noexcept(std::initializer_list<TypeParam>{}), "Must be noexcept");
}

/// @test Constructing an initializer list from empty braces yields an empty list
CONSTEXPR_TYPED_TEST(InitializerList, EmptyInitListHasEqualBeginAndEnd) {
  std::initializer_list<TypeParam> const list{};
  ASSERT_EQ(list.begin(), list.end());
}

/// @test Begin and End are noexcept
TYPED_TEST(InitializerList, BeginAndEndAreNoexcept) {
  static_assert(noexcept(std::initializer_list<TypeParam>{}.begin()), "Must be noexcept");
  static_assert(noexcept(std::initializer_list<TypeParam>{}.end()), "Must be noexcept");
}

/// @test Begin and End return pointers to the element type
TYPED_TEST(InitializerList, BeginAndEndReturnPointers) {
  testing::StaticAssertTypeEq<decltype(std::declval<std::initializer_list<TypeParam>&>().begin()), TypeParam const*>();
  testing::
      StaticAssertTypeEq<decltype(std::declval<std::initializer_list<TypeParam> const&>().begin()), TypeParam const*>();
  testing::StaticAssertTypeEq<decltype(std::declval<std::initializer_list<TypeParam>&>().end()), TypeParam const*>();
  testing::
      StaticAssertTypeEq<decltype(std::declval<std::initializer_list<TypeParam> const&>().end()), TypeParam const*>();
}

/// @test Constructing an initializer list from empty braces yields a size of zero
CONSTEXPR_TYPED_TEST(InitializerList, EmptyInitListHasSizeOfZero) {
  std::initializer_list<TypeParam> const list{};
  ASSERT_EQ(list.size(), 0U);
}

/// @test Getting the size is noexcept
TYPED_TEST(InitializerList, SizeIsNoexcept) {
  static_assert(noexcept(std::initializer_list<TypeParam>{}.size()), "Must be noexcept");
}

/// @test Constructing an initializer list from a brace init list of elements yields a range of the right size
CONSTEXPR_TYPED_TEST(InitializerList, CanConstructFromBraceInitList) {
  TypeParam const element1{static_cast<TypeParam>(1)};
  TypeParam const element2{static_cast<TypeParam>(2)};
  TypeParam const element3{static_cast<TypeParam>(3)};
  TypeParam const element4{static_cast<TypeParam>(4)};
  std::initializer_list<TypeParam> const list{element1, element2, element3, element4};
  ASSERT_EQ(list.size(), 4U);
  ASSERT_NE(list.begin(), list.end());
  ASSERT_EQ(list.begin() + list.size(), list.end());
}

/// @test Constructing an initializer list from a brace init list of elements yields a range with the right elements
CONSTEXPR_TYPED_TEST(InitializerList, ConstructedFromBraceInitListHasRightContents) {
  TypeParam const element1{static_cast<TypeParam>(10)};
  TypeParam const element2{static_cast<TypeParam>(20)};
  TypeParam const element3{static_cast<TypeParam>(30)};
  TypeParam const element4{static_cast<TypeParam>(40)};
  TypeParam const element5{static_cast<TypeParam>(50)};

  std::initializer_list<TypeParam> const list{element1, element2, element3, element4, element5};

  ASSERT_EQ(list.size(), 5U);
  ASSERT_EQ(list.begin() + list.size(), list.end());

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Test code: float equal comparison is on purpose");
  ASSERT_EQ(list.begin()[0], element1);
  ASSERT_EQ(list.begin()[1], element2);
  ASSERT_EQ(list.begin()[2], element3);
  ASSERT_EQ(list.begin()[3], element4);
  ASSERT_EQ(list.begin()[4], element5);
  ARENE_IGNORE_END();
}

/// @test Constructing an initializer list from a brace init list of elements with auto yields a list of the right size
/// with the right type and contents
CONSTEXPR_TYPED_TEST(InitializerList, ConstructedAutoFromBraceInitListHasRightContents) {
  TypeParam const element1{static_cast<TypeParam>(10)};
  TypeParam const element2{static_cast<TypeParam>(20)};
  TypeParam const element3{static_cast<TypeParam>(30)};
  TypeParam const element4{static_cast<TypeParam>(40)};
  TypeParam const element5{static_cast<TypeParam>(50)};

  auto const list = {element1, element2, element3, element4, element5};

#if ARENE_IS_ON(ARENE_COMPILER_GCC) && (__GNUC__ < 9)
  testing::StaticAssertTypeEq<decltype(list), std::initializer_list<TypeParam const> const>();
#else
  testing::StaticAssertTypeEq<decltype(list), std::initializer_list<TypeParam> const>();
#endif

  ASSERT_EQ(list.size(), 5U);
  ASSERT_EQ(list.begin() + list.size(), list.end());

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Test code: float equal comparison is on purpose");
  ASSERT_EQ(list.begin()[0], element1);
  ASSERT_EQ(list.begin()[1], element2);
  ASSERT_EQ(list.begin()[2], element3);
  ASSERT_EQ(list.begin()[3], element4);
  ASSERT_EQ(list.begin()[4], element5);
  ARENE_IGNORE_END();
}

/// @test Initializer list has the correct member type aliases
TYPED_TEST(InitializerList, HasRightTypeAliases) {
  testing::StaticAssertTypeEq<typename std::initializer_list<TypeParam>::value_type, TypeParam>();
  testing::StaticAssertTypeEq<typename std::initializer_list<TypeParam>::reference, TypeParam const&>();
  testing::StaticAssertTypeEq<typename std::initializer_list<TypeParam>::const_reference, TypeParam const&>();
  testing::StaticAssertTypeEq<typename std::initializer_list<TypeParam>::size_type, std::size_t>();
  testing::StaticAssertTypeEq<typename std::initializer_list<TypeParam>::iterator, TypeParam const*>();
  testing::StaticAssertTypeEq<typename std::initializer_list<TypeParam>::const_iterator, TypeParam const*>();
}

}  // namespace
