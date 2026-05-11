// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/cstdint"
#include "stdlib/include/initializer_list"
#include "stdlib/include/iterator"
#include "stdlib/include/numeric"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/iterator_types.hpp"

namespace {

class local {
  std::uint32_t value_;

 public:
  constexpr local()
      : value_(0) {}

  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr local(std::uint32_t val)
      : value_(val) {}

  constexpr auto value() const noexcept -> std::uint32_t { return value_; }

  friend constexpr auto operator+(local const& old, local const& val) -> local {
    return local{old.value() + val.value()};
  }
  friend constexpr auto operator+(std::uint32_t old, local const& val) -> std::uint32_t { return old + val.value(); }
};

class op_counting_accumulator {
  std::uint32_t value_;
  std::uint32_t move_construct_count_;
  std::uint32_t move_assign_count_;
  std::uint32_t copy_construct_count_;
  std::uint32_t copy_assign_count_;
  std::uint32_t add_count_;

 public:
  constexpr explicit op_counting_accumulator(std::uint32_t initial) noexcept
      : value_(initial),
        move_construct_count_(0),
        move_assign_count_(0),
        copy_construct_count_(0),
        copy_assign_count_(0),
        add_count_(0) {}
  constexpr op_counting_accumulator(op_counting_accumulator&& other) noexcept
      : value_(other.value_),
        move_construct_count_(other.move_construct_count_ + 1),
        move_assign_count_(other.move_assign_count_),
        copy_construct_count_(other.copy_construct_count_),
        copy_assign_count_(other.copy_assign_count_),
        add_count_(other.add_count_) {
    other.value_ = 0;
    other.move_construct_count_ = 0;
    other.move_assign_count_ = 0;
    other.add_count_ = 0;
  }

  constexpr op_counting_accumulator(op_counting_accumulator const& other) noexcept
      : value_(other.value_),
        move_construct_count_(other.move_construct_count_),
        move_assign_count_(other.move_assign_count_),
        copy_construct_count_(other.copy_construct_count_ + 1),
        copy_assign_count_(other.copy_assign_count_),
        add_count_(other.add_count_) {}

  constexpr auto operator=(op_counting_accumulator&& other) noexcept -> op_counting_accumulator& {
    value_ = other.value_;
    move_construct_count_ = other.move_construct_count_;
    move_assign_count_ = other.move_assign_count_ + 1;
    copy_construct_count_ = other.copy_construct_count_;
    copy_assign_count_ = other.copy_assign_count_;
    add_count_ = other.add_count_;
    other.value_ = 0;
    other.move_construct_count_ = 0;
    other.move_assign_count_ = 0;
    other.copy_construct_count_ = 0;
    other.copy_assign_count_ = 0;
    other.add_count_ = 0;
    return *this;
  }
  constexpr auto operator=(op_counting_accumulator const& other) noexcept -> op_counting_accumulator& {
    value_ = other.value_;
    move_construct_count_ = other.move_construct_count_;
    move_assign_count_ = other.move_assign_count_;
    copy_construct_count_ = other.copy_construct_count_;
    copy_assign_count_ = other.copy_assign_count_ + 1;
    add_count_ = other.add_count_;
    return *this;
  }

  ~op_counting_accumulator() = default;

  constexpr auto value() const noexcept -> std::uint32_t { return value_; }
  constexpr auto move_constructs() const noexcept -> std::uint32_t { return move_construct_count_; }
  constexpr auto move_assigns() const noexcept -> std::uint32_t { return move_assign_count_; }
  constexpr auto copy_constructs() const noexcept -> std::uint32_t { return copy_construct_count_; }
  constexpr auto copy_assigns() const noexcept -> std::uint32_t { return copy_assign_count_; }
  constexpr auto adds() const noexcept -> std::uint32_t { return add_count_; }

  friend constexpr auto operator+(op_counting_accumulator const& old, local const& val) -> op_counting_accumulator {
    op_counting_accumulator res{old};
    res.value_ += val.value();
    ++res.add_count_;
    return res;
  }
};

/// @test Accumulate has the same return type as the initial value
TEST(Accumulate, AccumulateHasSameReturnTypeAsInit) {
  testing::StaticAssertTypeEq<
      std::uint32_t,
      decltype(std::accumulate(
          std::declval<std::uint32_t*>(),
          std::declval<std::uint32_t*>(),
          std::declval<std::uint32_t>()
      ))>();
  testing::StaticAssertTypeEq<
      local,
      decltype(std::accumulate(std::declval<local*>(), std::declval<local*>(), std::declval<local>()))>();
  testing::StaticAssertTypeEq<
      local,
      decltype(std::accumulate(std::declval<std::uint32_t*>(), std::declval<std::uint32_t*>(), std::declval<local>()))>(
  );
  testing::StaticAssertTypeEq<
      std::uint32_t,
      decltype(std::accumulate(std::declval<local*>(), std::declval<local*>(), std::declval<std::uint32_t>()))>();
}

/// @test Accumulate returns the initial value for an empty range
TEST(Accumulate, AccumulateWithAnEmptyRangeReturnsInit) {
  local const dummy{};
  constexpr std::uint32_t init{1234U};

  auto const result = std::accumulate(&dummy, &dummy, init);

  ASSERT_EQ(result, init);
}

/// @test Accumulate returns the sum of the initial value and the elements of the range
TEST(Accumulate, AccumulateWithANonEmptyRangeReturnsInitPlusElements) {
  std::initializer_list<std::uint32_t> range{10, 20, 100, 200};
  constexpr std::uint32_t init{1234U};

  auto const result = std::accumulate(std::begin(range), std::end(range), init);

  ASSERT_EQ(result, init + range.begin()[0] + range.begin()[1] + range.begin()[2] + range.begin()[3]);
}

/// @test Accumulate returns the sum of the initial value and the elements of the range, when they are different types
TEST(Accumulate, AccumulateWithANonEmptyRangeAndDifferentAccumulatorReturnsInitPlusElements) {
  std::initializer_list<local> const range{10, 20, 100, 200};
  constexpr std::uint32_t init{1234U};

  auto const result = std::accumulate(std::begin(range), std::end(range), init);

  ASSERT_EQ(
      result,
      init + range.begin()[0].value() + range.begin()[1].value() + range.begin()[2].value() + range.begin()[3].value()
  );
}

/// @test Accumulate returns the sum of the initial value and the elements of the range, when they are class types
TEST(Accumulate, AccumulateWithANonEmptyRangeAndClassTypes) {
  std::initializer_list<local> const range{10, 20, 100, 200};
  constexpr local init{1234U};

  auto const result = std::accumulate(std::begin(range), std::end(range), init);

  ASSERT_EQ(
      result.value(),
      init.value() + range.begin()[0].value() + range.begin()[1].value() + range.begin()[2].value() +
          range.begin()[3].value()
  );
}

/// @test Check that @c accumulate does one add and one move-assignment per element, as well as one
/// move-constructions of the accumulator.
TEST(Accumulate, AccumulateDoesExactlyOneAddAndMovePerElement) {
  std::initializer_list<local> const range{100, 400, 900, 2000000, 12345678, 42};
  constexpr std::uint32_t init{98765U};

  auto const count = range.size();

  auto const result = std::accumulate(std::begin(range), std::end(range), op_counting_accumulator(init));

  ASSERT_EQ(
      result.value(),
      init + range.begin()[0].value() + range.begin()[1].value() + range.begin()[2].value() + range.begin()[3].value() +
          range.begin()[4].value() + range.begin()[5].value()
  );

  ASSERT_EQ(result.adds(), count);
  ASSERT_EQ(result.move_assigns(), count);
  ASSERT_EQ(result.copy_assigns(), 0);
  ASSERT_EQ(result.copy_constructs(), count);
  ASSERT_EQ(result.move_constructs(), 1);
}

class sequence_tracker {
  std::uint32_t& value_;

 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  sequence_tracker(std::uint32_t& value)
      : value_(value) {}

  struct counter {
    std::uint32_t op_count;
  };

  friend constexpr auto operator+(counter acc, sequence_tracker& element) -> counter {
    ++acc.op_count;

    if (element.value_ != 0) {
      MINITEST_PANIC("this should never be reached");
    } else {
      element.value_ = acc.op_count;
    }

    return acc;
  }
};

/// @test Check that the elements are visited exactly once, in the order they are in the range
TEST(Accumulate, ElementsVisitedInSequence) {
  constexpr std::uint32_t count = 6;
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  std::uint32_t tracking_values[count] = {};
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  sequence_tracker range[count] = {
      tracking_values[0],
      tracking_values[1],
      tracking_values[2],
      tracking_values[3],
      tracking_values[4],
      tracking_values[5]
  };
  auto const result = std::accumulate(std::begin(range), std::end(range), sequence_tracker::counter{0U});

  ASSERT_EQ(result.op_count, count);

  std::uint32_t expected = 1;
  for (auto const& element : tracking_values) {
    ASSERT_EQ(element, expected);
    ++expected;
  }
}

/// @test Accumulate is noexcept if all operations noexcept
TEST(Accumulate, NoexceptIfAllOperationsNoexcept) {
  static_assert(
      noexcept(std::accumulate(std::declval<std::int32_t*>(), std::declval<std::int32_t*>(), std::int32_t{})),
      "Must be noexcept"
  );
}

/// @test Accumulate is not noexcept if not all operations noexcept
TEST(Accumulate, NotNoexceptIfNotAllOperationsNoexcept) {
  struct not_noexcept_accumulator {
    auto operator+(std::int32_t) const noexcept(false) -> not_noexcept_accumulator { return *this; }
  };

  static_assert(
      !noexcept(
          std::accumulate(std::declval<std::int32_t*>(), std::declval<std::int32_t*>(), not_noexcept_accumulator{})
      ),
      "Must not be noexcept"
  );
}

struct value_type {
  std::uint32_t val;
};

struct element_type {
  std::uint32_t val;
};

class counting_binary_op {
  std::uint32_t& op_count_;

 public:
  static constexpr std::uint32_t shift{1000U};
  explicit counting_binary_op(std::uint32_t& counter) noexcept
      : op_count_{counter} {}

  class convertible {
    std::uint32_t val_;

   public:
    explicit convertible(std::uint32_t value) noexcept
        : val_(value) {}

    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    operator value_type() const noexcept { return {val_}; }
  };

  auto operator()(value_type const& lhs, element_type const& rhs) const noexcept -> convertible {
    ++op_count_;
    return convertible{lhs.val + rhs.val + shift};
  }
};

/// @test Ensure Accumulate with a binary operator has the right signature
TEST(Accumulate, InitialElementAndBinaryOperatorHasRightSignature) {
  testing::StaticAssertTypeEq<
      decltype(std::accumulate(
          std::declval<element_type const*>(),
          std::declval<element_type const*>(),
          std::declval<value_type>(),
          std::declval<counting_binary_op>()
      )),
      value_type>();
}

/// @test Ensure Accumulate with a binary operator returns the correct value
TEST(Accumulate, InitialElementAndBinaryOperatorHasRightResult) {
  constexpr std::uint32_t val1 = 6;
  constexpr std::uint32_t val2 = 3;
  constexpr std::uint32_t val3 = 1;
  constexpr std::uint32_t val4 = 12331;
  constexpr std::uint32_t val5 = 2183173;
  constexpr std::uint32_t val6 = 123;

  std::initializer_list<element_type> const data{
      element_type{val1},
      element_type{val2},
      element_type{val3},
      element_type{val4},
      element_type{val5},
      element_type{val6}
  };

  std::uint32_t op_count{0};
  constexpr std::uint32_t initial_val{42};

  auto const result =
      std::accumulate(std::begin(data), std::end(data), value_type{initial_val}, counting_binary_op{op_count});

  ASSERT_EQ(result.val, val1 + val2 + val3 + val4 + val5 + val6 + initial_val + op_count * counting_binary_op::shift);
}

/// @test Ensure Accumulate with a binary operator and empty range returns initial value
TEST(Accumulate, BinaryOpWithEmptyRangeReturnsInitialValue) {
  std::initializer_list<element_type> const data{};

  std::uint32_t op_count{0};
  constexpr std::uint32_t initial_val{42};

  auto const result =
      std::accumulate(std::begin(data), std::end(data), value_type{initial_val}, counting_binary_op{op_count});

  ASSERT_EQ(result.val, initial_val);
}

/// @test Ensure Accumulate executes the supplied binary operator the right number of times
TEST(Accumulate, InitialElementAndBinaryOperatorExecutesOpCorrectNumberOfTimes) {
  constexpr std::uint32_t val1 = 1237186;
  std::initializer_list<element_type> const data{
      element_type{val1},
      element_type{val1},
      element_type{val1},
      element_type{val1},
      element_type{val1},
      element_type{val1},
      element_type{val1}
  };

  std::uint32_t op_count{0};

  static_cast<void>(std::accumulate(std::begin(data), std::end(data), value_type{0}, counting_binary_op{op_count}));

  ASSERT_EQ(op_count, data.size());
}

template <typename T>
class AccumulateInputIteratorTests : public testing::Test {};

using iterator_tag_types = ::testing::Types<
    std::input_iterator_tag,
    std::forward_iterator_tag,
    std::bidirectional_iterator_tag,
    std::random_access_iterator_tag>;

TYPED_TEST_SUITE(AccumulateInputIteratorTests, iterator_tag_types, );

/// @test Ensure Accumulate with a binary operator returns the correct value across different iterator types
TYPED_TEST(AccumulateInputIteratorTests, InitialElementAndBinaryOperatorHasRightResult) {
  constexpr std::uint32_t val1 = 6;
  constexpr std::uint32_t val2 = 3;
  constexpr std::uint32_t val3 = 1;
  constexpr std::uint32_t val4 = 12331;
  constexpr std::uint32_t val5 = 2183173;
  constexpr std::uint32_t val6 = 123;

  std::initializer_list<element_type> const data{
      element_type{val1},
      element_type{val2},
      element_type{val3},
      element_type{val4},
      element_type{val5},
      element_type{val6}
  };

  std::uint32_t op_count{0};
  constexpr std::uint32_t initial_val{42};

  using iterator_type = ::testing::demoted_iterator<element_type const*, TypeParam>;

  auto const result = std::accumulate(
      iterator_type{std::begin(data)},
      iterator_type{std::end(data)},
      value_type{initial_val},
      counting_binary_op{op_count}
  );

  ASSERT_EQ(result.val, val1 + val2 + val3 + val4 + val5 + val6 + initial_val + op_count * counting_binary_op::shift);
}

/// @test Accumulate returns the sum of the initial value and the elements of the range across different iterator types
TYPED_TEST(AccumulateInputIteratorTests, AccumulateWithANonEmptyRangeReturnsInitPlusElements) {
  std::initializer_list<std::uint32_t> range{10, 20, 100, 200};
  constexpr std::uint32_t init{1234U};

  using iterator_type = ::testing::demoted_iterator<std::uint32_t const*, TypeParam>;
  auto const result = std::accumulate(iterator_type{std::begin(range)}, iterator_type{std::end(range)}, init);

  ASSERT_EQ(result, init + range.begin()[0] + range.begin()[1] + range.begin()[2] + range.begin()[3]);
}

/// @test Accumulate returns the sum of the initial value and the elements of the range, when they are class types
/// across different iterator types
TYPED_TEST(AccumulateInputIteratorTests, AccumulateWithANonEmptyRangeAndClassTypes) {
  std::initializer_list<local> const range{10, 20, 100, 200};
  constexpr local init{1234U};

  using iterator_type = ::testing::demoted_iterator<local const*, TypeParam>;
  auto const result = std::accumulate(iterator_type{std::begin(range)}, iterator_type{std::end(range)}, init);

  ASSERT_EQ(
      result.value(),
      init.value() + range.begin()[0].value() + range.begin()[1].value() + range.begin()[2].value() +
          range.begin()[3].value()
  );
}

}  // namespace
