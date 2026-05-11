// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/inline_container/map_reference_iterator.hpp"

#include <gtest/gtest.h>

#include "arene/base/inline_container/map.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"

namespace {

/// @brief empty type to use as the mapped type
///
struct empty {};

/// @test inline_map_reference_iterator is a Cpp17BidirectionalIterator
///
TEST(InlineMapReferenceIterator, IsBidirectionalIterator) {
  using map_base_type = arene::base::inline_map_detail::inline_map_base<int, empty>;
  using iterator = arene::base::inline_map_reference_iterator<map_base_type, false>;
  using const_iterator = arene::base::inline_map_reference_iterator<map_base_type, true>;

  using ::arene::base::is_bidirectional_iterator_v;
  static_assert(is_bidirectional_iterator_v<iterator>, "should be bidirectional iterator");
  static_assert(is_bidirectional_iterator_v<const_iterator>, "should be bidirectional iterator");
}

/// @test const inline_map_reference_iterator is constructible from an
/// inline_map::iterator or inline_map::const_iterator
///
TEST(InlineMapReferenceIterator, ConstConstructibleFromConstOrNonConstBase) {
  using map_type = arene::base::inline_map<int, empty, 1>;
  using const_iterator = arene::base::inline_map_reference_iterator<typename map_type::inline_map_base_type, true>;

  static_assert(std::is_constructible<const_iterator, map_type::iterator>::value, "should be constructible");
  static_assert(std::is_constructible<const_iterator, map_type::const_iterator>::value, "should be constructible");
}

/// @test non const inline_map_reference_iterator is constructible from an
/// inline_map::const_iterator and not constructible from a inline_map::iterator
///
TEST(InlineMapReferenceIterator, NonConstConstructibleFromNonConstBase) {
  using map_type = arene::base::inline_map<int, empty, 1>;
  using iterator = arene::base::inline_map_reference_iterator<typename map_type::inline_map_base_type, false>;

  static_assert(std::is_constructible<iterator, map_type::iterator>::value, "should be constructible");
  static_assert(!std::is_constructible<iterator, map_type::const_iterator>::value, "should not be constructible");
}

/// @test dereference of inline_map_reference_iterator returns the same as
/// inline_map::iterator
///
TEST(InlineMapReferenceIterator, DereferenceIsSame) {
  using map_type = arene::base::inline_map<int, empty, 5>;
  using iterator = arene::base::inline_map_reference_iterator<typename map_type::inline_map_base_type, false>;
  using const_iterator = arene::base::inline_map_reference_iterator<typename map_type::inline_map_base_type, true>;

  auto map = map_type{std::pair<int, empty>{1, {}}};

  auto const it1 = map.begin();
  auto const it2 = iterator{it1};
  auto const it3 = const_iterator{it1};

  ASSERT_EQ(&*it1, &*it2);
  ASSERT_EQ(&*it1, &*it3);
}

/// @test dereference of inline_map_reference_iterator returns the same as
/// inline_map::iterator after both are incremented
///
TEST(InlineMapReferenceIterator, DereferenceIsSameAfterIncrement) {
  using map_type = arene::base::inline_map<int, empty, 5>;
  using iterator = arene::base::inline_map_reference_iterator<typename map_type::inline_map_base_type, false>;
  using const_iterator = arene::base::inline_map_reference_iterator<typename map_type::inline_map_base_type, true>;

  auto map = map_type{std::pair<int, empty>{1, {}}, {2, {}}};

  auto it1 = map.begin();
  auto it2 = iterator{it1};
  auto it3 = const_iterator{it1};

  ++it1;
  ++it2;
  ++it3;

  ASSERT_EQ(&*it1, &*it2);
  ASSERT_EQ(&*it1, &*it3);
}

/// @test dereference of inline_map_reference_iterator returns the same as
/// inline_map::iterator after both are decremented
///
TEST(InlineMapReferenceIterator, DereferenceIsSameAfterDecrement) {
  using map_type = arene::base::inline_map<int, empty, 5>;
  using iterator = arene::base::inline_map_reference_iterator<typename map_type::inline_map_base_type, false>;
  using const_iterator = arene::base::inline_map_reference_iterator<typename map_type::inline_map_base_type, true>;

  auto map = map_type{std::pair<int, empty>{1, {}}, {2, {}}};

  auto it1 = arene::base::next(map.begin());
  auto it2 = iterator{it1};
  auto it3 = const_iterator{it1};

  --it1;
  --it2;
  --it3;

  ASSERT_EQ(&*it1, &*it2);
  ASSERT_EQ(&*it1, &*it3);
}

/// @test inline_map_reference_iterator is equality comparable
///
TEST(InlineMapReferenceIterator, SameEquality) {
  using map_type = arene::base::inline_map<int, empty, 5>;
  using iterator = arene::base::inline_map_reference_iterator<typename map_type::inline_map_base_type, false>;

  auto map = map_type{std::pair<int, empty>{1, {}}, {2, {}}};

  auto ita1 = map.begin();
  auto ita2 = arene::base::next(map.begin());

  auto itb1 = iterator{ita1};
  auto itb2 = iterator{ita2};

  ASSERT_EQ((ita1 == ita1), (itb1 == itb1));
  ASSERT_EQ((ita1 == ita2), (itb1 == itb2));
  ASSERT_EQ((ita2 == ita1), (itb2 == itb1));

  ASSERT_EQ((ita1 != ita1), (itb1 != itb1));
  ASSERT_EQ((ita1 != ita2), (itb1 != itb2));
  ASSERT_EQ((ita2 != ita1), (itb2 != itb1));
}

/// @test const inline_map_reference_iterator is implicitly constructible from a
/// non-const inline_map_reference_iterator
///
TEST(InlineMapReferenceIterator, ConstImplicitlyConstructibleFromNonConst) {
  using map_type = arene::base::inline_map<int, empty, 5>;
  using iterator = arene::base::inline_map_reference_iterator<typename map_type::inline_map_base_type, false>;
  using const_iterator = arene::base::inline_map_reference_iterator<typename map_type::inline_map_base_type, true>;

  auto map = map_type{std::pair<int, empty>{1, {}}};

  auto const it1 = map.begin();
  auto const it2 = iterator{it1};
  auto const it3 = const_iterator{it2};

  ASSERT_EQ(it2, it3);

  static_assert(std::is_convertible<iterator, const_iterator>::value, "should be convertible");
}

/// @test all operations are @c noexcept
///
TEST(InlineMapReferenceIterator, EverythingNoexcept) {
  using iterator =
      arene::base::inline_map_reference_iterator<arene::base::inline_map_detail::inline_map_base<int, empty>, false>;
  using const_iterator =
      arene::base::inline_map_reference_iterator<arene::base::inline_map_detail::inline_map_base<int, empty>, true>;

  static_assert(std::is_nothrow_constructible<iterator>::value, "should be noexcept");
  static_assert(
      std::is_nothrow_constructible<iterator, arene::base::inline_map<int, empty, 5>::iterator>::value,
      "should be noexcept"
  );

  static_assert(std::is_nothrow_constructible<const_iterator>::value, "should be noexcept");
  static_assert(
      std::is_nothrow_constructible<const_iterator, arene::base::inline_map<int, empty, 5>::const_iterator>::value,
      "should be noexcept"
  );

  // NOLINTBEGIN(misc-redundant-expression)

  static_assert(noexcept(*std::declval<iterator>()), "should be noexcept");
  static_assert(noexcept(++std::declval<iterator>()), "should be noexcept");
  static_assert(noexcept(std::declval<iterator>()++), "shouldbe noexcept");
  static_assert(noexcept(--std::declval<iterator>()), "should be noexcept");
  static_assert(noexcept(std::declval<iterator>()--), "should be noexcept");
  static_assert(noexcept(std::declval<iterator>() == std::declval<iterator>()), "should be noexcept");
  static_assert(noexcept(std::declval<iterator>() != std::declval<iterator>()), "should be noexcept");

  static_assert(noexcept(*std::declval<const_iterator>()), "should be noexcept");
  static_assert(noexcept(++std::declval<const_iterator>()), "should be noexcept");
  static_assert(noexcept(std::declval<const_iterator>()++), "shouldbe noexcept");
  static_assert(noexcept(--std::declval<const_iterator>()), "should be noexcept");
  static_assert(noexcept(std::declval<const_iterator>()--), "should be noexcept");
  static_assert(noexcept(std::declval<const_iterator>() == std::declval<const_iterator>()), "should be noexcept");
  static_assert(noexcept(std::declval<const_iterator>() != std::declval<const_iterator>()), "should be noexcept");

  // NOLINTEND(misc-redundant-expression)
}

}  // namespace
