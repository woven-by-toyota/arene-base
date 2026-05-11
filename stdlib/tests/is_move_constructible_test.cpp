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
constexpr auto check_is_move_constructible() -> bool {
  return is_unambiguously_publicly_derived_from_v<std::is_move_constructible<T>, std::bool_constant<Expected>> &&
         (std::is_move_constructible_v<T> == Expected);
}

template <bool Expected, class T>
constexpr auto check_is_nothrow_move_constructible() -> bool {
  return is_unambiguously_publicly_derived_from_v<
             std::is_nothrow_move_constructible<T>,
             std::bool_constant<Expected>> &&
         (std::is_nothrow_move_constructible_v<T> == Expected);
}

template <bool Expected, class T>
constexpr auto check_is_trivially_move_constructible() -> bool {
  return is_unambiguously_publicly_derived_from_v<
             std::is_trivially_move_constructible<T>,
             std::bool_constant<Expected>> &&
         (std::is_trivially_move_constructible_v<T> == Expected);
}

class some_user_type {};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class trivial_defaulted_move_construct {
 public:
  trivial_defaulted_move_construct(trivial_defaulted_move_construct&&) noexcept = default;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class trivial_move_construct_by_copy {
 public:
  trivial_move_construct_by_copy(trivial_move_construct_by_copy const&) noexcept = default;
};

class trivial_base_class_move : trivial_defaulted_move_construct {};

class trivial_member_move {
  trivial_defaulted_move_construct dummy_;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class non_trivial_move_construct {
 public:
  non_trivial_move_construct(non_trivial_move_construct&&) noexcept;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class non_trivial_move_construct_by_copy {
 public:
  non_trivial_move_construct_by_copy(non_trivial_move_construct_by_copy const&) noexcept;
};

class non_trivial_base_class_move : non_trivial_move_construct {};

class non_trivial_member_move {
  non_trivial_move_construct dummy_;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class non_trivial_destructor {
 public:
  ~non_trivial_destructor();
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class move_only {
 public:
  move_only(move_only const&) = delete;
  move_only(move_only&&) noexcept;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class not_move_constructible {
 public:
  not_move_constructible(not_move_constructible&&) = delete;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class throwing_move_constructible {
 public:
  throwing_move_constructible(throwing_move_constructible&&) noexcept(false);
};

/// @test A type is move constructible but not nothrow-move-constructible with throwing move
TEST(MoveConstructible, ThrowingMoveConstructibleType) {
  static_assert(check_is_move_constructible<true, throwing_move_constructible>(), "Should be move constructible");
  static_assert(
      check_is_nothrow_move_constructible<false, throwing_move_constructible>(),
      "Should not be nothrow move constructible"
  );
}

using move_constructible_types = arene::base::type_lists::concat_unique_t<
    ::testing::scalar_types,
    ::testing::union_types,
    ::testing::class_types,
    ::testing::
        Types<int const&, some_user_type const&, int* const&, int&, some_user_type&, int*&, some_user_type, move_only>>;

template <typename T>
class MoveConstructibleTest : public testing::Test {};

TYPED_TEST_SUITE(MoveConstructibleTest, move_constructible_types, );

/// @test A type is move constructible
TYPED_TEST(MoveConstructibleTest, IsMoveConstructible) {
  static_assert(check_is_move_constructible<true, TypeParam>(), "A type is move constructible");
  static_assert(check_is_nothrow_move_constructible<true, TypeParam>(), "A type is nothrow move constructible");
}

using not_move_constructible_types = arene::base::type_lists::
    concat_unique_t<testing::array_types, testing::non_referenceable_types, testing::Types<not_move_constructible>>;

template <typename T>
class NotMoveConstructibleTest : public testing::Test {};

TYPED_TEST_SUITE(NotMoveConstructibleTest, not_move_constructible_types, );

/// @test A type is not move constructible
TYPED_TEST(NotMoveConstructibleTest, IsNotMoveConstructible) {
  static_assert(check_is_move_constructible<false, TypeParam>(), "A type is not move constructible");
  static_assert(check_is_nothrow_move_constructible<false, TypeParam>(), "A type is not nothrow move constructible");
}

using non_trivial_move_constructible_types = testing::Types<
    non_trivial_move_construct,
    non_trivial_base_class_move,
    non_trivial_member_move,
    non_trivial_destructor,
    non_trivial_move_construct_by_copy>;

template <typename T>
class NonTrivialMoveConstructibleTest : public testing::Test {};

TYPED_TEST_SUITE(NonTrivialMoveConstructibleTest, non_trivial_move_constructible_types, );

/// @test A type is move constructible but not trivially move constructible
TYPED_TEST(NonTrivialMoveConstructibleTest, MoveConstructibleNotTriviallyMoveConstructible) {
  static_assert(check_is_move_constructible<true, TypeParam>(), "A type is move constructible");
  static_assert(
      check_is_trivially_move_constructible<false, TypeParam>(),
      "A type is not trivially move constructible"
  );
}

using trivial_move_constructible_types = arene::base::type_lists::concat_unique_t<
    testing::scalar_types,
    testing::union_types,
    testing::class_types,
    testing::Types<
        trivial_defaulted_move_construct,
        trivial_base_class_move,
        trivial_member_move,
        trivial_move_construct_by_copy>>;

template <typename T>
class TrivialMoveConstructibleTest : public testing::Test {};

TYPED_TEST_SUITE(TrivialMoveConstructibleTest, trivial_move_constructible_types, );

/// @test A type is trivially move constructible
TYPED_TEST(TrivialMoveConstructibleTest, MoveConstructibleNotTriviallyMoveConstructible) {
  static_assert(check_is_move_constructible<true, TypeParam>(), "A type is move constructible");
  static_assert(check_is_trivially_move_constructible<true, TypeParam>(), "A type is trivially move constructible");
}

}  // namespace
