// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/is_invocable.hpp"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <utility>

#include <gtest/gtest.h>

#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/functional.hpp"
#include "arene/base/type_list.hpp"

namespace {

using arene::base::is_invocable;
using arene::base::is_invocable_r;
using arene::base::is_invocable_r_v;
using arene::base::is_invocable_v;
using arene::base::is_nothrow_invocable;
using arene::base::is_nothrow_invocable_r;
using arene::base::is_nothrow_invocable_r_v;
using arene::base::is_nothrow_invocable_v;

using arene::base::invoke_result;
using arene::base::invoke_result_t;

using arene::base::type_list;
using arene::base::type_lists::at_t;
using arene::base::type_lists::concat_t;
using arene::base::type_lists::size_v;

struct test_struct1;
struct test_struct2;

// Parameter and return types used in test callables
using p1_1 = std::int32_t;
using p1_1_impconv = std::int16_t;  // a type implicitly convertible to p1_1
using p2_1 = test_struct1;
using p2_2 = std::int32_t&&;
using p2_2_impconv = std::int16_t&&;
using pr1_1 = std::int64_t;
using pr1_1_impconv = std::int8_t;  // a type implicitly convertible to pr1_1
using pr2_1 = test_struct1;
using pr2_2 = std::int64_t&&;
using pr2_2_impconv = std::int8_t&&;
using r0 = std::int64_t;
using r0_impconv = std::int32_t;  // a type to which r0 is implicitly convertible
using r0_notimpconv = test_struct2;
using r1 = test_struct2&;
using r1_notimpconv = std::uint8_t;
using r2 = double const&;
using r2_impconv = std::int32_t;
using r2_notimpconv = test_struct1&;

// Test structures
struct test_struct1 {};

struct test_struct2 {};

struct test_struct3 : public test_struct1 {};

// Test callables
template <typename Ret, typename... Ts>
struct test_functor final {
  auto operator()(Ts...) -> Ret;
};

template <typename Ret, typename... Ts>
struct test_nothrow_functor final {
  auto operator()(Ts...) noexcept -> Ret;
};

// explicitly making global functors
// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables, fuchsia-statically-constructed-objects)
test_struct2 glob_ts2{};
double glob_d{51279.0};

ARENE_MAYBE_UNUSED auto lv0 = []() {};
ARENE_MAYBE_UNUSED auto lv1 = [](p1_1) {};
ARENE_MAYBE_UNUSED auto lv2 = [](p2_1, p2_2) {};
ARENE_MAYBE_UNUSED auto lr0 = []() -> r0 { return 512UL; };
ARENE_MAYBE_UNUSED auto lr1 = [](p1_1) -> r1 { return glob_ts2; };
ARENE_MAYBE_UNUSED auto lr2 = [](p2_1, p2_2) -> r2 { return glob_d; };

ARENE_MAYBE_UNUSED auto lv0_no = []() noexcept {};
ARENE_MAYBE_UNUSED auto lv1_no = [](p1_1) noexcept {};
ARENE_MAYBE_UNUSED auto lv2_no = [](p2_1, p2_2) noexcept {};
ARENE_MAYBE_UNUSED auto lr0_no = []() noexcept -> r0 { return 512UL; };
ARENE_MAYBE_UNUSED auto lr1_no = [](p1_1) noexcept -> r1 { return glob_ts2; };
ARENE_MAYBE_UNUSED auto lr2_no = [](p2_1, p2_2) noexcept -> r2 { return glob_d; };
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables, fuchsia-statically-constructed-objects)

template <typename First, typename Second>
using pr = std::pair<First, Second>;

using ftorv0_no = test_nothrow_functor<void>;
using ftorv1_no = test_nothrow_functor<void, p1_1>;
using ftorv2_no = test_nothrow_functor<void, p2_1, p2_2>;
using ftorr0_no = test_nothrow_functor<r0>;
using ftorr1_no = test_nothrow_functor<r1, pr1_1>;
using ftorr2_no = test_nothrow_functor<r2, pr2_1, pr2_2>;

// Testing utilities
template <std::size_t Line, bool ExpectNoexcept, typename Ret, typename Fn>
struct test_is_invocable final {
  template <bool, typename... Args>
  static constexpr auto test(type_list<Args...>) noexcept -> bool {
    return false;
  }
};

template <typename Trait, bool Value>
constexpr auto is_type_trait_with_value =
    (Trait::value == Value) && (std::is_convertible<Trait, std::integral_constant<bool, Value>>::value);

template <typename T, typename = arene::base::constraints<>>
struct has_type : std::false_type {};
template <typename T>
struct has_type<T, arene::base::constraints<typename T::type>> : std::true_type {};

/// @brief determine if @c invoke_result correctly defines @c type
/// @tparam Fn type to invoke
/// @tparam Args argument types to invoke with
///
/// @{

/// overload if the associated call to @c is_invocable_v<Fn, Args...> is @c false
///
template <typename Fn, typename... Args>
constexpr auto is_invoke_result_correct_impl(std::false_type) noexcept -> bool {
  return !has_type<invoke_result<Fn, Args...>>::value;
}

/// overload if the associated call to @c is_invocable_v<Fn, Args...> is @c true
///
template <typename Fn, typename... Args>
constexpr auto is_invoke_result_correct_impl(std::true_type) noexcept -> bool {
  return std::is_same<invoke_result_t<Fn, Args...>, typename invoke_result<Fn, Args...>::type>::value &&
         std::is_same<
             invoke_result_t<Fn, Args...>,
             decltype(arene::base::invoke(std::declval<Fn>(), std::declval<Args>()...))>::value;
}

template <typename Fn, typename... Args>
constexpr auto is_invoke_result_correct = is_invoke_result_correct_impl<Fn, Args...>(is_invocable<Fn, Args...>{});

/// @}

template <std::size_t Line, typename Fn>
struct test_is_invocable<Line, false, void, Fn> final {
  template <bool Expectation, typename... Args>
  static constexpr auto test(type_list<Args...>) noexcept -> bool {
    return is_type_trait_with_value<is_invocable<Fn, Args...>, Expectation> &&
           is_invocable_v<Fn, Args...> == Expectation && is_invoke_result_correct<Fn, Args...> &&
           is_type_trait_with_value<is_invocable_r<void, Fn, Args...>, Expectation> &&
           is_invocable_r_v<void, Fn, Args...> == Expectation;
  }
};

template <std::size_t Line, typename Ret, typename Fn>
struct test_is_invocable<Line, false, Ret, Fn> final {
  template <bool Expectation, typename... Args>
  static constexpr auto test(type_list<Args...>) noexcept -> bool {
    return is_type_trait_with_value<is_invocable_r<Ret, Fn, Args...>, Expectation> &&
           is_invocable_r_v<Ret, Fn, Args...> == Expectation && is_invoke_result_correct<Fn, Args...>;
  }
};

template <std::size_t Line, typename Fn>
struct test_is_invocable<Line, true, void, Fn> final {
  template <bool Expectation, typename... Args>
  static constexpr auto test(type_list<Args...>) noexcept -> bool {
    return is_type_trait_with_value<is_nothrow_invocable<Fn, Args...>, Expectation> &&
           is_nothrow_invocable_v<Fn, Args...> == Expectation && is_invoke_result_correct<Fn, Args...> &&
           is_type_trait_with_value<is_nothrow_invocable_r<void, Fn, Args...>, Expectation> &&
           is_nothrow_invocable_r_v<void, Fn, Args...> == Expectation;
  }
};

template <std::size_t Line, typename Ret, typename Fn>
struct test_is_invocable<Line, true, Ret, Fn> final {
  template <bool Expectation, typename... Args>
  static constexpr auto test(type_list<Args...>) noexcept -> bool {
    return is_type_trait_with_value<is_nothrow_invocable_r<Ret, Fn, Args...>, Expectation> &&
           is_nothrow_invocable_r_v<Ret, Fn, Args...> == Expectation && is_invoke_result_correct<Fn, Args...>;
  }
};

template <std::size_t Line, typename T, typename MemFn>
struct test_is_invocable<Line, false, void, std::pair<T, MemFn>> final {
  template <bool Expectation, typename... Args>
  static constexpr auto test(type_list<Args...>) noexcept -> bool {
    return is_type_trait_with_value<is_invocable<MemFn, T, Args...>, Expectation> &&
           is_invocable_v<MemFn, T, Args...> == Expectation && is_invoke_result_correct<MemFn, T, Args...> &&
           is_type_trait_with_value<is_invocable_r<void, MemFn, T, Args...>, Expectation> &&
           is_invocable_r_v<void, MemFn, T, Args...> == Expectation;
  }
};

template <std::size_t Line, typename Ret, typename T, typename MemFn>
struct test_is_invocable<Line, false, Ret, std::pair<T, MemFn>> final {
  template <bool Expectation, typename... Args>
  static constexpr auto test(type_list<Args...>) noexcept -> bool {
    return is_type_trait_with_value<is_invocable_r<Ret, MemFn, T, Args...>, Expectation> &&
           is_invocable_r_v<Ret, MemFn, T, Args...> == Expectation && is_invoke_result_correct<MemFn, T, Args...>;
  }
};

template <std::size_t Line, typename T, typename MemFn>
struct test_is_invocable<Line, true, void, std::pair<T, MemFn>> final {
  template <bool Expectation, typename... Args>
  static constexpr auto test(type_list<Args...>) noexcept -> bool {
    return is_type_trait_with_value<is_nothrow_invocable<MemFn, T, Args...>, Expectation> &&
           is_nothrow_invocable_v<MemFn, T, Args...> == Expectation && is_invoke_result_correct<MemFn, T, Args...> &&
           is_type_trait_with_value<is_nothrow_invocable_r<void, MemFn, T, Args...>, Expectation> &&
           is_nothrow_invocable_r_v<void, MemFn, T, Args...> == Expectation;
  }
};

template <std::size_t Line, typename Ret, typename T, typename MemFn>
struct test_is_invocable<Line, true, Ret, std::pair<T, MemFn>> final {
  template <bool Expectation, typename... Args>
  static constexpr auto test(type_list<Args...>) noexcept -> bool {
    return is_nothrow_invocable_r<Ret, MemFn, T, Args...>::value == Expectation &&
           is_nothrow_invocable_r_v<Ret, MemFn, T, Args...> == Expectation &&
           is_invoke_result_correct<MemFn, T, Args...>;
  }
};

template <typename Fns, bool ExpectNoexcept, typename TestCases>
struct test_runner final {
  static void run_all() noexcept {
    run_all(std::make_index_sequence<size_v<Fns>>{}, std::make_index_sequence<size_v<TestCases>>{});
  }

 private:
  template <std::size_t... Is, std::size_t... Js>
  static void run_all(std::index_sequence<Is...> idxs, std::index_sequence<Js...> jdxs) noexcept {
    run_each_callable(idxs, jdxs);
  }

  template <std::size_t I, std::size_t... Js>
  static void run_each_callable(std::index_sequence<I>, std::index_sequence<Js...>) noexcept {
    run_each_test<I, Js...>();
  }

  template <std::size_t I, std::size_t I2, std::size_t... Is, std::size_t... Js>
  static void run_each_callable(std::index_sequence<I, I2, Is...>, std::index_sequence<Js...> jdxs) noexcept {
    run_each_test<I, Js...>();
    run_each_callable(std::index_sequence<I2, Is...>{}, jdxs);
  }

  template <std::size_t I, std::size_t J>
  static void run_each_test() noexcept {
    run_test<I, J>();
  }

  template <std::size_t I, std::size_t J, std::size_t J2, std::size_t... Js>
  static void run_each_test() noexcept {
    run_test<I, J>();
    run_each_test<I, J2, Js...>();
  }

  template <std::size_t I, std::size_t J>
  static void run_test() noexcept {
    using fn = at_t<Fns, I>;
    using test_case = at_t<TestCases, J>;
    static constexpr std::size_t line = at_t<test_case, 0U>::value;
    static constexpr bool expectation = at_t<test_case, 1U>::value;
    using ret = at_t<test_case, 2U>;
    using args = at_t<test_case, 3U>;
    static_assert(
        test_is_invocable<line, ExpectNoexcept, ret, fn>::template test<expectation>(args{}),
        "is_invocable_test"
    );
  }
};

template <std::size_t LineNo>
struct line_no final {
  static constexpr std::size_t value = LineNo;
};

// A macro to add the line number as part of the test case type to make debugging easier
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage) test macro wrapper
#define TEST_CASE(...) type_list<line_no<__LINE__>, __VA_ARGS__>

// Aliases and types to make test cases more readable
using throwing = std::false_type;
using nothrow = std::true_type;
using succeeds = std::true_type;
using fails = std::false_type;
template <typename... Ts>
using callables = type_list<Ts...>;
template <typename... Ts>
using test_group = type_list<Ts...>;
template <typename... Ts>
using test_cases = type_list<Ts...>;
template <typename... Ts>
using arg_types = type_list<Ts...>;

template <typename T>
struct not_reference_wrapper final {
  auto get() noexcept -> T& { return *t_; }
  auto get() const noexcept -> T const& { return *t_; }

 private:
  T* t_{nullptr};
};

template <typename T, typename Ret, typename... Args>
struct members final {
 private:
  using t = T;
  using ct = T const;
  using tref = T&;
  using ctref = T const&;
  using trref = T&&;
  using tptr = T*;
  using ctptr = T const*;
  using tuptr = std::unique_ptr<T>;
  using ctuptr = std::unique_ptr<T const>;
  using trefw = std::reference_wrapper<T>;
  using ctrefw = std::reference_wrapper<T const>;
  using trefwr = std::reference_wrapper<T>&;
  using ctrefwr = std::reference_wrapper<T const>&;
  using t_not_refw = not_reference_wrapper<T>;
  using ct_not_refw = not_reference_wrapper<T const>;

  using memfp = Ret (test_struct1::*)(Args...);
  using memfpc = Ret (test_struct1::*)(Args...) const;
  using memfpr = Ret (test_struct1::*)(Args...) &;
  using memfpcr = Ret (test_struct1::*)(Args...) const&;
  using memfprr = Ret (test_struct1::*)(Args...) &&;

 public:
  // clang-format off
  using valid_variable_pointers = type_list<
      pr<t, r0 test_struct1::*>,
      pr<tref, r0 test_struct1::*>,
      pr<trref, r0 test_struct1::*>,
      pr<tptr, r0 test_struct1::*>,
      pr<trefw, r0 test_struct1::*>,
      pr<ct, r0 test_struct1::*>,
      pr<ctref, r0 test_struct1::*>,
      pr<ctptr, r0 test_struct1::*>,
      pr<ctrefw, r0 test_struct1::*>>;
  using throwing_variable_pointers = type_list<
      pr<tuptr, r0 test_struct1::*>,
      pr<ctuptr, r0 test_struct1::*>>;
  using invalid_variable_pointers = type_list<
      pr<t_not_refw, r0 test_struct1::*>,
      pr<ct_not_refw, r0 test_struct1::*>>;
  using valid_function_invocations = callables<
      pr<t, memfp>, pr<t, memfpc>, pr<t, memfprr>,
      pr<ct, memfpc>,
      pr<tref, memfp>, pr<tref, memfpc>, pr<tref, memfpr>, pr<tref, memfpcr>,
      pr<ctref, memfpc>, pr<ctref, memfpcr>,
      pr<trref, memfp>, pr<trref, memfpc>, pr<trref, memfprr>,
      pr<tptr, memfp>, pr<tptr, memfpc>, pr<tptr, memfpr>, pr<tptr, memfpcr>,
      pr<ctptr, memfpc>, pr<ctptr, memfpcr>,
      pr<tuptr, memfp>, pr<tuptr, memfpc>, pr<tuptr, memfpr>,
        pr<tuptr, memfpcr>,
      pr<ctuptr, memfpc>, pr<ctuptr, memfpcr>,
      pr<trefw, memfp>, pr<trefw, memfpc>, pr<trefw, memfpr>,
        pr<trefw, memfpcr>,
    pr<ctrefw, memfpc>, pr<ctrefw, memfpcr>,
      pr<trefwr, memfp>, pr<trefwr, memfpc>, pr<trefwr, memfpr>,
        pr<trefwr, memfpcr>,
      pr<ctrefwr, memfpc>, pr<ctrefwr, memfpcr>>;
  using invalid_function_invocations = callables<
      pr<t, memfpr>, pr<t, memfpcr>,
      pr<ct, memfp>, pr<ct, memfpr>, pr<ct, memfpcr>, pr<ct, memfprr>,
      pr<tref, memfprr>,
      pr<ctref, memfp>, pr<ctref, memfpr>, pr<ctref, memfprr>,
      pr<trref, memfpr>, pr<trref, memfpcr>,
      pr<tptr, memfprr>,
      pr<ctptr, memfp>, pr<ctptr, memfpr>, pr<ctptr, memfprr>,
      pr<tuptr, memfprr>,
      pr<ctuptr, memfp>, pr<ctuptr, memfpr>, pr<ctuptr, memfprr>,
      pr<trefw, memfprr>,
      pr<ctrefw, memfp>, pr<ctrefw, memfpr>, pr<ctrefw, memfprr>,
      pr<t_not_refw, memfp>, pr<t_not_refw, memfpc>, pr<t_not_refw, memfpr>,
        pr<t_not_refw, memfpcr>, pr<t_not_refw, memfprr>,
      pr<ct_not_refw, memfpc>, pr<ct_not_refw, memfpcr>,
        pr<ct_not_refw, memfp>, pr<ct_not_refw, memfpr>,
        pr<ct_not_refw, memfprr>>;
  // clang-format on
};

template <typename Lambda, typename Ret, typename... Args>
struct callable_tests final {
 private:
  using basic_types = callables<Ret (*)(Args...), Lambda, std::function<Ret(Args...)>, test_functor<Ret, Args...>>;
  using ts1_members = members<test_struct1, Ret, Args...>;
  using ts3_members = members<test_struct3, Ret, Args...>;

 public:
  using valid_member_pointers =
      concat_t<typename ts1_members::valid_variable_pointers, typename ts3_members::valid_variable_pointers>;
  using throwing_member_pointers =
      concat_t<typename ts1_members::throwing_variable_pointers, typename ts3_members::throwing_variable_pointers>;
  using invalid_member_pointers =
      concat_t<typename ts1_members::invalid_variable_pointers, typename ts3_members::invalid_variable_pointers>;
  using valid_function_invocations = concat_t<
      basic_types,
      typename ts1_members::valid_function_invocations,
      typename ts3_members::valid_function_invocations>;
  using invalid_function_invocations =
      concat_t<typename ts1_members::invalid_function_invocations, typename ts3_members::invalid_function_invocations>;
};

using zero_arg_void = callable_tests<decltype(lv0), void>;
using valid_member_pointers = typename zero_arg_void::valid_member_pointers;
using throwing_member_pointers = typename zero_arg_void::throwing_member_pointers;
using invalid_member_pointers = typename zero_arg_void::invalid_member_pointers;
using zero_arg_void_callables = typename zero_arg_void::valid_function_invocations;
using invalid_zero_arg_void_mem_funcs = typename zero_arg_void::invalid_function_invocations;
using one_arg_void = callable_tests<decltype(lv1), void, p1_1>;
using one_arg_void_callables = typename one_arg_void::valid_function_invocations;
using invalid_one_arg_void_mem_funcs = typename one_arg_void::invalid_function_invocations;
using two_arg_void = callable_tests<decltype(lv2), void, p2_1, p2_2>;
using two_arg_void_callables = typename two_arg_void::valid_function_invocations;
using invalid_two_arg_void_mem_funcs = typename two_arg_void::invalid_function_invocations;
using zero_arg_ret = callable_tests<decltype(lr0), r0>;
using zero_arg_ret_callables = typename zero_arg_ret::valid_function_invocations;
using invalid_zero_arg_ret_mem_funcs = typename zero_arg_ret::invalid_function_invocations;
using one_arg_ret = callable_tests<decltype(lr1), r1, pr1_1>;
using one_arg_ret_callables = typename one_arg_ret::valid_function_invocations;
using invalid_one_arg_ret_mem_funcs = typename one_arg_ret::invalid_function_invocations;
using two_arg_ret = callable_tests<decltype(lr2), r2, pr2_1, pr2_2>;
using two_arg_ret_callables = typename two_arg_ret::valid_function_invocations;
using invalid_two_arg_ret_mem_funcs = typename two_arg_ret::invalid_function_invocations;
// Function pointers, member function pointers, and std::function cannot be
// tested by the noexcept operator in C++14
using zero_arg_void_nothrow_callables = callables<decltype(lv0_no), ftorv0_no>;
using one_arg_void_nothrow_callables = callables<decltype(lv1_no), ftorv1_no>;
using two_arg_void_nothrow_callables = callables<decltype(lv2_no), ftorv2_no>;
using zero_arg_ret_nothrow_callables = callables<decltype(lr0_no), ftorr0_no>;
using one_arg_ret_nothrow_callables = callables<decltype(lr1_no), ftorr1_no>;
using two_arg_ret_nothrow_callables = callables<decltype(lr2_no), ftorr2_no>;

template <typename T>
class IsInvocableTest : public ::testing::Test {};

TYPED_TEST_SUITE_P(IsInvocableTest);

/// @test Evaluate `is_invocable_v`, `is_invocable_r_v`, `is_nothrow_invocable_v` and `is_nothrow_invocable_r_v`  for
/// the specified types, and ensure they have the appropriate results.
/// @tparam TypeParam An instantiation of `test_group` holding the list of potentially-invocable types to check, a flag
/// to indicate if the checks is to be performed for the `is_invocable` or `is_nothrow_invocable` traits, and a list of
/// checks to perform. Each check indicates the list of arguments to check against the invocable, the return type to use
/// with the `_r_v` traits, and whether the trait is supposed to evaluate to `true` (`succeeds`), or `false` (`fails`)
TYPED_TEST_P(IsInvocableTest, TestTest) {
  using fn = at_t<TypeParam, 0U>;
  static constexpr bool expect_noexcept = at_t<TypeParam, 1U>::type::value;
  using test_cases = at_t<TypeParam, 2U>;
  test_runner<fn, expect_noexcept, test_cases>::run_all();
}

REGISTER_TYPED_TEST_SUITE_P(IsInvocableTest, TestTest);

// clang-format off
using IsInvocableVoidTestTypes = ::testing::Types<
    test_group<
      valid_member_pointers, nothrow, test_cases<
        TEST_CASE(succeeds, void, arg_types<>),
        TEST_CASE(succeeds, r0, arg_types<>),
        TEST_CASE(succeeds, r0_impconv, arg_types<>)
      >>,
    test_group<
      valid_member_pointers, nothrow, test_cases<
        TEST_CASE(fails, r0_notimpconv, arg_types<>),
        TEST_CASE(fails, r0, arg_types<int>)
      >>,
    test_group<
      throwing_member_pointers, throwing, test_cases<
        TEST_CASE(succeeds, void, arg_types<>),
        TEST_CASE(succeeds, r0, arg_types<>),
        TEST_CASE(succeeds, r0_impconv, arg_types<>)
      >>,
    test_group<
      throwing_member_pointers, throwing, test_cases<
        TEST_CASE(fails, r0_notimpconv, arg_types<>),
        TEST_CASE(fails, r0, arg_types<int>)
      >>,
    test_group<
      invalid_member_pointers, nothrow, test_cases<
        TEST_CASE(fails, r0, arg_types<>)
      >>,
    test_group<
      zero_arg_void_callables, throwing, test_cases<
        TEST_CASE(succeeds, void, arg_types<>),
        TEST_CASE(fails, void, arg_types<double, std::int32_t>),
        TEST_CASE(fails, r0, arg_types<>)
      >>,
    test_group<
      invalid_zero_arg_void_mem_funcs, throwing, test_cases<
        TEST_CASE(fails, void, arg_types<>)
      >>,
    test_group<
      zero_arg_void_callables, nothrow, test_cases<
        TEST_CASE(fails, void, arg_types<>)
      >>,
    test_group<
      zero_arg_void_nothrow_callables, nothrow, test_cases<
        TEST_CASE(succeeds, void, arg_types<>),
        TEST_CASE(fails, void, arg_types<double, std::int32_t>),
        TEST_CASE(fails, r0_notimpconv, arg_types<>)
      >>,
    test_group<
      one_arg_void_callables, throwing, test_cases<
        TEST_CASE(fails, void, arg_types<>),
        TEST_CASE(succeeds, void, arg_types<p1_1>),
        TEST_CASE(succeeds, void, arg_types<p1_1_impconv>),
        TEST_CASE(fails, void, arg_types<p1_1, float>),
        TEST_CASE(fails, r1_notimpconv, arg_types<p1_1>)
      >>,
    test_group<
      invalid_one_arg_void_mem_funcs, throwing, test_cases<
        TEST_CASE(fails, void, arg_types<p1_1>)
      >>,
    test_group<
      one_arg_void_callables, nothrow, test_cases<
        TEST_CASE(fails, void, arg_types<p1_1>)
      >>,
    test_group<
      one_arg_void_nothrow_callables, nothrow, test_cases<
        TEST_CASE(fails, void, arg_types<>),
        TEST_CASE(succeeds, void, arg_types<p1_1>),
        TEST_CASE(succeeds, void, arg_types<p1_1_impconv>),
        TEST_CASE(fails, void, arg_types<p1_1, float>),
        TEST_CASE(fails, r1_notimpconv, arg_types<p1_1>)
      >>,
    test_group<
      two_arg_void_callables, throwing, test_cases<
        TEST_CASE(fails, void, arg_types<>),
        TEST_CASE(succeeds, void, arg_types<p2_1, p2_2>),
        TEST_CASE(succeeds, void, arg_types<p2_1, p2_2_impconv>),
        TEST_CASE(fails, void, arg_types<p2_1, p2_2, double>),
        TEST_CASE(fails, r2_notimpconv, arg_types<p2_1, p2_2>)
      >>,
    test_group<
      invalid_two_arg_void_mem_funcs, throwing, test_cases<
        TEST_CASE(fails, void, arg_types<p2_1, p2_2>)
      >>,
    test_group<
      two_arg_void_callables, nothrow, test_cases<
        TEST_CASE(fails, void, arg_types<p2_1, p2_2>)
      >>,
    test_group<
      two_arg_void_nothrow_callables, nothrow, test_cases<
        TEST_CASE(fails, void, arg_types<>),
        TEST_CASE(succeeds, void, arg_types<p2_1, p2_2>),
        TEST_CASE(succeeds, void, arg_types<p2_1, p2_2_impconv>),
        TEST_CASE(fails, void, arg_types<p2_1, p2_2, double>),
        TEST_CASE(fails, r2_notimpconv, arg_types<p2_1, p2_2>)
      >>
    >;
// Tests are separated into two type groups because armclang fails to build
// them when they are combined (segfault, possibly related to the extraordinary
// length of type names).
using IsInvocableRetTestTypes = ::testing::Types<
    test_group<
      zero_arg_ret_callables, throwing, test_cases<
        TEST_CASE(succeeds, void, arg_types<>),
        TEST_CASE(fails, void, arg_types<double>),
        TEST_CASE(succeeds, r0, arg_types<>),
        TEST_CASE(succeeds, r0_impconv, arg_types<>),
        TEST_CASE(fails, r0, arg_types<double>),
        TEST_CASE(fails, r0_impconv, arg_types<std::int16_t>),
        TEST_CASE(fails, r0_notimpconv, arg_types<>)
      >>,
    test_group<
      invalid_zero_arg_ret_mem_funcs, throwing, test_cases<
        TEST_CASE(fails, r0, arg_types<>)
      >>,
    test_group<
      zero_arg_ret_callables, nothrow, test_cases<
        TEST_CASE(fails, r0, arg_types<>)
      >>,
    test_group<
      zero_arg_ret_nothrow_callables, nothrow, test_cases<
        TEST_CASE(succeeds, void, arg_types<>),
        TEST_CASE(fails, void, arg_types<double>),
        TEST_CASE(succeeds, r0, arg_types<>),
        TEST_CASE(succeeds, r0_impconv, arg_types<>),
        TEST_CASE(fails, r0, arg_types<double>),
        TEST_CASE(fails, r0_impconv, arg_types<std::int16_t>),
        TEST_CASE(fails, r0_notimpconv, arg_types<>)
      >>,
    test_group<
      one_arg_ret_callables, throwing, test_cases<
        TEST_CASE(fails, void, arg_types<>),
        TEST_CASE(succeeds, void, arg_types<pr1_1>),
        TEST_CASE(succeeds, void, arg_types<pr1_1_impconv>),
        TEST_CASE(fails, void, arg_types<pr1_1, float>),
        TEST_CASE(succeeds, r1, arg_types<pr1_1>),
        TEST_CASE(succeeds, r1, arg_types<pr1_1_impconv>),
        TEST_CASE(fails, r1_notimpconv, arg_types<pr1_1>)
      >>,
    test_group<
      invalid_one_arg_ret_mem_funcs, throwing, test_cases<
        TEST_CASE(fails, r1, arg_types<pr1_1>)
      >>,
    test_group<
      one_arg_ret_callables, nothrow, test_cases<
        TEST_CASE(fails, r1, arg_types<pr1_1>)
      >>,
    test_group<
      one_arg_ret_nothrow_callables, nothrow, test_cases<
        TEST_CASE(fails, void, arg_types<>),
        TEST_CASE(succeeds, void, arg_types<pr1_1>),
        TEST_CASE(succeeds, void, arg_types<pr1_1_impconv>),
        TEST_CASE(fails, void, arg_types<pr1_1, float>),
        TEST_CASE(succeeds, r1, arg_types<pr1_1>),
        TEST_CASE(succeeds, r1, arg_types<pr1_1_impconv>),
        TEST_CASE(fails, r1_notimpconv, arg_types<pr1_1>)
      >>,
    test_group<
      two_arg_ret_callables, throwing, test_cases<
        TEST_CASE(fails, void, arg_types<>),
        TEST_CASE(succeeds, void, arg_types<pr2_1, pr2_2>),
        TEST_CASE(succeeds, void, arg_types<pr2_1, pr2_2_impconv>),
        TEST_CASE(fails, void, arg_types<pr2_1, pr2_2, char>),
        TEST_CASE(succeeds, r2, arg_types<pr2_1, pr2_2>),
        TEST_CASE(succeeds, r2, arg_types<pr2_1, pr2_2_impconv>),
        TEST_CASE(succeeds, r2_impconv, arg_types<pr2_1, pr2_2>),
        TEST_CASE(fails, r2_notimpconv, arg_types<pr2_1, pr2_2>)
      >>,
    test_group<
      invalid_two_arg_ret_mem_funcs, throwing, test_cases<
        TEST_CASE(fails, r2, arg_types<pr2_1, pr2_2>)
      >>,
    test_group<
      two_arg_ret_callables, nothrow, test_cases<
        TEST_CASE(fails, r2, arg_types<pr2_1, pr2_2>)
      >>,
    test_group<
      two_arg_ret_nothrow_callables, nothrow, test_cases<
        TEST_CASE(fails, void, arg_types<>),
        TEST_CASE(succeeds, void, arg_types<pr2_1, pr2_2>),
        TEST_CASE(succeeds, void, arg_types<pr2_1, pr2_2_impconv>),
        TEST_CASE(fails, void, arg_types<pr2_1, pr2_2, char>),
        TEST_CASE(succeeds, r2, arg_types<pr2_1, pr2_2>),
        TEST_CASE(succeeds, r2, arg_types<pr2_1, pr2_2_impconv>),
        TEST_CASE(succeeds, r2_impconv, arg_types<pr2_1, pr2_2>),
        TEST_CASE(fails, r2_notimpconv, arg_types<pr2_1, pr2_2>)
      >>
    >;

// clang-format on
ARENE_IGNORE_START();
ARENE_IGNORE_CLANG("-Wgnu-zero-variadic-macro-arguments", "Test code: use of GNU extension OK.");
ARENE_IGNORE_CLANG("-Wshorten-64-to-32", "Test code is only checking that shortening is possible.");
INSTANTIATE_TYPED_TEST_SUITE_P(AreneVoid, IsInvocableTest, IsInvocableVoidTestTypes);
INSTANTIATE_TYPED_TEST_SUITE_P(AreneRet, IsInvocableTest, IsInvocableRetTestTypes);
ARENE_IGNORE_END();

}  // namespace
