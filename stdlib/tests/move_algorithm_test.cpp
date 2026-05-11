// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/diagnostics.hpp"
#include "stdlib/include/algorithm"
#include "stdlib/include/cstddef"
#include "stdlib/include/cstdint"
#include "stdlib/include/iterator"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"

namespace {

/// @test Ensure that moving from an empty range does not overwrite the destination, and returns the destination
CONSTEXPR_TEST(Move, MovingEmptyRangeDoesNothing) {
  constexpr std::uint32_t initial_target = 99;
  std::uint32_t const source = 42;
  std::uint32_t target = initial_target;
  auto const* const begin = &source;
  auto* const destination = &target;
  auto* const result = std::move(begin, begin, destination);

  ASSERT_EQ(result, destination);
  ASSERT_EQ(target, initial_target);
}

/// @test Moving from a source range transfers those elements to the destination
CONSTEXPR_TEST(Move, MoveTransfersTheElementsInTheRange) {
  constexpr std::size_t source_size = 6;
  constexpr std::size_t destination_size = 10;
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  std::uint32_t const source[source_size] = {1, 2, 3, 4, 5, 6};
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  std::uint32_t destination[destination_size] = {};

  auto* const result = std::move(&source[0], &source[source_size], &destination[0]);

  ASSERT_EQ(result, &destination[source_size]);

  for (std::uint32_t offset = 0; offset < source_size; ++offset) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    ASSERT_EQ(destination[offset], source[offset]);
  }
  for (std::uint32_t offset = source_size; offset < destination_size; ++offset) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    ASSERT_EQ(destination[offset], 0);
  }
}

class movable {
  std::uint32_t value_;

 public:
  constexpr movable() noexcept
      : value_(0) {}
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr movable(std::uint32_t value) noexcept
      : value_(value) {}
  constexpr movable(movable const&) = default;
  constexpr movable(movable&& other) noexcept
      : value_(other.value_) {
    other.value_ = 0;
  }

  constexpr auto operator=(movable const&) -> movable& = default;
  constexpr auto operator=(movable&& other) noexcept -> movable& {
    value_ = other.value_;
    other.value_ = 0;
    return *this;
  }

  ~movable() = default;

  constexpr auto val() const noexcept -> std::uint32_t { return value_; }
};

/// @test Moving from a source range transfers those elements to the destination, leaving the source in a moved-from
/// state
CONSTEXPR_TEST(Move, MoveMovesTheElementsInTheRangeFromNonConstSource) {
  constexpr std::size_t source_size = 6;
  constexpr std::size_t destination_size = 10;
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  movable const original[source_size] = {1, 2, 3, 4, 5, 6};
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  movable source[source_size] = {};
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  movable destination[destination_size] = {};

  for (std::uint32_t offset = 0; offset < source_size; ++offset) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    source[offset] = original[offset];
  }

  auto* const result = std::move(&source[0], &source[source_size], &destination[0]);

  ASSERT_EQ(result, &destination[source_size]);

  for (std::uint32_t offset = 0; offset < source_size; ++offset) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    ASSERT_EQ(destination[offset].val(), original[offset].val());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    ASSERT_EQ(source[offset].val(), 0);
  }
  for (std::uint32_t offset = source_size; offset < destination_size; ++offset) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    ASSERT_EQ(destination[offset].val(), 0);
  }
}

/// @test If all the iterator operations and move assignment are noexcept, so is @c move
TEST(Move, MoveIsNoexceptIfAllOperationsAreNoexcept) {
  static_assert(
      noexcept(std::move(std::declval<movable*>(), std::declval<movable*>(), std::declval<movable*>())),
      "Must be noexcept"
  );
  static_assert(
      noexcept(std::move(std::declval<std::int32_t*>(), std::declval<std::int32_t*>(), std::declval<std::int32_t*>())),
      "Must be noexcept"
  );
}

class throwing_move_assign {
 public:
  throwing_move_assign() noexcept = default;
  throwing_move_assign(throwing_move_assign const&) noexcept = default;
  throwing_move_assign(throwing_move_assign&&) noexcept = default;
  auto operator=(throwing_move_assign const&) noexcept -> throwing_move_assign& = default;
  auto operator=(throwing_move_assign&&) noexcept(false) -> throwing_move_assign& { return *this; }
  ~throwing_move_assign() = default;
};

/// @test If move assignment is not noexcept, @c move is not either
TEST(Move, MoveIsNotNoexceptIfMoveAssignmentNotNoexcept) {
  static_assert(
      !noexcept(std::move(
          std::declval<throwing_move_assign*>(),
          std::declval<throwing_move_assign*>(),
          std::declval<throwing_move_assign*>()
      )),
      "Must not be noexcept"
  );
}

class throwing_conversion_target {
 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  throwing_conversion_target(movable&&) noexcept(false) {}
  throwing_conversion_target() noexcept = default;
  throwing_conversion_target(throwing_conversion_target const&) noexcept = default;
  throwing_conversion_target(throwing_conversion_target&&) noexcept = default;
  auto operator=(throwing_conversion_target const&) noexcept -> throwing_conversion_target& = default;
  auto operator=(throwing_conversion_target&&) noexcept -> throwing_conversion_target& = default;
  ~throwing_conversion_target() = default;
};

/// @test If move assignment via conversion is not noexcept, @c move is not either
TEST(Move, MoveIsNotNoexceptIfMoveConversionNotNoexcept) {
  static_assert(
      !noexcept(
          std::move(std::declval<movable*>(), std::declval<movable*>(), std::declval<throwing_conversion_target*>())
      ),
      "Must not be noexcept"
  );
}

/// @test If source iterator operations are not noexcept, @c move is not either
TEST(Move, MoveIsNotNoexceptIfIteratorRangeNotNoexcept) {
  struct deref_result {};

  ARENE_IGNORE_START();
  ARENE_IGNORE_CLANG("-Wunused-local-typedef", "Typedefs required for iterator definition");
  ARENE_IGNORE_GCC("-Wunused-local-typedefs", "Typedefs required for iterator definition");

  struct basic_input_iterator {
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using reference = deref_result;
    using value_type = deref_result;
    using pointer = void;

    auto operator++() noexcept -> basic_input_iterator&;
    auto operator*() noexcept -> deref_result;

    struct post_increment_result {
      auto operator*() noexcept -> deref_result;
    };
    auto operator++(int) noexcept -> post_increment_result;

    auto operator==(basic_input_iterator const&) noexcept -> bool;
    auto operator!=(basic_input_iterator const&) noexcept(false) -> bool;
  };

  ARENE_IGNORE_END();
  static_assert(
      !noexcept(std::move(
          std::declval<basic_input_iterator>(),
          std::declval<basic_input_iterator>(),
          std::declval<deref_result*>()
      )),
      "Must not be noexcept"
  );
}

/// @test If destination iterator operations are not noexcept, @c move is not either
TEST(Move, MoveIsNotNoexceptIfOutputIteratorNotNoexcept) {
  struct deref_result {};

  ARENE_IGNORE_START();
  ARENE_IGNORE_CLANG("-Wunused-local-typedef", "Typedefs required for iterator definition");
  ARENE_IGNORE_GCC("-Wunused-local-typedefs", "Typedefs required for iterator definition");

  struct basic_output_iterator {
    using iterator_category = std::output_iterator_tag;
    using difference_type = void;
    using reference = void;
    using value_type = void;
    using pointer = void;

    auto operator++() -> basic_output_iterator&;
    auto operator*() noexcept -> deref_result;

    struct post_increment_result {
      struct post_inc_deref_result {};
      auto operator*() noexcept -> post_inc_deref_result;
    };
    auto operator++(int) noexcept -> post_increment_result;

    auto operator!=(basic_output_iterator const&) noexcept -> bool;
  };

  ARENE_IGNORE_END();
  static_assert(
      !noexcept(
          std::move(std::declval<deref_result*>(), std::declval<deref_result*>(), std::declval<basic_output_iterator>())
      ),
      "Must not be noexcept"
  );
}

/// @test Validate behaviour when the destination overlaps the start of the range
CONSTEXPR_TEST(Move, DestinationOverlapsStartOfRange) {
  constexpr std::size_t full_size = 10;
  constexpr std::size_t move_size = 5;
  constexpr std::size_t source_offset = 3;
  constexpr std::size_t destination_offset = 1;
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  movable data[full_size] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  auto* const result = std::move(&data[source_offset], &data[source_offset + move_size], &data[destination_offset]);

  ASSERT_EQ(result, &data[destination_offset + move_size]);

  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  std::uint32_t const expected[full_size] = {1, 4, 5, 6, 7, 8, 0, 0, 9, 10};

  for (std::uint32_t i = 0; i < full_size; ++i) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    ASSERT_EQ(data[i].val(), expected[i]);
  }
}

/// @test Validate behaviour when the destination overlaps the end of the range
CONSTEXPR_TEST(Move, DestinationOverlapsEndOfRange) {
  constexpr std::size_t full_size = 10;
  constexpr std::size_t move_size = 5;
  constexpr std::size_t source_offset = 1;
  constexpr std::size_t destination_offset = 3;
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  movable data[full_size] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  auto* const result = std::move(&data[source_offset], &data[source_offset + move_size], &data[destination_offset]);

  ASSERT_EQ(result, &data[destination_offset + move_size]);

  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  std::uint32_t const expected[full_size] = {1, 0, 0, 0, 0, 0, 3, 2, 9, 10};

  for (std::uint32_t i = 0; i < full_size; ++i) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    ASSERT_EQ(data[i].val(), expected[i]);
  }
}

}  // namespace
