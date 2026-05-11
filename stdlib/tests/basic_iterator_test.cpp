// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/cstddef"
#include "stdlib/include/iterator"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"

// NOLINTBEGIN(readability-identifier-length)

namespace {

/// @test Check that @c iterator is default constructible
CONSTEXPR_TEST(BasicIterator, DefaultConstructible) { static_cast<void>(std::iterator<int, int>{}); }

/// @test Check that @c iterator can be used as a base class
TEST(BasicIterator, CanBeBaseClass) { ASSERT_FALSE(std::is_final_v<std::iterator<int, int>>); }

/// @test Check that there are no requirements for template parameters of @c iterator
TEST(BasicIterator, NoReqsForTparams) {
  auto f0 = [] {};
  auto f1 = [] {};
  auto f2 = [] {};
  auto f3 = [] {};
  auto f4 = [] {};

  using iterator_type = std::iterator<decltype(f0), decltype(f1), decltype(f2), decltype(f3), decltype(f4)>;

  static_cast<void>(iterator_type{});

  ASSERT_TRUE(std::is_same_v<decltype(f0), typename iterator_type::iterator_category>);
  ASSERT_TRUE(std::is_same_v<decltype(f1), typename iterator_type::value_type>);
  ASSERT_TRUE(std::is_same_v<decltype(f2), typename iterator_type::difference_type>);
  ASSERT_TRUE(std::is_same_v<decltype(f3), typename iterator_type::pointer>);
  ASSERT_TRUE(std::is_same_v<decltype(f4), typename iterator_type::reference>);
}

/// @test Check that the reference type is defaulted if not provided
TEST(BasicIterator, DefaultReferenceType) {
  auto f0 = [] {};
  auto f2 = [] {};
  auto f3 = [] {};

  using iterator_type = std::iterator<decltype(f0), int, decltype(f2), decltype(f3)>;

  static_cast<void>(iterator_type{});

  ASSERT_TRUE(std::is_same_v<decltype(f0), typename iterator_type::iterator_category>);
  ASSERT_TRUE(std::is_same_v<int, typename iterator_type::value_type>);
  ASSERT_TRUE(std::is_same_v<decltype(f2), typename iterator_type::difference_type>);
  ASSERT_TRUE(std::is_same_v<decltype(f3), typename iterator_type::pointer>);
  ASSERT_TRUE(std::is_same_v<int&, typename iterator_type::reference>);
}

/// @test Check that the reference and pointer types are defaulted if not provided
TEST(BasicIterator, DefaultReferenceAndPointerTypes) {
  auto f0 = [] {};
  auto f2 = [] {};

  using iterator_type = std::iterator<decltype(f0), int, decltype(f2)>;

  static_cast<void>(iterator_type{});

  ASSERT_TRUE(std::is_same_v<decltype(f0), typename iterator_type::iterator_category>);
  ASSERT_TRUE(std::is_same_v<int, typename iterator_type::value_type>);
  ASSERT_TRUE(std::is_same_v<decltype(f2), typename iterator_type::difference_type>);
  ASSERT_TRUE(std::is_same_v<int*, typename iterator_type::pointer>);
  ASSERT_TRUE(std::is_same_v<int&, typename iterator_type::reference>);
}

/// @test Check that the reference, pointer, and distance types are defaulted if not provided
TEST(BasicIterator, DefaultReferencePointerAndDistanceTypes) {
  auto f0 = [] {};

  using iterator_type = std::iterator<decltype(f0), int>;

  static_cast<void>(iterator_type{});

  ASSERT_TRUE(std::is_same_v<decltype(f0), typename iterator_type::iterator_category>);
  ASSERT_TRUE(std::is_same_v<int, typename iterator_type::value_type>);
  ASSERT_TRUE(std::is_same_v<std::ptrdiff_t, typename iterator_type::difference_type>);
  ASSERT_TRUE(std::is_same_v<int*, typename iterator_type::pointer>);
  ASSERT_TRUE(std::is_same_v<int&, typename iterator_type::reference>);
}

}  // namespace

// NOLINTEND(readability-identifier-length)
