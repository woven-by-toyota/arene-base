// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/array/to_array.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/stdlib_choice/begin.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/end.hpp"
#include "arene/base/stdlib_choice/exchange.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "testlibs/utilities/configurable_value.hpp"

// NOLINTBEGIN(hicpp-avoid-c-arrays,cppcoreguidelines-pro-bounds-constant-array-index)

namespace {

using ::arene::base::to_array;
using ::arene::base::to_array_of;

/// @test `to_array` can be used to construct an `array` from a built-in array, deducing the size and element type from
/// the supplied built-in array. The elements of the constructed `array` are copy-constructed from the elements of the
/// source array.
TEST(ToArray, FromLvalueCarray) {
  static constexpr std::int32_t source[] = {1, 22, 333, 4444, 55555};
  static constexpr auto size = std::end(source) - std::begin(source);

  std::int32_t source2[] = {1, 22, 333, 4444, 55555};
  std::int32_t const(&source3)[size] = source2;

  constexpr auto dest = to_array(source);
  auto dest2 = to_array(source2);
  auto runtime_const_array = to_array(source3);

  ::testing::StaticAssertTypeEq<decltype(dest), arene::base::array<std::int32_t, size> const>();
  ::testing::StaticAssertTypeEq<decltype(dest2), arene::base::array<std::int32_t, size>>();

  EXPECT_THAT(dest, ::testing::ElementsAreArray(source));
  EXPECT_THAT(dest2, ::testing::ElementsAreArray(source2));
  EXPECT_THAT(runtime_const_array, ::testing::ElementsAreArray(source3));
}

/// @test `to_array` can be used to construct an `array` from an rvalue built-in array, deducing the size and element
/// type from the supplied built-in array. The elements of the constructed `array` are move-constructed from the
/// corresponding elements from the source array.
TEST(ToArray, ToArrayForRValues) {
  constexpr auto val = -123;

  struct custom_type {
    int value = val;
    custom_type() noexcept = default;
    custom_type(custom_type const&) noexcept = default;
    auto operator=(custom_type const&) noexcept -> custom_type& = default;
    custom_type(custom_type&& other) noexcept
        : value(std::exchange(other.value, 0)) {}
    auto operator=(custom_type&&) noexcept -> custom_type& = delete;
    ~custom_type() = default;
  };

  constexpr auto size = 5;
  custom_type source[size]{};

  auto dest = to_array(std::move(source));

  ::testing::StaticAssertTypeEq<decltype(dest), arene::base::array<custom_type, size>>();

  for (unsigned i = 0; i < size; ++i) {
    ASSERT_EQ(dest[i].value, val);
    // NOLINTNEXTLINE(bugprone-use-after-move) the type has a specific move ctor that sets value to 0. This proves we actually moved.
    ASSERT_EQ(source[i].value, 0);
  }
}

/// @test `to_array` can be used to construct an `array` from a braced initializer list, deducing the element type from
/// the list's elements and the size from the number of elements.
CONSTEXPR_TEST(ToArray, CanBeUsedWithBracedInitializerLists) {
  ::testing::StaticAssertTypeEq<decltype(to_array({0, 1, 2})), arene::base::array<int, 3>>();
  CONSTEXPR_ASSERT_EQ(to_array({0, 1, 2}), (arene::base::array<int, 3>{0, 1, 2}));
}

/// @test `to_array_of` can be used to construct an `array` from a braced initializer list, converting values to the
/// desired element type and the size from the number of elements.
CONSTEXPR_TEST(ToArrayOf, CanBeUsedWithBracedInitializerLists) {
  ::testing::StaticAssertTypeEq<decltype(to_array_of<std::size_t>({0, 1, 2})), arene::base::array<std::size_t, 3>>();
  CONSTEXPR_ASSERT_EQ(to_array_of<std::size_t>({0, 1, 2}), (arene::base::array<std::size_t, 3>{0, 1, 2}));

  ::testing::StaticAssertTypeEq<decltype(to_array_of<char>({0, 1, 2})), arene::base::array<char, 3>>();
  CONSTEXPR_ASSERT_EQ(to_array_of<char>({0, 1, 2}), (arene::base::array<char, 3>{0, 1, 2}));
}

/// @test `noexcept` specification of `to_array` and `to_array_of`
TEST(ToArray, Noexcept) {
  using arene::base::is_invocable_v;
  using arene::base::is_nothrow_invocable_v;
  using testing::configurable_value;
  using testing::throws_on;

  EXPECT_TRUE((is_invocable_v<decltype(to_array), configurable_value<int>(&)[3]>));
  EXPECT_TRUE((is_invocable_v<decltype(to_array), configurable_value<int, throws_on::move_construct>(&)[3]>));
  EXPECT_TRUE((is_invocable_v<decltype(to_array), configurable_value<int, throws_on::copy_construct>(&)[3]>));

  EXPECT_TRUE((is_nothrow_invocable_v<decltype(to_array), configurable_value<int>(&)[3]>));
  EXPECT_TRUE((is_nothrow_invocable_v<decltype(to_array), configurable_value<int, throws_on::move_construct>(&)[3]>));
  EXPECT_FALSE((is_nothrow_invocable_v<decltype(to_array), configurable_value<int, throws_on::copy_construct>(&)[3]>));

  EXPECT_TRUE((is_invocable_v<decltype(to_array), configurable_value<int>(&&)[3]>));
  EXPECT_TRUE((is_invocable_v<decltype(to_array), configurable_value<int, throws_on::move_construct>(&&)[3]>));
  EXPECT_TRUE((is_invocable_v<decltype(to_array), configurable_value<int, throws_on::copy_construct>(&&)[3]>));

  EXPECT_TRUE((is_nothrow_invocable_v<decltype(to_array), configurable_value<int>(&&)[3]>));
  EXPECT_FALSE((is_nothrow_invocable_v<decltype(to_array), configurable_value<int, throws_on::move_construct>(&&)[3]>));
  EXPECT_TRUE((is_nothrow_invocable_v<decltype(to_array), configurable_value<int, throws_on::copy_construct>(&&)[3]>));

  EXPECT_TRUE((is_invocable_v<decltype(to_array_of<configurable_value<int>>), int[3]>));
  EXPECT_TRUE((is_invocable_v<decltype(to_array_of<configurable_value<int, throws_on::value_construct>>), int[3]>));

  EXPECT_TRUE((is_nothrow_invocable_v<decltype(to_array_of<configurable_value<int>>), int[3]>));
  EXPECT_FALSE(
      (is_nothrow_invocable_v<decltype(to_array_of<configurable_value<int, throws_on::value_construct>>), int[3]>)
  );
}

/// @test invocability of `to_array` and `to_array_of`
TEST(ToArray, Invocability) {
  using arene::base::is_invocable_v;
  using testing::configurable_value;
  using testing::disable;
  using testing::throws_on;

  EXPECT_TRUE((is_invocable_v<decltype(to_array), configurable_value<int>(&)[3]>));
  EXPECT_FALSE(
      (is_invocable_v<decltype(to_array), configurable_value<int, throws_on::nothing, disable::copy_construct>(&)[3]>)
  );

  EXPECT_TRUE((is_invocable_v<decltype(to_array), configurable_value<int>(&&)[3]>));
  EXPECT_FALSE((is_invocable_v<
                decltype(to_array),
                configurable_value<int, throws_on::nothing, disable::move_construct | disable::copy_construct>(&&)[3]>)
  );
  EXPECT_TRUE(
      (is_invocable_v<decltype(to_array), configurable_value<int, throws_on::nothing, disable::copy_construct>(&&)[3]>)
  );

  EXPECT_TRUE((is_invocable_v<decltype(to_array_of<configurable_value<int, throws_on::nothing>>), int(&)[3]>));
  EXPECT_FALSE((is_invocable_v<
                decltype(to_array_of<configurable_value<int, throws_on::nothing, disable::value_construct>>),
                int(&)[3]>));
}

}  // namespace

// NOLINTEND(hicpp-avoid-c-arrays,cppcoreguidelines-pro-bounds-constant-array-index)
