// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/inline_container/detail/lower_bound_index.hpp"

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>
#include <string>
#include <tuple>
#include <vector>

#include <gtest/gtest.h>

#include "arene/base/compare.hpp"
#include "arene/base/math/log2.hpp"

namespace {
using ::arene::base::compare_three_way;
using ::arene::base::strong_ordering;
using ::arene::base::inline_container::detail::lower_bound_index;

// Helper to extract the index type used by a vector<T>
template <typename T>
using index_type = typename std::vector<T>::size_type;

// Helper that makes the values between 1 and 10 of a given value type. There is a gap at a specified position.
template <typename T>
class one_to_ten_maker {
 public:
  // The values in the range [1,10]
  std::vector<T> values;
  // constructor which filles values
  one_to_ten_maker() {
    for (T value = first(); value < last() + 1; ++value) {
      values.push_back(value);
    }
  }

  // Alias to values.size()
  auto size() const -> index_type<T> { return values.size(); }
  // A value in T which will compare less than all other values.
  static auto lowest() -> T { return std::numeric_limits<T>::lowest(); }
  // A value in T which is equivalent to zero.
  static auto zero() -> T { return 0; }
  // A value in T which is the first element in values.
  static auto first() -> T { return T{1}; }
  // A value in T which is the last element in values.
  static auto last() -> T { return T{10}; }
  // A value in T which is equivalent to 11.
  static auto one_greater() -> T { return 11; }
  // A value in T which will compare greater than all other values.
  static auto max() -> T { return std::numeric_limits<T>::max(); }
};

// A specialization which produces the 1st through 10th letters in the alphabet as strings, when "a" is the 0th letter.
template <>
class one_to_ten_maker<std::string> {
 public:
  // The values in the range ["b","k"]
  std::vector<std::string> values{"b", "c", "d", "e", "f", "g", "h", "i", "j", "k"};
  // Alias to values.size()
  auto size() const -> index_type<std::string> { return values.size(); }
  // A std::string which will compare less than all other values.
  static auto lowest() -> std::string { return ""; }
  /// A std::string equivalent to the zero'th position, "a" in this case.
  static auto zero() -> std::string { return "a"; }
  // A value in T which is the first element in values.
  static auto first() -> std::string { return "b"; }
  // A value in T which is the last element in values.
  static auto last() -> std::string { return "k"; }
  /// A std::string equivalent to the 11th position, "l" in this case.
  static auto one_greater() -> std::string { return "l"; }
  /// A std::string which will compare greater than all other single-character lower case letter strings.
  static auto max() -> std::string { return "z"; }
};

template <typename T>
auto in_one_to_ten(one_to_ten_maker<T> const& one_to_ten) {
  return [&one_to_ten](index_type<T> idx) -> T const& { return one_to_ten.values[idx]; };
}

template <typename T>
class LowerBoundIndexTest : public ::testing::Test {
 public:
  one_to_ten_maker<T> const one_to_ten;
};

using key_types = ::testing::
    Types<uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t, float, double, std::string>;

TYPED_TEST_SUITE(LowerBoundIndexTest, key_types, );

/// @test `lower_bound_index` returns the maximum value of the index type, and `strong_ordering::greater` if the value
/// supplied compares greater than all the elements in the supplied range
/// @tparam TypeParam The key and index type
TYPED_TEST(LowerBoundIndexTest, IfValueIsGreaterThanAllValuesReturnsMaxIndexAndGreater) {
  for (auto const& value : {this->one_to_ten.one_greater(), this->one_to_ten.max()}) {
    auto const result = lower_bound_index<TypeParam, TypeParam, index_type<TypeParam>>(
        value,
        this->one_to_ten.size(),
        compare_three_way{},
        in_one_to_ten(this->one_to_ten)
    );
    EXPECT_EQ(result.index, this->one_to_ten.size());
    EXPECT_EQ(result.order, strong_ordering::greater);
  }
}

/// @test `lower_bound_index` returns the zero value of the index type, and `strong_ordering::less` if the value
/// supplied compares less than all the elements in the supplied range
/// @tparam TypeParam The key and index type
TYPED_TEST(LowerBoundIndexTest, IfValueIsLessThanAllValuesReturnsZeroAndGreater) {
  for (auto const& value : {this->one_to_ten.zero(), this->one_to_ten.lowest()}) {
    auto const result = lower_bound_index<TypeParam, TypeParam, index_type<TypeParam>>(
        value,
        this->one_to_ten.size(),
        compare_three_way{},
        in_one_to_ten(this->one_to_ten)
    );
    EXPECT_EQ(result.index, 0);
    EXPECT_EQ(result.order, strong_ordering::greater);
  }
}

/// @test `lower_bound_index` returns the index of the value, and `strong_ordering::equal` if the value
/// supplied compares equal to one of the elements in the supplied range
/// @tparam TypeParam The key and index type
TYPED_TEST(LowerBoundIndexTest, IfValueIsInValuesReturnsIndexOfValueAndEqual) {
  auto const accessor = in_one_to_ten(this->one_to_ten);
  for (index_type<TypeParam> idx = 0; idx < this->one_to_ten.size(); ++idx) {
    auto const result = lower_bound_index<TypeParam, TypeParam, index_type<TypeParam>>(
        accessor(idx),
        this->one_to_ten.size(),
        compare_three_way{},
        accessor
    );
    EXPECT_EQ(result.index, idx);
    EXPECT_EQ(result.order, strong_ordering::equal);
  }
}

/// @test `lower_bound_index` returns the index of the first value that compares greater than the supplied value, and
/// `strong_ordering::greater` if the value supplied compares greater than some elements in the supplied range and less
/// than the remainder
/// @tparam TypeParam The key and index type
TYPED_TEST(LowerBoundIndexTest, IfValueGapInValuesReturnsIndexOfNextLargerAndGreater) {
  auto const cmp = compare_three_way{};
  for (decltype(auto) gap_element : this->one_to_ten.values) {
    auto one_to_ten_with_gap = this->one_to_ten;

    auto const location_of_gap_itr = one_to_ten_with_gap.values.erase(
        std::find(one_to_ten_with_gap.values.begin(), one_to_ten_with_gap.values.end(), gap_element)
    );
    auto const accessor = in_one_to_ten(one_to_ten_with_gap);
    auto const result = lower_bound_index<TypeParam, TypeParam, index_type<TypeParam>>(
        gap_element,
        one_to_ten_with_gap.size(),
        cmp,
        accessor
    );
    EXPECT_EQ(result.order, strong_ordering::greater);
    EXPECT_EQ(result.index, std::distance(one_to_ten_with_gap.values.begin(), location_of_gap_itr));
  }
}

/// @test `lower_bound_index` is a binary search, and therefore logarithmic in the number of comparisons
/// @tparam TypeParam The key and index type
TYPED_TEST(LowerBoundIndexTest, IsBinarySearch) {
  std::size_t element_access_count = 0U;
  auto const counted_in_one_to_ten = [&element_access_count,
                                      accessor = in_one_to_ten(this->one_to_ten)](index_type<TypeParam> idx
                                     ) -> TypeParam const& {
    ++element_access_count;
    return accessor(idx);
  };
  // For a binary search, the maximum number of comparisons should be the log2 of the size. However if size isn't an
  // even power of 2 the search isn't "balanced," so an extra access is needed, hence the + 1.
  auto const max_element_accesses = ::arene::base::log2(this->one_to_ten.size()) + 1;
  {
    SCOPED_TRACE("element outside all");
    for (auto const& value :
         {this->one_to_ten.zero(), this->one_to_ten.lowest(), this->one_to_ten.one_greater(), this->one_to_ten.max()}) {
      element_access_count = 0U;
      std::ignore = lower_bound_index<TypeParam, TypeParam, index_type<TypeParam>>(
          value,
          this->one_to_ten.size(),
          compare_three_way{},
          counted_in_one_to_ten
      );
      EXPECT_LE(element_access_count, max_element_accesses);
    }
  }
  {
    SCOPED_TRACE("elements in container");
    for (index_type<TypeParam> element_location = 0; element_location < this->one_to_ten.size(); ++element_location) {
      auto const& value = counted_in_one_to_ten(element_location);
      element_access_count = 0U;
      std::ignore = lower_bound_index<TypeParam, TypeParam, index_type<TypeParam>>(
          value,
          this->one_to_ten.size(),
          compare_three_way{},
          counted_in_one_to_ten
      );
      EXPECT_LE(element_access_count, max_element_accesses);
    }
  }
}

}  // namespace
