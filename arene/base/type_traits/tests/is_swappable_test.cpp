// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/is_swappable.hpp"

#include <cstdint>

#include <gtest/gtest.h>

#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_list.hpp"

namespace {

using ::arene::base::is_adl_swappable_v;
using ::arene::base::is_adl_swappable_with_v;
using ::arene::base::is_default_swappable_v;
using ::arene::base::is_nothrow_adl_swappable_v;
using ::arene::base::is_nothrow_adl_swappable_with_v;
using ::arene::base::is_nothrow_default_swappable_v;
using ::arene::base::is_nothrow_swappable_v;
using ::arene::base::is_nothrow_swappable_with_v;
using ::arene::base::is_swappable_v;
using ::arene::base::is_swappable_with_v;
using ::arene::base::type_list;

namespace type_lists = ::arene::base::type_lists;

/// Helper for casting a type_list to testing::Types
template <typename Tl>
using as_testing_types = type_lists::apply_all_t<Tl, ::testing::Types>;

template <typename T>
class IsSwappableVTest : public ::testing::Test {};

//////////////////// Default Swappable Types ////////////////////

// NOLINTNEXTLINE(hicpp-special-member-functions) we explicitly want a type which is _missing_ a move constructor, rather than it being deleted.
struct copy_only {
  copy_only() = default;
  ~copy_only() = default;

  // NOLINTBEGIN(hicpp-use-equals-default)
  //  these empty bodies are needed because gcc ignores the noexcept declaration on defaulted ctors/assignment
  //  operators.
  copy_only(copy_only const&) noexcept {}
  auto operator=(copy_only const&) noexcept -> copy_only& { return *this; }
  // NOLINTEND(hicpp-use-equals-default)
};

// NOLINTNEXTLINE(hicpp-special-member-functions) we explicitly want a type which is _missing_ a move constructor, rather than it being deleted.
struct throwing_copy_only {
  throwing_copy_only() = default;
  ~throwing_copy_only() = default;

  // NOLINTBEGIN(hicpp-use-equals-default)
  // these empty bodies are needed because gcc ignores the noexcept declaration on defaulted ctors/assignment operators.
  throwing_copy_only(throwing_copy_only const&) noexcept(false) {}
  auto operator=(throwing_copy_only const&) noexcept(false) -> throwing_copy_only& { return *this; }
  // NOLINTEND(hicpp-use-equals-default)
};

struct move_only {
  move_only() = default;
  ~move_only() = default;

  move_only(move_only const&) = delete;
  auto operator=(move_only const&) -> move_only& = delete;

  move_only(move_only&&) noexcept = default;
  auto operator=(move_only&&) noexcept -> move_only& = default;
};

struct throwing_move_only {
  throwing_move_only() = default;
  ~throwing_move_only() = default;

  throwing_move_only(throwing_move_only const&) noexcept(false) = delete;
  auto operator=(throwing_move_only const&) noexcept(false) -> throwing_move_only& = delete;

  throwing_move_only(throwing_move_only&&) noexcept(false) {}
  auto operator=(throwing_move_only&&) noexcept(false) -> throwing_move_only& { return *this; }
};

struct not_move_constructable {
  not_move_constructable() = default;
  ~not_move_constructable() = default;

  not_move_constructable(not_move_constructable const&) = delete;
  auto operator=(not_move_constructable const&) -> not_move_constructable& = delete;

  not_move_constructable(not_move_constructable&&) = delete;
  auto operator=(not_move_constructable&&) -> not_move_constructable& = default;
};

struct not_move_assignable {
  not_move_assignable() = default;
  ~not_move_assignable() = default;

  not_move_assignable(not_move_assignable const&) = delete;
  auto operator=(not_move_assignable const&) -> not_move_assignable& = delete;

  not_move_assignable(not_move_assignable&&) = default;
  auto operator=(not_move_assignable&&) -> not_move_assignable& = delete;
};

using primitive_types = type_list<int, char, std::int16_t, std::int32_t, std::int64_t>;

using throwing_movable_and_move_assignable_types = type_list<throwing_copy_only, throwing_move_only>;

using nothrow_movable_and_move_assignable_types = type_list<copy_only, move_only>;

using non_movable_types = type_list<not_move_assignable, not_move_constructable>;

using default_swappable_types = type_lists::
    concat_t<primitive_types, nothrow_movable_and_move_assignable_types, throwing_movable_and_move_assignable_types>;

template <typename T>
class IsDefaultSwappableVForTypesWhichAreMovableTest : public IsSwappableVTest<T> {};

TYPED_TEST_SUITE(IsDefaultSwappableVForTypesWhichAreMovableTest, as_testing_types<default_swappable_types>, );

/// @test `is_default_swappable_v` returns `true` for the supplied types, which are movable
/// @tparam TypeParam The type to check
TYPED_TEST(IsDefaultSwappableVForTypesWhichAreMovableTest, IsTrue) {
  STATIC_ASSERT_TRUE(is_default_swappable_v<TypeParam>);
}

template <typename T>
class IsDefaultSwappableVForTypesWhichAreNotMovableTest : public IsSwappableVTest<T> {};

TYPED_TEST_SUITE(IsDefaultSwappableVForTypesWhichAreNotMovableTest, as_testing_types<non_movable_types>, );

/// @test `is_default_swappable_v` returns `false` for the supplied types, which are not movable
/// @tparam TypeParam The type to check
TYPED_TEST(IsDefaultSwappableVForTypesWhichAreNotMovableTest, IsFalse) {
  STATIC_ASSERT_FALSE(is_default_swappable_v<TypeParam>);
}

using nothrow_default_swappable_types =
    type_lists::concat_t<primitive_types, nothrow_movable_and_move_assignable_types>;

template <typename T>
class IsNothrowDefaultSwappableVForTypesWhichAreNothrowMovableTest : public IsSwappableVTest<T> {};

TYPED_TEST_SUITE(
    IsNothrowDefaultSwappableVForTypesWhichAreNothrowMovableTest,
    as_testing_types<nothrow_default_swappable_types>,
);

/// @test `is_nothrow_default_swappable_v` returns `true` for the supplied types, which are nothrow-movable
/// @tparam TypeParam The type to check
TYPED_TEST(IsNothrowDefaultSwappableVForTypesWhichAreNothrowMovableTest, IsTrue) {
  STATIC_ASSERT_TRUE(is_nothrow_default_swappable_v<TypeParam>);
}

using throwing_or_missing_move_types =
    type_lists::concat_t<non_movable_types, throwing_movable_and_move_assignable_types>;

template <typename T>
class IsNothrowDefaultSwappableVForTypesWhichAreNotNothrowMovableTest : public IsSwappableVTest<T> {};

TYPED_TEST_SUITE(
    IsNothrowDefaultSwappableVForTypesWhichAreNotNothrowMovableTest,
    as_testing_types<throwing_or_missing_move_types>,
);

/// @test `is_nothrow_default_swappable_v` returns `false` for the supplied types, which are not nothrow-movable
/// @tparam TypeParam The type to check
TYPED_TEST(IsNothrowDefaultSwappableVForTypesWhichAreNotNothrowMovableTest, IsFalse) {
  STATIC_ASSERT_FALSE(is_nothrow_default_swappable_v<TypeParam>);
}

//////////////////// ADL Swappable Types ////////////////////

struct adl_swap_swappable {};
ARENE_MAYBE_UNUSED void swap(adl_swap_swappable&, adl_swap_swappable&) {}

struct nothrow_adl_swappable {};
ARENE_MAYBE_UNUSED void swap(nothrow_adl_swappable&, nothrow_adl_swappable&) noexcept {}

using throwing_adl_swappable_types = type_list<adl_swap_swappable>;

using nothrow_adl_swappable_types = type_list<nothrow_adl_swappable>;

using adl_swappable_types = type_lists::concat_t<throwing_adl_swappable_types, nothrow_adl_swappable_types>;

template <typename T>
class IsADLSwappableVForTypesWithADLSwap : public IsSwappableVTest<T> {};

TYPED_TEST_SUITE(IsADLSwappableVForTypesWithADLSwap, as_testing_types<adl_swappable_types>, );

/// @test `is_adl_swappable_v` returns `true` for the supplied types, which have a `swap` overload available via ADL
/// @tparam TypeParam The type to check
TYPED_TEST(IsADLSwappableVForTypesWithADLSwap, IsTrue) { STATIC_ASSERT_TRUE(is_adl_swappable_v<TypeParam>); }

/// @test `is_adl_swappable_v` correctly handles supplied reference types, which have a `swap` overload available via
/// ADL
/// @tparam TypeParam The type to check
TYPED_TEST(IsADLSwappableVForTypesWithADLSwap, HandlesReferenceTypes) {
  STATIC_ASSERT_TRUE(is_adl_swappable_v<TypeParam&>);
  STATIC_ASSERT_FALSE(is_adl_swappable_v<TypeParam const&>);
  STATIC_ASSERT_TRUE(is_adl_swappable_v<TypeParam&&>);
  STATIC_ASSERT_FALSE(is_adl_swappable_v<TypeParam const&>);
}

template <typename T>
class IsADLSwappableVForTypesWithoutADLSwap : public IsSwappableVTest<T> {};

TYPED_TEST_SUITE(IsADLSwappableVForTypesWithoutADLSwap, as_testing_types<default_swappable_types>, );

/// @test `is_adl_swappable_v` returns `false` for the supplied types, which do not have a `swap` overload available via
/// ADL
/// @tparam TypeParam The type to check
TYPED_TEST(IsADLSwappableVForTypesWithoutADLSwap, IsFalse) { STATIC_ASSERT_FALSE(is_adl_swappable_v<TypeParam>); }

/// @test `is_adl_swappable_v` returns `false` for the supplied reference types, which do not have a `swap` overload
/// available via ADL
/// @tparam TypeParam The type to check
TYPED_TEST(IsADLSwappableVForTypesWithoutADLSwap, IsFalseForReferences) {
  STATIC_ASSERT_FALSE(is_adl_swappable_v<TypeParam&>);
  STATIC_ASSERT_FALSE(is_adl_swappable_v<TypeParam const&>);
  STATIC_ASSERT_FALSE(is_adl_swappable_v<TypeParam&&>);
  STATIC_ASSERT_FALSE(is_adl_swappable_v<TypeParam const&&>);
}

template <typename T>
class IsNothrowADLSwappableVForTypesWithNothrowADLSwap : public IsSwappableVTest<T> {};

TYPED_TEST_SUITE(IsNothrowADLSwappableVForTypesWithNothrowADLSwap, as_testing_types<nothrow_adl_swappable_types>, );

/// @test `is_nothrow_adl_swappable_v` returns `true` for the supplied types, which have a `swap` overload available via
/// ADL that is declared `noexcept`
/// @tparam TypeParam The type to check
TYPED_TEST(IsNothrowADLSwappableVForTypesWithNothrowADLSwap, IsTrue) {
  STATIC_ASSERT_TRUE(is_nothrow_adl_swappable_v<TypeParam>);
}

using throwing_adl_swap_or_no_adl_swap_types =
    type_lists::concat<default_swappable_types, throwing_adl_swappable_types>;

template <typename T>
class IsNothrowADLSwappableVForTypesWithoutNothrowADLSwap : public IsSwappableVTest<T> {};

TYPED_TEST_SUITE(
    IsNothrowADLSwappableVForTypesWithoutNothrowADLSwap,
    as_testing_types<throwing_adl_swap_or_no_adl_swap_types>,
);

/// @test `is_nothrow_adl_swappable_v` returns `false` for the supplied types, which either do not have a `swap`
/// overload available via ADL or, that overload is not declared `noexcept`
/// @tparam TypeParam The type to check
TYPED_TEST(IsNothrowADLSwappableVForTypesWithoutNothrowADLSwap, IsFalse) {
  STATIC_ASSERT_FALSE(is_nothrow_adl_swappable_v<TypeParam>);
}

struct adl_swap_swappable_both_first {};
struct adl_swap_swappable_both_second {};
ARENE_MAYBE_UNUSED void swap(adl_swap_swappable_both_first&, adl_swap_swappable_both_second&) {}
ARENE_MAYBE_UNUSED void swap(adl_swap_swappable_both_second&, adl_swap_swappable_both_first&) {}

/// @test `is_adl_swappable_with_v` returns `true` for the supplied types when not-const, which have swap functions
/// between them.
TEST(IsADLSwappableWith, WithForTypesWithBothADLSwap) {
  STATIC_ASSERT_TRUE(is_adl_swappable_with_v<adl_swap_swappable_both_first&, adl_swap_swappable_both_second&>);
  STATIC_ASSERT_TRUE(is_adl_swappable_with_v<adl_swap_swappable_both_second&, adl_swap_swappable_both_first&>);

  STATIC_ASSERT_FALSE(is_adl_swappable_with_v<
                      adl_swap_swappable_both_first const&,
                      adl_swap_swappable_both_second const&>);
  STATIC_ASSERT_FALSE(is_adl_swappable_with_v<
                      adl_swap_swappable_both_second const&,
                      adl_swap_swappable_both_first const&>);

  STATIC_ASSERT_FALSE(is_adl_swappable_with_v<adl_swap_swappable_both_first&&, adl_swap_swappable_both_second&&>);
  STATIC_ASSERT_FALSE(is_adl_swappable_with_v<adl_swap_swappable_both_second&&, adl_swap_swappable_both_first&&>);

  STATIC_ASSERT_FALSE(is_adl_swappable_with_v<
                      adl_swap_swappable_both_first const&&,
                      adl_swap_swappable_both_second const&&>);
  STATIC_ASSERT_FALSE(is_adl_swappable_with_v<
                      adl_swap_swappable_both_second const&&,
                      adl_swap_swappable_both_first const&&>);
}

struct adl_swap_swappable_missing_first {};
struct adl_swap_swappable_missing_second {};
ARENE_MAYBE_UNUSED void swap(adl_swap_swappable_missing_first&, adl_swap_swappable_missing_second&) {}

/// @test `is_adl_swappable_with_v` returns `false` for the supplied types, which are missing the swap function in one
/// direction.
TEST(IsADLSwappableWith, WithForTypesWithMissingADLSwap) {
  STATIC_ASSERT_FALSE(is_adl_swappable_with_v<adl_swap_swappable_missing_first&, adl_swap_swappable_missing_second&>);
  STATIC_ASSERT_FALSE(is_adl_swappable_with_v<adl_swap_swappable_missing_second&, adl_swap_swappable_missing_first&>);

  STATIC_ASSERT_FALSE(is_adl_swappable_with_v<
                      adl_swap_swappable_missing_first const&,
                      adl_swap_swappable_missing_second const&>);
  STATIC_ASSERT_FALSE(is_adl_swappable_with_v<
                      adl_swap_swappable_missing_second const&,
                      adl_swap_swappable_missing_first const&>);

  STATIC_ASSERT_FALSE(is_adl_swappable_with_v<adl_swap_swappable_missing_first&&, adl_swap_swappable_missing_second&&>);
  STATIC_ASSERT_FALSE(is_adl_swappable_with_v<adl_swap_swappable_missing_second&&, adl_swap_swappable_missing_first&&>);

  STATIC_ASSERT_FALSE(is_adl_swappable_with_v<
                      adl_swap_swappable_missing_first const&&,
                      adl_swap_swappable_missing_second const&&>);
  STATIC_ASSERT_FALSE(is_adl_swappable_with_v<
                      adl_swap_swappable_missing_second const&&,
                      adl_swap_swappable_missing_first const&&>);
}

/// @test `is_adl_swappable_with_v` returns `false` for types with no swap function between them.
TEST(IsADLSwappableWith, WithForTypesWithNoADLSwap) { STATIC_ASSERT_FALSE(is_adl_swappable_with_v<int&, float&>); }

struct nothrow_adl_swap_swappable_both_first {};
struct nothrow_adl_swap_swappable_both_second {};
ARENE_MAYBE_UNUSED void swap(nothrow_adl_swap_swappable_both_first&, nothrow_adl_swap_swappable_both_second&) noexcept(
    true
) {}
ARENE_MAYBE_UNUSED void swap(nothrow_adl_swap_swappable_both_second&, nothrow_adl_swap_swappable_both_first&) noexcept(
    true
) {}

/// @test `is_nothrow_adl_swappable_with_v` returns `true` for the supplied types which have nothrow swap functions
/// between them.
TEST(IsNothrowADLSwappableWith, WithForTypesWithBothNothrowADLSwap) {
  STATIC_ASSERT_TRUE(is_nothrow_adl_swappable_with_v<
                     nothrow_adl_swap_swappable_both_first&,
                     nothrow_adl_swap_swappable_both_second&>);
  STATIC_ASSERT_TRUE(is_nothrow_adl_swappable_with_v<
                     nothrow_adl_swap_swappable_both_second&,
                     nothrow_adl_swap_swappable_both_first&>);
}

struct throwing_adl_swap_swappable_with_first {};
ARENE_MAYBE_UNUSED void swap(nothrow_adl_swap_swappable_both_first&, throwing_adl_swap_swappable_with_first&) noexcept(
    true
) {}
ARENE_MAYBE_UNUSED void swap(throwing_adl_swap_swappable_with_first&, nothrow_adl_swap_swappable_both_first&) noexcept(
    false
) {}

/// @test `is_nothrow_adl_swappable_with_v` returns `false` for the supplied types which have one throwing swap
/// functions between them.
TEST(IsNothrowADLSwappableWith, WithForTypesWithOneNothrowADLSwap) {
  STATIC_ASSERT_FALSE(is_nothrow_adl_swappable_with_v<
                      nothrow_adl_swap_swappable_both_first&,
                      throwing_adl_swap_swappable_with_first&>);
  STATIC_ASSERT_FALSE(is_nothrow_adl_swappable_with_v<
                      throwing_adl_swap_swappable_with_first&,
                      nothrow_adl_swap_swappable_both_first&>);
}

/// @test `is_nothrow_adl_swappable_with_v` returns `false` for the supplied types which have no swap function between
/// them.
TEST(IsNothrowADLSwappableWith, WithForTypesWithNoADLSwap) {
  STATIC_ASSERT_FALSE(is_nothrow_adl_swappable_with_v<int&, float&>);
}

//////////////////// Swappable Types ////////////////////

using swappable_types = type_lists::concat_t<default_swappable_types, adl_swappable_types>;

template <typename T>
class IsSwappableVForTypesWhichAreDefaultOrADLSwappable : public IsSwappableVTest<T> {};

TYPED_TEST_SUITE(IsSwappableVForTypesWhichAreDefaultOrADLSwappable, as_testing_types<swappable_types>, );

/// @test `is_swappable_v` returns `true` for the supplied types, which are those types which are movable, or which have
/// a `swap` overload available via ADL.
/// @tparam TypeParam The type to check
TYPED_TEST(IsSwappableVForTypesWhichAreDefaultOrADLSwappable, IsTrue) { STATIC_ASSERT_TRUE(is_swappable_v<TypeParam>); }

/// @test `is_swappable_v` correctly handles the supplied reference types, which are those types which are movable, or
/// which have a `swap` overload available via ADL.
/// @tparam TypeParam The type to check
TYPED_TEST(IsSwappableVForTypesWhichAreDefaultOrADLSwappable, HandlesReferenceTypes) {
  STATIC_ASSERT_TRUE(is_swappable_v<TypeParam&>);
  STATIC_ASSERT_FALSE(is_swappable_v<TypeParam const&>);
  STATIC_ASSERT_TRUE(is_swappable_v<TypeParam&&>);
  STATIC_ASSERT_FALSE(is_swappable_v<TypeParam const&&>);
}

template <typename T>
class IsSwappableVForTypesWhichAreNotDefaultOrADLSwappable : public IsSwappableVTest<T> {};

TYPED_TEST_SUITE(IsSwappableVForTypesWhichAreNotDefaultOrADLSwappable, as_testing_types<non_movable_types>, );

/// @test `is_swappable_v` returns `false` for the supplied types, which are those types which are not movable, and
/// which do not have a `swap` overload available via ADL.
/// @tparam TypeParam The type to check
TYPED_TEST(IsSwappableVForTypesWhichAreNotDefaultOrADLSwappable, IsFalse) {
  STATIC_ASSERT_FALSE(is_swappable_v<TypeParam>);
}

/// @test `is_swappable_v` returns `false` for the supplied reference types, which are not movable, and which do not
/// have a `swap` overload available via ADL
/// @tparam TypeParam The type to check
TYPED_TEST(IsSwappableVForTypesWhichAreNotDefaultOrADLSwappable, IsFalseForReferences) {
  STATIC_ASSERT_FALSE(is_swappable_v<TypeParam&>);
  STATIC_ASSERT_FALSE(is_swappable_v<TypeParam const&>);
  STATIC_ASSERT_FALSE(is_swappable_v<TypeParam&&>);
  STATIC_ASSERT_FALSE(is_swappable_v<TypeParam const&&>);
}

using nothrow_swappable_types = type_lists::concat_t<nothrow_default_swappable_types, nothrow_adl_swappable_types>;

template <typename T>
class IsNothrowSwappableVForTypesWhichAreNothrowDefaultOrADLSwappable : public IsSwappableVTest<T> {};

TYPED_TEST_SUITE(
    IsNothrowSwappableVForTypesWhichAreNothrowDefaultOrADLSwappable,
    as_testing_types<nothrow_swappable_types>,
);

/// @test `is_nothrow_swappable_v` returns `true` for the supplied types, which are those types which are
/// nothrow-movable, or which have a `swap` overload available via ADL which is declared `noexcept`.
/// @tparam TypeParam The type to check
TYPED_TEST(IsNothrowSwappableVForTypesWhichAreNothrowDefaultOrADLSwappable, IsTrue) {
  STATIC_ASSERT_TRUE(is_nothrow_swappable_v<TypeParam>);
}

using throwing_default_adl_member_or_not_swappable =
    type_lists::concat_t<throwing_movable_and_move_assignable_types, throwing_adl_swappable_types, non_movable_types>;

template <typename T>
class IsNothrowSwappableVForTypesWhichAreNotNothrowDefaultOrADLSwappable : public IsSwappableVTest<T> {};

TYPED_TEST_SUITE(
    IsNothrowSwappableVForTypesWhichAreNotNothrowDefaultOrADLSwappable,
    as_testing_types<throwing_default_adl_member_or_not_swappable>,
);

/// @test `is_nothrow_swappable_v` returns `false` for the supplied types, which are those types for which
/// `is_swappable_v` evaluates to `false`, or for which the move operations or ADL-chosen `swap` overload, as
/// appropriate, are not declared `noexcept`.
/// @tparam TypeParam The type to check
TYPED_TEST(IsNothrowSwappableVForTypesWhichAreNotNothrowDefaultOrADLSwappable, IsFalse) {
  STATIC_ASSERT_FALSE(is_nothrow_swappable_v<TypeParam>);
}

/// @test `is_swappable_with_v` returns `true` for the supplied types when not-const, which have swap functions between
/// them.
TEST(IsSwappableWith, WithForTypesWithBothADLSwap) {
  STATIC_ASSERT_TRUE(is_swappable_with_v<adl_swap_swappable_both_first&, adl_swap_swappable_both_second&>);
  STATIC_ASSERT_TRUE(is_swappable_with_v<adl_swap_swappable_both_second&, adl_swap_swappable_both_first&>);

  STATIC_ASSERT_FALSE(is_swappable_with_v<adl_swap_swappable_both_first const&, adl_swap_swappable_both_second const&>);
  STATIC_ASSERT_FALSE(is_swappable_with_v<adl_swap_swappable_both_second const&, adl_swap_swappable_both_first const&>);

  STATIC_ASSERT_FALSE(is_swappable_with_v<adl_swap_swappable_both_first&&, adl_swap_swappable_both_second&&>);
  STATIC_ASSERT_FALSE(is_swappable_with_v<adl_swap_swappable_both_second&&, adl_swap_swappable_both_first&&>);

  STATIC_ASSERT_FALSE(is_swappable_with_v<
                      adl_swap_swappable_both_first const&&,
                      adl_swap_swappable_both_second const&&>);
  STATIC_ASSERT_FALSE(is_swappable_with_v<
                      adl_swap_swappable_both_second const&&,
                      adl_swap_swappable_both_first const&&>);
}

/// @test `is_swappable_with_v` returns `false` for the supplied types, which are missing the swap function in one
/// direction.
TEST(IsSwappableWith, WithForTypesWithMissingADLSwap) {
  STATIC_ASSERT_FALSE(is_swappable_with_v<adl_swap_swappable_missing_first&, adl_swap_swappable_missing_second&>);
  STATIC_ASSERT_FALSE(is_swappable_with_v<adl_swap_swappable_missing_second&, adl_swap_swappable_missing_first&>);

  STATIC_ASSERT_FALSE(is_swappable_with_v<
                      adl_swap_swappable_missing_first const&,
                      adl_swap_swappable_missing_second const&>);
  STATIC_ASSERT_FALSE(is_swappable_with_v<
                      adl_swap_swappable_missing_second const&,
                      adl_swap_swappable_missing_first const&>);

  STATIC_ASSERT_FALSE(is_swappable_with_v<adl_swap_swappable_missing_first&&, adl_swap_swappable_missing_second&&>);
  STATIC_ASSERT_FALSE(is_swappable_with_v<adl_swap_swappable_missing_second&&, adl_swap_swappable_missing_first&&>);

  STATIC_ASSERT_FALSE(is_swappable_with_v<
                      adl_swap_swappable_missing_first const&&,
                      adl_swap_swappable_missing_second const&&>);
  STATIC_ASSERT_FALSE(is_swappable_with_v<
                      adl_swap_swappable_missing_second const&&,
                      adl_swap_swappable_missing_first const&&>);
}

/// @test `is_adl_swappable_with_v` returns `false` for types with no swap function between them.
TEST(IsSwappableWith, WithForTypesWithNoADLSwap) { STATIC_ASSERT_FALSE(is_swappable_with_v<int&, float&>); }

/// @test `is_nothrow_swappable_with_v` returns `true` for the supplied types which have nothrow swap functions between
/// them.
TEST(IsNothrowSwappableWith, WithForTypesWithBothNothrowADLSwap) {
  STATIC_ASSERT_TRUE(is_nothrow_swappable_with_v<
                     nothrow_adl_swap_swappable_both_first&,
                     nothrow_adl_swap_swappable_both_second&>);
  STATIC_ASSERT_TRUE(is_nothrow_swappable_with_v<
                     nothrow_adl_swap_swappable_both_second&,
                     nothrow_adl_swap_swappable_both_first&>);
}

/// @test `is_nothrow_swappable_with_v` returns `false` for the supplied types which have one throwing swap functions
/// between them.
TEST(IsNothrowSwappableWith, WithForTypesWithOneNothrowADLSwap) {
  STATIC_ASSERT_FALSE(is_nothrow_swappable_with_v<
                      nothrow_adl_swap_swappable_both_first&,
                      throwing_adl_swap_swappable_with_first&>);
  STATIC_ASSERT_FALSE(is_nothrow_swappable_with_v<
                      throwing_adl_swap_swappable_with_first&,
                      nothrow_adl_swap_swappable_both_first&>);
}

/// @test `is_nothrow_swappable_with_v` returns `false` for the supplied types which have no swap function between them.
TEST(IsNothrowSwappableWith, WithForTypesWithNoADLSwap) {
  STATIC_ASSERT_FALSE(is_nothrow_swappable_with_v<int&, float&>);
}

}  // namespace
