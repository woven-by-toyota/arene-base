// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/tuple/detail/get.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

// intentional use of 'std::move' to test specific overloads
// NOLINTBEGIN(hicpp-move-const-arg)

namespace {

// non-empty tuple likes
using TupleLikeTypes = ::testing::Types<  //
    std::tuple<int>,                      //
    arene::base::array<int, 1>,           //
    std::pair<int, int>,                  //
    arene::base::array<int, 2>,           //
    std::tuple<int, int, int>             //
    >;

enum struct overload {
  member_lvalue_ref,
  member_const_lvalue_ref,
  member_rvalue_ref,
  member_const_rvalue_ref,
  adl_lvalue_ref,
  adl_const_lvalue_ref,
  adl_rvalue_ref,
  adl_const_rvalue_ref
};

template <bool HasMemberGet, bool HasAdlGet, bool Throws>
struct maybe_tuple_like {
  template <std::size_t I, arene::base::constraints<std::enable_if_t<(I == 0) && HasMemberGet>> = nullptr>
  constexpr auto get() & noexcept(!Throws) -> std::integral_constant<overload, overload::member_lvalue_ref> {
    return {};
  }
  template <std::size_t I, arene::base::constraints<std::enable_if_t<(I == 0) && HasMemberGet>> = nullptr>
  constexpr auto get() const& noexcept(!Throws) -> std::integral_constant<overload, overload::member_const_lvalue_ref> {
    return {};
  }
  template <std::size_t I, arene::base::constraints<std::enable_if_t<(I == 0) && HasMemberGet>> = nullptr>
  constexpr auto get() && noexcept(!Throws) -> std::integral_constant<overload, overload::member_rvalue_ref> {
    return {};
  }
  template <std::size_t I, arene::base::constraints<std::enable_if_t<(I == 0) && HasMemberGet>> = nullptr>
  constexpr auto get() const&& noexcept(!Throws)
      -> std::integral_constant<overload, overload::member_const_rvalue_ref> {
    return {};
  }

  template <std::size_t I, arene::base::constraints<std::enable_if_t<(I == 0) && HasAdlGet>> = nullptr>
  friend constexpr auto get(maybe_tuple_like&) noexcept(!Throws)
      -> std::integral_constant<overload, overload::adl_lvalue_ref> {
    return {};
  }
  template <std::size_t I, arene::base::constraints<std::enable_if_t<(I == 0) && HasAdlGet>> = nullptr>
  friend constexpr auto get(maybe_tuple_like const&) noexcept(!Throws)
      -> std::integral_constant<overload, overload::adl_const_lvalue_ref> {
    return {};
  }
  template <std::size_t I, arene::base::constraints<std::enable_if_t<(I == 0) && HasAdlGet>> = nullptr>
  friend constexpr auto get(maybe_tuple_like&&) noexcept(!Throws)
      -> std::integral_constant<overload, overload::adl_rvalue_ref> {
    return {};
  }
  template <std::size_t I, arene::base::constraints<std::enable_if_t<(I == 0) && HasAdlGet>> = nullptr>
  friend constexpr auto get(maybe_tuple_like const&&) noexcept(!Throws)
      -> std::integral_constant<overload, overload::adl_const_rvalue_ref> {
    return {};
  }
};

using type_without_get = maybe_tuple_like<false, false, false>;
using type_with_member_get = maybe_tuple_like<true, false, false>;
using type_with_adl_get = maybe_tuple_like<false, true, false>;
using type_with_member_and_adl_get = maybe_tuple_like<true, true, false>;
using type_with_member_get_and_throws = maybe_tuple_like<true, false, true>;
using type_with_adl_get_and_throws = maybe_tuple_like<false, true, true>;

}  // namespace

template <bool B1, bool B2, bool B3>
class std::tuple_size<maybe_tuple_like<B1, B2, B3>> : public std::integral_constant<std::size_t, 1> {};

namespace {

template <class T>
struct TupleDetailGet : testing::Test {};

TYPED_TEST_SUITE(TupleDetailGet, TupleLikeTypes, );

namespace tuple_detail = ::arene::base::tuple_detail;

/// @test 'tuple_detail::get' is SFINAE friendly if 'std::tuple_size' is zero
CONSTEXPR_TYPED_TEST(TupleDetailGet, IsSfinaeFriendlyIfTupleSizeIsZero) {
  CONSTEXPR_ASSERT(!arene::base::is_invocable_v<decltype(tuple_detail::get<0>), std::tuple<>>);
  CONSTEXPR_ASSERT(!arene::base::is_invocable_v<decltype(tuple_detail::get<0>), arene::base::array<int, 0>>);
  CONSTEXPR_ASSERT(!arene::base::is_invocable_v<decltype(tuple_detail::get<1>), std::tuple<>>);
  CONSTEXPR_ASSERT(!arene::base::is_invocable_v<decltype(tuple_detail::get<1>), arene::base::array<int, 0>>);
}

/// @test 'tuple_detail::get' is SFINAE friendly if Index exceeds 'std::tuple_size'
CONSTEXPR_TYPED_TEST(TupleDetailGet, IsSfinaeFriendlyIfIndexExceedsTupleSize) {
  CONSTEXPR_ASSERT(arene::base::is_invocable_v<decltype(tuple_detail::get<0>), TypeParam&>);
  CONSTEXPR_ASSERT(arene::base::is_invocable_v<decltype(tuple_detail::get<0>), TypeParam const&>);
  CONSTEXPR_ASSERT(arene::base::is_invocable_v<decltype(tuple_detail::get<0>), TypeParam&&>);
  CONSTEXPR_ASSERT(arene::base::is_invocable_v<decltype(tuple_detail::get<0>), TypeParam const&&>);

  CONSTEXPR_ASSERT(!arene::base::is_invocable_v<decltype(tuple_detail::get<4>), TypeParam&>);
  CONSTEXPR_ASSERT(!arene::base::is_invocable_v<decltype(tuple_detail::get<4>), TypeParam const&>);
  CONSTEXPR_ASSERT(!arene::base::is_invocable_v<decltype(tuple_detail::get<4>), TypeParam&&>);
  CONSTEXPR_ASSERT(!arene::base::is_invocable_v<decltype(tuple_detail::get<4>), TypeParam const&&>);
}

/// @test 'tuple_detail::get' is SFINAE friendly if Index does not exceed 'std::tuple_size' but type doesn't implement
/// 'get'
CONSTEXPR_TEST(TupleDetailGet, TypeWithTupleSizeButNoGet) {
  CONSTEXPR_ASSERT(!arene::base::is_invocable_v<decltype(tuple_detail::get<0>), type_without_get>);
}

/// @test 'tuple_detail::get' works with member get
CONSTEXPR_TEST(TupleDetailGet, TypeWithMemberGet) {
  {
    auto tup = type_with_member_get{};
    CONSTEXPR_ASSERT(overload::member_lvalue_ref == tuple_detail::get<0>(tup));
  }
  {
    auto const tup = type_with_member_get{};
    CONSTEXPR_ASSERT(overload::member_const_lvalue_ref == tuple_detail::get<0>(tup));
  }
  {
    auto tup = type_with_member_get{};
    CONSTEXPR_ASSERT(overload::member_rvalue_ref == tuple_detail::get<0>(std::move(tup)));
  }
  {
    auto const tup = type_with_member_get{};
    CONSTEXPR_ASSERT(overload::member_const_rvalue_ref == tuple_detail::get<0>(std::move(tup)));
  }
}

/// @test 'tuple_detail::get' works with adl get
CONSTEXPR_TEST(TupleDetailGet, TypeWithAdlGet) {
  {
    auto tup = type_with_adl_get{};
    CONSTEXPR_ASSERT(overload::adl_lvalue_ref == tuple_detail::get<0>(tup));
  }
  {
    auto const tup = type_with_adl_get{};
    CONSTEXPR_ASSERT(overload::adl_const_lvalue_ref == tuple_detail::get<0>(tup));
  }
  {
    auto tup = type_with_adl_get{};
    CONSTEXPR_ASSERT(overload::adl_rvalue_ref == tuple_detail::get<0>(std::move(tup)));
  }
  {
    auto const tup = type_with_adl_get{};
    CONSTEXPR_ASSERT(overload::adl_const_rvalue_ref == tuple_detail::get<0>(std::move(tup)));
  }
}

/// @test 'tuple_detail::get' prioritizes member get
CONSTEXPR_TEST(TupleDetailGet, TypeWithMemberAndAdlGet) {
  {
    auto tup = type_with_member_and_adl_get{};
    CONSTEXPR_ASSERT(overload::member_lvalue_ref == tuple_detail::get<0>(tup));
  }
  {
    auto const tup = type_with_member_and_adl_get{};
    CONSTEXPR_ASSERT(overload::member_const_lvalue_ref == tuple_detail::get<0>(tup));
  }
  {
    auto tup = type_with_member_and_adl_get{};
    CONSTEXPR_ASSERT(overload::member_rvalue_ref == tuple_detail::get<0>(std::move(tup)));
  }
  {
    auto const tup = type_with_member_and_adl_get{};
    CONSTEXPR_ASSERT(overload::member_const_rvalue_ref == tuple_detail::get<0>(std::move(tup)));
  }
}

/// @test 'tuple_detail::get' returns the same as 'std::get'
CONSTEXPR_TEST(TupleDetailGet, ReturnsTheSameAsStdGet) {
  struct custom0 {};
  struct custom1 {};
  using tuple = std::tuple<custom0, custom1>;

  CONSTEXPR_ASSERT(std::is_same<
                   decltype(tuple_detail::get<0>(std::declval<tuple&>())),
                   decltype(std::get<0>(std::declval<tuple&>()))>::value);
  CONSTEXPR_ASSERT(std::is_same<
                   decltype(tuple_detail::get<0>(std::declval<tuple const&>())),
                   decltype(std::get<0>(std::declval<tuple const&>()))>::value);
  CONSTEXPR_ASSERT(std::is_same<
                   decltype(tuple_detail::get<0>(std::declval<tuple&&>())),
                   decltype(std::get<0>(std::declval<tuple&&>()))>::value);
  CONSTEXPR_ASSERT(std::is_same<
                   decltype(tuple_detail::get<0>(std::declval<tuple const&&>())),
                   decltype(std::get<0>(std::declval<tuple const&&>()))>::value);

  CONSTEXPR_ASSERT(std::is_same<
                   decltype(tuple_detail::get<1>(std::declval<tuple&>())),
                   decltype(std::get<1>(std::declval<tuple&>()))>::value);
  CONSTEXPR_ASSERT(std::is_same<
                   decltype(tuple_detail::get<1>(std::declval<tuple const&>())),
                   decltype(std::get<1>(std::declval<tuple const&>()))>::value);
  CONSTEXPR_ASSERT(std::is_same<
                   decltype(tuple_detail::get<1>(std::declval<tuple&&>())),
                   decltype(std::get<1>(std::declval<tuple&&>()))>::value);
  CONSTEXPR_ASSERT(std::is_same<
                   decltype(tuple_detail::get<1>(std::declval<tuple const&&>())),
                   decltype(std::get<1>(std::declval<tuple const&&>()))>::value);
}

/// @test 'tuple_detail::get' returns the same as 'arene::base::get'
CONSTEXPR_TEST(TupleDetailGet, ReturnsTheSameAsAreneBaseGet) {
  struct custom0 {};
  using tuple = arene::base::array<custom0, 2>;

  CONSTEXPR_ASSERT(std::is_same<
                   decltype(tuple_detail::get<0>(std::declval<tuple&>())),
                   decltype(arene::base::get<0>(std::declval<tuple&>()))>::value);
  CONSTEXPR_ASSERT(std::is_same<
                   decltype(tuple_detail::get<0>(std::declval<tuple const&>())),
                   decltype(arene::base::get<0>(std::declval<tuple const&>()))>::value);
  CONSTEXPR_ASSERT(std::is_same<
                   decltype(tuple_detail::get<0>(std::declval<tuple&&>())),
                   decltype(arene::base::get<0>(std::declval<tuple&&>()))>::value);
  CONSTEXPR_ASSERT(std::is_same<
                   decltype(tuple_detail::get<0>(std::declval<tuple const&&>())),
                   decltype(arene::base::get<0>(std::declval<tuple const&&>()))>::value);

  CONSTEXPR_ASSERT(std::is_same<
                   decltype(tuple_detail::get<1>(std::declval<tuple&>())),
                   decltype(arene::base::get<1>(std::declval<tuple&>()))>::value);
  CONSTEXPR_ASSERT(std::is_same<
                   decltype(tuple_detail::get<1>(std::declval<tuple const&>())),
                   decltype(arene::base::get<1>(std::declval<tuple const&>()))>::value);
  CONSTEXPR_ASSERT(std::is_same<
                   decltype(tuple_detail::get<1>(std::declval<tuple&&>())),
                   decltype(arene::base::get<1>(std::declval<tuple&&>()))>::value);
  CONSTEXPR_ASSERT(std::is_same<
                   decltype(tuple_detail::get<1>(std::declval<tuple const&&>())),
                   decltype(arene::base::get<1>(std::declval<tuple const&&>()))>::value);
}

/// @test 'tuple_detail::get' propagates noexcept
CONSTEXPR_TEST(TupleDetailGet, PropagatesNoexcept) {
  CONSTEXPR_ASSERT(noexcept(tuple_detail::get<0>(std::declval<type_with_member_get&>())));
  CONSTEXPR_ASSERT(noexcept(tuple_detail::get<0>(std::declval<type_with_member_get const&>())));
  CONSTEXPR_ASSERT(noexcept(tuple_detail::get<0>(std::declval<type_with_member_get&&>())));
  CONSTEXPR_ASSERT(noexcept(tuple_detail::get<0>(std::declval<type_with_member_get const&&>())));

  CONSTEXPR_ASSERT(noexcept(tuple_detail::get<0>(std::declval<type_with_adl_get&>())));
  CONSTEXPR_ASSERT(noexcept(tuple_detail::get<0>(std::declval<type_with_adl_get const&>())));
  CONSTEXPR_ASSERT(noexcept(tuple_detail::get<0>(std::declval<type_with_adl_get&&>())));
  CONSTEXPR_ASSERT(noexcept(tuple_detail::get<0>(std::declval<type_with_adl_get const&&>())));

  CONSTEXPR_ASSERT(!noexcept(tuple_detail::get<0>(std::declval<type_with_member_get_and_throws&>())));
  CONSTEXPR_ASSERT(!noexcept(tuple_detail::get<0>(std::declval<type_with_member_get_and_throws const&>())));
  CONSTEXPR_ASSERT(!noexcept(tuple_detail::get<0>(std::declval<type_with_member_get_and_throws&&>())));
  CONSTEXPR_ASSERT(!noexcept(tuple_detail::get<0>(std::declval<type_with_member_get_and_throws const&&>())));

  CONSTEXPR_ASSERT(!noexcept(tuple_detail::get<0>(std::declval<type_with_adl_get_and_throws&>())));
  CONSTEXPR_ASSERT(!noexcept(tuple_detail::get<0>(std::declval<type_with_adl_get_and_throws const&>())));
  CONSTEXPR_ASSERT(!noexcept(tuple_detail::get<0>(std::declval<type_with_adl_get_and_throws&&>())));
  CONSTEXPR_ASSERT(!noexcept(tuple_detail::get<0>(std::declval<type_with_adl_get_and_throws const&&>())));
}
}  // namespace

// NOLINTEND(hicpp-move-const-arg)
