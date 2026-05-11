// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/inline_container/deque.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/stdexcept.hpp"

namespace {

using ::arene::base::inline_deque;

constexpr int evil_value{123};

class special_members_throw {
 private:
  int value_;

 public:
  explicit special_members_throw(int val) noexcept
      : value_(val) {}

  special_members_throw(special_members_throw const& other) noexcept(false)
      : value_(other.value_) {
    if (value_ == evil_value) {
      throw std::logic_error("copy construct");
    }
  }
  special_members_throw(special_members_throw&& other) noexcept(false)
      : value_(other.value_) {
    if (value_ == evil_value) {
      throw std::logic_error("move construct");
    }
  }

  auto operator=(special_members_throw const& other) noexcept(false) -> special_members_throw& {
    value_ = other.value_;
    if (value_ == evil_value) {
      throw std::logic_error("copy assign");
    }
    return *this;
  }
  auto operator=(special_members_throw&& other) noexcept(false) -> special_members_throw& {
    value_ = other.value_;
    if (value_ == evil_value) {
      throw std::logic_error("move assign");
    }
    return *this;
  }

  ~special_members_throw() = default;

  auto set(int new_value) noexcept -> void { value_ = new_value; }

  friend auto operator==(special_members_throw const& left, special_members_throw const& right) noexcept -> bool {
    return left.value_ == right.value_;
  }
};

using throwing_deque = inline_deque<special_members_throw, 3U>;

/// @test If an exception occurs during `emplace_front`, it escapes into the calling code and the deque is unchanged
TEST(InlineDequeThrowTest, EmplaceFrontHasStrongExceptionGuarantee) {
  throwing_deque deque;

  special_members_throw const good{-1};
  deque.emplace_front(-1);
  ASSERT_EQ(deque.size(), 1U);
  ASSERT_EQ(deque.front(), good);

  // Emplacing the evil value doesn't actually invoke any special members so it doesn't throw
  special_members_throw const evil{evil_value};
  static_assert(noexcept(deque.emplace_front(evil_value)), "Emplace from value must be noexcept");
  deque.emplace_front(evil_value);
  ASSERT_EQ(deque.size(), 2U);
  ASSERT_EQ(deque.front(), evil);
  ASSERT_EQ(deque.back(), good);

  // Emplacing via copy constructor causes the exception to escape, and the existing deque doesn't change
  static_assert(!noexcept(deque.emplace_front(evil)), "Copy-emplace must not be noexcept");
  ASSERT_THROW(deque.emplace_front(evil), std::logic_error);
  ASSERT_EQ(deque.size(), 2U);
  ASSERT_EQ(deque.front(), evil);
  ASSERT_EQ(deque.back(), good);
}

/// @test If an exception occurs during `emplace_back`, it escapes into the calling code and the deque is unchanged
TEST(InlineDequeThrowTest, EmplaceBackHasStrongExceptionGuarantee) {
  throwing_deque deque;

  special_members_throw const good{-1};
  deque.emplace_back(-1);
  ASSERT_EQ(deque.size(), 1U);
  ASSERT_EQ(deque.back(), good);

  // Emplacing the evil value doesn't actually invoke any special members so it doesn't throw
  special_members_throw const evil{evil_value};
  static_assert(noexcept(deque.emplace_back(evil_value)), "Emplace from value must be noexcept");
  deque.emplace_back(evil_value);
  ASSERT_EQ(deque.size(), 2U);
  ASSERT_EQ(deque.back(), evil);
  ASSERT_EQ(deque.front(), good);

  // Emplacing via copy constructor causes the exception to escape, and the existing deque doesn't change
  static_assert(!noexcept(deque.emplace_back(evil)), "Copy-emplace must not be noexcept");
  ASSERT_THROW(deque.emplace_back(evil), std::logic_error);
  ASSERT_EQ(deque.size(), 2U);
  ASSERT_EQ(deque.back(), evil);
  ASSERT_EQ(deque.front(), good);
}

/// @test If an exception occurs during `push_front`, it escapes into the calling code and the deque is unchanged
TEST(InlineDequeThrowTest, PushFrontHasStrongExceptionGuarantee) {
  throwing_deque deque;

  special_members_throw const good{-1};
  deque.push_front(good);
  ASSERT_EQ(deque.size(), 1U);
  ASSERT_EQ(deque.front(), good);

  // Pushing this causes the exception to escape, and the existing deque doesn't change
  special_members_throw const evil{evil_value};
  static_assert(!noexcept(deque.push_front(evil)), "Must not be noexcept");
  ASSERT_THROW(deque.push_front(evil), std::logic_error);
  ASSERT_EQ(deque.size(), 1U);
  ASSERT_EQ(deque.front(), good);
}

/// @test If an exception occurs during `push_back`, it escapes into the calling code and the deque is unchanged
TEST(InlineDequeThrowTest, PushBackHasStrongExceptionGuarantee) {
  throwing_deque deque;

  special_members_throw const good{-1};
  deque.push_back(good);
  ASSERT_EQ(deque.size(), 1U);
  ASSERT_EQ(deque.back(), good);

  // Pushing this causes the exception to escape, and the existing deque doesn't change
  special_members_throw const evil{evil_value};
  static_assert(!noexcept(deque.push_back(evil)), "Must not be noexcept");
  ASSERT_THROW(deque.push_back(evil), std::logic_error);
  ASSERT_EQ(deque.size(), 1U);
  ASSERT_EQ(deque.back(), good);
}

/// @test If an exception occurs during copy construction, it escapes into the calling code and the deque is unchanged
TEST(InlineDequeThrowTest, CopyConstructorHasStrongExceptionGuarantee) {
  throwing_deque deque;

  special_members_throw const good{-1};
  deque.emplace_front(good);
  deque.emplace_front(good);
  ASSERT_EQ(deque.size(), 2U);
  deque.back().set(evil_value);

  static_assert(!noexcept(throwing_deque{deque}), "Must not be noexcept");
  ASSERT_THROW(throwing_deque{deque}, std::logic_error);
  ASSERT_EQ(deque.size(), 2U);
  ASSERT_EQ(deque.back(), special_members_throw{evil_value});
}

/// @test If an exception occurs during copy assignment, it escapes into the calling code and the deque is unchanged
TEST(InlineDequeThrowTest, CopyAssignmentHasStrongExceptionGuarantee) {
  throwing_deque source;

  special_members_throw const good{-1};
  source.emplace_front(good);
  source.emplace_front(good);
  ASSERT_EQ(source.size(), 2U);
  ASSERT_EQ(source.back(), good);

  throwing_deque dest{source};
  source.back().set(evil_value);

  static_assert(!noexcept(dest = source), "Must not be noexcept");
  ASSERT_THROW(dest = source, std::logic_error);
  ASSERT_EQ(source.size(), 2U);
  ASSERT_EQ(source.back(), special_members_throw{evil_value});
}

/// @test If an exception occurs during move construction, it escapes into the calling code and the deque is unchanged
TEST(InlineDequeThrowTest, MoveConstructorHasStrongExceptionGuarantee) {
  throwing_deque deque;

  special_members_throw const good{-1};
  deque.emplace_front(good);
  deque.emplace_front(good);
  ASSERT_EQ(deque.size(), 2U);
  deque.back().set(evil_value);

  static_assert(!noexcept(throwing_deque{std::move(deque)}), "Must not be noexcept");
  ASSERT_THROW(throwing_deque{std::move(deque)}, std::logic_error);
  // NOLINTBEGIN(bugprone-use-after-move)
  ASSERT_EQ(deque.size(), 2U);
  ASSERT_EQ(deque.back(), special_members_throw{evil_value});
  // NOLINTEND(bugprone-use-after-move)
}

/// @test If an exception occurs during move assignment, it escapes into the calling code and the deque is unchanged
TEST(InlineDequeThrowTest, MoveAssignmentHasStrongExceptionGuarantee) {
  throwing_deque source;

  special_members_throw const good{-1};
  source.emplace_front(good);
  source.emplace_front(good);
  ASSERT_EQ(source.size(), 2U);
  ASSERT_EQ(source.back(), good);

  throwing_deque dest{source};
  source.back().set(evil_value);

  static_assert(!noexcept(dest = std::move(source)), "Must not be noexcept");
  ASSERT_THROW(dest = std::move(source), std::logic_error);
  // NOLINTBEGIN(bugprone-use-after-move)
  ASSERT_EQ(source.size(), 2U);
  ASSERT_EQ(source.back(), special_members_throw{evil_value});
  // NOLINTEND(bugprone-use-after-move)
}

}  // namespace
