// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/functional/bind_back.hpp"

#include <gtest/gtest.h>

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/optional/optional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/minus.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/plus.hpp"
#include "arene/base/stdlib_choice/reference_wrapper.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

using ::arene::base::bind_back;
using ::arene::base::is_invocable_r_v;
using ::arene::base::is_invocable_v;
using ::arene::base::is_nothrow_invocable_v;

namespace {

template <typename Ret, typename... Args>
auto free_func_n_argument(Args const&...) -> Ret {
  static Ret val{};
  return val;
}

struct BindBackTest : ::testing::Test {
  using rtype = int const&;
  using arg1t = int;
  using arg2t = int*;
  using arg3t = char const*;

  struct function_object_n_argument {
    int val;
    auto operator()(arg1t const&, arg2t const&, arg3t const&) const -> rtype { return val; }
  };

  static auto lambda_callable() {
    return [val = 0](arg1t const&, arg2t const&, arg3t const&) -> rtype { return val; };
  }

  struct type_with_member_function {
    int val;
    auto do_stuff(arg1t const&, arg2t const&, arg3t const&) const -> rtype { return val; }
  };
};

/// @test Given an invocable `F` with `N` arguments, when `bind_back` is called with `F` and `0<=M<=N` arguments, then
/// the returned invocable is a new invocable, `G`, whose size is equivalent to `sizeof(F) + (sizeof(M) + ...)`. If
/// either `F` is an empty class, or `M==0`, then they do not contribute to the size of the object.
TEST_F(BindBackTest, SizeIsSizeOfFunctionPlusBoundArgsWithEBO) {
  STATIC_ASSERT_EQ(sizeof(decltype(bind_back(std::plus<>{}))), 1U);
  STATIC_ASSERT_EQ(sizeof(decltype(bind_back(std::plus<>{}, int{}))), sizeof(int));
  STATIC_ASSERT_EQ(sizeof(decltype(bind_back(std::plus<>{}, int{}, int{}))), sizeof(int) + sizeof(int{}));

  struct non_empty_function_object {
    int val;
    auto operator()(int, int) -> int& { return val; }
  };
  STATIC_ASSERT_EQ(sizeof(decltype(bind_back(non_empty_function_object{int{}}))), sizeof(non_empty_function_object));
  STATIC_ASSERT_EQ(
      sizeof(decltype(bind_back(non_empty_function_object{int{}}, int{}))),
      sizeof(non_empty_function_object) + sizeof(int)
  );
  STATIC_ASSERT_EQ(
      sizeof(decltype(bind_back(non_empty_function_object{int{}}, int{}, int{}))),
      sizeof(non_empty_function_object) + sizeof(int) + sizeof(int{})
  );
}

/// @test Given an invocable `F` with `N` arguments, when `bind_back` is called with `F` and `0<=M<=N` arguments, the
/// returned invocable is a new invocable, `G`, which is invocable with with the first `N-M` arguments of `F`.
TEST_F(BindBackTest, ReturnsAnInvocableWhichIsTheInputInvocableWithTheFirstNArgumentsCurried) {
  STATIC_ASSERT_TRUE(is_invocable_v<
                     decltype(bind_back(free_func_n_argument<rtype, arg1t, arg2t, arg3t>)),
                     arg1t,
                     arg2t,
                     arg3t>);
  STATIC_ASSERT_TRUE(is_invocable_v<
                     decltype(bind_back(free_func_n_argument<rtype, arg1t, arg2t, arg3t>, arg3t{})),
                     arg1t,
                     arg2t>);
  STATIC_ASSERT_TRUE(is_invocable_v<
                     decltype(bind_back(free_func_n_argument<rtype, arg1t, arg2t, arg3t>, arg2t{}, arg3t{})),
                     arg1t>);
  STATIC_ASSERT_TRUE(is_invocable_v<
                     decltype(bind_back(free_func_n_argument<rtype, arg1t, arg2t, arg3t>, arg1t{}, arg2t{}, arg3t{}))>);

  STATIC_ASSERT_TRUE(is_invocable_v<decltype(bind_back(function_object_n_argument{})), arg1t, arg2t, arg3t>);
  STATIC_ASSERT_TRUE(is_invocable_v<decltype(bind_back(function_object_n_argument{}, arg3t{})), arg1t, arg2t>);
  STATIC_ASSERT_TRUE(is_invocable_v<decltype(bind_back(function_object_n_argument{}, arg2t{}, arg3t{})), arg1t>);
  STATIC_ASSERT_TRUE(is_invocable_v<decltype(bind_back(function_object_n_argument{}, arg1t{}, arg2t{}, arg3t{}))>);

  STATIC_ASSERT_TRUE(is_invocable_v<decltype(bind_back(lambda_callable())), arg1t, arg2t, arg3t>);
  STATIC_ASSERT_TRUE(is_invocable_v<decltype(bind_back(lambda_callable(), arg3t{})), arg1t, arg2t>);
  STATIC_ASSERT_TRUE(is_invocable_v<decltype(bind_back(lambda_callable(), arg2t{}, arg3t{})), arg1t>);
  STATIC_ASSERT_TRUE(is_invocable_v<decltype(bind_back(lambda_callable(), arg1t{}, arg2t{}, arg3t{}))>);

  STATIC_ASSERT_TRUE(is_invocable_v<
                     decltype(bind_back(&type_with_member_function::do_stuff)),
                     type_with_member_function*,
                     arg1t,
                     arg2t,
                     arg3t>);
  STATIC_ASSERT_TRUE(is_invocable_v<
                     decltype(bind_back(&type_with_member_function::do_stuff, arg3t{})),
                     type_with_member_function*,
                     arg1t,
                     arg2t>);
  STATIC_ASSERT_TRUE(is_invocable_v<
                     decltype(bind_back(&type_with_member_function::do_stuff, arg2t{}, arg3t{})),
                     type_with_member_function*,
                     arg1t>);
  STATIC_ASSERT_TRUE(is_invocable_v<decltype(bind_back(
                         &type_with_member_function::do_stuff,
                         std::declval<type_with_member_function*>(),
                         arg1t{},
                         arg2t{},
                         arg3t{}
                     ))>);

  STATIC_ASSERT_TRUE(is_invocable_v<
                     decltype(bind_back(&type_with_member_function::do_stuff)),
                     arene::base::optional<type_with_member_function>,
                     arg1t,
                     arg2t,
                     arg3t>);
  STATIC_ASSERT_TRUE(is_invocable_v<
                     decltype(bind_back(&type_with_member_function::do_stuff, arg3t{})),
                     arene::base::optional<type_with_member_function>,
                     arg1t,
                     arg2t>);
  STATIC_ASSERT_TRUE(is_invocable_v<
                     decltype(bind_back(&type_with_member_function::do_stuff, arg2t{}, arg3t{})),
                     arene::base::optional<type_with_member_function>,
                     arg1t>);
  STATIC_ASSERT_TRUE(is_invocable_v<decltype(bind_back(
                         &type_with_member_function::do_stuff,
                         arene::base::optional<type_with_member_function>(),
                         arg1t{},
                         arg2t{},
                         arg3t{}
                     ))>);
}

/// @test Given an invocable `F` with `N` arguments, when `bind_back` is called with `F` and `0<=M` arguments and
/// either `M>N` or the type of the `Mth` argument does not match the `N[M]`th argument in `F`, then the returned
/// object `G`, is valid but has no call operator which participates in overload resolution.
TEST_F(BindBackTest, BoundArgumentMismatchIsNotAnErrorButDoesNotHaveCallOverloads) {
  /// Extra argument
  auto const extra_arg_ff = bind_back(free_func_n_argument<rtype, arg1t, arg2t, arg3t>, arg1t{}, arg2t{}, arg3t{}, 0);
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(extra_arg_ff)>);

  auto const extra_arg_fo = bind_back(function_object_n_argument{}, arg1t{}, arg2t{}, arg3t{}, 0);
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(extra_arg_fo)>);

  auto const extra_arg_lb = bind_back(lambda_callable(), arg1t{}, arg2t{}, arg3t{}, 0U);
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(extra_arg_lb)>);

  auto const extra_arg_mf =
      bind_back(&type_with_member_function::do_stuff, type_with_member_function{}, arg1t{}, arg2t{}, arg3t{}, 0U);
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(extra_arg_mf)>);

  /// Wrong argument
  auto const wrong_arg_ff = bind_back(free_func_n_argument<rtype, arg1t, arg2t, arg3t>, arg1t{}, arg3t{}, arg2t{});
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(wrong_arg_ff)>);

  auto const wrong_arg_fo = bind_back(function_object_n_argument{}, arg1t{}, arg3t{}, arg2t{});
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(wrong_arg_fo)>);

  auto const wrong_arg_lb = bind_back(lambda_callable(), arg1t{}, arg3t{}, arg2t{});
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(wrong_arg_lb)>);

  auto const wrong_arg_mf =
      bind_back(&type_with_member_function::do_stuff, type_with_member_function{}, arg1t{}, arg3t{}, arg2t{});
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(wrong_arg_mf)>);
}

/// @test Given an invocable `F` with `N` arguments, when `bind_back` is called with `F` and `0<=M<=N` arguments, then
/// the returned invocable is a new invocable, `G`, whose return type exactly matches `F` including cref qualification.
TEST_F(BindBackTest, TheRefQualificationOfTheReturnFromTheInputCallableIsRetained) {
  STATIC_ASSERT_TRUE(is_invocable_r_v<
                     rtype,
                     decltype(bind_back(free_func_n_argument<rtype, arg1t, arg2t, arg3t>)),
                     arg1t,
                     arg2t,
                     arg3t>);
  STATIC_ASSERT_TRUE(is_invocable_r_v<
                     rtype,
                     decltype(bind_back(free_func_n_argument<rtype, arg1t, arg2t, arg3t>, arg3t{})),
                     arg1t,
                     arg2t>);
  STATIC_ASSERT_TRUE(is_invocable_r_v<
                     rtype,
                     decltype(bind_back(free_func_n_argument<rtype, arg1t, arg2t, arg3t>, arg2t{}, arg3t{})),
                     arg1t>);
  STATIC_ASSERT_TRUE(is_invocable_r_v<
                     rtype,
                     decltype(bind_back(free_func_n_argument<rtype, arg1t, arg2t, arg3t>, arg1t{}, arg2t{}, arg3t{}))>);

  STATIC_ASSERT_TRUE(is_invocable_r_v<rtype, decltype(bind_back(function_object_n_argument{})), arg1t, arg2t, arg3t>);
  STATIC_ASSERT_TRUE(is_invocable_r_v<rtype, decltype(bind_back(function_object_n_argument{}, arg3t{})), arg1t, arg2t>);
  STATIC_ASSERT_TRUE(is_invocable_r_v<
                     rtype,
                     decltype(bind_back(function_object_n_argument{}, arg2t{}, arg3t{})),
                     arg1t>);
  STATIC_ASSERT_TRUE(is_invocable_r_v<
                     rtype,
                     decltype(bind_back(function_object_n_argument{}, arg1t{}, arg2t{}, arg3t{}))>);

  STATIC_ASSERT_TRUE(is_invocable_r_v<rtype, decltype(bind_back(lambda_callable())), arg1t, arg2t, arg3t>);
  STATIC_ASSERT_TRUE(is_invocable_r_v<rtype, decltype(bind_back(lambda_callable(), arg3t{})), arg1t, arg2t>);
  STATIC_ASSERT_TRUE(is_invocable_r_v<rtype, decltype(bind_back(lambda_callable(), arg2t{}, arg3t{})), arg1t>);
  STATIC_ASSERT_TRUE(is_invocable_r_v<rtype, decltype(bind_back(lambda_callable(), arg1t{}, arg2t{}, arg3t{}))>);

  STATIC_ASSERT_TRUE(is_invocable_r_v<
                     rtype,
                     decltype(bind_back(&type_with_member_function::do_stuff)),
                     type_with_member_function*,
                     arg1t,
                     arg2t,
                     arg3t>);
  STATIC_ASSERT_TRUE(is_invocable_r_v<
                     rtype,
                     decltype(bind_back(&type_with_member_function::do_stuff, arg3t{})),
                     type_with_member_function*,
                     arg1t,
                     arg2t>);
  STATIC_ASSERT_TRUE(is_invocable_r_v<
                     rtype,
                     decltype(bind_back(&type_with_member_function::do_stuff, arg2t{}, arg3t{})),
                     type_with_member_function*,
                     arg1t>);
  STATIC_ASSERT_TRUE(is_invocable_r_v<
                     rtype,
                     decltype(bind_back(
                         &type_with_member_function::do_stuff,
                         std::declval<type_with_member_function*>(),
                         arg1t{},
                         arg2t{},
                         arg3t{}
                     ))>);

  STATIC_ASSERT_TRUE(is_invocable_r_v<
                     rtype,
                     decltype(bind_back(&type_with_member_function::do_stuff)),
                     arene::base::optional<type_with_member_function>,
                     arg1t,
                     arg2t,
                     arg3t>);
  STATIC_ASSERT_TRUE(is_invocable_r_v<
                     rtype,
                     decltype(bind_back(&type_with_member_function::do_stuff, arg3t{})),
                     arene::base::optional<type_with_member_function>,
                     arg1t,
                     arg2t>);
  STATIC_ASSERT_TRUE(is_invocable_r_v<
                     rtype,
                     decltype(bind_back(&type_with_member_function::do_stuff, arg2t{}, arg3t{})),
                     arene::base::optional<type_with_member_function>,
                     arg1t>);
  STATIC_ASSERT_TRUE(is_invocable_r_v<
                     rtype,
                     decltype(bind_back(
                         &type_with_member_function::do_stuff,
                         arene::base::optional<type_with_member_function>{},
                         arg1t{},
                         arg2t{},
                         arg3t{}
                     ))>);
}

/// @test Given an invocable `F` with `N` arguments, when `bind_back` is called with `F` and `0<=M<=N` arguments, then
/// the returned invocable is a new invocable, `G`, whose `noexcept` specifier is equivalent to that of `F` for a given
/// ref qualification and combination of arguments.
TEST_F(BindBackTest, TheNoexceptnessOfTheInputCallableIsPreserved) {
  struct callable_with_arg_and_ref_based_noexcept {
    auto operator()(int, float*) & noexcept(false) -> int { return {}; }
    auto operator()(int, float*) const& noexcept(true) -> int { return {}; }
    auto operator()(int, float*) && noexcept(false) -> int { return {}; }
    auto operator()(int, float*) const&& noexcept(true) -> int { return {}; }

    auto operator()(float*, int) & noexcept(true) -> float* { return {}; }
    auto operator()(float*, int) const& noexcept(false) -> float* { return {}; }
    auto operator()(float*, int) && noexcept(true) -> float* { return {}; }
    auto operator()(float*, int) const&& noexcept(false) -> float* { return {}; }
  };

  // No arguments curried
  using no_args_curried_int = decltype(bind_back(std::declval<callable_with_arg_and_ref_based_noexcept>()));
  STATIC_ASSERT_EQ(
      (is_nothrow_invocable_v<no_args_curried_int&, int, int>),
      (is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&, int, int>)
  );
  STATIC_ASSERT_EQ(
      (is_nothrow_invocable_v<no_args_curried_int const&, int, int>),
      (is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept const&, int, int>)
  );
  STATIC_ASSERT_EQ(
      (is_nothrow_invocable_v<no_args_curried_int&&, int, int>),
      (is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&&, int, int>)
  );
  STATIC_ASSERT_EQ(
      (is_nothrow_invocable_v<no_args_curried_int const&&, int, int>),
      (is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept const&&, int, int>)
  );
  using no_args_curried_str = decltype(bind_back(std::declval<callable_with_arg_and_ref_based_noexcept>()));
  STATIC_ASSERT_EQ(
      (is_nothrow_invocable_v<no_args_curried_str&, float*, float*>),
      (is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&, float*, float*>)
  );
  STATIC_ASSERT_EQ(
      (is_nothrow_invocable_v<no_args_curried_str const&, float*, float*>),
      (is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept const&, float*, float*>)
  );
  STATIC_ASSERT_EQ(
      (is_nothrow_invocable_v<no_args_curried_str&&, float*, float*>),
      (is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&&, float*, float*>)
  );
  STATIC_ASSERT_EQ(
      (is_nothrow_invocable_v<no_args_curried_str const&&, float*, float*>),
      (is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept const&&, float*, float*>)
  );

  // Some arguments curried
  using some_args_curried_int = decltype(bind_back(std::declval<callable_with_arg_and_ref_based_noexcept>(), int{}));
  STATIC_ASSERT_EQ(
      (is_nothrow_invocable_v<some_args_curried_int&, int>),
      (is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&, int, int>)
  );
  STATIC_ASSERT_EQ(
      (is_nothrow_invocable_v<some_args_curried_int const&, int>),
      (is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept const&, int, int>)
  );
  STATIC_ASSERT_EQ(
      (is_nothrow_invocable_v<some_args_curried_int&&, int>),
      (is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&&, int, int>)
  );
  using some_args_curried_floatptr =
      decltype(bind_back(std::declval<callable_with_arg_and_ref_based_noexcept>(), std::declval<float*>()));
  STATIC_ASSERT_EQ(
      (is_nothrow_invocable_v<some_args_curried_floatptr&, float*>),
      (is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&, float*, float*>)
  );
  STATIC_ASSERT_EQ(
      (is_nothrow_invocable_v<some_args_curried_floatptr const&, float*>),
      (is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept const&, float*, float*>)
  );
  STATIC_ASSERT_EQ(
      (is_nothrow_invocable_v<some_args_curried_floatptr&&, float*>),
      (is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&&, float*, float*>)
  );

  // All arguments curried
  using all_args_curried_int =
      decltype(bind_back(std::declval<callable_with_arg_and_ref_based_noexcept>(), int{}, int{}));
  STATIC_ASSERT_EQ(
      (is_nothrow_invocable_v<all_args_curried_int&>),
      (is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&, int, int>)
  );
  STATIC_ASSERT_EQ(
      (is_nothrow_invocable_v<all_args_curried_int const&>),
      (is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept const&, int, int>)
  );
  STATIC_ASSERT_EQ(
      (is_nothrow_invocable_v<all_args_curried_int&&>),
      (is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&&, int, int>)
  );
  using all_args_curried_floatptr = decltype(bind_back(
      std::declval<callable_with_arg_and_ref_based_noexcept>(),
      std::declval<float*>(),
      std::declval<float*>()
  ));
  STATIC_ASSERT_EQ(
      (is_nothrow_invocable_v<all_args_curried_floatptr&>),
      (is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&, float*, float*>)
  );
  STATIC_ASSERT_EQ(
      (is_nothrow_invocable_v<all_args_curried_floatptr const&>),
      (is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept const&, float*, float*>)
  );
  STATIC_ASSERT_EQ(
      (is_nothrow_invocable_v<all_args_curried_floatptr&&>),
      (is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&&, float*, float*>)
  );
}

struct ctor_counted {
  struct ctor_counts {
    std::size_t move_ctor{};
    std::size_t copy_ctor{};
  };

  explicit ctor_counted() noexcept = default;

  ctor_counted(ctor_counted const& other) noexcept
      : counts(other.counts) {
    ++counts.copy_ctor;
  }
  ctor_counted(ctor_counted&& other) noexcept
      : counts(other.counts) {
    ++counts.move_ctor;
  }
  auto operator=(ctor_counted const& other) noexcept -> ctor_counted& = delete;
  auto operator=(ctor_counted&& other) noexcept -> ctor_counted& = delete;

  ~ctor_counted() = default;

  ctor_counts counts;
};

/// @test Given an invocable `F` with `N` arguments, when `bind_back` is called with `F` and `0<=M<=N` arguments, then
/// the bound arguments are perfect-forwarded into the returned wrapper type.
TEST_F(BindBackTest, BoundArgumentsArePerfectForwardedIntoTheWrapper) {
  auto const func = [](ctor_counted const& val) -> ctor_counted::ctor_counts const& { return val.counts; };

  {
    SCOPED_TRACE("lvalue bound argument");
    auto bound_arg = ctor_counted{};
    auto const get_ctor_counts = bind_back(func, bound_arg);
    EXPECT_EQ(get_ctor_counts().copy_ctor, 1);
    EXPECT_EQ(get_ctor_counts().move_ctor, 0);
  }
  {
    SCOPED_TRACE("rvalue bound argument");
    auto const get_ctor_counts = bind_back(func, ctor_counted{});
    EXPECT_EQ(get_ctor_counts().copy_ctor, 0);
    EXPECT_EQ(get_ctor_counts().move_ctor, 1);
  }
}

/// @test Given an invocable, `G`, which is the result of `bind_back(F, bind_args...)`, then the expression
/// `G(call_args...)` is expression-equivalent to `F(bind_args..., call_args..)`
TEST_F(BindBackTest, InvokingTheReturnedCallableIsEquivalentToInvokingTheInputCallableWithTheBoundArguments) {
  auto const add_three_args = [](int first, int second, int third) { return first + second + third; };
  auto const add_to_10_20 = bind_back(add_three_args, 10, 20);
  for (auto const value : {0, 1, 2, 3, 4, 5, 6, -1, -2, -3, -4, -5}) {
    EXPECT_EQ(add_to_10_20(value), add_three_args(10, 20, value));
  }
}

/// @test Given an invocable, `G`, which is the result of `bind_back(F, bind_args...)`, when `G(call_args...)` is
/// invoked, then the cref qualification of `G` is propagated to `bind_args`.
TEST_F(BindBackTest, TheReferenceQualificationOfTheReturnedCallableIsAppliedToTheBoundArguments) {
  struct int_wrapper {
    int value{};

    auto operator+(int_wrapper other) const -> int_wrapper { return int_wrapper{value + other.value}; }
    auto operator+=(int_wrapper other) -> int_wrapper& { return *this = (*this + other); }

    auto operator==(int_wrapper other) const -> bool { return value == other.value; }
  };

  auto const bound_last_arg_initial_state = int_wrapper{1};

  auto const last_arg_must_be_rvalue = [](int_wrapper const& lhs, int_wrapper&& rhs) -> int_wrapper {
    // NOLINTNEXTLINE(hicpp-move-const-arg)
    return lhs + std::move(rhs);
  };
  auto only_invocable_if_moved = bind_back(last_arg_must_be_rvalue, bound_last_arg_initial_state);

  STATIC_ASSERT_FALSE(is_invocable_v<decltype(only_invocable_if_moved)&, int_wrapper>);
  STATIC_ASSERT_FALSE(is_invocable_v<const decltype(only_invocable_if_moved)&, int_wrapper>);
  STATIC_ASSERT_TRUE(is_invocable_v<decltype(only_invocable_if_moved)&&, int_wrapper>);

  // NOLINTNEXTLINE(hicpp-move-const-arg)
  EXPECT_EQ(std::move(only_invocable_if_moved)(int_wrapper{2}), int_wrapper{2} + bound_last_arg_initial_state);

  auto const last_arg_must_be_non_const_lvalue = [](int_wrapper const& lhs, int_wrapper& rhs) -> int_wrapper {
    rhs += lhs;
    return rhs;
  };
  auto only_invocable_if_mutable = bind_back(last_arg_must_be_non_const_lvalue, bound_last_arg_initial_state);

  STATIC_ASSERT_TRUE(is_invocable_v<decltype(only_invocable_if_mutable)&, int_wrapper>);
  STATIC_ASSERT_FALSE(is_invocable_v<const decltype(only_invocable_if_mutable)&, int_wrapper>);
  STATIC_ASSERT_FALSE(is_invocable_v<decltype(only_invocable_if_mutable)&&, int_wrapper>);

  auto const first_invoke_arg = int_wrapper{2};
  auto const second_invoke_arg = int_wrapper{2};

  EXPECT_EQ(only_invocable_if_mutable(first_invoke_arg), bound_last_arg_initial_state + first_invoke_arg);
  EXPECT_EQ(
      only_invocable_if_mutable(second_invoke_arg),
      bound_last_arg_initial_state + first_invoke_arg + second_invoke_arg
  );
}

/// @test Given an invocable, `G`, which is the result of `bind_back(F, bind_args...)`, when `G(call_args...)` is
/// invoked, then the cref qualification of `G` is propagated to `F`.
TEST_F(BindBackTest, TheReferenceQualificationOfTheReturnedCallableIsAppliedToTheInvokedFunction) {
  struct refqualified_dependent_call_operator {
    auto operator()(int base, int offset) & { return base + offset; }
    auto operator()(int base, int offset) const& { return base * 2 + offset; }
    auto operator()(int base, int offset) && { return base * 3 + offset; }
    auto operator()(int base, int offset) const&& { return base * 4 + offset; }
  };
  auto const base = 10;
  auto const offset = 1;
  auto lvalue_bound_base = bind_back(refqualified_dependent_call_operator{}, offset);
  auto lvalue_original = refqualified_dependent_call_operator{};
  EXPECT_EQ(lvalue_bound_base(base), lvalue_original(base, offset));

  auto const clvalue_bound_base = bind_back(refqualified_dependent_call_operator{}, offset);
  auto const clvalue_original = refqualified_dependent_call_operator{};
  EXPECT_EQ(clvalue_bound_base(base), clvalue_original(base, offset));

  auto rvalue_bound_base = bind_back(refqualified_dependent_call_operator{}, offset);
  auto rvalue_original = refqualified_dependent_call_operator{};
  // NOLINTNEXTLINE(hicpp-move-const-arg) we explicitly want to test rvalue qualification behavior
  EXPECT_EQ(std::move(rvalue_bound_base)(base), std::move(rvalue_original)(base, offset));

  auto const crvalue_bound_base = bind_back(refqualified_dependent_call_operator{}, offset);
  auto const crvalue_original = refqualified_dependent_call_operator{};
  // NOLINTNEXTLINE(hicpp-move-const-arg) we explicitly want to test rvalue qualification behavior
  EXPECT_EQ(std::move(crvalue_bound_base)(base), std::move(crvalue_original)(base, offset));
}

/// @test Given an invocable, `G`, which is the result of `bind_back(F, bind_args...)`, and given that `F` has multiple
/// overloads of `operator()` which have overlapping prefixes of arguments, then invoking
/// `G(call_args...)` is expression-equivalent to `F(bind_args..., call_args..)` for the overload selected based on
/// `call_args..`.
TEST_F(BindBackTest, InvokingACallableWithMultipleOverloadsWithOverlappingPrefixesSelectsOverloadBasedOnCallArgs) {
  struct overloaded_call_operator {
    auto operator()(int arg1) & -> int { return arg1; }
    auto operator()(int arg1, int arg2) & -> int { return arg1 + arg2; }
    auto operator()(int arg1, int arg2, int arg3) & -> int { return arg1 + arg2 + arg3; }

    auto operator()(int arg1) const& -> int { return arg1; }
    auto operator()(int arg1, int arg2) const& -> int { return arg1 * arg2; }
    auto operator()(int arg1, int arg2, int arg3) const& -> int { return arg1 * arg2 * arg3; }

    auto operator()(int arg1) && -> int { return arg1; }
    auto operator()(int arg1, int arg2) && -> int { return arg1 - arg2; }
    auto operator()(int arg1, int arg2, int arg3) && -> int { return arg1 - arg2 - arg3; }

    auto operator()(int arg1) const&& -> int { return arg1; }
    auto operator()(int arg1, int arg2) const&& -> int { return arg1 / arg2; }
    auto operator()(int arg1, int arg2, int arg3) const&& -> int { return arg1 / arg2 / arg3; }
  };

  auto const ten = 10;
  auto const hundred = 100;
  auto const thousand = 1000;
  auto lvalue_bound_base = bind_back(overloaded_call_operator{}, ten);
  auto lvalue_original = overloaded_call_operator{};
  EXPECT_EQ(lvalue_bound_base(), lvalue_original(ten));
  EXPECT_EQ(lvalue_bound_base(hundred), lvalue_original(hundred, ten));
  EXPECT_EQ(lvalue_bound_base(hundred, thousand), lvalue_original(hundred, thousand, ten));

  auto const clvalue_bound_base = bind_back(overloaded_call_operator{}, ten);
  auto const clvalue_original = overloaded_call_operator{};
  EXPECT_EQ(clvalue_bound_base(), clvalue_original(ten));
  EXPECT_EQ(clvalue_bound_base(hundred), clvalue_original(hundred, ten));
  EXPECT_EQ(clvalue_bound_base(hundred, thousand), clvalue_original(hundred, thousand, ten));

  // NOLINTBEGIN(hicpp-move-const-arg) we explicitly want to test rvalue qualification behavior
  {
    auto rvalue_bound_base = bind_back(overloaded_call_operator{}, ten);
    auto rvalue_original = overloaded_call_operator{};
    EXPECT_EQ(std::move(rvalue_bound_base)(), std::move(rvalue_original)(ten));
  }
  {
    auto rvalue_bound_base = bind_back(overloaded_call_operator{}, ten);
    auto rvalue_original = overloaded_call_operator{};
    EXPECT_EQ(std::move(rvalue_bound_base)(hundred), std::move(rvalue_original)(hundred, ten));
  }
  {
    auto rvalue_bound_base = bind_back(overloaded_call_operator{}, ten);
    auto rvalue_original = overloaded_call_operator{};
    EXPECT_EQ(std::move(rvalue_bound_base)(hundred, thousand), std::move(rvalue_original)(hundred, thousand, ten));
  }

  {
    auto const crvalue_bound_base = bind_back(overloaded_call_operator{}, ten);
    auto const crvalue_original = overloaded_call_operator{};
    EXPECT_EQ(std::move(crvalue_bound_base)(), std::move(crvalue_original)(ten));
  }
  {
    auto const crvalue_bound_base = bind_back(overloaded_call_operator{}, ten);
    auto const crvalue_original = overloaded_call_operator{};
    EXPECT_EQ(std::move(crvalue_bound_base)(hundred), std::move(crvalue_original)(hundred, ten));
  }
  {
    auto const crvalue_bound_base = bind_back(overloaded_call_operator{}, ten);
    auto const crvalue_original = overloaded_call_operator{};
    EXPECT_EQ(std::move(crvalue_bound_base)(hundred, thousand), std::move(crvalue_original)(hundred, thousand, ten));
  }

  // NOLINTEND(hicpp-move-const-arg)
}

/// @test Given an invocable, `G`, which is the result of `bind_back(F, bind_args...)`, if one of the elements in
/// `bind_args` is wrapped in `std::reference_wrapper`, then when `G` is invoked the reference semantics are maintained
/// for that element.
TEST_F(BindBackTest, WrappingBoundArgsInReferenceWrapperAllowsReferenceSemantics) {
  int bound_accumulator{0};
  auto const out_param_accumulate = [](int value, int& accumulator) {
    accumulator += value;
    return accumulator;
  };

  auto non_refwrapped_accumulate = bind_back(out_param_accumulate, bound_accumulator);
  int expected_accumulator_state{0};
  for (auto const& value : {1, 2, 3, 4, 5, -6, -1, -10}) {
    expected_accumulator_state += value;
    auto const returned_accumulator_state = non_refwrapped_accumulate(value);
    // Since this wasn't refwrapped, the state of the internal accumulator in the call wrapper changes, but the state of
    // the original bound accumulator doesn't
    EXPECT_NE(returned_accumulator_state, bound_accumulator);
    EXPECT_EQ(returned_accumulator_state, expected_accumulator_state);
  }

  bound_accumulator = 0;
  expected_accumulator_state = 0;
  auto refwrapped_accumulate = bind_back(out_param_accumulate, std::ref(bound_accumulator));
  for (auto const& value : {1, 2, 3, 4, 5, -6, -1, -10}) {
    expected_accumulator_state += value;
    auto const returned_accumulator_state = refwrapped_accumulate(value);
    // Since this was refwrapped, the bound accumulator changes
    EXPECT_EQ(returned_accumulator_state, bound_accumulator);
    EXPECT_EQ(returned_accumulator_state, expected_accumulator_state);
  }
}

/// @test Given a `constexpr` functor `F`, it is possible to invoke `bind_back(F, bound_args...)` on it in a
/// `constexpr` context, and the resultant invocable `G` is also invocable in a `constexpr` context as
/// `G(call_args...)`.
TEST_F(BindBackTest, IsConstexprCompatible) {
  constexpr auto add_to_10 = bind_back(std::plus<>{}, 10);
  STATIC_ASSERT_EQ(add_to_10(20), 10 + 20);
}

enum deleted_qual_call_operator { lvalue, clvalue, rvalue, crvalue };

template <deleted_qual_call_operator DeleteThis>
struct deleted_call_operator {
  template <
      deleted_qual_call_operator MaybeDeleted = DeleteThis,
      ::arene::base::constraints<std::enable_if_t<!(MaybeDeleted == deleted_qual_call_operator::lvalue)>> = nullptr>
  auto operator()(int) & -> int {
    return {};
  }
  template <
      deleted_qual_call_operator MaybeDeleted = DeleteThis,
      ::arene::base::constraints<std::enable_if_t<MaybeDeleted == deleted_qual_call_operator::lvalue>> = nullptr>
  auto operator()(int) & -> int = delete;

  template <
      deleted_qual_call_operator MaybeDeleted = DeleteThis,
      ::arene::base::constraints<std::enable_if_t<!(MaybeDeleted == deleted_qual_call_operator::clvalue)>> = nullptr>
  auto operator()(int) const& -> int {
    return {};
  }
  template <
      deleted_qual_call_operator MaybeDeleted = DeleteThis,
      ::arene::base::constraints<std::enable_if_t<MaybeDeleted == deleted_qual_call_operator::clvalue>> = nullptr>
  auto operator()(int) const& -> int = delete;

  template <
      deleted_qual_call_operator MaybeDeleted = DeleteThis,
      ::arene::base::constraints<std::enable_if_t<!(MaybeDeleted == deleted_qual_call_operator::rvalue)>> = nullptr>
  auto operator()(int) && -> int {
    return {};
  }
  template <
      deleted_qual_call_operator MaybeDeleted = DeleteThis,
      ::arene::base::constraints<std::enable_if_t<MaybeDeleted == deleted_qual_call_operator::rvalue>> = nullptr>
  auto operator()(int) && -> int = delete;

  template <
      deleted_qual_call_operator MaybeDeleted = DeleteThis,
      ::arene::base::constraints<std::enable_if_t<!(MaybeDeleted == deleted_qual_call_operator::crvalue)>> = nullptr>
  auto operator()(int) const&& -> int {
    return {};
  }
  template <
      deleted_qual_call_operator MaybeDeleted = DeleteThis,
      ::arene::base::constraints<std::enable_if_t<MaybeDeleted == deleted_qual_call_operator::crvalue>> = nullptr>
  auto operator()(int) const&& -> int = delete;
};

template <typename Callable>
struct BindBackQualificationPropigationTest : ::testing::Test {};

using qualification_dependent_call_operators = ::testing::Types<
    deleted_call_operator<deleted_qual_call_operator::lvalue>,
    deleted_call_operator<deleted_qual_call_operator::clvalue>,
    deleted_call_operator<deleted_qual_call_operator::rvalue>,
    deleted_call_operator<deleted_qual_call_operator::crvalue>>;

TYPED_TEST_SUITE(BindBackQualificationPropigationTest, qualification_dependent_call_operators, );

/// @test Given an invocable, `G`, which is the result of `bind_back(F, bind_args...)`, if `operator()` is deleted for
/// a given cref qualification of `F` then it will be deleted in `G`.
TYPED_TEST(
    BindBackQualificationPropigationTest,
    WrapperIsNotInvocableWithRefQualificationIfInputInvocableHasDeletedOperatorForThatQualification
) {
  using bound_invocable_t = decltype(bind_back(TypeParam{}));
  STATIC_ASSERT_EQ((is_invocable_v<bound_invocable_t&, int>), (is_invocable_v<TypeParam&, int>));
  STATIC_ASSERT_EQ((is_invocable_v<bound_invocable_t const&, int>), (is_invocable_v<TypeParam const&, int>));
  STATIC_ASSERT_EQ((is_invocable_v<bound_invocable_t&&, int>), (is_invocable_v<TypeParam&&, int>));
  STATIC_ASSERT_EQ((is_invocable_v<bound_invocable_t const&&, int>), (is_invocable_v<TypeParam const&&, int>));
}

// NOLINTBEGIN(readability-identifier-length)

/// @test the result of back_bind can be used with bind_back
TEST_F(BindBackTest, PartiallyAppliedClosureIsBindable) {
  auto const f = bind_back(std::minus<>{}, 2);
  auto const flip = [](auto const& arg, auto const& func) { return func(arg); };
  auto const g = bind_back(flip, f);

  EXPECT_EQ(1, g(3));
}

/// @test the result of back_bind can be used with bind_back where the same argument type is bound
TEST_F(BindBackTest, PartiallyAppliedClosureIsBindableWithSameBoundArgType) {
  auto const f = bind_back(std::minus<>{}, 2);
  auto const g = bind_back(f, 3);

  EXPECT_EQ(1, g());
}

// NOLINTEND(readability-identifier-length)

/// @test bind_back is a function object
TEST_F(BindBackTest, IsFunctionObject) { std::ignore = bind_back; }

}  // namespace
