// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/algorithm"
#include "stdlib/include/iterator"
#include "stdlib/include/type_traits"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"
#include "testlibs/utilities/counted_algorithm.hpp"
#include "testlibs/utilities/iterator_types.hpp"
#include "testlibs/utilities/mutate.hpp"

namespace {

template <class Type>
struct add_basic_input_iterator {
  using type = testing::demoted_iterator<Type*, std::input_iterator_tag>;
};

class user_defined_type {
 public:
  constexpr user_defined_type() = default;
  explicit constexpr user_defined_type(int input)
      : value_(input) {}
  constexpr auto operator==(user_defined_type const& rhs) const -> bool { return value_ == rhs.value_; }

 private:
  int value_{};
};

using base_types =
    arene::base::type_lists::concat_unique_t<::testing::integral_types, ::testing::Types<user_defined_type>>;
using count_if_types = ::testing::mutate_t<base_types, std::add_pointer, add_basic_input_iterator>;

template <typename T>
class CountIfTest : public testing::Test {};

TYPED_TEST_SUITE(CountIfTest, count_if_types, );

/// @test @c count_if returns @c std::iterator_traits<T>::difference_type as the return type
TYPED_TEST(CountIfTest, HasRightReturnType) {
  using testing::always_true;

  testing::StaticAssertTypeEq<
      decltype(std::count_if(std::declval<TypeParam>(), std::declval<TypeParam>(), always_true)),
      typename std::iterator_traits<TypeParam>::difference_type>();
  testing::StaticAssertTypeEq<
      decltype(std::count_if(std::declval<TypeParam&>(), std::declval<TypeParam&>(), always_true)),
      typename std::iterator_traits<TypeParam>::difference_type>();
  testing::StaticAssertTypeEq<
      decltype(std::count_if(std::declval<TypeParam&&>(), std::declval<TypeParam&&>(), always_true)),
      typename std::iterator_traits<TypeParam>::difference_type>();

  testing::StaticAssertTypeEq<
      decltype(std::count_if(std::declval<TypeParam const>(), std::declval<TypeParam const>(), always_true)),
      typename std::iterator_traits<TypeParam>::difference_type>();
  testing::StaticAssertTypeEq<
      decltype(std::count_if(std::declval<TypeParam const&>(), std::declval<TypeParam const&>(), always_true)),
      typename std::iterator_traits<TypeParam>::difference_type>();
  testing::StaticAssertTypeEq<
      decltype(std::count_if(std::declval<TypeParam const&&>(), std::declval<TypeParam const&&>(), always_true)),
      typename std::iterator_traits<TypeParam>::difference_type>();
}

/// @test @c count_if is not invocable if one of the inputs is not an iterator
TYPED_TEST(CountIfTest, NotInvocableIfNotAnIterator) {
  using testing::always_true;

  struct not_an_iterator {};
  auto const lifted_count_if = FUNCTION_LIFT(std::count_if);

  EXPECT_TRUE(::testing::simple_is_invocable_v<decltype(lifted_count_if), TypeParam, TypeParam, decltype(always_true)>);
  EXPECT_FALSE(::testing::
                   simple_is_invocable_v<decltype(lifted_count_if), TypeParam, not_an_iterator, decltype(always_true)>);
  EXPECT_FALSE(::testing::
                   simple_is_invocable_v<decltype(lifted_count_if), not_an_iterator, TypeParam, decltype(always_true)>);
  EXPECT_FALSE(::testing::simple_is_invocable_v<
               decltype(lifted_count_if),
               not_an_iterator,
               not_an_iterator,
               decltype(always_true)>);
}

struct predicate_returns_type_not_convertible_to_bool {
  struct not_convertible_to_bool {};

  template <class T>
  constexpr auto operator()(T const&) const -> not_convertible_to_bool;
};

/// @test @c count_if is not invocable if the predicate return type is not convertible to bool
TYPED_TEST(CountIfTest, NotInvocableIfPredicateReturnTypeIsNotConvertibleToBool) {
  auto const lifted_count_if = FUNCTION_LIFT(std::count_if);
  EXPECT_FALSE(::testing::simple_is_invocable_v<
               decltype(lifted_count_if),
               TypeParam,
               TypeParam,
               predicate_returns_type_not_convertible_to_bool>);
}

template <bool Result>
struct predicate_returns_type_convertible_to_bool {
  struct convertible_to_bool {
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    constexpr operator bool() const { return Result; }
  };

  template <class T>
  constexpr auto operator()(T const&) const -> convertible_to_bool {
    return convertible_to_bool{};
  }
};

template <int Result>
struct predicate_returns_type_implicitly_convertible_to_bool {
  template <class T>
  constexpr auto operator()(T const&) const -> int {
    return Result;
  }
};

/// @test @c count_if correctly counts all of the elements or none of the elements in the range
CONSTEXPR_TYPED_TEST(CountIfTest, EdgeCasesOfRange) {
  using testing::always_false;
  using testing::always_true;
  using element_type = typename std::iterator_traits<TypeParam>::value_type;
  using return_type = typename std::iterator_traits<TypeParam>::difference_type;

  constexpr std::size_t array_size = 10;

  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  element_type const range[array_size] = {};

  EXPECT_EQ(std::count_if(std::begin(range), std::end(range), always_true), return_type{array_size});
  EXPECT_EQ(std::count_if(std::begin(range), std::end(range), always_false), return_type{0});

  EXPECT_EQ(
      std::count_if(std::begin(range), std::end(range), predicate_returns_type_convertible_to_bool<true>{}),
      return_type{array_size}
  );
  EXPECT_EQ(
      std::count_if(std::begin(range), std::end(range), predicate_returns_type_convertible_to_bool<false>{}),
      return_type{0}
  );

  EXPECT_EQ(
      std::count_if(std::begin(range), std::end(range), predicate_returns_type_implicitly_convertible_to_bool<1>{}),
      return_type{array_size}
  );
  EXPECT_EQ(
      std::count_if(std::begin(range), std::end(range), predicate_returns_type_implicitly_convertible_to_bool<0>{}),
      return_type{0}
  );
}

template <class T>
struct equal_to {
  constexpr explicit equal_to(T value)
      : value_(value) {}
  constexpr auto operator()(T const& lhs) const { return lhs == value_; }

 private:
  T value_;
};

/// @test @c count_if correctly counts some of the elements in the range, based on the predicate
CONSTEXPR_TYPED_TEST(CountIfTest, CountsSomeElements) {
  using element_type = typename std::iterator_traits<TypeParam>::value_type;
  using return_type = typename std::iterator_traits<TypeParam>::difference_type;

  constexpr std::size_t array_size = 10;

  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  element_type const range[array_size] = {
      element_type{0},
      element_type{0},
      element_type{1},
      element_type{0},
      element_type{1},
      element_type{1},
      element_type{1},
      element_type{0},
      element_type{1},
      element_type{0}
  };

  EXPECT_EQ(std::count_if(std::begin(range), std::end(range), equal_to<element_type>{element_type{0}}), return_type{5});
}

/// @test @c count_if applies the predicate function the correct number of times
TYPED_TEST(CountIfTest, AppliesPredicateCorrectNumberOfTimes) {
  using element_type = typename std::iterator_traits<TypeParam>::value_type;

  constexpr std::size_t array_size = 10;

  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  element_type const range[array_size] = {};

  auto result = testing::counted_algorithm(  //
      FUNCTION_LIFT(std::count_if),          //
      std::begin(range),                     //
      std::end(range),                       //
      testing::always_true                   //
  );

  ASSERT_EQ(result.applications, array_size);
}

}  // namespace
