// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/tests/max_element.hpp"  // IWYU pragma: keep

#include <gtest/gtest.h>

// IWYU pragma: no_include "arene/base/algorithm/max_element.hpp"
// IWYU pragma: no_include "arene/base/array/array.hpp"
#include "arene/base/array/to_array.hpp"
#include "arene/base/stdlib_choice/add_pointer.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/less.hpp"
#include "arene/base/stdlib_choice/negate.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "testlibs/utilities/iterator_types.hpp"

namespace {

/// @test @c arene::base::max_element can be used with a projection
CONSTEXPR_TYPED_TEST(MaxElement, HasProjection) {
  using iterator_tag = typename TypeParam::iterator_tag;

  auto const values = to_array({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
  auto const range = testing::demoted_view(iterator_tag{}, values);

  auto const iter = max_element_test::max_element_fn()(range.begin(), range.end(), {}, std::negate<>{});

  CONSTEXPR_ASSERT_EQ(iter, range.begin());
}

/// @test @c arene::base::max_element is not @c noexcept if the comparison can throw.
TEST(MaxElement, NotNoexceptIfProjectionCanThrow) {
  using iterator_type = int const*;

  struct proj {
    auto operator()(int) const noexcept(false) { return int{}; }
  };

  // one of the arguments must be 'declval', otherwise using GCC8 and C++17 will
  // *always* deduce 'noexcept' as 'true'
  ASSERT_FALSE(noexcept(max_element_test::max_element_fn()(std::declval<iterator_type>(), iterator_type{}, {}, proj{}))
  );
}

/// @test @c max_element requires the projection is invocable with the iterator
/// reference type
TEST(MaxElement, RequiresInvocableProjection) {
  auto const is_invocable_with = [](auto iter, auto cmp, auto... proj) {
    return arene::base::is_invocable_v<
        decltype(max_element_test::max_element_fn()),
        decltype(iter),
        decltype(iter),
        decltype(cmp),
        decltype(proj)...>;
  };

  ASSERT_TRUE(is_invocable_with(std::add_pointer_t<int>{}, std::less<>{}));

  auto const simple_proj = [](int value) -> int { return value; };
  ASSERT_TRUE(is_invocable_with(std::add_pointer_t<int>{}, std::less<>{}, simple_proj));

  auto const bad_proj = [](auto) -> void {};
  ASSERT_FALSE(is_invocable_with(std::add_pointer_t<int>{}, std::less<>{}, bad_proj));

  struct empty {};
  auto const non_comparable_proj_result = [](auto) -> empty { return {}; };
  ASSERT_FALSE(is_invocable_with(std::add_pointer_t<int>{}, std::less<>{}, non_comparable_proj_result));
}

}  // namespace
