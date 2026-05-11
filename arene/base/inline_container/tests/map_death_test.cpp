// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/inline_container/map.hpp"
#include "arene/base/optional/optional.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/strings/inline_string.hpp"

namespace {

using ::arene::base::inline_map;

constexpr std::size_t test_string_size{50};
using test_string = ::arene::base::inline_string<test_string_size>;

struct user_defined_key {
  std::uint32_t val;
  explicit user_defined_key(std::uint32_t value) noexcept
      : val(value) {}

  auto operator<(user_defined_key const& rhs) const noexcept -> bool { return val < rhs.val; }
};

struct move_only_value {
  arene::base::optional<std::uint32_t> val{};

  move_only_value() noexcept = default;
  explicit move_only_value(std::uint32_t value) noexcept
      : val(value) {}
  move_only_value(move_only_value const&) = delete;
  move_only_value(move_only_value&& other) noexcept
      : val(std::move(other.val)) {
    other.val.reset();  // explicitly reset so we can test which instances have been moved from
  }
  auto operator=(move_only_value const&) -> move_only_value& = delete;
  auto operator=(move_only_value&& other) noexcept -> move_only_value& {
    val = std::move(other.val);
    other.val.reset();  // explicitly reset so we can test which instances have been moved from
    return *this;
  }
  ~move_only_value() = default;
};

/// @test Constructing an `inline_map` with an initializer list of key/value pairs and a comparator where the number of
/// unique key values in the initializer list exceeds the capacity of the map terminates the program with a precondition
/// violation
TEST(InlineMapDeathTest, ConstructingFromOverlyLongInitializerListIsPreconditionViolation) {
  constexpr std::uint32_t capacity = 2;
  using imap = inline_map<std::uint32_t, test_string, capacity>;

  std::uint32_t const key1 = 42;
  std::uint32_t const key2 = 123;
  std::uint32_t const key3 = 5;

  test_string const value1 = "hello";
  test_string const value2 = "world";
  test_string const value3 = "goodbye";

  ASSERT_DEATH((imap{{key1, value1}, {key2, value2}, {key3, value3}}), "Precondition violation");
}

/// @test Invoking `emplace` when the size of the map is already equal to the capacity, and the key value constructed
/// from the supplied arguments is not already present in the map terminates the program with a precondition violation.
TEST(InlineMapDeathTest, EmplaceIfNoRoomIsPreconditionViolation) {
  constexpr std::uint32_t capacity = 2;
  using imap = inline_map<user_defined_key, move_only_value, capacity>;
  imap map{};

  std::uint32_t const key1 = 42;
  std::uint32_t const key2 = 99;
  std::uint32_t const key3 = 1000;
  std::uint32_t const value1 = 123;
  std::uint32_t const value2 = 456;
  std::uint32_t const value3 = 1;

  map.insert({user_defined_key{key1}, move_only_value{value1}});
  map.insert({user_defined_key{key2}, move_only_value{value2}});

  ASSERT_DEATH(map.emplace(user_defined_key{key3}, move_only_value{value3}), "Precondition violation");
}

/// @test Constructing an `inline_map` from an existing `inline_map` with a larger capacity and more elements than the
/// capacity of the destination map terminates the program with a precondition violation
TEST(InlineMapDeathTest, CopyConstructingSmallerFromLargerIfSizeTooBigIsPreconditionViolation) {
  constexpr auto capacity = 10U;
  using imap = inline_map<std::uint32_t, test_string, capacity>;
  constexpr auto capacity2 = 3U;
  using imap2 = inline_map<std::uint32_t, test_string, capacity2>;

  imap const map1{{42U, "hello"}, {99U, "goodbye"}, {103U, "foo"}, {2U, "bar"}, {12345U, "baz"}};
  ASSERT_DEATH(imap2{map1}, "Precondition violation");
}

/// @test Constructing an `inline_map` from an rvalue `inline_map` with a larger capacity and more elements than the
/// capacity of the destination map terminates the program with a precondition violation
TEST(InlineMapDeathTest, MoveConstructingSmallerFromLargerIfSizeTooBigIsPreconditionViolation) {
  constexpr auto capacity = 10U;
  using imap = inline_map<std::uint32_t, test_string, capacity>;
  constexpr auto capacity2 = 3U;
  using imap2 = inline_map<std::uint32_t, test_string, capacity2>;

  imap map1{{42U, "hello"}, {99U, "goodbye"}, {103U, "foo"}, {2U, "bar"}, {12345U, "baz"}};
  ASSERT_DEATH(imap2{std::move(map1)}, "Precondition violation");
}

/// @test Assigning to an `inline_map` with existing elements from a source map with size larger than the capacity of
/// the destination terminates the program with a precondition violation
TEST(InlineMapDeathTest, CopyAssignIfSourceSizeIsLargerThanCapacityIsPreconditionViolation) {
  constexpr auto capacity = 10U;
  using imap1 = inline_map<std::uint32_t, test_string, capacity>;
  constexpr auto capacity2 = 3U;
  using imap2 = inline_map<std::uint32_t, test_string, capacity2>;

  imap1 const map1{{42U, "hello"}, {99U, "goodbye"}, {103U, "foo"}, {2U, "bar"}, {12345U, "baz"}};
  imap2 map2{{1U, "other"}, {2U, "foo"}};
  ASSERT_DEATH(map2 = map1, "Precondition violation");
}

/// @test Assigning to an `inline_map` from an rvalue with a larger size than the capacity of the destination terminates
/// the program with a precondition violation
TEST(InlineMapDeathTest, MoveAssignIfSourceMapExceedsCapacityIsPreconditionViolation) {
  constexpr auto capacity = 10U;
  using imap1 = inline_map<std::uint32_t, move_only_value, capacity>;
  constexpr auto capacity2 = 2U;
  using imap2 = inline_map<std::uint32_t, move_only_value, capacity2>;

  std::uint32_t const key1 = 42U;
  std::int32_t const value1 = 11111;
  std::uint32_t const key2 = 99U;
  std::int32_t const value2 = 22222;
  std::uint32_t const key3 = 103U;
  std::int32_t const value3 = 33333;
  std::uint32_t const key4 = 2U;
  std::int32_t const value4 = 44444;
  std::uint32_t const key5 = 12345U;
  std::int32_t const value5 = 55555;
  imap1 map1;
  map1.emplace(key1, move_only_value(value1));
  map1.emplace(key2, move_only_value(value2));
  map1.emplace(key3, move_only_value(value3));
  map1.emplace(key4, move_only_value(value4));
  map1.emplace(key5, move_only_value(value5));
  imap2 map2;
  map2.emplace(102030U, move_only_value(66666));
  ASSERT_DEATH(map2 = std::move(map1), "Precondition violation");
}

}  // namespace
