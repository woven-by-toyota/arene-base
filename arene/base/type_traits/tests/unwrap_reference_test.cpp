// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/unwrap_reference.hpp"

#include <cstdint>
#include <functional>
#include <type_traits>

#include <gtest/gtest.h>

namespace {

template <class ToUnwrap, class Expected>
constexpr auto check_unwrap() noexcept -> void {
  testing::StaticAssertTypeEq<arene::base::unwrap_reference_t<ToUnwrap>, Expected>();
  testing::StaticAssertTypeEq<typename arene::base::unwrap_reference<ToUnwrap>::type, Expected>();
}

template <class ToUnwrap, class Expected>
constexpr auto check_unwrap_decay() noexcept -> void {
  testing::StaticAssertTypeEq<arene::base::unwrap_ref_decay_t<ToUnwrap>, Expected>();
  testing::StaticAssertTypeEq<typename arene::base::unwrap_ref_decay<ToUnwrap>::type, Expected>();
}

template <class T>
class UnwrapReference : public ::testing::Test {};

struct user_defined {};

// clang-format off
using inner_types = ::testing::Types<
  std::int32_t,
  float,
  char,
  std::uint64_t const,
  void*,
  void* const,
  const void*,
  int[],  // NOLINT(hicpp-avoid-c-arrays) We're deliberately testing the decay of an array here
  user_defined
>;
// clang-format on

TYPED_TEST_SUITE(UnwrapReference, inner_types, );

/// @test `unwrap_reference_t` gives the unwrapped type with no decaying or similar
TYPED_TEST(UnwrapReference, Unwrap) {
  check_unwrap<TypeParam, TypeParam>();
  check_unwrap<std::reference_wrapper<TypeParam>, TypeParam&>();
  check_unwrap<std::reference_wrapper<TypeParam const>, TypeParam const&>();
  check_unwrap<std::reference_wrapper<TypeParam volatile>, TypeParam volatile&>();
  check_unwrap<std::reference_wrapper<TypeParam const volatile>, TypeParam const volatile&>();
}

/// @test `unwrap_ref_decay_t` on a wrapper gives the unwrapped type with no decaying of the contents
TYPED_TEST(UnwrapReference, UnwrapAndDecay) {
  check_unwrap_decay<std::reference_wrapper<TypeParam>, TypeParam&>();
  check_unwrap_decay<std::reference_wrapper<TypeParam const>, TypeParam const&>();
  check_unwrap_decay<std::reference_wrapper<TypeParam volatile>, TypeParam volatile&>();
  check_unwrap_decay<std::reference_wrapper<TypeParam const volatile>, TypeParam const volatile&>();
}

/// @test `unwrap_ref_decay_t` applies `std::decay` before unwrapping
TYPED_TEST(UnwrapReference, DecayBeforeUnwrap) {
  check_unwrap_decay<TypeParam, std::decay_t<TypeParam>>();
  check_unwrap_decay<std::reference_wrapper<TypeParam>, TypeParam&>();
  check_unwrap_decay<std::reference_wrapper<TypeParam> const, TypeParam&>();
  check_unwrap_decay<std::reference_wrapper<TypeParam> volatile, TypeParam&>();
  check_unwrap_decay<std::reference_wrapper<TypeParam> const volatile, TypeParam&>();
}

/// @test `unwrap_ref_decay_t` on a type which can not be unwrapped just decays it
TYPED_TEST(UnwrapReference, JustDecayWhenTypeCanNotBeUnwrapped) {
  using function = std::decay_t<TypeParam>(std::decay_t<TypeParam>);

  // NOLINTNEXTLINE(hicpp-avoid-c-arrays) We're deliberately testing the decay of an array here
  check_unwrap_decay<std::reference_wrapper<TypeParam>[], std::reference_wrapper<TypeParam>*>();
  check_unwrap_decay<function, function*>();
  check_unwrap_decay<function&, function*>();
}

}  // namespace
