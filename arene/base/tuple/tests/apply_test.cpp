// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/tuple/apply.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"

namespace {

using ::arene::base::apply;

/// @test `apply` works with an empty tuple and a function taking no argument.
TEST(TupleApply, SuppliedFunctionWithoutNoArgumentsMustWorksWithEmptyTuple) {
  auto empty = std::tuple<>{};
  auto res = apply([]() -> int { return 42; }, empty);
  EXPECT_EQ(res, 42);
}

/// @test `apply` works with a function taking one argument.
TEST(TupleApply, SuppliedFuncCanBeInvokedWithOneArgument) {
  auto tuple_with_one_argument = std::tuple<int>{111};
  auto const res = apply([](int value) { return value * 2; }, tuple_with_one_argument);

  EXPECT_EQ(res, 222);
}

/// @test `apply` works with a function taking two arguments.
TEST(TupleApply, SuppliedFuncCanBeInvokedWithTwoArgument) {
  auto tuple_with_two_argument = std::tuple<int, int>{111, 3};
  auto const res = apply([](int value_a, int value_b) { return value_a * value_b * 2; }, tuple_with_two_argument);

  EXPECT_EQ(res, 666);
}

/// @test `apply` works with 'arene::base::array' and a function taking no argument.
TEST(TupleApply, SuppliedFunctionWithoutNoArgumentsMustWorksWithEmptyArray) {
  auto empty = arene::base::array<int, 0>{};
  auto res = apply([]() -> int { return 42; }, empty);
  EXPECT_EQ(res, 42);
}

/// @test `apply` works with 'arene::base::array' and a function taking one argument.
TEST(TupleApply, SuppliedFuncCanBeInvokedWithOneArgumentAndArray) {
  auto tuple_with_one_argument = arene::base::array<int, 1>{111};
  auto const res = apply([](int value) { return value * 2; }, tuple_with_one_argument);

  EXPECT_EQ(res, 222);
}

/// @test `apply` works with 'arene::base::array' and a function taking two arguments.
TEST(TupleApply, SuppliedFuncCanBeInvokedWithTwoArgumentAndArray) {
  auto tuple_with_two_argument = arene::base::array<int, 2>{111, 3};
  auto const res = apply([](int value_a, int value_b) { return value_a * value_b * 2; }, tuple_with_two_argument);

  EXPECT_EQ(res, 666);
}

/// @test `apply` supports lvalue reference functions.
TEST(TupleApply, SuppliedFunctionCanBeLValueReference) {
  auto empty = std::tuple<>{};

  auto lambda = []() -> int { return 41; };
  auto res = apply(lambda, empty);
  static_assert(
      std::is_same<decltype(res), decltype(lambda())>::value,
      "return type of the function must be same as return_type of apply itself"
  );
  EXPECT_EQ(res, 41);
}

auto simple_function_taking_no_arguments() -> int { return 17; }

/// @test `apply` supports function pointers.
TEST(TupleApply, SuppliedFunctionCanBeAFunctionPointer) {
  auto empty = std::tuple<>{};

  // we want to support function pointers too
  int (*fnc_ptr)() = &simple_function_taking_no_arguments;

  auto res = apply(simple_function_taking_no_arguments, empty);
  static_assert(
      std::is_same<decltype(res), decltype(fnc_ptr())>::value,
      "return type of the function must be same as return_type of apply itself"
  );

  EXPECT_EQ(res, 17);
}

struct pair_of_ints {
  int first;
  int second;

  friend auto operator==(pair_of_ints lhs, pair_of_ints rhs) -> bool {
    return lhs.first == rhs.first && lhs.second == rhs.second;
  }
};

auto function_returning_something_else_than_int() -> pair_of_ints { return {31, 23}; }

/// @test `apply` may be invoked with a function with any return type.
TEST(TupleApply, SuppliedFunctionMustBeAbleToReturnOtherTypeThanInt) {
  auto empty = std::tuple<>{};

  // different non-void type than int must be supported too
  auto res = apply(function_returning_something_else_than_int, empty);
  static_assert(
      std::is_same<decltype(res), decltype(function_returning_something_else_than_int())>::value,
      "return type of the function must be same as return_type of apply itself"
  );

  auto const expected_res = pair_of_ints{31, 23};
  EXPECT_EQ(res, expected_res);
}

/// @test `apply` supports void functions.
TEST(TupleApply, SuppliedFunctionMustBeAbleToReturnVoid) {
  auto two_ints = std::tuple<int, int>{1, 2};

  bool called = false;
  auto lambda = [&](int arg1, int arg2) -> void {
    EXPECT_EQ(arg1, 1);
    EXPECT_EQ(arg2, 2);
    called = true;
  };

  // different non-void type than int must be supported too
  apply(lambda, two_ints);
  static_assert(
      std::is_same<void, decltype(apply(lambda, two_ints))>::value,
      "return type of the apply with function must be void"
  );

  EXPECT_TRUE(called);
}

struct pass {
  template <typename T>
  auto operator()(T&& value) const noexcept -> decltype(auto) {
    return std::forward<decltype(value)>(value);
  }
};

/// @test `apply` preserves value categories for lvalue references.
TEST(TupleApply, RespectingValueCategoriesOfTupleWithLRef) {
  int value = 1234;
  auto tpl = std::tuple<int&>{value};
  auto&& res = apply(pass{}, tpl);
  ::testing::StaticAssertTypeEq<decltype(res), int&>();
  EXPECT_EQ(res, 1234);
}

/// @test `apply` preserves value categories for rvalue references.
TEST(TupleApply, RespectingValueCategoriesOfTupleWithRRef) {
  int value = 2345;
  auto tpl = std::tuple<int&&>{static_cast<int&&>(value)};
  auto&& res = apply(pass{}, tpl);
  ::testing::StaticAssertTypeEq<decltype(res), int&>();  // it's just a reference, as the tuple wasn't moved in
  EXPECT_EQ(res, 2345);
}

/// @test `apply` preserves value categories for const lvalue references.
TEST(TupleApply, RespectingValueCategoriesOfTupleWithConstLRef) {
  int const value = 3456;
  auto tpl = std::tuple<int const&>{value};
  auto&& res = apply(pass{}, tpl);
  ::testing::StaticAssertTypeEq<decltype(res), int const&>();
  EXPECT_EQ(res, 3456);
}

/// @test `apply` preserves value categories for const rvalue references.
TEST(TupleApply, RespectingValueCategoriesOfTupleWithConstRRef) {
  int const value = 4567;
  auto tpl = std::tuple<int const&&>{static_cast<int const&&>(value)};
  auto&& res = apply(pass{}, tpl);
  ::testing::StaticAssertTypeEq<decltype(res), int const&>();  // it's just a reference, as the tuple wasn't moved in
  EXPECT_EQ(res, 4567);
}

/// @test `apply` preserves value categories for moved-in tuples with lvalue references.
TEST(TupleApply, RespectingValueCategoriesOfMovedInTupleWithLRef) {
  int value = 1234;
  auto tpl = std::tuple<int&>{value};
  // NOLINTNEXTLINE(hicpp-move-const-arg)
  auto&& res = apply(pass{}, std::move(tpl));
  ::testing::StaticAssertTypeEq<decltype(res), int&>();
  EXPECT_EQ(res, 1234);
}

/// @test `apply` preserves value categories for moved-in tuples with rvalue references.
TEST(TupleApply, RespectingValueCategoriesOfMovedInTupleWithRRef) {
  int value = 2345;
  auto tpl = std::tuple<int&&>{static_cast<int&&>(value)};
  auto&& res = apply(pass{}, std::move(tpl));
  ::testing::StaticAssertTypeEq<decltype(res), int&&>();
  EXPECT_EQ(res, 2345);
}

/// @test `apply` preserves value categories for moved-in tuples with const lvalue references.
TEST(TupleApply, RespectingValueCategoriesOfMovedInTupleWithConstLRef) {
  int const value = 3456;
  auto tpl = std::tuple<int const&>{value};
  // NOLINTNEXTLINE(hicpp-move-const-arg)
  auto&& res = apply(pass{}, std::move(tpl));
  ::testing::StaticAssertTypeEq<decltype(res), int const&>();
  EXPECT_EQ(res, 3456);
}

/// @test `apply` preserves value categories for moved-in tuples with const rvalue references.
TEST(TupleApply, RespectingValueCategoriesOfMovedInTupleWithConstRRef) {
  int const value = 4567;
  auto tpl = std::tuple<int const&&>{static_cast<int const&&>(value)};
  auto&& res = apply(pass{}, std::move(tpl));
  ::testing::StaticAssertTypeEq<decltype(res), int const&&>();
  EXPECT_EQ(res, 4567);
}

struct result_type {
  int value;
};

/// @test `apply` preserves multiple different value categories.
TEST(TupleApply, RespectingMultipleDifferentValueCategories) {
  int val1 = 1;
  int val2 = 2;
  int const val3 = 3;
  int const val4 = 4;

  auto five_refs = std::tuple<int&, int&&, int const&, int const&&>{
      val1,
      static_cast<int&&>(val2),
      val3,
      static_cast<int const&&>(val4)
  };

  bool flag_was_called = false;
  auto lambda_accepting_only_proper_categories = [&](int& arg1, int&& arg2, int const& arg3, int const&& arg4) {
    flag_was_called = true;
    EXPECT_TRUE(&arg1 == &val1);
    EXPECT_TRUE(&arg2 == &val2);
    EXPECT_TRUE(&arg3 == &val3);
    EXPECT_TRUE(&arg4 == &val4);

    EXPECT_EQ(arg1, 1);
    EXPECT_EQ(arg2, 2);
    EXPECT_EQ(arg3, 3);
    EXPECT_EQ(arg4, 4);

    return result_type{arg1 + arg2 + arg3 + arg4};
  };

  auto result = apply(lambda_accepting_only_proper_categories, std::move(five_refs));

  ::testing::StaticAssertTypeEq<decltype(result), result_type>();
  EXPECT_EQ(result.value, 10);

  EXPECT_TRUE(flag_was_called);
}

struct simple_object_with_two_members {
  int value_a;
  int value_b;
};

/// @test `apply` supports member data pointers.
TEST(TupleApply, SuppliedFunctionCanBeAMemberDataPointer) {
  auto object = simple_object_with_two_members{5, 6};
  auto my_tuple = std::tuple<simple_object_with_two_members&>{object};

  using member_ptr = int simple_object_with_two_members::*;

  member_ptr member_data_pointer = &simple_object_with_two_members::value_a;

  auto&& ref = apply(member_data_pointer, my_tuple);
  ::testing::StaticAssertTypeEq<decltype(ref), int&>();

  EXPECT_EQ(ref, 5);

  object.value_a = 7;

  EXPECT_EQ(ref, 7);

  member_data_pointer = &simple_object_with_two_members::value_b;
  auto&& ref2 = apply(member_data_pointer, my_tuple);
  ::testing::StaticAssertTypeEq<decltype(ref2), int&>();

  EXPECT_EQ(ref2, 6);

  object.value_b = 8;

  EXPECT_EQ(ref2, 8);
}

struct object_with_object_inside {
  struct storage {
    int value{42};
  };

  storage data{};
};

/// @test `apply` can be invoked with a function that consumes arguments of different types than those held in the
/// tuple, so long as they are implicitly convertible from those types.
TEST(TupleApply, SuppliedFunctionCanBeAMemberDataPointerToOtherTypeThanFloat) {
  auto object = object_with_object_inside{};
  auto my_tuple = std::tuple<object_with_object_inside&>{object};

  auto member_data_pointer = &object_with_object_inside::data;

  auto&& ref = apply(member_data_pointer, my_tuple);
  ::testing::StaticAssertTypeEq<decltype(ref), object_with_object_inside::storage&>();

  EXPECT_EQ(ref.value, 42);

  object.data.value = 43;

  EXPECT_EQ(ref.value, 43);
}

struct simple_object {
  int test_value = 0;

  auto function_a() -> int {
    test_value = 17;
    return 11;
  }
  auto function_b() -> int {
    test_value = 23;
    return 13;
  }
  auto function_c(int input) -> int {
    test_value = input * 2;
    return input * 3;
  }
  auto function_d(int input) -> int {
    test_value = input * 3;
    return input * 5;
  }
};

/// @test `apply` supports member function pointers.
TEST(TupleApply, SupliedFunctionCanBeAMemberFunctionPointer) {
  auto object = simple_object{};
  auto my_tuple = std::tuple<simple_object&>{object};

  auto member_function_pointer = &simple_object::function_a;
  auto res1 = apply(member_function_pointer, my_tuple);
  EXPECT_EQ(res1, 11);
  EXPECT_EQ(object.test_value, 17);

  auto another_member_function_pointer = &simple_object::function_b;
  auto res2 = apply(another_member_function_pointer, my_tuple);
  EXPECT_EQ(res2, 13);
  EXPECT_EQ(object.test_value, 23);
}

/// @test `apply` supports member function pointers with arguments.
TEST(TupleApply, SupliedFunctionCanBeAMemberFunctionPointerWithArguments) {
  auto object = simple_object{};
  auto my_tuple = std::tuple<simple_object&, int>{object, 11};

  auto member_function_pointer = &simple_object::function_c;
  // functions pointers
  auto res1 = apply(member_function_pointer, my_tuple);
  EXPECT_EQ(res1, 33);
  EXPECT_EQ(object.test_value, 22);

  auto another_member_function_pointer = &simple_object::function_d;
  auto res2 = apply(another_member_function_pointer, my_tuple);
  EXPECT_EQ(res2, 55);
  EXPECT_EQ(object.test_value, 33);
}

template <typename... Ts>
auto return_number_of_arguments(Ts...) -> int {
  return static_cast<int>(sizeof...(Ts));
}

/// @test `apply` works with functions taking many arguments.
TEST(TupleApply, SuppliedFuncCanBeInvokedWithManyArgument) {
  auto tuple_with_many_arguments = std::tuple<
      int,
      int,
      int,
      int,
      int,
      int,
      int,
      int,
      int,
      int,
      int,
      int,
      int,
      int,
      int,
      int,
      int,
      int,
      int,
      int,
      int,
      int,
      int,
      int>{};

  auto const res1 = apply(
      return_number_of_arguments<
          int,
          int,
          int,
          int,
          int,
          int,
          int,
          int,
          int,
          int,
          int,
          int,
          int,
          int,
          int,
          int,
          int,
          int,
          int,
          int,
          int,
          int,
          int,
          int>,
      tuple_with_many_arguments
  );

  EXPECT_EQ(res1, 24);
}

/// @test `apply` allows observation of the exceptions a function throws.
TEST(TupleApply, ExceptnessOfSuppliedFunctionIsObservable) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)
  auto empty = std::tuple<>{};

  struct local_error {};

  auto const throwing_lambda = []() {
    // NOLINTNEXTLINE(hicpp-exception-baseclass)
    throw local_error();
  };
  bool const throws_exception = !noexcept(apply(throwing_lambda, empty));

  EXPECT_TRUE(throws_exception);
  ASSERT_THROW(apply(throwing_lambda, empty), local_error);

  auto const nonthrowing_lambda = []() noexcept { return 42; };
  bool const without_exception = noexcept(apply(nonthrowing_lambda, empty));

  EXPECT_TRUE(without_exception);
  ASSERT_NO_THROW(apply(nonthrowing_lambda, empty));
#else
  GTEST_SKIP() << "Testing exceptions makes no sense with exceptions disabled";
#endif
}

}  // namespace
