// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

// IWYU pragma: no_include "arene/base/detail/raw_c_string.hpp"
#include "arene/base/inline_container/map.hpp"
#include "arene/base/optional/optional.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/make_tuple.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/stdlib_choice/stdexcept.hpp"

namespace {

using ::arene::base::inline_map;

constexpr std::uint32_t bad_val = 42;

struct some_error : std::runtime_error {
  using std::runtime_error::runtime_error;
};

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables) explicitly tracking global instances
struct counted {
  static std::uint32_t instances;

  counted() noexcept { ++instances; }
  counted(counted const&) noexcept { ++instances; }
  counted(counted&&) noexcept { ++instances; }
  ~counted() { --instances; }

  auto operator=(counted const&) -> counted& = default;
  auto operator=(counted&&) noexcept -> counted& { return *this; }
};
std::uint32_t counted::instances = 0;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

struct throwing_move : counted {
  std::uint32_t val;

  explicit throwing_move(std::uint32_t that_val)
      : val(that_val) {}
  throwing_move(throwing_move const& other) = default;
  // NOLINTNEXTLINE(hicpp-noexcept-move,bugprone-exception-escape) intentionally throwing move ctor
  throwing_move(throwing_move&& other)
      : val(other.val) {
    if (val == bad_val) {
      throw some_error{"throw on move"};
    }
  }

  auto operator=(throwing_move&& other) -> throwing_move& = delete;
  auto operator=(throwing_move const& other) -> throwing_move& = delete;
  ~throwing_move() = default;

  friend auto operator<(throwing_move const& lhs, throwing_move const& rhs) noexcept -> bool {
    return lhs.val < rhs.val;
  }
};

/// @test Invoking `try_construct` with an rvalue `inline_map` *copies* the keys, so a map with a key type with a
/// throwing move can still be moved, and the resulting values can still be retrieved. The source map is empty after
/// invoking `try_construct`.
TEST(InlineMap, MoveConstructViaFactoryWithThrowingMoveKeys) {
  using imap = inline_map<throwing_move, std::uint32_t, 3>;

  throwing_move const key1(20);
  throwing_move const key2(21);
  throwing_move const throwing_key(bad_val);
  imap refmap;
  refmap.emplace(key1, 0U);
  refmap.emplace(key2, 1U);
  refmap.emplace(throwing_key, 2U);
  imap map1{refmap};

  arene::base::optional<imap> opmap{};
  ASSERT_NO_THROW(opmap = imap::try_construct(std::move(map1)));
  ASSERT_TRUE(opmap.has_value());
  ASSERT_EQ(opmap->size(), 3);
  ASSERT_EQ(opmap->at(throwing_move(bad_val)), 2U);
  // NOLINTNEXTLINE(bugprone-use-after-move)
  ASSERT_TRUE(map1.empty());
}

/// @test Invoking `try_construct` with an rvalue `inline_map` *moves* the mapped values, so if the move-constructor of
/// any of the mapped values throws, then `try_construct` throws.
TEST(InlineMap, MoveConstructViaFactoryWithThrowingMoveValues) {
  using imap = inline_map<std::uint32_t, throwing_move, 3>;

  std::uint32_t const key1 = 20;
  std::uint32_t const key2 = 21;
  throwing_move const val1{key1};
  throwing_move const val2{key2};
  throwing_move const throwing_value{bad_val};

  imap refmap;
  refmap.emplace(key1, val1);
  refmap.emplace(key2, val2);
  refmap.emplace(bad_val, throwing_value);
  imap map1{refmap};

  arene::base::optional<imap> opmap{};
  ASSERT_THROW(opmap = imap::try_construct(std::move(map1)), some_error);
  ASSERT_FALSE(opmap.has_value());
  // NOLINTNEXTLINE(bugprone-use-after-move)
  ASSERT_TRUE(map1.empty());
}

/// @test If the mapped type of an `inline_map` has a throwing move constructor, and it throws during move-assignment of
/// one `inline_map` to another, then the destination map is left in a valid state, containing only those elements that
/// were successfully moved.
TEST(InlineMap, IfMovingElementThrowsThenMapIsValid) {
  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, throwing_move, capacity>;

  std::uint32_t const key1 = 1;
  std::uint32_t const key2 = 2;
  std::uint32_t const key3 = 3;
  std::uint32_t const key4 = 4;
  std::uint32_t const value1 = 100;
  std::uint32_t const value2 = 1;
  std::uint32_t const value4 = 99;

  imap map1{{key1, throwing_move{value1}}, {key2, throwing_move{value2}}, {key4, throwing_move{value4}}};
  map1.emplace(std::piecewise_construct, std::make_tuple(key3), std::make_tuple(bad_val));

  std::uint32_t const key5 = 10;
  std::uint32_t const key6 = 20;
  std::uint32_t const key7 = 30;
  std::uint32_t const key8 = 40;
  std::uint32_t const key9 = 50;
  std::uint32_t const key10 = 60;

  imap map2{
      {key5, throwing_move{value1}},
      {key6, throwing_move{value1}},
      {key7, throwing_move{value1}},
      {key8, throwing_move{value1}},
      {key9, throwing_move{value1}},
      {key10, throwing_move{value1}}
  };

  ASSERT_THROW(map2 = std::move(map1), some_error);

  ASSERT_EQ(map2.size(), 2);
  ASSERT_NE(map2.find(key1), map2.end());
  ASSERT_EQ(map2.find(key1)->second.val, value1);
  ASSERT_NE(map2.find(key2), map2.end());
  ASSERT_EQ(map2.find(key2)->second.val, value2);
  // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved,clang-analyzer-cplusplus.Move)
  ASSERT_EQ(counted::instances, map1.size() + map2.size());
}

/// @test If the mapped type of an `inline_map` has a throwing copy constructor, and it throws during assignment of one
/// `inline_map` to another, then the destination map is left in a valid state, containing only those elements that were
/// successfully copied.
TEST(InlineMap, IfCopyingElementThrowsThenMapIsValid) {
  struct throwing_copy : counted {
    std::uint32_t val;

    explicit throwing_copy(std::uint32_t that_val)
        : counted(),
          val(that_val) {}
    throwing_copy(throwing_copy const& other)
        : counted(),
          val(other.val) {
      if (val == bad_val) {
        throw some_error{"throw on copy"};
      }
    }

    throwing_copy(throwing_copy&& other) = default;
    auto operator=(throwing_copy const& other) -> throwing_copy& = default;
    auto operator=(throwing_copy&& other) -> throwing_copy& = default;
    ~throwing_copy() = default;
  };

  constexpr std::uint32_t capacity = 20;
  using imap = inline_map<std::uint32_t, throwing_copy, capacity>;

  std::uint32_t const key1 = 1;
  std::uint32_t const key2 = 2;
  std::uint32_t const key3 = 3;
  std::uint32_t const key4 = 4;
  std::uint32_t const value1 = 100;
  std::uint32_t const value2 = 1;
  std::uint32_t const value4 = 99;

  imap map1{{key1, throwing_copy{value1}}, {key2, throwing_copy{value2}}, {key4, throwing_copy{value4}}};
  map1.emplace(key3, throwing_copy{bad_val});

  std::uint32_t const key5 = 10;
  std::uint32_t const key6 = 20;
  std::uint32_t const key7 = 30;
  std::uint32_t const key8 = 40;
  std::uint32_t const key9 = 50;
  std::uint32_t const key10 = 60;

  imap map2{
      {key5, throwing_copy{value1}},
      {key6, throwing_copy{value1}},
      {key7, throwing_copy{value1}},
      {key8, throwing_copy{value1}},
      {key9, throwing_copy{value1}},
      {key10, throwing_copy{value1}}
  };

  ASSERT_THROW(map2 = map1, some_error);

  ASSERT_EQ(map2.size(), 2);
  ASSERT_NE(map2.find(key1), map2.end());
  ASSERT_EQ(map2.find(key1)->second.val, value1);
  ASSERT_NE(map2.find(key2), map2.end());
  ASSERT_EQ(map2.find(key2)->second.val, value2);
  ASSERT_EQ(counted::instances, map1.size() + map2.size());
}

}  // namespace
