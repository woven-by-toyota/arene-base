// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_manipulation/ebo_holder.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/stdlib_choice/plus.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {
struct tag {};
struct first_field_tag {};
struct second_field_tag {};

struct my_type {};
struct my_final_type final {};

template <typename T1, typename T2>
// NOLINTNEXTLINE(fuchsia-multiple-inheritance)
class compressed_pair
    : arene::base::ebo_holder<first_field_tag, T1>
    , arene::base::ebo_holder<second_field_tag, T2> {
 public:
  auto first() -> T1& { return this->get_value(first_field_tag{}); }
  auto second() -> T2& { return this->get_value(second_field_tag{}); }

  // other members
};

struct empty_class {};
struct other_empty_class {};

class derived : compressed_pair<empty_class, other_empty_class> {
  std::int32_t i_ = 0;

 public:
  auto get_i() -> std::int32_t& { return i_; }
};

/// @test `ebo_holder` allows the use of the empty base optimization to reduce the size of an object
TEST(EBO, CompressedPairUseEBO) {
  STATIC_ASSERT_EQ(sizeof(std::pair<empty_class, other_empty_class>), 2);
  STATIC_ASSERT_EQ(sizeof(compressed_pair<empty_class, other_empty_class>), 1);
  STATIC_ASSERT_EQ(sizeof(std::pair<std::int32_t, empty_class>), 8);
  STATIC_ASSERT_EQ(sizeof(compressed_pair<std::int32_t, empty_class>), 4);
  STATIC_ASSERT_EQ(sizeof(std::pair<empty_class, std::int32_t>), 8);
  STATIC_ASSERT_EQ(sizeof(compressed_pair<empty_class, std::int32_t>), 4);
  STATIC_ASSERT_EQ(sizeof(std::pair<std::int32_t, std::int32_t>), 8);
  STATIC_ASSERT_EQ(sizeof(compressed_pair<std::int32_t, std::int32_t>), 8);
  STATIC_ASSERT_EQ(sizeof(derived), 4);
}

/// @test 'ebo_holder' can be used with final types
CONSTEXPR_TEST(EBO, FinalType) { std::ignore = arene::base::ebo_holder<tag, my_final_type>{}; }

/// @test in-place constructor of 'ebo_holder' can be used with multiple arguments
TEST(EBO, InPlaceConstructor) {
  using ebo_holder = arene::base::ebo_holder<tag, std::pair<std::plus<>, std::plus<>>>;

  static_assert(std::is_constructible<ebo_holder, std::plus<>, std::plus<>>::value, "must be constructible");
}

template <bool IsNoexcept>
struct configurable_type {
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  configurable_type() noexcept(IsNoexcept) {}
  configurable_type(int, int) noexcept(IsNoexcept) {}
};

/// @test 'noexcept' of 'ebo_holder' constructors  matches 'noexcept' of underlying type
TEST(EBO, ConstructorNoexcept) {
  static_assert(
      std::is_nothrow_constructible<arene::base::ebo_holder<tag, configurable_type<true>>>::value,
      "must be nothrow constructible"
  );
  static_assert(
      std::is_nothrow_constructible<arene::base::ebo_holder<tag, configurable_type<true>>, int, int>::value,
      "must be nothrow constructible"
  );

  static_assert(
      !std::is_nothrow_constructible<arene::base::ebo_holder<tag, configurable_type<false>>>::value,
      "must not be nothrow constructible"
  );
  static_assert(
      !std::is_nothrow_constructible<arene::base::ebo_holder<tag, configurable_type<false>>, int, int>::value,
      "must not be nothrow constructible"
  );
}

template <class T>
struct EBO : testing::Test {};

using test_types = ::testing::Types<int, my_type, my_final_type>;

TYPED_TEST_SUITE(EBO, test_types, );

/// @test 'value()' returns a reference with the correct ref-qualification
TYPED_TEST(EBO, ValueRefQual) {
  using T = TypeParam;
  using ebo_holder = arene::base::ebo_holder<tag, T>;

  static_assert(
      std::is_same<T&, decltype(std::declval<ebo_holder&>().get_value(tag{}))>::value,
      "must be non-const lvalue reference"
  );
  static_assert(
      std::is_same<T const&, decltype(std::declval<ebo_holder const&>().get_value(tag{}))>::value,
      "must be const lvalue reference"
  );
  static_assert(
      std::is_same<T&&, decltype(std::declval<ebo_holder&&>().get_value(tag{}))>::value,
      "must be non-const rvalue reference"
  );
  static_assert(
      std::is_same<T const&&, decltype(std::declval<ebo_holder const&&>().get_value(tag{}))>::value,
      "must be const rvalue reference"
  );
}

}  // namespace
