// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/give_cv_to.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/is_same.hpp"

namespace {

template <class T>
struct GiveCvTo : testing::Test {};

struct apply_give_cv_to {
  template <class From, class To>
  using type = typename arene::base::give_cv_to<From, To>::type;
};

struct apply_give_cv_to_t {
  template <class From, class To>
  using type = arene::base::give_cv_to_t<From, To>;
};

using test_types = ::testing::Types<apply_give_cv_to, apply_give_cv_to_t>;

TYPED_TEST_SUITE(GiveCvTo, test_types, );

/// @test 'give_cv_to<T, U>::type' is the same as 'T' if 'U' is 'remove_cvref_t<T>'
TYPED_TEST(GiveCvTo, IntDest) {
  static_assert(
      std::is_same<
          typename TypeParam::template type<int, int>,  //
          int>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const, int>,  //
          int const>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int volatile, int>,  //
          int volatile>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const volatile, int>,  //
          int const volatile>::value,
      "must be the same"
  );

  static_assert(
      std::is_same<
          typename TypeParam::template type<int&, int>,  //
          int>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const&, int>,  //
          int>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int volatile&, int>,  //
          int>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const volatile&, int>,  //
          int>::value,
      "must be the same"
  );

  static_assert(
      std::is_same<
          typename TypeParam::template type<int&&, int>,  //
          int>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const&&, int>,  //
          int>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int volatile&&, int>,  //
          int>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const volatile&&, int>,  //
          int>::value,
      "must be the same"
  );
}

/// @test 'give_cv_to<T, U>::type' copies cv qual to the destination type if the
/// destination type doesn't have cv-ref qualifiers
TYPED_TEST(GiveCvTo, FloatDest) {
  static_assert(
      std::is_same<
          typename TypeParam::template type<int, float>,  //
          float>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const, float>,  //
          float const>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int volatile, float>,  //
          float volatile>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const volatile, float>,  //
          float const volatile>::value,
      "must be the same"
  );

  static_assert(
      std::is_same<
          typename TypeParam::template type<int&, float>,  //
          float>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const&, float>,  //
          float>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int volatile&, float>,  //
          float>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const volatile&, float>,  //
          float>::value,
      "must be the same"
  );

  static_assert(
      std::is_same<
          typename TypeParam::template type<int&&, float>,  //
          float>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const&&, float>,  //
          float>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int volatile&&, float>,  //
          float>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const volatile&&, float>,  //
          float>::value,
      "must be the same"
  );
}

/// @test 'give_cv_to<T, U>' does not strip the const qualifier from the destination type
TYPED_TEST(GiveCvTo, DestWithConstQual) {
  static_assert(
      std::is_same<
          typename TypeParam::template type<int, float const>,  //
          float const>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const, float const>,  //
          float const>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int volatile, float const>,  //
          float const volatile>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const volatile, float const>,  //
          float const volatile>::value,
      "must be the same"
  );

  static_assert(
      std::is_same<
          typename TypeParam::template type<int&, float const>,  //
          float const>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const&, float const>,  //
          float const>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int volatile&, float const>,  //
          float const>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const volatile&, float const>,  //
          float const>::value,
      "must be the same"
  );

  static_assert(
      std::is_same<
          typename TypeParam::template type<int&&, float const>,  //
          float const>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const&&, float const>,  //
          float const>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int volatile&&, float const>,  //
          float const>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const volatile&&, float const>,  //
          float const>::value,
      "must be the same"
  );
}

/// @test 'give_cv_to<T, U>' does not strip the volatile qualifier from the destination type
TYPED_TEST(GiveCvTo, DestWithVolatileQual) {
  static_assert(
      std::is_same<
          typename TypeParam::template type<int, float volatile>,  //
          float volatile>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const, float volatile>,  //
          float const volatile>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int volatile, float volatile>,  //
          float volatile>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const volatile, float volatile>,  //
          float const volatile>::value,
      "must be the same"
  );

  static_assert(
      std::is_same<
          typename TypeParam::template type<int&, float volatile>,  //
          float volatile>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const&, float volatile>,  //
          float volatile>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int volatile&, float volatile>,  //
          float volatile>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const volatile&, float volatile>,  //
          float volatile>::value,
      "must be the same"
  );

  static_assert(
      std::is_same<
          typename TypeParam::template type<int&&, float volatile>,  //
          float volatile>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const&&, float volatile>,  //
          float volatile>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int volatile&&, float volatile>,  //
          float volatile>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const volatile&&, float volatile>,  //
          float volatile>::value,
      "must be the same"
  );
}

/// @test 'give_cv_to<T, U>' does not strip the lvalue reference qualifier from the destination type
TYPED_TEST(GiveCvTo, DestWithLvalueRefQual) {
  static_assert(
      std::is_same<
          typename TypeParam::template type<int, float&>,  //
          float&>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const, float&>,  //
          float&>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int volatile, float&>,  //
          float&>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const volatile, float&>,  //
          float&>::value,
      "must be the same"
  );

  static_assert(
      std::is_same<
          typename TypeParam::template type<int&, float&>,  //
          float&>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const&, float&>,  //
          float&>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int volatile&, float&>,  //
          float&>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const volatile&, float&>,  //
          float&>::value,
      "must be the same"
  );

  static_assert(
      std::is_same<
          typename TypeParam::template type<int&&, float&>,  //
          float&>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const&&, float&>,  //
          float&>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int volatile&&, float&>,  //
          float&>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const volatile&&, float&>,  //
          float&>::value,
      "must be the same"
  );
}

/// @test 'give_cv_to<T, U>' does not strip the rvalue reference qualifier from the destination type
TYPED_TEST(GiveCvTo, DestWithRvalueRefQual) {
  static_assert(
      std::is_same<
          typename TypeParam::template type<int, float&&>,  //
          float&&>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const, float&&>,  //
          float&&>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int volatile, float&&>,  //
          float&&>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const volatile, float&&>,  //
          float&&>::value,
      "must be the same"
  );

  static_assert(
      std::is_same<
          typename TypeParam::template type<int&, float&&>,  //
          float&&>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const&, float&&>,  //
          float&&>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int volatile&, float&&>,  //
          float&&>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const volatile&, float&&>,  //
          float&&>::value,
      "must be the same"
  );

  static_assert(
      std::is_same<
          typename TypeParam::template type<int&&, float&&>,  //
          float&&>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const&&, float&&>,  //
          float&&>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int volatile&&, float&&>,  //
          float&&>::value,
      "must be the same"
  );
  static_assert(
      std::is_same<
          typename TypeParam::template type<int const volatile&&, float&&>,  //
          float&&>::value,
      "must be the same"
  );
}

}  // namespace
