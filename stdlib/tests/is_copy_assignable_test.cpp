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
constexpr auto check_is_copy_assignable() -> bool {
  return is_unambiguously_publicly_derived_from_v<std::is_copy_assignable<T>, std::bool_constant<Expected>> &&
         (std::is_copy_assignable_v<T> == Expected);
}

template <bool Expected, class T>
constexpr auto check_is_nothrow_copy_assignable() -> bool {
  return is_unambiguously_publicly_derived_from_v<std::is_nothrow_copy_assignable<T>, std::bool_constant<Expected>> &&
         (std::is_nothrow_copy_assignable_v<T> == Expected);
}

template <bool Expected, class T>
constexpr auto check_is_trivially_copy_assignable() -> bool {
  return is_unambiguously_publicly_derived_from_v<std::is_trivially_copy_assignable<T>, std::bool_constant<Expected>> &&
         (std::is_trivially_copy_assignable_v<T> == Expected);
}

class some_user_type {};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class trivial_defaulted_copy_assign {
 public:
  auto operator=(trivial_defaulted_copy_assign const&) noexcept -> trivial_defaulted_copy_assign& = default;
};

class trivial_base_class_copy : trivial_defaulted_copy_assign {};

struct trivial_member_copy {
  trivial_defaulted_copy_assign dummy;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class non_trivial_copy_assign {
 public:
  auto operator=(non_trivial_copy_assign const&) noexcept -> non_trivial_copy_assign&;
};

class non_trivial_base_class_copy : non_trivial_copy_assign {};

struct non_trivial_member_copy {
  non_trivial_copy_assign dummy;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class non_trivial_destructor {
 public:
  ~non_trivial_destructor();
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class non_const_copy {
 public:
  // NOLINTNEXTLINE(misc-unconventional-assign-operator)
  auto operator=(non_const_copy&) noexcept -> non_const_copy&;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class not_copy_assignable {
 public:
  auto operator=(not_copy_assignable const&) -> not_copy_assignable& = delete;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class throwing_copy_assignable {
 public:
  auto operator=(throwing_copy_assignable const&) noexcept(false) -> throwing_copy_assignable&;
};

/// @test A type is copy assignable but not nothrow-copy-assignable with throwing assign
TEST(CopyAssignable, ThrowingCopyAssignableType) {
  static_assert(check_is_copy_assignable<true, throwing_copy_assignable>(), "Should be copy assignable");
  static_assert(
      check_is_nothrow_copy_assignable<false, throwing_copy_assignable>(),
      "Should not be nothrow copy assignable"
  );
}

using copy_assignable_types = arene::base::type_lists::concat_unique_t<
    ::testing::scalar_types,
    ::testing::union_types,
    ::testing::class_types,
    ::testing::
        Types<int&, some_user_type&, int*&, some_user_type, trivial_defaulted_copy_assign, non_trivial_destructor>>;

template <typename T>
class CopyAssignableTest : public testing::Test {};

TYPED_TEST_SUITE(CopyAssignableTest, copy_assignable_types, );

/// @test A type is copy assignable
TYPED_TEST(CopyAssignableTest, IsCopyAssignable) {
  static_assert(check_is_copy_assignable<true, TypeParam>(), "A type is copy assignable");
  static_assert(check_is_nothrow_copy_assignable<true, TypeParam>(), "A type is nothrow copy assignable");
}

using not_copy_assignable_types = arene::base::type_lists::concat_unique_t<
    testing::array_types,
    testing::non_referenceable_types,
    testing::Types<not_copy_assignable, non_const_copy, int const&, some_user_type const&, int* const&>>;

template <typename T>
class NotCopyAssignableTest : public testing::Test {};

TYPED_TEST_SUITE(NotCopyAssignableTest, not_copy_assignable_types, );

/// @test A type is not copy assignable
TYPED_TEST(NotCopyAssignableTest, IsNotCopyAssignable) {
  static_assert(check_is_copy_assignable<false, TypeParam>(), "A type is not copy assignable");
  static_assert(check_is_nothrow_copy_assignable<false, TypeParam>(), "A type is not nothrow copy assignable");
  static_assert(check_is_trivially_copy_assignable<false, TypeParam>(), "A type is not trivially copy assignable");
}

using non_trivial_copy_assignable_types =
    testing::Types<non_trivial_copy_assign, non_trivial_base_class_copy, non_trivial_member_copy>;

template <typename T>
class NonTrivialCopyAssignableTest : public testing::Test {};

TYPED_TEST_SUITE(NonTrivialCopyAssignableTest, non_trivial_copy_assignable_types, );

/// @test A type is copy assignable but not trivially copy assignable
TYPED_TEST(NonTrivialCopyAssignableTest, CopyAssignableNotTriviallyCopyAssignable) {
  static_assert(check_is_copy_assignable<true, TypeParam>(), "A type is copy assignable");
  static_assert(check_is_trivially_copy_assignable<false, TypeParam>(), "A type is not trivially copy assignable");
}

using trivial_copy_assignable_types = arene::base::type_lists::concat_unique_t<
    testing::scalar_types,
    testing::union_types,
    testing::class_types,
    testing::
        Types<trivial_defaulted_copy_assign, trivial_base_class_copy, trivial_member_copy, non_trivial_destructor>>;

template <typename T>
class TrivialCopyAssignableTest : public testing::Test {};

TYPED_TEST_SUITE(TrivialCopyAssignableTest, trivial_copy_assignable_types, );

/// @test A type is trivially copy assignable
TYPED_TEST(TrivialCopyAssignableTest, CopyAssignableNotTriviallyCopyAssignable) {
  static_assert(check_is_copy_assignable<true, TypeParam>(), "A type is copy assignable");
  static_assert(check_is_trivially_copy_assignable<true, TypeParam>(), "A type is trivially copy assignable");
}

}  // namespace
