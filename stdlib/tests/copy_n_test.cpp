// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/algorithm"
#include "stdlib/include/cstdint"
#include "stdlib/include/initializer_list"
#include "stdlib/include/iterator"
#include "stdlib/include/utility"
#include "stdlib/tests/iterator_types.hpp"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"
#include "testlibs/utilities/iterator_types.hpp"

namespace {
/// @test The return type of copy_n is the output iterator type
TEST(CopyN, TheReturnTypeIsTheOutputType) {
  testing::StaticAssertTypeEq<
      decltype(std::copy_n(std::declval<std::uint32_t*>(), std::declval<std::size_t>(), std::declval<std::uint64_t*>())
      ),
      std::uint64_t*>();
}

template <typename T>
class CopyNSizeTypeTests : public testing::Test {};

TYPED_TEST_SUITE(CopyNSizeTypeTests, ::testing::integral_types, );

/// @test If the size is zero, do nothing
TYPED_TEST(CopyNSizeTypeTests, IfSizeIsZeroDoNothing) {
  constexpr std::uint32_t count = 10;
  std::initializer_list<std::uint32_t> const source{1, 2, 3, 42, 5, 69, 7, 8, 9, 10};
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  std::uint32_t destination[count] = {};

  auto* const result = std::copy_n(std::begin(source), TypeParam{}, std::begin(destination));

  ASSERT_EQ(result, std::begin(destination));

  for (auto& element : destination) {
    ASSERT_EQ(element, 0U);
  }
}

/// @test If the size is negative, do nothing
TEST(CopyN, IfSizeIsNegativeDoNothing) {
  constexpr std::uint32_t count = 10;
  std::initializer_list<std::uint32_t> const source{23, 123, 412, 45, 72, 12, 1, 62, 3};
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  std::uint32_t destination[count] = {};

  auto* const result = std::copy_n(std::begin(source), -1, std::begin(destination));

  ASSERT_EQ(result, std::begin(destination));

  for (auto& element : destination) {
    ASSERT_EQ(element, 0U);
  }
}

/// @test If the size is greater than zero, copy that number of elements
TYPED_TEST(CopyNSizeTypeTests, IfSizeIsNotZeroCopySizeElements) {
  constexpr std::uint32_t count = 10;
  std::initializer_list<std::uint32_t> const source{23, 123456, 412, 45, 7200, 102, 12, 62, 3};

  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  std::uint32_t destination[count] = {};

  constexpr std::size_t size{4};

  constexpr auto expected_size = static_cast<std::size_t>(static_cast<TypeParam>(size));
  ASSERT_TRUE(expected_size <= source.size());
  ASSERT_TRUE(expected_size < count);

  auto* const result = std::copy_n(std::begin(source), static_cast<TypeParam>(size), std::begin(destination));

  ASSERT_EQ(result, &destination[expected_size]);

  for (std::size_t i{0U}; i < expected_size; ++i) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    ASSERT_EQ(destination[i], source.begin()[i]);
  }
  for (std::size_t i{expected_size}; i < count; ++i) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    ASSERT_EQ(destination[i], 0U);
  }
}

/// @test Allow the size parameter to be something that is convertible to an integral type
TYPED_TEST(CopyNSizeTypeTests, AllowNonIntegralSizeWithConversion) {
  constexpr std::uint32_t count = 10;
  std::initializer_list<std::uint32_t> const source{1, 2, 3, 42, 5, 69, 7, 8, 9, 10};
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  std::uint32_t destination[count] = {};

  constexpr std::uint32_t size = 4;

  class ConvertibleSize {
   public:
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    constexpr operator TypeParam() const { return static_cast<TypeParam>(size); }
  };

  constexpr auto expected_size = static_cast<std::size_t>(static_cast<TypeParam>(size));
  ASSERT_TRUE(expected_size <= source.size());
  ASSERT_TRUE(expected_size < count);

  auto* const result = std::copy_n(std::begin(source), ConvertibleSize{}, std::begin(destination));

  ASSERT_EQ(result, &destination[expected_size]);

  for (std::uint32_t i = 0; i < expected_size; ++i) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    ASSERT_EQ(destination[i], source.begin()[i]);
  }
  for (std::uint32_t i = expected_size; i < count; ++i) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    ASSERT_EQ(destination[i], 0U);
  }
}

template <typename T>
class CopyNFloatSizeConversionTests : public testing::Test {};

using float_types = ::testing::Types<float, double>;

TYPED_TEST_SUITE(CopyNFloatSizeConversionTests, float_types, );

/// @test Allow the size parameter to be a floating point value
TYPED_TEST(CopyNFloatSizeConversionTests, AllowFloatingPointSize) {
  constexpr std::uint32_t count = 10;
  std::initializer_list<std::uint32_t> const source{1, 2, 3, 42, 5, 69, 7, 8, 9, 10};
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  std::uint32_t destination[count] = {};

  constexpr std::uint32_t size = 4;

  constexpr auto expected_size = static_cast<std::size_t>(static_cast<TypeParam>(size));
  ASSERT_TRUE(expected_size <= source.size());
  ASSERT_TRUE(expected_size < count);

  auto const result = std::copy_n(std::begin(source), static_cast<TypeParam>(size), std::begin(destination));

  ASSERT_EQ(result, &destination[expected_size]);

  for (std::uint32_t i = 0; i < expected_size; ++i) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    ASSERT_EQ(destination[i], source.begin()[i]);
  }
  for (std::uint32_t i = expected_size; i < count; ++i) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    ASSERT_EQ(destination[i], 0U);
  }
}

struct local_nothrow_copy {};

/// @test If all operations are @c noexcept, so is @c copy_n
TEST(CopyN, CopyNIsNoexceptIfOperationsAreNoexcept) {
  static_assert(
      noexcept(std::copy_n(
          std::declval<local_nothrow_copy*>(),
          std::declval<std::size_t>(),
          std::declval<local_nothrow_copy*>()
      )),
      "Must be noexcept"
  );
  static_assert(
      noexcept(std::copy_n(
          std::declval<testing::non_throwing_forward_iterator<local_nothrow_copy>>(),
          std::declval<std::size_t>(),
          std::declval<testing::non_throwing_forward_iterator<local_nothrow_copy>>()
      )),
      "Must be noexcept"
  );
  static_assert(
      noexcept(std::copy_n(std::declval<std::int32_t*>(), std::declval<std::size_t>(), std::declval<std::int32_t*>())),
      "Must be noexcept"
  );
  static_assert(
      noexcept(std::copy_n(
          std::declval<testing::non_throwing_forward_iterator<std::uint32_t>>(),
          std::declval<std::size_t>(),
          std::declval<testing::non_throwing_forward_iterator<std::uint32_t>>()
      )),
      "Must be noexcept"
  );
}

/// @test If the source iterator operations are not @c noexcept, @c copy_n is not @c noexcept
TEST(CopyN, CopyNIsNotNoexceptIfSourceIteratorNotNoexcept) {
  static_assert(
      !noexcept(std::copy_n(
          std::declval<testing::throwing_forward_iterator<local_nothrow_copy>>(),
          std::declval<std::size_t>(),
          std::declval<testing::non_throwing_forward_iterator<local_nothrow_copy>>()
      )),
      "Must not be noexcept"
  );
}

/// @test If the destination iterator operations are not @c noexcept, @c copy_n is not @c noexcept
TEST(CopyN, CopyNIsNotNoexceptIfDestinationIteratorNotNoexcept) {
  static_assert(
      !noexcept(std::copy_n(
          std::declval<testing::non_throwing_forward_iterator<local_nothrow_copy>>(),
          std::declval<std::size_t>(),
          std::declval<testing::throwing_forward_iterator<local_nothrow_copy>>()
      )),
      "Must not be noexcept"
  );
}

/// @test If the conversion of the size to an integral type is not @c noexcept, @c copy_n is not @c noexcept
TEST(CopyN, CopyNIsNotNoexceptIfSizeConversionNotNoexcept) {
  struct throwing_size {
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    operator std::size_t() noexcept(false);
  };

  static_assert(
      !noexcept(std::copy_n(
          std::declval<testing::non_throwing_forward_iterator<local_nothrow_copy>>(),
          std::declval<throwing_size>(),
          std::declval<testing::non_throwing_forward_iterator<local_nothrow_copy>>()
      )),
      "Must not be noexcept"
  );
}

// NOLINTNEXTLINE(hicpp-special-member-functions)
struct local_throwing_copy {
  auto operator=(local_throwing_copy const&) noexcept(false) -> local_throwing_copy&;
};

/// @test If the assignment is not @c noexcept, @c copy_n is not @c noexcept
TEST(CopyN, CopyNIsNotNoexceptIfAssignmentNotNoexcept) {
  static_assert(
      !noexcept(std::copy_n(
          std::declval<testing::non_throwing_forward_iterator<local_throwing_copy>>(),
          std::declval<std::size_t>(),
          std::declval<testing::non_throwing_forward_iterator<local_throwing_copy>>()
      )),
      "Must not be noexcept"
  );
}

// NOLINTNEXTLINE(hicpp-special-member-functions)
class copy_counter {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
  static std::uint32_t counter;
  std::uint32_t value_;

 public:
  copy_counter()
      : value_(0) {}
  explicit copy_counter(std::uint32_t value)
      : value_(value) {}

  copy_counter(copy_counter const&) = default;

  auto operator=(copy_counter const& other) noexcept -> copy_counter& {
    ++counter;
    value_ = other.value_;
    return *this;
  }

  auto get_value() const noexcept -> std::uint32_t { return value_; }

  static void clear_count() noexcept { counter = 0; }
  static auto get_count() noexcept -> std::uint32_t { return counter; }
};

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
std::uint32_t copy_counter::counter{0};

/// @test Copy exactly the specified number of elements
TEST(CopyNTests, ExactlyOneCopyPerElement) {
  constexpr std::uint32_t count = 10;
  std::initializer_list<copy_counter> const source{
      copy_counter{23},
      copy_counter{123456},
      copy_counter{412},
      copy_counter{45},
      copy_counter{7200},
      copy_counter{102},
      copy_counter{12},
      copy_counter{62},
      copy_counter{3}
  };

  for (std::size_t size = 0; size < count; ++size) {
    // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
    copy_counter destination[count + 1] = {};

    copy_counter::clear_count();
    auto* const result = std::copy_n(std::begin(source), size, std::begin(destination));

    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    ASSERT_EQ(result, &destination[size]);
    ASSERT_EQ(copy_counter::get_count(), size);

    for (std::size_t i{0U}; i < size; ++i) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
      ASSERT_EQ(destination[i].get_value(), source.begin()[i].get_value());
    }
    for (std::size_t i{size}; i < count; ++i) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
      ASSERT_EQ(destination[i].get_value(), 0U);
    }
  }
}

template <typename T>
class CopyNInputIteratorTests : public testing::Test {};

using input_iterator_types = ::testing::Types<
    ::testing::demoted_iterator<copy_counter const*, std::input_iterator_tag>,
    ::testing::demoted_iterator<copy_counter const*, std::forward_iterator_tag>,
    ::testing::demoted_iterator<copy_counter const*, std::bidirectional_iterator_tag>,
    ::testing::demoted_iterator<copy_counter const*, std::random_access_iterator_tag>>;

TYPED_TEST_SUITE(CopyNInputIteratorTests, input_iterator_types, );

/// @test Copy exactly the specified number of elements with any input iterator
TYPED_TEST(CopyNInputIteratorTests, ExactlyOneCopyPerElement) {
  using source_iterator = TypeParam;
  constexpr std::uint32_t count = 10;
  std::initializer_list<copy_counter> const source{
      copy_counter{23},
      copy_counter{123456},
      copy_counter{412},
      copy_counter{45},
      copy_counter{7200},
      copy_counter{102},
      copy_counter{12},
      copy_counter{62},
      copy_counter{3}
  };

  for (std::size_t size = 0; size < count; ++size) {
    // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
    copy_counter destination[count + 1] = {};

    copy_counter::clear_count();
    auto* const result = std::copy_n(source_iterator{std::begin(source)}, size, std::begin(destination));

    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    ASSERT_EQ(result, &destination[size]);
    ASSERT_EQ(copy_counter::get_count(), size);

    for (std::size_t i{0U}; i < size; ++i) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
      ASSERT_EQ(destination[i].get_value(), source.begin()[i].get_value());
    }
    for (std::size_t i{size}; i < count; ++i) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
      ASSERT_EQ(destination[i].get_value(), 0U);
    }
  }
}

template <typename T>
class CopyNOutputIteratorTests : public testing::Test {};

using output_iterator_types = ::testing::Types<
    ::testing::demoted_iterator<copy_counter*, std::output_iterator_tag>,
    ::testing::demoted_iterator<copy_counter*, std::forward_iterator_tag>,
    ::testing::demoted_iterator<copy_counter*, std::bidirectional_iterator_tag>,
    ::testing::demoted_iterator<copy_counter*, std::random_access_iterator_tag>>;

TYPED_TEST_SUITE(CopyNOutputIteratorTests, output_iterator_types, );

/// @test Copy exactly the specified number of elements with any output iterator
TYPED_TEST(CopyNOutputIteratorTests, ExactlyOneCopyPerElement) {
  using target_iterator = TypeParam;
  constexpr std::uint32_t count = 10;
  std::initializer_list<copy_counter> const source{
      copy_counter{23},
      copy_counter{123456},
      copy_counter{412},
      copy_counter{45},
      copy_counter{7200},
      copy_counter{102},
      copy_counter{12},
      copy_counter{62},
      copy_counter{3}
  };

  for (std::size_t size = 0; size < count; ++size) {
    // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
    copy_counter destination[count + 1] = {};

    copy_counter::clear_count();
    target_iterator const result = std::copy_n(std::begin(source), size, target_iterator{std::begin(destination)});

    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    ASSERT_EQ(result, target_iterator{&destination[size]});
    ASSERT_EQ(copy_counter::get_count(), size);

    for (std::size_t i{0U}; i < size; ++i) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
      ASSERT_EQ(destination[i].get_value(), source.begin()[i].get_value());
    }
    for (std::size_t i{size}; i < count; ++i) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
      ASSERT_EQ(destination[i].get_value(), 0U);
    }
  }
}

}  // namespace
