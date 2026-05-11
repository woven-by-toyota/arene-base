// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/result.hpp"
#include "arene/base/result/detail/in_place_error.hpp"
#include "arene/base/result/detail/in_place_value.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/stdexcept.hpp"

namespace {

using arene::base::in_place_error;
using arene::base::in_place_value;
using arene::base::result;

/// @test @c arene::base::result<T,E> 's copy constructor propagates any exception thrown by the copy constructor of the
/// currently populated channel.
TEST(ResultConstructor, ExceptionOnCopyConstructionPropagates) {
  struct test_exception : public std::exception {};
  struct throws_on_copy {
    throws_on_copy() = default;
    ARENE_NORETURN throws_on_copy(throws_on_copy const&) { throw test_exception{}; }

    throws_on_copy(throws_on_copy&&) = delete;

    auto operator=(throws_on_copy const&) -> throws_on_copy& = delete;
    auto operator=(throws_on_copy&&) -> throws_on_copy& = delete;
    ~throws_on_copy() = default;
  };

  // Throw on value copy
  {
    result<throws_on_copy, int> source_r{in_place_value};
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    auto const do_copy = [&source_r]() { result<throws_on_copy, int> const res{source_r}; };
    ASSERT_THROW(do_copy(), test_exception);
  }
  // Throw on error copy
  {
    result<int, throws_on_copy> source_r{in_place_error};
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    auto const do_copy = [&source_r]() { result<int, throws_on_copy> const res{source_r}; };
    ASSERT_THROW(do_copy(), test_exception);
  }
  // Value copy throws but is inactive
  {
    result<throws_on_copy, int> source_r{in_place_error};
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    auto const do_copy = [&source_r]() { result<throws_on_copy, int> const res{source_r}; };
    ASSERT_NO_THROW(do_copy());
  }
  // Error copy throws but is inactive
  {
    result<int, throws_on_copy> source_r{in_place_value};
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    auto const do_copy = [&source_r]() { result<int, throws_on_copy> const res{source_r}; };
    ASSERT_NO_THROW(do_copy());
  }
}

/// @test @c arene::base::result<T,E> 's move constructor propagates any exception thrown by the move constructor of the
/// currently populated channel.
TEST(ResultConstructor, ExceptionOnMoveConstructionPropagates) {
  struct test_exception : public std::exception {};
  struct throws_on_move {
    throws_on_move() = default;
    // NOLINTNEXTLINE(hicpp-noexcept-move, bugprone-exception-escape) explicitly testing throw-on-move behavior
    ARENE_NORETURN throws_on_move(throws_on_move&&) { throw test_exception{}; }

    throws_on_move(throws_on_move const&) = default;

    auto operator=(throws_on_move const&) -> throws_on_move& = delete;
    auto operator=(throws_on_move&&) -> throws_on_move& = delete;
    ~throws_on_move() = default;
  };

  // Throw on value move
  {
    result<throws_on_move, int> source_r{in_place_value};
    auto const do_move = [&source_r]() { result<throws_on_move, int> const res{std::move(source_r)}; };
    ASSERT_THROW(do_move(), test_exception);
  }
  // Throw on error move
  {
    result<int, throws_on_move> source_r{in_place_error};
    auto const do_move = [&source_r]() { result<int, throws_on_move> const res{std::move(source_r)}; };
    ASSERT_THROW(do_move(), test_exception);
  }
  // Value move throws but is inactive
  {
    result<throws_on_move, int> source_r{in_place_error};
    auto const do_move = [&source_r]() { result<throws_on_move, int> const res{std::move(source_r)}; };
    ASSERT_NO_THROW(do_move());
  }
  // Error move throws but is inactive
  {
    result<int, throws_on_move> source_r{in_place_value};
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    auto const do_move = [&source_r]() { result<int, throws_on_move> const res{source_r}; };
    ASSERT_NO_THROW(do_move());
  }
}

}  // namespace
