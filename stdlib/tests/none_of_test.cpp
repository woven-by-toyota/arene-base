// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "iterator_types.hpp"
#include "stdlib/include/algorithm"
#include "stdlib/include/cstdint"
#include "stdlib/include/initializer_list"
#include "stdlib/include/iterator"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/iterator_types.hpp"

namespace {

struct less_than_five {
  constexpr auto operator()(std::uint32_t val) const noexcept -> bool { return val < 5U; }
};

struct throwing_greater_than_ten {
  constexpr auto operator()(std::uint32_t val) const noexcept(false) -> bool { return val > 10U; }
};

constexpr std::initializer_list<std::uint32_t> large_data{100, 99, 52323, 200, 11};

constexpr std::initializer_list<std::uint32_t> small_data{0, 1, 2, 3, 4, 1, 2, 3, 4, 2, 1, 2, 1};

constexpr std::initializer_list<std::uint32_t> mixed_data{10, 2, 30, 4, 5, 20};

/// @test An empty range returns true
TEST(NoneOf, EmptyRangeReturnsTrue) {
  std::uint32_t const dummy{2};

  auto const result = std::none_of(&dummy, &dummy, less_than_five{});
  auto const result2 = std::none_of(&dummy, &dummy, throwing_greater_than_ten{});

  ASSERT_TRUE(result);
  ASSERT_TRUE(result2);
}

/// @test a range where the predicate always returns @c true yields @c false
TEST(NoneOf, RangeWhereEverythingPassesReturnsFalse) {
  auto const result = std::none_of(std::begin(small_data), std::end(small_data), less_than_five{});

  ASSERT_FALSE(result);
}

/// @test a range where the predicate always returns @c true yields @c false
TEST(NoneOf, RangeWhereEverythingPassesReturnsFalseDifferentPredicate) {
  auto const result = std::none_of(std::begin(large_data), std::end(large_data), throwing_greater_than_ten{});

  ASSERT_FALSE(result);
}

/// @test a range where the predicate always returns @c false yields @c true
TEST(NoneOf, RangeWhereNothingPassesReturnsTrue) {
  auto const result = std::none_of(std::begin(large_data), std::end(large_data), less_than_five{});

  ASSERT_TRUE(result);
}

/// @test a range where the predicate always returns @c false yields @c true
TEST(NoneOf, RangeWhereNothingPassesReturnsTrueDifferentPredicate) {
  auto const result = std::none_of(std::begin(small_data), std::end(small_data), throwing_greater_than_ten{});

  ASSERT_TRUE(result);
}

/// @test a range where the predicate returns different values returns @c false
TEST(NoneOf, ReturnsFalseIfTheresAnEntryThatPasses) {
  auto const result = std::none_of(std::begin(mixed_data), std::end(mixed_data), less_than_five{});

  ASSERT_FALSE(result);
}

/// @test a range where the predicate returns different values returns @c false
TEST(NoneOf, ReturnsFalseIfTheresAnEntryThatPassesDifferentPredicate) {
  auto const result = std::none_of(std::begin(mixed_data), std::end(mixed_data), throwing_greater_than_ten{});

  ASSERT_FALSE(result);
}

/// @test the predicate is invoked up to the first passing value
TEST(NoneOf, ThePredicateIsInvokedUpToTheFirstPassingValue) {
  auto const linear_data = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

  class is_greater_than_value {
    std::uint32_t& count_;
    std::int32_t value_;

   public:
    explicit is_greater_than_value(std::uint32_t& counter, std::int32_t val)
        : count_(counter),
          value_(val) {}

    auto operator()(std::int32_t val_to_check) -> bool {
      ++count_;
      return val_to_check > value_;
    }
  };

  std::uint32_t count{0};

  bool result = std::none_of(std::begin(linear_data), std::end(linear_data), is_greater_than_value{count, 0});
  ASSERT_FALSE(result);
  ASSERT_EQ(count, 1);

  count = 0;
  result = std::none_of(std::begin(linear_data), std::end(linear_data), is_greater_than_value{count, 30});
  ASSERT_FALSE(result);
  ASSERT_EQ(count, 4);

  count = 0;
  result = std::none_of(std::begin(linear_data), std::end(linear_data), is_greater_than_value{count, 99});
  ASSERT_FALSE(result);
  ASSERT_EQ(count, 10);

  count = 0;
  result = std::none_of(std::begin(linear_data), std::end(linear_data), is_greater_than_value{count, 200});
  ASSERT_TRUE(result);
  ASSERT_EQ(count, 10);
}

/// @test none_of is noexcept if all operations are noexcept
TEST(NoneOf, NoexceptIfAllOperationsAreNoexcept) {
  static_assert(
      noexcept(
          std::none_of(std::declval<std::uint32_t const*>(), std::declval<std::uint32_t const*>(), less_than_five{})
      ),
      "Must be noexcept"
  );
  static_assert(
      noexcept(std::none_of(
          std::declval<testing::non_throwing_forward_iterator<std::uint32_t>>(),
          std::declval<testing::non_throwing_forward_iterator<std::uint32_t>>(),
          less_than_five{}
      )),
      "Must be noexcept"
  );
}

/// @test none_of is not noexcept if the predicate is not noexcept
TEST(NoneOf, NotNoexceptIfPredicateIsNotNoexcept) {
  static_assert(
      !noexcept(std::none_of(
          std::declval<std::uint32_t const*>(),
          std::declval<std::uint32_t const*>(),
          throwing_greater_than_ten{}
      )),
      "Must not be noexcept"
  );
}

/// @test none_of is not noexcept if the iterator is not noexcept
TEST(NoneOf, NotNoexceptIfIteratorIsNotNoexcept) {
  static_assert(
      !noexcept(std::none_of(
          std::declval<testing::throwing_forward_iterator<std::uint32_t>>(),
          std::declval<testing::throwing_forward_iterator<std::uint32_t>>(),
          less_than_five{}
      )),
      "Must not be noexcept"
  );
}

template <typename T>
class NoneOfInputIteratorTests : public testing::Test {};

using input_iterator_types = ::testing::Types<
    ::testing::demoted_iterator<std::uint32_t const*, std::input_iterator_tag>,
    ::testing::demoted_iterator<std::uint32_t const*, std::forward_iterator_tag>,
    ::testing::demoted_iterator<std::uint32_t const*, std::bidirectional_iterator_tag>,
    ::testing::demoted_iterator<std::uint32_t const*, std::random_access_iterator_tag>>;

TYPED_TEST_SUITE(NoneOfInputIteratorTests, input_iterator_types, );

/// @test Empty range returns @c true with any input iterator
TYPED_TEST(NoneOfInputIteratorTests, EmptyRangeReturnsTrue) {
  using test_iterator = TypeParam;

  ASSERT_TRUE(
      std::none_of(test_iterator{std::begin(small_data)}, test_iterator{std::begin(small_data)}, less_than_five{})
  );
  ASSERT_TRUE(std::none_of(
      test_iterator{std::begin(small_data)},
      test_iterator{std::begin(small_data)},
      throwing_greater_than_ten{}
  ));
}

/// @test a range where the predicate always returns @c true yields @c false
TYPED_TEST(NoneOfInputIteratorTests, RangeWhereEverythingPassesReturnsFalse) {
  using test_iterator = TypeParam;
  ASSERT_FALSE(
      std::none_of(test_iterator{std::begin(small_data)}, test_iterator{std::end(small_data)}, less_than_five{})
  );
}

/// @test a range where the predicate always returns @c false yields @c true
TYPED_TEST(NoneOfInputIteratorTests, RangeWhereEverythingFailsReturnsTrue) {
  using test_iterator = TypeParam;
  ASSERT_TRUE(std::none_of(test_iterator{std::begin(large_data)}, test_iterator{std::end(large_data)}, less_than_five{})
  );
}

/// @test a range where the predicate has different values yields @c false
TYPED_TEST(NoneOfInputIteratorTests, RangeWithMixedValuesReturnsFalse) {
  using test_iterator = TypeParam;
  ASSERT_FALSE(
      std::none_of(test_iterator{std::begin(mixed_data)}, test_iterator{std::end(mixed_data)}, less_than_five{})
  );
}

/// @test @c std::none_of should work and yield the correct result when dereferencing the iterator yields an rvalue
TEST(NoneOf, IteratorDereferenceYieldsRvalue) {
  struct element {
    std::uint32_t value;
  };

  struct basic_input_iterator {
    std::uint32_t const* underlying_pointer;

    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using reference = element;
    using value_type = element;
    using pointer = void;

    auto operator++() noexcept -> basic_input_iterator& {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      ++underlying_pointer;
      return *this;
    }
    auto operator*() const noexcept -> element { return element{*underlying_pointer}; }

    struct post_increment_result {
      std::uint32_t value;

      auto operator*() noexcept -> element { return element{value}; }
    };
    auto operator++(int) noexcept -> post_increment_result {
      post_increment_result res{*underlying_pointer};
      ++*this;
      return res;
    }

    auto operator==(basic_input_iterator const& other) const noexcept -> bool {
      return underlying_pointer == other.underlying_pointer;
    }
    auto operator!=(basic_input_iterator const& other) const noexcept -> bool {
      return underlying_pointer != other.underlying_pointer;
    }
  };

  struct predicate {
    auto operator()(element&& val) { return val.value < 5; }
  };

  ASSERT_FALSE(std::none_of(
      basic_input_iterator{std::begin(mixed_data)},
      basic_input_iterator{std::end(mixed_data)},
      predicate{}
  ));
  ASSERT_TRUE(std::none_of(
      basic_input_iterator{std::begin(large_data)},
      basic_input_iterator{std::end(large_data)},
      predicate{}
  ));
}

}  // namespace
