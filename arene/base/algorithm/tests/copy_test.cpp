// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/copy.hpp"

#include <gmock/gmock.h>  // IWYU pragma: keep
#include <gtest/gtest.h>  // IWYU pragma: keep

#include "arene/base/algorithm/tests/test_iterators.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/make_signed.hpp"
#include "arene/base/stdlib_choice/move_iterator.hpp"

namespace {

template <typename T>
class CopyTest : public ::testing::Test {};

using test_types =
    ::testing::Types<std::int16_t, std::int32_t, std::int64_t, std::uint16_t, std::uint32_t, std::uint64_t>;
TYPED_TEST_SUITE(CopyTest, test_types, );

/// @test `copy` from an empty array to an empty destination array does nothing.
TYPED_TEST(CopyTest, CanCopyEmptyArray) {
  arene::base::array<TypeParam, 0> values;
  arene::base::array<TypeParam, 0> dst;
  arene::base::copy(values.begin(), values.end(), dst.begin());
}

/// @test `copy` from an empty array to another array won't change the destination array.
TYPED_TEST(CopyTest, CopyEmptyToAnotherArray) {
  arene::base::array<TypeParam, 0> const values{};
  arene::base::array<TypeParam, 8> dst{1, 2, 3, 4, 5, 6, 7, 8};
  arene::base::copy(values.begin(), values.end(), dst.begin());
  ASSERT_EQ(dst.front(), 1);
}

/// @test `copy` from an empty array to itself yields an empty array.
TYPED_TEST(CopyTest, CopyEmptyToSelf) {
  arene::base::array<TypeParam, 0> values{};
  arene::base::copy(values.begin(), values.end(), values.begin());
  ASSERT_TRUE(values.empty());
}

/// @test `copy` from a single element array to another array yields a destination array containing the value.
TYPED_TEST(CopyTest, CanCopyOneElementArray) {
  constexpr auto value = 42;
  arene::base::array<TypeParam, 8> const values{value};
  arene::base::array<TypeParam, 8> dst{0};
  arene::base::copy(values.begin(), values.end(), dst.begin());
  ASSERT_EQ(dst.front(), value);
}

/// @test `copy` from a subset of an array to itself at different offsets, resulting in the correct values in the array.
TYPED_TEST(CopyTest, CopyToSameArray) {
  arene::base::array<TypeParam, 8> values{1, 2, 3, 4, 5, 6, 7, 8};
  arene::base::array<TypeParam, 8> const reference{1, 2, 3, 4, 5, 1, 2, 3};
  constexpr auto shift = 3U;
  arene::base::copy(values.begin(), values.begin() + shift, values.end() - shift);
  for (auto itr = reference.cbegin(), itv = values.cbegin(); itr < reference.end(); ++itr, ++itv) {
    ASSERT_EQ(*itr, *itv);
  }
}

/// @test `copy` from a source array to a destination array places all source values in the destination.
TYPED_TEST(CopyTest, CopyToAnotherArray) {
  arene::base::array<TypeParam, 8> const values{1, 2, 3, 4, 5, 6, 7, 8};
  arene::base::array<TypeParam, 8> dst{0, 0, 0, 0, 0, 0, 0, 0};
  arene::base::array<TypeParam, 8> const reference{1, 2, 3, 4, 5, 6, 7, 8};
  arene::base::copy(values.begin(), values.end(), dst.begin());
  for (auto itr = reference.cbegin(), itv = dst.cbegin(); itr < reference.end(); ++itr, ++itv) {
    ASSERT_EQ(*itr, *itv);
  }
}

/// @brief An input iterator that yields every value of the specified type in sequence, starting with the requested one
template <typename T>
class iota_yielder {
  T current_;

 public:
  using iterator_category = std::input_iterator_tag;
  using difference_type = std::make_signed_t<T>;
  using value_type = T;
  using reference = T const&;
  using pointer = T const*;

  explicit constexpr iota_yielder(T start = T{}) noexcept
      : current_(start) {}

  constexpr auto operator*() const noexcept -> reference { return current_; }

  constexpr auto operator->() const noexcept -> pointer { return &current_; }

  constexpr auto operator++() noexcept -> iota_yielder& {
    ++current_;
    return *this;
  }

  constexpr auto operator++(int) noexcept -> iota_yielder {
    iota_yielder const with_old_value(*this);
    current_++;
    return with_old_value;
  }

  constexpr auto operator==(iota_yielder const& other) noexcept -> bool { return current_ == other.current_; }

  constexpr auto operator!=(iota_yielder const& other) noexcept -> bool { return !(*this == other); }
};

/// @test `copy` from an iota iterator to a destination array places all values from the iota into the destination.
TYPED_TEST(CopyTest, CopyFromStreamIterator) {
  arene::base::array<TypeParam, 8> dst = {0, 0, 0, 0, 0, 0, 0, 0};

  arene::base::copy(iota_yielder<TypeParam>(1), iota_yielder<TypeParam>(9), dst.begin());

  arene::base::array<TypeParam, 8> const reference{1, 2, 3, 4, 5, 6, 7, 8};
  for (auto itr = reference.cbegin(), itv = dst.cbegin(); itr < reference.end(); ++itr, ++itv) {
    ASSERT_EQ(*itr, *itv);
  }
}

/// @brief An output iterator that checks that the written values are sequential values of the specified type
template <typename T>
class iota_checker {
 public:
  /// @brief When assigned from a <c>T</c>, does a GoogleTest assertion that the value is the expected one
  class value_checker {
    T expected_value_;

   public:
    explicit constexpr value_checker(T expected_value) noexcept
        : expected_value_(expected_value) {}

    auto operator=(T to_check) noexcept -> value_checker& {
      EXPECT_EQ(expected_value_, to_check);
      return *this;
    }
  };

 private:
  T current_value_;
  value_checker current_checker_;

 public:
  using iterator_category = std::output_iterator_tag;
  using difference_type = std::make_signed_t<T>;
  using value_type = value_checker;
  using reference = value_checker&;
  using pointer = value_checker*;

  explicit constexpr iota_checker(T start = T{}) noexcept
      : current_value_(start),
        current_checker_(current_value_) {}

  constexpr auto operator*() noexcept -> reference { return current_checker_; }

  constexpr auto operator->() noexcept -> pointer { return &current_checker_; }

  constexpr auto operator++() noexcept -> iota_checker& {
    ++current_value_;
    current_checker_ = value_checker(current_value_);
    return *this;
  }

  constexpr auto operator++(int) noexcept -> iota_checker {
    iota_checker const with_old_value(*this);
    ++(*this);
    return with_old_value;
  }

  constexpr auto operator==(iota_checker const& other) noexcept -> bool {
    return current_value_ == other.current_value_;
  }

  constexpr auto operator!=(iota_checker const& other) noexcept -> bool { return !(*this == other); }
};

/// @test `copy` to an iota iterator from an array causes all of the array values to be copied in sequence
TYPED_TEST(CopyTest, CopyToStreamIterator) {
  arene::base::array<TypeParam, 8> const values{1, 2, 3, 4, 5, 6, 7, 8};
  arene::base::copy(values.begin(), values.end(), iota_checker<TypeParam>(1));
}

namespace {

template <typename T>
using array8 = arene::base::array<T, 8>;

template <typename T>
constexpr auto do_copy(array8<T> values) -> array8<T> {
  auto dst = array8<T>{};
  arene::base::copy(values.begin(), values.end(), dst.begin());
  return dst;
}

template <typename T>
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
constexpr auto do_copy(array8<T> init, array8<T> values, unsigned values_offset, unsigned len, unsigned dst_offset)
    -> array8<T> {
  auto dst = array8<T>(init);
  arene::base::copy(values.begin() + values_offset, values.begin() + values_offset + len, dst.begin() + dst_offset);
  return dst;
}

}  // namespace

/// @test `copy` from an array of numbers in a constexpr context, and the results are as expected.
TYPED_TEST(CopyTest, CopyInConstexprContext) {
  static constexpr arene::base::array<TypeParam, 8> values{{1, 2, 3, 4, 5, 6, 7, 8}};

  constexpr auto dst = do_copy(values);

  static constexpr arene::base::array<TypeParam, 8> reference{{1, 2, 3, 4, 5, 6, 7, 8}};

  static_assert(dst[0] == reference[0], "Can't copy value");
  static_assert(dst[1] == reference[1], "Can't copy value");
  static_assert(dst[2] == reference[2], "Can't copy value");
  static_assert(dst[3] == reference[3], "Can't copy value");
  static_assert(dst[4] == reference[4], "Can't copy value");
  static_assert(dst[5] == reference[5], "Can't copy value");
  static_assert(dst[6] == reference[6], "Can't copy value");
  static_assert(dst[7] == reference[7], "Can't copy value");
}

/// @test `copy` from a subset of an array of numbers in a constexpr context, and the results are as expected.
TYPED_TEST(CopyTest, CopyPartialInConstexprContext) {
  static constexpr arene::base::array<TypeParam, 8> values{{1, 2, 3, 4, 5, 6, 7, 8}};
  static constexpr arene::base::array<TypeParam, 8> initial{{101, 102, 103, 104, 105, 106, 107, 108}};

  constexpr auto dst = do_copy(initial, values, 2, 4, 3);

  static constexpr arene::base::array<TypeParam, 8> reference{101, 102, 103, 3, 4, 5, 6, 108};

  static_assert(dst[0] == reference[0], "Can't copy value");
  static_assert(dst[1] == reference[1], "Can't copy value");
  static_assert(dst[2] == reference[2], "Can't copy value");
  static_assert(dst[3] == reference[3], "Can't copy value");
  static_assert(dst[4] == reference[4], "Can't copy value");
  static_assert(dst[5] == reference[5], "Can't copy value");
  static_assert(dst[6] == reference[6], "Can't copy value");
  static_assert(dst[7] == reference[7], "Can't copy value");
}

/// @test `copy` can copy (move) from an input range that yields rvalue references instead of lvalue references
TYPED_TEST(CopyTest, IteratorYieldingRvalueRef) {
  constexpr arene::base::array<TypeParam, 8> original{1, 2, 3, 4, 5, 6, 7, 8};
  arene::base::array<TypeParam, 8> source{};
  arene::base::copy(original.begin(), original.end(), source.begin());

  arene::base::array<TypeParam, 8> dest{};
  arene::base::copy(std::make_move_iterator(source.begin()), std::make_move_iterator(source.end()), dest.begin());
  ASSERT_THAT(dest, ::testing::ElementsAreArray(original));
}

/*
 * Tests for noexcept conditions
 */
/// @test Verify `copy` is noexcept if all the arguments are noexcept.
TEST(CopyTest, IsNoexceptIfAllNoexcept) {
  static_assert(
      noexcept(arene::base::copy(noexcept_iterator(), noexcept_iterator(), noexcept_iterator())),
      "Must be noexcept"
  );
}

/// @test Verify `copy` is not noexcept if the increment operator of the iterator argument throws.
TEST(CopyTest, IsNotNoexceptIfIncrementThrows) {
  static_assert(
      !noexcept(
          arene::base::copy(throwing_increment_iterator(), throwing_increment_iterator(), throwing_increment_iterator())
      ),
      "Must not be noexcept"
  );
}

/// @test Verify `copy` is not noexcept if the comparison operator of the iterator argument throws.
TEST(CopyTest, IsNotNoexceptIfComparisonThrows) {
  static_assert(
      !noexcept(arene::base::copy(
          throwing_comparison_iterator(),
          throwing_comparison_iterator(),
          throwing_comparison_iterator()
      )),
      "Must not be noexcept"
  );
}

/// @test Verify `copy` is not noexcept if the assignment operator of the iterator argument throws.
TEST(CopyTest, IsNotNoexceptIfReferenceThrows) {
  static_assert(
      !noexcept(
          arene::base::copy(throwing_reference_iterator(), throwing_reference_iterator(), throwing_reference_iterator())
      ),
      "Must not be noexcept"
  );
}

/// @test Verify `copy` is not noexcept if the assignment operator of the iterator argument throws.
TEST(CopyTest, IsNotNoexceptIfAssignmentThrows) {
  static_assert(
      !noexcept(arene::base::copy(
          throwing_assignment_iterator(),
          throwing_assignment_iterator(),
          throwing_assignment_iterator()
      )),
      "Must not be noexcept"
  );
}

/// @test Verify `copy` is not noexcept if all of the operators of the iterator argument throw.
TEST(CopyTest, IsNotNoexceptIfAllThrows) {
  static_assert(
      !noexcept(arene::base::copy(throwing_iterator(), throwing_iterator(), throwing_iterator())),
      "Must not be noexcept"
  );
}
}  // namespace
