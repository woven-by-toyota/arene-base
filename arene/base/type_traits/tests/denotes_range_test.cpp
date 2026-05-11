// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/denotes_range.hpp"

#include <iterator>
#include <list>
#include <memory>
#include <vector>

#include <gtest/gtest.h>

namespace {

/// @test Pointers of the same type denote a range
TEST(IterableRange, PointersOfSameTypeFormIterableRange) {
  static_assert(arene::base::denotes_range_v<int*, int*>, "Must form an iterable range");
}

/// @test Pointers of different types do not denote a range
TEST(IterableRange, PointersOfDifferentTypeDoNotFormIterableRange) {
  static_assert(!arene::base::denotes_range_v<int*, double*>, "Must not form an iterable range");
}

/// @test Iterators of the same type denote a range
TEST(IterableRange, IteratorsOfSameTypeFormIterableRange) {
  static_assert(
      arene::base::denotes_range_v<std::vector<double>::iterator, std::vector<double>::iterator>,
      "Must form an iterable range"
  );
  static_assert(
      arene::base::denotes_range_v<std::list<std::vector<double>>::iterator, std::list<std::vector<double>>::iterator>,
      "Must form an iterable range"
  );
}

/// @test `const_iterator`s and `iterator`s from the same container type denote a range
TEST(IterableRange, ConstNonConstIteratorsOfSameTypeFormIterableRange) {
  static_assert(
      arene::base::denotes_range_v<std::vector<double>::const_iterator, std::vector<double>::iterator>,
      "Must form an iterable range"
  );
  static_assert(
      arene::base::
          denotes_range_v<std::list<std::vector<double>>::iterator, std::list<std::vector<double>>::const_iterator>,
      "Must form an iterable range"
  );
}

/// @test Iterators of different types do not denote a range
TEST(IterableRange, IteratorsOfDifferentTypeDoNotFormIterableRange) {
  static_assert(!arene::base::denotes_range_v<double*, int*>, "Must not form an iterable range");
  static_assert(
      !arene::base::denotes_range_v<std::list<std::vector<double>>::iterator, std::list<double>::iterator>,
      "Must not form an iterable range"
  );
}

/// @test Integers do not denote a range
TEST(IterableRange, IntegersDoNotFormIterableRange) {
  static_assert(!arene::base::denotes_range_v<int, int>, "Must not form an iterable range");
}

/// @test Smart pointers do not denote a range
TEST(IterableRange, SmartPointersDoNotFormIterableRange) {
  static_assert(
      !arene::base::denotes_range_v<std::shared_ptr<int>, std::shared_ptr<int>>,
      "Must not form an iterable range"
  );
  static_assert(
      !arene::base::denotes_range_v<std::unique_ptr<int>, std::unique_ptr<int>>,
      "Must not form an iterable range"
  );
}

/// @test Raw pointers of the same type denote a no-throw iterable range
TEST(IterableRange, PointersOfSameTypeFormNoThrowIterableRange) {
  static_assert(arene::base::denotes_nothrow_iterable_range_v<int*, int*>, "Must form an iterable range");
}

/// @test Raw pointers of different types do not denote a no-throw iterable range
TEST(IterableRange, PointersOfDifferentTypeDoNotFormNoThrowIterableRange) {
  static_assert(!arene::base::denotes_nothrow_iterable_range_v<int*, double*>, "Must not form an iterable range");
}

/// @test Iterators of the same type from `std::vector` denote a no-throw iterable range
TEST(IterableRange, IteratorsOfSameTypeFormNoThrowIterableRange) {
  static_assert(
      arene::base::denotes_nothrow_iterable_range_v<std::vector<double>::iterator, std::vector<double>::iterator>,
      "Must form an iterable range"
  );
  static_assert(
      arene::base::denotes_nothrow_iterable_range_v<std::vector<double>*, std::vector<double>*>,
      "Must form a nothrow iterable range"
  );
}

/// @test `const_iterator`s and `iterator`s from `std::vector` denote a no-throw iterable range
TEST(IterableRange, ConstNonConstIteratorsOfSameTypeFormNoThrowIterableRange) {
  static_assert(
      arene::base::denotes_nothrow_iterable_range_v<std::vector<double>::const_iterator, std::vector<double>::iterator>,
      "Must form an iterable range"
  );
  static_assert(
      arene::base::denotes_nothrow_iterable_range_v<std::vector<double>*, std::vector<double> const*>,
      "Must form an iterable range"
  );
}

/// @test Iterators from different container types do not denote a nothrow iterable range
TEST(IterableRange, IteratorsOfDifferentTypeDoNotFormNoThrowIterableRange) {
  static_assert(
      !arene::base::denotes_nothrow_iterable_range_v<std::vector<double>*, std::vector<int>*>,
      "Must not form an iterable range"
  );
  static_assert(
      !arene::base::
          denotes_nothrow_iterable_range_v<std::list<std::vector<double>>::iterator, std::list<double>::iterator>,
      "Must not form an iterable range"
  );
}

/// @test Integers do not denote a nothrow iterable range
TEST(IterableRange, IntegersDoNotFormNoThrowIterableRange) {
  static_assert(!arene::base::denotes_nothrow_iterable_range_v<int, int>, "Must not form an iterable range");
}

/// @test Smart pointers do not denote a nothrow iterable range
TEST(IterableRange, SmartPointersDoNotFormNoThrowIterableRange) {
  static_assert(
      !arene::base::denotes_nothrow_iterable_range_v<std::shared_ptr<int>, std::shared_ptr<int>>,
      "Must not form an iterable range"
  );
  static_assert(
      !arene::base::denotes_nothrow_iterable_range_v<std::unique_ptr<int>, std::unique_ptr<int>>,
      "Must not form an iterable range"
  );
}

/// @test Identical output iterators with nothrow operations denote a nothrow-iterable range
TEST(IterableRange, IteratorsWithNoThrowOperationsFormNothrowIterableRange) {
  struct basic_output_iterator {
    using iterator_category = std::output_iterator_tag;
    using difference_type = void;
    using reference = void;
    using value_type = void;
    using pointer = void;

    struct deref_result {};

    auto operator++() noexcept -> basic_output_iterator&;
    auto operator*() noexcept -> deref_result;

    struct post_increment_result {
      struct post_inc_deref_result {};
      auto operator*() noexcept -> post_inc_deref_result;
    };
    auto operator++(int) noexcept -> post_increment_result;

    auto operator!=(basic_output_iterator const&) noexcept -> bool;
  };

  static_assert(
      arene::base::denotes_range_v<basic_output_iterator, basic_output_iterator>,
      "Must make an iterable range"
  );
  static_assert(
      arene::base::denotes_nothrow_iterable_range_v<basic_output_iterator, basic_output_iterator>,
      "Must make a nothrow iterable range"
  );
}

/// @test Identical output iterators with throwing preincrement do not denote a nothrow-iterable range
TEST(IterableRange, IteratorsWithThrowingPreincrementDoNotFormNothrowIterableRange) {
  struct basic_output_iterator {
    using iterator_category = std::output_iterator_tag;
    using difference_type = void;
    using reference = void;
    using value_type = void;
    using pointer = void;

    struct deref_result {};

    auto operator++() -> basic_output_iterator&;
    auto operator*() noexcept -> deref_result;

    struct post_increment_result {
      struct post_inc_deref_result {};
      auto operator*() noexcept -> post_inc_deref_result;
    };
    auto operator++(int) noexcept -> post_increment_result;

    auto operator!=(basic_output_iterator const&) noexcept -> bool;
  };

  static_assert(
      arene::base::denotes_range_v<basic_output_iterator, basic_output_iterator>,
      "Must make an iterable range"
  );
  static_assert(
      !arene::base::denotes_nothrow_iterable_range_v<basic_output_iterator, basic_output_iterator>,
      "Must not make a nothrow iterable range"
  );
}

/// @test Identical output iterators with throwing post-increment do not denote a nothrow-iterable range
TEST(IterableRange, IteratorsWithThrowingPostincrementDoNotFormNothrowIterableRange) {
  struct basic_output_iterator {
    using iterator_category = std::output_iterator_tag;
    using difference_type = void;
    using reference = void;
    using value_type = void;
    using pointer = void;

    struct deref_result {};

    auto operator++() noexcept -> basic_output_iterator&;
    auto operator*() noexcept -> deref_result;

    struct post_increment_result {
      struct post_inc_deref_result {};
      auto operator*() noexcept -> post_inc_deref_result;
    };
    auto operator++(int) -> post_increment_result;

    auto operator!=(basic_output_iterator const&) noexcept -> bool;
  };

  static_assert(
      arene::base::denotes_range_v<basic_output_iterator, basic_output_iterator>,
      "Must make an iterable range"
  );
  static_assert(
      !arene::base::denotes_nothrow_iterable_range_v<basic_output_iterator, basic_output_iterator>,
      "Must not make a nothrow iterable range"
  );
}

/// @test Identical output iterators with a throwing derefernce operator do not denote a nothrow-iterable range
TEST(IterableRange, IteratorsWithThrowingDereferenceDoNotFormNothrowIterableRange) {
  struct basic_output_iterator {
    using iterator_category = std::output_iterator_tag;
    using difference_type = void;
    using reference = void;
    using value_type = void;
    using pointer = void;

    struct deref_result {};

    auto operator++() noexcept -> basic_output_iterator&;
    auto operator*() -> deref_result;

    struct post_increment_result {
      struct post_inc_deref_result {};
      auto operator*() noexcept -> post_inc_deref_result;
    };
    auto operator++(int) noexcept -> post_increment_result;

    auto operator!=(basic_output_iterator const&) noexcept -> bool;
  };

  static_assert(
      arene::base::denotes_range_v<basic_output_iterator, basic_output_iterator>,
      "Must make an iterable range"
  );
  static_assert(
      !arene::base::denotes_nothrow_iterable_range_v<basic_output_iterator, basic_output_iterator>,
      "Must not make a nothrow iterable range"
  );
}

/// @test Identical output iterators with a throwing inequality operator do not denote a nothrow-iterable range
TEST(IterableRange, IteratorsWithThrowingInequalityComparisonDoNotFormNothrowIterableRange) {
  struct basic_output_iterator {
    using iterator_category = std::output_iterator_tag;
    using difference_type = void;
    using reference = void;
    using value_type = void;
    using pointer = void;

    struct deref_result {};

    auto operator++() noexcept -> basic_output_iterator&;
    auto operator*() noexcept -> deref_result;

    struct post_increment_result {
      struct post_inc_deref_result {};
      auto operator*() noexcept -> post_inc_deref_result;
    };
    auto operator++(int) noexcept -> post_increment_result;

    auto operator!=(basic_output_iterator const&) -> bool;
  };

  static_assert(
      arene::base::denotes_range_v<basic_output_iterator, basic_output_iterator>,
      "Must make an iterable range"
  );
  static_assert(
      !arene::base::denotes_nothrow_iterable_range_v<basic_output_iterator, basic_output_iterator>,
      "Must not make a nothrow iterable range"
  );
}
}  // namespace
