// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

using ::testing::is_unambiguously_publicly_derived_from_v;

// NOLINTNEXTLINE(hicpp-special-member-functions)
class private_destructor {
 private:
  ~private_destructor() = default;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class protected_destructor {
 protected:
  ~protected_destructor() = default;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class deleted_destructor {
 public:
  ~deleted_destructor() = delete;
};

struct non_destructible_member {
  deleted_destructor dummy;
};

class non_destructible_base : deleted_destructor {};

template <bool Expected, class T>
constexpr auto check_is_destructible() -> bool {
  return is_unambiguously_publicly_derived_from_v<std::is_destructible<T>, std::bool_constant<Expected>> &&
         (std::is_destructible_v<T> == Expected);
}

template <bool Expected, class T>
constexpr auto check_is_nothrow_destructible() -> bool {
  return is_unambiguously_publicly_derived_from_v<std::is_nothrow_destructible<T>, std::bool_constant<Expected>> &&
         (std::is_nothrow_destructible_v<T> == Expected);
}

template <bool Expected, class T>
constexpr auto check_is_trivially_destructible() -> bool {
  return is_unambiguously_publicly_derived_from_v<std::is_trivially_destructible<T>, std::bool_constant<Expected>> &&
         (std::is_trivially_destructible_v<T> == Expected);
}

using destructible_types = arene::base::type_lists::concat_unique_t<
    ::testing::object_types,
    ::testing::pointer_types,
    ::testing::member_pointer_types,
    ::testing::class_types,
    ::testing::union_types,
    ::testing::enum_types,
    ::testing::reference_types,
    ::testing::Types<
        // NOLINTBEGIN(hicpp-avoid-c-arrays)
        int[3],
        int[5][3],
        ::testing::class_type[3],
        ::testing::class_type[5][3],
        // NOLINTEND(hicpp-avoid-c-arrays)
        deleted_destructor&,
        deleted_destructor const&,
        deleted_destructor volatile&,
        deleted_destructor const volatile&,
        deleted_destructor&&,
        deleted_destructor const&&,
        deleted_destructor volatile&&,
        deleted_destructor const volatile&&>>;

template <typename T>
class IsDestructibleTest : public testing::Test {};

TYPED_TEST_SUITE(IsDestructibleTest, destructible_types, );

/// @test Types that are destructible yield @c true for @c is_destructible
TYPED_TEST(IsDestructibleTest, DestructibleTypesYieldTrue) {
  static_assert(check_is_destructible<true, TypeParam>(), "Type is destructible");
  static_assert(check_is_destructible<true, TypeParam const>(), "Type is destructible");
  static_assert(check_is_destructible<true, TypeParam volatile>(), "Type is destructible");
  static_assert(check_is_destructible<true, TypeParam const volatile>(), "Type is destructible");
}

using nothrow_destructible_types = destructible_types;

template <typename T>
class IsNothrowDestructibleTest : public testing::Test {};

TYPED_TEST_SUITE(IsNothrowDestructibleTest, nothrow_destructible_types, );

/// @test Types that are nothrow destructible yield @c true for @c is_nothrow_destructible
TYPED_TEST(IsNothrowDestructibleTest, NothrowDestructibleTypesYieldTrue) {
  static_assert(check_is_nothrow_destructible<true, TypeParam>(), "Type is Nothrow Destructible");
  static_assert(check_is_nothrow_destructible<true, TypeParam const>(), "Type is Nothrow Destructible");
  static_assert(check_is_nothrow_destructible<true, TypeParam volatile>(), "Type is Nothrow Destructible");
  static_assert(check_is_nothrow_destructible<true, TypeParam const volatile>(), "Type is Nothrow Destructible");
}

class trivial_class {};

class non_trivial_constructor {
  int i_;

 public:
  non_trivial_constructor()
      : i_(42) {}

  auto value() const -> int { return i_; }
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class non_trivial_destructor {
 public:
  // NOLINTNEXTLINE(performance-trivially-destructible)
  ~non_trivial_destructor();
};

non_trivial_destructor::~non_trivial_destructor() = default;

class trivial_with_base_class_and_member : trivial_class {
  non_trivial_constructor i_{};
};

class non_trivial_base_class_destructor : non_trivial_destructor {};

class non_trivial_member_destructor {
  non_trivial_destructor i_{};
};

using trivially_destructible_types = arene::base::type_lists::concat_unique_t<
    destructible_types,
    testing::Types<trivial_class, non_trivial_constructor, trivial_with_base_class_and_member>>;

template <typename T>
class IsTriviallyDestructibleTest : public testing::Test {};

TYPED_TEST_SUITE(IsTriviallyDestructibleTest, trivially_destructible_types, );

/// @test Types that are trivially destructible yield @c true for @c is_trivially_destructible and @c is_destructible
TYPED_TEST(IsTriviallyDestructibleTest, TriviallyDestructibleTypesYieldTrue) {
  static_assert(check_is_destructible<true, TypeParam>(), "Type is Destructible");
  static_assert(check_is_destructible<true, TypeParam const>(), "Type is Destructible");
  static_assert(check_is_destructible<true, TypeParam volatile>(), "Type is Destructible");
  static_assert(check_is_destructible<true, TypeParam const volatile>(), "Type is Destructible");
  static_assert(check_is_nothrow_destructible<true, TypeParam>(), "Type is nothrow Destructible");
  static_assert(check_is_nothrow_destructible<true, TypeParam const>(), "Type is nothrow Destructible");
  static_assert(check_is_nothrow_destructible<true, TypeParam volatile>(), "Type is nothrow Destructible");
  static_assert(check_is_nothrow_destructible<true, TypeParam const volatile>(), "Type is nothrow Destructible");
  static_assert(check_is_trivially_destructible<true, TypeParam>(), "Type is Trivially Destructible");
  static_assert(check_is_trivially_destructible<true, TypeParam const>(), "Type is Trivially Destructible");
  static_assert(check_is_trivially_destructible<true, TypeParam volatile>(), "Type is Trivially Destructible");
  static_assert(check_is_trivially_destructible<true, TypeParam const volatile>(), "Type is Trivially Destructible");
}

using non_destructible_classes = ::testing::
    Types<private_destructor, protected_destructor, deleted_destructor, non_destructible_member, non_destructible_base>;

using not_destructible_types = arene::base::type_lists::concat_unique_t<
    ::testing::function_types,
    ::testing::Types<
        // NOLINTBEGIN(hicpp-avoid-c-arrays)
        int[],
        int[][5],
        ::testing::class_type[],
        ::testing::class_type[][5],
        // NOLINTEND(hicpp-avoid-c-arrays)
        void>,
    non_destructible_classes>;

template <typename T>
class IsNotDestructibleTest : public testing::Test {};

TYPED_TEST_SUITE(IsNotDestructibleTest, not_destructible_types, );

/// @test Types that are not destructible yield @c false for @c is_destructible
TYPED_TEST(IsNotDestructibleTest, NotDestructibleTypesYieldFalse) {
  static_assert(check_is_destructible<false, TypeParam>(), "Type is not destructible");
  static_assert(check_is_destructible<false, TypeParam const>(), "Type is not destructible");
  static_assert(check_is_destructible<false, TypeParam volatile>(), "Type is not destructible");
  static_assert(check_is_destructible<false, TypeParam const volatile>(), "Type is not destructible");
}

// NOLINTNEXTLINE(hicpp-special-member-functions)
class noexcept_false_destructor {
 public:
  ~noexcept_false_destructor() noexcept(false) = default;
};

class throwing_base_class_destructor : noexcept_false_destructor {};

struct throwing_member_destructor {
  noexcept_false_destructor dummy;
};

using not_nothrow_destructible_types = arene::base::type_lists::concat_unique_t<
    not_destructible_types,
    ::testing::Types<
        noexcept_false_destructor,
        throwing_base_class_destructor,
        throwing_member_destructor,
        noexcept_false_destructor>>;

template <typename T>
class IsNotNothrowDestructibleTest : public testing::Test {};

TYPED_TEST_SUITE(IsNotNothrowDestructibleTest, not_destructible_types, );

/// @test Types that are not nothrow destructible yield @c false for @c is_nothrow_destructible
TYPED_TEST(IsNotNothrowDestructibleTest, NotNothrowDestructibleTypesYieldFalse) {
  static_assert(check_is_nothrow_destructible<false, TypeParam>(), "Type is not nothrow destructible");
  static_assert(check_is_nothrow_destructible<false, TypeParam const>(), "Type is not nothrow destructible");
  static_assert(check_is_nothrow_destructible<false, TypeParam volatile>(), "Type is not nothrow destructible");
  static_assert(check_is_nothrow_destructible<false, TypeParam const volatile>(), "Type is not nothrow destructible");
}

using non_trivial_destructible_types = testing::Types<
    non_trivial_destructor,
    non_trivial_base_class_destructor,
    non_trivial_member_destructor,
    // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
    non_trivial_destructor[3]>;

/// @test Types that are not destructible yield @c false for @c is_trivially_destructible
TYPED_TEST(IsNotDestructibleTest, NotDestructibleTypesAreNotTriviallyDestructible) {
  static_assert(check_is_trivially_destructible<false, TypeParam>(), "Type is not trivially destructible");
  static_assert(check_is_trivially_destructible<false, TypeParam const>(), "Type is not trivially destructible");
  static_assert(check_is_trivially_destructible<false, TypeParam volatile>(), "Type is not trivially destructible");
  static_assert(
      check_is_trivially_destructible<false, TypeParam const volatile>(),
      "Type is not trivially destructible"
  );
}

template <typename T>
class IsNotTriviallyDestructibleTest : public testing::Test {};

TYPED_TEST_SUITE(IsNotTriviallyDestructibleTest, non_trivial_destructible_types, );

/// @test Destructible but not trivially destructible types yield @c true for @c is_destructible, but @c false for @c
/// is_trivially_destructible
TYPED_TEST(IsNotTriviallyDestructibleTest, NotTriviallyDestructibleTypesYieldFalse) {
  static_assert(check_is_destructible<true, TypeParam>(), "Type is destructible");
  static_assert(check_is_destructible<true, TypeParam const>(), "Type is destructible");
  static_assert(check_is_destructible<true, TypeParam volatile>(), "Type is destructible");
  static_assert(check_is_destructible<true, TypeParam const volatile>(), "Type is destructible");

  static_assert(check_is_trivially_destructible<false, TypeParam>(), "Type is not trivially destructible");
  static_assert(check_is_trivially_destructible<false, TypeParam const>(), "Type is not trivially destructible");
  static_assert(check_is_trivially_destructible<false, TypeParam volatile>(), "Type is not trivially destructible");
  static_assert(
      check_is_trivially_destructible<false, TypeParam const volatile>(),
      "Type is not trivially destructible"
  );
}

}  // namespace
