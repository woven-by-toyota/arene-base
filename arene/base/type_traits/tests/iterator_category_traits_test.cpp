// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/iterator_category_traits.hpp"

#include <cstddef>
#include <iterator>
#include <list>
#include <vector>

#include <gtest/gtest.h>

namespace {

/// @test `is_iterator_v` is `false` when instantiated with an `int`
TEST(IteratorCategoryTraits, IntIsNotAnIterator) {
  static_assert(!arene::base::is_iterator_v<int>, "Must not be an iterator");
}

/// @test `is_iterator_v` is `true` when instantiated with an `int*`
TEST(IteratorCategoryTraits, IntPointerIsAnIterator) {
  static_assert(arene::base::is_iterator_v<int*>, "Must be an iterator");
}

/// @test `is_iterator_v` is `true` when instantiated with an `int const*`
TEST(IteratorCategoryTraits, ConstIntPointerIsAnIterator) {
  static_assert(arene::base::is_iterator_v<int const*>, "Must be an iterator");
}

/// @test `is_iterator_v` is `true` when instantiated with an `std::ostream_iterator`
TEST(IteratorCategoryTraits, OStreamIteratorIsAnIterator) {
  static_assert(arene::base::is_iterator_v<std::ostream_iterator<int>>, "Must be an iterator");
}

/// @test `is_iterator_v` is `true` when instantiated with an `std::istream_iterator`
TEST(IteratorCategoryTraits, IstreamIteratorIsAnIterator) {
  static_assert(arene::base::is_iterator_v<std::istream_iterator<int>>, "Must be an iterator");
}

/// @test `is_output_iterator_v` is `false` when instantiated with an `int`
TEST(IteratorCategoryTraits, IntIsNotAnOutputIterator) {
  static_assert(!arene::base::is_output_iterator_v<int>, "Must not be an output iterator");
}

/// @test `is_output_iterator_v` is `true` when instantiated with an `int*`
TEST(IteratorCategoryTraits, IntPointerIsAnOutputIterator) {
  static_assert(arene::base::is_output_iterator_v<int*>, "Must be an output iterator");
}

/// @test `is_output_iterator_v` is `true` when instantiated with an `int const*`
TEST(IteratorCategoryTraits, ConstIntPointerIsNotAnOutputIterator) {
  static_assert(!arene::base::is_output_iterator_v<int const*>, "Must not be an output iterator");
}

/// @test `is_output_iterator_v` is `true` when instantiated with an `std::ostream_iterator`
TEST(IteratorCategoryTraits, OStreamIteratorIsAnOutputIterator) {
  static_assert(arene::base::is_output_iterator_v<std::ostream_iterator<int>>, "Must be an output iterator");
}

/// @test `is_output_iterator_v` is `false` when instantiated with an `std::istream_iterator`
TEST(IteratorCategoryTraits, IstreamIteratorIsNotAnOutputIterator) {
  static_assert(!arene::base::is_output_iterator_v<std::istream_iterator<int>>, "Must not be an output iterator");
}

/// @test `is_input_iterator_v` is `false` when instantiated with an `int`
TEST(IteratorCategoryTraits, IntIsNotAnInputIterator) {
  static_assert(!arene::base::is_input_iterator_v<int>, "Must not be an input iterator");
}

/// @test `is_input_iterator_v` is `true` when instantiated with an `int*`
TEST(IteratorCategoryTraits, IntPointerIsAnInputIterator) {
  static_assert(arene::base::is_input_iterator_v<int*>, "Must be an input iterator");
}

/// @test `is_input_iterator_v` is `true` when instantiated with an `int const*`
TEST(IteratorCategoryTraits, ConstIntPointerIsAnInputIterator) {
  static_assert(arene::base::is_input_iterator_v<int const*>, "Must be an input iterator");
}

/// @test `is_input_iterator_v` is `false` when instantiated with an `std::ostream_iterator`
TEST(IteratorCategoryTraits, OStreamIteratorIsNotAnInputIterator) {
  static_assert(!arene::base::is_input_iterator_v<std::ostream_iterator<int>>, "Must not be an input iterator");
}

/// @test `is_input_iterator_v` is `true` when instantiated with an `std::istream_iterator`
TEST(IteratorCategoryTraits, IstreamIteratorIsAnInputIterator) {
  static_assert(arene::base::is_input_iterator_v<std::istream_iterator<int>>, "Must be an input iterator");
}

/// @test `is_random_access_iterator_v` is `true` when instantiated with an `std::move_iterator<int*>`
TEST(IteratorCategoryTraits, MoveIteratorIsAnInputIterator) {
  static_assert(arene::base::is_random_access_iterator_v<std::move_iterator<int*>>, "Must be a random access iterator");
}

/// @test `is_output_iterator_v` is `true` when instantiated with a custom type that implements the output iterator
/// functions and provides the appropriate type aliases.
TEST(IteratorCategoryTraits, CanDefineCustomOutputIterator) {
  struct basic_output_iterator {
    using iterator_category = std::output_iterator_tag;
    using difference_type = void;
    using reference = void;
    using value_type = void;
    using pointer = void;

    struct deref_result {};

    auto operator++() -> basic_output_iterator&;
    auto operator*() -> deref_result;

    struct post_increment_result {
      struct post_inc_deref_result {};
      auto operator*() -> post_inc_deref_result;
    };
    auto operator++(int) -> post_increment_result;
  };

  static_assert(
      std::is_same<basic_output_iterator::iterator_category, std::output_iterator_tag>::value,
      "Declares output iterator category"
  );
  static_assert(std::is_same<void, basic_output_iterator::difference_type>::value, "void");
  static_assert(std::is_same<void, basic_output_iterator::reference>::value, "void");
  static_assert(std::is_same<void, basic_output_iterator::value_type>::value, "void");
  static_assert(std::is_same<void, basic_output_iterator::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<basic_output_iterator>, "Must be an iterator");
  static_assert(arene::base::is_output_iterator_v<basic_output_iterator>, "Must be an output iterator");
}

/// @test `is_output_iterator_v` is `false` when instantiated with a custom type that implements the output iterator
/// functions and provides the appropriate type aliases except not `reference`
TEST(IteratorCategoryTraits, MissingReferenceTypeMeansNotAnOutputIterator) {
  struct not_output_iterator {
    using iterator_category = std::output_iterator_tag;
    using difference_type = void;
    using value_type = void;
    using pointer = void;

    struct deref_result {};

    auto operator++() -> not_output_iterator&;
    auto operator*() -> deref_result;

    struct post_increment_result {
      struct post_inc_deref_result {};
      auto operator*() -> post_inc_deref_result;
    };
    auto operator++(int) -> post_increment_result;
  };

  static_assert(
      std::is_same<not_output_iterator::iterator_category, std::output_iterator_tag>::value,
      "Declares output iterator category"
  );
  static_assert(std::is_same<void, not_output_iterator::difference_type>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::value_type>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::pointer>::value, "void");
  static_assert(!arene::base::is_iterator_v<not_output_iterator>, "Must not be an iterator");
  static_assert(!arene::base::is_output_iterator_v<not_output_iterator>, "Must not be an output iterator");
}

/// @test `is_output_iterator_v` is `false` when instantiated with a custom type that implements the output iterator
/// functions and provides the appropriate type aliases except not `difference_type`
TEST(IteratorCategoryTraits, MissingDifferenceTypeMeansNotAnOutputIterator) {
  struct not_output_iterator {
    using iterator_category = std::output_iterator_tag;
    using reference = void;
    using value_type = void;
    using pointer = void;

    struct deref_result {};

    auto operator++() -> not_output_iterator&;
    auto operator*() -> deref_result;

    struct post_increment_result {
      struct post_inc_deref_result {};
      auto operator*() -> post_inc_deref_result;
    };
    auto operator++(int) -> post_increment_result;
  };

  static_assert(
      std::is_same<not_output_iterator::iterator_category, std::output_iterator_tag>::value,
      "Declares output iterator category"
  );
  static_assert(std::is_same<void, not_output_iterator::reference>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::value_type>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::pointer>::value, "void");
  static_assert(!arene::base::is_iterator_v<not_output_iterator>, "Must not be an iterator");
  static_assert(!arene::base::is_output_iterator_v<not_output_iterator>, "Must not be an output iterator");
}

/// @test `is_output_iterator_v` is `false` when instantiated with a custom type that implements the output iterator
/// functions and provides the appropriate type aliases except not `pointer`
TEST(IteratorCategoryTraits, MissingPointerMeansNotAnOutputIterator) {
  struct not_output_iterator {
    using iterator_category = std::output_iterator_tag;
    using reference = void;
    using value_type = void;
    using difference_type = void;

    struct deref_result {};

    auto operator++() -> not_output_iterator&;
    auto operator*() -> deref_result;

    struct post_increment_result {
      struct post_inc_deref_result {};
      auto operator*() -> post_inc_deref_result;
    };
    auto operator++(int) -> post_increment_result;
  };

  static_assert(
      std::is_same<not_output_iterator::iterator_category, std::output_iterator_tag>::value,
      "Declares output iterator category"
  );
  static_assert(std::is_same<void, not_output_iterator::reference>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::value_type>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::difference_type>::value, "void");
  static_assert(!arene::base::is_iterator_v<not_output_iterator>, "Must not be an iterator");
  static_assert(!arene::base::is_output_iterator_v<not_output_iterator>, "Must not be an output iterator");
}

/// @test `is_output_iterator_v` is `false` when instantiated with a custom type that implements the output iterator
/// functions and provides the appropriate type aliases except not `value_type`
TEST(IteratorCategoryTraits, MissingValueTypeMeansNotAnOutputIterator) {
  struct not_output_iterator {
    using iterator_category = std::output_iterator_tag;
    using reference = void;
    using pointer = void;
    using difference_type = void;

    struct deref_result {};

    auto operator++() -> not_output_iterator&;
    auto operator*() -> deref_result;

    struct post_increment_result {
      struct post_inc_deref_result {};
      auto operator*() -> post_inc_deref_result;
    };
    auto operator++(int) -> post_increment_result;
  };

  static_assert(
      std::is_same<not_output_iterator::iterator_category, std::output_iterator_tag>::value,
      "Declares output iterator category"
  );
  static_assert(std::is_same<void, not_output_iterator::reference>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::pointer>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::difference_type>::value, "void");
  static_assert(!arene::base::is_iterator_v<not_output_iterator>, "Must not be an iterator");
  static_assert(!arene::base::is_output_iterator_v<not_output_iterator>, "Must not be an output iterator");
}

/// @test `is_output_iterator_v` is `false` when instantiated with a custom type that implements the output iterator
/// functions and provides the appropriate type aliases except not `iterator_category`
TEST(IteratorCategoryTraits, MissingIteratorCategoryMeansNotAnOutputIterator) {
  struct not_output_iterator {
    using reference = void;
    using pointer = void;
    using difference_type = void;
    using value_type = void;

    struct deref_result {};

    auto operator++() -> not_output_iterator&;
    auto operator*() -> deref_result;

    struct post_increment_result {
      struct post_inc_deref_result {};
      auto operator*() -> post_inc_deref_result;
    };
    auto operator++(int) -> post_increment_result;
  };

  static_assert(std::is_same<void, not_output_iterator::reference>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::pointer>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::difference_type>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::value_type>::value, "void");
  static_assert(!arene::base::is_iterator_v<not_output_iterator>, "Must not be an iterator");
  static_assert(!arene::base::is_output_iterator_v<not_output_iterator>, "Must not be an output iterator");
}

/// @test `is_output_iterator_v` is `false` when instantiated with a custom type that implements the output iterator
/// functions and provides the appropriate type aliases except not a pre-increment operator
TEST(IteratorCategoryTraits, MissingPreincrementMeansNotAnOutputIterator) {
  struct not_output_iterator {
    using iterator_category = std::output_iterator_tag;
    using reference = void;
    using value_type = void;
    using pointer = void;
    using difference_type = void;

    struct deref_result {};

    auto operator*() -> deref_result;

    struct post_increment_result {
      struct post_inc_deref_result {};
      auto operator*() -> post_inc_deref_result;
    };
    auto operator++(int) -> post_increment_result;
  };

  static_assert(
      std::is_same<not_output_iterator::iterator_category, std::output_iterator_tag>::value,
      "Declares output iterator category"
  );
  static_assert(std::is_same<void, not_output_iterator::reference>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::value_type>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::difference_type>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::pointer>::value, "void");
  static_assert(!arene::base::is_iterator_v<not_output_iterator>, "Must not be an iterator");
  static_assert(!arene::base::is_output_iterator_v<not_output_iterator>, "Must not be an output iterator");
}

/// @test `is_output_iterator_v` is `false` when instantiated with a custom type that implements the output iterator
/// functions and provides the appropriate type aliases except a pre-increment operator with the wrong signature
TEST(IteratorCategoryTraits, WrongPreincrementMeansNotAnOutputIterator) {
  struct not_output_iterator {
    using iterator_category = std::output_iterator_tag;
    using reference = void;
    using value_type = void;
    using pointer = void;
    using difference_type = void;

    struct deref_result {};
    auto operator*() -> deref_result;

    auto operator++() -> not_output_iterator;

    struct post_increment_result {
      struct post_inc_deref_result {};
      auto operator*() -> post_inc_deref_result;
    };
    auto operator++(int) -> post_increment_result;
  };

  static_assert(
      std::is_same<not_output_iterator::iterator_category, std::output_iterator_tag>::value,
      "Declares output iterator category"
  );
  static_assert(std::is_same<void, not_output_iterator::reference>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::value_type>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::difference_type>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::pointer>::value, "void");
  static_assert(!arene::base::is_iterator_v<not_output_iterator>, "Must not be an iterator");
  static_assert(!arene::base::is_output_iterator_v<not_output_iterator>, "Must not be an output iterator");
}

/// @test `is_output_iterator_v` is `false` when instantiated with a custom type that implements the output iterator
/// functions and provides the appropriate type aliases except not a dereference operator
TEST(IteratorCategoryTraits, NoDereferenceOperatorMeansNotAnOutputIterator) {
  struct not_output_iterator {
    using iterator_category = std::output_iterator_tag;
    using reference = void;
    using value_type = void;
    using pointer = void;
    using difference_type = void;

    auto operator++() -> not_output_iterator&;

    struct post_increment_result {
      struct post_inc_deref_result {};
      auto operator*() -> post_inc_deref_result;
    };
    auto operator++(int) -> post_increment_result;
  };

  static_assert(
      std::is_same<not_output_iterator::iterator_category, std::output_iterator_tag>::value,
      "Declares output iterator category"
  );
  static_assert(std::is_same<void, not_output_iterator::reference>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::value_type>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::difference_type>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::pointer>::value, "void");
  static_assert(!arene::base::is_iterator_v<not_output_iterator>, "Must not be an iterator");
  static_assert(!arene::base::is_output_iterator_v<not_output_iterator>, "Must not be an output iterator");
}

/// @test `is_output_iterator_v` is `false` when instantiated with a custom type that implements the output iterator
/// functions and provides the appropriate type aliases except a dereference operator with the wrong signature
TEST(IteratorCategoryTraits, WrongDereferenceOperatorMeansNotAnOutputIterator) {
  struct not_output_iterator {
    using iterator_category = std::output_iterator_tag;
    using reference = void;
    using value_type = void;
    using pointer = void;
    using difference_type = void;

    auto operator++() -> not_output_iterator&;
    void operator*();

    struct post_increment_result {
      struct post_inc_deref_result {};
      auto operator*() -> post_inc_deref_result;
    };
    auto operator++(int) -> post_increment_result;
  };

  static_assert(
      std::is_same<not_output_iterator::iterator_category, std::output_iterator_tag>::value,
      "Declares output iterator category"
  );
  static_assert(std::is_same<void, not_output_iterator::reference>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::value_type>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::difference_type>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::pointer>::value, "void");
  static_assert(!arene::base::is_iterator_v<not_output_iterator>, "Must not be an iterator");
  static_assert(!arene::base::is_output_iterator_v<not_output_iterator>, "Must not be an output iterator");
}

/// @test `is_output_iterator_v` is `false` when instantiated with a custom type that implements the output iterator
/// functions and provides the appropriate type aliases except not a post-increment operator
TEST(IteratorCategoryTraits, NoPostincrementMeansNotAnOutputIterator) {
  struct not_output_iterator {
    using iterator_category = std::output_iterator_tag;
    using reference = void;
    using value_type = void;
    using pointer = void;
    using difference_type = void;

    struct deref_result {};
    auto operator*() -> deref_result;

    auto operator++() -> not_output_iterator&;
  };

  static_assert(
      std::is_same<not_output_iterator::iterator_category, std::output_iterator_tag>::value,
      "Declares output iterator category"
  );
  static_assert(std::is_same<void, not_output_iterator::reference>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::value_type>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::difference_type>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::pointer>::value, "void");
  static_assert(!arene::base::is_iterator_v<not_output_iterator>, "Must not be an iterator");
  static_assert(!arene::base::is_output_iterator_v<not_output_iterator>, "Must not be an output iterator");
}

/// @test `is_output_iterator_v` is `false` when instantiated with a custom type that implements the output iterator
/// functions and provides the appropriate type aliases except a post-increment operator with the wrong signature
TEST(IteratorCategoryTraits, NonDereferenceablePostincrementResultMeansNotAnOutputIterator) {
  struct not_output_iterator {
    using iterator_category = std::output_iterator_tag;
    using reference = void;
    using value_type = void;
    using pointer = void;
    using difference_type = void;

    struct deref_result {};
    auto operator*() -> deref_result;

    auto operator++() -> not_output_iterator&;

    struct post_increment_result {};
    auto operator++(int) -> post_increment_result;
  };

  static_assert(
      std::is_same<not_output_iterator::iterator_category, std::output_iterator_tag>::value,
      "Declares output iterator category"
  );
  static_assert(std::is_same<void, not_output_iterator::reference>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::value_type>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::difference_type>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::pointer>::value, "void");
  static_assert(!arene::base::is_iterator_v<not_output_iterator>, "Must not be an iterator");
  static_assert(!arene::base::is_output_iterator_v<not_output_iterator>, "Must not be an output iterator");
}

/// @test `is_output_iterator_v` is `false` when instantiated with a custom type that implements the output iterator
/// functions and provides the appropriate type aliases except a post-increment operator with the wrong signature
TEST(IteratorCategoryTraits, InvalidDereferenceResultForPostIncrementMeansNotAnOutputIterator) {
  struct not_output_iterator {
    using iterator_category = std::output_iterator_tag;
    using reference = void;
    using value_type = void;
    using pointer = void;
    using difference_type = void;

    struct deref_result {};
    auto operator*() -> deref_result;

    auto operator++() -> not_output_iterator&;

    struct post_increment_result {
      void operator*();
    };
    auto operator++(int) -> post_increment_result;
  };

  static_assert(
      std::is_same<not_output_iterator::iterator_category, std::output_iterator_tag>::value,
      "Declares output iterator category"
  );
  static_assert(std::is_same<void, not_output_iterator::reference>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::value_type>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::difference_type>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::pointer>::value, "void");
  static_assert(!arene::base::is_iterator_v<not_output_iterator>, "Must not be an iterator");
  static_assert(!arene::base::is_output_iterator_v<not_output_iterator>, "Must not be an output iterator");
}

/// @test `is_output_iterator_v` is `false` when instantiated with a custom type that implements the output iterator
/// functions and provides the appropriate type aliases except not copy-constructible
TEST(IteratorCategoryTraits, NotCopyConstructibleMeansNotAnOutputIterator) {
  struct not_output_iterator {
    ~not_output_iterator();
    not_output_iterator(not_output_iterator const&) = delete;
    auto operator=(not_output_iterator const&) -> not_output_iterator& = delete;
    not_output_iterator(not_output_iterator&&) = delete;
    auto operator=(not_output_iterator&&) -> not_output_iterator& = delete;
    using iterator_category = std::output_iterator_tag;
    using reference = void;
    using value_type = void;
    using pointer = void;
    using difference_type = void;

    struct deref_result {};
    auto operator*() -> deref_result;

    auto operator++() -> not_output_iterator&;

    struct post_increment_result {
      struct post_inc_deref_result {};
      auto operator*() -> post_inc_deref_result;
    };
    auto operator++(int) -> post_increment_result;
  };

  static_assert(
      std::is_same<not_output_iterator::iterator_category, std::output_iterator_tag>::value,
      "Declares output iterator category"
  );
  static_assert(std::is_same<void, not_output_iterator::reference>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::value_type>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::difference_type>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::pointer>::value, "void");
  static_assert(!arene::base::is_iterator_v<not_output_iterator>, "Must not be an iterator");
  static_assert(!arene::base::is_output_iterator_v<not_output_iterator>, "Must not be an output iterator");
}

/// @test `is_output_iterator_v` is `false` when instantiated with a custom type that implements the output iterator
/// functions and provides the appropriate type aliases except not copy-assignable
TEST(IteratorCategoryTraits, NotCopyAssignableMeansNotAnOutputIterator) {
  struct not_output_iterator {
    ~not_output_iterator();
    not_output_iterator(not_output_iterator const&) = delete;
    auto operator=(not_output_iterator const&) -> not_output_iterator& = delete;
    not_output_iterator(not_output_iterator&&) = delete;
    auto operator=(not_output_iterator&&) -> not_output_iterator& = delete;
    using iterator_category = std::output_iterator_tag;
    using reference = void;
    using value_type = void;
    using pointer = void;
    using difference_type = void;

    struct deref_result {};
    auto operator*() -> deref_result;

    auto operator++() -> not_output_iterator&;

    struct post_increment_result {
      struct post_inc_deref_result {};
      auto operator*() -> post_inc_deref_result;
    };
    auto operator++(int) -> post_increment_result;
  };

  static_assert(
      std::is_same<not_output_iterator::iterator_category, std::output_iterator_tag>::value,
      "Declares output iterator category"
  );
  static_assert(std::is_same<void, not_output_iterator::reference>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::value_type>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::difference_type>::value, "void");
  static_assert(std::is_same<void, not_output_iterator::pointer>::value, "void");
  static_assert(!arene::base::is_iterator_v<not_output_iterator>, "Must not be an iterator");
  static_assert(!arene::base::is_output_iterator_v<not_output_iterator>, "Must not be an output iterator");
}

/// @test `is_input_iterator_v` is `true` when instantiated with a custom type that implements the input iterator
/// functions and provides the appropriate type aliases
TEST(IteratorCategoryTraits, CanDefineCustomInputIterator) {
  struct basic_input_iterator {
    struct deref_result {};

    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using reference = deref_result;
    using value_type = deref_result;
    using pointer = void;

    auto operator++() -> basic_input_iterator&;
    auto operator*() -> deref_result;

    struct post_increment_result {
      auto operator*() -> deref_result;
    };
    auto operator++(int) -> post_increment_result;

    auto operator==(basic_input_iterator const&) -> bool;
    auto operator!=(basic_input_iterator const&) -> bool;
  };

  static_assert(
      std::is_same<basic_input_iterator::iterator_category, std::input_iterator_tag>::value,
      "Declares input iterator category"
  );
  static_assert(!std::is_same<void, basic_input_iterator::difference_type>::value, "not void");
  static_assert(!std::is_same<void, basic_input_iterator::reference>::value, "not void");
  static_assert(!std::is_same<void, basic_input_iterator::value_type>::value, "not void");
  static_assert(std::is_same<void, basic_input_iterator::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<basic_input_iterator>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<basic_input_iterator>, "Must be an input iterator");
}

/// @test `is_input_iterator_v` is `true` when instantiated with a
/// `std::move_iterator<S*>` and `S` is a type that is move constructible but not
/// copy constructible
TEST(IteratorCategoryTraits, IteratorReferenceIsRvalueReferenceToNonCopyableType) {
  // NOLINTNEXTLINE(hicpp-special-member-functions)
  struct not_copyable {
    not_copyable(not_copyable const&) = delete;
    not_copyable(not_copyable&&) = default;
  };

  static_assert(arene::base::is_iterator_v<std::move_iterator<not_copyable*>>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<std::move_iterator<not_copyable*>>, "Must be an input iterator");
}

/// @test `is_input_iterator_v` is `false` when instantiated with a custom type that implements the input iterator
/// functions and provides the appropriate type aliases except no `iterator_category`
TEST(IteratorCategoryTraits, NoIteratorCategoryMeansNotAnInputIterator) {
  struct not_input_iterator {
    struct deref_result {};

    using difference_type = std::ptrdiff_t;
    using reference = deref_result;
    using value_type = deref_result;
    using pointer = void;

    auto operator++() -> not_input_iterator&;
    auto operator*() -> deref_result;

    struct post_increment_result {
      auto operator*() -> deref_result;
    };
    auto operator++(int) -> post_increment_result;
    auto operator==(not_input_iterator const&) -> bool;
    auto operator!=(not_input_iterator const&) -> bool;
  };

  static_assert(!std::is_same<void, not_input_iterator::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_input_iterator::reference>::value, "not void");
  static_assert(!std::is_same<void, not_input_iterator::value_type>::value, "not void");
  static_assert(std::is_same<void, not_input_iterator::pointer>::value, "void");
  static_assert(!arene::base::is_iterator_v<not_input_iterator>, "Must not be an iterator");
  static_assert(!arene::base::is_input_iterator_v<not_input_iterator>, "Must not be an input iterator");
}

/// @test `is_input_iterator_v` is `false` when instantiated with a custom type that implements the input iterator
/// functions and provides the appropriate type aliases except `pointer` does not match the return from the arrow
/// operator
TEST(IteratorCategoryTraits, PointerTypeNotMatchingArrowReturnMeansNotAnInputIterator) {
  struct not_input_iterator {
    struct deref_result {};

    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using reference = deref_result;
    using value_type = deref_result;
    using pointer = void;

    auto operator++() -> not_input_iterator&;
    auto operator*() -> deref_result;
    auto operator->() -> deref_result*;

    struct post_increment_result {
      auto operator*() -> deref_result;
    };
    auto operator++(int) -> post_increment_result;
    auto operator==(not_input_iterator const&) -> bool;
    auto operator!=(not_input_iterator const&) -> bool;
  };

  static_assert(
      std::is_same<not_input_iterator::iterator_category, std::input_iterator_tag>::value,
      "Declares input iterator category"
  );
  static_assert(!std::is_same<void, not_input_iterator::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_input_iterator::reference>::value, "not void");
  static_assert(!std::is_same<void, not_input_iterator::value_type>::value, "not void");
  static_assert(std::is_same<void, not_input_iterator::pointer>::value, "void");
  static_assert(!arene::base::is_iterator_v<not_input_iterator>, "Must not be an iterator");
  static_assert(!arene::base::is_input_iterator_v<not_input_iterator>, "Must not be an input iterator");
}

/// @test `is_input_iterator_v` is `false` when instantiated with a custom type that implements the input iterator
/// functions and provides the appropriate type aliases except `pointer` provided as non-`void`, but the arrow operator
/// is not defined.
TEST(IteratorCategoryTraits, NonVoidPointerTypeAndNoArrowOpMeansNotAnInputIterator) {
  struct not_input_iterator {
    struct deref_result {};

    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using reference = deref_result;
    using value_type = deref_result;
    using pointer = deref_result*;

    auto operator++() -> not_input_iterator&;
    auto operator*() -> deref_result;

    struct post_increment_result {
      auto operator*() -> deref_result;
    };
    auto operator++(int) -> post_increment_result;
    auto operator==(not_input_iterator const&) -> bool;
    auto operator!=(not_input_iterator const&) -> bool;
  };

  static_assert(
      std::is_same<not_input_iterator::iterator_category, std::input_iterator_tag>::value,
      "Declares input iterator category"
  );
  static_assert(!std::is_same<void, not_input_iterator::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_input_iterator::reference>::value, "not void");
  static_assert(!std::is_same<void, not_input_iterator::value_type>::value, "not void");
  static_assert(!std::is_same<void, not_input_iterator::pointer>::value, "not void");
  static_assert(!arene::base::is_iterator_v<not_input_iterator>, "Must not be an iterator");
  static_assert(!arene::base::is_input_iterator_v<not_input_iterator>, "Must not be an input iterator");
}

/// @test `is_input_iterator_v` is `false` when instantiated with a custom type that implements the input iterator
/// functions and provides the appropriate type aliases except no `difference_type` is not an integral type
TEST(IteratorCategoryTraits, NonIntegralDifferenceTypeMeansNotAnInputIterator) {
  struct not_input_iterator {
    struct deref_result {};

    using iterator_category = std::input_iterator_tag;
    using difference_type = float;
    using reference = deref_result;
    using value_type = deref_result;
    using pointer = void;

    auto operator++() -> not_input_iterator&;
    auto operator*() -> deref_result;

    struct post_increment_result {
      auto operator*() -> deref_result;
    };
    auto operator++(int) -> post_increment_result;
    auto operator==(not_input_iterator const&) -> bool;
    auto operator!=(not_input_iterator const&) -> bool;
  };

  static_assert(
      std::is_same<not_input_iterator::iterator_category, std::input_iterator_tag>::value,
      "Declares input iterator category"
  );
  static_assert(!std::is_same<void, not_input_iterator::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_input_iterator::reference>::value, "not void");
  static_assert(!std::is_same<void, not_input_iterator::value_type>::value, "not void");
  static_assert(std::is_same<void, not_input_iterator::pointer>::value, "void");
  static_assert(!arene::base::is_iterator_v<not_input_iterator>, "Must not be an iterator");
  static_assert(!arene::base::is_input_iterator_v<not_input_iterator>, "Must not be an input iterator");
}

/// @test `is_input_iterator_v` is `false` when instantiated with a custom type that implements the input iterator
/// functions and provides the appropriate type aliases except `difference_type` is unsigned
TEST(IteratorCategoryTraits, UnsignedDifferenceTypeMeansNotAnInputIterator) {
  struct not_input_iterator {
    struct deref_result {};

    using iterator_category = std::input_iterator_tag;
    using difference_type = unsigned;
    using reference = deref_result;
    using value_type = deref_result;
    using pointer = void;

    auto operator++() -> not_input_iterator&;
    auto operator*() -> deref_result;

    struct post_increment_result {
      auto operator*() -> deref_result;
    };
    auto operator++(int) -> post_increment_result;
    auto operator==(not_input_iterator const&) -> bool;
    auto operator!=(not_input_iterator const&) -> bool;
  };

  static_assert(
      std::is_same<not_input_iterator::iterator_category, std::input_iterator_tag>::value,
      "Declares input iterator category"
  );
  static_assert(!std::is_same<void, not_input_iterator::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_input_iterator::reference>::value, "not void");
  static_assert(!std::is_same<void, not_input_iterator::value_type>::value, "not void");
  static_assert(std::is_same<void, not_input_iterator::pointer>::value, "void");
  static_assert(!arene::base::is_iterator_v<not_input_iterator>, "Must not be an iterator");
  static_assert(!arene::base::is_input_iterator_v<not_input_iterator>, "Must not be an input iterator");
}

/// @test `is_input_iterator_v` is `false` when instantiated with a custom type that implements the input iterator
/// functions and provides the appropriate type aliases except `reference` does not match the return type of the
/// dereference operator
TEST(IteratorCategoryTraits, MismatchedDereferenceAndReferenceTypeMeansNotAnInputIterator) {
  struct not_input_iterator {
    struct deref_result {};
    struct reference_result {};

    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using reference = reference_result;
    using value_type = deref_result;
    using pointer = void;

    auto operator++() -> not_input_iterator&;
    auto operator*() -> deref_result;

    struct post_increment_result {
      auto operator*() -> deref_result;
    };
    auto operator++(int) -> post_increment_result;
    auto operator==(not_input_iterator const&) -> bool;
    auto operator!=(not_input_iterator const&) -> bool;
  };

  static_assert(
      std::is_same<not_input_iterator::iterator_category, std::input_iterator_tag>::value,
      "Declares input iterator category"
  );
  static_assert(!std::is_same<void, not_input_iterator::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_input_iterator::reference>::value, "not void");
  static_assert(!std::is_same<void, not_input_iterator::value_type>::value, "not void");
  static_assert(std::is_same<void, not_input_iterator::pointer>::value, "void");
  static_assert(!arene::base::is_iterator_v<not_input_iterator>, "Must not be an iterator");
  static_assert(!arene::base::is_input_iterator_v<not_input_iterator>, "Must not be an input iterator");
}

/// @test `is_input_iterator_v` is `false` when instantiated with a custom type that implements the input iterator
/// functions and provides the appropriate type aliases except `reference` does not match the return type of the
/// dereference operator on the result of post-increment
TEST(IteratorCategoryTraits, MismatchedPostIncrementDereferenceAndReferenceTypeMeansNotAnInputIterator) {
  struct not_input_iterator {
    struct deref_result {};

    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using reference = deref_result;
    using value_type = deref_result;
    using pointer = void;

    auto operator++() -> not_input_iterator&;
    auto operator*() -> deref_result;

    struct post_increment_result {
      struct postinc_result {};
      auto operator*() -> postinc_result;
    };
    auto operator++(int) -> post_increment_result;
    auto operator==(not_input_iterator const&) -> bool;
    auto operator!=(not_input_iterator const&) -> bool;
  };

  static_assert(
      std::is_same<not_input_iterator::iterator_category, std::input_iterator_tag>::value,
      "Declares input iterator category"
  );
  static_assert(!std::is_same<void, not_input_iterator::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_input_iterator::reference>::value, "not void");
  static_assert(!std::is_same<void, not_input_iterator::value_type>::value, "not void");
  static_assert(std::is_same<void, not_input_iterator::pointer>::value, "void");
  static_assert(!arene::base::is_iterator_v<not_input_iterator>, "Must not be an iterator");
  static_assert(!arene::base::is_input_iterator_v<not_input_iterator>, "Must not be an input iterator");
}

/// @test `is_input_iterator_v` is `false` when instantiated with a custom type that implements the input iterator
/// functions and provides the appropriate type aliases except `reference` is not a consistent with the `value_type`
TEST(IteratorCategoryTraits, MismatchedValueAndReferenceTypeMeansNotAnInputIterator) {
  struct not_input_iterator {
    struct deref_result {};

    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using reference = deref_result;
    struct value_type {};
    using pointer = void;

    auto operator++() -> not_input_iterator&;
    auto operator*() -> deref_result;

    struct post_increment_result {
      auto operator*() -> deref_result;
    };
    auto operator++(int) -> post_increment_result;
    auto operator==(not_input_iterator const&) -> bool;
    auto operator!=(not_input_iterator const&) -> bool;
  };

  static_assert(
      std::is_same<not_input_iterator::iterator_category, std::input_iterator_tag>::value,
      "Declares input iterator category"
  );
  static_assert(!std::is_same<void, not_input_iterator::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_input_iterator::reference>::value, "not void");
  static_assert(!std::is_same<void, not_input_iterator::value_type>::value, "not void");
  static_assert(std::is_same<void, not_input_iterator::pointer>::value, "void");
  static_assert(!arene::base::is_iterator_v<not_input_iterator>, "Must not be an iterator");
  static_assert(!arene::base::is_input_iterator_v<not_input_iterator>, "Must not be an input iterator");
}

/// @test `is_input_iterator_v` is `false` when instantiated with a custom type that implements the input iterator
/// functions and provides the appropriate type aliases except it is not equality-comparable
TEST(IteratorCategoryTraits, NotEqualityComparableMeansNotAnInputIterator) {
  struct not_input_iterator {
    struct deref_result {};

    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using reference = deref_result;
    using value_type = deref_result;
    using pointer = void;

    auto operator++() -> not_input_iterator&;
    auto operator*() -> deref_result;

    struct post_increment_result {
      auto operator*() -> deref_result;
    };
    auto operator++(int) -> post_increment_result;
    auto operator!=(not_input_iterator const&) -> bool;
  };

  static_assert(
      std::is_same<not_input_iterator::iterator_category, std::input_iterator_tag>::value,
      "Declares input iterator category"
  );
  static_assert(!std::is_same<void, not_input_iterator::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_input_iterator::reference>::value, "not void");
  static_assert(!std::is_same<void, not_input_iterator::value_type>::value, "not void");
  static_assert(std::is_same<void, not_input_iterator::pointer>::value, "void");
  static_assert(!arene::base::is_iterator_v<not_input_iterator>, "Must not be an iterator");
  static_assert(!arene::base::is_input_iterator_v<not_input_iterator>, "Must not be an input iterator");
}

/// @test `is_input_iterator_v` is `false` when instantiated with a custom type that implements the input iterator
/// functions and provides the appropriate type aliases except it is not inequality-comparable
TEST(IteratorCategoryTraits, NotInequalityComparableMeansNotAnInputIterator) {
  struct not_input_iterator {
    struct deref_result {};

    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using reference = deref_result;
    using value_type = deref_result;
    using pointer = void;

    auto operator++() -> not_input_iterator&;
    auto operator*() -> deref_result;

    struct post_increment_result {
      auto operator*() -> deref_result;
    };
    auto operator++(int) -> post_increment_result;
    auto operator==(not_input_iterator const&) -> bool;
  };

  static_assert(
      std::is_same<not_input_iterator::iterator_category, std::input_iterator_tag>::value,
      "Declares input iterator category"
  );
  static_assert(!std::is_same<void, not_input_iterator::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_input_iterator::reference>::value, "not void");
  static_assert(!std::is_same<void, not_input_iterator::value_type>::value, "not void");
  static_assert(std::is_same<void, not_input_iterator::pointer>::value, "void");
  static_assert(!arene::base::is_iterator_v<not_input_iterator>, "Must not be an iterator");
  static_assert(!arene::base::is_input_iterator_v<not_input_iterator>, "Must not be an input iterator");
}

/// @test `is_forward_iterator_v` is `true` when instantiated with a custom type that implements the forward iterator
/// functions and provides the appropriate type aliases
TEST(IteratorCategoryTraits, MinimalForwardIterator) {
  struct basic_forward_iterator {
    struct some_value {};

    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using reference = some_value&;
    using value_type = some_value;
    using pointer = void;

    auto operator++() -> basic_forward_iterator&;
    auto operator*() -> reference;

    auto operator++(int) -> basic_forward_iterator;
    auto operator==(basic_forward_iterator const&) -> bool;
    auto operator!=(basic_forward_iterator const&) -> bool;
  };

  static_assert(
      std::is_same<basic_forward_iterator::iterator_category, std::forward_iterator_tag>::value,
      "Declares forward iterator category"
  );
  static_assert(!std::is_same<void, basic_forward_iterator::difference_type>::value, "not void");
  static_assert(!std::is_same<void, basic_forward_iterator::reference>::value, "not void");
  static_assert(!std::is_same<void, basic_forward_iterator::value_type>::value, "not void");
  static_assert(std::is_same<void, basic_forward_iterator::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<basic_forward_iterator>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<basic_forward_iterator>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<basic_forward_iterator>, "Must be a forward iterator");
}

/// @test `is_forward_iterator_v` is `false` when instantiated with a custom type that implements the forward iterator
/// functions and provides the appropriate type aliases, except that post-increment does not return the iterator type
TEST(IteratorCategoryTraits, WrongPostIncReturnMeansNotForwardIterator) {
  struct not_forward_iterator {
    struct some_value {};

    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using reference = some_value&;
    using value_type = some_value;
    using pointer = void;

    auto operator++() -> not_forward_iterator&;
    auto operator*() -> reference;

    struct postinc_result {
      auto operator*() -> reference;
    };
    auto operator++(int) -> postinc_result;
    auto operator==(not_forward_iterator const&) -> bool;
    auto operator!=(not_forward_iterator const&) -> bool;
  };

  static_assert(
      std::is_same<not_forward_iterator::iterator_category, std::forward_iterator_tag>::value,
      "Declares forward iterator category"
  );
  static_assert(!std::is_same<void, not_forward_iterator::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_forward_iterator::reference>::value, "not void");
  static_assert(!std::is_same<void, not_forward_iterator::value_type>::value, "not void");
  static_assert(std::is_same<void, not_forward_iterator::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_forward_iterator>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_forward_iterator>, "Must be an input iterator");
  static_assert(!arene::base::is_forward_iterator_v<not_forward_iterator>, "Must not be a forward iterator");
}

/// @test `is_forward_iterator_v` is `false` when instantiated with a custom type that implements the forward iterator
/// functions and provides the appropriate type aliases, except that `reference` is not a reference.
TEST(IteratorCategoryTraits, ReferenceNotAReferenceMeansNotForwardIterator) {
  struct not_forward_iterator {
    struct some_value {};

    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using reference = some_value;
    using value_type = some_value;
    using pointer = void;

    auto operator++() -> not_forward_iterator&;
    auto operator*() -> reference;

    auto operator++(int) -> not_forward_iterator;
    auto operator==(not_forward_iterator const&) -> bool;
    auto operator!=(not_forward_iterator const&) -> bool;
  };

  static_assert(
      std::is_same<not_forward_iterator::iterator_category, std::forward_iterator_tag>::value,
      "Declares forward iterator category"
  );
  static_assert(!std::is_same<void, not_forward_iterator::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_forward_iterator::reference>::value, "not void");
  static_assert(!std::is_same<void, not_forward_iterator::value_type>::value, "not void");
  static_assert(std::is_same<void, not_forward_iterator::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_forward_iterator>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_forward_iterator>, "Must be an input iterator");
  static_assert(!arene::base::is_forward_iterator_v<not_forward_iterator>, "Must not be a forward iterator");
}

/// @test `is_forward_iterator_v` is `false` when instantiated with a custom type that implements the forward iterator
/// functions and provides the appropriate type aliases, except that `reference` is not a reference to `value_type`.
TEST(IteratorCategoryTraits, ReferenceNotAReferenceToValueMeansNotForwardIterator) {
  struct not_forward_iterator {
    struct some_value {};
    struct derived_value : some_value {};

    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using reference = derived_value&;
    using value_type = some_value;
    using pointer = void;

    auto operator++() -> not_forward_iterator&;
    auto operator*() -> reference;

    auto operator++(int) -> not_forward_iterator;
    auto operator==(not_forward_iterator const&) -> bool;
    auto operator!=(not_forward_iterator const&) -> bool;
  };

  static_assert(
      std::is_same<not_forward_iterator::iterator_category, std::forward_iterator_tag>::value,
      "Declares forward iterator category"
  );
  static_assert(!std::is_same<void, not_forward_iterator::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_forward_iterator::reference>::value, "not void");
  static_assert(!std::is_same<void, not_forward_iterator::value_type>::value, "not void");
  static_assert(std::is_same<void, not_forward_iterator::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_forward_iterator>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_forward_iterator>, "Must be an input iterator");
  static_assert(!arene::base::is_forward_iterator_v<not_forward_iterator>, "Must not be a forward iterator");
}

/// @test `is_forward_iterator_v` is `false` when instantiated with a custom type that implements the forward iterator
/// functions and provides the appropriate type aliases, except that it is not default-constructible
TEST(IteratorCategoryTraits, NotDefaultConstructibleMeansNotForwardIterator) {
  struct not_forward_iterator {
    not_forward_iterator() = delete;
    struct some_value {};

    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using reference = some_value&;
    using value_type = some_value;
    using pointer = void;

    auto operator++() -> not_forward_iterator&;
    auto operator*() -> reference;

    auto operator++(int) -> not_forward_iterator;
    auto operator==(not_forward_iterator const&) -> bool;
    auto operator!=(not_forward_iterator const&) -> bool;
  };

  static_assert(
      std::is_same<not_forward_iterator::iterator_category, std::forward_iterator_tag>::value,
      "Declares forward iterator category"
  );
  static_assert(!std::is_same<void, not_forward_iterator::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_forward_iterator::reference>::value, "not void");
  static_assert(!std::is_same<void, not_forward_iterator::value_type>::value, "not void");
  static_assert(std::is_same<void, not_forward_iterator::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_forward_iterator>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_forward_iterator>, "Must be an input iterator");
  static_assert(!arene::base::is_forward_iterator_v<not_forward_iterator>, "Must not be a forward iterator");
}

/// @test `std::list<T>::iterator` is a bidrectional iterator, forward iterator, input iterator and output iterator, but
/// not a random access iterator. `std::list<T>::const_iterator` is a bidrectional iterator, forward iterator and input
/// iterator, but not a random access iterator or output iterator.
TEST(IteratorCategoryTraits, ListIteratorsAreBidirectional) {
  using iter = std::list<int>::iterator;
  using const_iter = std::list<int>::const_iterator;
  static_assert(arene::base::is_iterator_v<iter>, "Must be an iterator");
  static_assert(arene::base::is_iterator_v<const_iter>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<iter>, "Must be an input iterator");
  static_assert(arene::base::is_input_iterator_v<const_iter>, "Must be an input iterator");
  static_assert(arene::base::is_output_iterator_v<iter>, "Must be an output iterator");
  static_assert(!arene::base::is_output_iterator_v<const_iter>, "Must not be an output iterator");
  static_assert(arene::base::is_forward_iterator_v<iter>, "Must be a forward iterator");
  static_assert(arene::base::is_forward_iterator_v<const_iter>, "Must be a forward iterator");
  static_assert(arene::base::is_bidirectional_iterator_v<iter>, "Must be a bidirectional iterator");
  static_assert(arene::base::is_bidirectional_iterator_v<const_iter>, "Must be a bidirectional iterator");
  static_assert(!arene::base::is_random_access_iterator_v<iter>, "Must not be a random_access iterator");
  static_assert(!arene::base::is_random_access_iterator_v<const_iter>, "Must not be a random_access iterator");
}

/// @test `std::vector<T>::iterator` is a bidrectional iterator, forward iterator, input iterator, output iterator, and
/// random access iterator. `std::vector<T>::const_iterator` is a bidrectional iterator, forward iterator, input
/// iterator, and random access iterator, but not an output iterator.
TEST(IteratorCategoryTraits, VectorIteratorsAreRandomAccess) {
  using iter = std::vector<int>::iterator;
  using const_iter = std::vector<int>::const_iterator;
  static_assert(arene::base::is_iterator_v<iter>, "Must be an iterator");
  static_assert(arene::base::is_iterator_v<const_iter>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<iter>, "Must be an input iterator");
  static_assert(arene::base::is_input_iterator_v<const_iter>, "Must be an input iterator");
  static_assert(arene::base::is_output_iterator_v<iter>, "Must be an output iterator");
  static_assert(!arene::base::is_output_iterator_v<const_iter>, "Must not be an output iterator");
  static_assert(arene::base::is_forward_iterator_v<iter>, "Must be a forward iterator");
  static_assert(arene::base::is_forward_iterator_v<const_iter>, "Must be a forward iterator");
  static_assert(arene::base::is_bidirectional_iterator_v<iter>, "Must be a bidirectional iterator");
  static_assert(arene::base::is_bidirectional_iterator_v<const_iter>, "Must be a bidirectional iterator");
  static_assert(arene::base::is_random_access_iterator_v<iter>, "Must be a random_access iterator");
  static_assert(arene::base::is_random_access_iterator_v<const_iter>, "Must be a random_access iterator");
}

/// @test `is_bidirectional_iterator_v` is `true` when instantiated with a custom type that implements the bidirectional
/// iterator functions and provides the appropriate type aliases
TEST(IteratorCategoryTraits, MinimalBidirectionalIterator) {
  struct basic_bidirectional_iterator {
    struct some_value {};

    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using reference = some_value&;
    using value_type = some_value;
    using pointer = void;

    auto operator*() -> reference;

    auto operator++() -> basic_bidirectional_iterator&;
    auto operator++(int) -> basic_bidirectional_iterator;
    auto operator--() -> basic_bidirectional_iterator&;
    auto operator--(int) -> basic_bidirectional_iterator;
    auto operator==(basic_bidirectional_iterator const&) -> bool;
    auto operator!=(basic_bidirectional_iterator const&) -> bool;
  };

  static_assert(
      std::is_same<basic_bidirectional_iterator::iterator_category, std::bidirectional_iterator_tag>::value,
      "Declares bidirectional iterator category"
  );
  static_assert(!std::is_same<void, basic_bidirectional_iterator::difference_type>::value, "not void");
  static_assert(!std::is_same<void, basic_bidirectional_iterator::reference>::value, "not void");
  static_assert(!std::is_same<void, basic_bidirectional_iterator::value_type>::value, "not void");
  static_assert(std::is_same<void, basic_bidirectional_iterator::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<basic_bidirectional_iterator>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<basic_bidirectional_iterator>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<basic_bidirectional_iterator>, "Must be a forward iterator");
  static_assert(
      arene::base::is_bidirectional_iterator_v<basic_bidirectional_iterator>,
      "Must be a bidirectional iterator"
  );
}

/// @test `is_bidirectional_iterator_v` is `false` when instantiated with a custom type that implements the
/// bidirectional iterator functions and provides the appropriate type aliases, except there is no pre-increment
/// operator
TEST(IteratorCategoryTraits, NoPreDecrementMeansNotBidirectionalIterator) {
  struct not_bidirectional_iterator {
    struct some_value {};

    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using reference = some_value&;
    using value_type = some_value;
    using pointer = void;

    auto operator*() -> reference;

    auto operator++() -> not_bidirectional_iterator&;
    auto operator++(int) -> not_bidirectional_iterator;
    auto operator--(int) -> not_bidirectional_iterator;
    auto operator==(not_bidirectional_iterator const&) -> bool;
    auto operator!=(not_bidirectional_iterator const&) -> bool;
  };

  static_assert(
      std::is_same<not_bidirectional_iterator::iterator_category, std::bidirectional_iterator_tag>::value,
      "Declares bidirectional iterator category"
  );
  static_assert(!std::is_same<void, not_bidirectional_iterator::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_bidirectional_iterator::reference>::value, "not void");
  static_assert(!std::is_same<void, not_bidirectional_iterator::value_type>::value, "not void");
  static_assert(std::is_same<void, not_bidirectional_iterator::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_bidirectional_iterator>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_bidirectional_iterator>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<not_bidirectional_iterator>, "Must be a forward iterator");
  static_assert(
      !arene::base::is_bidirectional_iterator_v<not_bidirectional_iterator>,
      "Must not be a bidirectional iterator"
  );
}

/// @test `is_bidirectional_iterator_v` is `false` when instantiated with a custom type that implements the
/// bidirectional iterator functions and provides the appropriate type aliases, except there is no post-increment
/// operator
TEST(IteratorCategoryTraits, NoPostDecrementMeansNotBidirectionalIterator) {
  struct not_bidirectional_iterator {
    struct some_value {};

    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using reference = some_value&;
    using value_type = some_value;
    using pointer = void;

    auto operator*() -> reference;

    auto operator++() -> not_bidirectional_iterator&;
    auto operator++(int) -> not_bidirectional_iterator;
    auto operator--() -> not_bidirectional_iterator&;
    auto operator==(not_bidirectional_iterator const&) -> bool;
    auto operator!=(not_bidirectional_iterator const&) -> bool;
  };

  static_assert(
      std::is_same<not_bidirectional_iterator::iterator_category, std::bidirectional_iterator_tag>::value,
      "Declares bidirectional iterator category"
  );
  static_assert(!std::is_same<void, not_bidirectional_iterator::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_bidirectional_iterator::reference>::value, "not void");
  static_assert(!std::is_same<void, not_bidirectional_iterator::value_type>::value, "not void");
  static_assert(std::is_same<void, not_bidirectional_iterator::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_bidirectional_iterator>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_bidirectional_iterator>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<not_bidirectional_iterator>, "Must be a forward iterator");
  static_assert(
      !arene::base::is_bidirectional_iterator_v<not_bidirectional_iterator>,
      "Must not be a bidirectional iterator"
  );
}

/// @test `is_bidirectional_iterator_v` is `false` when instantiated with a custom type that implements the
/// bidirectional iterator functions and provides the appropriate type aliases, except the pre-increment operator has
/// the wrong signature
TEST(IteratorCategoryTraits, WrongPreDecrementMeansNotBidirectionalIterator) {
  struct not_bidirectional_iterator {
    struct some_value {};

    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using reference = some_value&;
    using value_type = some_value;
    using pointer = void;

    auto operator*() -> reference;

    auto operator++() -> not_bidirectional_iterator&;
    auto operator++(int) -> not_bidirectional_iterator;
    auto operator--() -> not_bidirectional_iterator;
    auto operator--(int) -> not_bidirectional_iterator;
    auto operator==(not_bidirectional_iterator const&) -> bool;
    auto operator!=(not_bidirectional_iterator const&) -> bool;
  };

  static_assert(
      std::is_same<not_bidirectional_iterator::iterator_category, std::bidirectional_iterator_tag>::value,
      "Declares bidirectional iterator category"
  );
  static_assert(!std::is_same<void, not_bidirectional_iterator::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_bidirectional_iterator::reference>::value, "not void");
  static_assert(!std::is_same<void, not_bidirectional_iterator::value_type>::value, "not void");
  static_assert(std::is_same<void, not_bidirectional_iterator::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_bidirectional_iterator>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_bidirectional_iterator>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<not_bidirectional_iterator>, "Must be a forward iterator");
  static_assert(
      !arene::base::is_bidirectional_iterator_v<not_bidirectional_iterator>,
      "Must not be a bidirectional iterator"
  );
}

/// @test `is_bidirectional_iterator_v` is `false` when instantiated with a custom type that implements the
/// bidirectional iterator functions and provides the appropriate type aliases, except the post-increment operator has
/// the wrong signature
TEST(IteratorCategoryTraits, WrongPostDecrementMeansNotBidirectionalIterator) {
  struct not_bidirectional_iterator {
    struct some_value {};

    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using reference = some_value&;
    using value_type = some_value;
    using pointer = void;

    auto operator*() -> reference;

    auto operator++() -> not_bidirectional_iterator&;
    auto operator++(int) -> not_bidirectional_iterator;
    auto operator--() -> not_bidirectional_iterator&;
    auto operator--(int) -> not_bidirectional_iterator&;
    auto operator==(not_bidirectional_iterator const&) -> bool;
    auto operator!=(not_bidirectional_iterator const&) -> bool;
  };

  static_assert(
      std::is_same<not_bidirectional_iterator::iterator_category, std::bidirectional_iterator_tag>::value,
      "Declares bidirectional iterator category"
  );
  static_assert(!std::is_same<void, not_bidirectional_iterator::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_bidirectional_iterator::reference>::value, "not void");
  static_assert(!std::is_same<void, not_bidirectional_iterator::value_type>::value, "not void");
  static_assert(std::is_same<void, not_bidirectional_iterator::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_bidirectional_iterator>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_bidirectional_iterator>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<not_bidirectional_iterator>, "Must be a forward iterator");
  static_assert(
      !arene::base::is_bidirectional_iterator_v<not_bidirectional_iterator>,
      "Must not be a bidirectional iterator"
  );
}

namespace {
struct basic_random_access_iterator {
  struct some_value {};

  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = some_value&;
  using value_type = some_value;
  using pointer = void;

  auto operator*() -> reference;

  auto operator++() -> basic_random_access_iterator&;
  auto operator++(int) -> basic_random_access_iterator;
  auto operator--() -> basic_random_access_iterator&;
  auto operator--(int) -> basic_random_access_iterator;
  auto operator==(basic_random_access_iterator const&) -> bool;
  auto operator!=(basic_random_access_iterator const&) -> bool;

  auto operator+=(difference_type) -> basic_random_access_iterator&;

  friend auto operator+(basic_random_access_iterator, difference_type) -> basic_random_access_iterator { return {}; }
  friend auto operator+(difference_type, basic_random_access_iterator) -> basic_random_access_iterator { return {}; }

  auto operator-=(difference_type) -> basic_random_access_iterator&;

  friend auto operator-(basic_random_access_iterator, difference_type) -> basic_random_access_iterator { return {}; }

  friend auto operator-(basic_random_access_iterator, basic_random_access_iterator) -> difference_type { return 0; }

  auto operator[](difference_type) -> reference;

  friend auto operator<(basic_random_access_iterator, basic_random_access_iterator) -> bool { return false; }
  friend auto operator>(basic_random_access_iterator, basic_random_access_iterator) -> bool { return false; }
  friend auto operator<=(basic_random_access_iterator, basic_random_access_iterator) -> bool { return false; }
  friend auto operator>=(basic_random_access_iterator, basic_random_access_iterator) -> bool { return false; }
};

}  // namespace

/// @test `is_random_access_iterator_v` is `true` when instantiated with a custom type that implements the random access
/// iterator functions and provides the appropriate type aliases
TEST(IteratorCategoryTraits, MinimalRandomAccessIterator) {
  basic_random_access_iterator const itr;
  itr + 3;
  3 + itr;
  itr - 3;
  itr - itr;
  static_cast<void>(itr < itr);
  static_cast<void>(itr > itr);
  static_cast<void>(itr <= itr);
  static_cast<void>(itr >= itr);

  static_assert(
      std::is_same<basic_random_access_iterator::iterator_category, std::random_access_iterator_tag>::value,
      "Declares random access iterator category"
  );
  static_assert(!std::is_same<void, basic_random_access_iterator::difference_type>::value, "not void");
  static_assert(!std::is_same<void, basic_random_access_iterator::reference>::value, "not void");
  static_assert(!std::is_same<void, basic_random_access_iterator::value_type>::value, "not void");
  static_assert(std::is_same<void, basic_random_access_iterator::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<basic_random_access_iterator>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<basic_random_access_iterator>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<basic_random_access_iterator>, "Must be a forward iterator");
  static_assert(
      arene::base::is_bidirectional_iterator_v<basic_random_access_iterator>,
      "Must be a bidirectional iterator"
  );
  static_assert(
      arene::base::is_random_access_iterator_v<basic_random_access_iterator>,
      "Must be a random access iterator"
  );
}

namespace {
struct not_random_access_iterator1 {
  struct some_value {};

  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = some_value&;
  using value_type = some_value;
  using pointer = void;

  auto operator*() -> reference;

  auto operator++() -> not_random_access_iterator1&;
  auto operator++(int) -> not_random_access_iterator1;
  auto operator--() -> not_random_access_iterator1&;
  auto operator--(int) -> not_random_access_iterator1;
  auto operator==(not_random_access_iterator1 const&) -> bool;
  auto operator!=(not_random_access_iterator1 const&) -> bool;

  friend auto operator+(not_random_access_iterator1, difference_type) -> not_random_access_iterator1 { return {}; }
  friend auto operator+(difference_type, not_random_access_iterator1) -> not_random_access_iterator1 { return {}; }

  auto operator-=(difference_type) -> not_random_access_iterator1&;

  friend auto operator-(not_random_access_iterator1, difference_type) -> not_random_access_iterator1 { return {}; }

  friend auto operator-(not_random_access_iterator1, not_random_access_iterator1) -> difference_type { return 0; }

  auto operator[](difference_type) -> reference;

  friend auto operator<(not_random_access_iterator1, not_random_access_iterator1) -> bool { return false; }
  friend auto operator>(not_random_access_iterator1, not_random_access_iterator1) -> bool { return false; }
  friend auto operator<=(not_random_access_iterator1, not_random_access_iterator1) -> bool { return false; }
  friend auto operator>=(not_random_access_iterator1, not_random_access_iterator1) -> bool { return false; }
};

}  // namespace

/// @test `is_random_access_iterator_v` is `false` when instantiated with a custom type that implements the random
/// access iterator functions and provides the appropriate type aliases, except there is no `+=` operator
TEST(IteratorCategoryTraits, NoPlusEqualsMeansNotRandomAccessIterator) {
  not_random_access_iterator1 const itr;
  itr + 3;
  3 + itr;
  itr - 3;
  itr - itr;
  static_cast<void>(itr < itr);
  static_cast<void>(itr > itr);
  static_cast<void>(itr <= itr);
  static_cast<void>(itr >= itr);

  static_assert(
      std::is_same<not_random_access_iterator1::iterator_category, std::random_access_iterator_tag>::value,
      "Declares random access iterator category"
  );
  static_assert(!std::is_same<void, not_random_access_iterator1::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator1::reference>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator1::value_type>::value, "not void");
  static_assert(std::is_same<void, not_random_access_iterator1::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_random_access_iterator1>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_random_access_iterator1>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<not_random_access_iterator1>, "Must be a forward iterator");
  static_assert(
      arene::base::is_bidirectional_iterator_v<not_random_access_iterator1>,
      "Must be a bidirectional iterator"
  );
  static_assert(
      !arene::base::is_random_access_iterator_v<not_random_access_iterator1>,
      "Must not be a random access iterator"
  );
}

namespace {
struct not_random_access_iterator2 {
  struct some_value {};

  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = some_value&;
  using value_type = some_value;
  using pointer = void;

  auto operator*() -> reference;

  auto operator+=(difference_type) -> not_random_access_iterator2;

  auto operator++() -> not_random_access_iterator2&;
  auto operator++(int) -> not_random_access_iterator2;
  auto operator--() -> not_random_access_iterator2&;
  auto operator--(int) -> not_random_access_iterator2;
  auto operator==(not_random_access_iterator2 const&) -> bool;
  auto operator!=(not_random_access_iterator2 const&) -> bool;

  friend auto operator+(not_random_access_iterator2, difference_type) -> not_random_access_iterator2 { return {}; }
  friend auto operator+(difference_type, not_random_access_iterator2) -> not_random_access_iterator2 { return {}; }

  auto operator-=(difference_type) -> not_random_access_iterator2&;

  friend auto operator-(not_random_access_iterator2, difference_type) -> not_random_access_iterator2 { return {}; }

  friend auto operator-(not_random_access_iterator2, not_random_access_iterator2) -> difference_type { return 0; }

  auto operator[](difference_type) -> reference;

  friend auto operator<(not_random_access_iterator2, not_random_access_iterator2) -> bool { return false; }
  friend auto operator>(not_random_access_iterator2, not_random_access_iterator2) -> bool { return false; }
  friend auto operator<=(not_random_access_iterator2, not_random_access_iterator2) -> bool { return false; }
  friend auto operator>=(not_random_access_iterator2, not_random_access_iterator2) -> bool { return false; }
};

}  // namespace

/// @test `is_random_access_iterator_v` is `false` when instantiated with a custom type that implements the random
/// access iterator functions and provides the appropriate type aliases, except the `+=` operator has the wrong
/// signature
TEST(IteratorCategoryTraits, WrongPlusEqualsReturnMeansNotRandomAccessIterator) {
  not_random_access_iterator2 const itr;
  itr + 3;
  3 + itr;
  itr - 3;
  itr - itr;
  static_cast<void>(itr < itr);
  static_cast<void>(itr > itr);
  static_cast<void>(itr <= itr);
  static_cast<void>(itr >= itr);

  static_assert(
      std::is_same<not_random_access_iterator2::iterator_category, std::random_access_iterator_tag>::value,
      "Declares random access iterator category"
  );
  static_assert(!std::is_same<void, not_random_access_iterator2::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator2::reference>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator2::value_type>::value, "not void");
  static_assert(std::is_same<void, not_random_access_iterator2::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_random_access_iterator2>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_random_access_iterator2>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<not_random_access_iterator2>, "Must be a forward iterator");
  static_assert(
      arene::base::is_bidirectional_iterator_v<not_random_access_iterator2>,
      "Must be a bidirectional iterator"
  );
  static_assert(
      !arene::base::is_random_access_iterator_v<not_random_access_iterator2>,
      "Must not be a random access iterator"
  );
}

namespace {
struct not_random_access_iterator3 {
  struct some_value {};

  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = some_value&;
  using value_type = some_value;
  using pointer = void;

  auto operator*() -> reference;

  auto operator++() -> not_random_access_iterator3&;
  auto operator++(int) -> not_random_access_iterator3;
  auto operator--() -> not_random_access_iterator3&;
  auto operator--(int) -> not_random_access_iterator3;
  auto operator==(not_random_access_iterator3 const&) -> bool;
  auto operator!=(not_random_access_iterator3 const&) -> bool;

  auto operator+=(difference_type) -> not_random_access_iterator3&;

  friend auto operator+(difference_type, not_random_access_iterator3) -> not_random_access_iterator3 { return {}; }

  auto operator-=(difference_type) -> not_random_access_iterator3&;

  friend auto operator-(not_random_access_iterator3, difference_type) -> not_random_access_iterator3 { return {}; }

  friend auto operator-(not_random_access_iterator3, not_random_access_iterator3) -> difference_type { return 0; }

  auto operator[](difference_type) -> reference;

  friend auto operator<(not_random_access_iterator3, not_random_access_iterator3) -> bool { return false; }
  friend auto operator>(not_random_access_iterator3, not_random_access_iterator3) -> bool { return false; }
  friend auto operator<=(not_random_access_iterator3, not_random_access_iterator3) -> bool { return false; }
  friend auto operator>=(not_random_access_iterator3, not_random_access_iterator3) -> bool { return false; }
};

}  // namespace

/// @test `is_random_access_iterator_v` is `false` when instantiated with a custom type that implements the random
/// access iterator functions and provides the appropriate type aliases, except there is no `+` operator
TEST(IteratorCategoryTraits, NoIterPlusNMeansNotRandomAccessIterator) {
  not_random_access_iterator3 const itr;
  3 + itr;
  itr - 3;
  itr - itr;
  static_cast<void>(itr < itr);
  static_cast<void>(itr > itr);
  static_cast<void>(itr <= itr);
  static_cast<void>(itr >= itr);

  static_assert(
      std::is_same<not_random_access_iterator3::iterator_category, std::random_access_iterator_tag>::value,
      "Declares random access iterator category"
  );
  static_assert(!std::is_same<void, not_random_access_iterator3::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator3::reference>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator3::value_type>::value, "not void");
  static_assert(std::is_same<void, not_random_access_iterator3::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_random_access_iterator3>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_random_access_iterator3>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<not_random_access_iterator3>, "Must be a forward iterator");
  static_assert(
      arene::base::is_bidirectional_iterator_v<not_random_access_iterator3>,
      "Must be a bidirectional iterator"
  );
  static_assert(
      !arene::base::is_random_access_iterator_v<not_random_access_iterator3>,
      "Must not be a random access iterator"
  );
}

namespace {
struct not_random_access_iterator4 {
  struct some_value {};

  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = some_value&;
  using value_type = some_value;
  using pointer = void;

  auto operator*() -> reference;

  auto operator++() -> not_random_access_iterator4&;
  auto operator++(int) -> not_random_access_iterator4;
  auto operator--() -> not_random_access_iterator4&;
  auto operator--(int) -> not_random_access_iterator4;
  auto operator==(not_random_access_iterator4 const&) -> bool;
  auto operator!=(not_random_access_iterator4 const&) -> bool;

  auto operator+=(difference_type) -> not_random_access_iterator4&;

  friend auto operator+(not_random_access_iterator4, difference_type) -> difference_type { return 0; }
  friend auto operator+(difference_type, not_random_access_iterator4) -> not_random_access_iterator4 { return {}; }

  auto operator-=(difference_type) -> not_random_access_iterator4&;

  friend auto operator-(not_random_access_iterator4, difference_type) -> not_random_access_iterator4 { return {}; }

  friend auto operator-(not_random_access_iterator4, not_random_access_iterator4) -> difference_type { return 0; }

  auto operator[](difference_type) -> reference;

  friend auto operator<(not_random_access_iterator4, not_random_access_iterator4) -> bool { return false; }
  friend auto operator>(not_random_access_iterator4, not_random_access_iterator4) -> bool { return false; }
  friend auto operator<=(not_random_access_iterator4, not_random_access_iterator4) -> bool { return false; }
  friend auto operator>=(not_random_access_iterator4, not_random_access_iterator4) -> bool { return false; }
};

}  // namespace

/// @test `is_random_access_iterator_v` is `false` when instantiated with a custom type that implements the random
/// access iterator functions and provides the appropriate type aliases, except the `+` operator has the wrong signature
TEST(IteratorCategoryTraits, WrongIterPlusNMeansNotRandomAccessIterator) {
  not_random_access_iterator4 const itr;
  itr + 3;
  3 + itr;
  itr - 3;
  itr - itr;
  static_cast<void>(itr < itr);
  static_cast<void>(itr > itr);
  static_cast<void>(itr <= itr);
  static_cast<void>(itr >= itr);

  static_assert(
      std::is_same<not_random_access_iterator4::iterator_category, std::random_access_iterator_tag>::value,
      "Declares random access iterator category"
  );
  static_assert(!std::is_same<void, not_random_access_iterator4::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator4::reference>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator4::value_type>::value, "not void");
  static_assert(std::is_same<void, not_random_access_iterator4::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_random_access_iterator4>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_random_access_iterator4>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<not_random_access_iterator4>, "Must be a forward iterator");
  static_assert(
      arene::base::is_bidirectional_iterator_v<not_random_access_iterator4>,
      "Must be a bidirectional iterator"
  );
  static_assert(
      !arene::base::is_random_access_iterator_v<not_random_access_iterator4>,
      "Must not be a random access iterator"
  );
}

namespace {
struct not_random_access_iterator5 {
  struct some_value {};

  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = some_value&;
  using value_type = some_value;
  using pointer = void;

  auto operator*() -> reference;

  auto operator++() -> not_random_access_iterator5&;
  auto operator++(int) -> not_random_access_iterator5;
  auto operator--() -> not_random_access_iterator5&;
  auto operator--(int) -> not_random_access_iterator5;
  auto operator==(not_random_access_iterator5 const&) -> bool;
  auto operator!=(not_random_access_iterator5 const&) -> bool;

  auto operator+=(difference_type) -> not_random_access_iterator5&;

  friend auto operator+(not_random_access_iterator5, difference_type) -> not_random_access_iterator5 { return {}; }

  auto operator-=(difference_type) -> not_random_access_iterator5&;

  friend auto operator-(not_random_access_iterator5, difference_type) -> not_random_access_iterator5 { return {}; }

  friend auto operator-(not_random_access_iterator5, not_random_access_iterator5) -> difference_type { return 0; }

  auto operator[](difference_type) -> reference;

  friend auto operator<(not_random_access_iterator5, not_random_access_iterator5) -> bool { return false; }
  friend auto operator>(not_random_access_iterator5, not_random_access_iterator5) -> bool { return false; }
  friend auto operator<=(not_random_access_iterator5, not_random_access_iterator5) -> bool { return false; }
  friend auto operator>=(not_random_access_iterator5, not_random_access_iterator5) -> bool { return false; }
};

}  // namespace

/// @test `is_random_access_iterator_v` is `false` when instantiated with a custom type that implements the random
/// access iterator functions and provides the appropriate type aliases, except there is no `+` operator with a count on
/// the left-hand side
TEST(IteratorCategoryTraits, NoNPlusIterMeansNotRandomAccessIterator) {
  not_random_access_iterator5 const itr;
  itr + 3;
  itr - 3;
  itr - itr;
  static_cast<void>(itr < itr);
  static_cast<void>(itr > itr);
  static_cast<void>(itr <= itr);
  static_cast<void>(itr >= itr);

  static_assert(
      std::is_same<not_random_access_iterator5::iterator_category, std::random_access_iterator_tag>::value,
      "Declares random access iterator category"
  );
  static_assert(!std::is_same<void, not_random_access_iterator5::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator5::reference>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator5::value_type>::value, "not void");
  static_assert(std::is_same<void, not_random_access_iterator5::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_random_access_iterator5>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_random_access_iterator5>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<not_random_access_iterator5>, "Must be a forward iterator");
  static_assert(
      arene::base::is_bidirectional_iterator_v<not_random_access_iterator5>,
      "Must be a bidirectional iterator"
  );
  static_assert(
      !arene::base::is_random_access_iterator_v<not_random_access_iterator5>,
      "Must not be a random access iterator"
  );
}

namespace {
struct not_random_access_iterator6 {
  struct some_value {};

  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = some_value&;
  using value_type = some_value;
  using pointer = void;

  auto operator*() -> reference;

  auto operator++() -> not_random_access_iterator6&;
  auto operator++(int) -> not_random_access_iterator6;
  auto operator--() -> not_random_access_iterator6&;
  auto operator--(int) -> not_random_access_iterator6;
  auto operator==(not_random_access_iterator6 const&) -> bool;
  auto operator!=(not_random_access_iterator6 const&) -> bool;

  auto operator+=(difference_type) -> not_random_access_iterator6&;

  friend auto operator+(not_random_access_iterator6, difference_type) -> not_random_access_iterator6 { return {}; }

  friend auto operator+(difference_type, not_random_access_iterator6) -> difference_type { return 0; }

  auto operator-=(difference_type) -> not_random_access_iterator6&;

  friend auto operator-(not_random_access_iterator6, difference_type) -> not_random_access_iterator6 { return {}; }

  friend auto operator-(not_random_access_iterator6, not_random_access_iterator6) -> difference_type { return 0; }

  auto operator[](difference_type) -> reference;

  friend auto operator<(not_random_access_iterator6, not_random_access_iterator6) -> bool { return false; }
  friend auto operator>(not_random_access_iterator6, not_random_access_iterator6) -> bool { return false; }
  friend auto operator<=(not_random_access_iterator6, not_random_access_iterator6) -> bool { return false; }
  friend auto operator>=(not_random_access_iterator6, not_random_access_iterator6) -> bool { return false; }
};

}  // namespace

/// @test `is_random_access_iterator_v` is `false` when instantiated with a custom type that implements the random
/// access iterator functions and provides the appropriate type aliases, except the `+` operator with the count on the
/// left hand side has the wrong signature
TEST(IteratorCategoryTraits, WrongNPlusIterMeansNotRandomAccessIterator) {
  not_random_access_iterator6 const itr;
  itr + 3;
  3 + itr;
  itr - 3;
  itr - itr;
  static_cast<void>(itr < itr);
  static_cast<void>(itr > itr);
  static_cast<void>(itr <= itr);
  static_cast<void>(itr >= itr);

  static_assert(
      std::is_same<not_random_access_iterator6::iterator_category, std::random_access_iterator_tag>::value,
      "Declares random access iterator category"
  );
  static_assert(!std::is_same<void, not_random_access_iterator6::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator6::reference>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator6::value_type>::value, "not void");
  static_assert(std::is_same<void, not_random_access_iterator6::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_random_access_iterator6>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_random_access_iterator6>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<not_random_access_iterator6>, "Must be a forward iterator");
  static_assert(
      arene::base::is_bidirectional_iterator_v<not_random_access_iterator6>,
      "Must be a bidirectional iterator"
  );
  static_assert(
      !arene::base::is_random_access_iterator_v<not_random_access_iterator6>,
      "Must not be a random access iterator"
  );
}

namespace {
struct not_random_access_iterator7 {
  struct some_value {};

  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = some_value&;
  using value_type = some_value;
  using pointer = void;

  auto operator*() -> reference;

  auto operator++() -> not_random_access_iterator7&;
  auto operator++(int) -> not_random_access_iterator7;
  auto operator--() -> not_random_access_iterator7&;
  auto operator--(int) -> not_random_access_iterator7;
  auto operator==(not_random_access_iterator7 const&) -> bool;
  auto operator!=(not_random_access_iterator7 const&) -> bool;

  auto operator+=(difference_type) -> not_random_access_iterator7&;

  friend auto operator+(not_random_access_iterator7, difference_type) -> not_random_access_iterator7 { return {}; }

  friend auto operator+(difference_type, not_random_access_iterator7) -> not_random_access_iterator7 { return {}; }

  friend auto operator-(not_random_access_iterator7, difference_type) -> not_random_access_iterator7 { return {}; }

  friend auto operator-(not_random_access_iterator7, not_random_access_iterator7) -> difference_type { return 0; }

  auto operator[](difference_type) -> reference;

  friend auto operator<(not_random_access_iterator7, not_random_access_iterator7) -> bool { return false; }
  friend auto operator>(not_random_access_iterator7, not_random_access_iterator7) -> bool { return false; }
  friend auto operator<=(not_random_access_iterator7, not_random_access_iterator7) -> bool { return false; }
  friend auto operator>=(not_random_access_iterator7, not_random_access_iterator7) -> bool { return false; }
};

}  // namespace

/// @test `is_random_access_iterator_v` is `false` when instantiated with a custom type that implements the random
/// access iterator functions and provides the appropriate type aliases, except there is no `-=` operator
TEST(IteratorCategoryTraits, NoMinusEqualsMeansNotRandomAccessIterator) {
  not_random_access_iterator7 const itr;
  itr + 3;
  3 + itr;
  itr - 3;
  itr - itr;
  static_cast<void>(itr < itr);
  static_cast<void>(itr > itr);
  static_cast<void>(itr <= itr);
  static_cast<void>(itr >= itr);

  static_assert(
      std::is_same<not_random_access_iterator7::iterator_category, std::random_access_iterator_tag>::value,
      "Declares random access iterator category"
  );
  static_assert(!std::is_same<void, not_random_access_iterator7::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator7::reference>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator7::value_type>::value, "not void");
  static_assert(std::is_same<void, not_random_access_iterator7::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_random_access_iterator7>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_random_access_iterator7>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<not_random_access_iterator7>, "Must be a forward iterator");
  static_assert(
      arene::base::is_bidirectional_iterator_v<not_random_access_iterator7>,
      "Must be a bidirectional iterator"
  );
  static_assert(
      !arene::base::is_random_access_iterator_v<not_random_access_iterator7>,
      "Must not be a random access iterator"
  );
}

namespace {
struct not_random_access_iterator8 {
  struct some_value {};

  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = some_value&;
  using value_type = some_value;
  using pointer = void;

  auto operator*() -> reference;

  auto operator++() -> not_random_access_iterator8&;
  auto operator++(int) -> not_random_access_iterator8;
  auto operator--() -> not_random_access_iterator8&;
  auto operator--(int) -> not_random_access_iterator8;
  auto operator==(not_random_access_iterator8 const&) -> bool;
  auto operator!=(not_random_access_iterator8 const&) -> bool;

  auto operator+=(difference_type) -> not_random_access_iterator8&;

  friend auto operator+(not_random_access_iterator8, difference_type) -> not_random_access_iterator8 { return {}; }

  friend auto operator+(difference_type, not_random_access_iterator8) -> not_random_access_iterator8 { return {}; }

  auto operator-=(difference_type) -> not_random_access_iterator8;

  friend auto operator-(not_random_access_iterator8, difference_type) -> not_random_access_iterator8 { return {}; }

  friend auto operator-(not_random_access_iterator8, not_random_access_iterator8) -> difference_type { return 0; }

  auto operator[](difference_type) -> reference;

  friend auto operator<(not_random_access_iterator8, not_random_access_iterator8) -> bool { return false; }
  friend auto operator>(not_random_access_iterator8, not_random_access_iterator8) -> bool { return false; }
  friend auto operator<=(not_random_access_iterator8, not_random_access_iterator8) -> bool { return false; }
  friend auto operator>=(not_random_access_iterator8, not_random_access_iterator8) -> bool { return false; }
};

}  // namespace

/// @test `is_random_access_iterator_v` is `false` when instantiated with a custom type that implements the random
/// access iterator functions and provides the appropriate type aliases, except the `-=` operator
TEST(IteratorCategoryTraits, WrongMinusEqualsMeansNotRandomAccessIterator) {
  not_random_access_iterator8 const itr;
  itr + 3;
  3 + itr;
  itr - 3;
  itr - itr;
  static_cast<void>(itr < itr);
  static_cast<void>(itr > itr);
  static_cast<void>(itr <= itr);
  static_cast<void>(itr >= itr);

  static_assert(
      std::is_same<not_random_access_iterator8::iterator_category, std::random_access_iterator_tag>::value,
      "Declares random access iterator category"
  );
  static_assert(!std::is_same<void, not_random_access_iterator8::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator8::reference>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator8::value_type>::value, "not void");
  static_assert(std::is_same<void, not_random_access_iterator8::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_random_access_iterator8>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_random_access_iterator8>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<not_random_access_iterator8>, "Must be a forward iterator");
  static_assert(
      arene::base::is_bidirectional_iterator_v<not_random_access_iterator8>,
      "Must be a bidirectional iterator"
  );
  static_assert(
      !arene::base::is_random_access_iterator_v<not_random_access_iterator8>,
      "Must not be a random access iterator"
  );
}

namespace {
struct not_random_access_iterator9 {
  struct some_value {};

  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = some_value&;
  using value_type = some_value;
  using pointer = void;

  auto operator*() -> reference;

  auto operator++() -> not_random_access_iterator9&;
  auto operator++(int) -> not_random_access_iterator9;
  auto operator--() -> not_random_access_iterator9&;
  auto operator--(int) -> not_random_access_iterator9;
  auto operator==(not_random_access_iterator9 const&) -> bool;
  auto operator!=(not_random_access_iterator9 const&) -> bool;

  auto operator+=(difference_type) -> not_random_access_iterator9&;

  friend auto operator+(not_random_access_iterator9, difference_type) -> not_random_access_iterator9 { return {}; }

  friend auto operator+(difference_type, not_random_access_iterator9) -> not_random_access_iterator9 { return {}; }

  auto operator-=(difference_type) -> not_random_access_iterator9&;

  friend auto operator-(not_random_access_iterator9, not_random_access_iterator9) -> difference_type { return 0; }

  auto operator[](difference_type) -> reference;

  friend auto operator<(not_random_access_iterator9, not_random_access_iterator9) -> bool { return false; }
  friend auto operator>(not_random_access_iterator9, not_random_access_iterator9) -> bool { return false; }
  friend auto operator<=(not_random_access_iterator9, not_random_access_iterator9) -> bool { return false; }
  friend auto operator>=(not_random_access_iterator9, not_random_access_iterator9) -> bool { return false; }
};

}  // namespace

/// @test `is_random_access_iterator_v` is `false` when instantiated with a custom type that implements the random
/// access iterator functions and provides the appropriate type aliases, except there is no `-` operator
TEST(IteratorCategoryTraits, NoIterMinusNMeansNotRandomAccessIterator) {
  not_random_access_iterator9 const itr;
  itr + 3;
  3 + itr;
  itr - itr;
  static_cast<void>(itr < itr);
  static_cast<void>(itr > itr);
  static_cast<void>(itr <= itr);
  static_cast<void>(itr >= itr);

  static_assert(
      std::is_same<not_random_access_iterator9::iterator_category, std::random_access_iterator_tag>::value,
      "Declares random access iterator category"
  );
  static_assert(!std::is_same<void, not_random_access_iterator9::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator9::reference>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator9::value_type>::value, "not void");
  static_assert(std::is_same<void, not_random_access_iterator9::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_random_access_iterator9>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_random_access_iterator9>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<not_random_access_iterator9>, "Must be a forward iterator");
  static_assert(
      arene::base::is_bidirectional_iterator_v<not_random_access_iterator9>,
      "Must be a bidirectional iterator"
  );
  static_assert(
      !arene::base::is_random_access_iterator_v<not_random_access_iterator9>,
      "Must not be a random access iterator"
  );
}

namespace {
struct not_random_access_iterator10 {
  struct some_value {};

  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = some_value&;
  using value_type = some_value;
  using pointer = void;

  auto operator*() -> reference;

  auto operator++() -> not_random_access_iterator10&;
  auto operator++(int) -> not_random_access_iterator10;
  auto operator--() -> not_random_access_iterator10&;
  auto operator--(int) -> not_random_access_iterator10;
  auto operator==(not_random_access_iterator10 const&) -> bool;
  auto operator!=(not_random_access_iterator10 const&) -> bool;

  auto operator+=(difference_type) -> not_random_access_iterator10&;

  friend auto operator+(not_random_access_iterator10, difference_type) -> not_random_access_iterator10 { return {}; }

  friend auto operator+(difference_type, not_random_access_iterator10) -> not_random_access_iterator10 { return {}; }

  auto operator-=(difference_type) -> not_random_access_iterator10&;

  friend auto operator-(not_random_access_iterator10, difference_type) -> difference_type { return 0; }

  friend auto operator-(not_random_access_iterator10, not_random_access_iterator10) -> difference_type { return 0; }

  auto operator[](difference_type) -> reference;

  friend auto operator<(not_random_access_iterator10, not_random_access_iterator10) -> bool { return false; }
  friend auto operator>(not_random_access_iterator10, not_random_access_iterator10) -> bool { return false; }
  friend auto operator<=(not_random_access_iterator10, not_random_access_iterator10) -> bool { return false; }
  friend auto operator>=(not_random_access_iterator10, not_random_access_iterator10) -> bool { return false; }
};

}  // namespace

/// @test `is_random_access_iterator_v` is `false` when instantiated with a custom type that implements the random
/// access iterator functions and provides the appropriate type aliases, except the `-` operator has the wrong signature
TEST(IteratorCategoryTraits, WrongIterMinusNMeansNotRandomAccessIterator) {
  not_random_access_iterator10 const itr;
  itr + 3;
  3 + itr;
  itr - 3;
  itr - itr;
  static_cast<void>(itr < itr);
  static_cast<void>(itr > itr);
  static_cast<void>(itr <= itr);
  static_cast<void>(itr >= itr);

  static_assert(
      std::is_same<not_random_access_iterator10::iterator_category, std::random_access_iterator_tag>::value,
      "Declares random access iterator category"
  );
  static_assert(!std::is_same<void, not_random_access_iterator10::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator10::reference>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator10::value_type>::value, "not void");
  static_assert(std::is_same<void, not_random_access_iterator10::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_random_access_iterator10>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_random_access_iterator10>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<not_random_access_iterator10>, "Must be a forward iterator");
  static_assert(
      arene::base::is_bidirectional_iterator_v<not_random_access_iterator10>,
      "Must be a bidirectional iterator"
  );
  static_assert(
      !arene::base::is_random_access_iterator_v<not_random_access_iterator10>,
      "Must not be a random access iterator"
  );
}

namespace {
struct not_random_access_iterator11 {
  struct some_value {};

  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = some_value&;
  using value_type = some_value;
  using pointer = void;

  auto operator*() -> reference;

  auto operator++() -> not_random_access_iterator11&;
  auto operator++(int) -> not_random_access_iterator11;
  auto operator--() -> not_random_access_iterator11&;
  auto operator--(int) -> not_random_access_iterator11;
  auto operator==(not_random_access_iterator11 const&) -> bool;
  auto operator!=(not_random_access_iterator11 const&) -> bool;

  auto operator+=(difference_type) -> not_random_access_iterator11&;

  friend auto operator+(not_random_access_iterator11, difference_type) -> not_random_access_iterator11 { return {}; }

  friend auto operator+(difference_type, not_random_access_iterator11) -> not_random_access_iterator11 { return {}; }

  auto operator-=(difference_type) -> not_random_access_iterator11&;

  friend auto operator-(not_random_access_iterator11, difference_type) -> not_random_access_iterator11 { return {}; }

  auto operator[](difference_type) -> reference;

  friend auto operator<(not_random_access_iterator11, not_random_access_iterator11) -> bool { return false; }
  friend auto operator>(not_random_access_iterator11, not_random_access_iterator11) -> bool { return false; }
  friend auto operator<=(not_random_access_iterator11, not_random_access_iterator11) -> bool { return false; }
  friend auto operator>=(not_random_access_iterator11, not_random_access_iterator11) -> bool { return false; }
};

}  // namespace

/// @test `is_random_access_iterator_v` is `false` when instantiated with a custom type that implements the random
/// access iterator functions and provides the appropriate type aliases, except there is no `-` operator
TEST(IteratorCategoryTraits, NoIterMinusIterMeansNotRandomAccessIterator) {
  not_random_access_iterator11 const itr;
  itr + 3;
  3 + itr;
  itr - 3;
  static_cast<void>(itr < itr);
  static_cast<void>(itr > itr);
  static_cast<void>(itr <= itr);
  static_cast<void>(itr >= itr);

  static_assert(
      std::is_same<not_random_access_iterator11::iterator_category, std::random_access_iterator_tag>::value,
      "Declares random access iterator category"
  );
  static_assert(!std::is_same<void, not_random_access_iterator11::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator11::reference>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator11::value_type>::value, "not void");
  static_assert(std::is_same<void, not_random_access_iterator11::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_random_access_iterator11>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_random_access_iterator11>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<not_random_access_iterator11>, "Must be a forward iterator");
  static_assert(
      arene::base::is_bidirectional_iterator_v<not_random_access_iterator11>,
      "Must be a bidirectional iterator"
  );
  static_assert(
      !arene::base::is_random_access_iterator_v<not_random_access_iterator11>,
      "Must not be a random access iterator"
  );
}

namespace {
struct not_random_access_iterator12 {
  struct some_value {};

  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = some_value&;
  using value_type = some_value;
  using pointer = void;

  auto operator*() -> reference;

  auto operator++() -> not_random_access_iterator12&;
  auto operator++(int) -> not_random_access_iterator12;
  auto operator--() -> not_random_access_iterator12&;
  auto operator--(int) -> not_random_access_iterator12;
  auto operator==(not_random_access_iterator12 const&) -> bool;
  auto operator!=(not_random_access_iterator12 const&) -> bool;

  auto operator+=(difference_type) -> not_random_access_iterator12&;

  friend auto operator+(not_random_access_iterator12, difference_type) -> not_random_access_iterator12 { return {}; }

  friend auto operator+(difference_type, not_random_access_iterator12) -> not_random_access_iterator12 { return {}; }

  auto operator-=(difference_type) -> not_random_access_iterator12&;

  friend auto operator-(not_random_access_iterator12, difference_type) -> not_random_access_iterator12 { return {}; }

  friend auto operator-(not_random_access_iterator12, not_random_access_iterator12) -> not_random_access_iterator12 {
    return {};
  }

  auto operator[](difference_type) -> reference;

  friend auto operator<(not_random_access_iterator12, not_random_access_iterator12) -> bool { return false; }
  friend auto operator>(not_random_access_iterator12, not_random_access_iterator12) -> bool { return false; }
  friend auto operator<=(not_random_access_iterator12, not_random_access_iterator12) -> bool { return false; }
  friend auto operator>=(not_random_access_iterator12, not_random_access_iterator12) -> bool { return false; }
};

}  // namespace

/// @test `is_random_access_iterator_v` is `false` when instantiated with a custom type that implements the random
/// access iterator functions and provides the appropriate type aliases, except the `-` operator has the wrong signature
TEST(IteratorCategoryTraits, WrongIterMinusIterMeansNotRandomAccessIterator) {
  not_random_access_iterator12 const itr;
  itr + 3;
  3 + itr;
  itr - 3;
  itr - itr;
  static_cast<void>(itr < itr);
  static_cast<void>(itr > itr);
  static_cast<void>(itr <= itr);
  static_cast<void>(itr >= itr);

  static_assert(
      std::is_same<not_random_access_iterator12::iterator_category, std::random_access_iterator_tag>::value,
      "Declares random access iterator category"
  );
  static_assert(!std::is_same<void, not_random_access_iterator12::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator12::reference>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator12::value_type>::value, "not void");
  static_assert(std::is_same<void, not_random_access_iterator12::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_random_access_iterator12>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_random_access_iterator12>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<not_random_access_iterator12>, "Must be a forward iterator");
  static_assert(
      arene::base::is_bidirectional_iterator_v<not_random_access_iterator12>,
      "Must be a bidirectional iterator"
  );
  static_assert(
      !arene::base::is_random_access_iterator_v<not_random_access_iterator12>,
      "Must not be a random access iterator"
  );
}

namespace {
struct not_random_access_iterator13 {
  struct some_value {};

  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = some_value&;
  using value_type = some_value;
  using pointer = void;

  auto operator*() -> reference;

  auto operator++() -> not_random_access_iterator13&;
  auto operator++(int) -> not_random_access_iterator13;
  auto operator--() -> not_random_access_iterator13&;
  auto operator--(int) -> not_random_access_iterator13;
  auto operator==(not_random_access_iterator13 const&) -> bool;
  auto operator!=(not_random_access_iterator13 const&) -> bool;

  auto operator+=(difference_type) -> not_random_access_iterator13&;

  friend auto operator+(not_random_access_iterator13, difference_type) -> not_random_access_iterator13 { return {}; }

  friend auto operator+(difference_type, not_random_access_iterator13) -> not_random_access_iterator13 { return {}; }

  auto operator-=(difference_type) -> not_random_access_iterator13&;

  friend auto operator-(not_random_access_iterator13, difference_type) -> not_random_access_iterator13 { return {}; }

  friend auto operator-(not_random_access_iterator13, not_random_access_iterator13) -> difference_type { return 0; }

  auto operator[](difference_type) -> reference;

  friend auto operator>(not_random_access_iterator13, not_random_access_iterator13) -> bool { return false; }
  friend auto operator<=(not_random_access_iterator13, not_random_access_iterator13) -> bool { return false; }
  friend auto operator>=(not_random_access_iterator13, not_random_access_iterator13) -> bool { return false; }
};

}  // namespace

/// @test `is_random_access_iterator_v` is `false` when instantiated with a custom type that implements the random
/// access iterator functions and provides the appropriate type aliases, except it is not less-than comparable
TEST(IteratorCategoryTraits, NotLessThanComparableMeansNotRandomAccessIterator) {
  not_random_access_iterator13 const itr;
  itr + 3;
  3 + itr;
  itr - 3;
  itr - itr;
  static_cast<void>(itr > itr);
  static_cast<void>(itr <= itr);
  static_cast<void>(itr >= itr);

  static_assert(
      std::is_same<not_random_access_iterator13::iterator_category, std::random_access_iterator_tag>::value,
      "Declares random access iterator category"
  );
  static_assert(!std::is_same<void, not_random_access_iterator13::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator13::reference>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator13::value_type>::value, "not void");
  static_assert(std::is_same<void, not_random_access_iterator13::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_random_access_iterator13>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_random_access_iterator13>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<not_random_access_iterator13>, "Must be a forward iterator");
  static_assert(
      arene::base::is_bidirectional_iterator_v<not_random_access_iterator13>,
      "Must be a bidirectional iterator"
  );
  static_assert(
      !arene::base::is_random_access_iterator_v<not_random_access_iterator13>,
      "Must not be a random access iterator"
  );
}

namespace {
struct not_random_access_iterator14 {
  struct some_value {};

  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = some_value&;
  using value_type = some_value;
  using pointer = void;

  auto operator*() -> reference;

  auto operator++() -> not_random_access_iterator14&;
  auto operator++(int) -> not_random_access_iterator14;
  auto operator--() -> not_random_access_iterator14&;
  auto operator--(int) -> not_random_access_iterator14;
  auto operator==(not_random_access_iterator14 const&) -> bool;
  auto operator!=(not_random_access_iterator14 const&) -> bool;

  auto operator+=(difference_type) -> not_random_access_iterator14&;

  friend auto operator+(not_random_access_iterator14, difference_type) -> not_random_access_iterator14 { return {}; }

  friend auto operator+(difference_type, not_random_access_iterator14) -> not_random_access_iterator14 { return {}; }

  auto operator-=(difference_type) -> not_random_access_iterator14&;

  friend auto operator-(not_random_access_iterator14, difference_type) -> not_random_access_iterator14 { return {}; }

  friend auto operator-(not_random_access_iterator14, not_random_access_iterator14) -> difference_type { return 0; }

  auto operator[](difference_type) -> reference;

  friend auto operator<(not_random_access_iterator14, not_random_access_iterator14) -> bool { return false; }
  friend auto operator<=(not_random_access_iterator14, not_random_access_iterator14) -> bool { return false; }
  friend auto operator>=(not_random_access_iterator14, not_random_access_iterator14) -> bool { return false; }
};

}  // namespace

/// @test `is_random_access_iterator_v` is `false` when instantiated with a custom type that implements the random
/// access iterator functions and provides the appropriate type aliases, except it is not greater-than comparable
TEST(IteratorCategoryTraits, NotGreaterThanComparableMeansNotRandomAccessIterator) {
  not_random_access_iterator14 const itr;
  itr + 3;
  3 + itr;
  itr - 3;
  itr - itr;
  static_cast<void>(itr < itr);
  static_cast<void>(itr <= itr);
  static_cast<void>(itr >= itr);

  static_assert(
      std::is_same<not_random_access_iterator14::iterator_category, std::random_access_iterator_tag>::value,
      "Declares random access iterator category"
  );
  static_assert(!std::is_same<void, not_random_access_iterator14::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator14::reference>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator14::value_type>::value, "not void");
  static_assert(std::is_same<void, not_random_access_iterator14::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_random_access_iterator14>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_random_access_iterator14>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<not_random_access_iterator14>, "Must be a forward iterator");
  static_assert(
      arene::base::is_bidirectional_iterator_v<not_random_access_iterator14>,
      "Must be a bidirectional iterator"
  );
  static_assert(
      !arene::base::is_random_access_iterator_v<not_random_access_iterator14>,
      "Must not be a random access iterator"
  );
}

namespace {
struct not_random_access_iterator15 {
  struct some_value {};

  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = some_value&;
  using value_type = some_value;
  using pointer = void;

  auto operator*() -> reference;

  auto operator++() -> not_random_access_iterator15&;
  auto operator++(int) -> not_random_access_iterator15;
  auto operator--() -> not_random_access_iterator15&;
  auto operator--(int) -> not_random_access_iterator15;
  auto operator==(not_random_access_iterator15 const&) -> bool;
  auto operator!=(not_random_access_iterator15 const&) -> bool;

  auto operator+=(difference_type) -> not_random_access_iterator15&;

  friend auto operator+(not_random_access_iterator15, difference_type) -> not_random_access_iterator15 { return {}; }

  friend auto operator+(difference_type, not_random_access_iterator15) -> not_random_access_iterator15 { return {}; }

  auto operator-=(difference_type) -> not_random_access_iterator15&;

  friend auto operator-(not_random_access_iterator15, difference_type) -> not_random_access_iterator15 { return {}; }

  friend auto operator-(not_random_access_iterator15, not_random_access_iterator15) -> difference_type { return 0; }

  auto operator[](difference_type) -> reference;

  friend auto operator<(not_random_access_iterator15, not_random_access_iterator15) -> bool { return false; }
  friend auto operator>(not_random_access_iterator15, not_random_access_iterator15) -> bool { return false; }
  friend auto operator>=(not_random_access_iterator15, not_random_access_iterator15) -> bool { return false; }
};

}  // namespace

/// @test `is_random_access_iterator_v` is `false` when instantiated with a custom type that implements the random
/// access iterator functions and provides the appropriate type aliases, except it is not less-than-or-equals comparable
TEST(IteratorCategoryTraits, NotLessThanOrEqualComparableMeansNotRandomAccessIterator) {
  not_random_access_iterator15 const itr;
  itr + 3;
  3 + itr;
  itr - 3;
  itr - itr;
  static_cast<void>(itr < itr);
  static_cast<void>(itr > itr);
  static_cast<void>(itr >= itr);

  static_assert(
      std::is_same<not_random_access_iterator15::iterator_category, std::random_access_iterator_tag>::value,
      "Declares random access iterator category"
  );
  static_assert(!std::is_same<void, not_random_access_iterator15::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator15::reference>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator15::value_type>::value, "not void");
  static_assert(std::is_same<void, not_random_access_iterator15::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_random_access_iterator15>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_random_access_iterator15>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<not_random_access_iterator15>, "Must be a forward iterator");
  static_assert(
      arene::base::is_bidirectional_iterator_v<not_random_access_iterator15>,
      "Must be a bidirectional iterator"
  );
  static_assert(
      !arene::base::is_random_access_iterator_v<not_random_access_iterator15>,
      "Must not be a random access iterator"
  );
}

namespace {
struct not_random_access_iterator16 {
  struct some_value {};

  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = some_value&;
  using value_type = some_value;
  using pointer = void;

  auto operator*() -> reference;

  auto operator++() -> not_random_access_iterator16&;
  auto operator++(int) -> not_random_access_iterator16;
  auto operator--() -> not_random_access_iterator16&;
  auto operator--(int) -> not_random_access_iterator16;
  auto operator==(not_random_access_iterator16 const&) -> bool;
  auto operator!=(not_random_access_iterator16 const&) -> bool;

  auto operator+=(difference_type) -> not_random_access_iterator16&;

  friend auto operator+(not_random_access_iterator16, difference_type) -> not_random_access_iterator16 { return {}; }

  friend auto operator+(difference_type, not_random_access_iterator16) -> not_random_access_iterator16 { return {}; }

  auto operator-=(difference_type) -> not_random_access_iterator16&;

  friend auto operator-(not_random_access_iterator16, difference_type) -> not_random_access_iterator16 { return {}; }

  friend auto operator-(not_random_access_iterator16, not_random_access_iterator16) -> difference_type { return 0; }

  auto operator[](difference_type) -> reference;

  friend auto operator<(not_random_access_iterator16, not_random_access_iterator16) -> bool { return false; }
  friend auto operator>(not_random_access_iterator16, not_random_access_iterator16) -> bool { return false; }
  friend auto operator<=(not_random_access_iterator16, not_random_access_iterator16) -> bool { return false; }
};

}  // namespace

/// @test `is_random_access_iterator_v` is `false` when instantiated with a custom type that implements the random
/// access iterator functions and provides the appropriate type aliases, except it is not greater-than-or-equals
/// comparable
TEST(IteratorCategoryTraits, NotGreaterThanOrEqualComparableMeansNotRandomAccessIterator) {
  not_random_access_iterator16 const itr;
  itr + 3;
  3 + itr;
  itr - 3;
  itr - itr;
  static_cast<void>(itr < itr);
  static_cast<void>(itr > itr);
  static_cast<void>(itr <= itr);

  static_assert(
      std::is_same<not_random_access_iterator16::iterator_category, std::random_access_iterator_tag>::value,
      "Declares random access iterator category"
  );
  static_assert(!std::is_same<void, not_random_access_iterator16::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator16::reference>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator16::value_type>::value, "not void");
  static_assert(std::is_same<void, not_random_access_iterator16::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_random_access_iterator16>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_random_access_iterator16>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<not_random_access_iterator16>, "Must be a forward iterator");
  static_assert(
      arene::base::is_bidirectional_iterator_v<not_random_access_iterator16>,
      "Must be a bidirectional iterator"
  );
  static_assert(
      !arene::base::is_random_access_iterator_v<not_random_access_iterator16>,
      "Must not be a random access iterator"
  );
}

namespace {
struct not_random_access_iterator17 {
  struct some_value {};

  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = some_value&;
  using value_type = some_value;
  using pointer = void;

  auto operator*() -> reference;

  auto operator++() -> not_random_access_iterator17&;
  auto operator++(int) -> not_random_access_iterator17;
  auto operator--() -> not_random_access_iterator17&;
  auto operator--(int) -> not_random_access_iterator17;
  auto operator==(not_random_access_iterator17 const&) -> bool;
  auto operator!=(not_random_access_iterator17 const&) -> bool;

  auto operator+=(difference_type) -> not_random_access_iterator17&;

  friend auto operator+(not_random_access_iterator17, difference_type) -> not_random_access_iterator17 { return {}; }

  friend auto operator+(difference_type, not_random_access_iterator17) -> not_random_access_iterator17 { return {}; }

  auto operator-=(difference_type) -> not_random_access_iterator17&;

  friend auto operator-(not_random_access_iterator17, difference_type) -> not_random_access_iterator17 { return {}; }

  friend auto operator-(not_random_access_iterator17, not_random_access_iterator17) -> difference_type { return 0; }

  friend auto operator<(not_random_access_iterator17, not_random_access_iterator17) -> bool { return false; }
  friend auto operator>(not_random_access_iterator17, not_random_access_iterator17) -> bool { return false; }
  friend auto operator<=(not_random_access_iterator17, not_random_access_iterator17) -> bool { return false; }
  friend auto operator>=(not_random_access_iterator17, not_random_access_iterator17) -> bool { return false; }
};

}  // namespace

/// @test `is_random_access_iterator_v` is `false` when instantiated with a custom type that implements the random
/// access iterator functions and provides the appropriate type aliases, except it is not indexable via `operator[]`
TEST(IteratorCategoryTraits, NotIndexableComparableMeansNotRandomAccessIterator) {
  not_random_access_iterator17 const itr;
  itr + 3;
  3 + itr;
  itr - 3;
  itr - itr;
  static_cast<void>(itr < itr);
  static_cast<void>(itr > itr);
  static_cast<void>(itr <= itr);
  static_cast<void>(itr >= itr);

  static_assert(
      std::is_same<not_random_access_iterator17::iterator_category, std::random_access_iterator_tag>::value,
      "Declares random access iterator category"
  );
  static_assert(!std::is_same<void, not_random_access_iterator17::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator17::reference>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator17::value_type>::value, "not void");
  static_assert(std::is_same<void, not_random_access_iterator17::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_random_access_iterator17>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_random_access_iterator17>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<not_random_access_iterator17>, "Must be a forward iterator");
  static_assert(
      arene::base::is_bidirectional_iterator_v<not_random_access_iterator17>,
      "Must be a bidirectional iterator"
  );
  static_assert(
      !arene::base::is_random_access_iterator_v<not_random_access_iterator17>,
      "Must not be a random access iterator"
  );
}

namespace {
struct not_random_access_iterator18 {
  struct some_value {};

  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using reference = some_value&;
  using value_type = some_value;
  using pointer = void;

  auto operator*() -> reference;

  auto operator++() -> not_random_access_iterator18&;
  auto operator++(int) -> not_random_access_iterator18;
  auto operator--() -> not_random_access_iterator18&;
  auto operator--(int) -> not_random_access_iterator18;
  auto operator==(not_random_access_iterator18 const&) -> bool;
  auto operator!=(not_random_access_iterator18 const&) -> bool;

  auto operator+=(difference_type) -> not_random_access_iterator18&;

  friend auto operator+(not_random_access_iterator18, difference_type) -> not_random_access_iterator18 { return {}; }

  friend auto operator+(difference_type, not_random_access_iterator18) -> not_random_access_iterator18 { return {}; }

  auto operator-=(difference_type) -> not_random_access_iterator18&;

  friend auto operator-(not_random_access_iterator18, difference_type) -> not_random_access_iterator18 { return {}; }

  friend auto operator-(not_random_access_iterator18, not_random_access_iterator18) -> difference_type { return 0; }

  auto operator[](difference_type) -> some_value;

  friend auto operator<(not_random_access_iterator18, not_random_access_iterator18) -> bool { return false; }
  friend auto operator>(not_random_access_iterator18, not_random_access_iterator18) -> bool { return false; }
  friend auto operator<=(not_random_access_iterator18, not_random_access_iterator18) -> bool { return false; }
  friend auto operator>=(not_random_access_iterator18, not_random_access_iterator18) -> bool { return false; }
};

}  // namespace

/// @test `is_random_access_iterator_v` is `false` when instantiated with a custom type that implements the random
/// access iterator functions and provides the appropriate type aliases, except `operator[]` has the wrong signature
TEST(IteratorCategoryTraits, WrongIndexResultComparableMeansNotRandomAccessIterator) {
  not_random_access_iterator18 const itr;
  itr + 3;
  3 + itr;
  itr - 3;
  itr - itr;
  static_cast<void>(itr < itr);
  static_cast<void>(itr > itr);
  static_cast<void>(itr <= itr);
  static_cast<void>(itr >= itr);

  static_assert(
      std::is_same<not_random_access_iterator18::iterator_category, std::random_access_iterator_tag>::value,
      "Declares random access iterator category"
  );
  static_assert(!std::is_same<void, not_random_access_iterator18::difference_type>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator18::reference>::value, "not void");
  static_assert(!std::is_same<void, not_random_access_iterator18::value_type>::value, "not void");
  static_assert(std::is_same<void, not_random_access_iterator18::pointer>::value, "void");
  static_assert(arene::base::is_iterator_v<not_random_access_iterator18>, "Must be an iterator");
  static_assert(arene::base::is_input_iterator_v<not_random_access_iterator18>, "Must be an input iterator");
  static_assert(arene::base::is_forward_iterator_v<not_random_access_iterator18>, "Must be a forward iterator");
  static_assert(
      arene::base::is_bidirectional_iterator_v<not_random_access_iterator18>,
      "Must be a bidirectional iterator"
  );
  static_assert(
      !arene::base::is_random_access_iterator_v<not_random_access_iterator18>,
      "Must not be a random access iterator"
  );
}

}  // namespace
