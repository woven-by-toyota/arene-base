// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/functional/bind_front.hpp"

#include <functional>
#include <initializer_list>
#include <memory>
#include <string>

#include <gtest/gtest.h>

#include "arene/base/constraints.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits.hpp"

using ::arene::base::bind_front;

namespace {

template <typename Ret, typename... Args>
auto free_func_n_argument(Args const&...) -> Ret {
  static Ret val{};
  return val;
}

struct BindFrontTest : ::testing::Test {
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

  struct member_function_pointer {
    int val;
    auto do_stuff(arg1t const&, arg2t const&, arg3t const&) const -> rtype { return val; }
  };
};

/// @test Given an invocable `F` with `N` arguments, when `bind_front` is called with `F` and `0<=M<=N` arguments, then
/// the returned invocable is a new invocable, `G`, whose size is equivalent to `sizeof(F) + (sizeof(M) + ...)`. If
/// either `F` is an empty class, or `M==0`, then they do not contribute to the size of the object.
TEST_F(BindFrontTest, SizeIsSizeOfFunctionPlusBoundArgsWithEBO) {
  STATIC_ASSERT_EQ(sizeof(decltype(bind_front(std::plus<>{}))), 1U);
  STATIC_ASSERT_EQ(sizeof(decltype(bind_front(std::plus<>{}, int{}))), sizeof(int));
  STATIC_ASSERT_EQ(sizeof(decltype(bind_front(std::plus<>{}, int{}, int{}))), sizeof(int) + sizeof(int{}));

  struct non_empty_function_object {
    int val;
    auto operator()(int, int) -> int& { return val; }
  };
  STATIC_ASSERT_EQ(sizeof(decltype(bind_front(non_empty_function_object{int{}}))), sizeof(non_empty_function_object));
  STATIC_ASSERT_EQ(
      sizeof(decltype(bind_front(non_empty_function_object{int{}}, int{}))),
      sizeof(non_empty_function_object) + sizeof(int)
  );
  STATIC_ASSERT_EQ(
      sizeof(decltype(bind_front(non_empty_function_object{int{}}, int{}, int{}))),
      sizeof(non_empty_function_object) + sizeof(int) + sizeof(int{})
  );
}

/// @test Given an invocable `F` with `N` arguments, when `bind_front` is called with `F` and `0<=M<=N` arguments, then
/// the returned invocable is a new invocable, `G`, which is invocable with with the last `N-M` arguments of `F`.
TEST_F(BindFrontTest, ReturnsAnInvocableWhichIsTheInputInvocableWithTheFirstNArgumentsCurried) {
  STATIC_ASSERT_TRUE(arene::base::is_invocable_v<
                     decltype(bind_front(free_func_n_argument<rtype, arg1t, arg2t, arg3t>)),
                     arg1t,
                     arg2t,
                     arg3t>);
  STATIC_ASSERT_TRUE(arene::base::is_invocable_v<
                     decltype(bind_front(free_func_n_argument<rtype, arg1t, arg2t, arg3t>, arg1t{})),
                     arg2t,
                     arg3t>);
  STATIC_ASSERT_TRUE(arene::base::is_invocable_v<
                     decltype(bind_front(free_func_n_argument<rtype, arg1t, arg2t, arg3t>, arg1t{}, arg2t{})),
                     arg3t>);
  STATIC_ASSERT_TRUE(arene::base::is_invocable_v<
                     decltype(bind_front(free_func_n_argument<rtype, arg1t, arg2t, arg3t>, arg1t{}, arg2t{}, arg3t{})
                     )>);

  STATIC_ASSERT_TRUE(arene::base::
                         is_invocable_v<decltype(bind_front(function_object_n_argument{})), arg1t, arg2t, arg3t>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_invocable_v<decltype(bind_front(function_object_n_argument{}, arg1t{})), arg2t, arg3t>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_invocable_v<decltype(bind_front(function_object_n_argument{}, arg1t{}, arg2t{})), arg3t>);
  STATIC_ASSERT_TRUE(arene::base::is_invocable_v<
                     decltype(bind_front(function_object_n_argument{}, arg1t{}, arg2t{}, arg3t{}))>);

  STATIC_ASSERT_TRUE(arene::base::is_invocable_v<decltype(bind_front(lambda_callable())), arg1t, arg2t, arg3t>);
  STATIC_ASSERT_TRUE(arene::base::is_invocable_v<decltype(bind_front(lambda_callable(), arg1t{})), arg2t, arg3t>);
  STATIC_ASSERT_TRUE(arene::base::is_invocable_v<decltype(bind_front(lambda_callable(), arg1t{}, arg2t{})), arg3t>);
  STATIC_ASSERT_TRUE(arene::base::is_invocable_v<decltype(bind_front(lambda_callable(), arg1t{}, arg2t{}, arg3t{}))>);

  STATIC_ASSERT_TRUE(arene::base::is_invocable_v<
                     decltype(bind_front(&member_function_pointer::do_stuff)),
                     member_function_pointer*,
                     arg1t,
                     arg2t,
                     arg3t>);
  STATIC_ASSERT_TRUE(arene::base::is_invocable_v<
                     decltype(bind_front(
                         &member_function_pointer::do_stuff,
                         std::declval<member_function_pointer*>(),
                         arg1t{}
                     )),
                     arg2t,
                     arg3t>);
  STATIC_ASSERT_TRUE(arene::base::is_invocable_v<
                     decltype(bind_front(
                         &member_function_pointer::do_stuff,
                         std::declval<member_function_pointer*>(),
                         arg1t{},
                         arg2t{}
                     )),
                     arg3t>);
  STATIC_ASSERT_TRUE(arene::base::is_invocable_v<decltype(bind_front(
                         &member_function_pointer::do_stuff,
                         std::declval<member_function_pointer*>(),
                         arg1t{},
                         arg2t{},
                         arg3t{}
                     ))>);

  STATIC_ASSERT_TRUE(arene::base::is_invocable_v<
                     decltype(bind_front(&member_function_pointer::do_stuff)),
                     std::shared_ptr<member_function_pointer>,
                     arg1t,
                     arg2t,
                     arg3t>);
  STATIC_ASSERT_TRUE(arene::base::is_invocable_v<
                     decltype(bind_front(
                         &member_function_pointer::do_stuff,
                         std::shared_ptr<member_function_pointer>(),
                         arg1t{}
                     )),
                     arg2t,
                     arg3t>);
  STATIC_ASSERT_TRUE(arene::base::is_invocable_v<
                     decltype(bind_front(
                         &member_function_pointer::do_stuff,
                         std::shared_ptr<member_function_pointer>(),
                         arg1t{},
                         arg2t{}
                     )),
                     arg3t>);
  STATIC_ASSERT_TRUE(arene::base::is_invocable_v<decltype(bind_front(
                         &member_function_pointer::do_stuff,
                         std::shared_ptr<member_function_pointer>(),
                         arg1t{},
                         arg2t{},
                         arg3t{}
                     ))>);
}

/// @test Given an invocable `F` with `N` arguments, when `bind_front` is called with `F` and `0<=M` arguments and
/// either `M>N` or the type of the `Mth` argument does not match the `N[M]`th argument in `F`, then the returned
/// object `G`, is valid but has no call operator which participates in overload resolution.
TEST_F(BindFrontTest, BoundArgumentMismatchIsNotAnErrorButDoesNotHaveCallOverloads) {
  /// Extra argument
  auto const extra_arg_ff = bind_front(free_func_n_argument<rtype, arg1t, arg2t, arg3t>, arg1t{}, arg2t{}, arg3t{}, 0);
  STATIC_ASSERT_FALSE(arene::base::is_invocable_v<decltype(extra_arg_ff)>);

  auto const extra_arg_fo = bind_front(function_object_n_argument{}, arg1t{}, arg2t{}, arg3t{}, 0);
  STATIC_ASSERT_FALSE(arene::base::is_invocable_v<decltype(extra_arg_fo)>);

  auto const extra_arg_lb = bind_front(lambda_callable(), arg1t{}, arg2t{}, arg3t{}, 0U);
  STATIC_ASSERT_FALSE(arene::base::is_invocable_v<decltype(extra_arg_lb)>);

  auto const extra_arg_mf =
      bind_front(&member_function_pointer::do_stuff, member_function_pointer{}, arg1t{}, arg2t{}, arg3t{}, 0U);
  STATIC_ASSERT_FALSE(arene::base::is_invocable_v<decltype(extra_arg_mf)>);

  /// Wrong argument
  auto const wrong_arg_ff = bind_front(free_func_n_argument<rtype, arg1t, arg2t, arg3t>, arg1t{}, arg3t{}, arg2t{});
  STATIC_ASSERT_FALSE(arene::base::is_invocable_v<decltype(wrong_arg_ff)>);

  auto const wrong_arg_fo = bind_front(function_object_n_argument{}, arg1t{}, arg3t{}, arg2t{});
  STATIC_ASSERT_FALSE(arene::base::is_invocable_v<decltype(wrong_arg_fo)>);

  auto const wrong_arg_lb = bind_front(lambda_callable(), arg1t{}, arg3t{}, arg2t{});
  STATIC_ASSERT_FALSE(arene::base::is_invocable_v<decltype(wrong_arg_lb)>);

  auto const wrong_arg_mf =
      bind_front(&member_function_pointer::do_stuff, member_function_pointer{}, arg1t{}, arg3t{}, arg2t{});
  STATIC_ASSERT_FALSE(arene::base::is_invocable_v<decltype(wrong_arg_mf)>);
}

/// @test Given an invocable `F` with `N` arguments, when `bind_front` is called with `F` and `0<=M<=N` arguments, then
/// the returned invocable is a new invocable, `G`, whose return type exactly matches `F` including cref qualification.
TEST_F(BindFrontTest, TheRefQualificationOfTheReturnFromTheInputCallableIsRetained) {
  STATIC_ASSERT_TRUE(arene::base::is_invocable_r_v<
                     rtype,
                     decltype(bind_front(free_func_n_argument<rtype, arg1t, arg2t, arg3t>)),
                     arg1t,
                     arg2t,
                     arg3t>);
  STATIC_ASSERT_TRUE(arene::base::is_invocable_r_v<
                     rtype,
                     decltype(bind_front(free_func_n_argument<rtype, arg1t, arg2t, arg3t>, arg1t{})),
                     arg2t,
                     arg3t>);
  STATIC_ASSERT_TRUE(arene::base::is_invocable_r_v<
                     rtype,
                     decltype(bind_front(free_func_n_argument<rtype, arg1t, arg2t, arg3t>, arg1t{}, arg2t{})),
                     arg3t>);
  STATIC_ASSERT_TRUE(arene::base::is_invocable_r_v<
                     rtype,
                     decltype(bind_front(free_func_n_argument<rtype, arg1t, arg2t, arg3t>, arg1t{}, arg2t{}, arg3t{})
                     )>);

  STATIC_ASSERT_TRUE(arene::base::is_invocable_r_v<
                     rtype,
                     decltype(bind_front(function_object_n_argument{})),
                     arg1t,
                     arg2t,
                     arg3t>);
  STATIC_ASSERT_TRUE(arene::base::is_invocable_r_v<
                     rtype,
                     decltype(bind_front(function_object_n_argument{}, arg1t{})),
                     arg2t,
                     arg3t>);
  STATIC_ASSERT_TRUE(arene::base::is_invocable_r_v<
                     rtype,
                     decltype(bind_front(function_object_n_argument{}, arg1t{}, arg2t{})),
                     arg3t>);
  STATIC_ASSERT_TRUE(arene::base::is_invocable_r_v<
                     rtype,
                     decltype(bind_front(function_object_n_argument{}, arg1t{}, arg2t{}, arg3t{}))>);

  STATIC_ASSERT_TRUE(arene::base::is_invocable_r_v<
                     rtype,
                     decltype(bind_front(&member_function_pointer::do_stuff)),
                     member_function_pointer*,
                     arg1t,
                     arg2t,
                     arg3t>);
  STATIC_ASSERT_TRUE(arene::base::is_invocable_r_v<
                     rtype,
                     decltype(bind_front(&member_function_pointer::do_stuff, std::declval<member_function_pointer*>())),
                     arg1t,
                     arg2t,
                     arg3t>);
  STATIC_ASSERT_TRUE(arene::base::is_invocable_r_v<
                     rtype,
                     decltype(bind_front(
                         &member_function_pointer::do_stuff,
                         std::declval<member_function_pointer*>(),
                         arg1t{}
                     )),
                     arg2t,
                     arg3t>);
  STATIC_ASSERT_TRUE(arene::base::is_invocable_r_v<
                     rtype,
                     decltype(bind_front(
                         &member_function_pointer::do_stuff,
                         std::declval<member_function_pointer*>(),
                         arg1t{},
                         arg2t{},
                         arg3t{}
                     ))>);

  STATIC_ASSERT_TRUE(arene::base::
                         is_invocable_r_v<rtype, decltype(bind_front(lambda_callable())), arg1t, arg2t, arg3t>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_invocable_r_v<rtype, decltype(bind_front(lambda_callable(), arg1t{})), arg2t, arg3t>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_invocable_r_v<rtype, decltype(bind_front(lambda_callable(), arg1t{}, arg2t{})), arg3t>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_invocable_r_v<rtype, decltype(bind_front(lambda_callable(), arg1t{}, arg2t{}, arg3t{}))>);
}

/// @test Given an invocable `F` with `N` arguments, when `bind_front` is called with `F` and `0<=M<=N` arguments, then
/// the returned invocable is a new invocable, `G`, whose `noexcept` specifier is equivalent to that of `F` for a given
/// ref qualification and combination of arguments.
TEST_F(BindFrontTest, TheNoexceptnessOfTheInputCallableIsPreserved) {
  struct callable_with_arg_and_ref_based_noexcept {
    auto operator()(int, int) & noexcept(false) -> int { return {}; }
    auto operator()(int, int) const& noexcept(true) -> int { return {}; }
    auto operator()(int, int) && noexcept(false) -> int { return {}; }
    auto operator()(int, int) const&& noexcept(true) -> int { return {}; }

    auto operator()(float*, float*) & noexcept(true) -> float* { return {}; }
    auto operator()(float*, float*) const& noexcept(false) -> float* { return {}; }
    auto operator()(float*, float*) && noexcept(true) -> float* { return {}; }
    auto operator()(float*, float*) const&& noexcept(false) -> float* { return {}; }
  };

  // No arguments curried
  using no_args_curried_int = decltype(bind_front(std::declval<callable_with_arg_and_ref_based_noexcept>()));
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<no_args_curried_int&, int, int>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&, int, int>)
  );
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<no_args_curried_int const&, int, int>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept const&, int, int>)
  );
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<no_args_curried_int&&, int, int>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&&, int, int>)
  );
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<no_args_curried_int const&&, int, int>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept const&&, int, int>)
  );
  using no_args_curried_str = decltype(bind_front(std::declval<callable_with_arg_and_ref_based_noexcept>()));
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<no_args_curried_str&, float*, float*>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&, float*, float*>)
  );
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<no_args_curried_str const&, float*, float*>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept const&, float*, float*>)
  );
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<no_args_curried_str&&, float*, float*>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&&, float*, float*>)
  );
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<no_args_curried_str const&&, float*, float*>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept const&&, float*, float*>)
  );

  // Some arguments curried
  using some_args_curried_int = decltype(bind_front(std::declval<callable_with_arg_and_ref_based_noexcept>(), int{}));
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<some_args_curried_int&, int>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&, int, int>)
  );
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<some_args_curried_int const&, int>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept const&, int, int>)
  );
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<some_args_curried_int&&, int>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&&, int, int>)
  );
  using some_args_curried_str =
      decltype(bind_front(std::declval<callable_with_arg_and_ref_based_noexcept>(), std::string{}));
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<some_args_curried_str&, std::string>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&, std::string, std::string>)
  );
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<some_args_curried_str const&, std::string>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept const&, std::string, std::string>)
  );
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<some_args_curried_str&&, std::string>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&&, std::string, std::string>)
  );

  // All arguments curried
  using all_args_curried_int =
      decltype(bind_front(std::declval<callable_with_arg_and_ref_based_noexcept>(), int{}, int{}));
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<all_args_curried_int&>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&, int, int>)
  );
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<all_args_curried_int const&>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept const&, int, int>)
  );
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<all_args_curried_int&&>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&&, int, int>)
  );
  using all_args_curried_str =
      decltype(bind_front(std::declval<callable_with_arg_and_ref_based_noexcept>(), std::string{}, std::string{}));
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<all_args_curried_str&>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&, std::string, std::string>)
  );
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<all_args_curried_str const&>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept const&, std::string, std::string>)
  );
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<all_args_curried_str&&>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&&, std::string, std::string>)
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

/// @test Given an invocable `F` with `N` arguments, when `bind_front` is called with `F` and `0<=M<=N` arguments, then
/// the bound arguments are perfect-forwarded into the returned wrapper type.
TEST_F(BindFrontTest, BoundArgumentsArePerfectForwardedIntoTheWrapper) {
  auto const func = [](ctor_counted const& val) -> ctor_counted::ctor_counts const& { return val.counts; };

  {
    SCOPED_TRACE("lvalue bound argument");
    auto bound_arg = ctor_counted{};
    auto const get_ctor_counts = bind_front(func, bound_arg);
    EXPECT_EQ(get_ctor_counts().copy_ctor, 1);
    EXPECT_EQ(get_ctor_counts().move_ctor, 0);
  }
  {
    SCOPED_TRACE("rvalue bound argument");
    auto const get_ctor_counts = bind_front(func, ctor_counted{});
    EXPECT_EQ(get_ctor_counts().copy_ctor, 0);
    EXPECT_EQ(get_ctor_counts().move_ctor, 1);
  }
}

/// @test Given an invocable, `G`, which is the result of `bind_front(F, bind_args...)`, then the expression
/// `G(call_args...)` is expression-equivalent to `F(bind_args..., call_args..)`
TEST_F(BindFrontTest, InvokingTheReturnedCallableIsEquivalentToInvokingTheInputCallableWithTheBoundArguments) {
  auto const add_three_args = [](int first, int second, int third) { return first + second + third; };
  auto const add_to_10_20 = bind_front(add_three_args, 10, 20);
  for (auto const value : {0, 1, 2, 3, 4, 5, 6, -1, -2, -3, -4, -5}) {
    EXPECT_EQ(add_to_10_20(value), add_three_args(10, 20, value));
  }
}

/// @test Given an invocable, `G`, which is the result of `bind_front(F, bind_args...)`, when `G(call_args...)` is
/// invoked, then the cref qualification of `G` is propagated to `bind_args`.
TEST_F(BindFrontTest, TheReferenceQualificationOfTheReturnedCallableIsAppliedToTheBoundArguments) {
  std::string const bound_first_arg_initial_state = "foo";

  auto const first_arg_must_be_rvalue = [](std::string&& lhs, std::string const& rhs) -> std::string {
    return std::move(lhs) + rhs;
  };
  auto only_invocable_if_moved = bind_front(first_arg_must_be_rvalue, bound_first_arg_initial_state);
  STATIC_ASSERT_FALSE(::arene::base::is_invocable_v<decltype(only_invocable_if_moved)&, std::string>);
  STATIC_ASSERT_FALSE(::arene::base::is_invocable_v<const decltype(only_invocable_if_moved)&, std::string>);
  STATIC_ASSERT_TRUE(::arene::base::is_invocable_v<decltype(only_invocable_if_moved)&&, std::string>);
  EXPECT_EQ(std::move(only_invocable_if_moved)("bar"), bound_first_arg_initial_state + std::string{"bar"});

  auto const first_arg_must_be_non_const_lvalue = [](std::string& lhs, std::string const& rhs) -> std::string {
    lhs += rhs;
    return lhs;
  };
  auto only_invocable_if_mutable = bind_front(first_arg_must_be_non_const_lvalue, bound_first_arg_initial_state);
  STATIC_ASSERT_TRUE(::arene::base::is_invocable_v<decltype(only_invocable_if_mutable)&, std::string>);
  STATIC_ASSERT_FALSE(::arene::base::is_invocable_v<const decltype(only_invocable_if_mutable)&, std::string>);
  STATIC_ASSERT_FALSE(::arene::base::is_invocable_v<decltype(only_invocable_if_mutable)&&, std::string>);
  std::string const first_invoke_arg{"bar"};
  std::string const second_invoke_arg{"bar"};
  EXPECT_EQ(only_invocable_if_mutable(first_invoke_arg), bound_first_arg_initial_state + first_invoke_arg);
  EXPECT_EQ(
      only_invocable_if_mutable(second_invoke_arg),
      bound_first_arg_initial_state + first_invoke_arg + second_invoke_arg
  );
}

/// @test Given an invocable, `G`, which is the result of `bind_front(F, bind_args...)`, when `G(call_args...)` is
/// invoked, then the cref qualification of `G` is propagated to `F`.
TEST_F(BindFrontTest, TheReferenceQualificationOfTheReturnedCallableIsAppliedToTheInvokedFunction) {
  struct refqualified_dependent_call_operator {
    auto operator()(int base, int offset) & { return base + offset; }
    auto operator()(int base, int offset) const& { return base * 2 + offset; }
    auto operator()(int base, int offset) && { return base * 3 + offset; }
    auto operator()(int base, int offset) const&& { return base * 4 + offset; }
  };
  auto const base = 10;
  auto const offset = 1;
  auto lvalue_bound_base = bind_front(refqualified_dependent_call_operator{}, base);
  auto lvalue_original = refqualified_dependent_call_operator{};
  EXPECT_EQ(lvalue_bound_base(offset), lvalue_original(base, offset));

  auto const clvalue_bound_base = bind_front(refqualified_dependent_call_operator{}, base);
  auto const clvalue_original = refqualified_dependent_call_operator{};
  EXPECT_EQ(clvalue_bound_base(offset), clvalue_original(base, offset));

  auto rvalue_bound_base = bind_front(refqualified_dependent_call_operator{}, base);
  auto rvalue_original = refqualified_dependent_call_operator{};
  // NOLINTNEXTLINE(hicpp-move-const-arg) we explicitly want to test rvalue qualification behavior
  EXPECT_EQ(std::move(rvalue_bound_base)(offset), std::move(rvalue_original)(base, offset));

  auto const crvalue_bound_base = bind_front(refqualified_dependent_call_operator{}, base);
  auto const crvalue_original = refqualified_dependent_call_operator{};
  // NOLINTNEXTLINE(hicpp-move-const-arg) we explicitly want to test rvalue qualification behavior
  EXPECT_EQ(std::move(crvalue_bound_base)(offset), std::move(crvalue_original)(base, offset));
}

/// @test Given an invocable, `G`, which is the result of `bind_front(F, bind_args...)`, and given that `F` has multiple
/// overloads of `operator()` which have overlapping prefixes of arguments, then invoking
/// `G(call_args...)` is expression-equivalent to `F(bind_args..., call_args..)` for the overload selected based on
/// `call_args..`.
TEST_F(BindFrontTest, InvokingACallableWithMultipleOverloadsWithOverlappingPrefixesSelectsOverloadBasedOnCallArgs) {
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

  auto const arg1 = 10;
  auto const arg2 = 100;
  auto const arg3 = 1000;
  auto lvalue_bound_base = bind_front(overloaded_call_operator{}, arg1);
  auto lvalue_original = overloaded_call_operator{};
  EXPECT_EQ(lvalue_bound_base(), lvalue_original(arg1));
  EXPECT_EQ(lvalue_bound_base(arg2), lvalue_original(arg1, arg2));
  EXPECT_EQ(lvalue_bound_base(arg2, arg3), lvalue_original(arg1, arg2, arg3));

  auto const clvalue_bound_base = bind_front(overloaded_call_operator{}, arg1);
  auto const clvalue_original = overloaded_call_operator{};
  EXPECT_EQ(clvalue_bound_base(), clvalue_original(arg1));
  EXPECT_EQ(clvalue_bound_base(arg2), clvalue_original(arg1, arg2));
  EXPECT_EQ(clvalue_bound_base(arg2, arg3), clvalue_original(arg1, arg2, arg3));

  // NOLINTBEGIN(hicpp-move-const-arg) we explicitly want to test rvalue qualification behavior
  {
    auto rvalue_bound_base = bind_front(overloaded_call_operator{}, arg1);
    auto rvalue_original = overloaded_call_operator{};
    EXPECT_EQ(std::move(rvalue_bound_base)(), std::move(rvalue_original)(arg1));
  }
  {
    auto rvalue_bound_base = bind_front(overloaded_call_operator{}, arg1);
    auto rvalue_original = overloaded_call_operator{};
    EXPECT_EQ(std::move(rvalue_bound_base)(arg2), std::move(rvalue_original)(arg1, arg2));
  }
  {
    auto rvalue_bound_base = bind_front(overloaded_call_operator{}, arg1);
    auto rvalue_original = overloaded_call_operator{};
    EXPECT_EQ(std::move(rvalue_bound_base)(arg2, arg3), std::move(rvalue_original)(arg1, arg2, arg3));
  }

  {
    auto const crvalue_bound_base = bind_front(overloaded_call_operator{}, arg1);
    auto const crvalue_original = overloaded_call_operator{};
    EXPECT_EQ(std::move(crvalue_bound_base)(), std::move(crvalue_original)(arg1));
  }
  {
    auto const crvalue_bound_base = bind_front(overloaded_call_operator{}, arg1);
    auto const crvalue_original = overloaded_call_operator{};
    EXPECT_EQ(std::move(crvalue_bound_base)(arg2), std::move(crvalue_original)(arg1, arg2));
  }
  {
    auto const crvalue_bound_base = bind_front(overloaded_call_operator{}, arg1);
    auto const crvalue_original = overloaded_call_operator{};
    EXPECT_EQ(std::move(crvalue_bound_base)(arg2, arg3), std::move(crvalue_original)(arg1, arg2, arg3));
  }

  // NOLINTEND(hicpp-move-const-arg)
}

/// @test Given an invocable, `G`, which is the result of `bind_front(F, bind_args...)`, if one of the elements in
/// `bind_args` is wrapped in `std::reference_wrapper`, then when `G` is invoked the reference semantics are maintained
/// for that element.
TEST_F(BindFrontTest, WrappingBoundArgsInReferenceWrapperAllowsReferenceSemantics) {
  int bound_accumulator{0};
  auto const out_param_accumulate = [](int& accumulator, int value) {
    accumulator += value;
    return accumulator;
  };

  auto non_refwrapped_accumulate = bind_front(out_param_accumulate, bound_accumulator);
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
  auto refwrapped_accumulate = bind_front(out_param_accumulate, std::ref(bound_accumulator));
  for (auto const& value : {1, 2, 3, 4, 5, -6, -1, -10}) {
    expected_accumulator_state += value;
    auto const returned_accumulator_state = refwrapped_accumulate(value);
    // Since this was refwrapped, the bound accumulator changes
    EXPECT_EQ(returned_accumulator_state, bound_accumulator);
    EXPECT_EQ(returned_accumulator_state, expected_accumulator_state);
  }
}

/// @test Given a `constexpr` functor `F`, it is possible to invoke `bind_front(F, bound_args...)` on it in a
/// `constexpr` context, and the resultant invocable `G` is also invocable in a `constexpr` context as
/// `G(call_args...)`.
TEST_F(BindFrontTest, IsConstexprCompatible) {
  constexpr auto add_to_10 = bind_front(std::plus<>{}, 10);
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
struct BindFrontQualificationPropigationTest : ::testing::Test {};

using qualification_dependent_call_operators = ::testing::Types<
    deleted_call_operator<deleted_qual_call_operator::lvalue>,
    deleted_call_operator<deleted_qual_call_operator::clvalue>,
    deleted_call_operator<deleted_qual_call_operator::rvalue>,
    deleted_call_operator<deleted_qual_call_operator::crvalue>>;

TYPED_TEST_SUITE(BindFrontQualificationPropigationTest, qualification_dependent_call_operators, );

/// @test Given an invocable, `G`, which is the result of `bind_front(F, bind_args...)`, if `operator()` is deleted for
/// a given cref qualification of `F` then it will be deleted in `G`.
TYPED_TEST(
    BindFrontQualificationPropigationTest,
    WrapperIsNotInvocableWithRefQualificationIfInputInvocableHasDeletedOperatorForThatQualification
) {
  using bound_invocable_t = decltype(bind_front(TypeParam{}));
  STATIC_ASSERT_EQ(
      (::arene::base::is_invocable_v<bound_invocable_t&, int>),
      (::arene::base::is_invocable_v<TypeParam&, int>)
  );
  STATIC_ASSERT_EQ(
      (::arene::base::is_invocable_v<bound_invocable_t const&, int>),
      (::arene::base::is_invocable_v<TypeParam const&, int>)
  );
  STATIC_ASSERT_EQ(
      (::arene::base::is_invocable_v<bound_invocable_t&&, int>),
      (::arene::base::is_invocable_v<TypeParam&&, int>)
  );
  STATIC_ASSERT_EQ(
      (::arene::base::is_invocable_v<bound_invocable_t const&&, int>),
      (::arene::base::is_invocable_v<TypeParam const&&, int>)
  );
}

}  // namespace
