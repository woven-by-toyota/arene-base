// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/functional/function_ref.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/conditional.hpp"
#include "arene/base/utility/to_underlying.hpp"

namespace {

using ::arene::base::function_ref;
using ::arene::base::noexcept_function_ref;

/// @brief simple enum specifying if the function should be noexcept or not.
enum class noexcept_specifier : bool { is_noexcept = true, is_not_noexcept = false };
/// @brief simple enum specifying if the function should be const or not.
enum class const_specifier : bool { is_const = true, is_not_const = false };

/// @brief A helper template for producing a `function_ref` specifier of a given const and noexcept qualification.
/// @tparam IsConst If the function is const or not
/// @tparam IsNoexcept If the function is noexcept or not
template <const_specifier IsConst, noexcept_specifier IsNoexcept>
struct function_ref_type {
  /// @brief trait for exposing if the function should be noexcept
  static constexpr bool is_noexcept = ::arene::base::to_underlying(IsNoexcept);
  /// @brief trait for exposing if the function should be const
  static constexpr bool is_const = ::arene::base::to_underlying(IsConst);

  /// @brief Helper alias for producing a `function_ref` with a given signature whose const-ness and noexcept-ness are
  /// set by the outer template.
  /// @tparam Signature The signature of the generated function_ref.
  template <typename Signature>
  using function_ref = arene::base::conditional_t<
      is_const,
      ::arene::base::function_ref<Signature const, is_noexcept>,
      ::arene::base::function_ref<Signature, is_noexcept>>;
};

template <const_specifier IsConst, noexcept_specifier IsNoexcept>
constexpr bool function_ref_type<IsConst, IsNoexcept>::is_noexcept;
template <const_specifier IsConst, noexcept_specifier IsNoexcept>
constexpr bool function_ref_type<IsConst, IsNoexcept>::is_const;

/// @brief A set of function_ref generators which includes all permutations of const/noexcept.
using all_function_refs = ::testing::Types<
    function_ref_type<const_specifier::is_const, noexcept_specifier::is_noexcept>,
    function_ref_type<const_specifier::is_const, noexcept_specifier::is_not_noexcept>,
    function_ref_type<const_specifier::is_not_const, noexcept_specifier::is_noexcept>,
    function_ref_type<const_specifier::is_not_const, noexcept_specifier::is_not_noexcept>>;

/// A functor which implements the add operation
struct add_func {
  constexpr auto operator()(int lhs, int rhs) noexcept { return lhs + rhs; }
  constexpr auto operator()(int lhs, int rhs) const noexcept { return lhs + rhs; }
};

/// A functor which implements the multiply operation
struct multiply_func {
  constexpr auto operator()(int lhs, int rhs) noexcept { return lhs * rhs; }
  constexpr auto operator()(int lhs, int rhs) const noexcept { return lhs * rhs; }
};

/// A functor which always returns zero
struct always_zero_func {
  constexpr auto operator()(int, int) noexcept { return 0; }
  constexpr auto operator()(int, int) const noexcept { return 0; }
};

template <typename T>
struct AllFunctionRefsTest : public testing::Test {
  add_func const add{};
  multiply_func const multiply{};
  always_zero_func const always_zero{};

  int const the_answer = 42;
};

TYPED_TEST_SUITE(AllFunctionRefsTest, all_function_refs, );

/// @test `function_ref` can take a simple lambda and returns the expected result from its call operator.
TYPED_TEST(AllFunctionRefsTest, CanPassSimpleLambda) {
  auto const returns_the_answer = [this] { return this->the_answer; };
  function_ref<int()> const func(returns_the_answer);

  ASSERT_EQ(func(), this->the_answer);
}

/// @test `function_ref` can take a simple lambda with capture and returns the expected result from its call operator.
TYPED_TEST(AllFunctionRefsTest, CanPassLambdaWithCapture) {
  int value = 0;
  auto const returns_value = [&value]() noexcept { return value; };
  typename TypeParam::template function_ref<int()> const func(returns_value);
  value = this->the_answer + 1;

  ASSERT_EQ(func(), value);
}

/// @test `function_ref` can take a simple lambda with arguments and returns the expected result from its call operator
/// invoked with arguments.
TYPED_TEST(AllFunctionRefsTest, InvokesProvidedCallableWithArguments) {
  typename TypeParam::template function_ref<int(int, int)> const func{this->add};

  ASSERT_EQ(func(this->the_answer - 2, 2), this->the_answer);
}

/// @test `function_ref` can take a simple lambda with capture and arguments, and returns the expected result from its
/// call operator invoked with arguments.
TYPED_TEST(AllFunctionRefsTest, CanPassLambdaWithCaptureAndArguments) {
  int value = 0;
  auto const sum_with_value = [&value](int lhs, int rhs) noexcept { return value + lhs + rhs; };
  typename TypeParam::template function_ref<int(int, int)> const func(sum_with_value);
  value = 10;

  ASSERT_EQ(func(10, -2), value + 10 - 2);
}

/// @test `function_ref` cannot take a function with a signature that is not compatible with the signature used as
/// `function_ref`'s template parameter. Functions with extra arguments, missing arguments or wrong return type cannot
/// be used.
TYPED_TEST(AllFunctionRefsTest, CannotPassCallableWithIncorrectArgumentsOrReturnType) {
  auto const extra_arg = [](int) noexcept -> int { return 0; };
  STATIC_ASSERT_FALSE(
      std::is_constructible<typename TypeParam::template function_ref<int()>, decltype(extra_arg)>::value
  );
  auto const missing_arg = []() noexcept -> int { return 0; };
  STATIC_ASSERT_FALSE(
      std::is_constructible<typename TypeParam::template function_ref<int(int)>, decltype(missing_arg)>::value
  );
  auto const wrong_return_type = []() noexcept -> int* { return nullptr; };
  STATIC_ASSERT_FALSE(
      std::is_constructible<typename TypeParam::template function_ref<int()>, decltype(wrong_return_type)>::value
  );
}

/// @test `function_ref` can be assigned a lambda.
TYPED_TEST(AllFunctionRefsTest, CanAssignLambda) {
  typename TypeParam::template function_ref<int(int, int)> func([](int, int) noexcept { return 0; });

  auto const lambda_add = [](int lhs, int rhs) noexcept { return lhs + rhs; };
  func = lambda_add;

  ASSERT_EQ(func(10, 22), lambda_add(10, 22));
}

/// @test `function_ref` can be copy-constructed. Modifying the original `function_ref` does not affect the copy.
TYPED_TEST(AllFunctionRefsTest, CanCopy) {
  typename TypeParam::template function_ref<int(int, int)> func(this->add);

  ASSERT_EQ(func(10, 22), this->add(10, 22));
  typename TypeParam::template function_ref<int(int, int)> const func2(func);
  func = [](int, int) noexcept { return 0; };

  ASSERT_EQ(func2(10, 22), this->add(10, 22));
}

/// @test `function_ref` can be copy-assigned. Modifying the original `function_ref` does not affect the copy.
TYPED_TEST(AllFunctionRefsTest, CanCopyAssign) {
  typename TypeParam::template function_ref<int(int, int)> func{this->add};
  typename TypeParam::template function_ref<int(int, int)> func2(this->multiply);

  func2 = func;
  func = this->multiply;

  ASSERT_EQ(func2(10, 22), this->add(10, 22));
}

/// @test `function_ref` can take a lambda with a reference as argument and with a reference as return value.
TYPED_TEST(AllFunctionRefsTest, PassByReference) {
  struct user_type {
    int value = 10;
  };
  auto const take_address = [](user_type& arg) noexcept -> user_type& { return arg; };

  typename TypeParam::template function_ref<user_type&(user_type&)> const func(take_address);

  user_type value;
  ASSERT_EQ(&func(value), &value);
}

/// @test `function_ref` parameterized with a signature having references to a derived class can take a lambda with a
/// signature having references to a base class.
TYPED_TEST(AllFunctionRefsTest, PassByCompatibleReference) {
  struct base {
    int value = 10;
  };
  auto take_address = [](base& value) noexcept -> base& { return value; };

  struct derived : base {};

  typename TypeParam::template function_ref<base&(derived&)> const func(take_address);

  derived value;
  ASSERT_EQ(&func(value), &value);
}

/// @test `function_ref` can take a lambda returning `void`. The lambda modifies a value captured by reference and the
/// final value is verified to ensure the call succeeds.
TYPED_TEST(AllFunctionRefsTest, ReturningVoid) {
  int value = 0;

  auto const increment_value_by = [&value](int delta) noexcept -> void { value += delta; };
  typename TypeParam::template function_ref<void(int)> const func(increment_value_by);

  int const delta = 10;
  func(delta);
  ASSERT_EQ(value, delta);
}

/// @test `function_ref` parameterized with a signature returning `void` can take a lambda with a compatible signature
/// returning a non-`void` value. The lambda modifies a value captured by reference and the final value is verified to
/// ensure the call succeeds.
TYPED_TEST(AllFunctionRefsTest, ReturningVoidFromNonVoid) {
  int value = 0;

  auto const increment_value_by_and_return_it = [&value](int delta) noexcept { return value += delta; };
  typename TypeParam::template function_ref<void(int)> const func(increment_value_by_and_return_it);

  int const offset = 10;
  func(offset);
  ASSERT_EQ(value, offset);
}

struct x_type {
  int value;
};
struct y_type {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  y_type(x_type val) noexcept
      : value(val.value) {}

  int value;
};

/// @test `function_ref`'s call operator can be invoked with an argument of a type that is convertible to the type in
/// its signature.
TYPED_TEST(AllFunctionRefsTest, ImplicitConversionInArgument) {
  auto const lambda = [](y_type value) noexcept { return value; };
  typename TypeParam::template function_ref<y_type(y_type)> const func(lambda);

  constexpr auto const_val1 = 42;
  ASSERT_EQ(func(x_type{const_val1}).value, const_val1);
}

/// @test `function_ref` can take a lambda returning a type convertible to the return type in `function_ref` signature.
TYPED_TEST(AllFunctionRefsTest, ImplicitConversionReturn) {
  auto const lambda = [](x_type value) noexcept { return value; };
  typename TypeParam::template function_ref<y_type(x_type)> const func(lambda);

  constexpr auto const_val1 = 99;
  ASSERT_EQ(func(x_type{const_val1}).value, const_val1);
}

/// @test `function_ref`'s call operator invokes the call operator of the proper constness when passed a callable that
/// supports `const` and non-`const` calls, depending on the constness of the callable and the constness of the
/// `function_ref`'s signature.
TYPED_TEST(AllFunctionRefsTest, ConstQualifiedCallable) {
  constexpr auto const_call_result = 123;
  constexpr auto non_const_call_result = -999;
  struct my_callable {
    auto operator()() const noexcept -> int { return const_call_result; }
    auto operator()() noexcept -> int { return non_const_call_result; }
  };

  my_callable callable;
  my_callable const const_callable;
  typename TypeParam::template function_ref<int()> const non_const_from_non_const = callable;
  typename TypeParam::template function_ref<int() const> const const_from_non_const = callable;
  typename TypeParam::template function_ref<int()> const non_const_from_const = const_callable;
  typename TypeParam::template function_ref<int() const> const const_from_const = const_callable;

  ASSERT_EQ(non_const_from_non_const(), non_const_call_result);
  ASSERT_EQ(const_from_non_const(), const_call_result);
  ASSERT_EQ(non_const_from_const(), const_call_result);
  ASSERT_EQ(const_from_const(), const_call_result);
}

/// @test `function_ref`'s call operator invokes the call operator of the proper signature when passed a callable that
/// implements overloads.
TYPED_TEST(AllFunctionRefsTest, OverloadedCallOperator) {
  constexpr std::int16_t short_arg = 42;
  constexpr std::int8_t char_arg = 123;

  struct overloaded {
    auto operator()(std::int64_t value) noexcept -> std::int64_t { return static_cast<std::int64_t>(value + 1); }
    auto operator()(std::int16_t value) noexcept -> std::int16_t { return static_cast<std::int16_t>(value - 1); }
  };

  overloaded callable;

  typename TypeParam::template function_ref<std::int64_t(std::int64_t)> const i64_overload = callable;
  typename TypeParam::template function_ref<std::int16_t(std::int16_t)> const i16_overload = callable;

  ASSERT_EQ(i64_overload(char_arg), char_arg + 1);
  ASSERT_EQ(i64_overload(short_arg), short_arg + 1);
  ASSERT_EQ(i16_overload(char_arg), char_arg - 1);
  ASSERT_EQ(i16_overload(short_arg), short_arg - 1);
}

/// @test `function_ref`'s call operator's noexcept specifier matches the `IsNoexcept` template parameter.
TYPED_TEST(AllFunctionRefsTest, NoexceptSpecifierOnCallOperatorMatchesIsNoexcept) {
  STATIC_ASSERT_EQ(
      noexcept(std::declval<typename TypeParam::template function_ref<void(void)>>()()),
      TypeParam::is_noexcept
  );
  STATIC_ASSERT_EQ(
      noexcept(std::declval<typename TypeParam::template function_ref<int(void)>>()()),
      TypeParam::is_noexcept
  );
  STATIC_ASSERT_EQ(
      noexcept(std::declval<typename TypeParam::template function_ref<void(int)>>()(int{})),
      TypeParam::is_noexcept
  );
  STATIC_ASSERT_EQ(
      noexcept(std::declval<typename TypeParam::template function_ref<int(int)>>()(int{})),
      TypeParam::is_noexcept
  );
}

/// @test A `function_ref` which is not `noexcept` can be assigned a function.
TEST(NoexceptFunctionRef, IsAnAliasForFunctionRefWithIsNoExceptSetTrue) {
  ::testing::StaticAssertTypeEq<noexcept_function_ref<void(void)>, function_ref<void(void), true>>();
  ::testing::StaticAssertTypeEq<noexcept_function_ref<int(void)>, function_ref<int(void), true>>();
  ::testing::StaticAssertTypeEq<noexcept_function_ref<void(int)>, function_ref<void(int), true>>();
  ::testing::StaticAssertTypeEq<noexcept_function_ref<int(int)>, function_ref<int(int), true>>();
  ::testing::StaticAssertTypeEq<noexcept_function_ref<void(void) const>, function_ref<void(void) const, true>>();
  ::testing::StaticAssertTypeEq<noexcept_function_ref<int(void) const>, function_ref<int(void) const, true>>();
  ::testing::StaticAssertTypeEq<noexcept_function_ref<void(int) const>, function_ref<void(int) const, true>>();
  ::testing::StaticAssertTypeEq<noexcept_function_ref<int(int) const>, function_ref<int(int) const, true>>();
}

void definitely_not_noexcept_free_function() noexcept(false) {}

/// @test A `function_ref` which is noexcept cannot be constructed from a callable which is not noexcept.
TEST(NoexceptFunctionRef, CannotBeConstructedFromFunctorWhichIsNotNoexcept) {
  auto const non_noexcept_lambda = []() noexcept(false) {};
  STATIC_ASSERT_FALSE(std::is_constructible<noexcept_function_ref<void(void)>, decltype(non_noexcept_lambda)>::value);
  STATIC_ASSERT_FALSE(
      std::is_constructible<noexcept_function_ref<void(void)>, decltype(definitely_not_noexcept_free_function)>::value
  );
  // we have to call this once so the function definition is needed, else you get unneeded internal declaration errors
  definitely_not_noexcept_free_function();
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables) testing interaction with global
int global = 0;

void increment_global(int value) { global += value; }

auto increment_global_and_return_it(int value) -> int { return global += value; }

/// @test `function_ref` parameterized with a signature returning `void` can take a function with the same signature.
/// The function modifies a global variable and the final value is verified to ensure the call succeeds.
TEST(NonNoexceptFunctionRef, WillInvokeAVoidReturningFunctionPointer) {
  global = 0;
  function_ref<void(int)> const func(increment_global);

  int const offset = 10;
  func(offset);
  ASSERT_EQ(global, offset);
}

/// @test `function_ref` parameterized with a signature returning `void` can take a function with a compatible signature
/// returning a non-`void` value. The function modifies a global variable and the final value is verified to ensure the
/// call succeeds.
TEST(NonNoexceptFunctionRef, AllowsConstructionFromNonVoidReturningFunctionPointerIfSignatureReturnsVoid) {
  global = 0;
  function_ref<void(int)> const func(increment_global_and_return_it);

  int const offset = 10;
  func(offset);
  ASSERT_EQ(global, offset);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables) testing interaction with global
bool global_called = false;

void global_no_args() { global_called = true; }

/// @test A `function_ref` which is not `noexcept` and which is parameterized with a signature with no arguments can
/// take a lambda or a function with the same signature. The lambda and the function modify a global variable and the
/// final value is verified to ensure the calls succeed.
TEST(NonNoexceptFunctionRef, AVoidReturningNullaryFunctionSignatureCanBeSatisfiedByBothFunctionPointersAndCallables) {
  bool called = false;
  auto const lambda = [&called] { called = true; };
  function_ref<void()> func(lambda);

  ASSERT_FALSE(called);
  func();
  ASSERT_TRUE(called);

  global_called = false;
  called = false;
  func = global_no_args;
  ASSERT_FALSE(global_called);
  func();
  ASSERT_FALSE(called);
  ASSERT_TRUE(global_called);
}

constexpr int the_answer = 42;

auto find_the_answer() -> int { return the_answer; }

auto multiply(int lhs, int rhs) -> int { return lhs * rhs; }

/// @test `function_ref` can take a simple function and returns the expected result from its call operator.
TEST(FunctionRef, CanPassSimpleFunction) {
  function_ref<int()> const func(find_the_answer);

  ASSERT_EQ(func(), the_answer);
}

/// @test A `function_ref` which is not `noexcept` can take a simple function with arguments and returns the expected
/// result from its call operator invoked with arguments.
TEST(NonNoexceptFunctionRef, CanPassSimpleFunctionWithArguments) {
  function_ref<int(int, int)> const func(multiply);

  ASSERT_EQ(func(6, 7), the_answer);
}

/// @test A `function_ref` which is not `noexcept` can be assigned a function.
TEST(NonNoexceptFunctionRef, CanAssignFunction) {
  auto const add = [](int lhs, int rhs) { return lhs + rhs; };
  function_ref<int(int, int)> func(add);
  func = &multiply;

  ASSERT_EQ(func(6, 7), multiply(6, 7));
}

}  // namespace
