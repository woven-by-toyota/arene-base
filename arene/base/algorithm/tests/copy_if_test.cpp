// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/copy_if.hpp"

#include <cstdint>
#include <initializer_list>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arene/base/algorithm/tests/test_iterators.hpp"
#include "arene/base/array.hpp"
#include "arene/base/detail/wrapped_iterator.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {
template <typename T, T N>
struct greater {
  constexpr auto operator()(T const& value) const -> bool { return value > N; }
};

struct always {
  template <typename T>
  constexpr auto operator()(T) const -> bool {
    return true;
  }
};

struct always_noexcept {
  template <typename T>
  constexpr auto operator()(T) const noexcept -> bool {
    return true;
  }
};

struct never {
  constexpr auto operator()(int) const -> bool { return false; }
};

struct odd {
  constexpr auto operator()(int value) const -> bool { return value % 2 == 1; }
};
/*
 * Correctness tests
 */
/// @test `copy_if` an empty vector to an empty destination vector leaves the destination vector empty.
TEST(CopyIf, CanCopyEmptyVector) {
  std::vector<int> values;
  std::vector<int> dst;
  arene::base::copy_if(values.begin(), values.end(), dst.begin(), always());
  ASSERT_TRUE(dst.empty());
}

/// @test `copy_if` an empty vector to another vector won't change the destination vector.
TEST(CopyIf, CopyEmptyToAnotherVector) {
  auto const values = std::vector<int>{};
  auto dst = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8};
  arene::base::copy_if(values.begin(), values.end(), dst.begin(), always());
  auto const reference = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8};
  for (auto itr = reference.cbegin(), itv = dst.cbegin(); itr < reference.end(); ++itr, ++itv) {
    ASSERT_EQ(*itr, *itv);
  }
}

/// @test `copy_if` an empty vector to itself results in an empty vector.
TEST(CopyIf, CopyEmptyToSelf) {
  auto values = std::vector<int>{};
  arene::base::copy_if(values.begin(), values.end(), values.begin(), always());
  ASSERT_TRUE(values.empty());
}

/// @test `copy_if` a vector and a predicate that always returns false results in an empty
/// destination vector.
TEST(CopyIf, CopyNever) {
  auto values = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8};
  auto dst = std::vector<int>{};
  arene::base::copy_if(values.begin(), values.end(), dst.begin(), never());
  ASSERT_TRUE(dst.empty());
}

/// @test `copy_if` a vector with only one element and two predicates that
/// always return false and true respectively yields the expected result.
TEST(CopyIf, CanCopyOneElementVector) {
  constexpr auto value = 42;
  constexpr auto old_value = 0;
  auto const values = std::vector<int>{value};
  auto dst = std::vector<int>{old_value};
  arene::base::copy_if(values.begin(), values.end(), dst.begin(), never());
  ASSERT_EQ(dst.front(), old_value);
  arene::base::copy_if(values.begin(), values.end(), dst.begin(), always());
  ASSERT_EQ(dst.front(), value);
}

/// @test `copy_if` a subset of vector elements to the same vector at a
/// different location unconditionally with a predicate that always returns
/// true yields the expected result.
TEST(CopyIf, CopyToSameVectorUnconditional) {
  auto values = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8};
  arene::base::copy_if(values.begin(), values.begin() + 3, values.end() - 3, always());
  ASSERT_EQ(values.front(), *(values.end() - 3));
}

/// @test `copy_if` a subset of vector elements to the same vector at a
/// different location conditionally with a predicate that returns true for
/// values greater than 2 yields the expected result.
TEST(CopyIf, CopyToSameVectorConditional) {
  auto values = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8};
  arene::base::copy_if(values.begin(), values.begin() + 3, values.end() - 3, greater<int, 2>());
  auto const reference = std::vector<int>{1, 2, 3, 4, 5, 3, 7, 8};
  for (auto itr = reference.cbegin(), itv = values.cbegin(); itr < reference.end(); ++itr, ++itv) {
    ASSERT_EQ(*itr, *itv);
  }
}

/// @test `copy_if` a subset of vector elements to a different vector
/// unconditionally with a predicate that always returns true yields the
/// expected result.
TEST(CopyIf, CopyToAnotherVectorUnconditional) {
  auto const values = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8};
  auto dst = std::vector<int>{0, 0, 0, 0, 0, 0, 0, 0};
  auto const reference = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8};
  arene::base::copy_if(values.begin(), values.end(), dst.begin(), always());
  for (auto itr = reference.cbegin(), itv = dst.cbegin(); itr < reference.end(); ++itr, ++itv) {
    ASSERT_EQ(*itr, *itv);
  }
}

/// @test `copy_if` vector elements to a different vector conditionally with
/// a predicate that returns true for values greater than 4 yields the
/// expected result.
TEST(CopyIf, CopyToAnotherVectorConditional) {
  auto const values = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8};
  auto dst = std::vector<int>{0, 0, 0, 0, 0, 0, 0, 0};
  auto const reference = std::vector<int>{5, 6, 7, 8, 0, 0, 0, 0};
  arene::base::copy_if(values.begin(), values.end(), dst.begin(), greater<int, 4>());
  for (auto itr = reference.cbegin(), itv = dst.cbegin(); itr < reference.end(); ++itr, ++itv) {
    ASSERT_EQ(*itr, *itv);
  }
}

/// @test `copy_if` from a stream iterator to a vector, conditionally with
/// a predicate that returns true for odd values, yields the expected result.
TEST(CopyIf, CopyFromStreamIterator) {
  auto str = std::stringstream("1 2 3 4 5 6 7 8");
  auto dst = std::vector<int>{0, 0, 0, 0, 0, 0, 0, 0};

  arene::base::copy_if(std::istream_iterator<int>(str), std::istream_iterator<int>(), dst.begin(), odd());

  auto const reference = std::vector<int>{1, 3, 5, 7};
  for (auto itr = reference.cbegin(), itv = dst.cbegin(); itr < reference.end(); ++itr, ++itv) {
    ASSERT_EQ(*itr, *itv);
  }
}

/// @test `copy_if` from a vector to a stream iterator, conditionally with
/// a predicate that returns true for odd values, yields the expected result.
TEST(CopyIf, CopyToStreamIterator) {
  auto output = std::stringstream();
  auto dst = std::ostream_iterator<int>(output, " ");

  auto const values = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8};
  arene::base::copy_if(values.begin(), values.end(), dst, odd());

  auto result = std::string(output.str());
  ASSERT_EQ(result, "1 3 5 7 ");
}

/*
 * Tests for work in constexpr context
 */

using array8 = arene::base::array<std::uint32_t, 8>;

constexpr auto do_copy_if(array8 values) -> array8 {
  auto dst = array8{};
  arene::base::copy_if(values.begin(), values.end(), dst.begin(), greater<std::uint32_t, 4>());
  return dst;
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
constexpr auto do_copy_if(array8 init, array8 values, unsigned values_offset, unsigned len, unsigned dst_offset)
    -> array8 {
  auto dst = array8(init);
  arene::base::copy_if(
      values.begin() + values_offset,
      values.begin() + values_offset + len,
      dst.begin() + dst_offset,
      greater<std::uint32_t, 4>()
  );
  return dst;
}

/// @test `copy_if` an array of numbers in a constexpr context yields the
/// expected result.
TEST(CopyIf, CopyInConstexprContext) {
  static constexpr auto values = arene::base::array<std::uint32_t, 8>{1, 2, 3, 4, 5, 6, 7, 8};
  static constexpr auto reference = arene::base::array<std::uint32_t, 8>{5, 6, 7, 8, 0, 0, 0, 0};
  STATIC_ASSERT_EQ(do_copy_if(values), reference);
}

/// @test `copy_if` a subset of array elements in a constexpr context yields
/// the expected result.
TEST(CopyIf, CopyPartialInConstexprContext) {
  static constexpr auto values = arene::base::array<std::uint32_t, 8>{{1, 2, 3, 4, 5, 6, 7, 8}};
  static constexpr auto initial = arene::base::array<std::uint32_t, 8>{{101, 102, 103, 104, 105, 106, 107, 108}};
  static constexpr auto reference = arene::base::array<std::uint32_t, 8>{101, 102, 103, 5, 6, 106, 107, 108};
  STATIC_ASSERT_EQ(do_copy_if(initial, values, 2, 4, 3), reference);
}

/*
 * Tests for noexcept conditions
 */

/// @test Verify `copy_if` is noexcept if all the arguments are noexcept.
TEST(CopyIf, IsNoexceptIfAllNoexcept) {
  STATIC_ASSERT_TRUE(
      noexcept(arene::base::copy_if(noexcept_iterator(), noexcept_iterator(), noexcept_iterator(), always_noexcept()))
  );
}

/// @test Verify `copy_if` is not noexcept if the predicate throws.
TEST(CopyIf, IsNotNoexceptIfPredicateThrows) {
  STATIC_ASSERT_TRUE(
      !noexcept(arene::base::copy_if(noexcept_iterator(), noexcept_iterator(), noexcept_iterator(), always()))
  );
}

/// @test Verify `copy_if` is not noexcept if the increment operator of the iterator
/// argument throws.
TEST(CopyIf, IsNotNoexceptIfIncrementThrows) {
  STATIC_ASSERT_FALSE(noexcept(arene::base::copy_if(
      throwing_increment_iterator(),
      throwing_increment_iterator(),
      throwing_increment_iterator(),
      always_noexcept()
  )));
}

/// @test Verify `copy_if` is not noexcept if the comparison operator of the iterator
/// argument throws.
TEST(CopyIf, IsNotNoexceptIfComparisonThrows) {
  STATIC_ASSERT_FALSE(noexcept(arene::base::copy_if(
      throwing_comparison_iterator(),
      throwing_comparison_iterator(),
      throwing_comparison_iterator(),
      always_noexcept()
  )));
}

/// @test Verify `copy_if` is not noexcept if the reference operator of the iterator
/// argument throws.
TEST(CopyIf, IsNotNoexceptIfReferenceThrows) {
  STATIC_ASSERT_FALSE(noexcept(arene::base::copy_if(
      throwing_reference_iterator(),
      throwing_reference_iterator(),
      throwing_reference_iterator(),
      always_noexcept()
  )));
}

/// @test Verify `copy_if` is not noexcept if the assignment operator of the iterator
/// argument throws.
TEST(CopyIf, IsNotNoexceptIfAssignmentThrows) {
  STATIC_ASSERT_FALSE(noexcept(arene::base::copy_if(
      throwing_assignment_iterator(),
      throwing_assignment_iterator(),
      throwing_assignment_iterator(),
      always_noexcept()
  )));
}

/// @test Verify `copy_if` is not noexcept if all of the operators of the iterator
/// argument throw.
TEST(CopyIf, IsNotNoexceptIfAllThrows) {
  STATIC_ASSERT_FALSE(
      noexcept(arene::base::copy_if(throwing_iterator(), throwing_iterator(), throwing_iterator(), always()))
  );
}

/// @test Verify `copy_if` can be used with a predicate which self-mutates.
TEST(CopyIf, AllowsMutablePredicate) {
  auto const input_data = {2, 1, 1, 3, 2, 1, 5};
  auto copy_increasing_elements = [max = std::numeric_limits<int>::lowest()](auto value) mutable {
    if (value > max) {
      max = value;
      return true;
    }
    return false;
  };
  std::vector<int> output;
  arene::base::copy_if(input_data.begin(), input_data.end(), std::back_inserter(output), copy_increasing_elements);
  EXPECT_THAT(output, ::testing::ElementsAre(2, 3, 5));
}

}  // namespace
