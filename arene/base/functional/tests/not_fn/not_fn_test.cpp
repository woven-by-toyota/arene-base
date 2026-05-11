// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/functional/not_fn.hpp"

#include <tuple>

#include <gtest/gtest.h>

#include "arene/base/constraints.hpp"
#include "arene/base/functional/bind_front.hpp"
#include "arene/base/optional.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_empty.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/tuple.hpp"
#include "arene/base/type_traits.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace {

using ::arene::base::not_fn;

using int_like = testing::configurable_value<int>;
using double_like = testing::configurable_value<double>;

class convertible_to_bool_wrapper {
  bool value_;

 public:
  constexpr explicit convertible_to_bool_wrapper(bool value)
      : value_(value) {}
  // NOLINTNEXTLINE(hicpp-explicit-conversions) Explicitly testing behavior of implicit conversions
  constexpr operator bool() const noexcept { return value_; }
};

template <typename Ret>
constexpr auto true_if_all_positive(int_like const& lhs, double_like const& rhs) -> Ret {
  return Ret{lhs.value > 0 && rhs.value > 0};
}

template <typename BoolLikeType>
struct NotFnTest : ::testing::Test {
  using rtype = BoolLikeType;
  using arg1t = int_like;
  using arg2t = double_like;

  struct function_object_true_if_all_positive {
    constexpr auto operator()(arg1t lhs, arg2t rhs) const -> rtype { return true_if_all_positive<rtype>(lhs, rhs); }
  };

  static auto lambda_true_if_all_positive() {
    return [](arg1t lhs, arg2t rhs) -> rtype { return true_if_all_positive<rtype>(lhs, rhs); };
  }

  struct member_function_pointer {
    constexpr auto do_stuff(arg1t lhs, arg2t rhs) const -> rtype { return true_if_all_positive<rtype>(lhs, rhs); }
  };
};

using bool_like_types = ::testing::Types<bool, convertible_to_bool_wrapper>;

TYPED_TEST_SUITE(NotFnTest, bool_like_types, );

/// @test Given an invocable `F` with `N` arguments, when `not_fn` is called with `F`, then
/// the returned invocable is a new invocable, `G`, whose size is equivalent to `sizeof(F)`. If
/// `F` is an empty class, then it does not contribute to the size of the object.
TYPED_TEST(NotFnTest, SizeIsSizeOfFunctionWithEBO) {
  STATIC_ASSERT_TRUE(std::is_empty<decltype(not_fn(std::plus<>{}))>::value);
  STATIC_ASSERT_EQ(
      sizeof(decltype(not_fn(true_if_all_positive<TypeParam>))),
      sizeof(decltype(&true_if_all_positive<TypeParam>))
  );

  struct non_empty_function_object {
    int val;
    auto operator()(int, int) -> bool { return val > 0; }
  };
  STATIC_ASSERT_EQ(sizeof(decltype(not_fn(non_empty_function_object{int{}}))), sizeof(non_empty_function_object));
}

/// @test Given an invocable `F` with `N` arguments, when `not_fn` is called with `F`, then the returned invocable is a
/// new invocable, `G`, which is invocable with with the `N` arguments of `F`.
TYPED_TEST(NotFnTest, ReturnsAnInvocableWhichIsInvocableAsIfItWasTheInputInvocable) {
  STATIC_ASSERT_TRUE(arene::base::is_invocable_v<
                     decltype(not_fn(true_if_all_positive<TypeParam>)),
                     typename TestFixture::arg1t,
                     typename TestFixture::arg2t>);

  STATIC_ASSERT_TRUE(arene::base::is_invocable_v<
                     decltype(not_fn(this->lambda_true_if_all_positive())),
                     typename TestFixture::arg1t,
                     typename TestFixture::arg2t>);

  STATIC_ASSERT_TRUE(arene::base::is_invocable_v<
                     decltype(not_fn(typename TestFixture::function_object_true_if_all_positive{})),
                     typename TestFixture::arg1t,
                     typename TestFixture::arg2t>);

  STATIC_ASSERT_TRUE(arene::base::is_invocable_v<
                     decltype(not_fn(&TestFixture::member_function_pointer::do_stuff)),
                     typename TestFixture::member_function_pointer*,
                     typename TestFixture::arg1t,
                     typename TestFixture::arg2t>);

  STATIC_ASSERT_TRUE(arene::base::is_invocable_v<
                     decltype(not_fn(&TestFixture::member_function_pointer::do_stuff)),
                     std::shared_ptr<typename TestFixture::member_function_pointer>,
                     typename TestFixture::arg1t,
                     typename TestFixture::arg2t>);
}

/// @test Given an invocable `F` with `N` arguments, when `not_fn` is called with `F`, then
/// the returned invocable is a new invocable, `G`, whose return type is always `bool`.
TYPED_TEST(NotFnTest, TheReturnFromTheInputCallableIsImplicitlyCastToBool) {
  STATIC_ASSERT_TRUE(arene::base::is_invocable_r_v<
                     bool,
                     decltype(not_fn(true_if_all_positive<typename TestFixture::rtype>)),
                     typename TestFixture::arg1t,
                     typename TestFixture::arg2t>);

  STATIC_ASSERT_TRUE(arene::base::is_invocable_r_v<
                     bool,
                     decltype(not_fn(this->lambda_true_if_all_positive())),
                     typename TestFixture::arg1t,
                     typename TestFixture::arg2t>);

  STATIC_ASSERT_TRUE(arene::base::is_invocable_r_v<
                     bool,
                     decltype(not_fn(typename TestFixture::function_object_true_if_all_positive{})),
                     typename TestFixture::arg1t,
                     typename TestFixture::arg2t>);

  STATIC_ASSERT_TRUE(arene::base::is_invocable_r_v<
                     bool,
                     decltype(not_fn(&TestFixture::member_function_pointer::do_stuff)),
                     typename TestFixture::member_function_pointer*,
                     typename TestFixture::arg1t,
                     typename TestFixture::arg2t>);
}

/// @test Given an invocable `F` with `N` arguments, when `not_fn` is called with `F`, then the returned invocable is a
/// new invocable, `G`, whose `noexcept` specifier is equivalent to that of `F` for a given ref qualification and
/// combination of arguments.
TYPED_TEST(NotFnTest, TheNoexceptnessOfTheInputCallableIsPreserved) {
  struct callable_with_arg_and_ref_based_noexcept {
    auto operator()(int_like const&) & noexcept(false) -> TypeParam { return TypeParam{true}; }
    auto operator()(int_like const&) const& noexcept(true) -> TypeParam { return TypeParam{true}; }
    auto operator()(int_like const&) && noexcept(false) -> TypeParam { return TypeParam{true}; }
    auto operator()(int_like const&) const&& noexcept(true) -> TypeParam { return TypeParam{true}; }

    auto operator()(double_like const&) & noexcept(true) -> TypeParam { return TypeParam{true}; }
    auto operator()(double_like const&) const& noexcept(false) -> TypeParam { return TypeParam{true}; }
    auto operator()(double_like const&) && noexcept(true) -> TypeParam { return TypeParam{true}; }
    auto operator()(double_like const&) const&& noexcept(false) -> TypeParam { return TypeParam{true}; }
  };

  using not_fn_return_t = decltype(not_fn(callable_with_arg_and_ref_based_noexcept{}));
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<not_fn_return_t&, int_like const&>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&, int_like const&>)
  );
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<not_fn_return_t const&, int_like const&>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept const&, int_like const&>)
  );
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<not_fn_return_t&&, int_like const&>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&&, int_like const&>)
  );
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<not_fn_return_t const&&, int_like const&>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept const&&, int_like const&>)
  );

  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<not_fn_return_t&, double_like const&>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&, double_like const&>)
  );
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<not_fn_return_t const&, double_like const&>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept const&, double_like const&>)
  );
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<not_fn_return_t&&, double_like const&>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept&&, double_like const&>)
  );
  STATIC_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<not_fn_return_t const&&, double_like const&>),
      (arene::base::is_nothrow_invocable_v<callable_with_arg_and_ref_based_noexcept const&&, double_like const&>)
  );
}

/// @test Given an invocable, `G`, which is the result of `not_fn(F)`, then the expression
/// `G(call_args...)` is expression-equivalent to `!F(call_args...)`
TYPED_TEST(NotFnTest, InvokingTheReturnedCallableIsEquivalentToInvokingTheInputCallableAndNegatingTheResult) {
  for (auto const& args :
       {std::make_pair(typename TestFixture::arg1t{1}, typename TestFixture::arg2t{1}),
        std::make_pair(typename TestFixture::arg1t{-1}, typename TestFixture::arg2t{1})}) {
    EXPECT_EQ(
        not_fn(true_if_all_positive<typename TestFixture::rtype>)(args.first, args.second),
        !true_if_all_positive<typename TestFixture::rtype>(args.first, args.second)
    );
    EXPECT_EQ(
        not_fn(typename TestFixture::function_object_true_if_all_positive{})(args.first, args.second),
        !typename TestFixture::function_object_true_if_all_positive{}(args.first, args.second)
    );
    EXPECT_EQ(
        not_fn(this->lambda_true_if_all_positive())(args.first, args.second),
        !this->lambda_true_if_all_positive()(args.first, args.second)
    );
    auto const member_function_callable = typename TestFixture::member_function_pointer{};
    EXPECT_EQ(
        not_fn(&TestFixture::member_function_pointer::do_stuff)(&member_function_callable, args.first, args.second),
        !member_function_callable.do_stuff(args.first, args.second)
    );
  }
}

/// @test Given a `constexpr` functor `F`, it is possible to invoke `not_fn(F)` on it in a
/// `constexpr` context, and the resultant invocable `G` is also invocable in a `constexpr` context as
/// `G(call_args...)`.
TYPED_TEST(NotFnTest, IsConstexprCompatible) {
  constexpr auto true_if_either_negative = not_fn(true_if_all_positive<typename TestFixture::rtype>);
  STATIC_ASSERT_TRUE(true_if_either_negative(typename TestFixture::arg1t{-1}, typename TestFixture::arg2t{1}));
  STATIC_ASSERT_FALSE(true_if_either_negative(typename TestFixture::arg1t{1}, typename TestFixture::arg2t{1}));
}

enum ref_qualification { lvalue, clvalue, rvalue, crvalue, none };

template <ref_qualification DeleteThisCallOperator = ref_qualification::none>
struct refqualified_dependent_call_operator {
  template <
      ref_qualification MaybeDeleted = DeleteThisCallOperator,
      ::arene::base::constraints<std::enable_if_t<!(MaybeDeleted == ref_qualification::lvalue)>> = nullptr>
  constexpr auto operator()(ref_qualification expected) & -> bool {
    return expected != ref_qualification::lvalue;
  }
  template <
      ref_qualification MaybeDeleted = DeleteThisCallOperator,
      ::arene::base::constraints<std::enable_if_t<MaybeDeleted == ref_qualification::lvalue>> = nullptr>
  constexpr auto operator()(ref_qualification expected) & -> bool = delete;

  template <
      ref_qualification MaybeDeleted = DeleteThisCallOperator,
      ::arene::base::constraints<std::enable_if_t<!(MaybeDeleted == ref_qualification::clvalue)>> = nullptr>
  constexpr auto operator()(ref_qualification expected) const& -> bool {
    return expected != ref_qualification::clvalue;
  }
  template <
      ref_qualification MaybeDeleted = DeleteThisCallOperator,
      ::arene::base::constraints<std::enable_if_t<MaybeDeleted == ref_qualification::clvalue>> = nullptr>
  constexpr auto operator()(ref_qualification expected) const& -> bool = delete;

  template <
      ref_qualification MaybeDeleted = DeleteThisCallOperator,
      ::arene::base::constraints<std::enable_if_t<!(MaybeDeleted == ref_qualification::rvalue)>> = nullptr>
  constexpr auto operator()(ref_qualification expected) && -> bool {
    return expected != ref_qualification::rvalue;
  }
  template <
      ref_qualification MaybeDeleted = DeleteThisCallOperator,
      ::arene::base::constraints<std::enable_if_t<MaybeDeleted == ref_qualification::rvalue>> = nullptr>
  constexpr auto operator()(ref_qualification expected) && -> bool = delete;

  template <
      ref_qualification MaybeDeleted = DeleteThisCallOperator,
      ::arene::base::constraints<std::enable_if_t<!(MaybeDeleted == ref_qualification::crvalue)>> = nullptr>
  constexpr auto operator()(ref_qualification expected) const&& -> bool {
    return expected != ref_qualification::crvalue;
  }
  template <
      ref_qualification MaybeDeleted = DeleteThisCallOperator,
      ::arene::base::constraints<std::enable_if_t<MaybeDeleted == ref_qualification::crvalue>> = nullptr>
  constexpr auto operator()(ref_qualification expected) const&& -> bool = delete;
};

/// @test Given an invocable, `G`, which is the result of `not_fn(F)`, when `G(call_args...)` is
/// invoked, then the cref qualification of `G` is propagated to `F`.
TYPED_TEST(NotFnTest, TheReferenceQualificationOfTheReturnedCallableIsAppliedToTheInvokedFunction) {
  auto lvalue_bound_base = not_fn(refqualified_dependent_call_operator<>{});
  EXPECT_TRUE(lvalue_bound_base(ref_qualification::lvalue));

  auto const clvalue_bound_base = not_fn(refqualified_dependent_call_operator<>{});
  EXPECT_TRUE(clvalue_bound_base(ref_qualification::clvalue));

  auto rvalue_bound_base = not_fn(refqualified_dependent_call_operator<>{});
  // NOLINTNEXTLINE(hicpp-move-const-arg) we explicitly want to test rvalue qualification behavior
  EXPECT_TRUE(std::move(rvalue_bound_base)(ref_qualification::rvalue));

  auto const crvalue_bound_base = not_fn(refqualified_dependent_call_operator<>{});
  // NOLINTNEXTLINE(hicpp-move-const-arg) we explicitly want to test rvalue qualification behavior
  EXPECT_TRUE(std::move(crvalue_bound_base)(ref_qualification::crvalue));
}

template <typename Callable>
struct NotFnQualificationPropagationTest : ::testing::Test {};

using qualification_dependent_call_operators = ::testing::Types<
    refqualified_dependent_call_operator<ref_qualification::lvalue>,
    refqualified_dependent_call_operator<ref_qualification::clvalue>,
    refqualified_dependent_call_operator<ref_qualification::rvalue>,
    refqualified_dependent_call_operator<ref_qualification::crvalue>>;

TYPED_TEST_SUITE(NotFnQualificationPropagationTest, qualification_dependent_call_operators, );

/// @test Given an invocable, `G`, which is the result of `not_fn(F)`, if `operator()` is deleted for
/// a given cref qualification of `F` then it will be deleted in `G`.
TYPED_TEST(
    NotFnQualificationPropagationTest,
    WrapperIsNotInvocableWithRefQualificationIfInputInvocableHasDeletedOperatorForThatQualification
) {
  using bound_invocable_t = decltype(not_fn(TypeParam{}));
  STATIC_ASSERT_EQ(
      (::arene::base::is_invocable_v<bound_invocable_t&, ref_qualification>),
      (::arene::base::is_invocable_v<TypeParam&, ref_qualification>)
  );
  STATIC_ASSERT_EQ(
      (::arene::base::is_invocable_v<bound_invocable_t const&, ref_qualification>),
      (::arene::base::is_invocable_v<TypeParam const&, ref_qualification>)
  );
  STATIC_ASSERT_EQ(
      (::arene::base::is_invocable_v<bound_invocable_t&&, ref_qualification>),
      (::arene::base::is_invocable_v<TypeParam&&, ref_qualification>)
  );
  STATIC_ASSERT_EQ(
      (::arene::base::is_invocable_v<bound_invocable_t const&&, ref_qualification>),
      (::arene::base::is_invocable_v<TypeParam const&&, ref_qualification>)
  );
}

template <typename T>
class NotFnVariableArgsTest;
template <typename... ArgTs>
class NotFnVariableArgsTest<std::tuple<ArgTs...>> : public ::testing::Test {
 public:
  using call_args_t = std::tuple<ArgTs...>;
  using was_called_with_t = std::tuple<arene::base::optional<ArgTs>...>;
  was_called_with_t was_last_called_with_args;

  template <std::size_t... Is>
  auto make_call_args(std::index_sequence<Is...>) const -> call_args_t {
    return call_args_t{ArgTs{Is}...};
  }

  auto call_fn(ArgTs const&... args) -> bool {
    was_last_called_with_args = std::make_tuple(arene::base::optional<ArgTs>{args}...);
    return false;
  }
};

using call_arg_sets = ::testing::Types<
    std::tuple<>,
    std::tuple<int>,
    std::tuple<int, int>,
    std::tuple<int, int, int>,
    std::tuple<int, int, int, int>>;

TYPED_TEST_SUITE(NotFnVariableArgsTest, call_arg_sets, );

/// @test Given an invocable, `G`, which is the result of `not_fn(F)`, then the expression
/// `G(call_args...)` invokes `F` with `call_args...`.
TYPED_TEST(
    NotFnVariableArgsTest,
    InvokingTheReturnedCallableIsEquivalentToInvokingTheInputCallableWithTheInputArguments
) {
  auto const inverse_fn = not_fn(arene::base::bind_front(&TestFixture::call_fn, this));
  auto const call_args =
      this->make_call_args(std::make_index_sequence<std::tuple_size<typename TestFixture::call_args_t>::value>{});
  EXPECT_TRUE(arene::base::apply(inverse_fn, call_args));
  EXPECT_EQ(this->was_last_called_with_args, call_args);
}

}  // namespace
