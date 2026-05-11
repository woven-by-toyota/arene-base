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
constexpr auto check_is_copy_constructible() -> bool {
  return is_unambiguously_publicly_derived_from_v<std::is_copy_constructible<T>, std::bool_constant<Expected>> &&
         (std::is_copy_constructible_v<T> == Expected);
}

template <bool Expected, class T>
constexpr auto check_is_nothrow_copy_constructible() -> bool {
  return is_unambiguously_publicly_derived_from_v<
             std::is_nothrow_copy_constructible<T>,
             std::bool_constant<Expected>> &&
         (std::is_nothrow_copy_constructible_v<T> == Expected);
}

template <bool Expected, class T>
constexpr auto check_is_trivially_copy_constructible() -> bool {
  return is_unambiguously_publicly_derived_from_v<
             std::is_trivially_copy_constructible<T>,
             std::bool_constant<Expected>> &&
         (std::is_trivially_copy_constructible_v<T> == Expected);
}

class some_user_type {};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class non_const_copy {
 public:
  non_const_copy(non_const_copy&) noexcept;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class not_copy_constructible {
 public:
  not_copy_constructible(not_copy_constructible const&) = delete;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class throwing_copy_constructible {
 public:
  throwing_copy_constructible(throwing_copy_constructible const&) noexcept(false);
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class trivial_defaulted_copy {
 public:
  trivial_defaulted_copy(trivial_defaulted_copy const&) = default;
};

class trivial_base_class_copy : trivial_defaulted_copy {};

class trivial_member_copy {
  trivial_defaulted_copy dummy_;
};

// NOLINTNEXTLINE(hicpp-special-member-functions)
class non_trivial_copy {
 public:
  non_trivial_copy(non_trivial_copy const&) noexcept;
};

class non_trivial_base_class_copy : non_trivial_copy {};

class non_trivial_member_copy {
  non_trivial_copy dummy_;
};

/// @test A type is copy constructible but not nothrow-copy-constructible with throwing copy
TEST(CopyConstructible, ThrowingCopyConstructibleType) {
  static_assert(check_is_copy_constructible<true, throwing_copy_constructible>(), "Should be copy constructible");
  static_assert(
      check_is_nothrow_copy_constructible<false, throwing_copy_constructible>(),
      "Should not be nothrow copy constructible"
  );
}

using copy_constructible_types = arene::base::type_lists::concat_unique_t<
    ::testing::scalar_types,
    ::testing::union_types,
    ::testing::class_types,
    ::testing::Types<int const&, some_user_type const&, int* const&, int&, some_user_type&, int*&, some_user_type>>;

template <typename T>
class CopyConstructibleTest : public testing::Test {};

TYPED_TEST_SUITE(CopyConstructibleTest, copy_constructible_types, );

/// @test A type is copy constructible
TYPED_TEST(CopyConstructibleTest, IsCopyConstructible) {
  static_assert(check_is_copy_constructible<true, TypeParam>(), "A type is copy constructible");
  static_assert(check_is_nothrow_copy_constructible<true, TypeParam>(), "A type is nothrow copy constructible");
}

using not_copy_constructible_types = arene::base::type_lists::concat_unique_t<
    testing::array_types,
    testing::non_referenceable_types,
    testing::Types<not_copy_constructible, non_const_copy>>;

template <typename T>
class NotCopyConstructibleTest : public testing::Test {};

TYPED_TEST_SUITE(NotCopyConstructibleTest, not_copy_constructible_types, );

/// @test A type is not copy constructible
TYPED_TEST(NotCopyConstructibleTest, IsNotCopyConstructible) {
  static_assert(check_is_copy_constructible<false, TypeParam>(), "A type is not copy constructible");
  static_assert(check_is_nothrow_copy_constructible<false, TypeParam>(), "A type is not nothrow copy constructible");
  static_assert(
      check_is_trivially_copy_constructible<false, TypeParam>(),
      "A type is not trivially copy constructible"
  );
}

using non_trivial_copy_constructible_types =
    testing::Types<non_trivial_copy, non_trivial_base_class_copy, non_trivial_member_copy>;

template <typename T>
class NonTrivialCopyConstructibleTest : public testing::Test {};

TYPED_TEST_SUITE(NonTrivialCopyConstructibleTest, non_trivial_copy_constructible_types, );

/// @test A type is copy constructible but not trivially copy constructible
TYPED_TEST(NonTrivialCopyConstructibleTest, CopyConstructibleNotTriviallyCopyConstructible) {
  static_assert(check_is_copy_constructible<true, TypeParam>(), "A type is copy constructible");
  static_assert(
      check_is_trivially_copy_constructible<false, TypeParam>(),
      "A type is not trivially copy constructible"
  );
}

using trivial_copy_constructible_types = arene::base::type_lists::concat_unique_t<
    testing::scalar_types,
    testing::union_types,
    testing::class_types,
    testing::Types<trivial_defaulted_copy, trivial_base_class_copy, trivial_member_copy>>;

template <typename T>
class TrivialCopyConstructibleTest : public testing::Test {};

TYPED_TEST_SUITE(TrivialCopyConstructibleTest, trivial_copy_constructible_types, );

/// @test A type is trivially copy constructible
TYPED_TEST(TrivialCopyConstructibleTest, CopyConstructibleNotTriviallyCopyConstructible) {
  static_assert(check_is_copy_constructible<true, TypeParam>(), "A type is copy constructible");
  static_assert(check_is_trivially_copy_constructible<true, TypeParam>(), "A type is trivially copy constructible");
}

}  // namespace
