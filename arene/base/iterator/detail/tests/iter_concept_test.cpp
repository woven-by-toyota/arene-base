// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/detail/iter_concept.hpp"

#include <gtest/gtest.h>

#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"

namespace {
struct iterator_traits_has_concept_type {};

struct iterator_traits_and_type_have_concept_type {
  using value_type = int;
  using difference_type = std::ptrdiff_t;
  using pointer = int*;
  using reference = int&;
  using iterator_category = std::input_iterator_tag;
  using iterator_concept = std::forward_iterator_tag;
};

}  // namespace

template <>
class std::iterator_traits<iterator_traits_has_concept_type> {
 public:
  using value_type = int;
  using difference_type = std::ptrdiff_t;
  using pointer = int*;
  using reference = int&;
  using iterator_category = std::input_iterator_tag;
  using iterator_concept = std::random_access_iterator_tag;
};

template <>
class std::iterator_traits<iterator_traits_and_type_have_concept_type> {
 public:
  using value_type = int;
  using difference_type = std::ptrdiff_t;
  using pointer = int*;
  using reference = int&;
  using iterator_category = std::input_iterator_tag;
  using iterator_concept = std::random_access_iterator_tag;
};

namespace {

using ::arene::base::iterator_detail::iter_concept_t;

/// @test Given a pointer, `iter_concept_t<T*>` is @c std::random_access_iterator_tag
/// specialization of @c std::iterator_traits.
TEST(IterConceptTest, ConceptOfPointerTypes) {
  ::testing::StaticAssertTypeEq<std::random_access_iterator_tag, iter_concept_t<int*> >();
  ::testing::StaticAssertTypeEq<std::random_access_iterator_tag, iter_concept_t<int const*> >();
}

/// @test Given a type @c I that does not specialize `std::iterator_traits<I>`
/// but @c I::iterator_concept is valid and names a type, then
/// `iter_concept_t<I>` denotes @c I::iterator_concept.
TEST(IterConceptTest, UsesIteratorConceptType) {
  struct has_concept_type {
    using value_type = int;
    using difference_type = std::ptrdiff_t;
    using pointer = int*;
    using reference = int&;
    using iterator_category = std::input_iterator_tag;
    using iterator_concept = std::random_access_iterator_tag;
  };
  ::testing::StaticAssertTypeEq<std::random_access_iterator_tag, iter_concept_t<has_concept_type> >();
}

/// @test Given a type @c I that does specialize `std::iterator_traits<I>` and defines
/// `std::iterator_traits<I>::iterator_concept`, then
/// `iter_concept_t<I>` denotes `std::iterator_traits<I>::iterator_concept`.
TEST(IterConceptTest, UsesIteratorTraitsConceptType) {
  ::testing::StaticAssertTypeEq<std::random_access_iterator_tag, iter_concept_t<iterator_traits_has_concept_type> >();
}

/// @test Given a type @c I that does specialize `std::iterator_traits<I>` and defines
/// `std::iterator_traits<I>::iterator_concept` *and* defines alias @c I::iterator_concept, then `iter_concept_t<I>`
/// denotes `std::iterator_traits<I>::iterator_concept`.
TEST(IterConceptTest, PrioritizesIteratorTraitsConceptType) {
  ::testing::
      StaticAssertTypeEq<std::random_access_iterator_tag, iter_concept_t<iterator_traits_and_type_have_concept_type> >(
      );
}

/// @test Given a type @c I with no @c iterator_concept , then
/// `iter_concept_t<I>` falls back to
/// `std::iterator_traits<I>::iterator_category`.
TEST(IterConceptTest, FallsBackToTraitsIteratorCategory) {
  struct no_concept_type {
    using value_type = int;
    using difference_type = std::ptrdiff_t;
    using pointer = int*;
    using reference = int&;
    using iterator_category = std::bidirectional_iterator_tag;
  };
  ::testing::StaticAssertTypeEq<iter_concept_t<no_concept_type>, std::bidirectional_iterator_tag>();
}

/// @test Given a type @c I where `std::iterator_traits<I>` does not have all required type aliases,
/// `iter_concept_t<I>` is ill-formed.
TEST(IterConceptTest, RequiresValidIteratorTraits) {
  struct empty {};
  ASSERT_FALSE((arene::base::substitution_succeeds<iter_concept_t, empty>));
}

}  // namespace
