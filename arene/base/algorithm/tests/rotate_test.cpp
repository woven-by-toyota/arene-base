// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/rotate.hpp"

#include <gmock/gmock.h>

#include "arene/base/algorithm/copy.hpp"
#include "arene/base/algorithm/equal.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/iterator/distance.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/iterator/prev.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "testlibs/utilities/configurable_value.hpp"
#include "testlibs/utilities/iterator_types.hpp"
// IWYU pragma: no_include "arene/base/utility/make_subrange.hpp"

namespace {
/// @test @c rotate an empty vector results in an empty vector.
TEST(Rotate, CanRotateEmptyVector) {
  auto values = arene::base::array<int, 0>{};
  arene::base::rotate(values.begin(), values.end(), values.end());
  SUCCEED();
}

/// @test @c rotate an array of one element results in the same vector.
TEST(Rotate, CanRotateVectorOf1) {
  auto values = arene::base::to_array({1});
  arene::base::rotate(values.begin(), values.end(), values.end());
  ASSERT_EQ(values.front(), 1);
  ASSERT_THAT(values, ::testing::ElementsAreArray({1}));
}

/// @test @c rotate an array of two elements results in the vector being reversed.
TEST(Rotate, CanRotateVectorOf2) {
  auto values = arene::base::to_array({1, 2});
  arene::base::rotate(values.begin(), values.end() - 1, values.end());
  ASSERT_THAT(values, ::testing::ElementsAreArray({2, 1}));
}

/// @test @c rotate an array of three elements results in the vector being rotated by one.
TEST(Rotate, CanRotateVectorOf3) {
  auto values = arene::base::to_array({1, 2, 3});
  arene::base::rotate(values.begin(), values.end() - 1, values.end());
  ASSERT_THAT(values, ::testing::ElementsAreArray({3, 1, 2}));
}

/// @test Partially @c rotate an array of three elements results in the vector being rotated by one.
TEST(Rotate, CanRotatePartiallyVectorOf3) {
  auto values = arene::base::to_array({1, 2, 3});
  arene::base::rotate(values.begin(), values.begin() + 1, values.begin() + 2);
  ASSERT_THAT(values, ::testing::ElementsAreArray({2, 1, 3}));
}

/// @test Partially @c rotate an array yields the correct result.
TEST(Rotate, CanRotateCommonUsage) {
  auto values = arene::base::to_array({1, 2, 3, 4, 5, 6, 7, 8, 9});
  constexpr auto shift = 4U;
  arene::base::rotate(values.begin(), values.begin() + shift, values.end());
  ASSERT_THAT(values, ::testing::ElementsAreArray({5, 6, 7, 8, 9, 1, 2, 3, 4}));
}

template <class I>
constexpr auto into_forward_iterator(I iter) {
  return testing::make_demoted_iterator<std::forward_iterator_tag>(std::move(iter));
}

/// @test @c rotate an array with forward iterators yields the correct result.
TEST(Rotate, CanRotateCommonUsageForwardIterator) {
  auto values = arene::base::to_array({1, 2, 3, 4, 5, 6, 7, 8, 9});

  constexpr auto shift = 4U;
  arene::base::rotate(  //
      into_forward_iterator(values.begin()),
      into_forward_iterator(values.begin() + shift),
      into_forward_iterator(values.end())
  );
  ASSERT_THAT(values, ::testing::ElementsAreArray({5, 6, 7, 8, 9, 1, 2, 3, 4}));
}

/// @test @c rotate an array using random access iterators and using forward iterators yields the same result.
TEST(Rotate, ForwardIteratorRotateAndRandomAccessRotateAreSame) {
  auto const source = arene::base::to_array({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});

  for (unsigned offset = 0; offset < source.size(); ++offset) {
    auto reference = source;
    auto const ref_result = arene::base::rotate(reference.begin(), reference.begin() + offset, reference.end());

    auto values = source;
    auto const val_result = arene::base::rotate(  //
        into_forward_iterator(values.begin()),
        into_forward_iterator(values.begin() + offset),
        into_forward_iterator(values.end())
    );

    ASSERT_THAT(values, ::testing::ElementsAreArray(reference));
    ASSERT_EQ(ref_result - reference.begin(), arene::base::distance(into_forward_iterator(values.begin()), val_result));
  }
}

/// @test @c rotate an array using random access iterators and using forward iterators yields the same result
/// for a prime size vector.
TEST(Rotate, ForwardIteratorRotateAndRandomAccessRotateAreSamePrimeSize) {
  auto const source = arene::base::to_array({1, 2, 3, 4, 5, 6, 7});

  for (unsigned offset = 0; offset < source.size(); ++offset) {
    auto reference = source;
    auto const ref_result = arene::base::rotate(reference.begin(), reference.begin() + offset, reference.end());

    auto values = source;
    auto const val_result = arene::base::rotate(  //
        into_forward_iterator(values.begin()),
        into_forward_iterator(values.begin() + offset),
        into_forward_iterator(values.end())
    );

    ASSERT_THAT(values, ::testing::ElementsAreArray(reference));
    ASSERT_EQ(ref_result - reference.begin(), arene::base::distance(into_forward_iterator(values.begin()), val_result));
  }
}

/// @test @c rotate a subset of an array yields the correct result.
TEST(Rotate, CanRotateSubrange) {
  auto values = arene::base::to_array({1, 2, 3, 4, 5, 6, 7, 8, 9});

  arene::base::rotate(values.begin() + 2, values.begin() + 4, values.begin() + 6);

  ASSERT_THAT(values, ::testing::ElementsAreArray({1, 2, 5, 6, 3, 4, 7, 8, 9}));
}

/// @test @c rotate an empty subset yields the same vector.
TEST(Rotate, CanRotateEmptySubrange) {
  auto values = arene::base::to_array({1, 2, 3, 4, 5, 6, 7, 8, 9});

  constexpr auto shift = 5U;
  arene::base::rotate(values.begin() + shift, values.begin() + shift, values.begin() + shift);

  ASSERT_THAT(values, ::testing::ElementsAreArray({1, 2, 3, 4, 5, 6, 7, 8, 9}));
}

/// @test @c rotate a subset of a range with forward iterators yields the correct result.
TEST(Rotate, CanRotateBidirectionalIterator) {
  auto values = arene::base::to_array({1, 2, 3, 4, 5, 6, 7, 8, 9});
  auto view = testing::demoted_view(std::bidirectional_iterator_tag{}, values);

  arene::base::rotate(
      arene::base::next(view.begin(), 2),
      arene::base::next(view.begin(), 4),
      arene::base::next(view.begin(), 6)
  );

  ASSERT_THAT(values, ::testing::ElementsAreArray({1, 2, 5, 6, 3, 4, 7, 8, 9}));
}

/// @test @c rotate a subset of an array with bidirectional iterators yields the correct result.
TEST(Rotate, SingleLeftRotateWithBidirectionalIterators) {
  auto values = arene::base::to_array({1, 2, 3, 4, 5, 6, 7, 8, 9});
  auto view = testing::demoted_view(std::bidirectional_iterator_tag{}, values);

  arene::base::rotate(view.begin(), arene::base::next(view.begin(), 1), view.end());

  ASSERT_THAT(values, ::testing::ElementsAreArray({2, 3, 4, 5, 6, 7, 8, 9, 1}));
}

/// @test @c rotate a subset of an array with bidirectional iterators yields the correct result.
TEST(Rotate, SingleRightRotateWithBidirectionalIterators) {
  auto values = arene::base::to_array({1, 2, 3, 4, 5, 6, 7, 8, 9});
  auto view = testing::demoted_view(std::bidirectional_iterator_tag{}, values);

  arene::base::rotate(view.begin(), arene::base::prev(view.end(), 1), view.end());

  ASSERT_THAT(view, ::testing::ElementsAreArray({9, 1, 2, 3, 4, 5, 6, 7, 8}));
}

/// @test @c rotate an array to the left by one with random access iterators yields the correct result.
TEST(Rotate, SingleLeftRotateWithRandomAccessIterators) {
  auto values = arene::base::to_array({1, 2, 3, 4, 5, 6, 7, 8, 9});

  arene::base::rotate(values.begin(), arene::base::next(values.begin(), 1), values.end());

  ASSERT_THAT(values, ::testing::ElementsAreArray({2, 3, 4, 5, 6, 7, 8, 9, 1}));
}

/// @test @c rotate an array to the right by one with random access iterators yields the correct result.
TEST(Rotate, SingleRightRotateWithRandomAccessIterators) {
  auto values = arene::base::to_array({1, 2, 3, 4, 5, 6, 7, 8, 9});

  arene::base::rotate(values.begin(), arene::base::prev(values.end(), 1), values.end());

  ASSERT_THAT(values, ::testing::ElementsAreArray({9, 1, 2, 3, 4, 5, 6, 7, 8}));
}

/// @test @c rotate an array by various amounts yields the correct result.
TEST(Rotate, RotateByVariousAmounts) {
  auto values = arene::base::to_array({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17});

  arene::base::rotate(values.begin(), arene::base::next(values.begin(), 2), values.end());
  ASSERT_THAT(values, ::testing::ElementsAreArray({3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 1, 2}));

  arene::base::rotate(values.begin(), arene::base::next(values.begin(), 13), values.end());
  ASSERT_THAT(values, ::testing::ElementsAreArray({16, 17, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}));

  arene::base::rotate(values.begin(), arene::base::next(values.begin(), 4), arene::base::prev(values.end()));
  ASSERT_THAT(values, ::testing::ElementsAreArray({3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 1, 2, 15}));
}

/// @test @c rotate an array ia a no-op when the rotation parameters indicate no movement is necessary.
TEST(Rotate, NoOpRotates) {
  auto values = arene::base::to_array({1, 2, 3, 4, 5, 6, 7, 8, 9});
  auto view = testing::demoted_view(std::bidirectional_iterator_tag{}, values);
  auto const reference = values;

  ASSERT_EQ(arene::base::rotate(view.begin(), view.begin(), view.end()), view.end());
  ASSERT_THAT(view, ::testing::ElementsAreArray(reference));

  ASSERT_EQ(arene::base::rotate(view.begin(), view.end(), view.end()), view.begin());
  ASSERT_THAT(view, ::testing::ElementsAreArray(reference));
}

/// @test @c rotate an array with move-only type.
TEST(Rotate, CanRotateMoveOnlyType) {
  using move_only_type = testing::configurable_value<
      int,
      testing::throws_on::nothing,
      testing::disable::copy_construct | testing::disable::copy_assign>;

  auto values = arene::base::array<move_only_type, 8>{
      move_only_type{1},
      move_only_type{2},
      move_only_type{3},
      move_only_type{4},
      move_only_type{5},
      move_only_type{6},
      move_only_type{7},
      move_only_type{8}
  };

  arene::base::rotate(
      arene::base::next(values.begin(), 2),
      arene::base::next(values.begin(), 4),
      arene::base::next(values.begin(), 6)
  );

  // 'ElementsAreArray' requires the value to be copyable so we copy into an
  // array of 'int's. 'configurable_value' is convertible to the
  // underlying type so this can be done with the 'copy' algorithm.
  auto copied_values = arene::base::array<int, 8>{};
  arene::base::copy(values.begin(), values.end(), copied_values.begin());
  ASSERT_THAT(copied_values, ::testing::ElementsAreArray({1, 2, 5, 6, 3, 4, 7, 8}));
}

/// @test @c rotate an array in a constexpr context.
CONSTEXPR_TEST(Rotate, RotateInConstexprContext) {
  auto values = arene::base::to_array({1, 2, 3, 4, 5, 6, 7, 8});

  arene::base::rotate(values.begin(), arene::base::next(values.begin(), 4), values.end());

  auto const reference = arene::base::to_array({5, 6, 7, 8, 1, 2, 3, 4});

  // Googletest assertion macros are not usable in a 'CONSTEXPR_TEST'
  CONSTEXPR_ASSERT(arene::base::equal(values.cbegin(), values.cend(), reference.cbegin(), reference.cend()));
}

/// @brief determine if @c rotate is invocable
/// @tparam Iter iterator value type
/// @tparam Category iterator category type
///
/// @return @c true if invocable, otherwise @c false
///
template <class IterValue, class Category>
auto is_invocable_with(Category) -> bool {
  using iterator_type = testing::demoted_iterator<IterValue*, Category>;
  return arene::base::is_invocable_v<decltype(arene::base::rotate), iterator_type, iterator_type, iterator_type>;
}

/// @test @c rotate is not invocable with iterator types with
/// category weaker than forward
///
TEST(Rotate, RequiresForwardIterator) {
  ASSERT_TRUE(is_invocable_with<int>(std::random_access_iterator_tag{}));
  ASSERT_TRUE(is_invocable_with<int>(std::bidirectional_iterator_tag{}));
  ASSERT_TRUE(is_invocable_with<int>(std::forward_iterator_tag{}));
  ASSERT_FALSE(is_invocable_with<int>(std::input_iterator_tag{}));
  ASSERT_FALSE(is_invocable_with<int>(std::output_iterator_tag{}));
}

/// @test @c rotate is not invocable with iterator value types is not swappable or movable
///
TEST(Rotate, RequiresSwappableAndMovable) {
  ASSERT_TRUE(is_invocable_with<int>(std::forward_iterator_tag{}));

  auto const is_invocable_with_spec = [](auto spec) {
    using value_type = testing::configurable_value<int, testing::throws_on::nothing, spec>;
    return is_invocable_with<value_type>(std::forward_iterator_tag{});
  };

  using testing::disable;
  ASSERT_TRUE(is_invocable_with_spec(std::integral_constant<disable, disable::nothing>{}));

  ASSERT_FALSE(is_invocable_with_spec(std::integral_constant<disable, disable::move_assign>{}));
  ASSERT_FALSE(is_invocable_with_spec(std::integral_constant<disable, disable::move_construct>{}));
  ASSERT_FALSE(is_invocable_with_spec(std::integral_constant<disable, disable::swap | disable::move_assign>{}));

  // This type has the custom swap disabled (not deleted!), but is still swappable with 'std::swap'
  ASSERT_TRUE(is_invocable_with_spec(std::integral_constant<disable, disable::swap>{}));
}

/// @test @c rotate is @c noexcept(false) if value type operations can throw
///
TEST(Rotate, NoexceptFalseIfValueTypeCanThrow) {
  auto const is_noexcept_with = [](auto category, auto specifier_constant) {
    static_cast<void>(category);
    using value_type = testing::configurable_value<int, specifier_constant>;
    using iterator_type = testing::demoted_iterator<value_type*, decltype(category)>;
    return noexcept(arene::base::rotate(  //
        std::declval<iterator_type>(),
        std::declval<iterator_type>(),
        std::declval<iterator_type>()
    ));
  };

  using throws_on = testing::throws_on_specifiers;

  ASSERT_TRUE(is_noexcept_with(std::forward_iterator_tag{}, throws_on::nothing));
  ASSERT_TRUE(is_noexcept_with(std::bidirectional_iterator_tag{}, throws_on::nothing));
  ASSERT_TRUE(is_noexcept_with(std::random_access_iterator_tag{}, throws_on::nothing));

  ASSERT_FALSE(is_noexcept_with(std::forward_iterator_tag{}, throws_on::move_construct));
  ASSERT_FALSE(is_noexcept_with(std::bidirectional_iterator_tag{}, throws_on::move_construct));
  ASSERT_FALSE(is_noexcept_with(std::random_access_iterator_tag{}, throws_on::move_construct));

  ASSERT_FALSE(is_noexcept_with(std::forward_iterator_tag{}, throws_on::move_assign));
  ASSERT_FALSE(is_noexcept_with(std::bidirectional_iterator_tag{}, throws_on::move_assign));
  ASSERT_FALSE(is_noexcept_with(std::random_access_iterator_tag{}, throws_on::move_assign));

  ASSERT_FALSE(is_noexcept_with(std::forward_iterator_tag{}, throws_on::swap));
  ASSERT_FALSE(is_noexcept_with(std::bidirectional_iterator_tag{}, throws_on::swap));
  ASSERT_TRUE(is_noexcept_with(std::random_access_iterator_tag{}, throws_on::swap));
}

/// @test @c rotate is @c noexcept(false) if iterator operations can throw
///
TEST(Rotate, NoexceptFalseIfIteratorTypeCanThrow) {
  auto const is_noexcept_with = [](auto category, auto value, auto specifier_constant) {
    using iterator = testing::noexcept_configurable_iterator<specifier_constant, decltype(category), decltype(value)>;
    return noexcept(arene::base::rotate(  //
        std::declval<iterator>(),
        std::declval<iterator>(),
        std::declval<iterator>()
    ));
  };

  using throws_on = testing::throws_on_specifiers;

  ASSERT_TRUE(is_noexcept_with(std::forward_iterator_tag{}, int{}, throws_on::nothing));
  ASSERT_TRUE(is_noexcept_with(std::bidirectional_iterator_tag{}, int{}, throws_on::nothing));
  ASSERT_TRUE(is_noexcept_with(std::random_access_iterator_tag{}, int{}, throws_on::nothing));

  ASSERT_FALSE(is_noexcept_with(std::forward_iterator_tag{}, int{}, throws_on::dereference));
  ASSERT_FALSE(is_noexcept_with(std::forward_iterator_tag{}, int{}, throws_on::pre_increment));
  ASSERT_FALSE(is_noexcept_with(std::forward_iterator_tag{}, int{}, throws_on::post_increment));
  ASSERT_FALSE(is_noexcept_with(std::forward_iterator_tag{}, int{}, throws_on::equal));
  ASSERT_FALSE(is_noexcept_with(std::forward_iterator_tag{}, int{}, throws_on::not_equal));
  ASSERT_FALSE(is_noexcept_with(std::forward_iterator_tag{}, int{}, throws_on::copy_construct));
  ASSERT_FALSE(is_noexcept_with(std::forward_iterator_tag{}, int{}, throws_on::copy_assign));

  ASSERT_FALSE(is_noexcept_with(std::bidirectional_iterator_tag{}, int{}, throws_on::dereference));
  ASSERT_FALSE(is_noexcept_with(std::bidirectional_iterator_tag{}, int{}, throws_on::pre_increment));
  ASSERT_FALSE(is_noexcept_with(std::bidirectional_iterator_tag{}, int{}, throws_on::post_increment));
  ASSERT_FALSE(is_noexcept_with(std::bidirectional_iterator_tag{}, int{}, throws_on::equal));
  ASSERT_FALSE(is_noexcept_with(std::bidirectional_iterator_tag{}, int{}, throws_on::not_equal));
  ASSERT_FALSE(is_noexcept_with(std::bidirectional_iterator_tag{}, int{}, throws_on::copy_construct));
  ASSERT_FALSE(is_noexcept_with(std::bidirectional_iterator_tag{}, int{}, throws_on::copy_assign));

  ASSERT_FALSE(is_noexcept_with(std::random_access_iterator_tag{}, int{}, throws_on::dereference));
  ASSERT_FALSE(is_noexcept_with(std::random_access_iterator_tag{}, int{}, throws_on::pre_increment));
  ASSERT_FALSE(is_noexcept_with(std::random_access_iterator_tag{}, int{}, throws_on::post_increment));
  ASSERT_FALSE(is_noexcept_with(std::random_access_iterator_tag{}, int{}, throws_on::equal));
  ASSERT_FALSE(is_noexcept_with(std::random_access_iterator_tag{}, int{}, throws_on::not_equal));
  ASSERT_FALSE(is_noexcept_with(std::random_access_iterator_tag{}, int{}, throws_on::copy_construct));
  ASSERT_FALSE(is_noexcept_with(std::random_access_iterator_tag{}, int{}, throws_on::copy_assign));
}

}  // namespace
