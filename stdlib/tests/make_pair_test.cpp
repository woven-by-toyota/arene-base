// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/type_list/concat_unique.hpp"
#include "arene/base/type_list/remove_duplicates.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/is_implicitly_constructible.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/utility/as_const.hpp"
#include "stdlib/include/cstdint"
#include "stdlib/include/functional"
#include "stdlib/include/tuple"
#include "stdlib/include/type_traits"
#include "stdlib/include/utility"
#include "stdlib/tests/pair_utilities.hpp"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"
#include "testlibs/utilities/mutate.hpp"

namespace {

using test::pair_utilities::constexpr_check;
using test::pair_utilities::construction_tracker;
using test::pair_utilities::throwable_copy_construction;

using arene::base::type_lists::concat_unique_t;
using arene::base::type_lists::remove_duplicates_t;

using make_pair_types = remove_duplicates_t<
    ::testing::extend_with_all_t<concat_unique_t<::testing::object_types, ::testing::compound_types>>>;

template <class T1, class T2>
using make_pair_return_type_t = decltype(std::make_pair(std::declval<T1>(), std::declval<T2>()));

template <typename TypeParam>
class StdMakePairTests : public testing::Test {};

TYPED_TEST_SUITE(StdMakePairTests, make_pair_types, );

/// @test The @c make_pair function decays both of the input arguments
TYPED_TEST(StdMakePairTests, MakePairDecaysInputValueTypes) {
  using pair_type = make_pair_return_type_t<TypeParam, TypeParam>;

  testing::StaticAssertTypeEq<typename pair_type::first_type, std::decay_t<TypeParam>>();
  testing::StaticAssertTypeEq<typename pair_type::second_type, std::decay_t<TypeParam>>();
}

/// @test The @c make_pair function extracts a reference type from a @c std::reference_wrapper
CONDITIONAL_TYPED_TEST(StdMakePairTests, MakePairExtractsFromReferenceWrapper, !std::is_reference_v<TypeParam>) {
  using reference_type = std::reference_wrapper<TypeParam>;
  using pair_type = make_pair_return_type_t<reference_type, reference_type>;

  testing::StaticAssertTypeEq<typename pair_type::first_type, typename reference_type::type&>();
  testing::StaticAssertTypeEq<typename pair_type::second_type, typename reference_type::type&>();
}

/// @test The @c make_pair function forwards the arguments it is given.
TEST(StdMakePairTests, MakePairForwardsTheArguments) {
  auto first_tracker = construction_tracker{};
  auto second_tracker = construction_tracker{};
  auto const copied = std::make_pair(first_tracker, second_tracker);

  ASSERT_EQ(copied.first.get_tracked_state(), construction_tracker::state::copy_constructed);
  ASSERT_EQ(copied.second.get_tracked_state(), construction_tracker::state::copy_constructed);

  auto const moved = std::make_pair(std::move(first_tracker), std::move(second_tracker));

  ASSERT_EQ(moved.first.get_tracked_state(), construction_tracker::state::move_constructed);
  ASSERT_EQ(moved.second.get_tracked_state(), construction_tracker::state::move_constructed);
}

/// @test The @c make_pair function is usable in a constant expression if the arguments are
CONDITIONAL_TYPED_TEST(
    StdMakePairTests,
    MakePairUsableInConstantExpressionIfArgsAre,
    (std::is_trivially_copy_constructible_v<std::decay_t<TypeParam>> &&
     std::is_trivially_default_constructible_v<std::decay_t<TypeParam>> &&
     std::is_default_constructible_v<std::decay_t<TypeParam>>)
) {
  static_assert(
      constexpr_check(std::make_pair(std::decay_t<TypeParam>{}, std::decay_t<TypeParam>{})),
      "Must be callable in a constant expression"
  );
}

template <class T1, class T2>
constexpr auto is_nothrow_invocable_with() -> bool {
  return noexcept(std::make_pair(std::declval<T1>(), std::declval<T2>()));
}

/// @test The @c std::make_pair is noexcept if and only if a pair can be nothrow constructed from the given arguments.
CONSTEXPR_TEST(StdMakePairTests, MakePairNoexceptIfConstructionIs) {
  ASSERT_TRUE(noexcept(std::make_pair(std::declval<std::int32_t>(), std::declval<std::int32_t>())));
  ASSERT_FALSE(noexcept(
      std::make_pair(std::declval<throwable_copy_construction&>(), std::declval<throwable_copy_construction&>())
  ));
}

}  // namespace
