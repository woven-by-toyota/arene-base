// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/type_traits/is_binary_predicate.hpp"
#include "arene/base/type_traits/is_compare.hpp"
#include "arene/base/type_traits/tests/predicate_test_utilities.hpp"
#include "testlibs/utilities/configurable_value.hpp"  // IWYU pragma: keep

// NOLINTBEGIN(readability-identifier-length)

namespace {

using ::arene::base::is_binary_predicate_v;
using ::arene::base::is_compare_v;
using type_traits_test::lambda_predicate_candidates;
using type_traits_test::tuple_all_of;
using type_traits_test::tuple_none_of;

struct dummy {};

/// @test @ is_binary_predicate and @c is_compare return @c true for function
/// objects and function pointers
TEST(IsCompare, InvocableTypesAreTrue) {
  EXPECT_TRUE(tuple_all_of(  //
      lambda_predicate_candidates<int, int>(),
      [](auto f) -> bool {                                       //
        return is_binary_predicate_v<decltype(f), int, int> &&   //
               is_binary_predicate_v<decltype(+f), int, int> &&  //
               is_compare_v<decltype(f), int, int> &&            //
               is_compare_v<decltype(+f), int, int>;
      }
  ));

  EXPECT_TRUE(tuple_all_of(  //
      lambda_predicate_candidates<int, int>(),
      [](auto f) -> bool {                                         //
        return is_binary_predicate_v<decltype(f), char, char> &&   //
               is_binary_predicate_v<decltype(+f), char, char> &&  //
               is_compare_v<decltype(f), char, char> &&            //
               is_compare_v<decltype(+f), char, char>;
      }
  ));

  EXPECT_TRUE(tuple_all_of(  //
      lambda_predicate_candidates<int, int>(),
      [](auto f) -> bool {                                  //
        return is_binary_predicate_v<decltype(f), int> &&   //
               is_binary_predicate_v<decltype(+f), int> &&  //
               is_compare_v<decltype(f), int> &&            //
               is_compare_v<decltype(+f), int>;
      }
  ));

  auto const g_arg2 = [](auto&&, auto&&) -> bool { return {}; };

  EXPECT_TRUE((is_binary_predicate_v<decltype(g_arg2), int, int>));
  EXPECT_TRUE((is_binary_predicate_v<decltype(g_arg2), int, char>));
  EXPECT_TRUE((is_binary_predicate_v<decltype(g_arg2), int*, int*>));

  EXPECT_TRUE((is_compare_v<decltype(g_arg2), int, int>));
  EXPECT_TRUE((is_compare_v<decltype(g_arg2), int, char>));
  EXPECT_TRUE((is_compare_v<decltype(g_arg2), int*, int*>));

  auto const g_argn = [](auto&&...) -> bool { return {}; };

  EXPECT_TRUE((is_binary_predicate_v<decltype(g_argn), int, int>));
  EXPECT_TRUE((is_binary_predicate_v<decltype(g_argn), int, char>));
  EXPECT_TRUE((is_binary_predicate_v<decltype(g_argn), int*, int*>));

  EXPECT_TRUE((is_compare_v<decltype(g_argn), int, int>));
  EXPECT_TRUE((is_compare_v<decltype(g_argn), int, char>));
  EXPECT_TRUE((is_compare_v<decltype(g_argn), int*, int*>));
}

/// @test @c is_binary_predicate and @c is_compare return @c false for function
/// objects and function pointers if called with the wrong argument types
TEST(IsCompare, WithWrongArgType) {
  EXPECT_TRUE(tuple_all_of(  //
      lambda_predicate_candidates<dummy, dummy>(),
      [](auto f) -> bool {                                           //
        return is_binary_predicate_v<decltype(f), dummy, dummy> ||   //
               is_binary_predicate_v<decltype(+f), dummy, dummy> ||  //
               is_compare_v<decltype(f), dummy, dummy> ||            //
               is_compare_v<decltype(+f), dummy, dummy>;
      }
  ));
  EXPECT_TRUE(tuple_none_of(  //
      lambda_predicate_candidates<int, int>(),
      [](auto f) -> bool {                                           //
        return is_binary_predicate_v<decltype(f), dummy, dummy> ||   //
               is_binary_predicate_v<decltype(+f), dummy, dummy> ||  //
               is_compare_v<decltype(f), dummy, dummy> ||            //
               is_compare_v<decltype(+f), dummy, dummy>;
      }
  ));
  EXPECT_TRUE(tuple_none_of(  //
      lambda_predicate_candidates<dummy, dummy>(),
      [](auto f) -> bool {                                       //
        return is_binary_predicate_v<decltype(f), int, int> ||   //
               is_binary_predicate_v<decltype(+f), int, int> ||  //
               is_compare_v<decltype(f), int, int> ||            //
               is_compare_v<decltype(+f), int, int>;
      }
  ));

  auto const g_arg2 = [](auto arg1, auto arg2)  //
      -> std::enable_if_t<std::is_integral<decltype(arg1)>::value && std::is_integral<decltype(arg2)>::value, bool> {
    return {};
  };

  EXPECT_TRUE((is_binary_predicate_v<decltype(g_arg2), int, int>));
  EXPECT_TRUE((!is_binary_predicate_v<decltype(g_arg2), int, dummy>));
  EXPECT_TRUE((!is_binary_predicate_v<decltype(g_arg2), int, int*>));
  EXPECT_TRUE((!is_binary_predicate_v<decltype(g_arg2), dummy, int>));

  EXPECT_TRUE((is_compare_v<decltype(g_arg2), int, int>));
  EXPECT_TRUE((!is_compare_v<decltype(g_arg2), int, dummy>));
  EXPECT_TRUE((!is_compare_v<decltype(g_arg2), int, int*>));
  EXPECT_TRUE((!is_compare_v<decltype(g_arg2), dummy, int>));
}

/// @test @c is_binary_predicate and @c is_compare return @c false for function
/// objects and function pointers if not invocable with const arguments
TEST(IsCompare, NotInvocableWithConstArgs) {
  auto const only_mutable_lvalue_refs = [](int&, int&) -> bool { return {}; };

  EXPECT_TRUE((!is_binary_predicate_v<decltype(only_mutable_lvalue_refs), int&, int&>));
  EXPECT_TRUE((!is_binary_predicate_v<decltype(+only_mutable_lvalue_refs), int&, int&>));

  EXPECT_TRUE((!is_compare_v<decltype(only_mutable_lvalue_refs), int&, int&>));
  EXPECT_TRUE((!is_compare_v<decltype(+only_mutable_lvalue_refs), int&, int&>));
}

/// @test @c is_binary_predicate and @c is_compare return @c false for function
/// objects invoked with the wrong ref qualifier arguments
TEST(IsCompare, NotInvocableWithWrongRefQualifier) {
  struct only_lvalue_ref_invocable {
    auto operator()(int, int) & -> bool { return true; }
  };

  EXPECT_TRUE((is_binary_predicate_v<only_lvalue_ref_invocable&, int, int>));
  EXPECT_TRUE((!is_binary_predicate_v<only_lvalue_ref_invocable const&, int, int>));
  EXPECT_TRUE((!is_binary_predicate_v<only_lvalue_ref_invocable&&, int, int>));
  EXPECT_TRUE((!is_binary_predicate_v<only_lvalue_ref_invocable const&&, int, int>));

  EXPECT_TRUE((is_compare_v<only_lvalue_ref_invocable&, int, int>));
  EXPECT_TRUE((!is_compare_v<only_lvalue_ref_invocable const&, int, int>));
  EXPECT_TRUE((!is_compare_v<only_lvalue_ref_invocable&&, int, int>));
  EXPECT_TRUE((!is_compare_v<only_lvalue_ref_invocable const&&, int, int>));
}

/// @test @c is_compare returns @c false for function objects and function
/// pointers arguments cannot be flipped while @c is_binary_predicate returns @c
/// true if the arguments are provided in the correct order
TEST(IsCompare, ArgsNotFlippable) {
  auto const not_flippable = [](dummy, int) -> bool { return {}; };

  EXPECT_TRUE((is_binary_predicate_v<decltype(not_flippable), dummy, int>));
  EXPECT_TRUE((is_binary_predicate_v<decltype(+not_flippable), dummy, int>));

  EXPECT_TRUE((!is_binary_predicate_v<decltype(not_flippable), int, dummy>));
  EXPECT_TRUE((!is_binary_predicate_v<decltype(+not_flippable), int, dummy>));

  EXPECT_TRUE((!is_compare_v<decltype(not_flippable), dummy, int>));
  EXPECT_TRUE((!is_compare_v<decltype(+not_flippable), dummy, int>));
}

/// @brief function object that always returns a default constructed type
/// @tparam T type to return
/// @tparam Disable special member function to disable in the function object
template <class T, testing::disable Disable = testing::disable::nothing>
struct always_returns  //
    : testing::configurable_value<int, testing::throws_on::nothing, Disable> {
  template <class... Args>
  auto operator()(Args const&...) const -> T {
    return {};
  }
};

/// @test @c is_binary_predicate and @c is_compare return @c false for function
/// objects that are not copy constructible
TEST(IsCompare, NotCopyConstructible) {
  using testing::disable;

  EXPECT_TRUE((is_binary_predicate_v<always_returns<int>, int, int>));
  EXPECT_TRUE((is_binary_predicate_v<always_returns<int, disable::copy_assign>, int, int>));
  EXPECT_TRUE((is_binary_predicate_v<always_returns<int, disable::copy_assign | disable::move_assign>, int, int>));

  EXPECT_TRUE((is_compare_v<always_returns<int>, int, int>));
  EXPECT_TRUE((is_compare_v<always_returns<int, disable::copy_assign>, int, int>));
  EXPECT_TRUE((is_compare_v<always_returns<int, disable::copy_assign | disable::move_assign>, int, int>));

  EXPECT_TRUE((!is_binary_predicate_v<always_returns<int, disable::copy_construct>, int, int>));
  EXPECT_TRUE((!is_binary_predicate_v<always_returns<int, disable::move_construct>, int, int>));
  EXPECT_TRUE((!is_binary_predicate_v<always_returns<int, disable::copy_construct | disable::move_construct>, int, int>)
  );

  EXPECT_TRUE((!is_compare_v<always_returns<int, disable::copy_construct>, int, int>));
  EXPECT_TRUE((!is_compare_v<always_returns<int, disable::move_construct>, int, int>));
  EXPECT_TRUE((!is_compare_v<always_returns<int, disable::copy_construct | disable::move_construct>, int, int>));
}

/// @test @c is_binary_predicate and @c is_compare return @c false for function
/// objects and function pointers if invocation returns the wrong type
TEST(IsCompare, WithWrongReturnType) {
  struct not_convertible_to_bool {};

  // NOLINTBEGIN(hicpp-explicit-conversions)

  struct convertible_to_bool {
    operator bool() const { return {}; }
  };
  struct explicitly_convertible_to_bool {
    explicit operator bool() const { return {}; }
  };
  struct convertible_to_int {
    operator int() const { return {}; }
  };
  struct deleted_conversion_to_bool {
    operator bool() = delete;
    operator int() const { return {}; }
  };

  // NOLINTEND(hicpp-explicit-conversions)

  EXPECT_TRUE((!is_binary_predicate_v<always_returns<not_convertible_to_bool>, int, int>));
  EXPECT_TRUE((is_binary_predicate_v<always_returns<convertible_to_bool>, int, int>));
  EXPECT_TRUE((!is_binary_predicate_v<always_returns<explicitly_convertible_to_bool>, int, int>));
  EXPECT_TRUE((is_binary_predicate_v<always_returns<convertible_to_int>, int, int>));
  EXPECT_TRUE((!is_binary_predicate_v<always_returns<deleted_conversion_to_bool>, int, int>));

  EXPECT_TRUE((!is_compare_v<always_returns<not_convertible_to_bool>, int, int>));
  EXPECT_TRUE((is_compare_v<always_returns<convertible_to_bool>, int, int>));
  EXPECT_TRUE((!is_compare_v<always_returns<explicitly_convertible_to_bool>, int, int>));
  EXPECT_TRUE((is_compare_v<always_returns<convertible_to_int>, int, int>));
  EXPECT_TRUE((!is_compare_v<always_returns<deleted_conversion_to_bool>, int, int>));
}

/// @test @c is_binary_predicate and @c is_compare return @c false for other basic types
TEST(IsCompare, OtherBasicTypes) {
  EXPECT_TRUE((!is_binary_predicate_v<int, int, int>));
  EXPECT_TRUE((!is_binary_predicate_v<int*, int, int>));
  EXPECT_TRUE((!is_binary_predicate_v<int&, int, int>));
  EXPECT_TRUE((!is_binary_predicate_v<void, int, int>));
  EXPECT_TRUE((!is_binary_predicate_v<decltype(nullptr), int, int>));
  EXPECT_TRUE((!is_binary_predicate_v<int[], int, int>));  // NOLINT(hicpp-avoid-c-arrays)

  EXPECT_TRUE((!is_compare_v<int, int, int>));
  EXPECT_TRUE((!is_compare_v<int*, int, int>));
  EXPECT_TRUE((!is_compare_v<int&, int, int>));
  EXPECT_TRUE((!is_compare_v<void, int, int>));
  EXPECT_TRUE((!is_compare_v<decltype(nullptr), int, int>));
  EXPECT_TRUE((!is_compare_v<int[], int, int>));  // NOLINT(hicpp-avoid-c-arrays)
}

}  // namespace

// NOLINTEND(readability-identifier-length)
