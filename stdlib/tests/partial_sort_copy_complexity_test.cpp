// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/array/array.hpp"
#include "arene/base/math/log2.hpp"
#include "arene/base/utility/make_subrange.hpp"
#include "stdlib/include/algorithm"
#include "stdlib/include/cstdint"
#include "testlibs/minitest/minitest.hpp"

namespace {

class counted_compare {
  std::uint32_t value_;

 public:
  counted_compare() noexcept
      : value_{} {}
  counted_compare(counted_compare const&) = default;
  counted_compare(counted_compare&&) = default;
  auto operator=(counted_compare const&) -> counted_compare& = default;
  auto operator=(counted_compare&&) -> counted_compare& = default;
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  counted_compare(std::uint32_t value) noexcept
      : value_(value) {}

  ~counted_compare() = default;

  auto get_value() const noexcept -> std::uint32_t { return value_; }

  friend auto operator<(counted_compare const& lhs, counted_compare const& rhs) noexcept -> bool {
    ++compare_count;
    return lhs.value_ < rhs.value_;
  }

  friend auto operator==(counted_compare const& lhs, counted_compare const& rhs) noexcept -> bool {
    return lhs.value_ == rhs.value_;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
  static std::uint64_t compare_count;
};

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
std::uint64_t counted_compare::compare_count{0};

class simple_rng {
  std::uint64_t state_;

 public:
  explicit simple_rng(std::uint64_t state) noexcept
      : state_(state) {}

  auto operator()() noexcept -> std::uint32_t {
    constexpr std::uint64_t multiplier{6364136223846793005};
    constexpr std::uint64_t increment{1442695040888963407};

    state_ = multiplier * state_ + increment;
    return static_cast<std::uint32_t>(state_ & 0XFFFFFFFFU);
  }
};

/// @test The number of comparisons should be approximately <c>(source range size) * log(min((source range
/// size),(target range size)))</c>. This test checks it is within the range <c>(source range size)</c> and <c>(source
/// range size) * (log2(min((source range size),(target range size))) + 1)</c>
TEST(PartialSortCopyComplexity, NLogKComplexity) {
  simple_rng rng{123456789};
  for (std::uint32_t i = 0; i < 10; ++i) {
    arene::base::array<counted_compare, SOURCE_SIZE> source{};
    arene::base::array<counted_compare, TARGET_SIZE> target{};

    for (auto& elem : source) {
      elem = rng();
    }

    ASSERT_FALSE(std::is_sorted(source.begin(), source.end()));

    counted_compare::compare_count = 0;

    auto result = std::partial_sort_copy(source.begin(), source.end(), target.begin(), target.end());

    auto const sorted_size{std::min(source.size(), target.size())};

    ASSERT_EQ(result, target.begin() + sorted_size);
    ASSERT_LE(counted_compare::compare_count, source.size() * (arene::base::log2(sorted_size) + 1));
    ASSERT_GE(counted_compare::compare_count, source.size());

    ASSERT_TRUE(std::is_sorted(target.begin(), target.begin() + sorted_size));

    std::sort(source.begin(), source.end());

    if (target.size() <= source.size()) {
      ASSERT_THAT(target, testing::ElementsAreArray(source.begin(), source.begin() + sorted_size));
    } else {
      ASSERT_THAT(source, testing::ElementsAreArray(target.begin(), target.begin() + sorted_size));
      for (auto& elem : arene::base::make_subrange(target.begin() + sorted_size, target.end())) {
        ASSERT_EQ(elem.get_value(), 0);
      }
    }
  }
}

/// @test The number of comparisons should be approximately <c>(source range size) * log(min((source range
/// size),(target range size)))</c> even if the source is already sorted
TEST(PartialSortCopyComplexity, NLogKComplexityAlreadySorted) {
  arene::base::array<counted_compare, SOURCE_SIZE> source{};
  arene::base::array<counted_compare, TARGET_SIZE> target{};

  std::uint32_t val{0};
  for (auto& elem : source) {
    elem = counted_compare{val};
    ++val;
  }

  ASSERT_TRUE(std::is_sorted(source.begin(), source.end()));

  counted_compare::compare_count = 0;

  auto result = std::partial_sort_copy(source.begin(), source.end(), target.begin(), target.end());

  auto const sorted_size{std::min(source.size(), target.size())};

  ASSERT_EQ(result, target.begin() + sorted_size);
  ASSERT_LE(counted_compare::compare_count, source.size() * (arene::base::log2(sorted_size) + 1));
  ASSERT_GE(counted_compare::compare_count, source.size());

  if (target.size() <= source.size()) {
    ASSERT_THAT(target, testing::ElementsAreArray(source.begin(), source.begin() + sorted_size));
  } else {
    ASSERT_THAT(source, testing::ElementsAreArray(target.begin(), target.begin() + sorted_size));
    for (auto& elem : arene::base::make_subrange(target.begin() + sorted_size, target.end())) {
      ASSERT_EQ(elem.get_value(), 0);
    }
  }
}

/// @test The number of comparisons should be approximately <c>(source range size) * log(min((source range
/// size),(target range size)))</c> even if the source is in reverse order
TEST(PartialSortCopyComplexity, NLogKComplexityReverseOrder) {
  arene::base::array<counted_compare, SOURCE_SIZE> source{};
  arene::base::array<counted_compare, TARGET_SIZE> target{};

  std::uint32_t val{SOURCE_SIZE};
  for (auto& elem : source) {
    elem = counted_compare{val};
    --val;
  }

  ASSERT_TRUE(std::is_sorted(source.rbegin(), source.rend()));

  counted_compare::compare_count = 0;

  auto result = std::partial_sort_copy(source.begin(), source.end(), target.begin(), target.end());

  auto const sorted_size{std::min(source.size(), target.size())};

  ASSERT_EQ(result, target.begin() + sorted_size);

  ASSERT_LE(counted_compare::compare_count, source.size() * (arene::base::log2(sorted_size) + 1));
  ASSERT_GE(counted_compare::compare_count, source.size());

  std::sort(source.begin(), source.end());

  if (target.size() <= source.size()) {
    ASSERT_THAT(target, testing::ElementsAreArray(source.begin(), source.begin() + sorted_size));
  } else {
    ASSERT_THAT(source, testing::ElementsAreArray(target.begin(), target.begin() + sorted_size));
    for (auto& elem : arene::base::make_subrange(target.begin() + sorted_size, target.end())) {
      ASSERT_EQ(elem.get_value(), 0);
    }
  }
}

/// @test The number of comparisons should be approximately <c>(source range size) * log(min((source range
/// size),(target range size)))</c> when using a custom comparator
TEST(PartialSortCopyComplexity, NLogKComplexityCustomComparator) {
  simple_rng rng{123456789};

  struct CountingComparator {
    std::uint32_t& count;

    auto operator()(counted_compare const& lhs, counted_compare const& rhs) noexcept -> bool {
      ++count;
      return rhs.get_value() < lhs.get_value();
    }
  };

  for (std::uint32_t i = 0; i < 10; ++i) {
    arene::base::array<counted_compare, SOURCE_SIZE> source{};
    arene::base::array<counted_compare, TARGET_SIZE> target{};

    for (auto& elem : source) {
      elem = rng();
    }

    std::uint32_t dummy_count{0};
    ASSERT_FALSE(std::is_sorted(source.begin(), source.end(), CountingComparator{dummy_count}));

    counted_compare::compare_count = 0;
    std::uint32_t count{0};

    auto result =
        std::partial_sort_copy(source.begin(), source.end(), target.begin(), target.end(), CountingComparator{count});

    auto const sorted_size{std::min(source.size(), target.size())};

    ASSERT_EQ(counted_compare::compare_count, 0);
    ASSERT_EQ(result, target.begin() + sorted_size);
    ASSERT_LE(count, source.size() * (arene::base::log2(sorted_size) + 1));
    ASSERT_GE(count, source.size());

    ASSERT_TRUE(std::is_sorted(target.begin(), target.begin() + sorted_size, CountingComparator{dummy_count}));

    std::sort(source.begin(), source.end(), CountingComparator{dummy_count});

    if (target.size() <= source.size()) {
      ASSERT_THAT(target, testing::ElementsAreArray(source.begin(), source.begin() + sorted_size));
    } else {
      ASSERT_THAT(source, testing::ElementsAreArray(target.begin(), target.begin() + sorted_size));
      for (auto& elem : arene::base::make_subrange(target.begin() + sorted_size, target.end())) {
        ASSERT_EQ(elem.get_value(), 0);
      }
    }
  }
}

}  // namespace
