// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/cstdint"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"

namespace {

/// @test Exchange replaces the target with the new value
CONSTEXPR_TEST(Exchange, ExchangeReplacesTheSpecifiedValueWithTheNewValue) {
  constexpr std::uint32_t new_value = 42;
  std::uint32_t target{99};
  std::exchange(target, new_value);
  ASSERT_EQ(target, new_value);
}

/// @test Exchange returns the old value of the target
CONSTEXPR_TEST(Exchange, ExchangeReturnsTheOldValue) {
  constexpr std::uint32_t new_value = 42;
  constexpr std::uint32_t old_value = 99;
  std::uint32_t target{old_value};
  auto const result = std::exchange(target, new_value);
  ASSERT_EQ(result, old_value);
  testing::StaticAssertTypeEq<decltype(std::exchange(target, new_value)), std::uint32_t>();
}

class move_source {
  std::uint32_t value_;

 public:
  constexpr explicit move_source(std::uint32_t value) noexcept
      : value_(value) {}

  constexpr auto get() const noexcept -> std::uint32_t { return value_; }

  move_source(move_source&& other) = delete;
  move_source(move_source const& other) = delete;
  auto operator=(move_source&&) = delete;
  auto operator=(move_source const&) = delete;

  ~move_source() = default;

  constexpr void clear() { value_ = 0; }
};

class move_target {
  std::uint32_t value_;
  std::uint32_t move_count_;

 public:
  constexpr explicit move_target(std::uint32_t value) noexcept
      : value_(value),
        move_count_(0) {}

  constexpr move_target(move_target&& other) noexcept
      : value_(other.value_),
        move_count_(other.move_count_ + 1U) {}

  constexpr move_target(move_target const&) = delete;

  constexpr auto get() const noexcept -> std::uint32_t { return value_; }
  constexpr auto moves() const noexcept -> std::uint32_t { return move_count_; }

  auto operator=(move_target&&) = delete;
  auto operator=(move_target const&) = delete;

  constexpr auto operator=(move_source&& other) noexcept -> move_target& {
    value_ = other.get();
    move_count_ = 1;
    other.clear();
    return *this;
  }

  constexpr auto operator=(move_source const& other) noexcept -> move_target& {
    value_ = other.get();
    move_count_ = 0;
    return *this;
  }

  ~move_target() = default;
};

/// @test Exchange *moves* the old value and *moves* the new value if the new value is a temporary
CONSTEXPR_TEST(Exchange, ExchangeMovesOldAndMoveAssigns) {
  constexpr std::uint32_t new_value = 42;
  constexpr std::uint32_t old_value = 99;
  move_target target{old_value};
  auto const result = std::exchange(target, move_source{new_value});
  ASSERT_EQ(result.get(), old_value);
  // Move for return may or may not be elided.
  ASSERT_TRUE((result.moves() == 1) || (result.moves() == 2));
  ASSERT_EQ(target.get(), new_value);
  ASSERT_EQ(target.moves(), 1);
  testing::StaticAssertTypeEq<decltype(std::exchange(target, move_source{new_value})), move_target>();
}

/// @test Exchange *moves* the old value and *copies* the new value if the new value is an lvalue
CONSTEXPR_TEST(Exchange, ExchangeMovesOldAndAssignsLvalue) {
  constexpr std::uint32_t new_value = 42;
  constexpr std::uint32_t old_value = 99;
  move_target target{old_value};
  // NOLINTNEXTLINE(misc-const-correctness)
  move_source source{new_value};
  auto const result = std::exchange(target, source);
  ASSERT_EQ(result.get(), old_value);
  // Move for return may or may not be elided.
  ASSERT_TRUE((result.moves() == 1) || (result.moves() == 2));
  ASSERT_EQ(target.get(), new_value);
  ASSERT_EQ(target.moves(), 0);
  ASSERT_EQ(source.get(), new_value);
  testing::StaticAssertTypeEq<decltype(std::exchange(target, source)), move_target>();
}

/// @test Exchange *moves* the old value and the new value if the new value is an rvalue
CONSTEXPR_TEST(Exchange, ExchangeMovesOldAndMoveAssignsMovedLvalue) {
  constexpr std::uint32_t new_value = 42;
  constexpr std::uint32_t old_value = 99;
  move_target target{old_value};
  move_source source{new_value};
  auto const result = std::exchange(target, std::move(source));
  ASSERT_EQ(result.get(), old_value);
  // Move for return may or may not be elided.
  ASSERT_TRUE((result.moves() == 1) || (result.moves() == 2));
  ASSERT_EQ(target.get(), new_value);
  ASSERT_EQ(target.moves(), 1);
  // NOLINTNEXTLINE(bugprone-use-after-move)
  ASSERT_EQ(source.get(), 0);
  testing::StaticAssertTypeEq<decltype(std::exchange(target, std::move(source))), move_target>();
}

/// @test Exchange can throw if the move construction of the old value can throw
TEST(Exchange, ExchangeIsNotNoexceptIfVariableIsNotNothrowMoveConstructible) {
  class throwing_move {
   public:
    throwing_move() noexcept = default;
    throwing_move(throwing_move&&) noexcept(false) {}
    throwing_move(throwing_move const&) noexcept = default;

    auto operator=(throwing_move&&) noexcept -> throwing_move& = default;
    auto operator=(throwing_move const&) noexcept -> throwing_move& = default;

    ~throwing_move() = default;
  };

  static_assert(
      !noexcept(std::exchange(std::declval<throwing_move&>(), std::declval<throwing_move const&>())),
      "Must not be noexcept"
  );
}

/// @test Exchange can throw if the assignment of the new value can throw
TEST(Exchange, ExchangeIsNotNoexceptIfAssignmentIsNotNoThrow) {
  class source {};

  class throwing_assign {
   public:
    throwing_assign() noexcept = default;
    throwing_assign(throwing_assign&&) noexcept = default;
    throwing_assign(throwing_assign const&) noexcept = default;

    auto operator=(throwing_assign&&) noexcept -> throwing_assign& = default;
    auto operator=(throwing_assign const&) noexcept -> throwing_assign& = default;

    auto operator=(source) noexcept(false) -> throwing_assign& { return *this; }

    ~throwing_assign() = default;
  };

  static_assert(
      !noexcept(std::exchange(std::declval<throwing_assign&>(), std::declval<source>())),
      "Must not be noexcept"
  );
}

/// @test Exchange must not throw if neither the move construction of the old value nor the assignment of the new value
/// throw
TEST(Exchange, ExchangeIsNoexceptIfMoveAndAssignmentAreNoThrow) {
  class source {};

  class nothrow_assign {
   public:
    nothrow_assign() noexcept = default;
    nothrow_assign(nothrow_assign&&) noexcept = default;
    nothrow_assign(nothrow_assign const&) noexcept = default;

    auto operator=(nothrow_assign&&) noexcept -> nothrow_assign& = default;
    auto operator=(nothrow_assign const&) noexcept -> nothrow_assign& = default;

    auto operator=(source) & noexcept -> nothrow_assign& { return *this; }
    auto operator=(source) && noexcept(false) -> nothrow_assign& { return *this; }

    ~nothrow_assign() = default;
  };

  static_assert(noexcept(std::exchange(std::declval<nothrow_assign&>(), std::declval<source>())), "Must be noexcept");
}

}  // namespace
