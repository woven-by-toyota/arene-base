// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/iterator"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"

namespace {

/// @test `input_iterator_tag` is not convertible to an other iterator tag
TEST(IteratorTags, InputIteratorTagNotConvertibleToAnyOtherIteratorTag) {
  ASSERT_TRUE(std::is_convertible_v<std::input_iterator_tag, std::input_iterator_tag>);
  ASSERT_FALSE(std::is_convertible_v<std::input_iterator_tag, std::output_iterator_tag>);
  ASSERT_FALSE(std::is_convertible_v<std::input_iterator_tag, std::forward_iterator_tag>);
  ASSERT_FALSE(std::is_convertible_v<std::input_iterator_tag, std::bidirectional_iterator_tag>);
  ASSERT_FALSE(std::is_convertible_v<std::input_iterator_tag, std::random_access_iterator_tag>);
}

/// @test `output_iterator_tag` is not convertible to an other iterator tag
TEST(IteratorTags, OutputIteratorTagNotConvertibleToAnyOtherIteratorTag) {
  ASSERT_FALSE(std::is_convertible_v<std::output_iterator_tag, std::input_iterator_tag>);
  ASSERT_TRUE(std::is_convertible_v<std::output_iterator_tag, std::output_iterator_tag>);
  ASSERT_FALSE(std::is_convertible_v<std::output_iterator_tag, std::forward_iterator_tag>);
  ASSERT_FALSE(std::is_convertible_v<std::output_iterator_tag, std::bidirectional_iterator_tag>);
  ASSERT_FALSE(std::is_convertible_v<std::output_iterator_tag, std::random_access_iterator_tag>);
}

/// @test `forward_iterator_tag` is convertible to `input_iterator_tag`
TEST(IteratorTags, ForwardIteratorTagConvertibleToInputIteratorTag) {
  ASSERT_TRUE(std::is_convertible_v<std::forward_iterator_tag, std::input_iterator_tag>);
  ASSERT_FALSE(std::is_convertible_v<std::forward_iterator_tag, std::output_iterator_tag>);
  ASSERT_TRUE(std::is_convertible_v<std::forward_iterator_tag, std::forward_iterator_tag>);
  ASSERT_FALSE(std::is_convertible_v<std::forward_iterator_tag, std::bidirectional_iterator_tag>);
  ASSERT_FALSE(std::is_convertible_v<std::forward_iterator_tag, std::random_access_iterator_tag>);
}

/// @test `bidirectional_iterator_tag` is convertible to `forward_iterator_tag` and weaker iterator tags
TEST(IteratorTags, BidirectionalIteratorTagConvertibleToForwardIteratorTagAndWeaker) {
  ASSERT_TRUE(std::is_convertible_v<std::bidirectional_iterator_tag, std::input_iterator_tag>);
  ASSERT_FALSE(std::is_convertible_v<std::bidirectional_iterator_tag, std::output_iterator_tag>);
  ASSERT_TRUE(std::is_convertible_v<std::bidirectional_iterator_tag, std::forward_iterator_tag>);
  ASSERT_TRUE(std::is_convertible_v<std::bidirectional_iterator_tag, std::bidirectional_iterator_tag>);
  ASSERT_FALSE(std::is_convertible_v<std::bidirectional_iterator_tag, std::random_access_iterator_tag>);
}

/// @test `random_access_iterator_tag` is convertible to `bidirectional_iterator_tag` and weaker iterator tags
TEST(IteratorTags, RandomAccessIteratorTagConvertibleToBidirectionalIteratorTagAndWeaker) {
  ASSERT_TRUE(std::is_convertible_v<std::random_access_iterator_tag, std::input_iterator_tag>);
  ASSERT_FALSE(std::is_convertible_v<std::random_access_iterator_tag, std::output_iterator_tag>);
  ASSERT_TRUE(std::is_convertible_v<std::random_access_iterator_tag, std::forward_iterator_tag>);
  ASSERT_TRUE(std::is_convertible_v<std::random_access_iterator_tag, std::bidirectional_iterator_tag>);
  ASSERT_TRUE(std::is_convertible_v<std::random_access_iterator_tag, std::random_access_iterator_tag>);
}

/// @test check that `input_iterator_tag` and stronger iterator tags are publicly derived from `input_iterator_tag`
TEST(IteratorTags, TagsDerivedFromInputIterator) {
  using testing::is_unambiguously_publicly_derived_from_v;

  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::input_iterator_tag, std::input_iterator_tag>);
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::forward_iterator_tag, std::input_iterator_tag>);
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::bidirectional_iterator_tag, std::input_iterator_tag>);
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::random_access_iterator_tag, std::input_iterator_tag>);
}

/// @test check that `forward_iterator_tag` and stronger iterator tags are publicly derived from `forward_iterator_tag`
TEST(IteratorTags, TagsDerivedFromForwardIterator) {
  using testing::is_unambiguously_publicly_derived_from_v;

  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::forward_iterator_tag, std::forward_iterator_tag>);
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::bidirectional_iterator_tag, std::forward_iterator_tag>);
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::random_access_iterator_tag, std::forward_iterator_tag>);
}

/// @test check that `bidirectional_iterator_tag` and stronger iterator tags are publicly derived from
/// `bidirectional_iterator_tag`
TEST(IteratorTags, TagsDerivedFromBidrectionalIterator) {
  using testing::is_unambiguously_publicly_derived_from_v;

  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<
              std::bidirectional_iterator_tag,
              std::bidirectional_iterator_tag>);
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<
              std::random_access_iterator_tag,
              std::bidirectional_iterator_tag>);
}

template <class... F>
struct overloaded_t {
  auto operator()() const -> void;
};

// NOLINTBEGIN(readability-identifier-length)

template <class F0, class... Fs>
// NOLINTNEXTLINE(fuchsia-multiple-inheritance)
struct overloaded_t<F0, Fs...>
    : F0
    , overloaded_t<Fs...> {
  using F0::operator();
  using overloaded_t<Fs...>::operator();

  constexpr explicit overloaded_t(F0 f0, Fs... fs)
      : F0{f0},
        overloaded_t<Fs...>{fs...} {}
};

template <class... F>
constexpr auto overloaded(F... f) -> overloaded_t<F...> {
  return overloaded_t<F...>{f...};
}

/// @test `input_iterator_tag` and stronger can be used to select an input iterator algorithm at compile time.
TEST(IteratorTags, InputAndStrongerCanSelectInputAlgorithm) {
  auto const f = overloaded(
      [](std::input_iterator_tag) { return std::true_type{}; },
      [](std::output_iterator_tag) { return std::false_type{}; }
  );

  ASSERT_TRUE(f(std::input_iterator_tag{}));
  ASSERT_FALSE(f(std::output_iterator_tag{}));
  ASSERT_TRUE(f(std::forward_iterator_tag{}));
  ASSERT_TRUE(f(std::bidirectional_iterator_tag{}));
  ASSERT_TRUE(f(std::random_access_iterator_tag{}));
}

/// @test `forward_iterator_tag` and stronger can be used to select a forward iterator algorithm at compile time.
TEST(IteratorTags, ForwardAndStrongerCanSelectForwardAlgorithm) {
  auto const f = overloaded(
      [](std::forward_iterator_tag) { return std::true_type{}; },
      [](std::input_iterator_tag) { return std::false_type{}; },
      [](std::output_iterator_tag) { return std::false_type{}; }
  );

  ASSERT_FALSE(f(std::input_iterator_tag{}));
  ASSERT_FALSE(f(std::output_iterator_tag{}));
  ASSERT_TRUE(f(std::forward_iterator_tag{}));
  ASSERT_TRUE(f(std::bidirectional_iterator_tag{}));
  ASSERT_TRUE(f(std::random_access_iterator_tag{}));
}

/// @test iterator tag can be used to select either a forward iterator algorithm
/// or random access algorithm at compile time.
TEST(IteratorTags, IteratorTagCanSelectForwardOrRandomAccessAlgorithm) {
  auto const f = overloaded(
      [](std::forward_iterator_tag) { return std::integral_constant<std::size_t, 1>{}; },
      [](std::random_access_iterator_tag) { return std::integral_constant<std::size_t, 2>{}; },
      [](std::input_iterator_tag) { return std::integral_constant<std::size_t, 0>{}; },
      [](std::output_iterator_tag) { return std::integral_constant<std::size_t, 0>{}; }
  );

  ASSERT_EQ(0, f(std::input_iterator_tag{}));
  ASSERT_EQ(0, f(std::output_iterator_tag{}));
  ASSERT_EQ(1, f(std::forward_iterator_tag{}));
  ASSERT_EQ(1, f(std::bidirectional_iterator_tag{}));
  ASSERT_EQ(2, f(std::random_access_iterator_tag{}));
}

template <typename T>
struct IteratorTags : ::testing::Test {};

using tag_types = ::testing::Types<
    std::input_iterator_tag,
    std::forward_iterator_tag,
    std::bidirectional_iterator_tag,
    std::random_access_iterator_tag>;

TYPED_TEST_SUITE(IteratorTags, tag_types, );

/// @test input and stronger iterator tags can be constructed and converted at compile time.
CONSTEXPR_TYPED_TEST(IteratorTags, InputTagsConstexprConvertible) {
  auto const _ = static_cast<std::input_iterator_tag>(TypeParam{});
  static_cast<void>(_);
}

/// @test output iterator tags can be constructed at compile time.
CONSTEXPR_TEST(IteratorTags, OutputTagConstexprConvertible) {
  auto const _ = std::output_iterator_tag{};
  static_cast<void>(_);
}

// NOLINTEND(readability-identifier-length)

}  // namespace
