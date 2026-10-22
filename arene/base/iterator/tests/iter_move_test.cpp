// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/iter_move.hpp"

#include <gtest/gtest.h>

#include "arene/base/iterator/tests/test_helpers.hpp"
#include "arene/base/stdlib_choice/copy_n.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace {

namespace iterator_tests = ::arene::base::iterator_tests;
enum class noexceptness { yes, no };

namespace adl {
template <noexceptness Noexceptness>
struct specialized_itr {
  int value;
};

template <noexceptness Noexceptness>
constexpr auto iter_move(specialized_itr<Noexceptness>& itr) noexcept(Noexceptness == noexceptness::yes) -> int& {
  return itr.value;
}

template <noexceptness Noexceptness>
constexpr auto iter_move(specialized_itr<Noexceptness> const& itr) noexcept(Noexceptness == noexceptness::yes)
    -> int const& {
  return itr.value;
}

template <noexceptness Noexceptness>
constexpr auto iter_move(specialized_itr<Noexceptness>&& itr) noexcept(Noexceptness == noexceptness::yes) -> int&& {
  return std::move(itr.value);
}

template <noexceptness Noexceptness>
constexpr auto iter_move(specialized_itr<Noexceptness> const&& itr) noexcept(Noexceptness == noexceptness::yes)
    -> int const&& {
  return std::move(itr.value);
}

struct adl_and_dereferenceable_itr {
  int value;
  bool adl_called;

  constexpr auto operator*() & noexcept -> int& { return value; }
};

constexpr auto iter_move(adl_and_dereferenceable_itr& itr) noexcept -> int& {
  itr.adl_called = true;
  return itr.value;
}

struct value_proxy {
  int const* value;

  constexpr explicit operator int() const noexcept { return *value; }
};

struct adl_and_dereferenceable_with_proxy_return_type_itr {
  int value;

  constexpr auto operator*() & noexcept -> value_proxy { return value_proxy{&value}; }
};

constexpr auto iter_move(adl_and_dereferenceable_with_proxy_return_type_itr& itr) noexcept -> int { return itr.value; }
}  // namespace adl

template <noexceptness Noexceptness>
struct rvalue_dereference_iterator {
  static constexpr bool is_noexcept = Noexceptness == noexceptness::yes;
  int value;

  constexpr explicit rvalue_dereference_iterator(int initial_value) noexcept
      : value{initial_value} {}
  constexpr auto operator*() & noexcept(Noexceptness == noexceptness::yes) -> int&& {
    return static_cast<int&&>(value);
  }
  constexpr auto operator*() const& noexcept(Noexceptness == noexceptness::yes) -> int const&& {
    return static_cast<int const&&>(value);
  }
  constexpr auto operator*() && noexcept(Noexceptness == noexceptness::yes) -> int&& {
    return static_cast<int&&>(value);
  }
  constexpr auto operator*() const&& noexcept(Noexceptness == noexceptness::yes) -> int const&& {
    return static_cast<int const&&>(value);
  }
};

/// @test Given a type with an ADL customization for lvalue arguments, when `::arene::base::ranges::iter_move` is
/// called, then it returns the customized result.
CONSTEXPR_TEST(IterMoveSpecializedTypesTest, UsesAdlCustomizationForLvalue) {
  adl::specialized_itr<noexceptness::yes> itr{42};
  auto&& moved = ::arene::base::ranges::iter_move(itr);
  ::testing::StaticAssertTypeEq<decltype(::arene::base::ranges::iter_move(itr)), int&>();
  CONSTEXPR_ASSERT_EQ(&moved, &itr.value);
}

/// @test Given a const lvalue type with an ADL customization, when `::arene::base::ranges::iter_move` is called,
/// then it returns the customized result.
CONSTEXPR_TEST(IterMoveSpecializedTypesTest, UsesAdlCustomizationForConstLvalue) {
  adl::specialized_itr<noexceptness::yes> const itr{42};
  auto&& moved = ::arene::base::ranges::iter_move(itr);
  ::testing::StaticAssertTypeEq<decltype(::arene::base::ranges::iter_move(itr)), int const&>();
  CONSTEXPR_ASSERT_EQ(&moved, &itr.value);
}

/// @test Given a type passed as an rvalue with an ADL customization, when `::arene::base::ranges::iter_move` is
/// called, then it returns the customized result.
CONSTEXPR_TEST(IterMoveSpecializedTypesTest, UsesAdlCustomization) {
  adl::specialized_itr<noexceptness::yes> itr{42};
  auto&& moved = ::arene::base::ranges::iter_move(static_cast<decltype(itr)&&>(itr));
  ::testing::StaticAssertTypeEq<decltype(::arene::base::ranges::iter_move(static_cast<decltype(itr)&&>(itr))), int&&>();
  // NOLINTNEXTLINE(bugprone-use-after-move): This verifies that iter_move preserves the referent.
  CONSTEXPR_ASSERT_EQ(&moved, &itr.value);
}

/// @test Given a const type passed as an rvalue with an ADL customization, when `::arene::base::ranges::iter_move`
/// is called, then it returns the customized result.
CONSTEXPR_TEST(IterMoveSpecializedTypesTest, UsesAdlCustomizationForConstType) {
  adl::specialized_itr<noexceptness::yes> const itr{42};
  auto&& moved = ::arene::base::ranges::iter_move(static_cast<decltype(itr)&&>(itr));
  ::testing::
      StaticAssertTypeEq<decltype(::arene::base::ranges::iter_move(static_cast<decltype(itr)&&>(itr))), int const&&>();
  // NOLINTNEXTLINE(bugprone-use-after-move): This verifies that iter_move preserves the referent.
  CONSTEXPR_ASSERT_EQ(&moved, &itr.value);
}

/// @test Given a type for which both `*itr` and unqualified ADL lookup of `iter_move(itr)` are well-formed, when
/// `::arene::base::ranges::iter_move` is called, then the ADL customization is selected.
CONSTEXPR_TEST(IterMoveSpecializedTypesTest, AdlCustomizationTakesPrecedenceOverDereference) {
  adl::adl_and_dereferenceable_itr itr{42, false};
  ::testing::StaticAssertTypeEq<decltype(*itr), int&>();
  ::testing::StaticAssertTypeEq<decltype(adl::iter_move(itr)), int&>();
  auto&& moved = ::arene::base::ranges::iter_move(itr);
  CONSTEXPR_ASSERT_EQ(&moved, &itr.value);
  CONSTEXPR_ASSERT(itr.adl_called);
}

/// @test Given a type `Itr` for which both `*itr` and unqualified ADL lookup of `iter_move(itr)` are well-formed and
/// return distinct types but the relationship `iter_move(Itr) == *Itr` holds semantically, when
/// `::arene::base::ranges::iter_move` is called, then the ADL customization is selected.
CONSTEXPR_TEST(IterMoveSpecializedTypesTest, AdlCustomizationTakesPrecedenceOverProxyDereference) {
  adl::adl_and_dereferenceable_with_proxy_return_type_itr itr{};
  CONSTEXPR_ASSERT(!std::is_same<decltype(*itr), decltype(adl::iter_move(itr))>::value);
  ::testing::StaticAssertTypeEq<decltype(::arene::base::ranges::iter_move(itr)), decltype(adl::iter_move(itr))>();
  auto moved = ::arene::base::ranges::iter_move(itr);
  CONSTEXPR_ASSERT_EQ(moved, itr.value);
}

using specialized_iterator_types =
    ::testing::Types<adl::specialized_itr<noexceptness::yes>, adl::specialized_itr<noexceptness::no>>;

template <typename T>
class IterMoveSpecializedNoexceptTest : public ::testing::Test {};

TYPED_TEST_SUITE(IterMoveSpecializedNoexceptTest, specialized_iterator_types, );

/// @test Given a specialized type, when `::arene::base::ranges::iter_move` is called with any cvref qualification,
/// then its noexcept-ness matches the corresponding ADL customization.
TYPED_TEST(IterMoveSpecializedNoexceptTest, NoexceptMatchesAdl) {
  STATIC_ASSERT_EQ(
      noexcept(::arene::base::ranges::iter_move(std::declval<TypeParam&>())),
      noexcept(adl::iter_move(std::declval<TypeParam&>()))
  );
  STATIC_ASSERT_EQ(
      noexcept(::arene::base::ranges::iter_move(std::declval<TypeParam const&>())),
      noexcept(adl::iter_move(std::declval<TypeParam const&>()))
  );
  STATIC_ASSERT_EQ(
      noexcept(::arene::base::ranges::iter_move(std::declval<TypeParam&&>())),
      noexcept(adl::iter_move(std::declval<TypeParam&&>()))
  );
  STATIC_ASSERT_EQ(
      noexcept(::arene::base::ranges::iter_move(std::declval<TypeParam const&&>())),
      noexcept(adl::iter_move(std::declval<TypeParam const&&>()))
  );
}

template <typename T>
class IterMoveNonSpecializedTypesTest : public ::testing::Test {
 protected:
  iterator_tests::backing_data<T> backing_data_{};
};

TYPED_TEST_SUITE(IterMoveNonSpecializedTypesTest, iterator_tests::all_iterator_category_containers, );

/// @test Given a type without an ADL customization whose dereference expression yields an lvalue reference, when
/// `::arene::base::ranges::iter_move` is called, then its result is equivalent to `std::move(*itr)`.
TYPED_TEST(IterMoveNonSpecializedTypesTest, LvalueDereferenceReturnsMove) {
  auto itr = this->backing_data_.begin();
  ::testing::StaticAssertTypeEq<decltype(::arene::base::ranges::iter_move(itr)), decltype(std::move(*itr))>();
  auto&& moved = ::arene::base::ranges::iter_move(itr);
  EXPECT_EQ(&moved, &*std::move(itr));
}

template <typename T>
class IterMoveConstNonSpecializedTypesTest : public ::testing::Test {
 protected:
  iterator_tests::backing_data<T> backing_data_{};
};

TYPED_TEST_SUITE(IterMoveConstNonSpecializedTypesTest, iterator_tests::at_least_forward_iterator_containers, );

/// @test Given a type without an ADL customization whose dereference expression yields a const lvalue reference,
/// when `::arene::base::ranges::iter_move` is called, then its result is equivalent to `std::move(*itr)`.
TYPED_TEST(IterMoveConstNonSpecializedTypesTest, ConstLvalueDereferenceReturnsMove) {
  auto const& backing_data = this->backing_data_;
  auto itr = backing_data.begin();
  ::testing::StaticAssertTypeEq<decltype(::arene::base::ranges::iter_move(itr)), decltype(std::move(*itr))>();
  auto&& moved = ::arene::base::ranges::iter_move(itr);
  EXPECT_EQ(&moved, &*std::move(itr));
}

/// @test Given a type whose dereference expression yields an rvalue reference, when
/// `::arene::base::ranges::iter_move` is called, then it preserves the dereference result.
CONSTEXPR_TEST(IterMoveRvalueDereferenceTest, RvalueDereferenceReturnsRvalue) {
  rvalue_dereference_iterator<noexceptness::yes> itr{42};
  auto&& moved = ::arene::base::ranges::iter_move(itr);
  ::testing::StaticAssertTypeEq<decltype(::arene::base::ranges::iter_move(itr)), int&&>();
  CONSTEXPR_ASSERT_EQ(&moved, &itr.value);
}

/// @test Given a type whose const dereference expression yields a const rvalue reference, when
/// `::arene::base::ranges::iter_move` is called, then it preserves the dereference result.
CONSTEXPR_TEST(IterMoveRvalueDereferenceTest, ConstRvalueDereferenceReturnsConstRvalue) {
  rvalue_dereference_iterator<noexceptness::yes> const itr{42};
  auto&& moved = ::arene::base::ranges::iter_move(itr);
  ::testing::StaticAssertTypeEq<decltype(::arene::base::ranges::iter_move(itr)), int const&&>();
  CONSTEXPR_ASSERT_EQ(&moved, &itr.value);
}

using rvalue_dereference_iterator_types =
    ::testing::Types<rvalue_dereference_iterator<noexceptness::yes>, rvalue_dereference_iterator<noexceptness::no>>;

template <typename T>
class IterMoveNonSpecializedNoexceptTest : public ::testing::Test {};

TYPED_TEST_SUITE(IterMoveNonSpecializedNoexceptTest, rvalue_dereference_iterator_types, );

/// @test Given a type without an ADL customization, when `::arene::base::ranges::iter_move` is called with any
/// cvref qualification, then its noexcept-ness matches the corresponding dereference expression.
TYPED_TEST(IterMoveNonSpecializedNoexceptTest, NoexceptMatchesDereference) {
  STATIC_ASSERT_EQ(
      noexcept(::arene::base::ranges::iter_move(std::declval<TypeParam&>())),
      noexcept(*std::declval<TypeParam&>())
  );
  STATIC_ASSERT_EQ(
      noexcept(::arene::base::ranges::iter_move(std::declval<TypeParam const&>())),
      noexcept(*std::declval<TypeParam const&>())
  );
  STATIC_ASSERT_EQ(
      noexcept(::arene::base::ranges::iter_move(std::declval<TypeParam&&>())),
      noexcept(*std::declval<TypeParam&&>())
  );
  STATIC_ASSERT_EQ(
      noexcept(::arene::base::ranges::iter_move(std::declval<TypeParam const&&>())),
      noexcept(*std::declval<TypeParam const&&>())
  );
}

template <typename T>
class IterMoveNonDeferenceablesTest : public ::testing::Test {};

struct non_dereferenceable {};

using non_dereferenceable_types = ::testing::Types<int, void, non_dereferenceable>;
TYPED_TEST_SUITE(IterMoveNonDeferenceablesTest, non_dereferenceable_types, );

/// @test Given a type with neither an ADL customization nor a valid dereference expression, when
/// `::arene::base::ranges::iter_move` is called, then it is not callable.
TYPED_TEST(IterMoveNonDeferenceablesTest, NotCallableForNonDereferenceableTypes) {
  STATIC_ASSERT_FALSE((arene::base::is_invocable_v<decltype(::arene::base::ranges::iter_move), TypeParam>));
}

}  // namespace
