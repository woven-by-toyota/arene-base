// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/detail/exceptions.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/stdexcept.hpp"

namespace {

static_assert(arene::base::detail::are_exceptions_enabled_v, "should be enabled");

struct user_type {};

/// @test `at` returns a reference to the element at the specified index. This is a `const` reference if `at` is invoked
/// via a `const` reference to the array. If the index is greater than or equal to the size of the array, then
/// `std::out_of_range` is thrown.
TEST(Array, At) {
  struct custom_type {
    std::int64_t i = 0;
  };
  constexpr auto size = 123;
  using arr_t = arene::base::array<custom_type, size>;

  arr_t array{};

  ::testing::StaticAssertTypeEq<decltype(array.at(0)), custom_type&>();

#if !ARENE_IS_ON(ARENE_COMPILER_GCC) || __GNUC__ > 8
  // This doesn't work on gcc <=8 due to a compiler bug.
  static_assert(!noexcept(array.at(0)), "Not noexcept");
#endif

  // NOLINTNEXTLINE(readability-container-data-pointer)
  ASSERT_EQ(&array.at(0), &array[0]);
  ASSERT_EQ(&array.at(size - 1), &array[size - 1]);
  ASSERT_THROW(array.at(size), std::out_of_range);
  ASSERT_THROW(array.at(size + 1), std::out_of_range);
  ASSERT_THROW(array.at(static_cast<std::size_t>(size * 2)), std::out_of_range);

  auto const& const_array = array;

  ::testing::StaticAssertTypeEq<decltype(const_array.at(0)), custom_type const&>();
  static_assert(!noexcept(const_array.at(0)), "Not noexcept");

  // NOLINTNEXTLINE(readability-container-data-pointer)
  ASSERT_EQ(&const_array.at(0), &array[0]);
  ASSERT_EQ(&const_array.at(size - 1), &array[size - 1]);
  ASSERT_THROW(const_array.at(size), std::out_of_range);
  ASSERT_THROW(const_array.at(size + 1), std::out_of_range);
  ASSERT_THROW(const_array.at(static_cast<std::size_t>(size * 2)), std::out_of_range);

  constexpr auto val1 = 0x12345678;
  constexpr arr_t array2{{{val1}}};

  // NOLINTNEXTLINE(readability-container-data-pointer)
  static_assert(&array2.at(0) == &array2[0], "Usable in constexpr");
  static_assert(array2.at(0).i == val1, "Usable in constexpr");
}

using zero_size_element_types = ::testing::Types<int, user_type>;

template <typename ElementType>
class ZeroSizedArrayTests : public ::testing::Test {};

TYPED_TEST_SUITE(ZeroSizedArrayTests, zero_size_element_types, );

/// @test Zero-sized `array.at()` always throws `std::out_of_range`.
/// @tparam TypeParam The element type of the array
TYPED_TEST(ZeroSizedArrayTests, ZeroSizedArrayAt) {
  using arr_t = arene::base::array<TypeParam, 0>;
  arr_t array{};
  auto const& const_array = array;

  ASSERT_THROW(array.at(0), std::out_of_range);
  ASSERT_THROW(const_array.at(0), std::out_of_range);
}

}  // namespace
