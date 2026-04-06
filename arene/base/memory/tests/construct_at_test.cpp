// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/memory/construct_at.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/byte/byte.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::array;
using ::arene::base::byte;
using ::arene::base::construct_at;
using ::arene::base::substitution_succeeds;

template <typename... Args>
using use_construct_at = decltype(construct_at(std::declval<Args>()...));

/// @test `construct_at` is unable to construct a type using invalid arguments.
TEST(ConstructAtWithInvalidArguments, IsNotDefined) {
  STATIC_ASSERT_FALSE((substitution_succeeds<use_construct_at, arene::base::array<std::int32_t, 5>*, double>));
  STATIC_ASSERT_FALSE((substitution_succeeds<use_construct_at, std::int32_t*, char const*>));
}

struct user_type {
  constexpr user_type() = default;
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  constexpr user_type(std::int64_t fst, std::int64_t snd, std::int64_t trd)
      : first(fst),
        second(snd),
        third(trd) {}
  std::int64_t first = 0;
  std::int64_t second = 0;
  std::int64_t third = 0;
};

constexpr auto operator==(user_type const& lhs, user_type const& rhs) -> bool {
  return lhs.first == rhs.first && lhs.second == rhs.second && lhs.third == rhs.third;
}

template <typename T>
constexpr auto non_default_value(T) -> T {
  return std::numeric_limits<T>::max() / 2;
}

constexpr auto non_default_value(user_type) -> user_type { return user_type{1, 2, 3}; }

template <typename T>
using buffer_of_elements = array<byte, 10 * sizeof(T)>;

template <typename T>
struct ConstructAtWithValidArguments : ::testing::Test {
  buffer_of_elements<T> buffer{};
};

using constructable_types =
    ::testing::Types<std::int8_t, std::uint8_t, std::int16_t, std::int32_t, std::int64_t, float, double, user_type>;

TYPED_TEST_SUITE(ConstructAtWithValidArguments, constructable_types, );

// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast) Explicitly testing placement construction into buffers.
/// @test `construct_at` returns a pointer to the constructed object.
TYPED_TEST(ConstructAtWithValidArguments, ReturnsPointerToConstructedObject) {
  auto const first_location = this->buffer.begin();
  auto const* first_address = reinterpret_cast<TypeParam*>(&*first_location);
  EXPECT_EQ(construct_at(first_address, non_default_value(TypeParam{})), first_address);

  auto const second_location = first_location + sizeof(TypeParam);
  auto const* second_address = reinterpret_cast<TypeParam*>(&*second_location);
  EXPECT_EQ(construct_at(second_address, non_default_value(TypeParam{})), second_address);
}

/// @test `construct_at` constructs the object at the location with the given arguments.
TYPED_TEST(ConstructAtWithValidArguments, ConstructsTheObjectAtTheLocationWithTheGivenArguments) {
  auto const* address =
      reinterpret_cast<TypeParam*>(&*(this->buffer.begin() + static_cast<std::ptrdiff_t>(this->buffer.size() / 2)));
  auto const expected_value = non_default_value(TypeParam{});
  auto const* constructed_value_ptr = construct_at(address, expected_value);
  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Testing for unmodified exact copy");
  EXPECT_EQ(*constructed_value_ptr, expected_value);
  EXPECT_EQ(*constructed_value_ptr, *address);
  ARENE_IGNORE_END();
}

/// @test `construct_at` constructs the object at the location with multiple given arguments.
TEST(ConstructAtWithMultipleArguments, ConstructsTheObjectAtTheLocationWithTheGivenArguments) {
  buffer_of_elements<user_type> buffer{};
  auto const* address =
      reinterpret_cast<user_type*>(&*(buffer.begin() + static_cast<std::ptrdiff_t>(buffer.size() / 2)));
  std::int64_t const first = 1;
  std::int64_t const second = 2;
  std::int64_t const third = 3;
  auto const* constructed_value_ptr = construct_at(address, first, second, third);
  EXPECT_EQ(constructed_value_ptr->first, first);
  EXPECT_EQ(constructed_value_ptr->second, second);
  EXPECT_EQ(constructed_value_ptr->third, third);
}

class nothrow_move_throw_copy {
 public:
  nothrow_move_throw_copy() = default;
  nothrow_move_throw_copy(nothrow_move_throw_copy&&) noexcept = default;
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  nothrow_move_throw_copy(nothrow_move_throw_copy const&) noexcept(false) {}

  auto operator=(nothrow_move_throw_copy&&) -> nothrow_move_throw_copy = delete;
  auto operator=(nothrow_move_throw_copy const&) -> nothrow_move_throw_copy = delete;

  ~nothrow_move_throw_copy() = default;
};

/// @test `construct_at` is `noexcept` if the constructor is `noexcept`.
TEST(ConstructAtNoexcept, IsNoexceptIfCtorIsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(construct_at(std::declval<int*>())));
  STATIC_ASSERT_TRUE(
      noexcept(construct_at(std::declval<nothrow_move_throw_copy*>(), std::declval<nothrow_move_throw_copy&&>()))
  );
  STATIC_ASSERT_FALSE(
      noexcept(construct_at(std::declval<nothrow_move_throw_copy*>(), std::declval<nothrow_move_throw_copy const&>()))
  );
}

// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
}  // namespace
