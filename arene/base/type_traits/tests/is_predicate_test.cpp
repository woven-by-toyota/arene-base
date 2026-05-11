// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/is_predicate.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/type_traits/tests/predicate_test_utilities.hpp"

// NOLINTBEGIN(readability-identifier-length)

namespace {

using ::arene::base::is_predicate_v;
using type_traits_test::lambda_predicate_candidates;
using type_traits_test::tuple_all_of;
using type_traits_test::tuple_none_of;

/// @test @c is_predicate returns @c true for function objects and function pointers
TEST(IsPredicate, InvocableTypesAreTrue) {
  EXPECT_TRUE(tuple_all_of(  //
      lambda_predicate_candidates<>(),
      [](auto f) -> bool {                     //
        return is_predicate_v<decltype(f)> &&  //
               is_predicate_v<decltype(+f)>;
      }
  ));

  EXPECT_TRUE(tuple_all_of(  //
      lambda_predicate_candidates<int>(),
      [](auto f) -> bool {                          //
        return is_predicate_v<decltype(f), int> &&  //
               is_predicate_v<decltype(+f), int>;
      }
  ));

  EXPECT_TRUE(tuple_all_of(  //
      lambda_predicate_candidates<int>(),
      [](auto f) -> bool {                           //
        return is_predicate_v<decltype(f), char> &&  //
               is_predicate_v<decltype(+f), char>;
      }
  ));

  auto const g = [](auto&&...) -> bool { return {}; };
  EXPECT_TRUE((is_predicate_v<decltype(g)>));
  EXPECT_TRUE((is_predicate_v<decltype(g), int>));
  EXPECT_TRUE((is_predicate_v<decltype(g), int, char>));
}

/// @test @c is_predicate returns @c false for function objects and function pointers if called with the wrong number of
/// arguments
TEST(IsPredicate, WithWrongNumberOfArgs) {
  EXPECT_TRUE(tuple_none_of(  //
      lambda_predicate_candidates<>(),
      [](auto f) -> bool {                          //
        return is_predicate_v<decltype(f), int> ||  //
               is_predicate_v<decltype(+f), int>;
      }
  ));
  EXPECT_TRUE(tuple_none_of(  //
      lambda_predicate_candidates<>(),
      [](auto f) -> bool {                               //
        return is_predicate_v<decltype(f), int, int> ||  //
               is_predicate_v<decltype(+f), int, int>;
      }
  ));

  EXPECT_TRUE(tuple_none_of(  //
      lambda_predicate_candidates<int>(),
      [](auto f) -> bool {                     //
        return is_predicate_v<decltype(f)> ||  //
               is_predicate_v<decltype(+f)>;
      }
  ));
  EXPECT_TRUE(tuple_none_of(  //
      lambda_predicate_candidates<int>(),
      [](auto f) -> bool {                               //
        return is_predicate_v<decltype(f), int, int> ||  //
               is_predicate_v<decltype(+f), int, int>;
      }
  ));

  auto const g0 = []() -> bool { return {}; };
  auto const g1 = [](auto) -> bool { return {}; };

  EXPECT_TRUE(!(is_predicate_v<decltype(g0), int>));
  EXPECT_TRUE(!(is_predicate_v<decltype(g0), int, int>));
  EXPECT_TRUE(!(is_predicate_v<decltype(g1)>));
  EXPECT_TRUE(!(is_predicate_v<decltype(g1), int, int>));
}

/// @test @c is_predicate returns @c false for function objects and function pointers if called with the wrong argument
/// types
TEST(IsPredicate, WithWrongArgType) {
  struct dummy {};
  EXPECT_TRUE(tuple_all_of(  //
      lambda_predicate_candidates<dummy>(),
      [](auto f) -> bool {                            //
        return is_predicate_v<decltype(f), dummy> &&  //
               is_predicate_v<decltype(+f), dummy>;
      }
  ));
  EXPECT_TRUE(tuple_none_of(  //
      lambda_predicate_candidates<int>(),
      [](auto f) -> bool {                            //
        return is_predicate_v<decltype(f), dummy> ||  //
               is_predicate_v<decltype(+f), dummy>;
      }
  ));
  EXPECT_TRUE(tuple_none_of(  //
      lambda_predicate_candidates<dummy>(),
      [](auto f) -> bool {                          //
        return is_predicate_v<decltype(f), int> ||  //
               is_predicate_v<decltype(+f), int>;
      }
  ));

  EXPECT_TRUE(tuple_all_of(  //
      lambda_predicate_candidates<dummy&&>(),
      [](auto f) -> bool {                              //
        return is_predicate_v<decltype(f), dummy&&> &&  //
               is_predicate_v<decltype(+f), dummy&&>;
      }
  ));
  EXPECT_TRUE(tuple_none_of(  //
      lambda_predicate_candidates<dummy&&>(),
      [](auto f) -> bool {                             //
        return is_predicate_v<decltype(f), dummy&> ||  //
               is_predicate_v<decltype(+f), dummy&>;
      }
  ));

  auto const g1 = [](auto arg) -> std::enable_if_t<std::is_integral<decltype(arg)>::value, bool> { return {}; };
  EXPECT_TRUE((is_predicate_v<decltype(g1), int>));
  EXPECT_TRUE((is_predicate_v<decltype(g1), char>));
  EXPECT_TRUE(!(is_predicate_v<decltype(g1), dummy>));
}

/// @test @c is_predicate returns @c false for function objects and function pointers if called with the wrong ref
/// qualifier
TEST(IsPredicate, WithWrongFunctionRefQual) {
  struct one_shot {
    auto operator()() && -> bool { return {}; }
  };

  EXPECT_TRUE(is_predicate_v<one_shot&&>);
  EXPECT_TRUE(!is_predicate_v<one_shot const&&>);
  EXPECT_TRUE(!is_predicate_v<one_shot&>);
  EXPECT_TRUE(!is_predicate_v<one_shot const&>);
}

template <class T>
struct always_returns {
  template <class... Args>
  auto operator()(Args const&...) const -> T {
    return {};
  }
};

/// @test @c is_predicate returns @c false for function objects and function pointers if invocation returns the wrong
/// type
TEST(IsPredicate, WithWrongReturnType) {
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

  EXPECT_TRUE(!is_predicate_v<always_returns<not_convertible_to_bool>>);
  EXPECT_TRUE(is_predicate_v<always_returns<convertible_to_bool>>);
  EXPECT_TRUE(!is_predicate_v<always_returns<explicitly_convertible_to_bool>>);
  EXPECT_TRUE(is_predicate_v<always_returns<convertible_to_int>>);
  EXPECT_TRUE(!is_predicate_v<always_returns<deleted_conversion_to_bool>>);
}

/// @test @c is_predicate returns @c false for other basic types
TEST(IsPredicate, OtherBasicTypes) {
  EXPECT_TRUE(!is_predicate_v<int>);
  EXPECT_TRUE(!is_predicate_v<int*>);
  EXPECT_TRUE(!is_predicate_v<int&>);
  EXPECT_TRUE(!is_predicate_v<void>);
  EXPECT_TRUE(!is_predicate_v<decltype(nullptr)>);
  EXPECT_TRUE(!is_predicate_v<int[]>);  // NOLINT(hicpp-avoid-c-arrays)
}

}  // namespace

// NOLINTEND(readability-identifier-length)
