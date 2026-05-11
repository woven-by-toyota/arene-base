// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/optional/optional.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/nothrow_t.hpp"
#include "arene/base/stdlib_choice/stdexcept.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::bad_optional_access;
using ::arene::base::optional;

/// @test A call to an empty `optional`'s `value()` method throws a `bad_optional_access` exception, regardless of its
/// constness.
TEST(Optional, CallingValueWhenEmptyThrows) {
  optional<int> opt_int;
  optional<int> const copt_int;
  ASSERT_THROW(opt_int.value(), bad_optional_access);
  ASSERT_THROW(copt_int.value(), bad_optional_access);

  optional<int> rval_opt_int;
  // NOLINTNEXTLINE(bugprone-use-after-move) testing rvalue overload, move is non-destructive.
  ASSERT_THROW(std::move(rval_opt_int).value(), bad_optional_access);

  optional<int> const rval_copt_int;
  // NOLINTNEXTLINE(hicpp-move-const-arg,bugprone-use-after-move) testing crvalue overload
  ASSERT_THROW(std::move(rval_copt_int).value(), bad_optional_access);
}

/// @test The return type of `optional::value()` is a reference to the held type, `T`, qualified the same as the
/// `optional` instance on which it is invoked.
TEST(Optional, ReturnTypeOfValueIsTRefQualifiedAsOptional) {
  ::testing::StaticAssertTypeEq<decltype(std::declval<optional<int>&>().value()), int&>();
  ::testing::StaticAssertTypeEq<decltype(std::declval<optional<int> const&>().value()), int const&>();
  ::testing::StaticAssertTypeEq<decltype(std::declval<optional<int>&&>().value()), int&&>();
  ::testing::StaticAssertTypeEq<decltype(std::declval<optional<int> const&&>().value()), int const&&>();
}

/// @test The `value()` method returns a reference to the held value, and that reference is identical to the one
/// returned by dereferencing the `optional`.
CONSTEXPR_TEST(Optional, ValueReturnsReferenceToHeldValue) {
  {
    optional<int> value_optional{42};
    CONSTEXPR_ASSERT_EQ(&value_optional.value(), &*value_optional);
    CONSTEXPR_ASSERT_EQ(value_optional.value(), value_optional);
  }
  {
    optional<int> const value_optional{42};
    CONSTEXPR_ASSERT_EQ(&value_optional.value(), &*value_optional);
    CONSTEXPR_ASSERT_EQ(value_optional.value(), *value_optional);
  }
  {
    optional<int> value_optional{42};
    // NOLINTNEXTLINE(bugprone-use-after-move) testing rvalue overload, move is non-destructive.
    CONSTEXPR_ASSERT_EQ(std::move(value_optional).value(), *std::move(value_optional));
  }
  {
    optional<int> const value_optional{42};
    // NOLINTNEXTLINE(hicpp-move-const-arg,bugprone-use-after-move) testing crvalue overload
    CONSTEXPR_ASSERT_EQ(std::move(value_optional).value(), *std::move(value_optional));
  }
}

/// @test Given an instance of `arene::base::optional<T>` and given `T(args...)` is `noexcept(false)`, when
/// `optional.emplace(args...)` is invoked and the resulting constructor call throws, then the optional is left in the
/// disengaged state.
TEST(Optional, EmplaceWithThrowingConstructor) {
  struct ctor_error : std::exception {};
  struct throwing_constructor {
    // NOLINTNEXTLINE(hicpp-exception-baseclass)
    ARENE_NORETURN throwing_constructor() { throw ctor_error{}; }
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    throwing_constructor(std::nothrow_t) {}
  };

  optional<throwing_constructor> opt_itc;

  ASSERT_THROW(opt_itc.emplace(), ctor_error);
  ASSERT_TRUE(!opt_itc);

  opt_itc.emplace(std::nothrow);
  ASSERT_TRUE(opt_itc);
}

}  // namespace
