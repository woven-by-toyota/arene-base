// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

using testing::is_unambiguously_publicly_derived_from_v;

template <bool Expected, class T>
constexpr auto check_is_move_assignable() -> bool {
  return is_unambiguously_publicly_derived_from_v<std::is_move_assignable<T>, std::bool_constant<Expected>> &&
         (std::is_move_assignable_v<T> == Expected);
}

template <bool Expected, class T>
constexpr auto check_is_nothrow_move_assignable() -> bool {
  return is_unambiguously_publicly_derived_from_v<std::is_nothrow_move_assignable<T>, std::bool_constant<Expected>> &&
         (std::is_nothrow_move_assignable_v<T> == Expected);
}

template <bool Expected, class T>
constexpr auto check_is_trivially_move_assignable() -> bool {
  return is_unambiguously_publicly_derived_from_v<std::is_trivially_move_assignable<T>, std::bool_constant<Expected>> &&
         (std::is_trivially_move_assignable_v<T> == Expected);
}

class some_user_type {};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class trivial_defaulted_move_assign {
 public:
  auto operator=(trivial_defaulted_move_assign&&) noexcept -> trivial_defaulted_move_assign& = default;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class trivial_move_assign_by_copy {
 public:
  auto operator=(trivial_move_assign_by_copy const&) noexcept -> trivial_move_assign_by_copy& = default;
};

class trivial_base_class_move : trivial_defaulted_move_assign {};

struct trivial_member_move {
  trivial_defaulted_move_assign dummy;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class non_trivial_move_assign {
 public:
  auto operator=(non_trivial_move_assign&&) noexcept -> non_trivial_move_assign&;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class non_trivial_move_assign_by_copy {
 public:
  auto operator=(non_trivial_move_assign_by_copy const&) noexcept -> non_trivial_move_assign_by_copy&;
};

class non_trivial_base_class_move : non_trivial_move_assign {};

struct non_trivial_member_move {
  non_trivial_move_assign dummy;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class non_trivial_destructor {
 public:
  ~non_trivial_destructor();
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class not_move_assignable {
 public:
  auto operator=(not_move_assignable&&) -> not_move_assignable& = delete;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class throwing_move_assignable {
 public:
  auto operator=(throwing_move_assignable&&) noexcept(false) -> throwing_move_assignable&;
};

/// @test A type is move assignable but not nothrow-move-assignable with throwing assign
TEST(MoveAssignable, ThrowingMoveAssignableType) {
  static_assert(check_is_move_assignable<true, throwing_move_assignable>(), "Should be move assignable");
  static_assert(
      check_is_nothrow_move_assignable<false, throwing_move_assignable>(),
      "Should not be nothrow move assignable"
  );
}

using move_assignable_types = arene::base::type_lists::concat_unique_t<
    ::testing::scalar_types,
    ::testing::union_types,
    ::testing::class_types,
    ::testing::Types<int&, some_user_type&, int*&, some_user_type>>;

template <typename T>
class MoveAssignableTest : public testing::Test {};

TYPED_TEST_SUITE(MoveAssignableTest, move_assignable_types, );

/// @test A type is move assignable
TYPED_TEST(MoveAssignableTest, IsMoveAssignable) {
  static_assert(check_is_move_assignable<true, TypeParam>(), "A type is move assignable");
  static_assert(check_is_nothrow_move_assignable<true, TypeParam>(), "A type is nothrow move assignable");
}

using not_move_assignable_types = arene::base::type_lists::concat_unique_t<
    testing::array_types,
    testing::non_referenceable_types,
    testing::Types<not_move_assignable, int const&, some_user_type const&, int* const&>>;

template <typename T>
class NotMoveAssignableTest : public testing::Test {};

TYPED_TEST_SUITE(NotMoveAssignableTest, not_move_assignable_types, );

/// @test A type is not move assignable
TYPED_TEST(NotMoveAssignableTest, IsNotMoveAssignable) {
  static_assert(check_is_move_assignable<false, TypeParam>(), "A type is not move assignable");
  static_assert(check_is_nothrow_move_assignable<false, TypeParam>(), "A type is not nothrow move assignable");
}

using non_trivial_move_assignable_types = testing::Types<
    non_trivial_move_assign,
    non_trivial_base_class_move,
    non_trivial_member_move,
    non_trivial_move_assign_by_copy>;

template <typename T>
class NonTrivialMoveAssignableTest : public testing::Test {};

TYPED_TEST_SUITE(NonTrivialMoveAssignableTest, non_trivial_move_assignable_types, );

/// @test A type is move assignable but not trivially move assignable
TYPED_TEST(NonTrivialMoveAssignableTest, MoveAssignableNotTriviallyMoveAssignable) {
  static_assert(check_is_move_assignable<true, TypeParam>(), "A type is move assignable");
  static_assert(check_is_trivially_move_assignable<false, TypeParam>(), "A type is not trivially move assignable");
}

using trivial_move_assignable_types = arene::base::type_lists::concat_unique_t<
    testing::scalar_types,
    testing::union_types,
    testing::class_types,
    testing::Types<
        trivial_defaulted_move_assign,
        trivial_base_class_move,
        trivial_member_move,
        non_trivial_destructor,
        trivial_move_assign_by_copy>>;

template <typename T>
class TrivialMoveAssignableTest : public testing::Test {};

TYPED_TEST_SUITE(TrivialMoveAssignableTest, trivial_move_assignable_types, );

/// @test A type is trivially move assignable
TYPED_TEST(TrivialMoveAssignableTest, MoveAssignableNotTriviallyMoveAssignable) {
  static_assert(check_is_move_assignable<true, TypeParam>(), "A type is move assignable");
  static_assert(check_is_trivially_move_assignable<true, TypeParam>(), "A type is trivially move assignable");
}

}  // namespace
