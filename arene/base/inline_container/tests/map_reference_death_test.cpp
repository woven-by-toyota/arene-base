// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/inline_container/map.hpp"
#include "arene/base/inline_container/map_reference.hpp"
#include "arene/base/iterator/distance.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/pair.hpp"

namespace {

template <bool IsNoexcept>
struct reverse_less_than {
  template <typename T, typename U>
  constexpr auto operator()(T const& lhs, U const& rhs) const noexcept(IsNoexcept) -> bool {
    return rhs < lhs;
  }

  friend constexpr auto operator==(reverse_less_than, reverse_less_than) noexcept -> bool { return true; }
};

class custom_key {
  int value_{};

 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr custom_key(int value)
      : value_{value} {}

  friend constexpr auto operator==(custom_key lhs, custom_key rhs) noexcept -> bool { return lhs.value_ == rhs.value_; }
  friend constexpr auto operator<(custom_key lhs, custom_key rhs) noexcept -> bool { return lhs.value_ < rhs.value_; }
};

template <std::size_t Capacity, bool IsNoexcept = true>
auto make_map_with_default_values() {
  return arene::base::inline_map<custom_key, int, Capacity, reverse_less_than<IsNoexcept>>{
      std::pair<custom_key, int>{3, {}},
      {2, {}},
      {1, {}},
  };
}

constexpr auto size_of_default_map = std::size_t{3};

template <std::size_t>
struct capacity {};
template <std::size_t>
struct param_value {};
template <bool>
struct compare_is_noexcept {};

template <class Capacity, class ParamValue, class CompareIsNoexcept>
struct test_case {};
template <std::size_t Capacity, std::size_t ParamValue, bool CompareIsNoexcept>
struct test_case<capacity<Capacity>, param_value<ParamValue>, compare_is_noexcept<CompareIsNoexcept>> {
  static constexpr auto capacity = Capacity;
  static constexpr auto param_value = ParamValue;
  static constexpr auto compare_is_noexcept = CompareIsNoexcept;
};

using MyTypes = ::testing::Types<
    test_case<capacity<5>, param_value<0>, compare_is_noexcept<true>>,
    test_case<capacity<5>, param_value<1>, compare_is_noexcept<true>>,
    test_case<capacity<5>, param_value<2>, compare_is_noexcept<true>>,
    test_case<capacity<5>, param_value<3>, compare_is_noexcept<true>>,
    test_case<capacity<5>, param_value<4>, compare_is_noexcept<true>>,
    test_case<capacity<10>, param_value<0>, compare_is_noexcept<true>>,
    test_case<capacity<10>, param_value<1>, compare_is_noexcept<true>>,
    test_case<capacity<10>, param_value<2>, compare_is_noexcept<true>>,
    test_case<capacity<10>, param_value<3>, compare_is_noexcept<true>>,
    test_case<capacity<10>, param_value<4>, compare_is_noexcept<true>>,
    test_case<capacity<5>, param_value<0>, compare_is_noexcept<false>>,
    test_case<capacity<5>, param_value<1>, compare_is_noexcept<false>>,
    test_case<capacity<5>, param_value<2>, compare_is_noexcept<false>>,
    test_case<capacity<5>, param_value<3>, compare_is_noexcept<false>>,
    test_case<capacity<5>, param_value<4>, compare_is_noexcept<false>>,
    test_case<capacity<10>, param_value<0>, compare_is_noexcept<false>>,
    test_case<capacity<10>, param_value<1>, compare_is_noexcept<false>>,
    test_case<capacity<10>, param_value<2>, compare_is_noexcept<false>>,
    test_case<capacity<10>, param_value<3>, compare_is_noexcept<false>>,
    test_case<capacity<10>, param_value<4>, compare_is_noexcept<false>>>;

template <class T>
struct MapReferenceDeathTest : ::testing::Test {};

TYPED_TEST_SUITE(MapReferenceDeathTest, MyTypes, );

template <class T>
struct MapReferenceCountedDeathTest : ::testing::Test {};

using integers_0_to_5 = ::testing::Types<
    std::integral_constant<int, 0>,
    std::integral_constant<int, 1>,
    std::integral_constant<int, 2>,
    std::integral_constant<int, 3>,
    std::integral_constant<int, 4>,
    std::integral_constant<int, 5>>;

TYPED_TEST_SUITE(MapReferenceCountedDeathTest, integers_0_to_5, );

template <bool IsNoexcept = true>
using map_ref = arene::base::inline_map_reference<custom_key, int, reverse_less_than<IsNoexcept>>;

/// @test `operator[]` behaves the same for an `inline_map_reference` as it does
/// for an `inline_map`, given that the map is at full capacity.
TYPED_TEST(MapReferenceCountedDeathTest, OperatorIndex) {
  auto map = make_map_with_default_values<size_of_default_map>();
  auto const key = custom_key{TypeParam()};

  using map_ref = ::map_ref<>;

  auto const func = [key](auto&& map_like) -> auto& { return map_like[key]; };

  if (map.contains(key)) {
    // if the key exists, operator[] should not modify the map
    auto const map_copy = map;

    auto& result1 = func(map);
    ASSERT_EQ(map, map_copy);
    auto& result2 = func(map_ref{map});
    ASSERT_EQ(map, map_copy);

    ::testing::StaticAssertTypeEq<decltype(&result1), decltype(&result2)>();
    ASSERT_EQ(&result1, &result2);
  } else {
    ASSERT_DEATH(func(map_ref{map}), "Precondition violation");
    ASSERT_DEATH(func(map), "Precondition violation");
  }
}

/// @test `insert` behaves the same for an `inline_map_reference` as it does
/// for an `inline_map`, given that the map is at full capacity.
TYPED_TEST(MapReferenceCountedDeathTest, Insert) {
  auto map = make_map_with_default_values<size_of_default_map>();
  auto const key = custom_key{TypeParam()};

  using map_ref = ::map_ref<>;

  auto const func = [key, value = 42](auto&& map_like) { return map_like.insert({key, value}); };

  if (map.contains(key)) {
    // if the key exists, insert should not modify the map
    auto const map_copy = map;

    auto const result1 = func(map);
    ASSERT_EQ(map, map_copy);
    auto const result2 = func(map_ref{map});
    ASSERT_EQ(map, map_copy);

    using map_reference_iterator = decltype(result2.first);
    ASSERT_EQ(map_reference_iterator{result1.first}, result2.first);
    ASSERT_EQ(result1.second, result2.second);
  } else {
    ASSERT_DEATH(func(map_ref{map}), "Precondition violation");
    ASSERT_DEATH(func(map), "Precondition violation");
  }
}

/// @test `insert_or_assign` behaves the same for an `inline_map_reference` as
/// it does for an `inline_map`, given that the map is at full capacity.
TYPED_TEST(MapReferenceCountedDeathTest, InsertOrAssign) {
  auto map = make_map_with_default_values<size_of_default_map>();
  auto const key = custom_key{TypeParam()};

  using map_ref = ::map_ref<>;

  auto const func = [key, value = 42](auto&& map_like) { return map_like.insert_or_assign(key, value); };

  if (map.contains(key)) {
    auto map2 = map;

    auto const result1 = func(map);
    auto const result2 = func(map_ref{map2});

    ASSERT_EQ(map, map2);
    ASSERT_EQ(*result1.first, *result2.first);
    ASSERT_EQ(result1.second, result2.second);
  } else {
    ASSERT_DEATH(func(map_ref{map}), "Precondition violation");
    ASSERT_DEATH(func(map), "Precondition violation");
  }
}

/// @test `erase` behaves the same for an `inline_map_reference` as it does for
/// an `inline_map`, given an iterator position argument
TYPED_TEST(MapReferenceDeathTest, ErasePosition) {
  auto map = make_map_with_default_values<TypeParam::capacity, TypeParam::compare_is_noexcept>();
  auto const key = custom_key{TypeParam::param_value};

  using map_ref = ::map_ref<TypeParam::compare_is_noexcept>;

  auto const func = [key](auto&& map_like) {
    auto const pos = map_like.find(key);
    return map_like.erase(pos);
  };

  if (map.contains(key)) {
    auto map2 = map;

    auto const result1 = func(map);
    auto const result2 = func(map_ref{map2});

    ASSERT_EQ(map, map2);
    ASSERT_EQ(arene::base::distance(map.begin(), result1), arene::base::distance(map_ref{map2}.begin(), result2));
    if (map.end() == result1) {
      ASSERT_EQ(map_ref{map2}.end(), result2);
    } else {
      ASSERT_EQ(*result1, *result2);
    }
  } else {
    // if the key does not exist, erase was called with `end()`
    ASSERT_DEATH(func(map_ref{map}), "Precondition violation");
    ASSERT_DEATH(func(map), "Precondition violation");
  }
}

}  // namespace
