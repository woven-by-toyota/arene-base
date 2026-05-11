// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @brief Provides tests for @c tuple_zip
/// @note These tests are not complete; this is not publicly exposed.
///

#include "arene/base/tuple/detail/tuple_zip.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/make_tuple.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace {

using arene::base::array;
using arene::base::tuple_detail::tuple_zip;
using std::make_tuple;
using std::tuple;

/// @test @c tuple_zip is not invocable if tuple sizes are different
TEST(TupleZip, NotInvocableIfTupleSizesAreDifferent) {
  static_assert(
      !arene::base::is_invocable_v<  //
          decltype(tuple_zip),
          tuple<>,
          tuple<int, int>>,
      "must not be invocable"
  );
}

/// @test @c tuple_zip with no arguments returns an empty tuple
CONSTEXPR_TEST(TupleZip, WithZeroArguments) {  //
  CONSTEXPR_ASSERT_EQ(tuple<>{}, tuple_zip());
}

/// @test @c tuple_zip with one argument returns a tuple where each element is
/// itself a tuple wrapping the elements of the input tuple
CONSTEXPR_TEST(TupleZip, WithOneArgument) {
  CONSTEXPR_ASSERT_EQ(  //
      (tuple<tuple<int>, tuple<int*>>{}),
      tuple_zip(tuple<int, int*>{})
  );

  CONSTEXPR_ASSERT_EQ(  //
      (tuple<tuple<int>, tuple<int>, tuple<int>>{}),
      tuple_zip(array<int, 3>{})
  );
}

/// @test @c tuple_zip with two arguments returns a tuple where each element is
/// itself a tuple wrapping the elements of the input tuple
CONSTEXPR_TEST(TupleZip, WithTwoArguments) {
  CONSTEXPR_ASSERT_EQ(
      (tuple<tuple<int, char*>, tuple<int*, char>>{}),
      tuple_zip(tuple<int, int*>{}, tuple<char*, char>{})
  );

  CONSTEXPR_ASSERT_EQ(  //
      (tuple<tuple<int, char>, tuple<int, char>, tuple<int, char>>{}),
      tuple_zip(array<int, 3>{}, array<char, 3>{})
  );
}

/// @test @c tuple_zip does not @c std::decay tuple elements
CONSTEXPR_TEST(TupleZip, DoesNotDecayTupleElements) {  //
  auto my_int = 42;
  auto const my_char = 'a';

  CONSTEXPR_ASSERT_EQ(  //
      make_tuple(tuple<int const, int* const, char const&>{my_int, &my_int, my_char}),
      tuple_zip(tuple<int const>{my_int}, tuple<int* const>{&my_int}, tuple<char const&>{my_char})
  );

  static_assert(
      std::is_same<
          std::tuple<std::tuple<int const>, std::tuple<int* const>, std::tuple<char const&>, std::tuple<char&&>>,
          decltype(tuple_zip(std::declval<tuple<int const, int* const, char const&, char&&>>()))>::value,
      "must not decay arguments"
  );
}

/// @test @c tuple_zip is @c noexcept if all element constructors are @c
/// noexcept and the standard library implementation has a noexcept tuple
/// constructor
TEST(TupleZip, AllConstructorsAreNoexcept) {  //
  static constexpr auto tuple_constructor_is_noexcept =
      std::is_nothrow_constructible<tuple<tuple<int>>, tuple<int>>::value;

  static_assert(
      tuple_constructor_is_noexcept == noexcept(tuple_zip(std::declval<tuple<int>>())),
      "noexcept must match"
  );
}

/// @test @c tuple_zip is @c noexcept(false) if any element constructor can throw
TEST(TupleZip, ConstructorCanThrow) {  //
  // NOLINTNEXTLINE(hicpp-special-member-functions)
  struct copy_can_throw {
    // NOLINTBEGIN(hicpp-use-equals-default)
    copy_can_throw(copy_can_throw&&) noexcept(true) {}
    copy_can_throw(copy_can_throw const&) noexcept(false) {}
    // NOLINTEND(hicpp-use-equals-default)
  };

  static_assert(  //
      !noexcept(tuple_zip(std::declval<tuple<copy_can_throw> const&>())),
      "must be noexcept(false)"
  );

  static constexpr auto tuple_constructor_is_noexcept =
      std::is_nothrow_constructible<tuple<tuple<int>>, tuple<int>>::value;

  static_assert(
      tuple_constructor_is_noexcept == noexcept(tuple_zip(std::declval<tuple<copy_can_throw>&&>())),
      "noexcept must match"
  );
}

}  // namespace
