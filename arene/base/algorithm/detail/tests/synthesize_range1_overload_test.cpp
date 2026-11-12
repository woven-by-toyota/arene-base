// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/algorithm/accumulate.hpp"
#include "arene/base/algorithm/all_of.hpp"
#include "arene/base/algorithm/any_of.hpp"
#include "arene/base/algorithm/inner_product.hpp"
#include "arene/base/algorithm/reduce.hpp"
#include "arene/base/algorithm/sort.hpp"
#include "arene/base/algorithm/stable_sort.hpp"
#include "arene/base/algorithm/transform_reduce.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/utility/make_subrange.hpp"
#include "testlibs/utilities/iterator_types.hpp"

namespace {

using throws_on = testing::throws_on_specifiers;

/// @test the synthesized range1 overload is noexcept(false) if the range
/// begin() or range end() member functions can throw
TEST(AlgorithmDetailSynthesizeRange1Overload, RangeOverloadIsNoexcepFalseIfBeginOrEndCanThrow) {
  auto is_noexcept_with_make_subrange = [](auto algorithm, auto... args) {
    using iterator = int*;
    return noexcept(algorithm(arene::base::make_subrange(iterator{}, iterator{}), args...));
  };

  EXPECT_TRUE(is_noexcept_with_make_subrange(arene::base::accumulate, 0));
  EXPECT_TRUE(is_noexcept_with_make_subrange(arene::base::all_of, [](auto) noexcept { return true; }));
  EXPECT_TRUE(is_noexcept_with_make_subrange(arene::base::any_of, [](auto) noexcept { return true; }));
  EXPECT_TRUE(is_noexcept_with_make_subrange(arene::base::inner_product, static_cast<int*>(nullptr), 0));
  EXPECT_TRUE(is_noexcept_with_make_subrange(arene::base::reduce, 0));
  EXPECT_TRUE(is_noexcept_with_make_subrange(arene::base::sort));
  EXPECT_TRUE(is_noexcept_with_make_subrange(arene::base::stable_sort));
  EXPECT_TRUE(is_noexcept_with_make_subrange(arene::base::transform_reduce, static_cast<int*>(nullptr), 0));

  auto is_noexcept_with_throwing_begin = [](auto algorithm, auto... args) {
    using iterator = int*;

    struct subrange {
      auto begin() const -> iterator { return {}; }
      auto end() const noexcept -> iterator { return {}; }
    };

    return noexcept(algorithm(subrange{}, args...));
  };

  ASSERT_FALSE(is_noexcept_with_throwing_begin(arene::base::accumulate, 0));
  ASSERT_FALSE(is_noexcept_with_throwing_begin(arene::base::all_of, [](auto) noexcept { return true; }));
  ASSERT_FALSE(is_noexcept_with_throwing_begin(arene::base::any_of, [](auto) noexcept { return true; }));
  ASSERT_FALSE(is_noexcept_with_throwing_begin(arene::base::inner_product, static_cast<int*>(nullptr), 0));
  ASSERT_FALSE(is_noexcept_with_throwing_begin(arene::base::reduce, 0));
  ASSERT_FALSE(is_noexcept_with_throwing_begin(arene::base::sort));
  ASSERT_FALSE(is_noexcept_with_throwing_begin(arene::base::stable_sort));
  ASSERT_FALSE(is_noexcept_with_throwing_begin(arene::base::transform_reduce, static_cast<int*>(nullptr), 0));

  auto is_noexcept_with_throwing_end = [](auto algorithm, auto... args) {
    using iterator = int*;

    struct subrange {
      auto begin() const noexcept -> iterator { return {}; }
      auto end() const -> iterator { return {}; }
    };

    return noexcept(algorithm(subrange{}, args...));
  };

  ASSERT_FALSE(is_noexcept_with_throwing_end(arene::base::accumulate, 0));
  ASSERT_FALSE(is_noexcept_with_throwing_end(arene::base::all_of, [](auto) noexcept { return true; }));
  ASSERT_FALSE(is_noexcept_with_throwing_end(arene::base::any_of, [](auto) noexcept { return true; }));
  ASSERT_FALSE(is_noexcept_with_throwing_end(arene::base::inner_product, static_cast<int*>(nullptr), 0));
  ASSERT_FALSE(is_noexcept_with_throwing_end(arene::base::reduce, 0));
  ASSERT_FALSE(is_noexcept_with_throwing_end(arene::base::sort));
  ASSERT_FALSE(is_noexcept_with_throwing_end(arene::base::stable_sort));
  ASSERT_FALSE(is_noexcept_with_throwing_end(arene::base::transform_reduce, static_cast<int*>(nullptr), 0));
}

template <class T>
struct SynthesizeRange1OverloadNoexceptIteratorTest : ::testing::Test {};

using IteratorNoexceptSpecifierTypes = ::testing::Types<  //
    decltype(throws_on::nothing),                         //
    decltype(throws_on::dereference),                     //
    decltype(throws_on::equal),                           //
    decltype(throws_on::not_equal),                       //
    decltype(throws_on::less),                            //
    decltype(throws_on::less_equal),                      //
    decltype(throws_on::greater),                         //
    decltype(throws_on::greater_equal),                   //
    decltype(throws_on::copy_construct),                  //
    decltype(throws_on::copy_assign)                      //
    >;

TYPED_TEST_SUITE(SynthesizeRange1OverloadNoexceptIteratorTest, IteratorNoexceptSpecifierTypes, );

/// @test when using the algorithm with an iterator type that has operations
/// that may throw, the synthesized range1 overload has the same noexcept
/// specification as the underlying iterator overload
TYPED_TEST(SynthesizeRange1OverloadNoexceptIteratorTest, RangeOverloadNoexceptMatchesIteratorOverload) {
  auto const matching_noexcept_with_args = [](auto algorithm, auto... args) {
    using iterator = testing::noexcept_configurable_random_access_iterator<TypeParam{}>;
    return noexcept(algorithm(iterator{}, iterator{}, args...)) ==
           noexcept(algorithm(arene::base::make_subrange(iterator{}, iterator{}), args...));
  };

  EXPECT_TRUE(matching_noexcept_with_args(arene::base::accumulate, 0));
  EXPECT_TRUE(matching_noexcept_with_args(arene::base::all_of, [](auto) noexcept { return true; }));
  EXPECT_TRUE(matching_noexcept_with_args(arene::base::any_of, [](auto) noexcept { return true; }));
  EXPECT_TRUE(matching_noexcept_with_args(arene::base::inner_product, static_cast<int*>(nullptr), 0));
  EXPECT_TRUE(matching_noexcept_with_args(arene::base::reduce, 0));
  EXPECT_TRUE(matching_noexcept_with_args(arene::base::sort));
  EXPECT_TRUE(matching_noexcept_with_args(arene::base::stable_sort));
  EXPECT_TRUE(matching_noexcept_with_args(arene::base::transform_reduce, static_cast<int*>(nullptr), 0));
}

/// @test the invocability of the synthesized range1 overload matches with the
/// invocability of the underlying iterator overload
TEST(AlgorithmDetailSynthesizeRange1Overload, RangeOverloadInvocabilityMatchesIteratorOverload) {
  auto const matching_invocability_with_args = [](auto algorithm, auto... args) {
    using iterator = int*;
    using range = decltype(arene::base::make_subrange(iterator{}, iterator{}));

    return arene::base::is_invocable_v<decltype(algorithm), iterator, iterator, decltype(args)...> ==
           arene::base::is_invocable_v<decltype(algorithm), range, decltype(args)...>;
  };

  EXPECT_TRUE(matching_invocability_with_args(arene::base::accumulate, 0, [](auto, auto) -> int { return {}; }));
  EXPECT_TRUE(matching_invocability_with_args(arene::base::accumulate, nullptr));
  EXPECT_TRUE(matching_invocability_with_args(arene::base::accumulate, 0, 0));

  EXPECT_TRUE(matching_invocability_with_args(arene::base::all_of, [](auto) -> bool { return {}; }));
  EXPECT_TRUE(matching_invocability_with_args(arene::base::all_of, [](int*) { return true; }));
  EXPECT_TRUE(matching_invocability_with_args(arene::base::all_of, [](auto, auto) { return true; }));

  EXPECT_TRUE(matching_invocability_with_args(arene::base::any_of, [](auto) -> bool { return {}; }));
  EXPECT_TRUE(matching_invocability_with_args(arene::base::any_of, [](int*) { return true; }));
  EXPECT_TRUE(matching_invocability_with_args(arene::base::any_of, [](auto, auto) { return true; }));

  EXPECT_TRUE(matching_invocability_with_args(
      arene::base::inner_product,
      static_cast<int*>(nullptr),
      0,
      [](auto, auto) -> int { return {}; },
      [](auto, auto) -> int { return {}; }
  ));
  EXPECT_TRUE(matching_invocability_with_args(
      arene::base::inner_product,
      0,
      0,
      [](auto, auto) -> int { return {}; },
      [](auto, auto) -> int { return {}; }
  ));
  EXPECT_TRUE(matching_invocability_with_args(
      arene::base::inner_product,
      static_cast<int*>(nullptr),
      0,
      [](auto) -> int { return {}; },
      [](auto, auto) -> int { return {}; }
  ));

  EXPECT_TRUE(matching_invocability_with_args(arene::base::reduce, 0, [](auto, auto) -> int { return {}; }));
  EXPECT_TRUE(matching_invocability_with_args(arene::base::reduce, nullptr));
  EXPECT_TRUE(matching_invocability_with_args(arene::base::reduce, 0, 0));

  EXPECT_TRUE(matching_invocability_with_args(arene::base::sort, [](auto, auto) -> bool { return {}; }));
  EXPECT_TRUE(matching_invocability_with_args(arene::base::sort, [](auto, auto) -> void {}));

  EXPECT_TRUE(matching_invocability_with_args(arene::base::stable_sort, [](auto, auto) -> bool { return {}; }));
  EXPECT_TRUE(matching_invocability_with_args(arene::base::stable_sort, [](auto, auto) -> void {}));

  EXPECT_TRUE(matching_invocability_with_args(
      arene::base::transform_reduce,
      static_cast<int*>(nullptr),
      0,
      [](auto, auto) -> int { return {}; },
      [](auto, auto) -> int { return {}; }
  ));
  EXPECT_TRUE(matching_invocability_with_args(
      arene::base::transform_reduce,
      0,
      0,
      [](auto, auto) -> int { return {}; },
      [](auto, auto) -> int { return {}; }
  ));
  EXPECT_TRUE(matching_invocability_with_args(
      arene::base::transform_reduce,
      static_cast<int*>(nullptr),
      0,
      [](auto) -> int { return {}; },
      [](auto, auto) -> int { return {}; }
  ));
}

/// @test the synthesized range1 overload requires the first argument to be an
/// actual range: a type that merely has begin()/end() members whose results do
/// not form an iterator/sentinel pair does not select the range overload
TEST(AlgorithmDetailSynthesizeRange1Overload, RangeOverloadRejectsNonRangeWithBeginEnd) {
  // begin()/end() return 'int', which is not an iterator, so this is not a range
  struct not_a_range {
    static auto begin() noexcept -> int { return 0; }
    static auto end() noexcept -> int { return 0; }
  };

  struct always_true_predicate {
    auto operator()(int) const noexcept -> bool { return true; }
  };

  EXPECT_FALSE((arene::base::is_invocable_v<decltype(arene::base::accumulate), not_a_range, int>));
  EXPECT_FALSE((arene::base::is_invocable_v<decltype(arene::base::reduce), not_a_range, int>));
  EXPECT_FALSE((arene::base::is_invocable_v<decltype(arene::base::all_of), not_a_range, always_true_predicate>));
}

}  // namespace
