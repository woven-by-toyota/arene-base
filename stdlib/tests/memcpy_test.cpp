// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/cstddef"
#include "stdlib/include/cstdint"
#include "stdlib/include/cstring"
#include "stdlib/include/limits"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "These tests don't perform arithmetic, so equality is OK even for floating point");

class wrapper {
  int value_;

 public:
  wrapper() = default;
  explicit constexpr wrapper(int value)
      : value_{value} {}

  static constexpr auto max_value() { return wrapper{std::numeric_limits<int>::max()}; }

  constexpr auto operator==(wrapper const& other) const { return value_ == other.value_; }
};

class class_with_data_and_padding {
  std::uint32_t data1_;
  std::uint16_t data2_;
  std::uint8_t data3_;
  std::uint16_t data4_;
  std::uint8_t data5_;
  std::uint64_t data6_;

 public:
  class_with_data_and_padding() = default;
  constexpr class_with_data_and_padding(
      std::uint32_t data1,
      std::uint16_t data2,
      std::uint8_t data3,
      std::uint16_t data4,
      std::uint8_t data5,
      std::uint64_t data6
  )
      : data1_{data1},
        data2_{data2},
        data3_{data3},
        data4_{data4},
        data5_{data5},
        data6_{data6} {}

  static constexpr auto max_value() {
    return class_with_data_and_padding{
        std::numeric_limits<std::uint32_t>::max(),
        std::numeric_limits<std::uint16_t>::max(),
        std::numeric_limits<std::uint8_t>::max(),
        std::numeric_limits<std::uint16_t>::max(),
        std::numeric_limits<std::uint8_t>::max(),
        std::numeric_limits<std::uint64_t>::max()
    };
  }

  constexpr auto operator==(class_with_data_and_padding const& other) const {
    return data1_ == other.data1_ && data2_ == other.data2_ && data3_ == other.data3_ && data4_ == other.data4_ &&
           data5_ == other.data5_ && data6_ == other.data6_;
  }
};

template <class T>
constexpr auto test_value_impl(std::true_type /* is_arithmetic */) -> T {
  return std::numeric_limits<T>::max();
}

template <class T>
constexpr auto test_value_impl(std::false_type /* is_arithmetic */) -> T {
  return T::max_value();
}

template <class T>
constexpr auto test_value() -> T {
  return test_value_impl<T>(std::is_arithmetic<T>{});
}

using arithmetic_types = arene::base::type_lists::concat_unique_t<
    ::testing::signed_integer_types,
    ::testing::unsigned_integer_types,
    ::testing::floating_point_types,
    ::testing::Types<wrapper, class_with_data_and_padding>>;

template <typename T>
class MemcpyTestSuite : public ::testing::Test {};

TYPED_TEST_SUITE(MemcpyTestSuite, arithmetic_types, );

/// @test @c memcpy copies a single value from source to destination
TYPED_TEST(MemcpyTestSuite, SingleValueIsCopied) {
  auto source = TypeParam{test_value<TypeParam>()};
  auto destination = TypeParam{};

  auto result = std::memcpy(&destination, &source, sizeof(source));
  ::testing::StaticAssertTypeEq<decltype(result), void*>();

  // Check that the data was copied correctly
  ASSERT_EQ(destination, test_value<TypeParam>());
  // Check that memcpy returns the destination pointer
  ASSERT_EQ(result, &destination);
}

/// @test @c memcpy copies an array of values from source to destination
TYPED_TEST(MemcpyTestSuite, ArrayValuesAreCopied) {
  // NOLINTBEGIN(hicpp-avoid-c-arrays)
  constexpr TypeParam source[] = {
      TypeParam{test_value<TypeParam>()},
      TypeParam{test_value<TypeParam>()},
      TypeParam{test_value<TypeParam>()},
      TypeParam{test_value<TypeParam>()},
      TypeParam{test_value<TypeParam>()}
  };
  TypeParam destination[] = {TypeParam{}, TypeParam{}, TypeParam{}, TypeParam{}, TypeParam{}};

  std::memcpy(&destination[0], &source[0], sizeof(source));

  // Check that the data was copied correctly
  for (size_t i = 0; i < 5; ++i) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    ASSERT_EQ(destination[i], source[i]);
  }
  // NOLINTEND(hicpp-avoid-c-arrays)
}

/// @test @c memcpy copies a subset of an array of values from source to destination
TYPED_TEST(MemcpyTestSuite, SubsetOfArrayValuesAreCopied) {
  // NOLINTBEGIN(hicpp-avoid-c-arrays)
  constexpr TypeParam source[] = {
      TypeParam{test_value<TypeParam>()},
      TypeParam{test_value<TypeParam>()},
      TypeParam{test_value<TypeParam>()},
      TypeParam{test_value<TypeParam>()},
      TypeParam{test_value<TypeParam>()}
  };
  TypeParam destination[] = {TypeParam{}, TypeParam{}, TypeParam{}, TypeParam{}, TypeParam{}};

  std::memcpy(&destination[1], &source[1], sizeof(TypeParam) * 3);

  // Check that the data was copied correctly
  // NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
  ASSERT_EQ(destination[0], TypeParam{});
  for (std::size_t i = 1; i < 4; ++i) {
    ASSERT_EQ(destination[i], source[i]);
  }
  ASSERT_EQ(destination[4], TypeParam{});
  // NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
  // NOLINTEND(hicpp-avoid-c-arrays)
}

/// @test @c memcpy with zero size does not change the destination
TYPED_TEST(MemcpyTestSuite, ZeroSizeDoesNotChangeDestination) {
  auto source = TypeParam{test_value<TypeParam>()};
  auto destination = TypeParam{};

  std::memcpy(&destination, &source, 0);

  // Check that the destination remains unchanged
  ASSERT_EQ(destination, TypeParam{});
}

/// @test @c memcpy can be used for type-punning
TEST(MemcpyTestSuite, TypePunning) {
  static_assert(std::numeric_limits<float>::is_iec559, "float must be IEC 559 compliant for this test");
  static_assert(sizeof(std::uint32_t) == sizeof(float), "int and float must have the same size for this test");
  std::uint32_t source_value = 0;
  float destination_value = 99.0F;

  std::memcpy(&destination_value, &source_value, sizeof(source_value));
  ASSERT_EQ(destination_value, 0.0F);
}

ARENE_IGNORE_END();
}  // namespace
