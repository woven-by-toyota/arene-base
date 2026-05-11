// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/inline_container/map_reference.hpp"

#include <gtest/gtest.h>

#include "arene/base/inline_container/map.hpp"
#include "arene/base/iterator/distance.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/utility/as_const.hpp"

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
struct ConstMapReference : ::testing::Test {};

template <class T>
struct MapReference : ::testing::Test {};

TYPED_TEST_SUITE(ConstMapReference, MyTypes, );
TYPED_TEST_SUITE(MapReference, MyTypes, );

template <bool IsNoexcept = true>
using const_map_ref = arene::base::const_inline_map_reference<custom_key, int, reverse_less_than<IsNoexcept>>;

template <bool IsNoexcept = true>
using map_ref = arene::base::inline_map_reference<custom_key, int, reverse_less_than<IsNoexcept>>;

/// @test A `const_inline_map_reference` cannot be created from an rvalue reference
TYPED_TEST(ConstMapReference, NotConstructibleFromRvalueReferences) {
  auto map = make_map_with_default_values<TypeParam::capacity>();

  static_assert(
      !std::is_constructible<const_map_ref<>, decltype(map)&&>::value,
      "const_inline_map_reference should not be constructible from an rvalue reference"
  );
  static_assert(
      !std::is_constructible<const_map_ref<>, decltype(map) const&&>::value,
      "const_inline_map_reference should not be constructible from an rvalue reference"
  );
}

/// @test An `inline_map_reference` cannot be created from an rvalue reference
TYPED_TEST(MapReference, NotConstructibleFromRvalueReferences) {
  auto map = make_map_with_default_values<TypeParam::capacity>();

  static_assert(
      !std::is_constructible<map_ref<>, decltype(map)&&>::value,
      "inline_map_reference should not be constructible from an rvalue reference"
  );
  static_assert(
      !std::is_constructible<map_ref<>, decltype(map) const&&>::value,
      "inline_map_reference should not be constructible from an rvalue reference"
  );
}

/// @test A `const_inline_map_reference` can be constructed from an `inline_map_reference`
TYPED_TEST(ConstMapReference, ConstructibleFromInlineMapReference) {
  auto map = make_map_with_default_values<TypeParam::capacity, TypeParam::compare_is_noexcept>();

  using const_map_ref = ::const_map_ref<TypeParam::compare_is_noexcept>;
  using map_ref = ::map_ref<TypeParam::compare_is_noexcept>;

  auto const ref1 = map_ref{map};
  auto const ref2 = const_map_ref{ref1};

  ASSERT_EQ(ref1.begin(), ref2.begin());
  ASSERT_EQ(ref1.end(), ref2.end());
}

/// @test A `const_inline_map_reference` can be created from a non-const
/// reference and a const reference.
TYPED_TEST(ConstMapReference, ConstructibleFromConstOrNonConstReference) {
  auto map = make_map_with_default_values<TypeParam::capacity, TypeParam::compare_is_noexcept>();

  using const_map_ref = ::const_map_ref<TypeParam::compare_is_noexcept>;

  // constructed objects are equivalent
  ::testing::StaticAssertTypeEq<
      decltype(const_map_ref{map}.begin()),
      decltype(const_map_ref{arene::base::as_const(map)}.begin())>();
  ASSERT_EQ(const_map_ref{map}.begin(), const_map_ref{arene::base::as_const(map)}.begin());
}

/// @test An `inline_map_reference` can be created from a non-const
/// reference but not a const reference.
TYPED_TEST(MapReference, ConstructibleFromNonConstReference) {
  auto map = make_map_with_default_values<TypeParam::capacity, TypeParam::compare_is_noexcept>();

  static_assert(
      std::is_constructible<map_ref<TypeParam::compare_is_noexcept>, decltype(map)&>::value,
      "inline_map_reference should be constructible from an lvalue reference"
  );
  static_assert(
      !std::is_constructible<map_ref<TypeParam::compare_is_noexcept>, decltype(map) const&>::value,
      "inline_map_reference should not be constructible from an lvalue reference to const"
  );
}

/// @test A `const_inline_map_reference` returns the same value as an
/// `inline_map` for const member functions that take zero args.
TYPED_TEST(ConstMapReference, SameReturnForNullaryMemberFunctions) {
  auto const map = make_map_with_default_values<TypeParam::capacity, TypeParam::compare_is_noexcept>();

  using const_map_ref = ::const_map_ref<TypeParam::compare_is_noexcept>;
  using const_const_map_ref = ::const_map_ref<TypeParam::compare_is_noexcept> const;
  using const_iterator = typename const_map_ref::const_iterator;
  using const_reverse_iterator = typename const_map_ref::const_reverse_iterator;

  // Helper to construct an inline_map_reference<..>::const_reverse_iterator from an
  // inline_map<..>::const_reverse_iterator.
  auto to_const_reference_iterator = [](auto itr) { return const_reverse_iterator{const_iterator{itr.base()}}; };

  ::testing::StaticAssertTypeEq<decltype(map.key_comp()), decltype(const_map_ref{map}.key_comp())>();
  ::testing::StaticAssertTypeEq<decltype(map.size()), decltype(const_map_ref{map}.size())>();
  ASSERT_EQ(map.key_comp(), const_map_ref{map}.key_comp());
  ASSERT_EQ(map.size(), const_map_ref{map}.size());
  ASSERT_EQ(map.capacity(), const_map_ref{map}.capacity());
  ASSERT_EQ(map.max_size(), const_map_ref{map}.max_size());

  STATIC_ASSERT_TRUE(std::is_same<decltype(const_map_ref{map}.begin()), const_iterator>::value);
  STATIC_ASSERT_TRUE(std::is_same<decltype(const_map_ref{map}.end()), const_iterator>::value);
  ASSERT_EQ(const_iterator{map.begin()}, const_map_ref{map}.begin());
  ASSERT_EQ(const_iterator{map.end()}, const_map_ref{map}.end());

  STATIC_ASSERT_TRUE(std::is_same<decltype(const_const_map_ref{map}.begin()), const_iterator>::value);
  STATIC_ASSERT_TRUE(std::is_same<decltype(const_const_map_ref{map}.end()), const_iterator>::value);
  ASSERT_EQ(const_iterator{map.begin()}, const_const_map_ref{map}.begin());
  ASSERT_EQ(const_iterator{map.end()}, const_const_map_ref{map}.end());

  STATIC_ASSERT_TRUE(std::is_same<decltype(const_map_ref{map}.cbegin()), const_iterator>::value);
  STATIC_ASSERT_TRUE(std::is_same<decltype(const_map_ref{map}.cend()), const_iterator>::value);
  ASSERT_EQ(const_iterator{map.cbegin()}, const_map_ref{map}.cbegin());
  ASSERT_EQ(const_iterator{map.cend()}, const_map_ref{map}.cend());

  STATIC_ASSERT_TRUE(std::is_same<decltype(const_map_ref{map}.rbegin()), const_reverse_iterator>::value);
  STATIC_ASSERT_TRUE(std::is_same<decltype(const_map_ref{map}.rend()), const_reverse_iterator>::value);
  ASSERT_EQ(to_const_reference_iterator(map.rbegin()), const_map_ref{map}.rbegin());
  ASSERT_EQ(to_const_reference_iterator(map.rend()), const_map_ref{map}.rend());

  STATIC_ASSERT_TRUE(std::is_same<decltype(const_const_map_ref{map}.rbegin()), const_reverse_iterator>::value);
  STATIC_ASSERT_TRUE(std::is_same<decltype(const_const_map_ref{map}.rend()), const_reverse_iterator>::value);
  ASSERT_EQ(to_const_reference_iterator(map.rbegin()), const_const_map_ref{map}.rbegin());
  ASSERT_EQ(to_const_reference_iterator(map.rend()), const_const_map_ref{map}.rend());

  STATIC_ASSERT_TRUE(std::is_same<decltype(const_map_ref{map}.crbegin()), const_reverse_iterator>::value);
  STATIC_ASSERT_TRUE(std::is_same<decltype(const_map_ref{map}.crend()), const_reverse_iterator>::value);
  ASSERT_EQ(to_const_reference_iterator(map.crbegin()), const_map_ref{map}.crbegin());
  ASSERT_EQ(to_const_reference_iterator(map.crend()), const_map_ref{map}.crend());
}

/// @test An `inline_map_reference` returns the same value as an
/// `inline_map` for const member functions that take zero args.
TYPED_TEST(MapReference, SameReturnForNullaryMemberFunctions) {
  auto map = make_map_with_default_values<TypeParam::capacity, TypeParam::compare_is_noexcept>();

  using map_ref = ::map_ref<TypeParam::compare_is_noexcept>;
  using const_map_ref = ::map_ref<TypeParam::compare_is_noexcept> const;
  using iterator = typename map_ref::iterator;
  using const_iterator = typename map_ref::const_iterator;
  using reverse_iterator = typename map_ref::reverse_iterator;
  using const_reverse_iterator = typename const_map_ref::const_reverse_iterator;

  // Helpers to construct an inline_map_reference<..>::reverse_iterator from an inline_map<..>::reverse_iterator.
  auto to_reference_iterator = [](auto itr) { return reverse_iterator{iterator{itr.base()}}; };
  auto to_const_reference_iterator = [](auto itr) { return const_reverse_iterator{const_iterator{itr.base()}}; };

  ::testing::StaticAssertTypeEq<decltype(map.key_comp()), decltype(map_ref{map}.key_comp())>();
  ::testing::StaticAssertTypeEq<decltype(map.size()), decltype(map_ref{map}.size())>();
  ASSERT_EQ(map.key_comp(), map_ref{map}.key_comp());
  ASSERT_EQ(map.size(), map_ref{map}.size());

  STATIC_ASSERT_TRUE(std::is_same<decltype(map_ref{map}.begin()), iterator>::value);
  STATIC_ASSERT_TRUE(std::is_same<decltype(map_ref{map}.end()), iterator>::value);
  ASSERT_EQ(iterator{map.begin()}, map_ref{map}.begin());
  ASSERT_EQ(iterator{map.end()}, map_ref{map}.end());

  STATIC_ASSERT_TRUE(std::is_same<decltype(const_map_ref{map}.begin()), iterator>::value);
  STATIC_ASSERT_TRUE(std::is_same<decltype(const_map_ref{map}.end()), iterator>::value);
  ASSERT_EQ(iterator{map.begin()}, const_map_ref{map}.begin());
  ASSERT_EQ(iterator{map.end()}, const_map_ref{map}.end());

  STATIC_ASSERT_TRUE(std::is_same<decltype(map_ref{map}.cbegin()), const_iterator>::value);
  STATIC_ASSERT_TRUE(std::is_same<decltype(map_ref{map}.cend()), const_iterator>::value);
  ASSERT_EQ(const_iterator{map.cbegin()}, map_ref{map}.cbegin());
  ASSERT_EQ(const_iterator{map.cend()}, map_ref{map}.cend());

  STATIC_ASSERT_TRUE(std::is_same<decltype(map_ref{map}.rbegin()), reverse_iterator>::value);
  STATIC_ASSERT_TRUE(std::is_same<decltype(map_ref{map}.rend()), reverse_iterator>::value);
  ASSERT_EQ(to_reference_iterator(map.rbegin()), map_ref{map}.rbegin());
  ASSERT_EQ(to_reference_iterator(map.rend()), map_ref{map}.rend());

  STATIC_ASSERT_TRUE(std::is_same<decltype(const_map_ref{map}.rbegin()), reverse_iterator>::value);
  STATIC_ASSERT_TRUE(std::is_same<decltype(const_map_ref{map}.rend()), reverse_iterator>::value);
  ASSERT_EQ(to_reference_iterator(map.rbegin()), const_map_ref{map}.rbegin());
  ASSERT_EQ(to_reference_iterator(map.rend()), const_map_ref{map}.rend());

  STATIC_ASSERT_TRUE(std::is_same<decltype(map_ref{map}.crbegin()), const_reverse_iterator>::value);
  STATIC_ASSERT_TRUE(std::is_same<decltype(map_ref{map}.crend()), const_reverse_iterator>::value);
  ASSERT_EQ(to_const_reference_iterator(map.crbegin()), map_ref{map}.crbegin());
  ASSERT_EQ(to_const_reference_iterator(map.crend()), map_ref{map}.crend());
}

/// @test A `const_inline_map_reference` returns the same value as an
/// `inline_map` for const member functions that take one key arg and return a
/// value, even if the key is not in the map.
TYPED_TEST(ConstMapReference, SameReturnForUnaryMemberFunctions) {
  auto const map = make_map_with_default_values<TypeParam::capacity, TypeParam::compare_is_noexcept>();
  auto const key = custom_key{TypeParam::param_value};

  using const_map_ref = ::const_map_ref<TypeParam::compare_is_noexcept>;
  using const_iterator = typename const_map_ref::const_iterator;

  ::testing::StaticAssertTypeEq<const_iterator, decltype(const_map_ref{map}.find(key))>();
  ::testing::StaticAssertTypeEq<decltype(map.contains(key)), decltype(const_map_ref{map}.contains(key))>();
  ::testing::StaticAssertTypeEq<decltype(map.count(key)), decltype(const_map_ref{map}.count(key))>();
  ::testing::StaticAssertTypeEq<const_iterator, decltype(const_map_ref{map}.lower_bound(key))>();
  ::testing::StaticAssertTypeEq<const_iterator, decltype(const_map_ref{map}.upper_bound(key))>();
  ::testing::
      StaticAssertTypeEq<std::pair<const_iterator, const_iterator>, decltype(const_map_ref{map}.equal_range(key))>();

  ASSERT_EQ(const_iterator{map.find(key)}, const_map_ref{map}.find(key));
  ASSERT_EQ(map.contains(key), const_map_ref{map}.contains(key));
  ASSERT_EQ(map.count(key), const_map_ref{map}.count(key));
  ASSERT_EQ(const_iterator{map.lower_bound(key)}, const_map_ref{map}.lower_bound(key));
  ASSERT_EQ(const_iterator{map.upper_bound(key)}, const_map_ref{map}.upper_bound(key));
  ASSERT_EQ((std::pair<const_iterator, const_iterator>{map.equal_range(key)}), const_map_ref{map}.equal_range(key));
}

/// @test An `inline_map_reference` returns the same value as an
/// `inline_map` for const member functions that take one key arg and return a
/// value, even if the key is not in the map.
TYPED_TEST(MapReference, SameReturnForUnaryMemberFunctions) {
  auto map = make_map_with_default_values<TypeParam::capacity, TypeParam::compare_is_noexcept>();
  auto const key = custom_key{TypeParam::param_value};

  using map_ref = ::map_ref<TypeParam::compare_is_noexcept>;
  using iterator = typename map_ref::iterator;

  ::testing::StaticAssertTypeEq<iterator, decltype(map_ref{map}.find(key))>();
  ::testing::StaticAssertTypeEq<decltype(map.contains(key)), decltype(map_ref{map}.contains(key))>();
  ::testing::StaticAssertTypeEq<decltype(map.count(key)), decltype(map_ref{map}.count(key))>();
  ::testing::StaticAssertTypeEq<iterator, decltype(map_ref{map}.lower_bound(key))>();
  ::testing::StaticAssertTypeEq<iterator, decltype(map_ref{map}.upper_bound(key))>();
  ::testing::StaticAssertTypeEq<std::pair<iterator, iterator>, decltype(map_ref{map}.equal_range(key))>();

  ASSERT_EQ(iterator{map.find(key)}, map_ref{map}.find(key));
  ASSERT_EQ(map.contains(key), map_ref{map}.contains(key));
  ASSERT_EQ(map.count(key), map_ref{map}.count(key));
  ASSERT_EQ(iterator{map.lower_bound(key)}, map_ref{map}.lower_bound(key));
  ASSERT_EQ(iterator{map.upper_bound(key)}, map_ref{map}.upper_bound(key));
  ASSERT_EQ((std::pair<iterator, iterator>{map.equal_range(key)}), map_ref{map}.equal_range(key));
}

/// @test `operator[]` behaves the same for an `inline_map_reference` as it does
/// for an `inline_map`, given that the map has space for a new element.
TYPED_TEST(MapReference, OperatorIndex) {
  auto map = make_map_with_default_values<TypeParam::capacity, TypeParam::compare_is_noexcept>();
  auto const key = custom_key{TypeParam::param_value};

  using map_ref = ::map_ref<TypeParam::compare_is_noexcept>;

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
    // otherwise, the entry is created in the map
    auto map2 = map;

    auto& result1 = func(map);
    auto& result2 = func(map_ref{map2});

    ASSERT_EQ(map, map2);
    ::testing::StaticAssertTypeEq<decltype(result1), decltype(result2)>();
    ASSERT_EQ(result1, result2);
  }
}

/// @test `insert` behaves the same for an `inline_map_reference` as it does
/// for an `inline_map`, given that the map has space for a new element.
TYPED_TEST(MapReference, Insert) {
  auto map = make_map_with_default_values<TypeParam::capacity, TypeParam::compare_is_noexcept>();
  auto const key = custom_key{TypeParam::param_value};

  using map_ref = ::map_ref<TypeParam::compare_is_noexcept>;

  auto const func = [key, value = 42](auto&& map_like) { return map_like.insert({key, value}); };

  if (map.contains(key)) {
    // if the key exists, insert should not modify the map
    auto const map_copy = map;

    auto const result1 = func(map);
    ASSERT_EQ(map, map_copy);
    auto const result2 = func(map_ref{map});
    ASSERT_EQ(map, map_copy);

    using map_reference_iterator = decltype(result2.first);
    ::testing::StaticAssertTypeEq<map_reference_iterator, decltype(result2.first)>();
    ASSERT_EQ(map_reference_iterator{result1.first}, result2.first);
    ASSERT_EQ(result1.second, result2.second);
  } else {
    // otherwise, the entry is created in the map
    auto map2 = map;

    auto const result1 = func(map);
    auto const result2 = func(map_ref{map2});

    ASSERT_EQ(*result1.first, *result2.first);
    ASSERT_EQ(result1.second, result2.second);
  }
}

/// @test `insert_or_assign` behaves the same for an `inline_map_reference` as
/// it does for an `inline_map`, given that the map has space for a new element.
TYPED_TEST(MapReference, InsertOrAssign) {
  auto map = make_map_with_default_values<TypeParam::capacity, TypeParam::compare_is_noexcept>();
  auto const key = custom_key{TypeParam::param_value};

  using map_ref = ::map_ref<TypeParam::compare_is_noexcept>;

  auto const func = [key, value = 42](auto&& map_like) { return map_like.insert_or_assign(key, value); };

  auto map2 = map;

  auto const result1 = func(map);
  auto const result2 = func(map_ref{map2});

  ASSERT_EQ(map, map2);
  ASSERT_EQ(*result1.first, *result2.first);
  ASSERT_EQ(result1.second, result2.second);
}

/// @test `erase` behaves the same for an `inline_map_reference` as it does for
/// an `inline_map`, given a key argument
TYPED_TEST(MapReference, Erase) {
  auto map = make_map_with_default_values<TypeParam::capacity, TypeParam::compare_is_noexcept>();
  auto const key = custom_key{TypeParam::param_value};

  using map_ref = ::map_ref<TypeParam::compare_is_noexcept>;

  auto const func = [key](auto&& map_like) { return map_like.erase(key); };

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
    // if the key does not exist, erase should not modify the map
    auto const map_copy = map;

    auto const result1 = func(map);
    ASSERT_EQ(map, map_copy);
    auto const result2 = func(map_ref{map});
    ASSERT_EQ(map, map_copy);

    using map_reference_iterator = decltype(result2);
    ASSERT_EQ(map_reference_iterator{result1}, result2);
  }
}

/// @test `erase` behaves the same for an `inline_map_reference` as it does for
/// an `inline_map`, given iterator range arguments
TYPED_TEST(MapReference, EraseRange) {
  auto map = make_map_with_default_values<TypeParam::capacity, TypeParam::compare_is_noexcept>();
  auto const key = custom_key{TypeParam::param_value};

  using map_ref = ::map_ref<TypeParam::compare_is_noexcept>;

  auto const func = [key](auto&& map_like) {
    auto const last = map_like.find(key);
    return map_like.erase(map_like.begin(), last);
  };

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
}

/// @test `clear` behaves the same for an `inline_map_reference` as it does for
/// an `inline_map`
TYPED_TEST(MapReference, clear) {
  auto map = make_map_with_default_values<TypeParam::capacity, TypeParam::compare_is_noexcept>();
  auto map2 = map;

  using map_ref = ::map_ref<TypeParam::compare_is_noexcept>;

  map.clear();
  map_ref{map2}.clear();

  ASSERT_EQ(map, map2);
}

/// @test `empty` behaves the same for an `inline_map_reference` as it does for
/// an `inline_map`
TYPED_TEST(MapReference, empty) {
  auto map = make_map_with_default_values<TypeParam::capacity, TypeParam::compare_is_noexcept>();
  using map_ref = ::map_ref<TypeParam::compare_is_noexcept>;
  using const_map_ref = ::const_map_ref<TypeParam::compare_is_noexcept>;

  ASSERT_FALSE(map_ref{map}.empty());
  ASSERT_FALSE(const_map_ref{map}.empty());

  map.clear();

  ASSERT_TRUE(map_ref{map}.empty());
  ASSERT_TRUE(const_map_ref{map}.empty());
}

/// @test The `noexcept`-ness of member functions of
/// `const_inline_map_reference` match the `noexcept`-ness of corresponding
/// member functions of `inline_map`.
TYPED_TEST(ConstMapReference, MemberFunctionNoexceptNess) {
  using map_type = decltype(make_map_with_default_values<TypeParam::capacity, TypeParam::compare_is_noexcept>());
  using const_map_ref = ::const_map_ref<TypeParam::compare_is_noexcept>;

  using key_type = typename const_map_ref::key_type;

  static_assert(
      noexcept(std::declval<map_type const&>().size()) == noexcept(std::declval<const_map_ref&>().size()),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type const&>().empty()) == noexcept(std::declval<const_map_ref&>().empty()),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type const&>().begin()) == noexcept(std::declval<const_map_ref&>().begin()),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type const&>().end()) == noexcept(std::declval<const_map_ref&>().end()),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type const&>().cbegin()) == noexcept(std::declval<const_map_ref&>().cbegin()),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type const&>().cend()) == noexcept(std::declval<const_map_ref&>().cend()),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type const&>().rbegin()) == noexcept(std::declval<const_map_ref&>().rbegin()),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type const&>().rend()) == noexcept(std::declval<const_map_ref&>().rend()),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type const&>().crbegin()) == noexcept(std::declval<const_map_ref&>().crbegin()),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type const&>().crend()) == noexcept(std::declval<const_map_ref&>().crend()),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type const&>().key_comp()) == noexcept(std::declval<const_map_ref&>().key_comp()),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type const&>().find(std::declval<key_type>())) ==
          noexcept(std::declval<const_map_ref&>().find(std::declval<key_type>())),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type const&>().contains(std::declval<key_type>())) ==
          noexcept(std::declval<const_map_ref&>().contains(std::declval<key_type>())),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type const&>().count(std::declval<key_type>())) ==
          noexcept(std::declval<const_map_ref&>().count(std::declval<key_type>())),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type const&>().lower_bound(std::declval<key_type>())) ==
          noexcept(std::declval<const_map_ref&>().lower_bound(std::declval<key_type>())),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type const&>().upper_bound(std::declval<key_type>())) ==
          noexcept(std::declval<const_map_ref&>().upper_bound(std::declval<key_type>())),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type const&>().equal_range(std::declval<key_type>())) ==
          noexcept(std::declval<const_map_ref&>().equal_range(std::declval<key_type>())),
      "noexcept should match"
  );
}

/// @test The `noexcept`-ness of member functions of
/// `inline_map_reference` match the `noexcept`-ness of corresponding
/// member functions of `inline_map`.
TYPED_TEST(MapReference, MemberFunctionNoexceptNess) {
  using map_type = decltype(make_map_with_default_values<TypeParam::capacity, TypeParam::compare_is_noexcept>());
  using map_ref = ::map_ref<TypeParam::compare_is_noexcept>;

  using key_type = typename map_ref::key_type;
  using value_type = typename map_ref::value_type;
  using mapped_type = typename map_ref::mapped_type;

  static_assert(
      noexcept(std::declval<map_type&>().size()) == noexcept(std::declval<map_ref&>().size()),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type&>().empty()) == noexcept(std::declval<map_ref&>().empty()),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type&>().begin()) == noexcept(std::declval<map_ref&>().begin()),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type&>().end()) == noexcept(std::declval<map_ref&>().end()),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type const&>().cbegin()) == noexcept(std::declval<map_ref&>().cbegin()),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type const&>().cend()) == noexcept(std::declval<map_ref&>().cend()),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type const&>().rbegin()) == noexcept(std::declval<map_ref&>().rbegin()),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type const&>().rend()) == noexcept(std::declval<map_ref&>().rend()),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type const&>().crbegin()) == noexcept(std::declval<map_ref&>().crbegin()),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type const&>().crend()) == noexcept(std::declval<map_ref&>().crend()),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type&>().key_comp()) == noexcept(std::declval<map_ref&>().key_comp()),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type&>().find(std::declval<key_type>())) ==
          noexcept(std::declval<map_ref&>().find(std::declval<key_type>())),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type&>().contains(std::declval<key_type>())) ==
          noexcept(std::declval<map_ref&>().contains(std::declval<key_type>())),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type&>().count(std::declval<key_type>())) ==
          noexcept(std::declval<map_ref&>().count(std::declval<key_type>())),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type&>().lower_bound(std::declval<key_type>())) ==
          noexcept(std::declval<map_ref&>().lower_bound(std::declval<key_type>())),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type&>().upper_bound(std::declval<key_type>())) ==
          noexcept(std::declval<map_ref&>().upper_bound(std::declval<key_type>())),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type&>().equal_range(std::declval<key_type>())) ==
          noexcept(std::declval<map_ref&>().equal_range(std::declval<key_type>())),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type&>()[std::declval<key_type>()]) ==
          noexcept(std::declval<map_ref&>()[std::declval<key_type>()]),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type&>().insert(std::declval<value_type>())) ==
          noexcept(std::declval<map_ref&>().insert(std::declval<value_type>())),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type&>().insert_or_assign(std::declval<key_type>(), std::declval<mapped_type>())) ==
          noexcept(std::declval<map_ref&>().insert_or_assign(std::declval<key_type>(), std::declval<mapped_type>())),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type&>().erase(std::declval<key_type>())) ==
          noexcept(std::declval<map_ref&>().erase(std::declval<key_type>())),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type&>().erase(std::declval<typename map_type::const_iterator>())) ==
          noexcept(std::declval<map_ref&>().erase(std::declval<typename map_ref::const_iterator>())),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type&>().erase(
          std::declval<typename map_type::const_iterator>(),
          std::declval<typename map_type::const_iterator>()
      )) ==
          noexcept(std::declval<map_ref&>().erase(
              std::declval<typename map_ref::const_iterator>(),
              std::declval<typename map_ref::const_iterator>()
          )),
      "noexcept should match"
  );
  static_assert(
      noexcept(std::declval<map_type&>().clear()) == noexcept(std::declval<map_ref&>().clear()),
      "noexcept should match"
  );
}

/// @test member functions of @c inline_map_reference are invocable with a @c const qualified @c inline_map_reference
TYPED_TEST(MapReference, MemberFunctionsAreConstInvocable) {
  using map_reference = ::map_ref<TypeParam::compare_is_noexcept>;
  using const_iterator = typename map_reference::const_iterator;
  using key_type = typename map_reference::key_type;
  using mapped_type = typename map_reference::mapped_type;
  using value_type = typename map_reference::value_type;

  using arene::base::is_invocable_v;

  ASSERT_TRUE((is_invocable_v<decltype(&map_reference::find), map_reference const&, key_type const&>));
  ASSERT_TRUE((is_invocable_v<decltype(&map_reference::lower_bound), map_reference const&, key_type const&>));
  ASSERT_TRUE((is_invocable_v<decltype(&map_reference::upper_bound), map_reference const&, key_type const&>));
  ASSERT_TRUE((is_invocable_v<decltype(&map_reference::equal_range), map_reference const&, key_type const&>));
  ASSERT_TRUE((is_invocable_v<decltype(&map_reference::operator[]), map_reference const&, key_type const&>));
  ASSERT_TRUE((is_invocable_v<decltype(&map_reference::insert), map_reference const&, value_type const&>));
  ASSERT_TRUE((is_invocable_v<
               decltype(&map_reference::insert_or_assign),
               map_reference const&,
               key_type const&,
               mapped_type const&>));

  auto const erase = [](auto const& ref, auto&&... args) { return ref.erase(std::forward<decltype(args)>(args)...); };
  ASSERT_TRUE((is_invocable_v<decltype(erase), map_reference const&, key_type const&>));
  ASSERT_TRUE((is_invocable_v<decltype(erase), map_reference const&, const_iterator>));
  ASSERT_TRUE((is_invocable_v<decltype(erase), map_reference const&, const_iterator, const_iterator>));

  ASSERT_TRUE((is_invocable_v<decltype(&map_reference::clear), map_reference const&>));
  ASSERT_TRUE((is_invocable_v<decltype(&map_reference::begin), map_reference const&>));
  ASSERT_TRUE((is_invocable_v<decltype(&map_reference::end), map_reference const&>));
}

}  // namespace
